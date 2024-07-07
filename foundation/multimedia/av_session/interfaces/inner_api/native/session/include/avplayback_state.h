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

#ifndef OHOS_AVPLAYBACK_STATE_H
#define OHOS_AVPLAYBACK_STATE_H

#include <bitset>
#include <parcel.h>

namespace OHOS::AVSession {
class AVPlaybackState : public Parcelable {
public:
    enum {
        PLAYBACK_STATE_INITIAL = 0,
        PLAYBACK_STATE_PREPARING = 1,
        PLAYBACK_STATE_PLAYING = 2,
        PLAYBACK_STATE_PAUSED = 3,
        PLAYBACK_STATE_FAST_FORWARD = 4,
        PLAYBACK_STATE_REWIND = 5,
        PLAYBACK_STATE_STOP = 6,
        PLAYBACK_STATE_MAX = 7
    };

    enum {
        PLAYBACK_KEY_STATE = 0,
        PLAYBACK_KEY_SPEED = 1,
        PLAYBACK_KEY_POSITION = 2,
        PLAYBACK_KEY_BUFFERED_TIME = 3,
        PLAYBACK_KEY_LOOP_MODE = 4,
        PLAYBACK_KEY_IS_FAVORITE = 5,
        PLAYBACK_KEY_MAX = 6
    };

    enum {
        LOOP_MODE_SEQUENCE = 0,
        LOOP_MODE_SINGLE = 1,
        LOOP_MODE_LIST = 2,
        LOOP_MODE_SHUFFLE = 3
    };

    struct Position {
        int64_t elapsedTime_ {};
        int64_t updateTime_ {};
    };

    using PlaybackStateMaskType = std::bitset<PLAYBACK_KEY_MAX>;

    AVPlaybackState();
    ~AVPlaybackState() override = default;

    static AVPlaybackState* Unmarshalling(Parcel& parcel);
    bool Marshalling(Parcel& parcel) const override;

    bool IsValid() const;

    void SetState(int32_t state);
    int32_t GetState() const;

    void SetSpeed(double speed);
    double GetSpeed() const;

    void SetPosition(const Position& position);
    Position GetPosition() const;

    void SetBufferedTime(int64_t time);
    int64_t GetBufferedTime() const;

    void SetLoopMode(int32_t mode);
    int32_t GetLoopMode() const;

    void SetFavorite(bool isFavorite);
    bool GetFavorite() const;

    PlaybackStateMaskType GetMask() const;

    bool CopyToByMask(PlaybackStateMaskType& mask, AVPlaybackState& out) const;
    bool CopyFrom(const AVPlaybackState& in);

    const static inline std::vector<int32_t> localCapability {
        PLAYBACK_KEY_STATE,
        PLAYBACK_KEY_SPEED,
        PLAYBACK_KEY_POSITION,
        PLAYBACK_KEY_BUFFERED_TIME,
        PLAYBACK_KEY_LOOP_MODE,
        PLAYBACK_KEY_IS_FAVORITE,
    };

private:
    PlaybackStateMaskType mask_;

    int32_t state_ = PLAYBACK_STATE_INITIAL;
    double speed_ = 1.0;
    Position position_;
    int64_t bufferedTime_ {};
    int32_t loopMode_ = LOOP_MODE_SEQUENCE;
    bool isFavorite_ {};

    static void CloneState(const AVPlaybackState& from, AVPlaybackState& to);
    static void CloneSpeed(const AVPlaybackState& from, AVPlaybackState& to);
    static void ClonePosition(const AVPlaybackState& from, AVPlaybackState& to);
    static void CloneBufferedTime(const AVPlaybackState& from, AVPlaybackState& to);
    static void CloneLoopMode(const AVPlaybackState& from, AVPlaybackState& to);
    static void CloneIsFavorite(const AVPlaybackState& from, AVPlaybackState& to);

    using CloneActionType = void(*)(const AVPlaybackState& from, AVPlaybackState& to);
    static inline CloneActionType cloneActions[PLAYBACK_KEY_MAX] = {
        [PLAYBACK_KEY_STATE] = &AVPlaybackState::CloneState,
        [PLAYBACK_KEY_SPEED] = &AVPlaybackState::CloneSpeed,
        [PLAYBACK_KEY_POSITION] = &AVPlaybackState::ClonePosition,
        [PLAYBACK_KEY_BUFFERED_TIME] = &AVPlaybackState::CloneBufferedTime,
        [PLAYBACK_KEY_LOOP_MODE] = &AVPlaybackState::CloneLoopMode,
        [PLAYBACK_KEY_IS_FAVORITE] = &AVPlaybackState::CloneIsFavorite,
    };
};
} // namespace OHOS::AVSession
#endif // OHOS_AVPLAYBACK_STATE_H