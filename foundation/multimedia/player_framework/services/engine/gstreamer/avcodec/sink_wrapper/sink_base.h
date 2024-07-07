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

#ifndef SINK_BASE_H
#define SINK_BASE_H

#include <cstdint>
#include <gst/gst.h>
#include "avcodec_common.h"
#include "avsharedmemory.h"
#include "codec_common.h"
#include "format.h"
#include "i_avcodec_engine.h"
#include "media_errors.h"
#include "surface.h"
#include "buffer_type_meta.h"

namespace OHOS {
namespace Media {
enum SinkType : int32_t {
    SINK_TYPE_BYTEBUFFER = 0,
    SINK_TYPE_SURFACE,
};

class SinkBase {
public:
    virtual ~SinkBase() = default;
    virtual int32_t Init() = 0;
    virtual int32_t Configure(std::shared_ptr<ProcessorConfig> config) = 0;
    virtual int32_t Flush() = 0;

    virtual int32_t SetOutputSurface(sptr<Surface> surface)
    {
        (void)surface;
        return MSERR_INVALID_OPERATION;
    }

    virtual int32_t SetOutputBuffersCount(uint32_t maxBuffers)
    {
        (void)maxBuffers;
        return MSERR_INVALID_OPERATION;
    }

    virtual int32_t SetCacheBuffersCount(uint32_t cacheBuffers)
    {
        (void)cacheBuffers;
        return MSERR_INVALID_OPERATION;
    }

    virtual std::shared_ptr<AVSharedMemory> GetOutputBuffer(uint32_t index)
    {
        (void)index;
        return nullptr;
    }

    virtual const GstElement *GetElement()
    {
        return sink_;
    }

    virtual int32_t ReleaseOutputBuffer(uint32_t index, bool render) = 0;

    virtual int32_t SetParameter(const Format &format)
    {
        (void)format;
        return MSERR_OK;
    }

    virtual int32_t SetCallback(const std::weak_ptr<IAVCodecEngineObs> &obs) = 0;

    virtual bool IsEos()
    {
        return false;
    }

protected:
    GstElement *sink_ = nullptr;
    static void GetFlagFromBuffer(GstBuffer *buffer, AVCodecBufferFlag &flag)
    {
        GstBufferTypeMeta *bufferType = gst_buffer_get_buffer_type_meta(buffer);
        if (bufferType == nullptr) {
            flag = AVCODEC_BUFFER_FLAG_NONE;
            return;
        }
        switch (static_cast<BufferFlags>(bufferType->bufferFlag)) {
            case BUFFER_FLAG_EOS:
                flag = AVCODEC_BUFFER_FLAG_EOS;
                break;
            case BUFFER_FLAG_SYNC_FRAME:
                flag = AVCODEC_BUFFER_FLAG_SYNC_FRAME;
                break;
            case BUFFER_FLAG_PARTIAL_FRAME:
                flag = AVCODEC_BUFFER_FLAG_PARTIAL_FRAME;
                break;
            case BUFFER_FLAG_CODEC_DATA:
                flag = AVCODEC_BUFFER_FLAG_CODEC_DATA;
                break;
            default:
                flag = AVCODEC_BUFFER_FLAG_NONE;
                break;
        }
    }
};
} // namespace Media
} // namespace OHOS
#endif // SINK_BASE_H