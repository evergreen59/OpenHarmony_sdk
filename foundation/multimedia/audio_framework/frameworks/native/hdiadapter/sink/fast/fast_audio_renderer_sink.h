/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FAST_AUDIO_RENDERER_SINK_H
#define FAST_AUDIO_RENDERER_SINK_H

#include "audio_info.h"
#include "audio_manager.h"
#include "audio_sink_callback.h"

#include <cstdio>
#include <list>

namespace OHOS {
namespace AudioStandard {
typedef struct {
    const char *adapterName;
    uint32_t openMicSpeaker;
    AudioFormat format;
    uint32_t sampleFmt;
    uint32_t sampleRate;
    uint32_t channel;
    float volume;
    const char *filePath;
} AudioSinkAttr;

class FastAudioRendererSink {
public:
    static FastAudioRendererSink *GetInstance(void);

    int32_t Init(AudioSinkAttr &atrr);
    void DeInit(void);
    int32_t Start(void);
    int32_t Stop(void);
    int32_t Flush(void);
    int32_t Reset(void);
    int32_t Pause(void);
    int32_t Resume(void);
    int32_t RenderFrame(char &frame, uint64_t len, uint64_t &writeLen);
    int32_t SetVolume(float left, float right);
    int32_t GetVolume(float &left, float &right);
    int32_t SetVoiceVolume(float volume);
    int32_t GetLatency(uint32_t *latency);
    bool rendererInited_;

    int32_t GetMmapHandlePosition(uint64_t &frames, int64_t &timeSec, int64_t &timeNanoSec);

    uint32_t frameSizeInByte_ = 1;
    uint32_t eachReadFrameSize_ = 0;
private:
    FastAudioRendererSink();
    ~FastAudioRendererSink();
    AudioSinkAttr attr_;
    bool started_;
    bool paused_;
    float leftVolume_;
    float rightVolume_;
    int32_t routeHandle_ = -1;
    std::string adapterNameCase_;
    struct AudioManager *audioManager_;
    struct AudioAdapter *audioAdapter_;
    struct AudioRender *audioRender_;
    struct AudioPort audioPort_ = {};

    char *bufferAddresss_ = nullptr;
    size_t bufferSize_ = 0;
    uint32_t bufferTotalFrameSize_ = 0;

    bool isFirstWrite_ = true;

    uint64_t alreadyReadFrames_ = 0;
    uint32_t curReadPos_ = 0;
    uint32_t curWritePos_ = 0;
    uint32_t writeAheadPeriod_ = 1;

    int bufferFd_ = -1;

    int32_t PrepareMmapBuffer();
    void ReleaseMmapBuffer();

    void PreparePosition();

    int32_t CreateRender(const struct AudioPort &renderPort);
    int32_t InitAudioManager();
#ifdef DUMPFILE
    FILE *pfd;
#endif // DUMPFILE
};
}  // namespace AudioStandard
}  // namespace OHOS
#endif // FAST_AUDIO_RENDERER_SINK_H
