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

#include <cstring>
#include <dlfcn.h>
#include <string>
#include <cinttypes>
#include "audio_errors.h"
#include "audio_log.h"
#include "audio_utils.h"
#include "audio_capturer_source.h"
#include "power_mgr_client.h"

using namespace std;

namespace OHOS {
namespace AudioStandard {
#ifdef CAPTURE_DUMP
const char *g_audioOutTestFilePath = "/data/local/tmp/audio_capture.pcm";
#endif // CAPTURE_DUMP
bool AudioCapturerSource::micMuteState_ = false;

AudioCapturerSource::AudioCapturerSource()
    : capturerInited_(false), started_(false), paused_(false), leftVolume_(MAX_VOLUME_LEVEL),
      rightVolume_(MAX_VOLUME_LEVEL), openMic_(0), audioManager_(nullptr), audioAdapter_(nullptr),
      audioCapture_(nullptr)
{
    attr_ = {};
#ifdef CAPTURE_DUMP
    pfd = nullptr;
#endif // CAPTURE_DUMP
}

AudioCapturerSource::~AudioCapturerSource()
{
    AUDIO_ERR_LOG("~AudioCapturerSource");
}

AudioCapturerSource *AudioCapturerSource::GetInstance()
{
    static AudioCapturerSource audioCapturer_;
    return &audioCapturer_;
}

bool AudioCapturerSource::IsInited(void)
{
    return capturerInited_;
}

void AudioCapturerSource::DeInit()
{
    started_ = false;
    capturerInited_ = false;

    if ((audioCapture_ != nullptr) && (audioAdapter_ != nullptr)) {
        audioAdapter_->DestroyCapture(audioAdapter_, audioCapture_);
    }
    audioCapture_ = nullptr;

    if ((audioManager_ != nullptr) && (audioAdapter_ != nullptr)) {
        audioManager_->UnloadAdapter(audioManager_, audioAdapter_);
    }
    audioAdapter_ = nullptr;
    audioManager_ = nullptr;
#ifdef CAPTURE_DUMP
    if (pfd) {
        fclose(pfd);
        pfd = nullptr;
    }
#endif // CAPTURE_DUMP
}

void InitAttrsCapture(struct AudioSampleAttributes &attrs)
{
    /* Initialization of audio parameters for playback */
    attrs.format = AUDIO_FORMAT_PCM_16_BIT;
    attrs.channelCount = AUDIO_CHANNELCOUNT;
    attrs.sampleRate = AUDIO_SAMPLE_RATE_48K;
    attrs.interleaved = true;
    attrs.streamId = INTERNAL_INPUT_STREAM_ID;
    attrs.type = AUDIO_IN_MEDIA;
    attrs.period = DEEP_BUFFER_CAPTURE_PERIOD_SIZE;
    attrs.frameSize = PCM_16_BIT * attrs.channelCount / PCM_8_BIT;
    attrs.isBigEndian = false;
    attrs.isSignedData = true;
    attrs.startThreshold = DEEP_BUFFER_CAPTURE_PERIOD_SIZE / (attrs.frameSize);
    attrs.stopThreshold = INT_32_MAX;
    /* 16 * 1024 */
    attrs.silenceThreshold = AUDIO_BUFF_SIZE;
}

int32_t SwitchAdapterCapture(struct AudioAdapterDescriptor *descs, int32_t size, const std::string &adapterNameCase,
    enum AudioPortDirection portFlag, struct AudioPort &capturePort)
{
    if (descs == nullptr) {
        return ERROR;
    }

    for (int32_t index = 0; index < size; index++) {
        struct AudioAdapterDescriptor *desc = &descs[index];
        if (desc == nullptr || desc->adapterName == nullptr) {
            continue;
        }
        if (!adapterNameCase.compare(desc->adapterName)) {
            for (uint32_t port = 0; port < desc->portNum; port++) {
                // Only find out the port of out in the sound card
                if (desc->ports[port].dir == portFlag) {
                    capturePort = desc->ports[port];
                    return index;
                }
            }
        }
    }
    AUDIO_ERR_LOG("SwitchAdapterCapture Fail");

    return ERR_INVALID_INDEX;
}

int32_t AudioCapturerSource::InitAudioManager()
{
    AUDIO_INFO_LOG("AudioCapturerSource: Initialize audio proxy manager");

    audioManager_ = GetAudioManagerFuncs();
    if (audioManager_ == nullptr) {
        return ERR_INVALID_HANDLE;
    }

    return 0;
}

int32_t AudioCapturerSource::CreateCapture(struct AudioPort &capturePort)
{
    int32_t ret;
    struct AudioSampleAttributes param;
    // User needs to set
    InitAttrsCapture(param);
    param.sampleRate = attr_.sampleRate;
    param.format = attr_.format;
    param.isBigEndian = attr_.isBigEndian;
    param.channelCount = attr_.channel;
    param.silenceThreshold = attr_.bufferSize;
    param.frameSize = param.format * param.channelCount;
    param.startThreshold = DEEP_BUFFER_CAPTURE_PERIOD_SIZE / (param.frameSize);

    struct AudioDeviceDescriptor deviceDesc;
    deviceDesc.portId = capturePort.portId;
    deviceDesc.pins = PIN_IN_MIC;
    deviceDesc.desc = nullptr;

    ret = audioAdapter_->CreateCapture(audioAdapter_, &deviceDesc, &param, &audioCapture_);
    if (audioCapture_ == nullptr || ret < 0) {
        AUDIO_ERR_LOG("Create capture failed");
        return ERR_NOT_STARTED;
    }

    return 0;
}

int32_t AudioCapturerSource::Init(IAudioSourceAttr &attr)
{
    attr_ = attr;
    int32_t ret;
    int32_t index;
    int32_t size = 0;
    struct AudioAdapterDescriptor *descs = nullptr;
    if (InitAudioManager() != 0) {
        AUDIO_ERR_LOG("Init audio manager Fail");
        return ERR_INVALID_HANDLE;
    }
    ret = audioManager_->GetAllAdapters(audioManager_, &descs, &size);
    // adapters is 0~3
    if (size > MAX_AUDIO_ADAPTER_NUM || size == 0 || descs == nullptr || ret != 0) {
        AUDIO_ERR_LOG("Get adapters Fail");
        return ERR_NOT_STARTED;
    }
    // Get qualified sound card and port
    adapterNameCase_ = attr_.adapterName;
    openMic_ = attr_.open_mic_speaker;
    index = SwitchAdapterCapture(descs, size, adapterNameCase_, PORT_IN, audioPort);
    if (index < 0) {
        AUDIO_ERR_LOG("Switch Adapter Capture Fail");
        return ERR_NOT_STARTED;
    }
    struct AudioAdapterDescriptor *desc = &descs[index];
    if (audioManager_->LoadAdapter(audioManager_, desc, &audioAdapter_) != 0) {
        AUDIO_ERR_LOG("Load Adapter Fail");
        return ERR_NOT_STARTED;
    }
    if (audioAdapter_ == nullptr) {
        AUDIO_ERR_LOG("Load audio device failed");
        return ERR_NOT_STARTED;
    }

    // Inittialization port information, can fill through mode and other parameters
    ret = audioAdapter_->InitAllPorts(audioAdapter_);
    if (ret != 0) {
        AUDIO_ERR_LOG("InitAllPorts failed");
        return ERR_DEVICE_INIT;
    }
    if (CreateCapture(audioPort) != 0) {
        AUDIO_ERR_LOG("Create capture failed");
        return ERR_NOT_STARTED;
    }
    if (openMic_) {
        ret = SetInputRoute(DEVICE_TYPE_MIC);
        if (ret < 0) {
            AUDIO_ERR_LOG("AudioCapturerSource:update route FAILED: %{public}d", ret);
        }
    }
    capturerInited_ = true;

#ifdef CAPTURE_DUMP
    pfd = fopen(g_audioOutTestFilePath, "wb+");
    if (pfd == nullptr) {
        AUDIO_ERR_LOG("Error opening pcm test file!");
    }
#endif // CAPTURE_DUMP

    return SUCCESS;
}

int32_t AudioCapturerSource::CaptureFrame(char *frame, uint64_t requestBytes, uint64_t &replyBytes)
{
    int64_t stamp = GetNowTimeMs();
    int32_t ret;
    if (audioCapture_ == nullptr) {
        AUDIO_ERR_LOG("Audio capture Handle is nullptr!");
        return ERR_INVALID_HANDLE;
    }

    ret = audioCapture_->CaptureFrame(audioCapture_, frame, requestBytes, &replyBytes);
    if (ret < 0) {
        AUDIO_ERR_LOG("Capture Frame Fail");
        return ERR_READ_FAILED;
    }

#ifdef CAPTURE_DUMP
    size_t writeResult = fwrite(frame, replyBytes, 1, pfd);
    if (writeResult != replyBytes) {
        AUDIO_ERR_LOG("Failed to write the file.");
    }
#endif // CAPTURE_DUMP

    stamp = GetNowTimeMs() - stamp;
    AUDIO_DEBUG_LOG("RenderFrame len[%{public}" PRIu64 "] cost[%{public}" PRId64 "]ms", requestBytes, stamp);
    return SUCCESS;
}

int32_t AudioCapturerSource::Start(void)
{
    AUDIO_INFO_LOG("Start.");

    if (mKeepRunningLock == nullptr) {
        mKeepRunningLock = PowerMgr::PowerMgrClient::GetInstance().CreateRunningLock("AudioPrimaryCapturer",
            PowerMgr::RunningLockType::RUNNINGLOCK_BACKGROUND);
    }

    if (mKeepRunningLock != nullptr) {
        AUDIO_INFO_LOG("AudioCapturerSourceInner call KeepRunningLock lock");
        mKeepRunningLock->Lock(0); // 0 for lasting.
    } else {
        AUDIO_ERR_LOG("mKeepRunningLock is null, start can not work well!");
    }

    int32_t ret;
    if (!started_) {
        ret = audioCapture_->control.Start((AudioHandle)audioCapture_);
        if (ret < 0) {
            return ERR_NOT_STARTED;
        }
        started_ = true;
    }

    return SUCCESS;
}

int32_t AudioCapturerSource::SetVolume(float left, float right)
{
    float volume;
    if (audioCapture_ == nullptr) {
        AUDIO_ERR_LOG("AudioCapturerSource::SetVolume failed audioCapture_ null");
        return ERR_INVALID_HANDLE;
    }

    rightVolume_ = right;
    leftVolume_ = left;
    if ((leftVolume_ == 0) && (rightVolume_ != 0)) {
        volume = rightVolume_;
    } else if ((leftVolume_ != 0) && (rightVolume_ == 0)) {
        volume = leftVolume_;
    } else {
        volume = (leftVolume_ + rightVolume_) / HALF_FACTOR;
    }

    audioCapture_->volume.SetVolume(reinterpret_cast<AudioHandle>(audioCapture_), volume);

    return SUCCESS;
}

int32_t AudioCapturerSource::GetVolume(float &left, float &right)
{
    float val = 0.0;
    audioCapture_->volume.GetVolume((AudioHandle)audioCapture_, &val);
    left = val;
    right = val;

    return SUCCESS;
}

int32_t AudioCapturerSource::SetMute(bool isMute)
{
    int32_t ret;
    if (audioCapture_ == nullptr) {
        AUDIO_ERR_LOG("AudioCapturerSource::SetMute failed audioCapture_ handle is null!");
        return ERR_INVALID_HANDLE;
    }

    ret = audioCapture_->volume.SetMute((AudioHandle)audioCapture_, isMute);
    if (ret != 0) {
        AUDIO_ERR_LOG("AudioCapturerSource::SetMute failed from hdi");
    }

    if (audioAdapter_ != nullptr) {
        ret = audioAdapter_->SetMicMute(audioAdapter_, isMute);
        if (ret != 0) {
            AUDIO_ERR_LOG("AudioCapturerSource::SetMicMute failed from hdi");
        }
    }

    micMuteState_ = isMute;

    return SUCCESS;
}

int32_t AudioCapturerSource::GetMute(bool &isMute)
{
    int32_t ret;
    if (audioCapture_ == nullptr) {
        AUDIO_ERR_LOG("AudioCapturerSource::GetMute failed audioCapture_ handle is null!");
        return ERR_INVALID_HANDLE;
    }

    bool isHdiMute = false;
    ret = audioCapture_->volume.GetMute((AudioHandle)audioCapture_, &isHdiMute);
    if (ret != 0) {
        AUDIO_ERR_LOG("AudioCapturerSource::GetMute failed from hdi");
    }

    isMute = micMuteState_;

    return SUCCESS;
}

static AudioCategory GetAudioCategory(AudioScene audioScene)
{
    AudioCategory audioCategory;
    switch (audioScene) {
        case AUDIO_SCENE_PHONE_CALL:
            audioCategory = AUDIO_IN_CALL;
            break;
        case AUDIO_SCENE_PHONE_CHAT:
            audioCategory = AUDIO_IN_COMMUNICATION;
            break;
        case AUDIO_SCENE_RINGING:
            audioCategory = AUDIO_IN_RINGTONE;
            break;
        case AUDIO_SCENE_DEFAULT:
            audioCategory = AUDIO_IN_MEDIA;
            break;
        default:
            audioCategory = AUDIO_IN_MEDIA;
            break;
    }
    AUDIO_DEBUG_LOG("AudioCapturerSource: Audio category returned is: %{public}d", audioCategory);

    return audioCategory;
}

static int32_t SetInputPortPin(DeviceType inputDevice, AudioRouteNode &source)
{
    int32_t ret = SUCCESS;

    switch (inputDevice) {
        case DEVICE_TYPE_MIC:
        case DEVICE_TYPE_SPEAKER:
            source.ext.device.type = PIN_IN_MIC;
            source.ext.device.desc = "pin_in_mic";
            break;
        case DEVICE_TYPE_WIRED_HEADSET:
            source.ext.device.type = PIN_IN_HS_MIC;
            source.ext.device.desc = "pin_in_hs_mic";
            break;
        case DEVICE_TYPE_USB_HEADSET:
            source.ext.device.type = PIN_IN_USB_EXT;
            source.ext.device.desc = "pin_in_usb_ext";
            break;
        case DEVICE_TYPE_BLUETOOTH_SCO:
            source.ext.device.type = PIN_IN_BLUETOOTH_SCO_HEADSET;
            source.ext.device.desc = "pin_in_bluetooth_sco_headset";
            break;
        default:
            ret = ERR_NOT_SUPPORTED;
            break;
    }

    return ret;
}

int32_t AudioCapturerSource::SetInputRoute(DeviceType inputDevice)
{
    AudioPortPin inputPortPin = PIN_IN_MIC;
    return SetInputRoute(inputDevice, inputPortPin);
}

int32_t AudioCapturerSource::SetInputRoute(DeviceType inputDevice, AudioPortPin &inputPortPin)
{
    AudioRouteNode source = {};
    AudioRouteNode sink = {};

    int32_t ret = SetInputPortPin(inputDevice, source);
    if (ret != SUCCESS) {
        AUDIO_ERR_LOG("AudioCapturerSource: SetOutputRoute FAILED: %{public}d", ret);
        return ret;
    }

    inputPortPin = source.ext.device.type;
    AUDIO_INFO_LOG("AudioCapturerSource: Input PIN is: 0x%{public}X", inputPortPin);
    source.portId = static_cast<int32_t>(audioPort.portId);
    source.role = AUDIO_PORT_SOURCE_ROLE;
    source.type = AUDIO_PORT_DEVICE_TYPE;
    source.ext.device.moduleId = 0;

    sink.portId = 0;
    sink.role = AUDIO_PORT_SINK_ROLE;
    sink.type = AUDIO_PORT_MIX_TYPE;
    sink.ext.mix.moduleId = 0;
    sink.ext.mix.streamId = INTERNAL_INPUT_STREAM_ID;

    AudioRoute route = {
        .sourcesNum = 1,
        .sources = &source,
        .sinksNum = 1,
        .sinks = &sink,
    };

    if (audioAdapter_ == nullptr) {
        AUDIO_ERR_LOG("SetInputRoute failed, audioAdapter_ is null");
        return ERR_INVALID_PARAM;
    }
    ret = audioAdapter_->UpdateAudioRoute(audioAdapter_, &route, &routeHandle_);
    if (ret != 0) {
        AUDIO_ERR_LOG("UpdateAudioRoute failed");
        return ERR_OPERATION_FAILED;
    }

    return SUCCESS;
}

int32_t AudioCapturerSource::SetAudioScene(AudioScene audioScene, DeviceType activeDevice)
{
    AUDIO_INFO_LOG("AudioCapturerSource::SetAudioScene scene: %{public}d, device: %{public}d",
        audioScene, activeDevice);
    CHECK_AND_RETURN_RET_LOG(audioScene >= AUDIO_SCENE_DEFAULT && audioScene <= AUDIO_SCENE_PHONE_CHAT,
        ERR_INVALID_PARAM, "invalid audioScene");
    if (audioCapture_ == nullptr) {
        AUDIO_ERR_LOG("AudioCapturerSource::SetAudioScene failed audioCapture_ handle is null!");
        return ERR_INVALID_HANDLE;
    }
    if (openMic_) {
        AudioPortPin audioSceneInPort = PIN_IN_MIC;
        int32_t ret = SetInputRoute(activeDevice, audioSceneInPort);
        if (ret < 0) {
            AUDIO_ERR_LOG("AudioCapturerSource: Update route FAILED: %{public}d", ret);
        }
        struct AudioSceneDescriptor scene;
        scene.scene.id = GetAudioCategory(audioScene);
        scene.desc.pins = audioSceneInPort;
        scene.desc.desc = nullptr;
        if (audioCapture_->scene.SelectScene == nullptr) {
            AUDIO_ERR_LOG("AudioCapturerSource: Select scene nullptr");
            return ERR_OPERATION_FAILED;
        }

        ret = audioCapture_->scene.SelectScene((AudioHandle)audioCapture_, &scene);
        if (ret < 0) {
            AUDIO_ERR_LOG("AudioCapturerSource: Select scene FAILED: %{public}d", ret);
            return ERR_OPERATION_FAILED;
        }
    }
    AUDIO_INFO_LOG("AudioCapturerSource::Select audio scene SUCCESS: %{public}d", audioScene);
    return SUCCESS;
}

uint64_t AudioCapturerSource::GetTransactionId()
{
    AUDIO_INFO_LOG("AudioCapturerSource::GetTransactionId in");
    return reinterpret_cast<uint64_t>(audioCapture_);
}

int32_t AudioCapturerSource::Stop(void)
{
    AUDIO_INFO_LOG("Stop.");

    if (mKeepRunningLock != nullptr) {
        AUDIO_INFO_LOG("AudioCapturerSourceInner call KeepRunningLock UnLock");
        mKeepRunningLock->UnLock();
    } else {
        AUDIO_ERR_LOG("mKeepRunningLock is null, stop can not work well!");
    }

    int32_t ret;
    if (started_ && audioCapture_ != nullptr) {
        ret = audioCapture_->control.Stop(reinterpret_cast<AudioHandle>(audioCapture_));
        if (ret < 0) {
            AUDIO_ERR_LOG("Stop capture Failed");
            return ERR_OPERATION_FAILED;
        }
    }
    started_ = false;

    return SUCCESS;
}

int32_t AudioCapturerSource::Pause(void)
{
    int32_t ret;
    if (started_ && audioCapture_ != nullptr) {
        ret = audioCapture_->control.Pause(reinterpret_cast<AudioHandle>(audioCapture_));
        if (ret != 0) {
            AUDIO_ERR_LOG("pause capture Failed");
            return ERR_OPERATION_FAILED;
        }
    }
    paused_ = true;

    return SUCCESS;
}

int32_t AudioCapturerSource::Resume(void)
{
    int32_t ret;
    if (paused_ && audioCapture_ != nullptr) {
        ret = audioCapture_->control.Resume(reinterpret_cast<AudioHandle>(audioCapture_));
        if (ret != 0) {
            AUDIO_ERR_LOG("resume capture Failed");
            return ERR_OPERATION_FAILED;
        }
    }
    paused_ = false;

    return SUCCESS;
}

int32_t AudioCapturerSource::Reset(void)
{
    if (started_ && audioCapture_ != nullptr) {
        audioCapture_->control.Flush(reinterpret_cast<AudioHandle>(audioCapture_));
    }

    return SUCCESS;
}

int32_t AudioCapturerSource::Flush(void)
{
    if (started_ && audioCapture_ != nullptr) {
        audioCapture_->control.Flush(reinterpret_cast<AudioHandle>(audioCapture_));
    }

    return SUCCESS;
}
} // namespace AudioStandard
} // namesapce OHOS
