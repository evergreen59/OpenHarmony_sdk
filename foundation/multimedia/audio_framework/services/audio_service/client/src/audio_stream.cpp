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

#include <chrono>
#include <thread>
#include <vector>

#include "audio_errors.h"
#include "audio_log.h"

#include "audio_stream.h"

using namespace std;

namespace OHOS {
namespace AudioStandard {
const unsigned long long TIME_CONVERSION_US_S = 1000000ULL; /* us to s */
const unsigned long long TIME_CONVERSION_NS_US = 1000ULL; /* ns to us */
const unsigned long long TIME_CONVERSION_NS_S = 1000000000ULL; /* ns to s */
constexpr int32_t WRITE_RETRY_DELAY_IN_US = 500;
constexpr int32_t READ_WRITE_WAIT_TIME_IN_US = 500;
constexpr int32_t CB_WRITE_BUFFERS_WAIT_IN_MS = 80;
constexpr int32_t CB_READ_BUFFERS_WAIT_IN_US = 500;

const map<pair<ContentType, StreamUsage>, AudioStreamType> AudioStream::streamTypeMap_ = AudioStream::CreateStreamMap();

map<pair<ContentType, StreamUsage>, AudioStreamType> AudioStream::CreateStreamMap()
{
    map<pair<ContentType, StreamUsage>, AudioStreamType> streamMap;

    streamMap[make_pair(CONTENT_TYPE_UNKNOWN, STREAM_USAGE_UNKNOWN)] = STREAM_MUSIC;
    streamMap[make_pair(CONTENT_TYPE_UNKNOWN, STREAM_USAGE_MEDIA)] = STREAM_MUSIC;
    streamMap[make_pair(CONTENT_TYPE_UNKNOWN, STREAM_USAGE_VOICE_COMMUNICATION)] = STREAM_MUSIC;
    streamMap[make_pair(CONTENT_TYPE_UNKNOWN, STREAM_USAGE_VOICE_ASSISTANT)] = STREAM_MUSIC;
    streamMap[make_pair(CONTENT_TYPE_UNKNOWN, STREAM_USAGE_NOTIFICATION_RINGTONE)] = STREAM_MUSIC;

    streamMap[make_pair(CONTENT_TYPE_SPEECH, STREAM_USAGE_UNKNOWN)] = STREAM_MUSIC;
    streamMap[make_pair(CONTENT_TYPE_SPEECH, STREAM_USAGE_MEDIA)] = STREAM_VOICE_ASSISTANT;
    streamMap[make_pair(CONTENT_TYPE_SPEECH, STREAM_USAGE_VOICE_COMMUNICATION)] = STREAM_VOICE_CALL;
    streamMap[make_pair(CONTENT_TYPE_SPEECH, STREAM_USAGE_VOICE_ASSISTANT)] = STREAM_VOICE_ASSISTANT;
    streamMap[make_pair(CONTENT_TYPE_SPEECH, STREAM_USAGE_NOTIFICATION_RINGTONE)] = STREAM_MUSIC;

    streamMap[make_pair(CONTENT_TYPE_MUSIC, STREAM_USAGE_UNKNOWN)] = STREAM_MUSIC;
    streamMap[make_pair(CONTENT_TYPE_MUSIC, STREAM_USAGE_MEDIA)] = STREAM_MUSIC;
    streamMap[make_pair(CONTENT_TYPE_MUSIC, STREAM_USAGE_VOICE_COMMUNICATION)] = STREAM_MUSIC;
    streamMap[make_pair(CONTENT_TYPE_MUSIC, STREAM_USAGE_VOICE_ASSISTANT)] = STREAM_VOICE_ASSISTANT;
    streamMap[make_pair(CONTENT_TYPE_MUSIC, STREAM_USAGE_NOTIFICATION_RINGTONE)] = STREAM_RING;

    streamMap[make_pair(CONTENT_TYPE_MOVIE, STREAM_USAGE_UNKNOWN)] = STREAM_MEDIA;
    streamMap[make_pair(CONTENT_TYPE_MOVIE, STREAM_USAGE_MEDIA)] = STREAM_MEDIA;
    streamMap[make_pair(CONTENT_TYPE_MOVIE, STREAM_USAGE_VOICE_COMMUNICATION)] = STREAM_MUSIC;
    streamMap[make_pair(CONTENT_TYPE_MOVIE, STREAM_USAGE_VOICE_ASSISTANT)] = STREAM_MUSIC;
    streamMap[make_pair(CONTENT_TYPE_MOVIE, STREAM_USAGE_NOTIFICATION_RINGTONE)] = STREAM_MUSIC;

    streamMap[make_pair(CONTENT_TYPE_SONIFICATION, STREAM_USAGE_UNKNOWN)] = STREAM_NOTIFICATION;
    streamMap[make_pair(CONTENT_TYPE_SONIFICATION, STREAM_USAGE_MEDIA)] = STREAM_NOTIFICATION;
    streamMap[make_pair(CONTENT_TYPE_SONIFICATION, STREAM_USAGE_VOICE_COMMUNICATION)] = STREAM_MUSIC;
    streamMap[make_pair(CONTENT_TYPE_SONIFICATION, STREAM_USAGE_VOICE_ASSISTANT)] = STREAM_MUSIC;
    streamMap[make_pair(CONTENT_TYPE_SONIFICATION, STREAM_USAGE_NOTIFICATION_RINGTONE)] = STREAM_RING;

    streamMap[make_pair(CONTENT_TYPE_RINGTONE, STREAM_USAGE_UNKNOWN)] = STREAM_RING;
    streamMap[make_pair(CONTENT_TYPE_RINGTONE, STREAM_USAGE_MEDIA)] = STREAM_RING;
    streamMap[make_pair(CONTENT_TYPE_RINGTONE, STREAM_USAGE_VOICE_COMMUNICATION)] = STREAM_MUSIC;
    streamMap[make_pair(CONTENT_TYPE_RINGTONE, STREAM_USAGE_VOICE_ASSISTANT)] = STREAM_MUSIC;
    streamMap[make_pair(CONTENT_TYPE_RINGTONE, STREAM_USAGE_NOTIFICATION_RINGTONE)] = STREAM_RING;

    return streamMap;
}

AudioStream::AudioStream(AudioStreamType eStreamType, AudioMode eMode, int32_t appUid)
    : eStreamType_(eStreamType),
      eMode_(eMode),
      state_(NEW),
      isReadInProgress_(false),
      isWriteInProgress_(false),
      resetTime_(false),
      resetTimestamp_(0),
      renderMode_(RENDER_MODE_NORMAL),
      captureMode_(CAPTURE_MODE_NORMAL),
      isReadyToWrite_(false),
      isReadyToRead_(false),
      isFirstRead_(false),
      isFirstWrite_(false)
{
    AUDIO_DEBUG_LOG("AudioStream ctor, appUID = %{public}d", appUid);
    audioStreamTracker_ =  std::make_unique<AudioStreamTracker>(eMode, appUid);
    AUDIO_DEBUG_LOG("AudioStreamTracker created");
}

AudioStream::~AudioStream()
{
    isReadyToWrite_ = false;
    isReadyToRead_ = false;

    if (writeThread_ && writeThread_->joinable()) {
        writeThread_->join();
    }

    if (readThread_ && readThread_->joinable()) {
        readThread_->join();
    }

    if (state_ != RELEASED && state_ != NEW) {
        ReleaseAudioStream(false);
    }

    if (audioStreamTracker_ && audioStreamTracker_.get()) {
        AUDIO_DEBUG_LOG("AudioStream:~AudioStream:Calling update tracker");
        AudioRendererInfo rendererInfo = {};
        AudioCapturerInfo capturerInfo = {};
        state_ = RELEASED;
        audioStreamTracker_->UpdateTracker(sessionId_, state_, rendererInfo, capturerInfo);
    }
}

void AudioStream::SetRendererInfo(const AudioRendererInfo &rendererInfo)
{
    rendererInfo_ = rendererInfo;
}

void AudioStream::SetCapturerInfo(const AudioCapturerInfo &capturerInfo)
{
    capturerInfo_ = capturerInfo;
}

State AudioStream::GetState()
{
    return state_;
}

int32_t AudioStream::GetAudioSessionID(uint32_t &sessionID)
{
    if ((state_ == RELEASED) || (state_ == NEW)) {
        return ERR_ILLEGAL_STATE;
    }

    if (GetSessionID(sessionID) != 0) {
        return ERR_INVALID_INDEX;
    }

    sessionId_ = sessionID;

    return SUCCESS;
}

bool AudioStream::GetAudioTime(Timestamp &timestamp, Timestamp::Timestampbase base)
{
    if (state_ == STOPPED) {
        return false;
    }
    uint64_t paTimeStamp = 0;
    if (GetCurrentTimeStamp(paTimeStamp) == SUCCESS) {
        if (resetTime_) {
            AUDIO_INFO_LOG("AudioStream::GetAudioTime resetTime_ %{public}d", resetTime_);
            resetTime_ = false;
            resetTimestamp_ = paTimeStamp;
        }

        timestamp.time.tv_sec = static_cast<time_t>((paTimeStamp - resetTimestamp_) / TIME_CONVERSION_US_S);
        timestamp.time.tv_nsec
            = static_cast<time_t>(((paTimeStamp - resetTimestamp_) - (timestamp.time.tv_sec * TIME_CONVERSION_US_S))
                                  * TIME_CONVERSION_NS_US);
        timestamp.time.tv_sec += baseTimestamp_.tv_sec;
        timestamp.time.tv_nsec += baseTimestamp_.tv_nsec;
        timestamp.time.tv_sec += (timestamp.time.tv_nsec / TIME_CONVERSION_NS_S);
        timestamp.time.tv_nsec = (timestamp.time.tv_nsec % TIME_CONVERSION_NS_S);

        return true;
    }
    return false;
}

int32_t AudioStream::GetBufferSize(size_t &bufferSize) const
{
    AUDIO_INFO_LOG("AudioStream: Get Buffer size");
    if (GetMinimumBufferSize(bufferSize) != 0) {
        return ERR_OPERATION_FAILED;
    }

    return SUCCESS;
}

int32_t AudioStream::GetFrameCount(uint32_t &frameCount) const
{
    AUDIO_INFO_LOG("AudioStream: Get frame count");
    if (GetMinimumFrameCount(frameCount) != 0) {
        return ERR_OPERATION_FAILED;
    }

    return SUCCESS;
}

int32_t AudioStream::GetLatency(uint64_t &latency)
{
    if (GetAudioLatency(latency) != SUCCESS) {
        return ERR_OPERATION_FAILED;
    } else {
        return SUCCESS;
    }
}

vector<AudioSampleFormat> AudioStream::GetSupportedFormats() const
{
    return AUDIO_SUPPORTED_FORMATS;
}

vector<AudioEncodingType> AudioStream::GetSupportedEncodingTypes() const
{
    return AUDIO_SUPPORTED_ENCODING_TYPES;
}

vector<AudioSamplingRate> AudioStream::GetSupportedSamplingRates() const
{
    return AUDIO_SUPPORTED_SAMPLING_RATES;
}

bool IsFormatValid(uint8_t format)
{
    bool isValidFormat = (find(AUDIO_SUPPORTED_FORMATS.begin(), AUDIO_SUPPORTED_FORMATS.end(), format)
                          != AUDIO_SUPPORTED_FORMATS.end());
    AUDIO_DEBUG_LOG("AudioStream: IsFormatValid: %{public}s", isValidFormat ? "true" : "false");
    return isValidFormat;
}

bool IsRendererChannelValid(uint8_t channel)
{
    bool isValidChannel = (find(RENDERER_SUPPORTED_CHANNELS.begin(), RENDERER_SUPPORTED_CHANNELS.end(), channel)
                           != RENDERER_SUPPORTED_CHANNELS.end());
    AUDIO_DEBUG_LOG("AudioStream: IsChannelValid: %{public}s", isValidChannel ? "true" : "false");
    return isValidChannel;
}

bool IsCapturerChannelValid(uint8_t channel)
{
    bool isValidChannel = (find(CAPTURER_SUPPORTED_CHANNELS.begin(), CAPTURER_SUPPORTED_CHANNELS.end(), channel)
                           != CAPTURER_SUPPORTED_CHANNELS.end());
    AUDIO_DEBUG_LOG("AudioStream: IsChannelValid: %{public}s", isValidChannel ? "true" : "false");
    return isValidChannel;
}

bool IsEncodingTypeValid(uint8_t encodingType)
{
    bool isValidEncodingType
            = (find(AUDIO_SUPPORTED_ENCODING_TYPES.begin(), AUDIO_SUPPORTED_ENCODING_TYPES.end(), encodingType)
               != AUDIO_SUPPORTED_ENCODING_TYPES.end());
    AUDIO_DEBUG_LOG("AudioStream: IsEncodingTypeValid: %{public}s", isValidEncodingType ? "true" : "false");
    return isValidEncodingType;
}

bool IsSamplingRateValid(uint32_t samplingRate)
{
    bool isValidSamplingRate
            = (find(AUDIO_SUPPORTED_SAMPLING_RATES.begin(), AUDIO_SUPPORTED_SAMPLING_RATES.end(), samplingRate)
               != AUDIO_SUPPORTED_SAMPLING_RATES.end());
    AUDIO_DEBUG_LOG("AudioStream: IsSamplingRateValid: %{public}s", isValidSamplingRate ? "true" : "false");
    return isValidSamplingRate;
}

int32_t AudioStream::GetAudioStreamInfo(AudioStreamParams &audioStreamInfo)
{
    AUDIO_INFO_LOG("AudioStream: GetAudioStreamInfo");
    if (GetAudioStreamParams(audioStreamInfo) != 0) {
        return ERR_OPERATION_FAILED;
    }

    return SUCCESS;
}

bool AudioStream::VerifyClientPermission(const std::string &permissionName, uint32_t appTokenId, int32_t appUid,
    bool privacyFlag, AudioPermissionState state)
{
    return AudioServiceClient::VerifyClientPermission(permissionName, appTokenId, appUid, privacyFlag, state);
}

bool AudioStream::getUsingPemissionFromPrivacy(const std::string &permissionName, uint32_t appTokenId,
    AudioPermissionState state)
{
    return AudioServiceClient::getUsingPemissionFromPrivacy(permissionName, appTokenId, state);
}

int32_t AudioStream::SetAudioStreamInfo(const AudioStreamParams info,
    const std::shared_ptr<AudioClientTracker> &proxyObj)
{
    AUDIO_INFO_LOG("AudioStreamInfo, Sampling rate: %{public}d, channels: %{public}d, format: %{public}d,"
        " stream type: %{public}d", info.samplingRate, info.channels, info.format, eStreamType_);

    if (!IsFormatValid(info.format) || !IsSamplingRateValid(info.samplingRate) || !IsEncodingTypeValid(info.encoding)) {
        AUDIO_ERR_LOG("AudioStream: Unsupported audio parameter");
        return ERR_NOT_SUPPORTED;
    }
    if (state_ != NEW) {
        AUDIO_INFO_LOG("AudioStream: State is not new, release existing stream");
        StopAudioStream();
        ReleaseAudioStream(false);
    }
    {
        int32_t ret = 0;
        static std::mutex connectServerMutex;
        std::lock_guard<std::mutex> lockConnect(connectServerMutex);
        if (eMode_ == AUDIO_MODE_PLAYBACK) {
            AUDIO_DEBUG_LOG("AudioStream: Initialize playback");
            if (!IsRendererChannelValid(info.channels)) {
                AUDIO_ERR_LOG("AudioStream: Invalid sink channel %{public}d", info.channels);
                return ERR_NOT_SUPPORTED;
            }
            ret = Initialize(AUDIO_SERVICE_CLIENT_PLAYBACK);
        } else if (eMode_ == AUDIO_MODE_RECORD) {
            AUDIO_DEBUG_LOG("AudioStream: Initialize recording");
            if (!IsCapturerChannelValid(info.channels)) {
                AUDIO_ERR_LOG("AudioStream: Invalid source channel %{public}d", info.channels);
                return ERR_NOT_SUPPORTED;
            }
            ret = Initialize(AUDIO_SERVICE_CLIENT_RECORD);
        } else {
            AUDIO_ERR_LOG("AudioStream: error eMode.");
            return ERR_INVALID_OPERATION;
        }
        CHECK_AND_RETURN_RET_LOG(ret == 0, ret, "AudioStream: Error initializing!");
    }

    if (CreateStream(info, eStreamType_) != SUCCESS) {
        AUDIO_ERR_LOG("AudioStream:Create stream failed");
        return ERROR;
    }
    state_ = PREPARED;
    AUDIO_INFO_LOG("AudioStream:Set stream Info SUCCESS");

    if (audioStreamTracker_ && audioStreamTracker_.get()) {
        (void)GetSessionID(sessionId_);
        AUDIO_DEBUG_LOG("AudioStream:Calling register tracker, sessionid = %{public}d", sessionId_);
        audioStreamTracker_->RegisterTracker(sessionId_, state_, rendererInfo_, capturerInfo_, proxyObj);
    }
    return SUCCESS;
}

bool AudioStream::StartAudioStream(StateChangeCmdType cmdType)
{
    if ((state_ != PREPARED) && (state_ != STOPPED) && (state_ != PAUSED)) {
        AUDIO_ERR_LOG("StartAudioStream Illegal state:%{public}u", state_);
        return false;
    }

    int32_t ret = StartStream(cmdType);
    if (ret != SUCCESS) {
        AUDIO_ERR_LOG("StartStream Start failed:%{public}d", ret);
        return false;
    }

    resetTime_ = true;
    int32_t retCode = clock_gettime(CLOCK_MONOTONIC, &baseTimestamp_);
    if (retCode != 0) {
        AUDIO_ERR_LOG("AudioStream::StartAudioStream get system elapsed time failed: %d", retCode);
    }

    isFirstRead_ = true;
    isFirstWrite_ = true;
    state_ = RUNNING;

    if (renderMode_ == RENDER_MODE_CALLBACK) {
        isReadyToWrite_ = true;
        writeThread_ = std::make_unique<std::thread>(&AudioStream::WriteCbTheadLoop, this);
    } else if (captureMode_ == CAPTURE_MODE_CALLBACK) {
        isReadyToRead_ = true;
        readThread_ = std::make_unique<std::thread>(&AudioStream::ReadCbThreadLoop, this);
    }

    AUDIO_INFO_LOG("StartAudioStream SUCCESS");

    if (audioStreamTracker_ && audioStreamTracker_.get()) {
        AUDIO_DEBUG_LOG("AudioStream:Calling Update tracker for Running");
        audioStreamTracker_->UpdateTracker(sessionId_, state_, rendererInfo_, capturerInfo_);
    }
    return true;
}

int32_t AudioStream::Read(uint8_t &buffer, size_t userSize, bool isBlockingRead)
{
    if (userSize <= 0) {
        AUDIO_ERR_LOG("Invalid userSize:%{public}zu", userSize);
        return ERR_INVALID_PARAM;
    }

    if (state_ != RUNNING) {
        AUDIO_ERR_LOG("Read: State is not RUNNNIG. Illegal  state:%{public}u", state_);
        return ERR_ILLEGAL_STATE;
    }

    if (isFirstRead_) {
        FlushAudioStream();
        isFirstRead_ = false;
    }

    StreamBuffer stream;
    stream.buffer = &buffer;
    stream.bufferLen = userSize;
    isReadInProgress_ = true;
    int32_t readLen = ReadStream(stream, isBlockingRead);
    isReadInProgress_ = false;
    if (readLen < 0) {
        AUDIO_ERR_LOG("ReadStream fail,ret:%{public}d", readLen);
        return ERR_INVALID_READ;
    }

    return readLen;
}

size_t AudioStream::Write(uint8_t *buffer, size_t buffer_size)
{
    if (renderMode_ == RENDER_MODE_CALLBACK) {
        AUDIO_ERR_LOG("AudioStream::Write not supported. RenderMode is callback");
        return ERR_INCORRECT_MODE;
    }

    if ((buffer == nullptr) || (buffer_size <= 0)) {
        AUDIO_ERR_LOG("Invalid buffer size:%{public}zu", buffer_size);
        return ERR_INVALID_PARAM;
    }

    if (state_ != RUNNING) {
        AUDIO_ERR_LOG("Write: Illegal  state:%{public}u", state_);
        // To allow context switch for APIs running in different thread contexts
        std::this_thread::sleep_for(std::chrono::microseconds(WRITE_RETRY_DELAY_IN_US));
        return ERR_ILLEGAL_STATE;
    }

    int32_t writeError;
    StreamBuffer stream;
    stream.buffer = buffer;
    stream.bufferLen = buffer_size;
    isWriteInProgress_ = true;

    if (isFirstWrite_) {
        if (RenderPrebuf(stream.bufferLen)) {
            return ERR_WRITE_FAILED;
        }
        isFirstWrite_ = false;
    }

    size_t bytesWritten = WriteStream(stream, writeError);
    isWriteInProgress_ = false;
    if (writeError != 0) {
        AUDIO_ERR_LOG("WriteStream fail,writeError:%{public}d", writeError);
        return ERR_WRITE_FAILED;
    }
    return bytesWritten;
}

bool AudioStream::PauseAudioStream(StateChangeCmdType cmdType)
{
    if (state_ != RUNNING) {
        AUDIO_ERR_LOG("PauseAudioStream: State is not RUNNING. Illegal state:%{public}u", state_);
        return false;
    }
    State oldState = state_;
    // Update state to stop write thread
    state_ = PAUSED;

    if (captureMode_ == CAPTURE_MODE_CALLBACK) {
        isReadyToRead_ = false;
        if (readThread_ && readThread_->joinable()) {
            readThread_->join();
        }
    }

    // Ends the WriteCb thread
    if (renderMode_ == RENDER_MODE_CALLBACK) {
        isReadyToWrite_ = false;
        // wake write thread to make pause faster
        bufferQueueCV_.notify_all();
        if (writeThread_ && writeThread_->joinable()) {
            writeThread_->join();
        }
    }

    while (isReadInProgress_ || isWriteInProgress_) {
        std::this_thread::sleep_for(std::chrono::microseconds(READ_WRITE_WAIT_TIME_IN_US));
    }
    
    AUDIO_DEBUG_LOG("AudioStream::PauseAudioStream:renderMode_ : %{public}d state_: %{public}d", renderMode_, state_);

    int32_t ret = PauseStream(cmdType);
    if (ret != SUCCESS) {
        AUDIO_DEBUG_LOG("StreamPause fail,ret:%{public}d", ret);
        state_ = oldState;
        return false;
    }

    AUDIO_INFO_LOG("PauseAudioStream SUCCESS");

    // flush stream after stream paused
    FlushAudioStream();

    if (audioStreamTracker_ && audioStreamTracker_.get()) {
        AUDIO_DEBUG_LOG("AudioStream:Calling Update tracker for Pause");
        audioStreamTracker_->UpdateTracker(sessionId_, state_, rendererInfo_, capturerInfo_);
    }
    return true;
}

bool AudioStream::StopAudioStream()
{
    AUDIO_INFO_LOG("AudioStream: begin StopAudioStream for sessionId %{public}d", sessionId_);
    if (state_ == PAUSED) {
        state_ = STOPPED;
        AUDIO_INFO_LOG("StopAudioStream SUCCESS");
        return true;
    }

    if (state_ != RUNNING) {
        AUDIO_ERR_LOG("StopAudioStream: State is not RUNNING. Illegal state:%{public}u", state_);
        return false;
    }
    State oldState = state_;
    state_ = STOPPED; // Set it before stopping as Read/Write and Stop can be called from different threads

    if (captureMode_ == CAPTURE_MODE_CALLBACK) {
        isReadyToRead_ = false;
        if (readThread_ && readThread_->joinable()) {
            readThread_->join();
        }
    }

    if (renderMode_ == RENDER_MODE_CALLBACK) {
        isReadyToWrite_ = false;
        if (writeThread_ && writeThread_->joinable()) {
            writeThread_->join();
        }
    }

    while (isReadInProgress_ || isWriteInProgress_) {
        std::this_thread::sleep_for(std::chrono::microseconds(READ_WRITE_WAIT_TIME_IN_US));
    }

    int32_t ret = StopStream();
    if (ret != SUCCESS) {
        AUDIO_DEBUG_LOG("StreamStop fail,ret:%{public}d", ret);
        state_ = oldState;
        return false;
    }

    AUDIO_INFO_LOG("StopAudioStream SUCCESS");

    if (audioStreamTracker_ && audioStreamTracker_.get()) {
        AUDIO_DEBUG_LOG("AudioStream:Calling Update tracker for stop");
        audioStreamTracker_->UpdateTracker(sessionId_, state_, rendererInfo_, capturerInfo_);
    }
    return true;
}

bool AudioStream::FlushAudioStream()
{
    if ((state_ != RUNNING) && (state_ != PAUSED) && (state_ != STOPPED)) {
        AUDIO_ERR_LOG("FlushAudioStream: State is not RUNNING. Illegal state:%{public}u", state_);
        return false;
    }

    int32_t ret = FlushStream();
    if (ret != SUCCESS) {
        AUDIO_DEBUG_LOG("Flush stream fail,ret:%{public}d", ret);
        return false;
    }

    AUDIO_INFO_LOG("Flush stream SUCCESS");
    return true;
}

bool AudioStream::DrainAudioStream()
{
    if (state_ != RUNNING) {
        AUDIO_ERR_LOG("DrainAudioStream: State is not RUNNING. Illegal  state:%{public}u", state_);
        return false;
    }

    int32_t ret = DrainStream();
    if (ret != SUCCESS) {
        AUDIO_DEBUG_LOG("Drain stream fail,ret:%{public}d", ret);
        return false;
    }

    AUDIO_INFO_LOG("Drain stream SUCCESS");
    return true;
}

bool AudioStream::ReleaseAudioStream(bool releaseRunner)
{
    if (state_ == RELEASED || state_ == NEW) {
        AUDIO_ERR_LOG("Illegal state: state = %{public}u", state_);
        return false;
    }
    // If state_ is RUNNING try to Stop it first and Release
    if (state_ == RUNNING) {
        StopAudioStream();
    }

    ReleaseStream(releaseRunner);
    state_ = RELEASED;
    AUDIO_INFO_LOG("ReleaseAudiostream SUCCESS");

    if (audioStreamTracker_ && audioStreamTracker_.get()) {
        AUDIO_DEBUG_LOG("AudioStream:Calling Update tracker for release");
        audioStreamTracker_->UpdateTracker(sessionId_, state_, rendererInfo_, capturerInfo_);
    }
    return true;
}

AudioStreamType AudioStream::GetStreamType(ContentType contentType, StreamUsage streamUsage)
{
    AudioStreamType streamType = STREAM_MUSIC;
    auto pos = streamTypeMap_.find(make_pair(contentType, streamUsage));
    if (pos != streamTypeMap_.end()) {
        streamType = pos->second;
    }

    if (streamType == STREAM_MEDIA) {
        streamType = STREAM_MUSIC;
    }

    return streamType;
}

int32_t AudioStream::SetAudioStreamType(AudioStreamType audioStreamType)
{
    return SetStreamType(audioStreamType);
}

int32_t AudioStream::SetVolume(float volume)
{
    return SetStreamVolume(volume);
}

float AudioStream::GetVolume()
{
    return GetStreamVolume();
}

int32_t AudioStream::SetRenderRate(AudioRendererRate renderRate)
{
    return SetStreamRenderRate(renderRate);
}

AudioRendererRate AudioStream::GetRenderRate()
{
    return GetStreamRenderRate();
}

int32_t AudioStream::SetStreamCallback(const std::shared_ptr<AudioStreamCallback> &callback)
{
    if (callback == nullptr) {
        AUDIO_ERR_LOG("AudioStream::SetStreamCallback failed. callback == nullptr");
        return ERR_INVALID_PARAM;
    }

    SaveStreamCallback(callback);

    return SUCCESS;
}

int32_t AudioStream::SetRenderMode(AudioRenderMode renderMode)
{
    int32_t ret = SetAudioRenderMode(renderMode);
    if (ret) {
        AUDIO_ERR_LOG("AudioStream::SetRenderMode: renderMode: %{public}d failed", renderMode);
        return ERR_OPERATION_FAILED;
    }
    renderMode_ = renderMode;

    lock_guard<mutex> lock(bufferQueueLock_);

    for (int32_t i = 0; i < MAX_WRITECB_NUM_BUFFERS; ++i) {
        size_t length;
        GetMinimumBufferSize(length);
        AUDIO_INFO_LOG("AudioServiceClient:: GetMinimumBufferSize: %{public}zu", length);

        writeBufferPool_[i] = std::make_unique<uint8_t[]>(length);
        if (writeBufferPool_[i] == nullptr) {
            AUDIO_INFO_LOG(
            "AudioServiceClient::GetBufferDescriptor writeBufferPool_[i]==nullptr. Allocate memory failed.");
            return ERR_OPERATION_FAILED;
        }

        BufferDesc bufDesc {};
        bufDesc.buffer = writeBufferPool_[i].get();
        bufDesc.bufLength = length;
        freeBufferQ_.emplace(bufDesc);
    }

    return SUCCESS;
}

AudioRenderMode AudioStream::GetRenderMode()
{
    return GetAudioRenderMode();
}

int32_t AudioStream::SetCaptureMode(AudioCaptureMode captureMode)
{
    int32_t ret = SetAudioCaptureMode(captureMode);
    if (ret) {
        AUDIO_ERR_LOG("AudioStream::SetCaptureMode: captureMode: %{public}d failed", captureMode);
        return ERR_OPERATION_FAILED;
    }
    captureMode_ = captureMode;

    lock_guard<mutex> lock(bufferQueueLock_);

    for (int32_t i = 0; i < MAX_READCB_NUM_BUFFERS; ++i) {
        size_t length;
        GetMinimumBufferSize(length);
        AUDIO_INFO_LOG("AudioStream::SetCaptureMode: length %{public}zu", length);

        readBufferPool_[i] = std::make_unique<uint8_t[]>(length);
        if (readBufferPool_[i] == nullptr) {
            AUDIO_INFO_LOG("AudioStream::SetCaptureMode readBufferPool_[i]==nullptr. Allocate memory failed.");
            return ERR_OPERATION_FAILED;
        }

        BufferDesc bufDesc {};
        bufDesc.buffer = readBufferPool_[i].get();
        bufDesc.bufLength = length;
        freeBufferQ_.emplace(bufDesc);
    }

    return SUCCESS;
}

AudioCaptureMode AudioStream::GetCaptureMode()
{
    return GetAudioCaptureMode();
}

int32_t AudioStream::SetRendererWriteCallback(const std::shared_ptr<AudioRendererWriteCallback> &callback)
{
    if (renderMode_ != RENDER_MODE_CALLBACK) {
        AUDIO_ERR_LOG("AudioStream::SetRendererWriteCallback not supported. Render mode is not callback.");
        return ERR_INCORRECT_MODE;
    }

    if (!callback) {
        AUDIO_ERR_LOG("SetRendererWriteCallback callback is nullptr");
        return ERR_INVALID_PARAM;
    }
    return AudioServiceClient::SetRendererWriteCallback(callback);
}

int32_t AudioStream::SetCapturerReadCallback(const std::shared_ptr<AudioCapturerReadCallback> &callback)
{
    if (captureMode_ != CAPTURE_MODE_CALLBACK) {
        AUDIO_ERR_LOG("AudioStream::SetCapturerReadCallback not supported. Capture mode is not callback.");
        return ERR_INCORRECT_MODE;
    }

    int32_t ret = SaveReadCallback(callback);
    if (ret) {
        AUDIO_ERR_LOG("AudioStream::SetCapturerReadCallback: failed.");
        return ERR_INVALID_PARAM;
    }

    return SUCCESS;
}

int32_t AudioStream::GetBufferDesc(BufferDesc &bufDesc)
{
    if ((renderMode_ != RENDER_MODE_CALLBACK) && (captureMode_ != CAPTURE_MODE_CALLBACK)) {
        AUDIO_ERR_LOG("AudioStream::GetBufferDesc not supported. Render or Capture mode is not callback.");
        return ERR_INCORRECT_MODE;
    }

    lock_guard<mutex> lock(bufferQueueLock_);

    if (renderMode_ == RENDER_MODE_CALLBACK) {
        if (!freeBufferQ_.empty()) {
            bufDesc.buffer = freeBufferQ_.front().buffer;
            bufDesc.bufLength = freeBufferQ_.front().bufLength;
            bufDesc.dataLength = freeBufferQ_.front().dataLength;
            freeBufferQ_.pop();
        } else {
            bufDesc.buffer = nullptr;
            AUDIO_INFO_LOG("AudioStream::GetBufferDesc freeBufferQ_.empty()");
            return ERR_OPERATION_FAILED;
        }
    }

    if (captureMode_ == CAPTURE_MODE_CALLBACK) {
        if (!filledBufferQ_.empty()) {
            bufDesc.buffer = filledBufferQ_.front().buffer;
            bufDesc.bufLength = filledBufferQ_.front().bufLength;
            bufDesc.dataLength = filledBufferQ_.front().dataLength;
            filledBufferQ_.pop();
        } else {
            bufDesc.buffer = nullptr;
            AUDIO_INFO_LOG("AudioStream::GetBufferDesc filledBufferQ_.empty()");
            return ERR_OPERATION_FAILED;
        }
    }
    return SUCCESS;
}

int32_t AudioStream::GetBufQueueState(BufferQueueState &bufState)
{
    if ((renderMode_ != RENDER_MODE_CALLBACK) && (captureMode_ != CAPTURE_MODE_CALLBACK)) {
        AUDIO_ERR_LOG("AudioStream::GetBufQueueState not supported. Render or Capture mode is not callback.");
        return ERR_INCORRECT_MODE;
    }

    lock_guard<mutex> lock(bufferQueueLock_);

    if (renderMode_ == RENDER_MODE_CALLBACK) {
        bufState.numBuffers = filledBufferQ_.size();
    }

    if (captureMode_ == CAPTURE_MODE_CALLBACK) {
        bufState.numBuffers = freeBufferQ_.size();
    }

    return SUCCESS;
}

int32_t AudioStream::Enqueue(const BufferDesc &bufDesc)
{
    AUDIO_INFO_LOG("AudioStream::Enqueue");
    if ((renderMode_ != RENDER_MODE_CALLBACK) && (captureMode_ != CAPTURE_MODE_CALLBACK)) {
        AUDIO_ERR_LOG("AudioStream::Enqueue not supported. Render or capture mode is not callback.");
        return ERR_INCORRECT_MODE;
    }

    if (bufDesc.buffer == nullptr) {
        AUDIO_ERR_LOG("AudioStream::Enqueue: failed. bufDesc.buffer == nullptr.");
        return ERR_INVALID_PARAM;
    }

    unique_lock<mutex> lock(bufferQueueLock_);

    if (renderMode_ == RENDER_MODE_CALLBACK) {
        AUDIO_DEBUG_LOG("AudioStream::Enqueue: filledBuffer length: %{public}zu.", bufDesc.bufLength);
        filledBufferQ_.emplace(bufDesc);
    }

    if (captureMode_ == CAPTURE_MODE_CALLBACK) {
        AUDIO_DEBUG_LOG("AudioStream::Enqueue: freeBuffer length: %{public}zu.", bufDesc.bufLength);
        freeBufferQ_.emplace(bufDesc);
    }

    bufferQueueCV_.notify_all();

    return SUCCESS;
}

int32_t AudioStream::Clear()
{
    if ((renderMode_ != RENDER_MODE_CALLBACK) && (captureMode_ != CAPTURE_MODE_CALLBACK)) {
        AUDIO_ERR_LOG("AudioStream::Clear not supported. Render or capture mode is not callback.");
        return ERR_INCORRECT_MODE;
    }

    lock_guard<mutex> lock(bufferQueueLock_);

    while (!filledBufferQ_.empty()) {
        freeBufferQ_.emplace(filledBufferQ_.front());
        filledBufferQ_.pop();
    }

    return SUCCESS;
}

void AudioStream::WriteCbTheadLoop()
{
    AUDIO_INFO_LOG("AudioStream::WriteCb thread start");
    StreamBuffer stream;
    size_t bytesWritten;
    int32_t writeError;

    if (isReadyToWrite_) {
        // send write events for application to fill all free buffers at the beginning
        SubmitAllFreeBuffers();

        while (true) {
            if (state_ != RUNNING) {
                AUDIO_INFO_LOG("Write: not running state: %{public}u", state_);
                isReadyToWrite_ = false;
                break;
            }

            unique_lock<mutex> lock(bufferQueueLock_);

            if (filledBufferQ_.empty()) {
                // wait signal with timeout
                bufferQueueCV_.wait_for(lock, chrono::milliseconds(CB_WRITE_BUFFERS_WAIT_IN_MS));
                continue;
            }
            stream.buffer = filledBufferQ_.front().buffer;
            stream.bufferLen = filledBufferQ_.front().bufLength;

            if (stream.buffer == nullptr) {
                AUDIO_ERR_LOG("AudioStream::WriteCb stream.buffer is nullptr return");
                break;
            }
            bytesWritten = WriteStreamInCb(stream, writeError);
            if (writeError != 0) {
                AUDIO_ERR_LOG("AudioStream::WriteStreamInCb fail, writeError:%{public}d", writeError);
            } else {
                AUDIO_DEBUG_LOG("AudioStream::WriteCb WriteStream, bytesWritten:%{public}zu", bytesWritten);
                freeBufferQ_.emplace(filledBufferQ_.front());
                filledBufferQ_.pop();
                SendWriteBufferRequestEvent();
            }
        }
    }
    AUDIO_INFO_LOG("AudioStream::WriteCb thread end");
}

void AudioStream::ReadCbThreadLoop()
{
    AUDIO_INFO_LOG("AudioStream::ReadCb thread start");
    StreamBuffer stream;
    int32_t readLen;
    bool isBlockingRead = true;

    while (isReadyToRead_) {
        while (!freeBufferQ_.empty()) {
            if (state_ != RUNNING) {
                AUDIO_ERR_LOG("AudioStream::ReadCb Read: Illegal  state:%{public}u", state_);
                isReadyToRead_ = false;
                return;
            }

            stream.buffer = freeBufferQ_.front().buffer;
            stream.bufferLen = freeBufferQ_.front().bufLength;
            AUDIO_DEBUG_LOG("AudioStream::ReadCb requested stream.bufferLen:%{public}d", stream.bufferLen);

            if (stream.buffer == nullptr) {
                AUDIO_ERR_LOG("AudioStream::ReadCb stream.buffer == nullptr return");
                return;
            }
            readLen = ReadStream(stream, isBlockingRead);
            if (readLen < 0) {
                AUDIO_ERR_LOG("AudioStream::ReadCb ReadStream fail, ret: %{public}d", readLen);
            } else {
                AUDIO_DEBUG_LOG("AudioStream::ReadCb ReadStream, bytesRead:%{public}d", readLen);
                freeBufferQ_.front().dataLength = readLen;
                filledBufferQ_.emplace(freeBufferQ_.front());
                freeBufferQ_.pop();
            }
        }
        std::this_thread::sleep_for(std::chrono::microseconds(CB_READ_BUFFERS_WAIT_IN_US));
    }

    AUDIO_INFO_LOG("AudioStream::ReadCb thread end");
}

int32_t AudioStream::SetLowPowerVolume(float volume)
{
    return SetStreamLowPowerVolume(volume);
}

float AudioStream::GetLowPowerVolume()
{
    return GetStreamLowPowerVolume();
}

float AudioStream::GetSingleStreamVolume()
{
    return GetSingleStreamVol();
}

void AudioStream::SubmitAllFreeBuffers()
{
    lock_guard<mutex> lock(bufferQueueLock_);
    for (size_t i = 0; i < freeBufferQ_.size(); ++i) {
        SendWriteBufferRequestEvent();
    }
}
} // namespace AudioStandard
} // namespace OHOS
