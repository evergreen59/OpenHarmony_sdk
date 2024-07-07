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

#ifndef AUDIO_SINK_H
#define AUDIO_SINK_H

#include <cstdint>
#include <cstddef>
#include <gst/gst.h>
#include <gst/base/gstbasesink.h>
#include <string>

namespace OHOS {
namespace Media {
class AudioSink {
public:
    virtual ~AudioSink() = default;

    virtual GstCaps *GetCaps() = 0;
    virtual int32_t SetVolume(float volume) = 0;
    virtual int32_t GetVolume(float &volume) = 0;
    virtual int32_t GetMaxVolume(float &volume) = 0;
    virtual int32_t GetMinVolume(float &volume) = 0;
    virtual int32_t Prepare(int32_t appUid, int32_t appPid) = 0;
    virtual int32_t Start() = 0;
    virtual int32_t Stop() = 0;
    virtual int32_t Pause() = 0;
    virtual int32_t Drain() = 0;
    virtual int32_t Flush() = 0;
    virtual int32_t Release() = 0;
    virtual int32_t SetParameters(uint32_t bitsPerSample, uint32_t channels, uint32_t sampleRate) = 0;
    virtual int32_t GetParameters(uint32_t &bitsPerSample, uint32_t &channels, uint32_t &sampleRate) = 0;
    virtual int32_t GetMinimumBufferSize(uint32_t &bufferSize) = 0;
    virtual int32_t GetMinimumFrameCount(uint32_t &frameCount) = 0;
    virtual int32_t Write(uint8_t *buffer, size_t size) = 0;
    virtual int32_t GetAudioTime(uint64_t &time) = 0;
    virtual int32_t GetLatency(uint64_t &latency) const = 0;
    virtual int32_t SetRendererInfo(int32_t desc, int32_t rendererFlags) = 0;
    virtual void SetAudioInterruptMode(int32_t interruptMode) = 0;
    virtual bool Writeable() const;
    virtual void SetAudioSinkCb(void (*interruptCb)(GstBaseSink *, guint, guint, guint),
                                void (*stateCb)(GstBaseSink *, guint),
                                void (*errorCb)(GstBaseSink *, const std::string &)) = 0;
};
} // namespace Media
} // namespace OHOS

#endif // AUDIO_SINK_H
