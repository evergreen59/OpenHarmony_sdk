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

#include "player_engine_gst_impl.h"

#include <unistd.h>
#include "media_log.h"
#include "media_errors.h"
#include "directory_ex.h"
#include "audio_system_manager.h"
#include "player_sinkprovider.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "PlayerEngineGstImpl"};
}

namespace OHOS {
namespace Media {
constexpr float EPSINON = 0.0001;
constexpr float SPEED_0_75_X = 0.75;
constexpr float SPEED_1_00_X = 1.00;
constexpr float SPEED_1_25_X = 1.25;
constexpr float SPEED_1_75_X = 1.75;
constexpr float SPEED_2_00_X = 2.00;
constexpr size_t MAX_URI_SIZE = 4096;
constexpr int32_t MSEC_PER_USEC = 1000;
constexpr int32_t MSEC_PER_NSEC = 1000000;
constexpr int32_t BUFFER_TIME_DEFAULT = 15000; // 15s
constexpr uint32_t INTERRUPT_EVENT_SHIFT = 8;
constexpr int32_t POSITION_REPORT_PER_TIMES = 10;

PlayerEngineGstImpl::PlayerEngineGstImpl(int32_t uid, int32_t pid)
    : appuid_(uid), apppid_(pid)
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances create", FAKE_POINTER(this));
}

PlayerEngineGstImpl::~PlayerEngineGstImpl()
{
    (void)OnReset();
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances destroy", FAKE_POINTER(this));
}

bool PlayerEngineGstImpl::IsFileUrl(const std::string &url) const
{
    return url.find("://") == std::string::npos || url.find("file://") == 0;
}

int32_t PlayerEngineGstImpl::GetRealPath(const std::string &url, std::string &realUrlPath) const
{
    std::string fileHead = "file://";
    std::string tempUrlPath;

    if (url.find(fileHead) == 0 && url.size() > fileHead.size()) {
        tempUrlPath = url.substr(fileHead.size());
    } else {
        tempUrlPath = url;
    }

    bool ret = PathToRealPath(tempUrlPath, realUrlPath);
    CHECK_AND_RETURN_RET_LOG(ret, MSERR_OPEN_FILE_FAILED,
        "invalid url. The Url (%{public}s) path may be invalid.", url.c_str());

    if (access(realUrlPath.c_str(), R_OK) != 0) {
        return MSERR_FILE_ACCESS_FAILED;
    }

    return MSERR_OK;
}

int32_t PlayerEngineGstImpl::SetSource(const std::string &url)
{
    std::unique_lock<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(!url.empty(), MSERR_INVALID_VAL, "input url is empty!");
    CHECK_AND_RETURN_RET_LOG(url.length() <= MAX_URI_SIZE, MSERR_INVALID_VAL, "input url length is invalid!");

    int32_t ret = MSERR_OK;
    if (IsFileUrl(url)) {
        std::string realUriPath;
        ret = GetRealPath(url, realUriPath);
        if (ret != MSERR_OK) {
            return ret;
        }
        url_ = "file://" + realUriPath;
    } else {
        url_ = url;
    }

    MEDIA_LOGD("set player source: %{public}s", url_.c_str());
    return ret;
}

int32_t PlayerEngineGstImpl::SetSource(const std::shared_ptr<IMediaDataSource> &dataSrc)
{
    std::unique_lock<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(dataSrc != nullptr, MSERR_INVALID_VAL, "input dataSrc is empty!");
    appsrcWrap_ = GstAppsrcWrap::Create(dataSrc);
    CHECK_AND_RETURN_RET_LOG(appsrcWrap_ != nullptr, MSERR_NO_MEMORY, "new appsrcwrap failed!");
    return MSERR_OK;
}

int32_t PlayerEngineGstImpl::SetObs(const std::weak_ptr<IPlayerEngineObs> &obs)
{
    std::unique_lock<std::mutex> lock(mutex_);
    obs_ = obs;
    return MSERR_OK;
}

int32_t PlayerEngineGstImpl::SetVideoSurface(sptr<Surface> surface)
{
    std::unique_lock<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(surface != nullptr, MSERR_INVALID_VAL, "surface is nullptr");

    producerSurface_ = surface;
    return MSERR_OK;
}

int32_t PlayerEngineGstImpl::Prepare()
{
    std::unique_lock<std::mutex> lock(mutex_);
    MEDIA_LOGD("Prepare in");

    int32_t ret = PlayBinCtrlerInit();
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_VAL, "PlayBinCtrlerInit failed");

    CHECK_AND_RETURN_RET_LOG(playBinCtrler_ != nullptr, MSERR_INVALID_VAL, "playBinCtrler_ is nullptr");
    ret = playBinCtrler_->Prepare();
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, ret, "Prepare failed");

    // The duration of some resources without header information cannot be obtained.
    MEDIA_LOGD("Prepared ok out");
    return MSERR_OK;
}

int32_t PlayerEngineGstImpl::PrepareAsync()
{
    std::unique_lock<std::mutex> lock(mutex_);
    MEDIA_LOGD("Prepare in");

    int32_t ret = PlayBinCtrlerInit();
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_VAL, "PlayBinCtrlerInit failed");

    CHECK_AND_RETURN_RET_LOG(playBinCtrler_ != nullptr, MSERR_INVALID_VAL, "playBinCtrler_ is nullptr");
    ret = playBinCtrler_->PrepareAsync();
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, ret, "PrepareAsync failed");

    // The duration of some resources without header information cannot be obtained.
    MEDIA_LOGD("Prepared ok out");
    return MSERR_OK;
}

void PlayerEngineGstImpl::HandleErrorMessage(const PlayBinMessage &msg)
{
    MEDIA_LOGE("error happended, cancel inprocessing job");

    int32_t errorCode = msg.code;
    std::string errorMsg = "Unknown Error";
    if (msg.subType == PlayBinMsgErrorSubType::PLAYBIN_SUB_MSG_ERROR_WITH_MESSAGE) {
        errorMsg = std::any_cast<std::string>(msg.extra);
    }

    std::shared_ptr<IPlayerEngineObs> notifyObs = obs_.lock();
    Format format;
    if (notifyObs != nullptr) {
        notifyObs->OnErrorMessage(errorCode, errorMsg);
        notifyObs->OnInfo(INFO_TYPE_STATE_CHANGE, PLAYER_STATE_ERROR, format);
    }
}

void PlayerEngineGstImpl::HandleInfoMessage(const PlayBinMessage &msg)
{
    MEDIA_LOGI("info msg type:%{public}d, value:%{public}d", msg.type, msg.code);

    int32_t status = msg.code;
    Format format;
    std::shared_ptr<IPlayerEngineObs> notifyObs = obs_.lock();
    if (notifyObs != nullptr) {
        notifyObs->OnInfo(static_cast<PlayerOnInfoType>(msg.type), status, format);
    }
}

void PlayerEngineGstImpl::HandleSeekDoneMessage(const PlayBinMessage &msg)
{
    MEDIA_LOGI("seek done, seek position = %{public}dms", msg.code);

    codecCtrl_.EnhanceSeekPerformance(false);

    int32_t status = msg.code;
    Format format;
    std::shared_ptr<IPlayerEngineObs> notifyObs = obs_.lock();
    if (notifyObs != nullptr) {
        notifyObs->OnInfo(INFO_TYPE_SEEKDONE, status, format);
    }
}

void PlayerEngineGstImpl::HandleSpeedDoneMessage(const PlayBinMessage &msg)
{
    std::shared_ptr<IPlayerEngineObs> notifyObs = obs_.lock();
    if (notifyObs != nullptr) {
        Format format;
        double rate = std::any_cast<double>(msg.extra);
        PlaybackRateMode speedMode = ChangeSpeedToMode(rate);
        notifyObs->OnInfo(INFO_TYPE_SPEEDDONE, speedMode, format);
    }
}

void PlayerEngineGstImpl::HandleBufferingStart()
{
    percent_ = 0;
    Format format;
    (void)format.PutIntValue(std::string(PlayerKeys::PLAYER_BUFFERING_START), 0);
    std::shared_ptr<IPlayerEngineObs> notifyObs = obs_.lock();
    if (notifyObs != nullptr) {
        notifyObs->OnInfo(INFO_TYPE_BUFFERING_UPDATE, 0, format);
    }
}

void PlayerEngineGstImpl::HandleBufferingEnd()
{
    Format format;
    (void)format.PutIntValue(std::string(PlayerKeys::PLAYER_BUFFERING_END), 0);
    std::shared_ptr<IPlayerEngineObs> notifyObs = obs_.lock();
    if (notifyObs != nullptr) {
        notifyObs->OnInfo(INFO_TYPE_BUFFERING_UPDATE, 0, format);
    }
}

void PlayerEngineGstImpl::HandleBufferingTime(const PlayBinMessage &msg)
{
    std::pair<uint32_t, int64_t> bufferingTimePair = std::any_cast<std::pair<uint32_t, int64_t>>(msg.extra);
    uint32_t mqNumId = bufferingTimePair.first;
    uint64_t bufferingTime = bufferingTimePair.second / MSEC_PER_NSEC;

    if (bufferingTime > BUFFER_TIME_DEFAULT) {
        bufferingTime = BUFFER_TIME_DEFAULT;
    }

    mqBufferingTime_[mqNumId] = bufferingTime;

    MEDIA_LOGD("ProcessBufferingTime(%{public}" PRIu64 " ms), mqNumId = %{public}u, "
        "mqNumUsedBuffering_ = %{public}u ms", bufferingTime, mqNumId, mqNumUsedBuffering_);

    if (mqBufferingTime_.size() != mqNumUsedBuffering_) {
        return;
    }

    uint64_t mqBufferingTime = BUFFER_TIME_DEFAULT;
    for (auto iter = mqBufferingTime_.begin(); iter != mqBufferingTime_.end(); ++iter) {
        if (iter->second < mqBufferingTime) {
            mqBufferingTime = iter->second;
        }
    }

    if (bufferingTime_ != mqBufferingTime) {
        bufferingTime_ = mqBufferingTime;
        std::shared_ptr<IPlayerEngineObs> notifyObs = obs_.lock();
        if (notifyObs != nullptr) {
            Format format;
            (void)format.PutIntValue(std::string(PlayerKeys::PLAYER_CACHED_DURATION),
                                     static_cast<int32_t>(mqBufferingTime));
            notifyObs->OnInfo(INFO_TYPE_BUFFERING_UPDATE, 0, format);
        }
    }
}

void PlayerEngineGstImpl::HandleBufferingPercent(const PlayBinMessage &msg)
{
    int32_t lastPercent = percent_;
    percent_ = msg.code;

    if (lastPercent == percent_) {
        return;
    }

    std::shared_ptr<IPlayerEngineObs> tempObs = obs_.lock();
    if (tempObs != nullptr) {
        Format format;
        (void)format.PutIntValue(std::string(PlayerKeys::PLAYER_BUFFERING_PERCENT), percent_);
        MEDIA_LOGD("percent = (%{public}d), percent_ = %{public}d, 0x%{public}06" PRIXPTR "",
            lastPercent, percent_, FAKE_POINTER(this));
        tempObs->OnInfo(INFO_TYPE_BUFFERING_UPDATE, 0, format);
    }
}

void PlayerEngineGstImpl::HandleBufferingUsedMqNum(const PlayBinMessage &msg)
{
    mqNumUsedBuffering_ = std::any_cast<uint32_t>(msg.extra);
}

void PlayerEngineGstImpl::HandleVideoRenderingStart()
{
    Format format;
    MEDIA_LOGD("video rendering start");
    std::shared_ptr<IPlayerEngineObs> notifyObs = obs_.lock();
    if (notifyObs != nullptr) {
        notifyObs->OnInfo(INFO_TYPE_MESSAGE, PlayerMessageType::PLAYER_INFO_VIDEO_RENDERING_START, format);
    }
}

void PlayerEngineGstImpl::HandleVideoSizeChanged(const PlayBinMessage &msg)
{
    std::pair<int32_t, int32_t> resolution = std::any_cast<std::pair<int32_t, int32_t>>(msg.extra);
    Format format;
    (void)format.PutIntValue(std::string(PlayerKeys::PLAYER_WIDTH), resolution.first);
    (void)format.PutIntValue(std::string(PlayerKeys::PLAYER_HEIGHT), resolution.second);
    MEDIA_LOGD("video size changed, width = %{public}d, height = %{public}d", resolution.first, resolution.second);
    std::shared_ptr<IPlayerEngineObs> notifyObs = obs_.lock();
    if (notifyObs != nullptr) {
        notifyObs->OnInfo(INFO_TYPE_RESOLUTION_CHANGE, 0, format);
    }
    videoWidth_ = resolution.first;
    videoHeight_ = resolution.second;
}

void PlayerEngineGstImpl::HandleBitRateCollect(const PlayBinMessage &msg)
{
    std::shared_ptr<IPlayerEngineObs> notifyObs = obs_.lock();
    if (notifyObs != nullptr) {
        notifyObs->OnInfo(INFO_TYPE_BITRATE_COLLECT, 0, std::any_cast<Format>(msg.extra));
    }
}

void PlayerEngineGstImpl::HandleSubTypeMessage(const PlayBinMessage &msg)
{
    switch (msg.subType) {
        case PLAYBIN_SUB_MSG_BUFFERING_START: {
            HandleBufferingStart();
            break;
        }
        case PLAYBIN_SUB_MSG_BUFFERING_END: {
            HandleBufferingEnd();
            break;
        }
        case PLAYBIN_SUB_MSG_BUFFERING_TIME: {
            HandleBufferingTime(msg);
            break;
        }
        case PLAYBIN_SUB_MSG_BUFFERING_PERCENT: {
            HandleBufferingPercent(msg);
            break;
        }
        case PLAYBIN_SUB_MSG_BUFFERING_USED_MQ_NUM: {
            HandleBufferingUsedMqNum(msg);
            break;
        }
        case PLAYBIN_SUB_MSG_VIDEO_RENDERING_START: {
            HandleVideoRenderingStart();
            break;
        }
        case PLAYBIN_SUB_MSG_VIDEO_SIZE_CHANGED: {
            HandleVideoSizeChanged(msg);
            break;
        }
        case PLAYBIN_SUB_MSG_BITRATE_COLLECT: {
            HandleBitRateCollect(msg);
            break;
        }
        default: {
            break;
        }
    }
}

void PlayerEngineGstImpl::HandleAudioMessage(const PlayBinMessage &msg)
{
    switch (msg.subType) {
        case PLAYBIN_MSG_INTERRUPT_EVENT: {
            HandleInterruptMessage(msg);
            break;
        }
        case PLAYBIN_MSG_AUDIO_STATE_EVENT: {
            HandleAudioStateMessage(msg);
            break;
        }
        default: {
            break;
        }
    }
}

void PlayerEngineGstImpl::HandleAudioStateMessage(const PlayBinMessage &msg)
{
    int32_t value = std::any_cast<int32_t>(msg.extra);
    MEDIA_LOGI("HandleAudioStateMessage:%{public}d", value);

    if (value == AudioStandard::RendererState::RENDERER_PAUSED) {
        std::shared_ptr<IPlayerEngineObs> notifyObs = obs_.lock();
        Format format;
        if (notifyObs != nullptr) {
            notifyObs->OnInfo(INFO_TYPE_STATE_CHANGE_BY_AUDIO, PlayerStates::PLAYER_PAUSED, format);
        }
    }
}

void PlayerEngineGstImpl::HandleInterruptMessage(const PlayBinMessage &msg)
{
    MEDIA_LOGI("Audio interrupt event in");
    uint32_t value = std::any_cast<uint32_t>(msg.extra);
    std::shared_ptr<IPlayerEngineObs> notifyObs = obs_.lock();
    if (notifyObs != nullptr) {
        Format format;
        int32_t hintType = value & 0x000000FF;
        int32_t forceType = (value >> INTERRUPT_EVENT_SHIFT) & 0x000000FF;
        int32_t eventType = value >> (INTERRUPT_EVENT_SHIFT * 2);
        (void)format.PutIntValue(PlayerKeys::AUDIO_INTERRUPT_TYPE, eventType);
        (void)format.PutIntValue(PlayerKeys::AUDIO_INTERRUPT_FORCE, forceType);
        (void)format.PutIntValue(PlayerKeys::AUDIO_INTERRUPT_HINT, hintType);
        notifyObs->OnInfo(INFO_TYPE_INTERRUPT_EVENT, 0, format);
    }
}

void PlayerEngineGstImpl::HandlePositionUpdateMessage(const PlayBinMessage &msg)
{
    currentTime_ = msg.code;
    int32_t duration = std::any_cast<int32_t>(msg.extra);
    MEDIA_LOGD("update position %{public}d ms, duration %{public}d ms", currentTime_, duration);

    if (duration != duration_) {
        duration_ = duration;
        Format format;
        std::shared_ptr<IPlayerEngineObs> notifyObs = obs_.lock();
        if (notifyObs != nullptr) {
            notifyObs->OnInfo(INFO_TYPE_DURATION_UPDATE, duration_, format);
        }
    }

    // 10: report once at 1000ms
    if (currentTimeOnInfoCnt_ % POSITION_REPORT_PER_TIMES == 0 ||
        msg.subType == PLAYBIN_SUB_MSG_POSITION_UPDATE_FORCE) {
        Format format;
        std::shared_ptr<IPlayerEngineObs> notifyObs = obs_.lock();
        if (notifyObs != nullptr) {
            notifyObs->OnInfo(INFO_TYPE_POSITION_UPDATE, currentTime_, format);
        }
        if (currentTimeOnInfoCnt_ % POSITION_REPORT_PER_TIMES == 0) {
            currentTimeOnInfoCnt_ = 0;
        }
    }
    if (msg.subType == PLAYBIN_SUB_MSG_POSITION_UPDATE_UNFORCE) {
        currentTimeOnInfoCnt_++;
    }
}

using MsgNotifyFunc = std::function<void(const PlayBinMessage&)>;

void PlayerEngineGstImpl::OnNotifyMessage(const PlayBinMessage &msg)
{
    const std::unordered_map<int32_t, MsgNotifyFunc> MSG_NOTIFY_FUNC_TABLE = {
        { PLAYBIN_MSG_ERROR, std::bind(&PlayerEngineGstImpl::HandleErrorMessage, this, std::placeholders::_1) },
        { PLAYBIN_MSG_SEEKDONE, std::bind(&PlayerEngineGstImpl::HandleSeekDoneMessage, this, std::placeholders::_1) },
        { PLAYBIN_MSG_SPEEDDONE, std::bind(&PlayerEngineGstImpl::HandleSpeedDoneMessage, this, std::placeholders::_1) },
        { PLAYBIN_MSG_BITRATEDONE, std::bind(&PlayerEngineGstImpl::HandleInfoMessage, this, std::placeholders::_1)},
        { PLAYBIN_MSG_EOS, std::bind(&PlayerEngineGstImpl::HandleInfoMessage, this, std::placeholders::_1) },
        { PLAYBIN_MSG_STATE_CHANGE, std::bind(&PlayerEngineGstImpl::HandleInfoMessage, this, std::placeholders::_1) },
        { PLAYBIN_MSG_SUBTYPE, std::bind(&PlayerEngineGstImpl::HandleSubTypeMessage, this, std::placeholders::_1) },
        { PLAYBIN_MSG_AUDIO_SINK, std::bind(&PlayerEngineGstImpl::HandleAudioMessage, this, std::placeholders::_1) },
        { PLAYBIN_MSG_POSITION_UPDATE, std::bind(&PlayerEngineGstImpl::HandlePositionUpdateMessage, this,
            std::placeholders::_1) },
    };
    if (MSG_NOTIFY_FUNC_TABLE.count(msg.type) != 0) {
        MSG_NOTIFY_FUNC_TABLE.at(msg.type)(msg);
    }
}

int32_t PlayerEngineGstImpl::PlayBinCtrlerInit()
{
    if (playBinCtrler_) {
        return MSERR_OK;
    }

    MEDIA_LOGD("PlayBinCtrlerInit in");
    int ret = PlayBinCtrlerPrepare();
    if (ret != MSERR_OK) {
        MEDIA_LOGE("PlayBinCtrlerPrepare failed");
        PlayBinCtrlerDeInit();
        return MSERR_INVALID_VAL;
    }

    MEDIA_LOGD("PlayBinCtrlerInit out");
    return MSERR_OK;
}

void PlayerEngineGstImpl::PlayBinCtrlerDeInit()
{
    url_.clear();
    useSoftDec_ = false;
    appsrcWrap_ = nullptr;

    if (playBinCtrler_ != nullptr) {
        playBinCtrler_->SetElemSetupListener(nullptr);
        playBinCtrler_->SetElemUnSetupListener(nullptr);
        playBinCtrler_->SetAutoPlugSortListener(nullptr);
        playBinCtrler_ = nullptr;
    }

    {
        std::unique_lock<std::mutex> lk(trackParseMutex_);
        trackParse_ = nullptr;
        sinkProvider_ = nullptr;
    }
}

int32_t PlayerEngineGstImpl::PlayBinCtrlerPrepare()
{
    uint8_t renderMode = IPlayBinCtrler::PlayBinRenderMode::DEFAULT_RENDER;
    auto notifier = std::bind(&PlayerEngineGstImpl::OnNotifyMessage, this, std::placeholders::_1);

    {
        std::unique_lock<std::mutex> lk(trackParseMutex_);
        sinkProvider_ = std::make_shared<PlayerSinkProvider>(producerSurface_);
        sinkProvider_->SetAppInfo(appuid_, apppid_);
    }

    IPlayBinCtrler::PlayBinCreateParam createParam = {
        static_cast<IPlayBinCtrler::PlayBinRenderMode>(renderMode), notifier, sinkProvider_
    };
    playBinCtrler_ = IPlayBinCtrler::Create(IPlayBinCtrler::PlayBinKind::PLAYBIN2, createParam);
    CHECK_AND_RETURN_RET_LOG(playBinCtrler_ != nullptr, MSERR_INVALID_VAL, "playBinCtrler_ is nullptr");

    int32_t ret;
    if (appsrcWrap_ == nullptr) {
        ret = playBinCtrler_->SetSource(url_);
    } else {
        ret = playBinCtrler_->SetSource(appsrcWrap_);
    }
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_VAL, "SetSource failed");

    ret = SetVideoScaleType(videoScaleType_);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_VAL, "SetVideoScaleType failed");

    ret = SetAudioRendererInfo(contentType_, streamUsage_, rendererFlag_);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_VAL, "SetAudioRendererInfo failed");

    auto setupListener = std::bind(&PlayerEngineGstImpl::OnNotifyElemSetup, this, std::placeholders::_1);
    playBinCtrler_->SetElemSetupListener(setupListener);

    auto unSetupListener = std::bind(&PlayerEngineGstImpl::OnNotifyElemUnSetup, this, std::placeholders::_1);
    playBinCtrler_->SetElemUnSetupListener(unSetupListener);

    auto autoPlugSortListener = std::bind(&PlayerEngineGstImpl::OnNotifyAutoPlugSort, this, std::placeholders::_1);
    playBinCtrler_->SetAutoPlugSortListener(autoPlugSortListener);

    {
        std::unique_lock<std::mutex> lk(trackParseMutex_);
        trackParse_ = PlayerTrackParse::Create();
        if (trackParse_ == nullptr) {
            MEDIA_LOGE("creat track parse failed");
        }
    }

    return MSERR_OK;
}

int32_t PlayerEngineGstImpl::Play()
{
    std::unique_lock<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(playBinCtrler_ != nullptr, MSERR_INVALID_OPERATION, "playBinCtrler_ is nullptr");

    MEDIA_LOGI("Play in");
    playBinCtrler_->Play();
    return MSERR_OK;
}

int32_t PlayerEngineGstImpl::Pause()
{
    std::unique_lock<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(playBinCtrler_ != nullptr, MSERR_INVALID_OPERATION, "playBinCtrler_ is nullptr");

    MEDIA_LOGI("Pause in");
    (void)playBinCtrler_->Pause();
    return MSERR_OK;
}

int32_t PlayerEngineGstImpl::GetCurrentTime(int32_t &currentTime)
{
    currentTime = currentTime_;
    MEDIA_LOGD("Time in milliseconds: %{public}d", currentTime);
    return MSERR_OK;
}

int32_t PlayerEngineGstImpl::GetVideoTrackInfo(std::vector<Format> &videoTrack)
{
    CHECK_AND_RETURN_RET_LOG(trackParse_ != nullptr, MSERR_INVALID_OPERATION, "trackParse_ is nullptr");

    return trackParse_->GetVideoTrackInfo(videoTrack);
}

int32_t PlayerEngineGstImpl::GetAudioTrackInfo(std::vector<Format> &audioTrack)
{
    CHECK_AND_RETURN_RET_LOG(trackParse_ != nullptr, MSERR_INVALID_OPERATION, "trackParse_ is nullptr");

    return trackParse_->GetAudioTrackInfo(audioTrack);
}

int32_t PlayerEngineGstImpl::GetVideoWidth()
{
    return videoWidth_;
}

int32_t PlayerEngineGstImpl::GetVideoHeight()
{
    return videoHeight_;
}

int32_t PlayerEngineGstImpl::GetDuration(int32_t &duration)
{
    duration = duration_;
    MEDIA_LOGD("Duration in milliseconds: %{public}d", duration);
    return MSERR_OK;
}

double PlayerEngineGstImpl::ChangeModeToSpeed(const PlaybackRateMode &mode) const
{
    switch (mode) {
        case SPEED_FORWARD_0_75_X:
            return SPEED_0_75_X;
        case SPEED_FORWARD_1_00_X:
            return SPEED_1_00_X;
        case SPEED_FORWARD_1_25_X:
            return SPEED_1_25_X;
        case SPEED_FORWARD_1_75_X:
            return SPEED_1_75_X;
        case SPEED_FORWARD_2_00_X:
            return SPEED_2_00_X;
        default:
            MEDIA_LOGW("unknown mode:%{public}d, return default speed(SPEED_1_00_X)", mode);
    }

    return SPEED_1_00_X;
}

PlaybackRateMode PlayerEngineGstImpl::ChangeSpeedToMode(double rate) const
{
    if (abs(rate - SPEED_0_75_X) < EPSINON) {
        return SPEED_FORWARD_0_75_X;
    }
    if (abs(rate - SPEED_1_00_X) < EPSINON) {
        return SPEED_FORWARD_1_00_X;
    }
    if (abs(rate - SPEED_1_25_X) < EPSINON) {
        return SPEED_FORWARD_1_25_X;
    }
    if (abs(rate - SPEED_1_75_X) < EPSINON) {
        return SPEED_FORWARD_1_75_X;
    }
    if (abs(rate - SPEED_2_00_X) < EPSINON) {
        return SPEED_FORWARD_2_00_X;
    }

    MEDIA_LOGW("unknown rate:%{public}lf, return default speed(SPEED_FORWARD_1_00_X)", rate);

    return  SPEED_FORWARD_1_00_X;
}

int32_t PlayerEngineGstImpl::SetPlaybackSpeed(PlaybackRateMode mode)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (playBinCtrler_ != nullptr) {
        double rate = ChangeModeToSpeed(mode);
        return playBinCtrler_->SetRate(rate);
    }
    return MSERR_OK;
}

int32_t PlayerEngineGstImpl::GetPlaybackSpeed(PlaybackRateMode &mode)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (playBinCtrler_ != nullptr) {
        double rate = playBinCtrler_->GetRate();
        mode = ChangeSpeedToMode(rate);
    }
    return MSERR_OK;
}

int32_t PlayerEngineGstImpl::SetLooping(bool loop)
{
    if (playBinCtrler_ != nullptr) {
        MEDIA_LOGD("SetLooping in");
        return playBinCtrler_->SetLoop(loop);
    }
    return MSERR_OK;
}

int32_t PlayerEngineGstImpl::SetParameter(const Format &param)
{
    if (param.ContainKey(PlayerKeys::VIDEO_SCALE_TYPE)) {
        int32_t videoScaleType = 0;
        param.GetIntValue(PlayerKeys::VIDEO_SCALE_TYPE, videoScaleType);
        return SetVideoScaleType(VideoScaleType(videoScaleType));
    }
    if (param.ContainKey(PlayerKeys::CONTENT_TYPE) && param.ContainKey(PlayerKeys::STREAM_USAGE)) {
        param.GetIntValue(PlayerKeys::CONTENT_TYPE, contentType_);
        param.GetIntValue(PlayerKeys::STREAM_USAGE, streamUsage_);
        param.GetIntValue(PlayerKeys::RENDERER_FLAG, rendererFlag_);
        return SetAudioRendererInfo(contentType_, streamUsage_, rendererFlag_);
    }
    if (param.ContainKey(PlayerKeys::AUDIO_INTERRUPT_MODE)) {
        int32_t interruptMode = 0;
        param.GetIntValue(PlayerKeys::AUDIO_INTERRUPT_MODE, interruptMode);
        return SetAudioInterruptMode(interruptMode);
    }
    return MSERR_OK;
}

int32_t PlayerEngineGstImpl::Stop()
{
    std::unique_lock<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(playBinCtrler_ != nullptr, MSERR_INVALID_OPERATION, "playBinCtrler_ is nullptr");

    MEDIA_LOGD("Stop in");
    if (trackParse_ != nullptr) {
        trackParse_->Stop();
    }
    playBinCtrler_->Stop(true);
    return MSERR_OK;
}

int32_t PlayerEngineGstImpl::Reset()
{
    MEDIA_LOGD("Reset in");
    OnReset();
    return MSERR_OK;
}

void PlayerEngineGstImpl::OnReset()
{
    if (taskQueue_ != nullptr) {
        (void)taskQueue_->Stop();
    }

    std::unique_lock<std::mutex> lock(mutex_);
    PlayBinCtrlerDeInit();
}

int32_t PlayerEngineGstImpl::Seek(int32_t mSeconds, PlayerSeekMode mode)
{
    std::unique_lock<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(playBinCtrler_ != nullptr, MSERR_INVALID_OPERATION, "playBinCtrler_ is nullptr");
    MEDIA_LOGI("Seek in %{public}dms", mSeconds);

    if (playBinCtrler_->QueryPosition() == mSeconds) {
        MEDIA_LOGW("current time same to seek time, invalid seek");
        Format format;
        std::shared_ptr<IPlayerEngineObs> notifyObs = obs_.lock();
        if (notifyObs != nullptr) {
            notifyObs->OnInfo(INFO_TYPE_SEEKDONE, mSeconds, format);
        }
        return MSERR_OK;
    }

    codecCtrl_.EnhanceSeekPerformance(true);

    int64_t position = static_cast<int64_t>(mSeconds) * MSEC_PER_USEC;
    return playBinCtrler_->Seek(position, mode);
}

int32_t PlayerEngineGstImpl::SetVolume(float leftVolume, float rightVolume)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (playBinCtrler_ != nullptr) {
        MEDIA_LOGD("SetVolume in");
        playBinCtrler_->SetVolume(leftVolume, rightVolume);
    }
    return MSERR_OK;
}

int32_t PlayerEngineGstImpl::SelectBitRate(uint32_t bitRate)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (playBinCtrler_ != nullptr) {
        MEDIA_LOGD("SelectBitRate in");
        return playBinCtrler_->SelectBitRate(bitRate);
    }
    return MSERR_INVALID_OPERATION;
}

int32_t PlayerEngineGstImpl::SetVideoScaleType(VideoScaleType videoScaleType)
{
    std::unique_lock<std::mutex> lock(mutex_, std::try_to_lock);
    if (sinkProvider_ != nullptr) {
        MEDIA_LOGD("SetVideoScaleType in");
        sinkProvider_->SetVideoScaleType(static_cast<uint32_t>(videoScaleType));
    }
    videoScaleType_ = videoScaleType;
    return MSERR_OK;
}

int32_t PlayerEngineGstImpl::SetAudioRendererInfo(const int32_t contentType,
    const int32_t streamUsage, const int32_t rendererFlag)
{
    std::unique_lock<std::mutex> lock(mutex_, std::try_to_lock);
    contentType_ = contentType;
    streamUsage_ = streamUsage;
    rendererFlag_ = rendererFlag;
    if (playBinCtrler_ != nullptr) {
        MEDIA_LOGD("SetAudioRendererInfo in");
        uint32_t rendererInfo(0);
        rendererInfo |= (contentType | (static_cast<uint32_t>(streamUsage) <<
            AudioStandard::RENDERER_STREAM_USAGE_SHIFT));
        playBinCtrler_->SetAudioRendererInfo(rendererInfo, rendererFlag);
    }
    return MSERR_OK;
}

int32_t PlayerEngineGstImpl::SetAudioInterruptMode(const int32_t interruptMode)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (playBinCtrler_ != nullptr) {
        MEDIA_LOGD("SetAudioInterruptMode in");
        playBinCtrler_->SetAudioInterruptMode(interruptMode);
    }
    return MSERR_OK;
}

GValueArray *PlayerEngineGstImpl::OnNotifyAutoPlugSort(GValueArray &factories)
{
    if (isPlaySinkFlagsSet_) {
        return nullptr;
    }

    if (useSoftDec_) {
        GValueArray *result = g_value_array_new(factories.n_values);

        for (uint32_t i = 0; i < factories.n_values; i++) {
            GstElementFactory *factory =
                static_cast<GstElementFactory *>(g_value_get_object(g_value_array_get_nth(&factories, i)));
            if (strstr(gst_element_factory_get_metadata(factory, GST_ELEMENT_METADATA_KLASS),
                "Codec/Decoder/Video/Hardware")) {
                MEDIA_LOGD("set remove hardware codec plugins from pipeline");
                continue;
            }
            GValue val = G_VALUE_INIT;
            g_value_init(&val, G_TYPE_OBJECT);
            g_value_set_object(&val, factory);
            result = g_value_array_append(result, &val);
            g_value_unset(&val);
        }
        return result;
    } else {
        for (uint32_t i = 0; i < factories.n_values; i++) {
            GstElementFactory *factory =
                static_cast<GstElementFactory *>(g_value_get_object(g_value_array_get_nth(&factories, i)));
            if (strstr(gst_element_factory_get_metadata(factory, GST_ELEMENT_METADATA_KLASS),
                "Codec/Decoder/Video/Hardware")) {
                MEDIA_LOGD("set remove GstPlaySinkVideoConvert plugins from pipeline");
                playBinCtrler_->RemoveGstPlaySinkVideoConvertPlugin();
                isPlaySinkFlagsSet_ = true;
                break;
            }
        }
    }

    return nullptr;
}

void PlayerEngineGstImpl::OnNotifyElemSetup(GstElement &elem)
{
    std::unique_lock<std::mutex> lock(trackParseMutex_);

    const gchar *metadata = gst_element_get_metadata(&elem, GST_ELEMENT_METADATA_KLASS);
    CHECK_AND_RETURN_LOG(metadata != nullptr, "gst_element_get_metadata return nullptr");

    MEDIA_LOGD("get element_name %{public}s, get metadata %{public}s", GST_ELEMENT_NAME(&elem), metadata);
    std::string metaStr(metadata);

    if (trackParse_ != nullptr && trackParse_->GetDemuxerElementFind() == false) {
        if (metaStr.find("Codec/Demuxer") != std::string::npos) {
            trackParse_->SetUpDemuxerElementCb(elem);
            trackParse_->SetDemuxerElementFind(true);
        } else if (metaStr.find("Codec/Parser") != std::string::npos) {
            trackParse_->SetUpParseElementCb(elem);
            trackParse_->SetDemuxerElementFind(true);
        }
    }

    if (metaStr.find("Codec/Decoder/Video") != std::string::npos || metaStr.find("Sink/Video") != std::string::npos) {
        if (producerSurface_ != nullptr) {
            CHECK_AND_RETURN_LOG(sinkProvider_ != nullptr, "sinkProvider_ is nullptr");
            GstElement *videoSink = sinkProvider_->GetVideoSink();
            CHECK_AND_RETURN_LOG(videoSink != nullptr, "videoSink is nullptr");
            codecCtrl_.DetectCodecSetup(metaStr, &elem, videoSink);
            auto notifier = std::bind(&PlayerEngineGstImpl::OnCapsFixError, this);
            codecCtrl_.SetCapsFixErrorCb(notifier);
        }
    }
}

void PlayerEngineGstImpl::OnNotifyElemUnSetup(GstElement &elem)
{
    std::unique_lock<std::mutex> lock(trackParseMutex_);

    const gchar *metadata = gst_element_get_metadata(&elem, GST_ELEMENT_METADATA_KLASS);
    CHECK_AND_RETURN_LOG(metadata != nullptr, "gst_element_get_metadata return nullptr");

    MEDIA_LOGD("get element_name %{public}s, get metadata %{public}s", GST_ELEMENT_NAME(&elem), metadata);
    std::string metaStr(metadata);

    if (metaStr.find("Codec/Decoder/Video") != std::string::npos) {
        if (producerSurface_ != nullptr) {
            CHECK_AND_RETURN_LOG(sinkProvider_ != nullptr, "sinkProvider_ is nullptr");
            GstElement *videoSink = sinkProvider_->GetVideoSink();
            CHECK_AND_RETURN_LOG(videoSink != nullptr, "videoSink is nullptr");
            codecCtrl_.DetectCodecUnSetup(&elem, videoSink);
        }
    }
}

void PlayerEngineGstImpl::OnCapsFixError()
{
    MEDIA_LOGD("OnCapsFixError in");

    if (taskQueue_ == nullptr) {
        taskQueue_ = std::make_unique<TaskQueue>("reset-playbin-task");
        int32_t ret = taskQueue_->Start();
        CHECK_AND_RETURN_LOG(ret == MSERR_OK, "task queue start failed");
    }

    useSoftDec_ = true;
    auto resetTask = std::make_shared<TaskHandler<void>>([this]() {
        ResetPlaybinToSoftDec();
    });
    (void)taskQueue_->EnqueueTask(resetTask);
}

// fix video with small resolution cannot play in hardware decoding
// reset pipeline to use software decoder
void PlayerEngineGstImpl::ResetPlaybinToSoftDec()
{
    std::unique_lock<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_LOG(playBinCtrler_ != nullptr, "playBinCtrler_ is nullptr");

    MEDIA_LOGD("ResetPlaybinToSoftDec in");
    isPlaySinkFlagsSet_ = false;

    if (playBinCtrler_ != nullptr) {
        playBinCtrler_->SetNotifier(nullptr);
        playBinCtrler_->Stop(false);
        playBinCtrler_->SetElemSetupListener(nullptr);
        playBinCtrler_->SetElemUnSetupListener(nullptr);
        playBinCtrler_->SetAutoPlugSortListener(nullptr);
        playBinCtrler_ = nullptr;
    }

    {
        std::unique_lock<std::mutex> lk(trackParseMutex_);
        trackParse_ = nullptr;
        sinkProvider_ = nullptr;
    }

    lock.unlock();
    PrepareAsync();
}
} // namespace Media
} // namespace OHOS
