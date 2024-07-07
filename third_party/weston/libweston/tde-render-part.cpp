/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "tde-render-part.h"

#include <sstream>
#include <unistd.h>
#include <vector>

#include <display_gfx.h>
#include <idisplay_gralloc.h>

extern "C" {
#include <securec.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <dlfcn.h>
#include <drm.h>
#include <drm/drm_fourcc.h>
#include <linux-dmabuf.h>
#include <linux-dmabuf-unstable-v1-server-protocol.h>
#include <linux-explicit-synchronization.h>
#include <sys/mman.h>

#include "libweston/weston-log.h"
#include "shared/helpers.h"
#include "pixman-renderer-protected.h"
#define virtual __keyword__virtual
#include <drm-internal.h>
#undef virtual
}

#include "libweston/trace.h"
DEFINE_LOG_LABEL("TDERenderer");

struct tde_image_t {
    int32_t width;
    int32_t height;
    uint32_t format;
    int n_planes; // 默认为1，引入plane是为了保证方案的通用性
    __u64 phyaddr; // 物理地址
    int fd[MAX_DMABUF_PLANES];
    uint32_t offset[MAX_DMABUF_PLANES];
    uint32_t stride[MAX_DMABUF_PLANES];
};

struct fill_rect_call {
    ISurface surface;
    IRect rect;
    uint32_t color;
    GfxOpt opt;
};

struct tde_output_state_t {
    int32_t draw_count;
    std::vector<struct fill_rect_call> calls;
};

struct tde_surface_state_t {
    struct tde_image_t image;
    struct linux_dmabuf_buffer *buffer;
    struct tde_renderer_t *renderer;
};

struct tde_renderer_t {
    GfxFuncs *gfx_funcs;
    ::OHOS::HDI::Display::V1_0::IDisplayGralloc *display_gralloc;
    void *module;
    int use_tde;
    int use_dmabuf;
};

struct drm_hisilicon_phy_addr {
    uint64_t phyaddr; // return the phy addr
    int fd; // dmabuf file descriptor
};

#define LIB_GFX_NAME "libdisplay_gfx.z.so"
#define LIB_GFX_FUNC_NAME_INIT "GfxInitialize"
#define LIB_GFX_FUNC_NAME_DEINIT "GfxUninitialize"
#define DRM_HISILICON_GEM_FD_TO_PHYADDR (0x1)
#define DRM_IOCTL_HISILICON_GEM_FD_TO_PHYADDR \
    (DRM_IOWR(DRM_COMMAND_BASE + DRM_HISILICON_GEM_FD_TO_PHYADDR, \
        struct drm_hisilicon_phy_addr))

static uint64_t dst_image_phyaddr(struct weston_output *wo)
{
    struct drm_output *output = to_drm_output(wo);
    struct drm_backend *backend = to_drm_backend(wo->compositor);

    int prime_fd;
    drmPrimeHandleToFD(backend->drm.fd,
        output->dumb[output->current_image]->handles[0],
        DRM_CLOEXEC, &prime_fd);

    struct drm_hisilicon_phy_addr args = { .fd = prime_fd };

    ioctl(backend->drm.fd, DRM_IOCTL_HISILICON_GEM_FD_TO_PHYADDR, &args);
    close(prime_fd);
    return args.phyaddr;
}

static void src_surface_init(ISurface *surface, struct tde_image_t buffer)
{
    surface->width = buffer.width;
    surface->height = buffer.height;
    surface->phyAddr = buffer.phyaddr;
    surface->stride = buffer.stride[0];
    surface->enColorFmt = PIXEL_FMT_RGBA_8888;
    surface->bAlphaExt1555 = true;
    surface->bAlphaMax255 = true;
    surface->alpha0 = 0XFF;
    surface->alpha1 = 0XFF;
}

static void dst_surface_init(ISurface *surface, pixman_image_t *target_image,
                             struct weston_output *output)
{
    surface->width = pixman_image_get_width(target_image);
    surface->height = pixman_image_get_height(target_image);
    surface->phyAddr = dst_image_phyaddr(output);
    surface->enColorFmt = PIXEL_FMT_BGRA_8888;
    surface->stride = pixman_image_get_stride(target_image);
    surface->bAlphaExt1555 = true;
    surface->bAlphaMax255 = true;
    surface->alpha0 = 0XFF;
    surface->alpha1 = 0XFF;
}

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

typedef void(*weston_view_compute_global_region_func)(struct weston_view *view,
    float x, float y, float *vx, float *vy);

static void
weston_view_compute_global_region(struct weston_view *view,
                                  pixman_region32_t *outr,
                                  pixman_region32_t *inr,
                                  weston_view_compute_global_region_func fn)
{
    float min_x = HUGE_VALF,  min_y = HUGE_VALF;
    float max_x = -HUGE_VALF, max_y = -HUGE_VALF;
    pixman_box32_t *inbox = pixman_region32_extents(inr);
    int32_t vs[4][2] = {
        { inbox->x1, inbox->y1 },
        { inbox->x1, inbox->y2 },
        { inbox->x2, inbox->y1 },
        { inbox->x2, inbox->y2 },
    };

    if (inbox->x1 == inbox->x2 || inbox->y1 == inbox->y2) {
        pixman_region32_init(outr);
        return;
    }

    for (int i = 0; i < 4; i++) {
        float x, y;
        fn(view, vs[i][0], vs[i][1], &x, &y);
        min_x = min(min_x, x);
        max_x = max(max_x, x);
        min_y = min(min_y, y);
        max_y = max(max_y, y);
    }

    float int_x = floorf(min_x);
    float int_y = floorf(min_y);
    pixman_region32_init_rect(outr, int_x, int_y,
            ceilf(max_x) - int_x, ceilf(max_y) - int_y);
}

#undef min
#undef max

static void
weston_view_to_global_region(struct weston_view *view,
                             pixman_region32_t *outr,
                             pixman_region32_t *inr)
{
    weston_view_compute_global_region(view, outr, inr, weston_view_to_global_float);
}

static void
weston_view_from_global_region(struct weston_view *view,
                               pixman_region32_t *outr,
                               pixman_region32_t *inr)
{
    weston_view_compute_global_region(view, outr, inr, weston_view_from_global_float);
}

static void dump_to_file(struct pixman_surface_state *ps)
{
    if (access("/data/tde_dump", F_OK) == -1) {
        return;
    }

    struct timeval now;
    gettimeofday(&now, nullptr);
    constexpr int secToUsec = 1000 * 1000;
    int64_t nowVal = (int64_t)now.tv_sec * secToUsec + (int64_t)now.tv_usec;

    std::stringstream ss;
    ss << "/data/tde-dumpimage-" << nowVal << ".raw";

    auto fp = fopen(ss.str().c_str(), "a+");
    if (fp == nullptr) {
        return;
    }

    void *viraddr = pixman_image_get_data(ps->image);
    int32_t size = pixman_image_get_stride(ps->image) * pixman_image_get_height(ps->image);
    fwrite(viraddr, size, 1, fp);
    fclose(fp);
}

static int tde_repaint_region(struct weston_view *ev,
                              struct weston_output *output,
                              pixman_region32_t *buffer_region,
                              pixman_region32_t *output_damage)
{
    struct pixman_renderer *renderer = (struct pixman_renderer *)output->compositor->renderer;
    struct pixman_surface_state *surface = get_surface_state(ev->surface);
    dump_to_file(surface);
    struct pixman_output_state *output_state = get_output_state(output);
    pixman_image_t *target_image = output_state->hw_buffer;
    if (output_state->shadow_image) {
        target_image = output_state->shadow_image;
    }

    GfxOpt opt = {
        .enPixelAlpha = true,
        .blendType = output_state->tde->draw_count ? BLEND_SRCOVER : BLEND_SRC,
        .enableScale = true,
    };

    // region calc
    pixman_region32_t global_repaint_region;
    pixman_region32_t buffer_repaint_region;
    {
        pixman_region32_t surface_region;
        pixman_region32_init_rect(&surface_region, 0, 0, ev->surface->width, ev->surface->height);

        pixman_region32_t repaint_output;
        pixman_region32_init(&repaint_output);
        pixman_region32_copy(&repaint_output, output_damage);
        if (output->zoom.active) {
            weston_matrix_transform_region(&repaint_output, &output->matrix, &repaint_output);
        } else {
            pixman_region32_translate(&repaint_output, -output->x, -output->y);
            weston_transformed_region(output->width, output->height,
                    static_cast<enum wl_output_transform>(output->transform),
                    output->current_scale,
                    &repaint_output, &repaint_output);
        }

        LOG_REGION(1, &surface_region);
        LOG_REGION(2, &repaint_output);

        struct weston_matrix matrix = output->inverse_matrix;
        if (ev->transform.enabled) {
            weston_matrix_multiply(&matrix, &ev->transform.inverse);
            LOG_INFO("transform enabled");
        } else {
            weston_matrix_translate(&matrix,
                    -ev->geometry.x, -ev->geometry.y, 0);
            LOG_INFO("transform disabled");
        }
        weston_matrix_multiply(&matrix, &ev->surface->surface_to_buffer_matrix);

        if (matrix.d[0] == matrix.d[5] && matrix.d[0] == 0) {
            if (matrix.d[4] > 0 && matrix.d[1] > 0) {
                LOG_INFO("90 mirror");
                opt.rotateType = ROTATE_90;
                opt.mirrorType = MIRROR_LR;
            } else if (matrix.d[4] < 0 && matrix.d[1] > 0) {
                LOG_INFO("90");
                opt.rotateType = ROTATE_90;
            } else if (matrix.d[4] < 0 && matrix.d[1] < 0) {
                LOG_INFO("270 mirror");
                opt.rotateType = ROTATE_270;
                opt.mirrorType = MIRROR_LR;
            } else if (matrix.d[4] > 0 && matrix.d[1] < 0) {
                LOG_INFO("270");
                opt.rotateType = ROTATE_270;
            }
        } else {
            if (matrix.d[0] > 0 && matrix.d[5] > 0) {
                LOG_INFO("0");
                opt.rotateType = ROTATE_NONE;
            } else if (matrix.d[0] < 0 && matrix.d[5] < 0) {
                LOG_INFO("180");
                opt.rotateType = ROTATE_180;
            } else if (matrix.d[0] < 0 && matrix.d[5] > 0) {
                LOG_INFO("0 mirror");
                opt.rotateType = ROTATE_NONE;
                opt.mirrorType = MIRROR_LR;
            } else if (matrix.d[0] > 0 && matrix.d[5] < 0) {
                LOG_INFO("180 mirror");
                opt.rotateType = ROTATE_180;
                opt.mirrorType = MIRROR_LR;
            }
        }
        LOG_INFO("%f %f %f %f", matrix.d[0], matrix.d[1], matrix.d[2], matrix.d[3]);
        LOG_INFO("%f %f %f %f", matrix.d[4], matrix.d[5], matrix.d[6], matrix.d[7]);
        LOG_INFO("%f %f %f %f", matrix.d[8], matrix.d[9], matrix.d[10], matrix.d[11]);
        LOG_INFO("%f %f %f %f", matrix.d[12], matrix.d[13], matrix.d[14], matrix.d[15]);
        LOG_INFO("%d %d", ev->surface->width, ev->surface->height);

        weston_view_to_global_region(ev, &global_repaint_region, &surface_region);
        pixman_region32_intersect(&global_repaint_region, &global_repaint_region, &repaint_output);
        LOG_REGION(3, &global_repaint_region);

        pixman_region32_t surface_repaint_region;
        pixman_region32_init(&surface_repaint_region);
        weston_view_from_global_region(ev, &surface_repaint_region, &global_repaint_region);
        LOG_REGION(4, &surface_repaint_region);

        pixman_region32_init(&buffer_repaint_region);
        weston_surface_to_buffer_region(ev->surface, &surface_repaint_region, &buffer_repaint_region);
        LOG_REGION(5, &buffer_repaint_region);
        pixman_region32_fini(&surface_repaint_region);
        pixman_region32_fini(&surface_region);
        pixman_region32_fini(&repaint_output);
    }
    pixman_box32_t *global_box = pixman_region32_extents(&global_repaint_region);
    IRect dstRect = {
        .x = global_box->x1, .y = global_box->y1,
        .w = global_box->x2 - global_box->x1,
        .h = global_box->y2 - global_box->y1
    };

    pixman_box32_t *buffer_box = pixman_region32_extents(&buffer_repaint_region);
    IRect srcRect = {
        .x = buffer_box->x1, .y = buffer_box->y1,
        .w = buffer_box->x2 - buffer_box->x1,
        .h = buffer_box->y2 - buffer_box->y1
    };
    pixman_region32_fini(&global_repaint_region);
    pixman_region32_fini(&buffer_repaint_region);

    // tde
    ISurface dstSurface = {};
    dst_surface_init(&dstSurface, target_image, output);
    ISurface srcSurface = {};
    src_surface_init(&srcSurface, surface->tde->image);

    if (ev->surface->type == WL_SURFACE_TYPE_VIDEO) {
        opt.blendType = BLEND_SRC;
        struct fill_rect_call call = {
            .surface = dstSurface,
            .rect = dstRect,
            .color = 0x00000000,
            .opt = opt,
        };
        output_state->tde->calls.push_back(call);
        return 0;
    } else {
        if (renderer->tde->gfx_funcs->InitGfx() != 0) {
            weston_log("tde_repaint_region InitGfx failed");
            return -1;
        }
        output_state->tde->draw_count++;
        renderer->tde->gfx_funcs->Blit(&srcSurface, &srcRect, &dstSurface, &dstRect, &opt);
        weston_log("Blit dst(%{public}d, %{public}d) %{public}dx%{public}d",
                   dstRect.x, dstRect.y, dstRect.w, dstRect.h);
        weston_log("Blit src(%{public}d, %{public}d) %{public}dx%{public}d",
                   srcRect.x, srcRect.y, srcRect.w, srcRect.h);
        renderer->tde->gfx_funcs->DeinitGfx();
    }
    return 0;
}

void tde_repaint_finish_hook(struct weston_output *output)
{
    struct pixman_renderer *renderer = (struct pixman_renderer *)output->compositor->renderer;
    struct pixman_output_state *output_state = get_output_state(output);
    for (auto &call : output_state->tde->calls) {
        if (renderer->tde->gfx_funcs->InitGfx() != 0) {
            weston_log("tde_repaint_finish_hook InitGfx failed");
            continue;
        }
        renderer->tde->gfx_funcs->FillRect(&call.surface, &call.rect, call.color, &call.opt);
        weston_log("FillRect (%{public}d, %{public}d) %{public}dx%{public}d",
                   call.rect.x, call.rect.y, call.rect.w, call.rect.h);
        renderer->tde->gfx_funcs->DeinitGfx();
    }
}

static bool import_dmabuf(struct weston_compositor *ec,
                          struct linux_dmabuf_buffer *dmabuf)
{
    return true;
}

static void query_dmabuf_formats(struct weston_compositor *wc,
                                 int **formats, int *num_formats)
{
    static const int fallback_formats[] = {
        DRM_FORMAT_ARGB8888,
        DRM_FORMAT_XRGB8888,
        DRM_FORMAT_YUYV,
        DRM_FORMAT_NV12,
        DRM_FORMAT_YUV420,
        DRM_FORMAT_YUV444,
    };
    int num = ARRAY_LENGTH(fallback_formats);
    if (num > 0) {
        *formats = (int *)calloc(num, sizeof(int));
    }

    memcpy_s(*formats, num * sizeof(int), fallback_formats, sizeof(fallback_formats));
    *num_formats = num;
}

static void query_dmabuf_modifiers(struct weston_compositor *wc, int format,
                                   uint64_t **modifiers, int *num_modifiers)
{
    *modifiers = NULL;
    *num_modifiers = 0;
}

static int32_t tde_render_gfx_init(struct tde_renderer_t *tde)
{
    tde->module = dlopen(LIB_GFX_NAME, RTLD_NOW | RTLD_NOLOAD);
	if (tde->module) {
		weston_log("Module '%{public}s' already loaded\n", LIB_GFX_NAME);
	} else {
		weston_log("Loading module '%{public}s'\n", LIB_GFX_NAME);
		tde->module = dlopen(LIB_GFX_NAME, RTLD_NOW);
		if (!tde->module) {
			weston_log("Failed to load module: %{public}s\n", dlerror());
			return DISPLAY_FAILURE;
		}
	}

    auto func = (int32_t(*)(GfxFuncs **funcs))dlsym(tde->module, LIB_GFX_FUNC_NAME_INIT);
    if (!func) {
		weston_log("Failed to lookup %{public}s function: %s\n", LIB_GFX_FUNC_NAME_INIT, dlerror());
		dlclose(tde->module);
		return DISPLAY_FAILURE;
	}

    return func(&tde->gfx_funcs);
}

static int32_t tde_render_gfx_deinit(struct tde_renderer_t *tde)
{
    int32_t ret = DISPLAY_FAILURE;
    if (tde->module) {
        auto func = (int32_t(*)(GfxFuncs *funcs))dlsym(tde->module, LIB_GFX_FUNC_NAME_DEINIT);
        if (!func) {
            weston_log("Failed to lookup %{public}s function: %s\n", LIB_GFX_FUNC_NAME_DEINIT, dlerror());
        } else {
            ret = func(tde->gfx_funcs);
        }
        dlclose(tde->module);
    }
    return ret;
}

int tde_renderer_alloc_hook(struct pixman_renderer *renderer, struct weston_compositor *ec)
{
    renderer->tde = (struct tde_renderer_t *)zalloc(sizeof(*renderer->tde));
    if (renderer->tde == NULL) {
        return -1;
    }

    // Gfx Init
    int ret = tde_render_gfx_init(renderer->tde);
    if (ret == DISPLAY_SUCCESS && renderer->tde->gfx_funcs != NULL) {
        renderer->tde->use_tde = 1;
        weston_log("use tde");
    } else {
        renderer->tde->use_tde = 0;
        weston_log("use no tde");
    }

    // Gralloc Init
    renderer->tde->display_gralloc = ::OHOS::HDI::Display::V1_0::IDisplayGralloc::Get();
    if (renderer->tde->display_gralloc != NULL) {
        renderer->tde->use_dmabuf = 1;
        weston_log("use dmabuf");
    } else {
        renderer->tde->use_tde = 0;
        weston_log("use no tde");
        renderer->tde->use_dmabuf = 0;
        weston_log("use no dmabuf");
    }

    renderer->base.import_dmabuf = import_dmabuf;
    renderer->base.query_dmabuf_formats = query_dmabuf_formats;
    renderer->base.query_dmabuf_modifiers = query_dmabuf_modifiers;
    return 0;
}

int tde_renderer_free_hook(struct pixman_renderer *renderer)
{
    tde_render_gfx_deinit(renderer->tde);
    delete renderer->tde->display_gralloc;
    free(renderer->tde);
    return 0;
}

int tde_output_state_alloc_hook(struct pixman_output_state *state)
{
    state->tde = (struct tde_output_state_t *)zalloc(sizeof(*state->tde));
    return 0;
}

int tde_output_state_init_hook(struct pixman_output_state *state)
{
    state->tde->draw_count = 0;
    state->tde->calls.clear();
    return 0;
}

int tde_output_state_free_hook(struct pixman_output_state *state)
{
    free(state->tde);
    return 0;
}

int tde_surface_state_alloc_hook(struct pixman_surface_state *state)
{
    state->tde = (struct tde_surface_state_t *)zalloc(sizeof(*state->tde));
    return 0;
}

int tde_surface_state_free_hook(struct pixman_surface_state *state)
{
    free(state->tde);
    return 0;
}

static void buffer_state_handle_buffer_destroy(struct wl_listener *listener,
                                               void *data)
{
    struct pixman_surface_state *ps = container_of(listener,
        struct pixman_surface_state, buffer_destroy_listener);
    if (ps->image) {
        tde_unref_image_hook(ps);

        pixman_image_unref(ps->image);
        ps->image = NULL;
    }

    ps->buffer_destroy_listener.notify = NULL;
}

int tde_render_attach_hook(struct weston_surface *es, struct weston_buffer *buffer)
{
    if (buffer == NULL) {
        return -1;
    }

    struct linux_dmabuf_buffer *dmabuf = linux_dmabuf_buffer_get(buffer->resource);
    if (dmabuf == NULL) {
        return -1;
    }

    struct pixman_surface_state *ps = get_surface_state(es);
    ps->tde->renderer = get_renderer(es->compositor)->tde;
    if (ps->tde->renderer->use_dmabuf != 1) {
        return -1;
    }

    void *ptr = ps->tde->renderer->display_gralloc->Mmap(*dmabuf->attributes.buffer_handle);
    if (ptr == NULL) {
        return -1;
    }

    weston_buffer_reference(&ps->buffer_ref, buffer);
    weston_buffer_release_reference(&ps->buffer_release_ref,
            es->buffer_release_ref.buffer_release);

    if (ps->buffer_destroy_listener.notify) {
        wl_list_remove(&ps->buffer_destroy_listener.link);
        ps->buffer_destroy_listener.notify = NULL;
    }

    if (ps->image) {
        tde_unref_image_hook(ps);

        pixman_image_unref(ps->image);
        ps->image = NULL;
    }

    pixman_format_code_t pixman_format = PIXMAN_a8b8g8r8;
    buffer->legacy_buffer = NULL;
    buffer->width = dmabuf->attributes.width;
    buffer->height = dmabuf->attributes.height;

    ps->tde->image.width = dmabuf->attributes.width;
    ps->tde->image.height = dmabuf->attributes.height;
    ps->tde->image.stride[0] = dmabuf->attributes.stride[0];
    ps->tde->image.format = dmabuf->attributes.format;
    ps->tde->image.fd[0] = dmabuf->attributes.fd[0];
    ps->tde->buffer = dmabuf;

    ps->tde->image.phyaddr = dmabuf->attributes.buffer_handle->phyAddr;
    if (ps->tde->image.phyaddr == 0) {
        weston_log("tde-renderer: phyAddr is invalid.\n");
    }

    ps->image = pixman_image_create_bits(pixman_format,
        buffer->width, buffer->height,
        (uint32_t *)dmabuf->attributes.buffer_handle->virAddr,
        dmabuf->attributes.stride[0]);

    ps->buffer_destroy_listener.notify = buffer_state_handle_buffer_destroy;
    wl_signal_add(&buffer->destroy_signal, &ps->buffer_destroy_listener);
    return 0;
}

int tde_repaint_region_hook(struct weston_view *ev, struct weston_output *output,
        pixman_region32_t *buffer_region, pixman_region32_t *repaint_output)
{
    struct pixman_renderer *renderer = (struct pixman_renderer *)output->compositor->renderer;
    if (!renderer->tde->use_tde) {
        return -1;
    }

    return tde_repaint_region(ev, output, buffer_region, repaint_output);
}

int tde_unref_image_hook(struct pixman_surface_state *ps)
{
    if (ps == NULL) {
        return 0;
    }

    if (ps->tde == NULL) {
        return 0;
    }

    struct tde_surface_state_t *tss = ps->tde;
    if (tss->renderer == NULL || tss->buffer == NULL) {
        return 0;
    }

    BufferHandle *bh = tss->buffer->attributes.buffer_handle;
    if (bh == NULL || bh->virAddr == NULL) {
        return 0;
    }

    if (tss->renderer->use_dmabuf == 1) {
        tss->renderer->display_gralloc->Unmap(*bh);
    }
    return 0;
}
