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

#include "inactive_device_state.h"

#include "telephony_log_wrapper.h"

#include "audio_control_manager.h"

namespace OHOS {
namespace Telephony {
bool InactiveDeviceState::ProcessEvent(int32_t event)
{
    bool result = false;
    std::lock_guard<std::mutex> lock(mutex_);
    switch (event) {
        case AudioEvent::AUDIO_ACTIVATED:
        case AudioEvent::AUDIO_RINGING:
            // should reinitialize audio device when audio interrupted or ringing
            result =
                DelayedSingleton<AudioDeviceManager>::GetInstance()->ProcessEvent(AudioEvent::INIT_AUDIO_DEVICE);
            break;
        case AudioEvent::AUDIO_DEACTIVATED:
            // do nothing
            break;
        default:
            break;
    }
    TELEPHONY_LOGI("devices inactive lock release");
    return result;
}
} // namespace Telephony
} // namespace OHOS