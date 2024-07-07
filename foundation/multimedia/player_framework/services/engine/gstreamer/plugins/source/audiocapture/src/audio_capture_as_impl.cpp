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

#include "audio_capture_as_impl.h"
#include <vector>
#include <cmath>
#include "media_log.h"
#include "audio_errors.h"
#include "media_errors.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "AudioCaptureAsImpl"};
    constexpr size_t MAXIMUM_BUFFER_SIZE = 100000;
    constexpr uint64_t SEC_TO_NANOSECOND = 1000000000;
}

namespace OHOS {
namespace Media {
AudioCaptureAsImpl::AudioCaptureAsImpl()
{
}

AudioCaptureAsImpl::~AudioCaptureAsImpl()
{
    if (audioCapturer_ != nullptr) {
        (void)audioCapturer_->Release();
        audioCapturer_ = nullptr;
    }
}

bool AudioCaptureAsImpl::CheckAndGetCaptureParameter(uint32_t bitrate, uint32_t channels, uint32_t sampleRate,
    AudioStandard::AudioCapturerParams &params)
{
    (void)bitrate;
    auto supportedSampleList = AudioStandard::AudioCapturer::GetSupportedSamplingRates();
    CHECK_AND_RETURN_RET(supportedSampleList.size() > 0, false);
    bool isValidSampleRate = false;
    for (auto iter = supportedSampleList.cbegin(); iter != supportedSampleList.cend(); ++iter) {
        CHECK_AND_RETURN_RET(static_cast<int32_t>(*iter) > 0, false);
        uint32_t supportedSampleRate = static_cast<uint32_t>(*iter);
        if (sampleRate <= supportedSampleRate) {
            params.samplingRate = *iter;
            isValidSampleRate = true;
            break;
        }
    }
    CHECK_AND_RETURN_RET(isValidSampleRate, false);

    auto supportedChannelsList = AudioStandard::AudioCapturer::GetSupportedChannels();
    CHECK_AND_RETURN_RET(supportedChannelsList.size() > 0, false);
    bool isValidChannels = false;
    for (auto iter = supportedChannelsList.cbegin(); iter != supportedChannelsList.cend(); ++iter) {
        CHECK_AND_RETURN_RET(static_cast<int32_t>(*iter) > 0, false);
        uint32_t supportedChannels = static_cast<uint32_t>(*iter);
        if (channels == supportedChannels) {
            params.audioChannel = *iter;
            isValidChannels = true;
            break;
        }
    }
    CHECK_AND_RETURN_RET(isValidChannels, false);

    return true;
}

bool AudioCaptureAsImpl::IsSupportedCaptureParameter(uint32_t bitrate, uint32_t channels, uint32_t sampleRate)
{
    AudioStandard::AudioCapturerParams params;
    CHECK_AND_RETURN_RET_LOG(CheckAndGetCaptureParameter(bitrate, channels, sampleRate, params), false,
        "unsupport audio params");

    return true;
}

int32_t AudioCaptureAsImpl::SetCaptureParameter(uint32_t bitrate, uint32_t channels, uint32_t sampleRate,
    const AppInfo &appInfo)
{
    if (audioCapturer_ == nullptr) {
        AudioStandard::AppInfo audioAppInfo = {};
        audioAppInfo.appUid = appInfo.appUid;
        audioAppInfo.appPid = appInfo.appPid;
        audioAppInfo.appTokenId = appInfo.appTokenId;

        audioCapturer_ = AudioStandard::AudioCapturer::Create(AudioStandard::AudioStreamType::STREAM_MUSIC,
                                                              audioAppInfo);
        CHECK_AND_RETURN_RET_LOG(audioCapturer_ != nullptr, MSERR_NO_MEMORY, "create audio capturer failed");
    }
    audioCacheCtrl_ = std::make_unique<AudioCacheCtrl>();
    CHECK_AND_RETURN_RET_LOG(audioCacheCtrl_ != nullptr, MSERR_NO_MEMORY, "create audio cache ctrl failed");

    AudioStandard::AudioCapturerParams params;
    CHECK_AND_RETURN_RET_LOG(CheckAndGetCaptureParameter(bitrate, channels, sampleRate, params),
        MSERR_UNSUPPORT_AUD_PARAMS, "unsupport audio params");

    params.audioSampleFormat = AudioStandard::SAMPLE_S16LE;
    params.audioEncoding = AudioStandard::ENCODING_PCM;
    MEDIA_LOGD("SetCaptureParameter out, channels:%{public}d, sampleRate:%{public}d",
        params.audioChannel, params.samplingRate);
    CHECK_AND_RETURN_RET(audioCapturer_->SetParams(params) == AudioStandard::SUCCESS, MSERR_UNKNOWN);
    CHECK_AND_RETURN_RET(audioCapturer_->GetBufferSize(bufferSize_) == AudioStandard::SUCCESS, MSERR_UNKNOWN);
    MEDIA_LOGD("audio buffer size is: %{public}zu", bufferSize_);
    CHECK_AND_RETURN_RET_LOG(bufferSize_ < MAXIMUM_BUFFER_SIZE, MSERR_UNKNOWN, "audio buffer size too long");
    return MSERR_OK;
}

int32_t AudioCaptureAsImpl::GetCaptureParameter(uint32_t &bitrate, uint32_t &channels, uint32_t &sampleRate)
{
    (void)bitrate;
    MEDIA_LOGD("GetCaptureParameter");
    CHECK_AND_RETURN_RET(audioCapturer_ != nullptr, MSERR_INVALID_OPERATION);
    AudioStandard::AudioCapturerParams params;
    CHECK_AND_RETURN_RET(audioCapturer_->GetParams(params) == AudioStandard::SUCCESS, MSERR_UNKNOWN);
    channels = params.audioChannel;
    sampleRate = params.samplingRate;
    MEDIA_LOGD("get channels:%{public}u, sampleRate:%{public}u from audio server", channels, sampleRate);
    CHECK_AND_RETURN_RET(bufferSize_ > 0 && channels > 0 && sampleRate > 0, MSERR_UNKNOWN);
    constexpr uint32_t bitsPerByte = 8;
    constexpr uint32_t audioSampleS16 = 16;
    bufferDurationNs_ = (bufferSize_ * SEC_TO_NANOSECOND) / (sampleRate * (audioSampleS16 / bitsPerByte) * channels);

    MEDIA_LOGD("audio frame duration is (%{public}" PRIu64 ") ns", bufferDurationNs_);
    return MSERR_OK;
}

int32_t AudioCaptureAsImpl::GetSegmentInfo(uint64_t &start)
{
    CHECK_AND_RETURN_RET(audioCapturer_ != nullptr, MSERR_INVALID_OPERATION);
    AudioStandard::Timestamp timeStamp;
    auto timestampBase = AudioStandard::Timestamp::Timestampbase::MONOTONIC;
    CHECK_AND_RETURN_RET_LOG(audioCapturer_->GetAudioTime(timeStamp, timestampBase), MSERR_UNKNOWN,
        "failed to GetAudioTime");
    CHECK_AND_RETURN_RET(timeStamp.time.tv_nsec >= 0 && timeStamp.time.tv_sec >= 0, MSERR_UNKNOWN);
    if (((UINT64_MAX - timeStamp.time.tv_nsec) / SEC_TO_NANOSECOND) <= static_cast<uint64_t>(timeStamp.time.tv_sec)) {
        MEDIA_LOGW("audio frame pts too long, this shouldn't happen");
    }
    start = timeStamp.time.tv_nsec + timeStamp.time.tv_sec * SEC_TO_NANOSECOND;
    MEDIA_LOGD("timestamp from audioCapturer: %{public}" PRIu64 "", start);
    MEDIA_LOGD("audioCapturer timestamp has increased: %{public}" PRIu64 "", start - lastInputTime_);
    lastInputTime_ = start;

    return MSERR_OK;
}

std::shared_ptr<AudioBuffer> AudioCaptureAsImpl::GetSegmentData()
{
    CHECK_AND_RETURN_RET(audioCapturer_ != nullptr, nullptr);
    std::shared_ptr<AudioBuffer> tempBuffer = std::make_shared<AudioBuffer>();
    CHECK_AND_RETURN_RET(tempBuffer != nullptr, nullptr);
    CHECK_AND_RETURN_RET(bufferSize_ > 0 && bufferSize_ < MAXIMUM_BUFFER_SIZE, nullptr);
    tempBuffer->gstBuffer = gst_buffer_new_allocate(nullptr, bufferSize_, nullptr);
    CHECK_AND_RETURN_RET(tempBuffer->gstBuffer != nullptr, nullptr);

    GstMapInfo map = GST_MAP_INFO_INIT;
    if (gst_buffer_map(tempBuffer->gstBuffer, &map, GST_MAP_READ) != TRUE) {
        gst_buffer_unref(tempBuffer->gstBuffer);
        return nullptr;
    }
    bool isBlocking = true;
    int32_t bytesRead = audioCapturer_->Read(*(map.data), map.size, isBlocking);
    gst_buffer_unmap(tempBuffer->gstBuffer, &map);
    if (bytesRead <= 0) {
        gst_buffer_unref(tempBuffer->gstBuffer);
        MEDIA_LOGE("audioCapturer read size %{public}d!", bytesRead);
        return nullptr;
    }

    return tempBuffer;
}

int32_t AudioCaptureAsImpl::AudioCaptureLoop()
{
    while (true) {
        if ((curState_.load() != RECORDER_RUNNING) && (curState_ != RECORDER_RESUME)) {
            return MSERR_OK;
        }

        {
            std::unique_lock<std::mutex> loopLock(audioCacheCtrl_->captureMutex_);
            if (audioCacheCtrl_->captureQueue_.size() >= MAX_QUEUE_SIZE) {
                audioCacheCtrl_->captureCond_.notify_all();
                continue;
            }
        }

        std::shared_ptr<AudioBuffer> tempBuffer = GetSegmentData();
        CHECK_AND_RETURN_RET_LOG(tempBuffer != nullptr, MSERR_UNKNOWN, "tempBuffer is nullptr!");

        uint64_t curTimeStamp = 0;
        if (GetSegmentInfo(curTimeStamp) != MSERR_OK) {
            gst_buffer_unref(tempBuffer->gstBuffer);
            MEDIA_LOGD("failed to  GetSegmentInfo!");
            return MSERR_UNKNOWN;
        }

        tempBuffer->timestamp = curTimeStamp;
        tempBuffer->duration = bufferDurationNs_;
        tempBuffer->dataLen = bufferSize_;

        {
            std::unique_lock<std::mutex> loopLock(audioCacheCtrl_->captureMutex_);
            audioCacheCtrl_->captureQueue_.push(tempBuffer);
            MEDIA_LOGD("audio cache queue size is %{public}zu", audioCacheCtrl_->captureQueue_.size());
            audioCacheCtrl_->captureCond_.notify_all();
        }
    }
}

void AudioCaptureAsImpl::GetAudioCaptureBuffer()
{
    MEDIA_LOGD("GetAudioCaptureBuffer");
    while (true) {
        {
            std::unique_lock<std::mutex> lock(pauseMutex_);
            audioCacheCtrl_->pauseCond_.wait(lock, [this]() {
                return curState_.load() != RECORDER_PAUSED;
            });
        }

        if ((curState_.load() == RECORDER_STOP) || (curState_.load() == RECORDER_INITIALIZED)) {
            MEDIA_LOGD("exit GetAudioCaptureBuffer!");
            return;
        }

        int32_t ret = AudioCaptureLoop();
        if (ret != MSERR_OK && ((curState_.load() == RECORDER_RUNNING) || (curState_ == RECORDER_RESUME))) {
            // runtime error
            captureLoopErr_.store(true);
            audioCacheCtrl_->captureCond_.notify_all();
            MEDIA_LOGE("failed to AudioCaptureLoop");
            return;
        }
    }
}

std::shared_ptr<AudioBuffer> AudioCaptureAsImpl::GetBuffer()
{
    std::unique_lock<std::mutex> loopLock(audioCacheCtrl_->captureMutex_);

    if (curState_.load() == RECORDER_RESUME && audioCacheCtrl_->pausedTime_ == 1) {
        EmptyCaptureQueue();
    }

    audioCacheCtrl_->captureCond_.wait(loopLock, [this]() {
        return ((audioCacheCtrl_->captureQueue_.size() > 0) || (curState_.load() == RECORDER_STOP) ||
            captureLoopErr_.load());
    });
    if (curState_.load() == RECORDER_STOP) {
        return nullptr;
    }

    CHECK_AND_RETURN_RET((audioCacheCtrl_->captureQueue_.size() > 0) || (!captureLoopErr_.load()), nullptr);

    std::shared_ptr<AudioBuffer> bufferOut = audioCacheCtrl_->captureQueue_.front();
    audioCacheCtrl_->captureQueue_.pop();

    if (curState_.load() == RECORDER_PAUSED) {
        audioCacheCtrl_->pausedTime_ = bufferOut->timestamp;
        MEDIA_LOGD("audio pause timestamp %{public}" PRIu64 "", audioCacheCtrl_->pausedTime_);
        EmptyCaptureQueue();
    }
    if (curState_.load() == RECORDER_RESUME) {
        curState_.store(RECORDER_RUNNING);
        if (audioCacheCtrl_->pausedTime_ == 1) {
            audioCacheCtrl_->pausedTime_ = audioCacheCtrl_->lastTimeStamp_;
            MEDIA_LOGD("audio pause timestamp %{public}" PRIu64 "", audioCacheCtrl_->pausedTime_);
        }
        audioCacheCtrl_->resumeTime_ = bufferOut->timestamp;
        MEDIA_LOGD("audio resume timestamp %{public}" PRIu64 "", audioCacheCtrl_->resumeTime_);
        audioCacheCtrl_->persistTime_ = std::fabs(audioCacheCtrl_->resumeTime_ - audioCacheCtrl_->pausedTime_);
        if (audioCacheCtrl_->persistTime_ >= bufferDurationNs_) {
            audioCacheCtrl_->persistTime_ -= bufferDurationNs_;
        }
        audioCacheCtrl_->pausedTime_ = 1; // reset the pause time
        audioCacheCtrl_->totalPauseTime_ += audioCacheCtrl_->persistTime_;
        MEDIA_LOGD("audio has %{public}d times pause, total PauseTime: %{public}" PRIu64 "",
            audioCacheCtrl_->pausedCount_, audioCacheCtrl_->totalPauseTime_);
    }
    audioCacheCtrl_->lastTimeStamp_ = bufferOut->timestamp;
    bufferOut->timestamp -= audioCacheCtrl_->totalPauseTime_;
    return bufferOut;
}

int32_t AudioCaptureAsImpl::StartAudioCapture()
{
    MEDIA_LOGD("StartAudioCapture");

    CHECK_AND_RETURN_RET(audioCapturer_ != nullptr, MSERR_INVALID_OPERATION);
    CHECK_AND_RETURN_RET(audioCapturer_->Start(), MSERR_UNKNOWN);

    curState_.store(RECORDER_RUNNING);
    captureLoop_ = std::make_unique<std::thread>(&AudioCaptureAsImpl::GetAudioCaptureBuffer, this);
    CHECK_AND_RETURN_RET_LOG(captureLoop_ != nullptr, MSERR_INVALID_OPERATION, "create audio cache thread failed");

    return MSERR_OK;
}

int32_t AudioCaptureAsImpl::StopAudioCapture()
{
    MEDIA_LOGD("StopAudioCapture");

    curState_.store(RECORDER_STOP);

    if (captureLoop_ != nullptr && captureLoop_->joinable()) {
        audioCacheCtrl_->pauseCond_.notify_all();
        captureLoop_->join();
        captureLoop_.reset();
    }

    audioCacheCtrl_->captureCond_.notify_all();

    CHECK_AND_RETURN_RET(audioCapturer_ != nullptr, MSERR_INVALID_OPERATION);
    if (audioCapturer_->GetStatus() == AudioStandard::CapturerState::CAPTURER_RUNNING) {
        CHECK_AND_RETURN_RET(audioCapturer_->Stop(), MSERR_UNKNOWN);
    }
    if (audioCapturer_->GetStatus() != AudioStandard::CapturerState::CAPTURER_RELEASED) {
        CHECK_AND_RETURN_RET(audioCapturer_->Release(), MSERR_UNKNOWN);
    }

    {
        std::unique_lock<std::mutex> loopLock(audioCacheCtrl_->captureMutex_);
        EmptyCaptureQueue();
    }

    audioCapturer_ = nullptr;
    audioCacheCtrl_ = nullptr;
    curState_.store(RECORDER_INITIALIZED);
    MEDIA_LOGD("exit StopAudioCapture");
    return MSERR_OK;
}

int32_t AudioCaptureAsImpl::PauseAudioCapture()
{
    MEDIA_LOGD("PauseAudioCapture");

    curState_.store(RECORDER_PAUSED);
    audioCacheCtrl_->pausedCount_++;

    CHECK_AND_RETURN_RET(audioCapturer_ != nullptr, MSERR_INVALID_OPERATION);
    if (audioCapturer_->GetStatus() == AudioStandard::CapturerState::CAPTURER_RUNNING) {
        CHECK_AND_RETURN_RET(audioCapturer_->Stop(), MSERR_UNKNOWN);
    }

    MEDIA_LOGD("exit PauseAudioCapture");
    return MSERR_OK;
}

int32_t AudioCaptureAsImpl::ResumeAudioCapture()
{
    MEDIA_LOGD("ResumeAudioCapture");

    CHECK_AND_RETURN_RET(audioCapturer_ != nullptr, MSERR_INVALID_OPERATION);
    CHECK_AND_RETURN_RET(audioCapturer_->Start(), MSERR_UNKNOWN);

    curState_.store(RECORDER_RESUME);
    audioCacheCtrl_->pauseCond_.notify_all();

    MEDIA_LOGD("exit ResumeAudioCapture");
    return MSERR_OK;
}

int32_t AudioCaptureAsImpl::WakeUpAudioThreads()
{
    MEDIA_LOGD("wake up threads when paused state");

    CHECK_AND_RETURN_RET(audioCapturer_ != nullptr, MSERR_INVALID_OPERATION);

    curState_.store(RECORDER_STOP);
    audioCacheCtrl_->pauseCond_.notify_all();

    {
        std::unique_lock<std::mutex> loopLock(audioCacheCtrl_->captureMutex_);
        audioCacheCtrl_->captureCond_.notify_all();
    }

    return MSERR_OK;
}

void AudioCaptureAsImpl::EmptyCaptureQueue()
{
    MEDIA_LOGD("%{public}zu audio buffer has been dropped", audioCacheCtrl_->captureQueue_.size());
    while (!audioCacheCtrl_->captureQueue_.empty()) {
        auto iter = audioCacheCtrl_->captureQueue_.front();
        if (iter != nullptr) {
            gst_buffer_unref(iter->gstBuffer);
        }
        audioCacheCtrl_->captureQueue_.pop();
    }
}
} // namespace Media
} // namespace OHOS
