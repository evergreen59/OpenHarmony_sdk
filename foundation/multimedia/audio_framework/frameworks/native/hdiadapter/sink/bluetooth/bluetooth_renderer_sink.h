/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef BLUETOOTH_RENDERER_SINK_H
#define BLUETOOTH_RENDERER_SINK_H

#include "audio_info.h"
#include "audio_proxy_manager.h"
#include "running_lock.h"

#include <cstdio>
#include <list>

namespace OHOS {
namespace AudioStandard {
typedef struct {
    HDI::Audio_Bluetooth::AudioFormat format;
    uint32_t sampleFmt;
    uint32_t sampleRate;
    uint32_t channel;
    float volume;
} BluetoothSinkAttr;

class BluetoothRendererSink {
public:
    int32_t Init(const BluetoothSinkAttr &attr);
    void DeInit(void);
    int32_t Start(void);
    int32_t Stop(void);
    int32_t Flush(void);
    int32_t Reset(void);
    int32_t Pause(void);
    int32_t Resume(void);
    int32_t RenderFrame(char &data, uint64_t len, uint64_t &writeLen);
    int32_t SetVolume(float left, float right);
    int32_t GetVolume(float &left, float &right);
    int32_t GetLatency(uint32_t *latency);
    int32_t GetTransactionId(uint64_t *transactionId);
    static BluetoothRendererSink *GetInstance(void);
    bool GetAudioMonoState();
    float GetAudioBalanceValue();
    void SetAudioMonoState(bool audioMono);
    void SetAudioBalanceValue(float audioBalance);
    bool rendererInited_;

private:
    BluetoothRendererSink();
    ~BluetoothRendererSink();
    BluetoothSinkAttr attr_;
    bool started_;
    bool paused_;
    float leftVolume_;
    float rightVolume_;
    struct HDI::Audio_Bluetooth::AudioProxyManager *audioManager_;
    struct HDI::Audio_Bluetooth::AudioAdapter *audioAdapter_;
    struct HDI::Audio_Bluetooth::AudioRender *audioRender_;
    struct HDI::Audio_Bluetooth::AudioPort audioPort = {};
    void *handle_;
    bool audioMonoState_ = false;
    bool audioBalanceState_ = false;
    float audioBalanceValue_ = 0.0f;
    float leftBalanceCoef_ = 1.0f;
    float rightBalanceCoef_ = 1.0f;

    std::shared_ptr<PowerMgr::RunningLock> mKeepRunningLock;

    int32_t CreateRender(struct HDI::Audio_Bluetooth::AudioPort &renderPort);
    int32_t InitAudioManager();
    void AdjustStereoToMono(char *data, uint64_t len);
    void AdjustAudioBalance(char *data, uint64_t len);
#ifdef BT_DUMPFILE
    FILE *pfd;
#endif // DUMPFILE
};
}  // namespace AudioStandard
}  // namespace OHOS
#endif // BLUETOOTH_RENDERER_SINK_H
