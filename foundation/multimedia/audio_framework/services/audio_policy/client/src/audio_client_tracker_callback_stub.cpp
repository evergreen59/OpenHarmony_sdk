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

#include "audio_client_tracker_callback_stub.h"
#include "audio_log.h"

namespace OHOS {
namespace AudioStandard {
AudioClientTrackerCallbackStub::AudioClientTrackerCallbackStub()
{
}

AudioClientTrackerCallbackStub::~AudioClientTrackerCallbackStub()
{
}

int AudioClientTrackerCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    AUDIO_DEBUG_LOG("AudioClientTrackerCallbackStub::OnRemoteRequest");
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        AUDIO_ERR_LOG("AudioClientTrackerCallbackStub: ReadInterfaceToken failed");
        return -1;
    }

    switch (code) {
        case PAUSEDSTREAM: {
            StreamSetStateEventInternal sreamSetStateEventInternal = {};
            sreamSetStateEventInternal.streamSetState= static_cast<StreamSetState>(data.ReadInt32());
            sreamSetStateEventInternal.audioStreamType = static_cast<AudioStreamType>(data.ReadInt32());
            PausedStreamImpl(sreamSetStateEventInternal);
            return AUDIO_OK;
        }
        case RESUMESTREAM: {
            StreamSetStateEventInternal sreamSetStateEventInternal = {};
            sreamSetStateEventInternal.streamSetState= static_cast<StreamSetState>(data.ReadInt32());
            sreamSetStateEventInternal.audioStreamType = static_cast<AudioStreamType>(data.ReadInt32());
            ResumeStreamImpl(sreamSetStateEventInternal);
            return AUDIO_OK;
        }
        case SETLOWPOWERVOL: {
            float volume = data.ReadFloat();
            SetLowPowerVolumeImpl(volume);
            return AUDIO_OK;
        }
        case GETLOWPOWERVOL: {
            float volume;
            GetLowPowerVolumeImpl(volume);
            reply.WriteFloat(volume);
            return AUDIO_OK;
        }
        case GETSINGLESTREAMVOL: {
            float volume;
            GetSingleStreamVolumeImpl(volume);
            reply.WriteFloat(volume);
            return AUDIO_OK;
        }
        default: {
            AUDIO_ERR_LOG("default case, need check AudioListenerStub");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }

    return 0;
}

void AudioClientTrackerCallbackStub::SetClientTrackerCallback(
    const std::weak_ptr<AudioClientTracker> &callback)
{
    AUDIO_DEBUG_LOG("AudioClientTrackerCallbackStub::SetClientTrackerCallback");
    callback_ = callback;
}

void AudioClientTrackerCallbackStub::PausedStreamImpl(
    const StreamSetStateEventInternal &streamSetStateEventInternal)
{
    AUDIO_DEBUG_LOG("AudioClientTrackerCallbackStub PausedStreamImpl start");
    std::shared_ptr<AudioClientTracker> cb = callback_.lock();
    if (cb != nullptr) {
        cb->PausedStreamImpl(streamSetStateEventInternal);
    } else {
        AUDIO_ERR_LOG("AudioClientTrackerCallbackStub: PausedStreamImpl callback_ is nullptr");
    }
}

void AudioClientTrackerCallbackStub::SetLowPowerVolumeImpl(float volume)
{
    AUDIO_DEBUG_LOG("AudioClientTrackerCallbackStub SetLowPowerVolumeImpl start");
    std::shared_ptr<AudioClientTracker> cb = callback_.lock();
    if (cb != nullptr) {
        cb->SetLowPowerVolumeImpl(volume);
    } else {
        AUDIO_ERR_LOG("AudioClientTrackerCallbackStub: SetLowPowerVolumeImpl callback_ is nullptr");
    }
}

void AudioClientTrackerCallbackStub::ResumeStreamImpl(
    const StreamSetStateEventInternal &streamSetStateEventInternal)
{
    AUDIO_DEBUG_LOG("AudioClientTrackerCallbackStub ResumeStreamImpl start");
    std::shared_ptr<AudioClientTracker> cb = callback_.lock();
    if (cb != nullptr) {
        cb->ResumeStreamImpl(streamSetStateEventInternal);
    } else {
        AUDIO_ERR_LOG("AudioClientTrackerCallbackStub: ResumeStreamImpl callback_ is nullptr");
    }
}

void AudioClientTrackerCallbackStub::GetLowPowerVolumeImpl(float &volume)
{
    AUDIO_DEBUG_LOG("AudioClientTrackerCallbackStub GetLowPowerVolumeImpl start");
    std::shared_ptr<AudioClientTracker> cb = callback_.lock();
    if (cb != nullptr) {
        cb->GetLowPowerVolumeImpl(volume);
    } else {
        AUDIO_ERR_LOG("AudioClientTrackerCallbackStub: GetLowPowerVolumeImpl callback_ is nullptr");
    }
}

void AudioClientTrackerCallbackStub::GetSingleStreamVolumeImpl(float &volume)
{
    AUDIO_DEBUG_LOG("AudioClientTrackerCallbackStub GetSingleStreamVolumeImpl start");
    std::shared_ptr<AudioClientTracker> cb = callback_.lock();
    if (cb != nullptr) {
        cb->GetSingleStreamVolumeImpl(volume);
    } else {
        AUDIO_ERR_LOG("AudioClientTrackerCallbackStub: GetSingleStreamVolumeImpl callback_ is nullptr");
    }
}
} // namespace AudioStandard
} // namespace OHOS
