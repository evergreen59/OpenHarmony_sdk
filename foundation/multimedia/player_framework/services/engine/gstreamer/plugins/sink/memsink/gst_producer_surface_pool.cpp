/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gst_producer_surface_pool.h"
#include <unordered_map>
#include <sys/time.h>
#include "media_log.h"
#include "display_type.h"
#include "surface_buffer.h"
#include "buffer_type_meta.h"
#include "gst_surface_allocator.h"
#include "gst/video/gstvideometa.h"
#include "media_dfx.h"
#include "securec.h"

namespace {
    const std::unordered_map<GstVideoFormat, PixelFormat> FORMAT_MAPPING = {
        { GST_VIDEO_FORMAT_RGBA, PIXEL_FMT_RGBA_8888 },
        { GST_VIDEO_FORMAT_NV21, PIXEL_FMT_YCRCB_420_SP },
        { GST_VIDEO_FORMAT_NV12, PIXEL_FMT_YCBCR_420_SP },
        { GST_VIDEO_FORMAT_I420, PIXEL_FMT_YCBCR_420_P },
    };
    constexpr int32_t TIME_VAL_US = 1000000;
    constexpr guint32 DEFAULT_PROP_DYNAMIC_BUFFER_NUM = 10;
}

enum {
    PROP_0,
    PROP_DYNAMIC_BUFFER_NUM,
    PROP_CACHE_BUFFERS_NUM,
    PROP_VIDEO_SCALE_TYPE,
};

#define GST_BUFFER_POOL_LOCK(pool)   (g_mutex_lock(&(pool)->lock))
#define GST_BUFFER_POOL_UNLOCK(pool) (g_mutex_unlock(&(pool)->lock))
#define GST_BUFFER_POOL_WAIT(pool) (g_cond_wait(&(pool)->cond, &(pool)->lock))
#define GST_BUFFER_POOL_NOTIFY(pool) (g_cond_signal(&(pool)->cond))

#define gst_producer_surface_pool_parent_class parent_class
G_DEFINE_TYPE(GstProducerSurfacePool, gst_producer_surface_pool, GST_TYPE_BUFFER_POOL);

GST_DEBUG_CATEGORY_STATIC(gst_producer_surface_pool_debug_category);
#define GST_CAT_DEFAULT gst_producer_surface_pool_debug_category

static void gst_producer_surface_pool_finalize(GObject *obj);
static const gchar **gst_producer_surface_pool_get_options (GstBufferPool *pool);
static gboolean gst_producer_surface_pool_set_config(GstBufferPool *pool, GstStructure *config);
static gboolean gst_producer_surface_pool_start(GstBufferPool *pool);
static gboolean gst_producer_surface_pool_stop(GstBufferPool *pool);
static GstFlowReturn gst_producer_surface_pool_alloc_buffer(GstBufferPool *pool,
    GstBuffer **buffer, GstBufferPoolAcquireParams *params);
static void gst_producer_surface_pool_free_buffer(GstBufferPool *pool, GstBuffer *buffer);
static GstFlowReturn gst_producer_surface_pool_acquire_buffer(GstBufferPool *pool,
    GstBuffer **buffer, GstBufferPoolAcquireParams *params);
static void gst_producer_surface_pool_release_buffer(GstBufferPool *pool, GstBuffer *buffer);
static void gst_producer_surface_pool_flush_start(GstBufferPool *pool);
static void gst_producer_surface_pool_set_property(GObject *object, guint prop_id,
    const GValue *value, GParamSpec *pspec);
static void gst_producer_surface_pool_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);

static void clear_preallocated_buffer(GstProducerSurfacePool *spool)
{
    g_return_if_fail(spool != nullptr);
    for (GList *node = g_list_first(spool->preAllocated); node != nullptr; node = g_list_next(node)) {
        GstBuffer *buffer = GST_BUFFER_CAST(node->data);
        if (buffer == nullptr) {
            continue;
        }
        gst_producer_surface_pool_free_buffer(GST_BUFFER_POOL_CAST(spool), buffer);
    }
    g_list_free(spool->preAllocated);
    spool->preAllocated = nullptr;
}

static void gst_producer_surface_pool_class_init(GstProducerSurfacePoolClass *klass)
{
    g_return_if_fail(klass != nullptr);
    GstBufferPoolClass *poolClass = GST_BUFFER_POOL_CLASS (klass);
    GObjectClass *gobjectClass = G_OBJECT_CLASS(klass);
    gobjectClass->set_property = gst_producer_surface_pool_set_property;
    gobjectClass->get_property = gst_producer_surface_pool_get_property;
    gobjectClass->finalize = gst_producer_surface_pool_finalize;

    g_object_class_install_property(gobjectClass, PROP_DYNAMIC_BUFFER_NUM,
        g_param_spec_uint("dynamic-buffer-num", "Dynamic Buffer Num",
            "Dynamic set buffer num when pool is active",
            0, G_MAXUINT, DEFAULT_PROP_DYNAMIC_BUFFER_NUM,
            (GParamFlags)(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobjectClass, PROP_CACHE_BUFFERS_NUM,
        g_param_spec_uint("cache-buffers-num", "Cached Buffer Num",
            "Set cached buffer nums for pool thread loop",
            0, G_MAXUINT, 0, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    g_object_class_install_property(gobjectClass, PROP_VIDEO_SCALE_TYPE,
        g_param_spec_uint("video-scale-type", "Video Scale Type",
            "Set video scale type for graphic",
            0, G_MAXUINT, 0, (GParamFlags)(G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    poolClass->get_options = gst_producer_surface_pool_get_options;
    poolClass->set_config = gst_producer_surface_pool_set_config;
    poolClass->start = gst_producer_surface_pool_start;
    poolClass->stop = gst_producer_surface_pool_stop;
    poolClass->acquire_buffer = gst_producer_surface_pool_acquire_buffer;
    poolClass->alloc_buffer = gst_producer_surface_pool_alloc_buffer;
    poolClass->release_buffer = gst_producer_surface_pool_release_buffer;
    poolClass->free_buffer = gst_producer_surface_pool_free_buffer;
    poolClass->flush_start = gst_producer_surface_pool_flush_start;
}

static void gst_producer_surface_pool_init(GstProducerSurfacePool *pool)
{
    g_return_if_fail(pool != nullptr);

    GST_DEBUG_CATEGORY_INIT(gst_producer_surface_pool_debug_category, "surfacepool", 0, "gst surface pool for sink");

    pool->surface = nullptr;
    pool->started = FALSE;
    g_mutex_init(&pool->lock);
    g_cond_init(&pool->cond);
    pool->allocator = nullptr;
    pool->preAllocated = nullptr;
    pool->minBuffers = 0;
    pool->maxBuffers = 0;
    pool->format = PixelFormat::PIXEL_FMT_BUTT;
    pool->usage = 0;
    gst_video_info_init(&pool->info);
    gst_allocation_params_init(&pool->params);
    pool->task = nullptr;
    g_rec_mutex_init(&pool->taskLock);
    (void)memset_s(&pool->beginTime, sizeof(timeval), 0, sizeof(timeval));
    (void)memset_s(&pool->endTime, sizeof(timeval), 0, sizeof(timeval));
    pool->callCnt = 0;
    pool->isDynamicCached = FALSE;
    pool->cachedBuffers = 0;
    pool->scale_type = 1; // VIDEO_SCALE_TYPE_FIT_CROP
}

static void gst_producer_surface_pool_finalize(GObject *obj)
{
    g_return_if_fail(obj != nullptr);
    GstProducerSurfacePool *spool = GST_PRODUCER_SURFACE_POOL_CAST(obj);
    GstBufferPool *pool = GST_BUFFER_POOL(obj);
    g_return_if_fail(spool != nullptr);

    clear_preallocated_buffer(spool);
    (void)gst_buffer_pool_set_active(pool, FALSE);

    spool->surface = nullptr;
    gst_object_unref(spool->allocator);
    spool->allocator = nullptr;
    g_mutex_clear(&spool->lock);
    g_cond_clear(&spool->cond);
    g_rec_mutex_clear(&spool->taskLock);
    if (spool->task != nullptr) {
        gst_object_unref(spool->task);
        spool->task = nullptr;
    }

    GST_INFO("pool finalize in");
    G_OBJECT_CLASS(parent_class)->finalize(obj);
    GST_INFO("pool finalize out");
}

static void gst_producer_surface_pool_set_property(GObject *object, guint prop_id,
    const GValue *value, GParamSpec *pspec)
{
    g_return_if_fail(object != nullptr);
    g_return_if_fail(value != nullptr);
    g_return_if_fail(pspec != nullptr);
    GstProducerSurfacePool *spool = GST_PRODUCER_SURFACE_POOL(object);

    switch (prop_id) {
        case PROP_DYNAMIC_BUFFER_NUM: {
            g_return_if_fail(spool->surface != nullptr);
            guint dynamicBuffers = g_value_get_uint(value);
            g_return_if_fail(dynamicBuffers != 0);
            GST_BUFFER_POOL_LOCK(spool);
            if (spool->freeBufCnt + dynamicBuffers < spool->maxBuffers) {
                GST_BUFFER_POOL_UNLOCK(spool);
                GST_ERROR_OBJECT(spool, "set queue size failed for free buffers %u", spool->freeBufCnt);
                return;
            }
            spool->freeBufCnt += (dynamicBuffers - spool->maxBuffers);
            spool->maxBuffers = dynamicBuffers;
            OHOS::SurfaceError err = spool->surface->SetQueueSize(spool->maxBuffers);
            if (err != OHOS::SurfaceError::SURFACE_ERROR_OK) {
                GST_BUFFER_POOL_UNLOCK(spool);
                GST_ERROR_OBJECT(spool, "set queue size to %u failed", spool->maxBuffers);
                return;
            }
            GST_DEBUG_OBJECT(spool, "set max buffer count: %u", spool->maxBuffers);
            GST_BUFFER_POOL_UNLOCK(spool);
            break;
        }
        case PROP_CACHE_BUFFERS_NUM: {
            GST_BUFFER_POOL_LOCK(spool);
            guint cache_num = g_value_get_uint(value);
            GST_DEBUG_OBJECT(spool, "set cache_num to: %u", cache_num);
            if (cache_num == 0) {
                spool->isDynamicCached = FALSE;
            } else {
                spool->isDynamicCached = TRUE;
                spool->cachedBuffers = cache_num;
            }
            GST_BUFFER_POOL_UNLOCK(spool);
            break;
        }
        case PROP_VIDEO_SCALE_TYPE: {
            GST_BUFFER_POOL_LOCK(spool);
            spool->scale_type = g_value_get_uint(value);
            GST_BUFFER_POOL_UNLOCK(spool);
            break;
        }
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

static void gst_producer_surface_pool_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    g_return_if_fail(object != nullptr);
    g_return_if_fail(value != nullptr);
    g_return_if_fail(pspec != nullptr);
    GstProducerSurfacePool *spool = GST_PRODUCER_SURFACE_POOL(object);

    switch (prop_id) {
        case PROP_DYNAMIC_BUFFER_NUM: {
            GST_BUFFER_POOL_LOCK(spool);
            g_value_set_uint(value, spool->maxBuffers);
            GST_BUFFER_POOL_UNLOCK(spool);
            break;
        }
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

GstProducerSurfacePool *gst_producer_surface_pool_new(void)
{
    GstProducerSurfacePool *pool = GST_PRODUCER_SURFACE_POOL_CAST(g_object_new(
        GST_TYPE_PRODUCER_SURFACE_POOL, "name", "SurfacePool", nullptr));
    (void)gst_object_ref_sink(pool);

    return pool;
}

static const gchar **gst_producer_surface_pool_get_options(GstBufferPool *pool)
{
    // add buffer type meta option at here
    (void)pool;
    static const gchar *options[] = { GST_BUFFER_POOL_OPTION_VIDEO_META, GST_BUFFER_POOL_OPTION_VIDEO_ALIGNMENT,
        nullptr };
    return options;
}

static gboolean parse_caps_info(GstCaps *caps, GstVideoInfo *info, PixelFormat *format)
{
    if (caps == nullptr) {
        GST_INFO("caps is nullptr");
        return FALSE;
    }

    if (!gst_video_info_from_caps(info, caps)) {
        GST_ERROR("wrong caps");
        return FALSE;
    }

    if (FORMAT_MAPPING.count(GST_VIDEO_INFO_FORMAT(info)) == 0) {
        GST_ERROR("unsupported format, %s", GST_VIDEO_INFO_NAME(info));
        return FALSE;
    }

    *format = FORMAT_MAPPING.at(GST_VIDEO_INFO_FORMAT(info));
    return TRUE;
}

static gboolean parse_config_usage(GstProducerSurfacePool *spool, GstStructure *config)
{
    g_return_val_if_fail(spool != nullptr, FALSE);
    g_return_val_if_fail(config != nullptr, FALSE);
    if (!gst_structure_get_uint64(config, "usage", &spool->usage)) {
        GST_INFO_OBJECT(spool, "no usage available");
        spool->usage = 0;
    }
    return TRUE;
}

static gboolean gst_producer_surface_pool_set_config(GstBufferPool *pool, GstStructure *config)
{
    GstProducerSurfacePool *spool = GST_PRODUCER_SURFACE_POOL_CAST(pool);
    g_return_val_if_fail(spool != nullptr, FALSE);
    g_return_val_if_fail(config != nullptr, FALSE);

    GstCaps *caps = nullptr;
    guint size; // ignore the size
    guint min_buffers;
    guint max_buffers;
    if (!gst_buffer_pool_config_get_params(config, &caps, &size, &min_buffers, &max_buffers)) {
        GST_ERROR_OBJECT(spool, "wrong config");
        return FALSE;
    }
    g_return_val_if_fail(min_buffers <= max_buffers, FALSE);

    GstVideoInfo info;
    PixelFormat format;
    if (!parse_caps_info(caps, &info, &format)) {
        return FALSE;
    }
    g_return_val_if_fail(parse_config_usage(spool, config), FALSE);
    GST_INFO_OBJECT(pool, "get usage %" G_GUINT64_FORMAT " ", spool->usage);

    GST_BUFFER_POOL_LOCK(spool);

    spool->info = info;
    spool->format = format;
    spool->minBuffers = min_buffers;
    spool->maxBuffers = max_buffers;

    GST_INFO_OBJECT(spool, "set config, width: %d, height: %d, format: %d, min_bufs: %u, max_bufs: %u",
        GST_VIDEO_INFO_WIDTH(&info), GST_VIDEO_INFO_HEIGHT(&info), format, min_buffers, max_buffers);

    GstAllocator *allocator = nullptr;
    GstAllocationParams params;
    (void)gst_buffer_pool_config_get_allocator(config, &allocator, &params);
    if (!(allocator && GST_IS_SURFACE_ALLOCATOR(allocator))) {
        GST_BUFFER_POOL_UNLOCK(spool);
        GST_WARNING_OBJECT(pool, "no valid allocator in pool");
        return TRUE;
    }
    spool->allocator = GST_SURFACE_ALLOCATOR_CAST(allocator);
    spool->params = params;

    GST_INFO_OBJECT(spool, "update allocator and params");

    GST_BUFFER_POOL_UNLOCK(spool);
    return TRUE;
}

gboolean gst_producer_surface_pool_set_surface(GstProducerSurfacePool *pool, OHOS::sptr<OHOS::Surface> surface)
{
    g_return_val_if_fail(surface != nullptr, FALSE);
    g_return_val_if_fail(pool != nullptr, FALSE);

    GST_BUFFER_POOL_LOCK(pool);

    if (pool->started) {
        GST_INFO_OBJECT(pool, "started already, reject to set surface");
        GST_BUFFER_POOL_UNLOCK(pool);
        return FALSE;
    }

    if (pool->surface != nullptr) {
        GST_INFO_OBJECT(pool, "surface has already been set");
        GST_BUFFER_POOL_UNLOCK(pool);
        return FALSE;
    }

    pool->surface = surface;

    GST_BUFFER_POOL_UNLOCK(pool);
    return TRUE;
}

static void gst_producer_surface_pool_statistics(GstProducerSurfacePool *spool)
{
    if (spool->callCnt == 0) {
        gettimeofday(&(spool->beginTime), nullptr);
    }
    spool->callCnt++;
    gettimeofday(&(spool->endTime), nullptr);
    if ((spool->endTime.tv_sec * TIME_VAL_US + spool->endTime.tv_usec) -
        (spool->beginTime.tv_sec * TIME_VAL_US + spool->beginTime.tv_usec) > TIME_VAL_US) {
        OHOS::Media::MediaEvent event;
        if (event.CreateMsg("The gst_producer_surface_pool_request_loop function is called in a second: %d",
            spool->callCnt)) {
            event.EventWrite("PLAYER_STATISTICS", OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "PLAYER");
            spool->callCnt = 0;
        } else {
            GST_ERROR_OBJECT(spool, "Failed to call CreateMsg");
        }
    }
}

static void gst_producer_surface_pool_request_loop(GstProducerSurfacePool *spool)
{
    g_return_if_fail(spool != nullptr);
    GstBufferPool *pool = GST_BUFFER_POOL_CAST(spool);
    GST_DEBUG_OBJECT(spool, "Loop In");

    GST_BUFFER_POOL_LOCK(spool);
    gst_producer_surface_pool_statistics(spool);

    while (spool->isDynamicCached && g_list_length(spool->preAllocated) >= spool->cachedBuffers && spool->started) {
        GST_BUFFER_POOL_WAIT(spool);
    }

    if (!spool->started) {
        GST_BUFFER_POOL_UNLOCK(spool);
        GST_WARNING_OBJECT(spool, "task is paused, exit");
        gst_task_pause(spool->task);
        return;
    }
    GST_BUFFER_POOL_UNLOCK(spool);

    GstBuffer *buffer = nullptr;
    GstFlowReturn ret = gst_producer_surface_pool_alloc_buffer(pool, &buffer, nullptr);
    if (ret != GST_FLOW_OK) {
        GST_WARNING_OBJECT(spool, "alloc buffer failed, exit");
        gst_task_pause(spool->task);
        GST_BUFFER_POOL_LOCK(spool);
        spool->started = FALSE;
        GST_BUFFER_POOL_NOTIFY(spool);
        GST_BUFFER_POOL_UNLOCK(spool);
        return;
    }

    GST_BUFFER_POOL_LOCK(spool);
    if (!spool->started) {
        gst_producer_surface_pool_free_buffer(pool, buffer);
    } else {
        spool->preAllocated = g_list_append(spool->preAllocated, buffer);
        GST_BUFFER_POOL_NOTIFY(spool);
    }
    GST_BUFFER_POOL_UNLOCK(spool);
}

static gboolean gst_producer_surface_pool_start(GstBufferPool *pool)
{
    g_return_val_if_fail(pool != nullptr, FALSE);
    GstProducerSurfacePool *spool = GST_PRODUCER_SURFACE_POOL_CAST(pool);
    g_return_val_if_fail(spool != nullptr, FALSE);

    GST_DEBUG_OBJECT(spool, "pool start");

    GST_BUFFER_POOL_LOCK(spool);
    if (spool->surface == nullptr) {
        GST_BUFFER_POOL_UNLOCK(spool);
        GST_ERROR_OBJECT(spool, "not set surface");
        return FALSE;
    }

    OHOS::SurfaceError err = spool->surface->SetQueueSize(spool->maxBuffers);
    if (err != OHOS::SurfaceError::SURFACE_ERROR_OK) {
        GST_BUFFER_POOL_UNLOCK(spool);
        GST_ERROR_OBJECT(spool, "set queue size to %u failed", spool->maxBuffers);
        return FALSE;
    }

    gst_surface_allocator_set_surface(spool->allocator, spool->surface);
    GST_INFO_OBJECT(spool, "Set pool minbuf %u maxbuf %u", spool->minBuffers, spool->maxBuffers);

    spool->freeBufCnt = spool->maxBuffers;
    GST_BUFFER_POOL_UNLOCK(spool);

    if (spool->task == nullptr) {
        spool->task = gst_task_new((GstTaskFunction)gst_producer_surface_pool_request_loop, spool, nullptr);
        g_return_val_if_fail(spool->task != nullptr, FALSE);
        gst_task_set_lock(spool->task, &spool->taskLock);
        gst_object_set_name(GST_OBJECT_CAST(spool->task), "surface_pool_req");
    }

    GST_BUFFER_POOL_LOCK(spool);
    spool->started = TRUE;
    if (!gst_task_set_state(spool->task, GST_TASK_STARTED)) {
        spool->started = FALSE;
        GST_BUFFER_POOL_UNLOCK(spool);
        return FALSE;
    }
    GST_BUFFER_POOL_UNLOCK(spool);
    return TRUE;
}

static gboolean gst_producer_surface_pool_stop(GstBufferPool *pool)
{
    g_return_val_if_fail(pool != nullptr, FALSE);
    GstProducerSurfacePool *spool = GST_PRODUCER_SURFACE_POOL_CAST(pool);
    g_return_val_if_fail(spool != nullptr, FALSE);

    GST_DEBUG_OBJECT(spool, "pool stop");

    GST_BUFFER_POOL_LOCK(spool);
    spool->started = FALSE;
    GST_BUFFER_POOL_NOTIFY(spool); // wakeup immediately
    if (spool->surface != nullptr) {
        spool->surface->CleanCache();
    }
    GST_BUFFER_POOL_UNLOCK(spool);
    (void)gst_task_stop(spool->task);
    GST_BUFFER_POOL_LOCK(spool);
    clear_preallocated_buffer(spool);

    // leave all configuration unchanged.
    gboolean ret = (spool->freeBufCnt == spool->maxBuffers);
    GST_BUFFER_POOL_UNLOCK(spool);

    g_rec_mutex_lock(&spool->taskLock);
    GST_INFO_OBJECT(spool, "surface pool req task lock got");
    g_rec_mutex_unlock(&spool->taskLock);
    (void)gst_task_join(spool->task);
    gst_object_unref(spool->task);
    spool->task = nullptr;

    return ret;
}

static GstFlowReturn do_alloc_memory_locked(GstProducerSurfacePool *spool,
    GstBufferPoolAcquireParams *params, GstSurfaceMemory **memory)
{
    GstVideoInfo *info = &spool->info;
    GstSurfaceAllocParam allocParam = {
        GST_VIDEO_INFO_WIDTH(info), GST_VIDEO_INFO_HEIGHT(info), spool->format, spool->usage,
        (params != nullptr ? ((params->flags & GST_BUFFER_POOL_ACQUIRE_FLAG_DONTWAIT) != 0) : FALSE),
        spool->scale_type
    };

    GST_DEBUG_OBJECT(spool, "do_alloc_memory_locked");
    *memory = gst_surface_allocator_alloc(spool->allocator, allocParam);
    if (*memory == nullptr) {
        return GST_FLOW_EOS;
    }
    return GST_FLOW_OK;
}

static GstFlowReturn gst_producer_surface_pool_alloc_buffer(GstBufferPool *pool,
    GstBuffer **buffer, GstBufferPoolAcquireParams *params)
{
    g_return_val_if_fail(pool != nullptr && buffer != nullptr, GST_FLOW_ERROR);
    GstProducerSurfacePool *spool = GST_PRODUCER_SURFACE_POOL_CAST(pool);
    g_return_val_if_fail(spool != nullptr, GST_FLOW_ERROR);

    GST_DEBUG_OBJECT(spool, "alloc surface buffer");

    GstSurfaceMemory *memory = nullptr;
    GstFlowReturn ret = do_alloc_memory_locked(spool, params, &memory);
    if (memory == nullptr) {
        GST_WARNING_OBJECT(spool, "allocator mem fail");
        return ret;
    }

    *buffer = gst_buffer_new();
    if (*buffer == nullptr) {
        GST_ERROR_OBJECT(spool, "alloc gst buffer failed");
        gst_allocator_free(reinterpret_cast<GstAllocator*>(spool->allocator), reinterpret_cast<GstMemory*>(memory));
        return GST_FLOW_ERROR;
    }

    gst_buffer_append_memory(*buffer, reinterpret_cast<GstMemory *>(memory));

    // add buffer type meta at here.
    OHOS::sptr<OHOS::SurfaceBuffer> buf = memory->buf;
    auto buffer_handle = buf->GetBufferHandle();
    g_return_val_if_fail(buffer_handle != nullptr, GST_FLOW_ERROR);
    int32_t stride = buffer_handle->stride;
    GstBufferHandleConfig config = { sizeof(buffer_handle), memory->fence, 0, 0, 0, 0, 0 };
    gst_buffer_add_buffer_handle_meta(*buffer, reinterpret_cast<intptr_t>(buffer_handle), config);

    GstVideoInfo *info = &spool->info;
    g_return_val_if_fail(info != nullptr && info->finfo != nullptr, GST_FLOW_ERROR);
    for (guint plane = 0; plane < info->finfo->n_planes; ++plane) {
        info->stride[plane] = stride;
        GST_DEBUG_OBJECT(spool, "new stride %d", stride);
    }
    gst_buffer_add_video_meta_full(*buffer, GST_VIDEO_FRAME_FLAG_NONE, GST_VIDEO_INFO_FORMAT(info),
        GST_VIDEO_INFO_WIDTH(info), GST_VIDEO_INFO_HEIGHT(info), info->finfo->n_planes, info->offset, info->stride);
    return GST_FLOW_OK;
}

static void gst_producer_surface_pool_free_buffer(GstBufferPool *pool, GstBuffer *buffer)
{
    (void)pool;
    g_return_if_fail(buffer != nullptr);
    gst_buffer_unref(buffer);
}

static GstFlowReturn gst_producer_surface_pool_acquire_buffer(GstBufferPool *pool,
    GstBuffer **buffer, GstBufferPoolAcquireParams *params)
{
    g_return_val_if_fail(pool != nullptr && buffer != nullptr, GST_FLOW_ERROR);
    GstProducerSurfacePool *spool = GST_PRODUCER_SURFACE_POOL_CAST(pool);
    g_return_val_if_fail(spool != nullptr, GST_FLOW_ERROR);
    GstFlowReturn ret = GST_FLOW_OK;

    GST_DEBUG_OBJECT(spool, "acquire buffer");
    GST_BUFFER_POOL_LOCK(spool);

    while (TRUE) {
        // when pool is set flushing or set inactive, the flushing state is true, refer to GstBufferPool
        if (GST_BUFFER_POOL_IS_FLUSHING(pool)) {
            ret = GST_FLOW_FLUSHING;
            GST_INFO_OBJECT(spool, "pool is flushing");
            break;
        }
        if (spool->started == FALSE) {
            ret = GST_FLOW_ERROR;
            GST_ERROR_OBJECT(spool, "surface pool is not started");
            break;
        }
        GList *node = g_list_first(spool->preAllocated);
        if (node != nullptr) {
            *buffer = GST_BUFFER_CAST(node->data);
            if (*buffer == nullptr) {
                ret = GST_FLOW_ERROR;
                GST_ERROR_OBJECT(spool, "buffer is nullptr");
                break;
            }
            spool->preAllocated = g_list_delete_link(spool->preAllocated, node);
            GST_DEBUG_OBJECT(spool, "acquire buffer from preallocated buffers");
            spool->freeBufCnt -= 1;
            GST_BUFFER_POOL_NOTIFY(spool);
            ret = GST_FLOW_OK;
            break;
        }

        if ((params != nullptr) && (params->flags & GST_BUFFER_POOL_ACQUIRE_FLAG_DONTWAIT)) {
            ret = GST_FLOW_EOS;
            break;
        }

        GST_BUFFER_POOL_WAIT(spool);
    }

    if (ret == GST_FLOW_EOS) {
        GST_DEBUG_OBJECT(spool, "no more buffers");
    }

    // The GstBufferPool will add the GstBuffer's pool ref to this pool.
    GST_BUFFER_POOL_UNLOCK(spool);
    return ret;
}

static void gst_producer_surface_pool_release_buffer(GstBufferPool *pool, GstBuffer *buffer)
{
    // The GstBufferPool has already cleared the GstBuffer's pool ref to this pool.

    GstProducerSurfacePool *spool = GST_PRODUCER_SURFACE_POOL_CAST(pool);
    g_return_if_fail(spool != nullptr);
    GST_DEBUG_OBJECT(spool, "release buffer 0x%06" PRIXPTR "", FAKE_POINTER(buffer));

    if (G_UNLIKELY(!gst_buffer_is_all_memory_writable(buffer))) {
        GST_WARNING_OBJECT(spool, "buffer is not writable, 0x%06" PRIXPTR, FAKE_POINTER(buffer));
    }

    // we dont queue the buffer to the idlelist. the memory rotation reuse feature
    // provided by the Surface itself.
    gst_producer_surface_pool_free_buffer(pool, buffer);

    GST_BUFFER_POOL_LOCK(spool);
    spool->freeBufCnt += 1;
    GST_BUFFER_POOL_UNLOCK(spool);
}

static void gst_producer_surface_pool_flush_start(GstBufferPool *pool)
{
    GstProducerSurfacePool *spool = GST_PRODUCER_SURFACE_POOL_CAST(pool);
    g_return_if_fail(spool != nullptr);

    GST_BUFFER_POOL_LOCK(spool);
    GST_BUFFER_POOL_NOTIFY(spool);
    GST_BUFFER_POOL_UNLOCK(spool);
}
