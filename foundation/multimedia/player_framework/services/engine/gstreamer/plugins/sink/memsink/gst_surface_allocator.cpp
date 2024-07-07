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

#include "gst_surface_allocator.h"
#include <sync_fence.h>
#include "media_log.h"
#include "media_dfx.h"

GST_DEBUG_CATEGORY_STATIC(gst_surface_allocator_debug_category);
#define GST_CAT_DEFAULT gst_surface_allocator_debug_category

#define gst_surface_allocator_parent_class parent_class
G_DEFINE_TYPE(GstSurfaceAllocator, gst_surface_allocator, GST_TYPE_ALLOCATOR);

using namespace OHOS;
using namespace OHOS::Media;

enum class VideoScaleType {
    VIDEO_SCALE_TYPE_FIT,
    VIDEO_SCALE_TYPE_FIT_CROP,
};

namespace {
    const std::unordered_map<VideoScaleType, OHOS::ScalingMode> SCALEMODE_MAP = {
        { VideoScaleType::VIDEO_SCALE_TYPE_FIT, OHOS::SCALING_MODE_SCALE_TO_WINDOW },
        { VideoScaleType::VIDEO_SCALE_TYPE_FIT_CROP, OHOS::SCALING_MODE_SCALE_CROP},
    };
}

gboolean gst_surface_allocator_set_surface(GstSurfaceAllocator *allocator, OHOS::sptr<OHOS::Surface> surface)
{
    if (allocator == nullptr) {
        GST_ERROR("allocator is nullptr");
        return FALSE;
    }
    if (surface == nullptr) {
        GST_ERROR("surface is nullptr");
        return FALSE;
    }
    allocator->surface = surface;
    return TRUE;
}

static OHOS::ScalingMode gst_surface_allocator_get_scale_type(GstSurfaceAllocParam param)
{
    if (SCALEMODE_MAP.find(static_cast<VideoScaleType>(param.scale_type)) == SCALEMODE_MAP.end()) {
        return OHOS::SCALING_MODE_SCALE_TO_WINDOW;
    }
    return SCALEMODE_MAP.at(static_cast<VideoScaleType>(param.scale_type));
}

static bool gst_surface_request_buffer(const GstSurfaceAllocator *allocator, GstSurfaceAllocParam param,
    OHOS::sptr<OHOS::SurfaceBuffer> &buffer)
{
    MediaTrace trace("Surface::RequestBuffer");
    static constexpr int32_t stride_alignment = 8;
    int32_t wait_time = param.dont_wait ? 0 : INT_MAX; // wait forever or no wait.
    OHOS::BufferRequestConfig request_config = {
        param.width, param.height, stride_alignment, param.format,
        param.usage | BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        wait_time
    };
    int32_t release_fence = -1;
    OHOS::SurfaceError ret = allocator->surface->RequestBuffer(buffer, release_fence, request_config);
    if (ret != OHOS::SurfaceError::SURFACE_ERROR_OK || buffer == nullptr) {
        GST_ERROR("there is no more surface buffer");
        return false;
    }
    {
        MediaTrace mapTrace("Surface::Map");
        if (buffer->Map() != OHOS::SurfaceError::SURFACE_ERROR_OK) {
            GST_ERROR("surface buffer Map failed");
            allocator->surface->CancelBuffer(buffer);
            return false;
        }
    }

    {
        MediaTrace FenceTrace("Surface::WaitFence");
        OHOS::sptr<OHOS::SyncFence> autoFence = new(std::nothrow) OHOS::SyncFence(release_fence);
        if (autoFence != nullptr) {
            autoFence->Wait(100); // 100ms
        }
    }

    {
        MediaTrace scaleTrace("Surface::SetScalingMode");
        auto scaleType = gst_surface_allocator_get_scale_type(param);
        ret = allocator->surface->SetScalingMode(buffer->GetSeqNum(), scaleType);
        if (ret != OHOS::SurfaceError::SURFACE_ERROR_OK) {
            GST_ERROR("surface buffer set scaling mode failed");
            allocator->surface->CancelBuffer(buffer);
            return false;
        }
    }
    return true;
}

GstSurfaceMemory *gst_surface_allocator_alloc(GstSurfaceAllocator *allocator, GstSurfaceAllocParam param)
{
    g_return_val_if_fail(allocator != nullptr && allocator->surface != nullptr, nullptr);

    OHOS::sptr<OHOS::SurfaceBuffer> buffer = nullptr;
    if (!gst_surface_request_buffer(allocator, param, buffer) || buffer == nullptr) {
        GST_ERROR("failed to request surface buffer");
        return nullptr;
    }

    GstSurfaceMemory *memory = reinterpret_cast<GstSurfaceMemory *>(g_slice_alloc0(sizeof(GstSurfaceMemory)));
    if (memory == nullptr) {
        GST_ERROR("alloc GstSurfaceMemory slice failed");
        allocator->surface->CancelBuffer(buffer);
        return nullptr;
    }

    gst_memory_init(GST_MEMORY_CAST(memory), (GstMemoryFlags)0, GST_ALLOCATOR_CAST(allocator), nullptr,
        buffer->GetSize(), 0, 0, buffer->GetSize());

    memory->buf = buffer;
    memory->fence = -1;
    memory->need_render = FALSE;
    GST_DEBUG("alloc surface buffer for width: %d, height: %d, format: %d, size: %u",
        param.width, param.height, param.format, buffer->GetSize());

    return memory;
}

static void gst_surface_allocator_free(GstAllocator *baseAllocator, GstMemory *baseMemory)
{
    GstSurfaceAllocator *allocator = reinterpret_cast<GstSurfaceAllocator*>(baseAllocator);
    GstSurfaceMemory *memory = reinterpret_cast<GstSurfaceMemory*>(baseMemory);
    g_return_if_fail(memory != nullptr && allocator != nullptr && allocator->surface != nullptr);

    GST_DEBUG("free surface buffer for width: %d, height: %d, format: %d, size: %u, need_render: %d, fence: %d",
        memory->buf->GetWidth(), memory->buf->GetHeight(), memory->buf->GetFormat(), memory->buf->GetSize(),
        memory->need_render, memory->fence);

    if (!memory->need_render) {
        OHOS::SurfaceError ret = allocator->surface->CancelBuffer(memory->buf);
        if (ret != OHOS::SurfaceError::SURFACE_ERROR_OK) {
            GST_INFO("cancel buffer to surface failed, %d", ret);
        }
    }

    memory->buf = nullptr;
    g_slice_free(GstSurfaceMemory, memory);
}

static GstMemory *gst_surface_allocator_alloc_dummy(GstAllocator *allocator, gsize size, GstAllocationParams *params)
{
    (void)allocator;
    (void)size;
    (void)params;
    return nullptr;
}

static gpointer gst_surface_allocator_mem_map(GstMemory *mem, gsize maxsize, GstMapFlags flags)
{
    (void)maxsize;
    (void)flags;
    g_return_val_if_fail(mem != nullptr, nullptr);
    g_return_val_if_fail(gst_is_surface_memory(mem), nullptr);

    GstSurfaceMemory *sf_mem = reinterpret_cast<GstSurfaceMemory *>(mem);
    g_return_val_if_fail(sf_mem->buf != nullptr, nullptr);

    return sf_mem->buf->GetVirAddr();
}

static void gst_surface_allocator_mem_unmap(GstMemory *mem)
{
    (void)mem;
}

static void gst_surface_allocator_init(GstSurfaceAllocator *allocator)
{
    GstAllocator *base_allocator = GST_ALLOCATOR_CAST(allocator);
    g_return_if_fail(base_allocator != nullptr);

    GST_DEBUG_OBJECT(allocator, "init allocator 0x%06" PRIXPTR "", FAKE_POINTER(allocator));

    base_allocator->mem_type = GST_SURFACE_MEMORY_TYPE;
    base_allocator->mem_map = (GstMemoryMapFunction)gst_surface_allocator_mem_map;
    base_allocator->mem_unmap = (GstMemoryUnmapFunction)gst_surface_allocator_mem_unmap;
}

static void gst_surface_allocator_finalize(GObject *obj)
{
    GstSurfaceAllocator *allocator = GST_SURFACE_ALLOCATOR_CAST(obj);
    g_return_if_fail(allocator != nullptr);

    allocator->surface = nullptr;
    GST_DEBUG_OBJECT(allocator, "finalize allocator 0x%06" PRIXPTR "", FAKE_POINTER(allocator));
    G_OBJECT_CLASS(parent_class)->finalize(obj);
}

static void gst_surface_allocator_class_init(GstSurfaceAllocatorClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    g_return_if_fail(gobject_class != nullptr);

    gobject_class->finalize = gst_surface_allocator_finalize;

    GstAllocatorClass *allocatorClass = GST_ALLOCATOR_CLASS(klass);
    g_return_if_fail(allocatorClass != nullptr);

    allocatorClass->alloc = gst_surface_allocator_alloc_dummy;
    allocatorClass->free = gst_surface_allocator_free;
    GST_DEBUG_CATEGORY_INIT(gst_surface_allocator_debug_category, "prosurallocator", 0, "surface allocator");
}

GstSurfaceAllocator *gst_surface_allocator_new()
{
    GstSurfaceAllocator *alloc = GST_SURFACE_ALLOCATOR_CAST(g_object_new(
        GST_TYPE_SURFACE_ALLOCATOR, "name", "SurfaceAllocator", nullptr));
    (void)gst_object_ref_sink(alloc);

    return alloc;
}
