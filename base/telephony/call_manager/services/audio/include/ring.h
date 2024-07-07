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

#ifndef TELEPHONY_AUDIO_RING_H
#define TELEPHONY_AUDIO_RING_H

#include <memory>

#include "audio_renderer.h"

#include "audio_player.h"
#include "audio_proxy.h"

namespace OHOS {
namespace Telephony {
struct RingtoneStream {
    std::string ringtonePath;
    uint32_t playedPosition;
};
enum RingState {
    RINGING = 0,
    STOPPED,
};

/**
 * @class Ring
 * plays the default or specific ringtone.
 */
class Ring {
public:
    Ring();
    explicit Ring(const std::string &path);
    virtual ~Ring();
    void Init(const std::string &ringtonePath);
    int32_t Play();
    int32_t Stop();
    int32_t StartVibrate();
    int32_t CancelVibrate();

private:
    bool isVibrating_;
    bool shouldRing_;
    bool shouldVibrate_;
    bool ShouldVibrate();
    std::string ringtonePath_;
    std::mutex mutex_;
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_AUDIO_RING_H