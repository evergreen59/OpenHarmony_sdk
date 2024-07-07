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

#include "player_server_hi.h"
#include <map>
#include "media_log.h"
#include "media_errors.h"
#include "engine_factory_repo.h"
#include "media_dfx.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "PlayerServerHi"};
    enum PlayerTaskID : int32_t {
        PREPARE_TASK_ID = 0,
        PLAY_TASK_ID = 1,
        PAUSE_TASK_ID = 2,
        STOP_TASK_ID = 3,
        SEEK_TASK_ID = 4,
    };
}

namespace OHOS {
namespace Media {
const std::string START_TAG = "PlayerCreate->Start";
const std::string STOP_TAG = "PlayerStop->Destroy";
static const std::unordered_map<int32_t, std::string> STATUS_TO_STATUS_DESCRIPTION_TABLE = {
    {PLAYER_STATE_ERROR, "PLAYER_STATE_ERROR"},
    {PLAYER_IDLE, "PLAYER_IDLE"},
    {PLAYER_INITIALIZED, "PLAYER_INITIALIZED"},
    {PLAYER_PREPARING, "PLAYER_PREPARING"},
    {PLAYER_PREPARED, "PLAYER_PREPARED"},
    {PLAYER_STARTED, "PLAYER_STARTED"},
    {PLAYER_PAUSED, "PLAYER_PAUSED"},
    {PLAYER_STOPPED, "PLAYER_STOPPED"},
    {PLAYER_PLAYBACK_COMPLETE, "PLAYER_PLAYBACK_COMPLETE"},
};
std::shared_ptr<IPlayerService> PlayerServerHi::Create()
{
    std::shared_ptr<PlayerServerHi> server = std::make_shared<PlayerServerHi>();
    CHECK_AND_RETURN_RET_LOG(server != nullptr, nullptr, "failed to new PlayerServerHi");

    (void)server->Init();
    return server;
}

PlayerServerHi::PlayerServerHi()
    : startTimeMonitor_(START_TAG),
      stopTimeMonitor_(STOP_TAG)
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances create", FAKE_POINTER(this));
}

PlayerServerHi::~PlayerServerHi()
{
    (void)Release();
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances destroy", FAKE_POINTER(this));
}

void PlayerServerHi::ResetProcessor()
{
    resetRet_ = playerEngine_->Reset();
    playerEngine_ = nullptr;
#ifdef SUPPORT_VIDEO
    surface_ = nullptr;
#endif
}

int32_t PlayerServerHi::Init()
{
    MediaTrace trace("PlayerServerHi::Init");
    return MSERR_OK;
}

int32_t PlayerServerHi::SetSource(const std::string &url)
{
    std::lock_guard<std::mutex> lock(mutex_);
    MediaTrace trace("PlayerServerHi::SetSource");
    MEDIA_LOGW("KPI-TRACE: PlayerServerHi SetSource in(url)");
    config_.url = url;
    int32_t ret = InitPlayEngine(url);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "SetSource Failed!");
    return ret;
}

int32_t PlayerServerHi::SetSource(const std::shared_ptr<IMediaDataSource> &dataSrc)
{
    std::lock_guard<std::mutex> lock(mutex_);
    MediaTrace trace("PlayerServerHi::SetSource");
    CHECK_AND_RETURN_RET_LOG(dataSrc != nullptr, MSERR_INVALID_VAL, "data source is nullptr");
    MEDIA_LOGW("KPI-TRACE: PlayerServerHi SetSource in(dataSrc)");
    dataSrc_ = dataSrc;
    std::string url = "media data source";
    config_.url = url;
    int32_t ret = InitPlayEngine(url);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "SetObs Failed!");
    int64_t size = 0;
    (void)dataSrc_->GetSize(size);
    if (size == -1) {
        config_.looping = false;
        config_.speedMode = SPEED_FORWARD_1_00_X;
    }
    return ret;
}

int32_t PlayerServerHi::SetSource(int32_t fd, int64_t offset, int64_t size)
{
    std::lock_guard<std::mutex> lock(mutex_);
    MediaTrace trace("PlayerServerHi::SetSource");
    MEDIA_LOGW("KPI-TRACE: PlayerServerHi SetSource in(fd)");
    auto uriHelper = std::make_unique<UriHelper>(fd, offset, size);
    CHECK_AND_RETURN_RET_LOG(uriHelper->AccessCheck(UriHelper::URI_READ), MSERR_INVALID_VAL, "Failed to read the fd");
    int32_t ret = InitPlayEngine(uriHelper->FormattedUri());
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "SetSource Failed!");
    uriHelper_ = std::move(uriHelper);
    config_.url = "file descriptor source";
    return ret;
}

int32_t PlayerServerHi::InitPlayEngine(const std::string &url)
{
    if (status_ != PLAYER_IDLE) {
        MEDIA_LOGE("current state is: %{public}s, not support SetSource", GetStatusDescription(status_).c_str());
        return MSERR_INVALID_OPERATION;
    }
    startTimeMonitor_.StartTime();
    MEDIA_LOGD("current url is : %{public}s", url.c_str());
    auto engineFactory = EngineFactoryRepo::Instance().GetEngineFactory(IEngineFactory::Scene::SCENE_PLAYBACK, url);
    CHECK_AND_RETURN_RET_LOG(engineFactory != nullptr, MSERR_CREATE_PLAYER_ENGINE_FAILED,
        "failed to get engine factory");
    playerEngine_ = engineFactory->CreatePlayerEngine();
    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_CREATE_PLAYER_ENGINE_FAILED,
        "failed to create player engine");
    int32_t ret = MSERR_OK;
    if (dataSrc_ == nullptr) {
        ret = playerEngine_->SetSource(url);
    } else {
        ret = playerEngine_->SetSource(dataSrc_);
    }
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "SetSource Failed!");

    std::shared_ptr<IPlayerEngineObs> obs = shared_from_this();
    ret = playerEngine_->SetObs(obs);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "SetObs Failed!");

    status_ = PLAYER_INITIALIZED;
    return MSERR_OK;
}

int32_t PlayerServerHi::Prepare()
{
    MEDIA_LOGW("KPI-TRACE: PlayerServerHi Prepare in");
    MediaTrace trace("PlayerServerHi::Prepare");
    return OnPrepare(false);
}

int32_t PlayerServerHi::OnPrepare(bool async)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (status_ != PLAYER_INITIALIZED && status_ != PLAYER_STOPPED && status_ != PLAYER_PREPARED) {
        MEDIA_LOGE("Can not Prepare, currentState is %{public}s", GetStatusDescription(status_).c_str());
        return MSERR_INVALID_OPERATION;
    }

    if (status_ == PLAYER_PREPARED) {
        Format format;
        OnInfo(INFO_TYPE_STATE_CHANGE, status_, format);
        return MSERR_OK;
    }

    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");
    int32_t ret = MSERR_OK;
#ifdef SUPPORT_VIDEO
    if (surface_ != nullptr) {
        ret = playerEngine_->SetVideoSurface(surface_);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine SetVideoSurface Failed!");
    }
#endif

    status_ = PLAYER_PREPARING;
    if (async) {
        ret = playerEngine_->PrepareAsync();
    } else {
        ret = playerEngine_->Prepare();
    }
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine Prepare Failed!");
    (void)playerEngine_->SetVolume(config_.leftVolume, config_.rightVolume);
    (void)playerEngine_->SetLooping(config_.looping);
    if (config_.speedMode != SPEED_FORWARD_1_00_X) {
        (void)playerEngine_->SetPlaybackSpeed(config_.speedMode);
    }
    return MSERR_OK;
}

int32_t PlayerServerHi::Play()
{
    std::lock_guard<std::mutex> lock(mutex_);
    MediaTrace::TraceBegin("PlayerServerHi::Play", PLAY_TASK_ID);
    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");
    MEDIA_LOGW("KPI-TRACE: PlayerServerHi Play in");
    if (status_ != PLAYER_PREPARED && status_ != PLAYER_PLAYBACK_COMPLETE &&
        status_ != PLAYER_PAUSED && status_ != PLAYER_STARTED) {
        MEDIA_LOGE("Can not Play, currentState is %{public}s", GetStatusDescription(status_).c_str());
        return MSERR_INVALID_OPERATION;
    }

    if (status_ == PLAYER_PLAYBACK_COMPLETE && dataSrc_ != nullptr) {
        int64_t size = 0;
        (void)dataSrc_->GetSize(size);
        if (size == -1) {
            MEDIA_LOGE("Can not play in complete status, it is live-stream");
            return MSERR_INVALID_OPERATION;
        }
    }

    if (status_ == PLAYER_STARTED) {
        Format format;
        OnInfo(INFO_TYPE_STATE_CHANGE, status_, format);
        return MSERR_OK;
    }

    int32_t ret = playerEngine_->Play();
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine Play Failed!");

    startTimeMonitor_.FinishTime();
    status_ = PLAYER_STARTED;
    return MSERR_OK;
}

int32_t PlayerServerHi::PrepareAsync()
{
    MEDIA_LOGW("KPI-TRACE: PlayerServerHi PrepareAsync in");
    MediaTrace::TraceBegin("PlayerServerHi::PrepareAsync", PREPARE_TASK_ID);
    return OnPrepare(true);
}

int32_t PlayerServerHi::Pause()
{
    std::lock_guard<std::mutex> lock(mutex_);
    MediaTrace::TraceBegin("PlayerServerHi::Pause", PAUSE_TASK_ID);
    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");

    if (status_ == PLAYER_STATE_ERROR) {
        MEDIA_LOGE("Can not Pause, currentState is PLAYER_STATE_ERROR");
        return MSERR_INVALID_OPERATION;
    }

    if (status_ == PLAYER_PAUSED) {
        Format format;
        OnInfo(INFO_TYPE_STATE_CHANGE, status_, format);
        return MSERR_OK;
    }

    if (status_ != PLAYER_STARTED) {
        MEDIA_LOGE("Can not Pause, status_ is %{public}s", GetStatusDescription(status_).c_str());
        return MSERR_INVALID_OPERATION;
    }

    int32_t ret = playerEngine_->Pause();
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine Pause Failed!");
    status_ = PLAYER_PAUSED;
    return MSERR_OK;
}

int32_t PlayerServerHi::Stop()
{
    std::lock_guard<std::mutex> lock(mutex_);
    MediaTrace::TraceBegin("PlayerServerHi::Stop", STOP_TASK_ID);
    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");
    MEDIA_LOGW("KPI-TRACE: PlayerServerHi Stop in");
    if (status_ == PLAYER_STATE_ERROR) {
        MEDIA_LOGE("Can not Stop, currentState is PLAYER_STATE_ERROR");
        return MSERR_INVALID_OPERATION;
    }

    if (status_ == PLAYER_STOPPED) {
        Format format;
        OnInfo(INFO_TYPE_STATE_CHANGE, status_, format);
        return MSERR_OK;
    }

    if ((status_ != PLAYER_PREPARED) && (status_ != PLAYER_STARTED) &&
        (status_ != PLAYER_PLAYBACK_COMPLETE) && (status_ != PLAYER_PAUSED)) {
        MEDIA_LOGE("current state: %{public}s, can not stop", GetStatusDescription(status_).c_str());
        return MSERR_INVALID_OPERATION;
    }

    stopTimeMonitor_.StartTime();

    int32_t ret = playerEngine_->Stop();
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine Stop Failed!");
    status_ = PLAYER_STOPPED;
    return MSERR_OK;
}

int32_t PlayerServerHi::Reset()
{
    std::lock_guard<std::mutex> lock(mutex_);
    MEDIA_LOGW("KPI-TRACE: PlayerServerHi Reset in");
    MediaTrace trace("PlayerServerHi::Reset");
    return OnReset();
}

int32_t PlayerServerHi::OnReset()
{
    if (status_ == PLAYER_IDLE) {
        Format format;
        OnInfo(INFO_TYPE_STATE_CHANGE, status_, format);
        return MSERR_OK;
    }

    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");
    std::unique_ptr<std::thread> thread = std::make_unique<std::thread>(&PlayerServerHi::ResetProcessor, this);
    if (thread != nullptr && thread->joinable()) {
        thread->join();
    }
    CHECK_AND_RETURN_RET_LOG(resetRet_ == MSERR_OK, MSERR_INVALID_OPERATION, "Engine Reset Failed!");
    dataSrc_ = nullptr;
    config_.looping = false;
    uriHelper_ = nullptr;
    lastErrMsg_.clear();
    Format format;
    OnInfo(INFO_TYPE_STATE_CHANGE, PLAYER_IDLE, format);
    stopTimeMonitor_.FinishTime();
    return MSERR_OK;
}

int32_t PlayerServerHi::Release()
{
    std::lock_guard<std::mutex> lock(mutex_);
    {
        std::lock_guard<std::mutex> lockCb(mutexCb_);
        playerCb_ = nullptr;
    }
    (void)OnReset();
    return MSERR_OK;
}

int32_t PlayerServerHi::ReleaseSync()
{
    return MSERR_OK;
}

int32_t PlayerServerHi::SetVolume(float leftVolume, float rightVolume)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (status_ == PLAYER_STATE_ERROR) {
        MEDIA_LOGE("Can not SetVolume, currentState is PLAYER_STATE_ERROR");
        return MSERR_INVALID_OPERATION;
    }

    constexpr float maxVolume = 1.0f;
    if ((leftVolume < 0) || (leftVolume > maxVolume) || (rightVolume < 0) || (rightVolume > maxVolume)) {
        MEDIA_LOGE("SetVolume failed, the volume should be set to a value ranging from 0 to 5");
        return MSERR_INVALID_OPERATION;
    }

    config_.leftVolume = leftVolume;
    config_.rightVolume = rightVolume;
    if (status_ == PLAYER_IDLE || status_ == PLAYER_INITIALIZED || status_ == PLAYER_STOPPED) {
        MEDIA_LOGI("Waiting for the engine state is <prepared> to take effect");
        Format format;
        OnInfo(INFO_TYPE_VOLUME_CHANGE, 0, format);
        return MSERR_OK;
    }

    if (playerEngine_ != nullptr) {
        int32_t ret = playerEngine_->SetVolume(config_.leftVolume, config_.rightVolume);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "SetVolume Failed!");
    }
    return MSERR_OK;
}

bool PlayerServerHi::IsValidSeekMode(PlayerSeekMode mode)
{
    switch (mode) {
        case SEEK_PREVIOUS_SYNC:
        case SEEK_NEXT_SYNC:
        case SEEK_CLOSEST_SYNC:
        case SEEK_CLOSEST:
            break;
        default:
            MEDIA_LOGE("Unknown seek mode %{public}d", mode);
            return false;
    }
    return true;
}

int32_t PlayerServerHi::Seek(int32_t mSeconds, PlayerSeekMode mode)
{
    std::lock_guard<std::mutex> lock(mutex_);
    MediaTrace::TraceBegin("Player::Seek", SEEK_TASK_ID);
    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");

    if (status_ != PLAYER_PREPARED && status_ != PLAYER_PAUSED &&
        status_ != PLAYER_STARTED && status_ != PLAYER_PLAYBACK_COMPLETE) {
        MEDIA_LOGE("Can not Seek, currentState is %{public}s", GetStatusDescription(status_).c_str());
        return MSERR_INVALID_OPERATION;
    }

    if (IsValidSeekMode(mode) != true) {
        MEDIA_LOGE("Seek failed, inValid mode");
        return MSERR_INVALID_VAL;
    }

    MEDIA_LOGD("seek position %{public}d, seek mode is %{public}d", mSeconds, mode);
    mSeconds = std::max(0, mSeconds);
    int32_t ret = playerEngine_->Seek(mSeconds, mode);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine Seek Failed!");
    return ret;
}

int32_t PlayerServerHi::GetCurrentTime(int32_t &currentTime)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (status_ == PLAYER_STATE_ERROR) {
        MEDIA_LOGE("Can not GetCurrentTime, currentState is PLAYER_STATE_ERROR");
        return MSERR_INVALID_OPERATION;
    }

    MEDIA_LOGI("PlayerServerHi::GetCurrentTime");
    currentTime = 0;
    if (playerEngine_ != nullptr) {
        int32_t ret = playerEngine_->GetCurrentTime(currentTime);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine GetCurrentTime Failed!");
    }
    return MSERR_OK;
}

int32_t PlayerServerHi::GetVideoTrackInfo(std::vector<Format> &videoTrack)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");

    if (status_ != PLAYER_PREPARED && status_ != PLAYER_PAUSED &&
        status_ != PLAYER_STARTED && status_ != PLAYER_STOPPED &&
        status_ != PLAYER_PLAYBACK_COMPLETE) {
        MEDIA_LOGE("Can not get track info, currentState is %{public}s", GetStatusDescription(status_).c_str());
        return MSERR_INVALID_OPERATION;
    }

    int32_t ret = playerEngine_->GetVideoTrackInfo(videoTrack);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine GetVideoTrackInfo Failed!");
    return MSERR_OK;
}

int32_t PlayerServerHi::GetAudioTrackInfo(std::vector<Format> &audioTrack)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");

    if (status_ != PLAYER_PREPARED && status_ != PLAYER_PAUSED &&
        status_ != PLAYER_STARTED && status_ != PLAYER_STOPPED &&
        status_ != PLAYER_PLAYBACK_COMPLETE) {
        MEDIA_LOGE("Can not get track info, currentState is %{public}s", GetStatusDescription(status_).c_str());
        return MSERR_INVALID_OPERATION;
    }

    int32_t ret = playerEngine_->GetAudioTrackInfo(audioTrack);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine GetAudioTrackInfo Failed!");
    return MSERR_OK;
}

int32_t PlayerServerHi::GetVideoWidth()
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");

    if (status_ != PLAYER_PREPARED && status_ != PLAYER_PAUSED &&
        status_ != PLAYER_STARTED && status_ != PLAYER_STOPPED &&
        status_ != PLAYER_PLAYBACK_COMPLETE) {
        MEDIA_LOGE("Can not get track info, currentState is %{public}s", GetStatusDescription(status_).c_str());
        return MSERR_INVALID_OPERATION;
    }

    return playerEngine_->GetVideoWidth();
}

int32_t PlayerServerHi::GetVideoHeight()
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");

    if (status_ != PLAYER_PREPARED && status_ != PLAYER_PAUSED &&
        status_ != PLAYER_STARTED && status_ != PLAYER_STOPPED &&
        status_ != PLAYER_PLAYBACK_COMPLETE) {
        MEDIA_LOGE("Can not get track info, currentState is %{public}s", GetStatusDescription(status_).c_str());
        return MSERR_INVALID_OPERATION;
    }

    return playerEngine_->GetVideoHeight();
}

int32_t PlayerServerHi::GetDuration(int32_t &duration)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (status_ == PLAYER_IDLE || status_ == PLAYER_INITIALIZED || status_ == PLAYER_STATE_ERROR) {
        MEDIA_LOGE("Can not GetDuration, currentState is %{public}s", GetStatusDescription(status_).c_str());
        return MSERR_INVALID_OPERATION;
    }
    duration = 0;
    if (playerEngine_ != nullptr) {
        int ret = playerEngine_->GetDuration(duration);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine GetDuration Failed!");
    }
    return MSERR_OK;
}

int32_t PlayerServerHi::SetPlaybackSpeed(PlaybackRateMode mode)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if ((status_ != PLAYER_STARTED) && (status_ != PLAYER_PREPARED) &&
        (status_ != PLAYER_PAUSED) && (status_ != PLAYER_PLAYBACK_COMPLETE)) {
        MEDIA_LOGE("Can not SetPlaybackSpeed, currentState is %{public}s", GetStatusDescription(status_).c_str());
        return MSERR_INVALID_OPERATION;
    }

    if (dataSrc_ != nullptr) {
        int64_t size = 0;
        (void)dataSrc_->GetSize(size);
        if (size == -1) {
            MEDIA_LOGE("Can not SetPlaybackSpeed, it is live-stream");
            return MSERR_INVALID_OPERATION;
        }
    }

    if (config_.speedMode == mode) {
        MEDIA_LOGD("The speed mode is same, mode = %{public}d", mode);
        Format format;
        OnInfo(INFO_TYPE_SPEEDDONE, 0, format);
        return MSERR_OK;
    }

    if (playerEngine_ != nullptr) {
        int ret = playerEngine_->SetPlaybackSpeed(mode);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine SetPlaybackSpeed Failed!");
    }
    config_.speedMode = mode;
    return MSERR_OK;
}

int32_t PlayerServerHi::GetPlaybackSpeed(PlaybackRateMode &mode)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (status_ == PLAYER_STATE_ERROR) {
        MEDIA_LOGE("Can not GetDuration, currentState is PLAYER_STATE_ERROR");
        return MSERR_INVALID_OPERATION;
    }

    mode = config_.speedMode;
    return MSERR_OK;
}

int32_t PlayerServerHi::SelectBitRate(uint32_t bitRate)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (playerEngine_ != nullptr) {
        int ret = playerEngine_->SelectBitRate(bitRate);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine SelectBitRate Failed!");
    }
    return MSERR_OK;
}

#ifdef SUPPORT_VIDEO
int32_t PlayerServerHi::SetVideoSurface(sptr<Surface> surface)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(surface != nullptr, MSERR_INVALID_VAL, "surface is nullptr");

    if (status_ != PLAYER_INITIALIZED) {
        MEDIA_LOGE("current state: %{public}s, can not SetVideoSurface", GetStatusDescription(status_).c_str());
        return MSERR_INVALID_OPERATION;
    }

    surface_ = surface;
    return MSERR_OK;
}
#endif

bool PlayerServerHi::IsPlaying()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (status_ == PLAYER_STATE_ERROR) {
        MEDIA_LOGE("Can not judge IsPlaying, currentState is PLAYER_STATE_ERROR");
        return false;
    }

    return status_ == PLAYER_STARTED;
}

bool PlayerServerHi::IsLooping()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (status_ == PLAYER_STATE_ERROR) {
        MEDIA_LOGE("Can not judge IsLooping, currentState is PLAYER_STATE_ERROR");
        return false;
    }

    return config_.looping;
}

int32_t PlayerServerHi::SetLooping(bool loop)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (status_ == PLAYER_STATE_ERROR) {
        MEDIA_LOGE("Can not SetLooping, currentState is PLAYER_STATE_ERROR");
        return MSERR_INVALID_OPERATION;
    }

    if (dataSrc_ != nullptr) {
        int64_t size = 0;
        (void)dataSrc_->GetSize(size);
        if (size == -1) {
            MEDIA_LOGE("Can not SetLooping, it is live-stream");
            return MSERR_INVALID_OPERATION;
        }
    }

    if (status_ == PLAYER_IDLE || status_ == PLAYER_INITIALIZED) {
        MEDIA_LOGI("Waiting for the engine state is <prepared> to take effect");
        config_.looping = loop;
        return MSERR_OK;
    }

    if (playerEngine_ != nullptr) {
        int32_t ret = playerEngine_->SetLooping(loop);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "SetLooping Failed!");
    }
    config_.looping = loop;
    return MSERR_OK;
}

int32_t PlayerServerHi::SetParameter(const Format &param)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (status_ == PLAYER_STATE_ERROR) {
        MEDIA_LOGE("Can not SetParameter, currentState is PLAYER_STATE_ERROR");
        return MSERR_INVALID_OPERATION;
    }

    if (playerEngine_ != nullptr) {
        int32_t ret = playerEngine_->SetParameter(param);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "SetParameter Failed!");
    }

    return MSERR_OK;
}

int32_t PlayerServerHi::SetPlayerCallback(const std::shared_ptr<PlayerCallback> &callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(callback != nullptr, MSERR_INVALID_VAL, "callback is nullptr");

    if (status_ != PLAYER_IDLE && status_ != PLAYER_INITIALIZED) {
        MEDIA_LOGE("Can not SetPlayerCallback, currentState is %{public}s", GetStatusDescription(status_).c_str());
        return MSERR_INVALID_OPERATION;
    }

    {
        std::lock_guard<std::mutex> lockCb(mutexCb_);
        playerCb_ = callback;
    }
    return MSERR_OK;
}

void PlayerServerHi::FormatToString(std::string &dumpString, std::vector<Format> &videoTrack)
{
    for (auto iter = videoTrack.begin(); iter != videoTrack.end(); iter++) {
        dumpString += iter->Stringify();
    }
}

int32_t PlayerServerHi::DumpInfo(int32_t fd)
{
    std::string dumpString;
    if (playerEngine_ == nullptr) {
        dumpString +=
            "The engine is not created, note: engine can't be created until set source.\n";
            write(fd, dumpString.c_str(), dumpString.size());
        return MSERR_OK;
    }
    dumpString += "PlayerServerHi current state is: " + std::to_string(status_) + "\n";
    if (lastErrMsg_.size() != 0) {
        dumpString += "PlayerServerHi last error is: " + lastErrMsg_ + "\n";
    }
    dumpString += "PlayerServerHi url is: " + config_.url + "\n";
    dumpString += "PlayerServerHi play back speed is: " + std::to_string(config_.speedMode) + "\n";
    std::string loopflag = config_.looping ? "" : "not ";
    dumpString += "PlayerServerHi current " + loopflag + "in looping mode\n";
    dumpString += "PlayerServerHi left volume and right volume is: " +
        std::to_string(config_.leftVolume) + ", " + std::to_string(config_.rightVolume) + "\n";

    std::vector<Format> videoTrack;
    CHECK_AND_RETURN_RET(GetVideoTrackInfo(videoTrack) == MSERR_OK, MSERR_INVALID_OPERATION);
    dumpString += "PlayerServerHi video tracks info: \n";
    FormatToString(dumpString, videoTrack);
    
    std::vector<Format> audioTrack;
    CHECK_AND_RETURN_RET(GetAudioTrackInfo(audioTrack) == MSERR_OK, MSERR_INVALID_OPERATION);
    dumpString += "PlayerServerHi audio tracks info: \n";
    FormatToString(dumpString, audioTrack);
    
    int32_t currentTime;
    CHECK_AND_RETURN_RET(GetCurrentTime(currentTime) == MSERR_OK, MSERR_INVALID_OPERATION);
    dumpString += "PlayerServerHi current time is: " + std::to_string(currentTime) + "\n";
    write(fd, dumpString.c_str(), dumpString.size());

    return MSERR_OK;
}

void PlayerServerHi::OnError(PlayerErrorType errorType, int32_t errorCode)
{
    (void)errorType;
    auto errorMsg = MSErrorToExtErrorString(static_cast<MediaServiceErrCode>(errorCode));
    return OnErrorMessage(errorCode, errorMsg);
}

void PlayerServerHi::OnErrorMessage(int32_t errorCode, const std::string &errorMsg)
{
    std::lock_guard<std::mutex> lockCb(mutexCb_);
    lastErrMsg_ = errorMsg;
    FaultEventWrite(lastErrMsg_, "Player");
    if (playerCb_ != nullptr) {
        playerCb_->OnError(errorCode, errorMsg);
    }
}

void PlayerServerHi::OnInfo(PlayerOnInfoType type, int32_t extra, const Format &infoBody)
{
    std::lock_guard<std::mutex> lockCb(mutexCb_);

    if (type == INFO_TYPE_STATE_CHANGE) {
        status_ = static_cast<PlayerStates>(extra);
        MEDIA_LOGI("Callback State change, currentState is %{public}d", status_);
        if (status_ == PLAYER_PREPARED) {
            MediaTrace::TraceEnd("PlayerServerHi::PrepareAsync", PREPARE_TASK_ID);
        } else if (status_ == PLAYER_STARTED) {
            MediaTrace::TraceEnd("PlayerServerHi::Play", PLAY_TASK_ID);
        } else if (status_ == PLAYER_PAUSED) {
            MediaTrace::TraceEnd("PlayerServerHi::Pause", PAUSE_TASK_ID);
        } else if (status_ == PLAYER_STOPPED) {
            MediaTrace::TraceEnd("PlayerServerHi::Stop", STOP_TASK_ID);
        }
        BehaviorEventWrite(GetStatusDescription(status_).c_str(), "Player");
        MEDIA_LOGI("Callback State change, currentState is %{public}s", GetStatusDescription(status_).c_str());
    } else if (type == INFO_TYPE_SEEKDONE) {
        MediaTrace::TraceEnd("Player::Seek", SEEK_TASK_ID);
    }

    if (playerCb_ != nullptr) {
        playerCb_->OnInfo(type, extra, infoBody);
    }
}

const std::string &PlayerServerHi::GetStatusDescription(int32_t status)
{
    static const std::string ILLEGAL_STATE = "PLAYER_STATUS_ILLEGAL";
    if (status < PLAYER_STATE_ERROR || status > PLAYER_PLAYBACK_COMPLETE) {
        return ILLEGAL_STATE;
    }

    return STATUS_TO_STATUS_DESCRIPTION_TABLE.find(status)->second;
}
} // namespace Media
} // namespace OHOS
