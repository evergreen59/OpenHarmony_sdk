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

#ifndef GST_CONSUMER_SURFACE_ALLOCATOR_H
#define GST_CONSUMER_SURFACE_ALLOCATOR_H

#include <gst/gst.h>
#include "surface.h"

G_BEGIN_DECLS

#define GST_TYPE_CONSUMER_SURFACE_ALLOCATOR (gst_consumer_surface_allocator_get_type())
#define GST_CONSUMER_SURFACE_ALLOCATOR(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_CONSUMER_SURFACE_ALLOCATOR, GstConsumerSurfaceAllocator))
#define GST_CONSUMER_SURFACE_ALLOCATOR_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_CONSUMER_SURFACE_ALLOCATOR, GstConsumerSurfaceAllocatorClass))
#define GST_IS_CONSUMER_SURFACE_ALLOCATOR(obj) \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_CONSUMER_SURFACE_ALLOCATOR))
#define GST_IS_CONSUMER_SURFACE_ALLOCATOR_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_CONSUMER_SURFACE_ALLOCATOR))
#define GST_CONSUMER_SURFACE_ALLOCATOR_CAST(obj) ((GstConsumerSurfaceAllocator*)(obj))

typedef struct _GstConsumerSurfaceAllocator GstConsumerSurfaceAllocator;
typedef struct _GstConsumerSurfaceAllocatorClass GstConsumerSurfaceAllocatorClass;
typedef struct _GstConsumerSurfaceAllocatorPrivate GstConsumerSurfaceAllocatorPrivate;

struct _GstConsumerSurfaceAllocator {
    GstAllocator parent;

    /* < private > */
    GstConsumerSurfaceAllocatorPrivate *priv;
};

struct _GstConsumerSurfaceAllocatorClass {
    GstAllocatorClass parent;
};

GType gst_consumer_surface_allocator_get_type(void);

GstAllocator *gst_consumer_surface_allocator_new();

void gst_consumer_surface_allocator_set_surface(GstAllocator *allocator,
    OHOS::sptr<OHOS::Surface> &consumerSurface);

G_END_DECLS

#endif
