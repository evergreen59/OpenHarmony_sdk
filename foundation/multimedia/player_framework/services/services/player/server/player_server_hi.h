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

#ifndef PLAYER_SERVICE_SERVER_HI_H
#define PLAYER_SERVICE_SERVER_HI_H

#include "i_player_service.h"
#include "i_player_engine.h"
#include "time_monitor.h"
#include "nocopyable.h"
#include "uri_helper.h"

namespace OHOS {
namespace Media {
class PlayerServerHi : public IPlayerService, public IPlayerEngineObs, public NoCopyable {
public:
    static std::shared_ptr<IPlayerService> Create();
    PlayerServerHi();
    virtual ~PlayerServerHi();

    int32_t SetSource(const std::string &url) override;
    int32_t SetSource(const std::shared_ptr<IMediaDataSource> &dataSrc) override;
    int32_t SetSource(int32_t fd, int64_t offset, int64_t size) override;
    int32_t Play() override;
    int32_t Prepare() override;
    int32_t PrepareAsync() override;
    int32_t Pause() override;
    int32_t Stop() override;
    int32_t Reset() override;
    int32_t Release() override;
    int32_t ReleaseSync() override;
    int32_t SetVolume(float leftVolume, float rightVolume) override;
    int32_t Seek(int32_t mSeconds, PlayerSeekMode mode) override;
    int32_t GetCurrentTime(int32_t &currentTime) override;
    int32_t GetVideoTrackInfo(std::vector<Format> &videoTrack) override;
    int32_t GetAudioTrackInfo(std::vector<Format> &audioTrack) override;
    int32_t GetVideoWidth() override;
    int32_t GetVideoHeight() override;
    int32_t GetDuration(int32_t &duration) override;
    int32_t SetPlaybackSpeed(PlaybackRateMode mode) override;
    int32_t GetPlaybackSpeed(PlaybackRateMode &mode) override;
#ifdef SUPPORT_VIDEO
    int32_t SetVideoSurface(sptr<Surface> surface) override;
#endif
    bool IsPlaying() override;
    bool IsLooping() override;
    int32_t SetLooping(bool loop) override;
    int32_t SetParameter(const Format &param) override;
    int32_t SetPlayerCallback(const std::shared_ptr<PlayerCallback> &callback) override;
    int32_t DumpInfo(int32_t fd);
    int32_t SelectBitRate(uint32_t bitRate) override;

    // IPlayerEngineObs override
    void OnError(PlayerErrorType errorType, int32_t errorCode) override;
    void OnErrorMessage(int32_t errorCode, const std::string &errorMsg) override;
    void OnInfo(PlayerOnInfoType type, int32_t extra, const Format &infoBody = {}) override;

private:
    int32_t Init();
    bool IsValidSeekMode(PlayerSeekMode mode);
    int32_t OnReset();
    int32_t InitPlayEngine(const std::string &url);
    int32_t OnPrepare(bool async);
    void FormatToString(std::string &dumpString, std::vector<Format> &videoTrack);
    const std::string &GetStatusDescription(int32_t status);
    void ResetProcessor();

    std::unique_ptr<IPlayerEngine> playerEngine_ = nullptr;
    std::shared_ptr<PlayerCallback> playerCb_ = nullptr;
#ifdef SUPPORT_VIDEO
    sptr<Surface> surface_ = nullptr;
#endif
    PlayerStates status_ = PLAYER_IDLE;
    std::mutex mutex_;
    std::mutex mutexCb_;
    TimeMonitor startTimeMonitor_;
    TimeMonitor stopTimeMonitor_;
    std::shared_ptr<IMediaDataSource> dataSrc_ = nullptr;
    std::unique_ptr<UriHelper> uriHelper_;
    struct ConfigInfo {
        bool looping = false;
        float leftVolume = 1.0f; // audiotrack volume range [0, 1]
        float rightVolume = 1.0f; // audiotrack volume range [0, 1]
        PlaybackRateMode speedMode = SPEED_FORWARD_1_00_X;
        std::string url;
    } config_;
    std::string lastErrMsg_;
    int32_t resetRet_ = 0;
};
} // namespace Media
} // namespace OHOS
#endif // PLAYER_SERVICE_SERVER_HI_H
