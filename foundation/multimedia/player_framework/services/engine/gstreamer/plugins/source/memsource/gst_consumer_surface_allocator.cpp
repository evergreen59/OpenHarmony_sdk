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

#include "gst_consumer_surface_allocator.h"
#include "gst_consumer_surface_memory.h"
#include "media_log.h"
#include "scope_guard.h"
#include "securec.h"

using namespace OHOS;

#define GST_CONSUMER_SURFACE_MEMORY_TYPE "ConsumerSurfaceMemory"

#define gst_consumer_surface_allocator_parent_class parent_class

GST_DEBUG_CATEGORY_STATIC(gst_consumer_surface_allocator_debug_category);
#define GST_CAT_DEFAULT gst_consumer_surface_allocator_debug_category

struct _GstConsumerSurfaceAllocatorPrivate {
    sptr<Surface> csurface;
};

G_DEFINE_TYPE_WITH_PRIVATE(GstConsumerSurfaceAllocator, gst_consumer_surface_allocator, GST_TYPE_ALLOCATOR);

static void gst_consumer_surface_allocator_class_init(GstConsumerSurfaceAllocatorClass *klass);
static void gst_consumer_surface_allocator_free(GstAllocator *allocator, GstMemory *mem);
static gpointer gst_consumer_surface_allocator_mem_map(GstMemory *mem, gsize maxsize, GstMapFlags flags);
static void gst_consumer_surface_allocator_init(GstConsumerSurfaceAllocator *sallocator);
static void gst_consumer_surface_allocator_finalize(GObject *obj);

gboolean gst_is_consumer_surface_memory(GstMemory *mem)
{
    return gst_memory_is_type(mem, GST_CONSUMER_SURFACE_MEMORY_TYPE);
}

static GstMemory *gst_consumer_surface_allocator_alloc(GstAllocator *allocator, gsize size, GstAllocationParams *params)
{
    (void)size;
    g_return_val_if_fail(params != nullptr, nullptr);
    g_return_val_if_fail(allocator != nullptr, nullptr);
    GstConsumerSurfaceAllocator *sallocator = GST_CONSUMER_SURFACE_ALLOCATOR(allocator);
    g_return_val_if_fail(sallocator != nullptr && sallocator->priv != nullptr, nullptr);
    g_return_val_if_fail(sallocator->priv->csurface != nullptr, nullptr);
    GstConsumerSurfaceMemory *mem =
        reinterpret_cast<GstConsumerSurfaceMemory *>(g_slice_alloc0(sizeof(GstConsumerSurfaceMemory)));
    g_return_val_if_fail(mem != nullptr, nullptr);

    ON_SCOPE_EXIT(0) { g_slice_free(GstConsumerSurfaceMemory, mem); };
    // shorten code
    sptr<Surface> surface = sallocator->priv->csurface;
    sptr<SurfaceBuffer> surface_buffer = nullptr;
    gint32 fencefd = -1;
    gint64 timestamp = 0;
    gint32 data_size = 0;
    gboolean end_of_stream = false;
    int32_t pixel_format = 0;
    Rect damage = {0, 0, 0, 0};
    if (surface->AcquireBuffer(surface_buffer, fencefd, timestamp, damage) != SURFACE_ERROR_OK) {
        GST_WARNING_OBJECT(allocator, "Acquire surface buffer failed");
        return nullptr;
    }
    g_return_val_if_fail(surface_buffer != nullptr, nullptr);
    ON_SCOPE_EXIT(1) { surface->ReleaseBuffer(surface_buffer, -1); };

    const sptr<OHOS::BufferExtraData>& extraData = surface_buffer->GetExtraData();
    if (extraData != nullptr) {
        (void)extraData->ExtraGet("timeStamp", timestamp);
        (void)extraData->ExtraGet("endOfStream", end_of_stream);
        (void)extraData->ExtraGet("dataSize", data_size);
    }

    pixel_format = surface_buffer->GetFormat();

    gsize realsize = surface_buffer->GetSize();
    g_return_val_if_fail(realsize > 0, nullptr);
    gst_memory_init(GST_MEMORY_CAST(mem), GST_MEMORY_FLAG_NO_SHARE,
        allocator, nullptr, realsize, 0, 0, realsize);
    mem->surface_buffer = surface_buffer;
    mem->fencefd = fencefd;
    mem->timestamp = timestamp;
    mem->data_size = data_size;
    mem->pixel_format = pixel_format;
    mem->damage = damage;
    mem->is_eos_frame = end_of_stream;
    mem->buffer_handle = surface_buffer->GetBufferHandle();
    mem->width = static_cast<uint32_t>(surface_buffer->GetWidth());
    mem->height = static_cast<uint32_t>(surface_buffer->GetHeight());
    GST_INFO_OBJECT(allocator, "acquire surface buffer");

    CANCEL_SCOPE_EXIT_GUARD(0);
    CANCEL_SCOPE_EXIT_GUARD(1);
    return GST_MEMORY_CAST(mem);
}

static void gst_consumer_surface_allocator_free(GstAllocator *allocator, GstMemory *mem)
{
    g_return_if_fail(mem != nullptr && allocator != nullptr);
    g_return_if_fail(gst_is_consumer_surface_memory(mem));
    GstConsumerSurfaceAllocator *sallocator = GST_CONSUMER_SURFACE_ALLOCATOR(allocator);
    g_return_if_fail(sallocator->priv != nullptr && sallocator->priv->csurface != nullptr);

    GstConsumerSurfaceMemory *surfacemem = reinterpret_cast<GstConsumerSurfaceMemory*>(mem);
    (void)sallocator->priv->csurface->ReleaseBuffer(surfacemem->surface_buffer, surfacemem->fencefd);
    GST_INFO_OBJECT(allocator, "release surface buffer");
    surfacemem->surface_buffer = nullptr;
    g_slice_free(GstConsumerSurfaceMemory, surfacemem);
}

static gpointer gst_consumer_surface_allocator_mem_map(GstMemory *mem, gsize maxsize, GstMapFlags flags)
{
    (void)maxsize;
    (void)flags;
    g_return_val_if_fail(mem != nullptr, nullptr);
    g_return_val_if_fail(gst_is_consumer_surface_memory(mem), nullptr);

    GstConsumerSurfaceMemory *surfacemem = reinterpret_cast<GstConsumerSurfaceMemory*>(mem);
    g_return_val_if_fail(surfacemem->surface_buffer != nullptr, nullptr);

    return surfacemem->surface_buffer->GetVirAddr();
}

static void gst_consumer_surface_allocator_mem_unmap(GstMemory *mem)
{
    (void)mem;
}

static GstMemory *gst_consumer_surface_allocator_mem_copy(GstConsumerSurfaceMemory *mem, gssize offset, gssize size)
{
    g_return_val_if_fail(mem != nullptr && mem->surface_buffer != nullptr, nullptr);
    g_return_val_if_fail(mem->surface_buffer->GetVirAddr() != nullptr, nullptr);

    gssize realOffset = 0;
    if (((gint64)mem->parent.offset + offset) > INT32_MAX) {
        GST_ERROR("invalid offset");
        return nullptr;
    } else {
        realOffset = static_cast<gssize>(mem->parent.offset) + offset;
    }
    g_return_val_if_fail(realOffset >= 0, nullptr);

    if (size == -1) {
        if (((gint64)mem->parent.size - offset) > INT32_MAX) {
            GST_ERROR("invalid size");
            return nullptr;
        } else {
            size = static_cast<gssize>(mem->parent.size) - offset;
        }
    }
    g_return_val_if_fail(size > 0, nullptr);

    if ((size < INT32_MAX) && ((INT32_MAX - size) <= realOffset)) {
        GST_ERROR("invalid limit");
        return nullptr;
    }

    gsize realLimit = static_cast<gsize>(size) + static_cast<gsize>(realOffset);
    g_return_val_if_fail(realLimit <= static_cast<gsize>(mem->surface_buffer->GetSize()), nullptr);

    GstMemory *copy = gst_allocator_alloc(nullptr, static_cast<gsize>(size), nullptr);
    g_return_val_if_fail(copy != nullptr, nullptr);

    GstMapInfo info = GST_MAP_INFO_INIT;
    if (!gst_memory_map(copy, &info, GST_MAP_WRITE)) {
        gst_memory_unref(copy);
        GST_ERROR("map failed");
        return nullptr;
    }

    uint8_t *src = static_cast<uint8_t *>(mem->surface_buffer->GetVirAddr()) + realOffset;
    errno_t rc = memcpy_s(info.data, info.size, src, static_cast<size_t>(size));
    if (rc != EOK) {
        GST_ERROR("memcpy failed");
        gst_memory_unmap(copy, &info);
        gst_memory_unref(copy);
        return nullptr;
    }

    gst_memory_unmap(copy, &info);
    GST_LOG("copy memory: 0x%06" PRIXPTR " for size: %" G_GSSIZE_FORMAT ", offset: %" G_GSSIZE_FORMAT
        ", gstmemory: 0x%06" PRIXPTR, FAKE_POINTER(mem), size, offset, FAKE_POINTER(copy));

    return copy;
}

static void gst_consumer_surface_allocator_init(GstConsumerSurfaceAllocator *sallocator)
{
    GstAllocator *allocator = GST_ALLOCATOR_CAST(sallocator);
    g_return_if_fail(allocator != nullptr);
    auto priv = reinterpret_cast<GstConsumerSurfaceAllocatorPrivate *>(
                gst_consumer_surface_allocator_get_instance_private(sallocator));
    g_return_if_fail(priv != nullptr);
    sallocator->priv = priv;

    GST_DEBUG_OBJECT(allocator, "init allocator 0x%06" PRIXPTR "", FAKE_POINTER(allocator));

    allocator->mem_type = GST_CONSUMER_SURFACE_MEMORY_TYPE;
    allocator->mem_map = (GstMemoryMapFunction)gst_consumer_surface_allocator_mem_map;
    allocator->mem_unmap = (GstMemoryUnmapFunction)gst_consumer_surface_allocator_mem_unmap;
    allocator->mem_copy = (GstMemoryCopyFunction)gst_consumer_surface_allocator_mem_copy;
}

static void gst_consumer_surface_allocator_finalize(GObject *obj)
{
    GstConsumerSurfaceAllocator *allocator = GST_CONSUMER_SURFACE_ALLOCATOR(obj);
    g_return_if_fail(allocator != nullptr && allocator->priv != nullptr);
    allocator->priv->csurface = nullptr;

    GST_DEBUG_OBJECT(allocator, "finalize allocator 0x%06" PRIXPTR "", FAKE_POINTER(allocator));
    G_OBJECT_CLASS(parent_class)->finalize(obj);
}

static void gst_consumer_surface_allocator_class_init(GstConsumerSurfaceAllocatorClass *klass)
{
    GObjectClass *gobjectClass = G_OBJECT_CLASS(klass);
    g_return_if_fail(gobjectClass != nullptr);
    GST_DEBUG_CATEGORY_INIT(gst_consumer_surface_allocator_debug_category, "surfaceallocator", 0, "surface allocator");
    gobjectClass->finalize = gst_consumer_surface_allocator_finalize;

    GstAllocatorClass *allocatorClass = GST_ALLOCATOR_CLASS(klass);
    g_return_if_fail(allocatorClass != nullptr);

    allocatorClass->alloc = gst_consumer_surface_allocator_alloc;
    allocatorClass->free = gst_consumer_surface_allocator_free;
}

void gst_consumer_surface_allocator_set_surface(GstAllocator *allocator, sptr<Surface> &consumerSurface)
{
    GstConsumerSurfaceAllocator *sallocator = GST_CONSUMER_SURFACE_ALLOCATOR(allocator);
    g_return_if_fail(sallocator != nullptr && sallocator->priv != nullptr);
    sallocator->priv->csurface = consumerSurface;
}

GstAllocator *gst_consumer_surface_allocator_new()
{
    GstAllocator *alloc = GST_ALLOCATOR_CAST(g_object_new(
        GST_TYPE_CONSUMER_SURFACE_ALLOCATOR, "name", "ConsumerSurface::Allocator", nullptr));
    (void)gst_object_ref_sink(alloc);

    return alloc;
}
