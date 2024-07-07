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

#ifndef GST_PRODUCER_SURFACE_POOL_H
#define GST_PRODUCER_SURFACE_POOL_H

#include <gst/gst.h>
#include <gst/video/video-info.h>
#include "surface.h"
#include "gst_surface_allocator.h"
#include "display_type.h"

G_BEGIN_DECLS

#define GST_TYPE_PRODUCER_SURFACE_POOL (gst_producer_surface_pool_get_type())
#define GST_PRODUCER_SURFACE_POOL(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_PRODUCER_SURFACE_POOL, GstProducerSurfacePool))
#define GST_PRODUCER_SURFACE_POOL_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_PRODUCER_SURFACE_POOL, GstProducerSurfacePoolClass))
#define GST_IS_SURFACE_POOL(obj) \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_PRODUCER_SURFACE_POOL))
#define GST_IS_SURFACE_POOL_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_PRODUCER_SURFACE_POOL))
#define GST_PRODUCER_SURFACE_POOL_CAST(obj) ((GstProducerSurfacePool*)(obj))

typedef struct _GstProducerSurfacePool GstProducerSurfacePool;
typedef struct _GstProducerSurfacePoolClass GstProducerSurfacePoolClass;

struct _GstProducerSurfacePool {
    GstBufferPool basepool;
    OHOS::sptr<OHOS::Surface> surface;
    gboolean started;
    GstSurfaceAllocator *allocator;
    GstAllocationParams params;
    PixelFormat format;
    GstVideoInfo info;
    guint minBuffers;
    guint maxBuffers;
    GMutex lock;
    GCond cond;
    GList *preAllocated;
    guint freeBufCnt;
    guint64 usage;
    GstTask *task;
    GRecMutex taskLock;
    timeval beginTime;
    timeval endTime;
    int32_t callCnt;
    gboolean isDynamicCached;
    guint cachedBuffers;
    guint scale_type;
};

struct _GstProducerSurfacePoolClass {
    GstBufferPoolClass basepool_class;
};

GType gst_producer_surface_pool_get_type(void);

GstProducerSurfacePool *gst_producer_surface_pool_new(void);

GST_API gboolean gst_producer_surface_pool_set_surface(GstProducerSurfacePool *pool,
    OHOS::sptr<OHOS::Surface> surface);

G_END_DECLS

#endif