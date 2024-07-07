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

#ifndef COMMON_TYPE_H
#define COMMON_TYPE_H

#include <cstdio>
#include <list>
#include <string>

#include "call_manager_inner_type.h"
#include "telephony_log_wrapper.h"

const int16_t RINGING_CALL_NUMBER_LEN = 30;
const int16_t DIALING_CALL_NUMBER_LEN = 30;
const int16_t CONTACT_NAME_LEN = 10;
const int16_t FILE_PATH_MAX_LEN = 60;
const int16_t CALL_START_ID = 0;

namespace OHOS {
namespace Telephony {
struct DialParaInfo {
    DialParaInfo() : accountId(0), callId(0), index(0), dialType(DialType::DIAL_CARRIER_TYPE),
        videoState(VideoStateType::TYPE_VOICE), callType(CallType::TYPE_ERR_CALL),
        callState(TelCallState::CALL_STATUS_IDLE), isDialing(false), isEcc(false) {}
    int32_t accountId;
    int32_t callId;
    int32_t index;
    std::string number;
    DialType dialType;
    VideoStateType videoState;
    CallType callType;
    TelCallState callState;
    bool isDialing;
    bool isEcc;
    std::string bundleName;
};

enum PolicyFlag : uint64_t {
    // Denote playing ring tone
    POLICY_FLAG_PLAY_RINGTONE = 0x00000001,
    // Denote vibration prompt
    POLICY_FLAG_VIBRATING_ALERT = 0x00000002,
    // Represents the play prompt tone
    POLICY_FLAG_PLAY_SOUND = 0x00000004,
    // Represents an audio routing handset
    POLICY_FLAG_AUDIO_DEVICE_EARPIECE = 0x00000008,
    // Represents an audio routing speaker
    POLICY_FLAG_AUDIO_DEVICE_SPEAKER = 0x00000010,
    // Represents audio routing wired headset
    POLICY_FLAG_AUDIO_DEVICE_WIRED_HEADSET = 0x00000020,
    // Represents an audio routing Bluetooth headset
    POLICY_FLAG_AUDIO_DEVICE_BLUETOOTH = 0x00000040,
    // Indicates that no audio device is active
    POLICY_FLAG_AUDIO_DEVICE_DISABLE = 0x00000080,
    // release the held call and the wait call
    POLICY_FLAG_HANG_UP_HOLD_WAIT = 0x00000100,
    // release the active call and recover the held call
    POLICY_FLAG_HANG_UP_ACTIVE = 0x00000200,
    // release all calls
    POLICY_FLAG_HANG_UP_ALL = 0x00000400,
};

struct ContactInfo {
    char name[CONTACT_NAME_LEN];
    char number[kMaxNumberLen];
    bool isContacterExists;
    char ringtonePath[FILE_PATH_MAX_LEN];
    bool isSendToVoicemail;
    bool isEcc;
    bool isVoiceMail;
};
} // namespace Telephony
} // namespace OHOS

#endif // COMMON_TYPE_H
