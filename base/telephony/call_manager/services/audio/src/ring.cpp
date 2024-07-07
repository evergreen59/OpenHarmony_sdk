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

#include "ring.h"

#include <thread>

#include "telephony_log_wrapper.h"

#include "audio_player.h"

namespace OHOS {
namespace Telephony {
Ring::Ring() : isVibrating_(false), shouldRing_(false), shouldVibrate_(false), ringtonePath_("")
{
    Init(DelayedSingleton<AudioProxy>::GetInstance()->GetDefaultRingPath());
}

Ring::Ring(const std::string &path)
    : isVibrating_(false), shouldRing_(false), shouldVibrate_(false), ringtonePath_("")
{
    Init(path);
}

Ring::~Ring() {}

void Ring::Init(const std::string &ringtonePath)
{
    if (ringtonePath.empty()) {
        TELEPHONY_LOGE("ringtone path empty");
        return;
    }
    if (AudioStandard::AudioRingerMode::RINGER_MODE_NORMAL ==
        DelayedSingleton<AudioProxy>::GetInstance()->GetRingerMode()) {
        shouldRing_ = true;
        shouldVibrate_ = true;
    } else if (AudioStandard::AudioRingerMode::RINGER_MODE_VIBRATE ==
        DelayedSingleton<AudioProxy>::GetInstance()->GetRingerMode()) {
        shouldRing_ = false;
        shouldVibrate_ = true;
    } else if (AudioStandard::AudioRingerMode::RINGER_MODE_SILENT ==
        DelayedSingleton<AudioProxy>::GetInstance()->GetRingerMode()) {
        shouldRing_ = false;
        shouldVibrate_ = false;
    }
    ringtonePath_ = ringtonePath;
}

int32_t Ring::Play()
{
    if (!shouldRing_ || ringtonePath_.empty()) {
        TELEPHONY_LOGE("should not ring or ringtone path empty");
        return CALL_ERR_INVALID_PATH;
    }
    int32_t result = TELEPHONY_SUCCESS;
    std::thread play(
        AudioPlayer::Play, ringtonePath_, AudioStandard::AudioStreamType::STREAM_RING, PlayerType::TYPE_RING);
    play.detach();
    if (shouldVibrate_) {
        result = StartVibrate();
    }
    return result;
}

int32_t Ring::Stop()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!shouldRing_ || ringtonePath_.empty()) {
        TELEPHONY_LOGE("should not ring or ringtone path empty");
        return CALL_ERR_INVALID_PATH;
    }
    int32_t result = TELEPHONY_SUCCESS;
    AudioPlayer::SetStop(PlayerType::TYPE_RING, true);
    if (isVibrating_) {
        result = CancelVibrate();
    }
    return result;
}

int32_t Ring::StartVibrate()
{
    if (DelayedSingleton<AudioProxy>::GetInstance()->StartVibrate() == TELEPHONY_SUCCESS) {
        isVibrating_ = true;
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGE("start vibrate failed");
    return CALL_ERR_AUDIO_START_VIBRATE_FAILED;
}

int32_t Ring::CancelVibrate()
{
    if (DelayedSingleton<AudioProxy>::GetInstance()->CancelVibrate() == TELEPHONY_SUCCESS) {
        isVibrating_ = false;
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGE("cancel vibrate failed");
    return CALL_ERR_AUDIO_CANCEL_VIBRATE_FAILED;
}

bool Ring::ShouldVibrate()
{
    return DelayedSingleton<AudioProxy>::GetInstance()->GetRingerMode() !=
        AudioStandard::AudioRingerMode::RINGER_MODE_SILENT;
}
} // namespace Telephony
} // namespace OHOS