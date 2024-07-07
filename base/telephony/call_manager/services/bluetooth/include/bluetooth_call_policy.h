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

#ifndef BLUETOOTH_CALL_POLICY_H
#define BLUETOOTH_CALL_POLICY_H

#include <string>
#include <memory>

#include "call_object_manager.h"

namespace OHOS {
namespace Telephony {
class BluetoothCallPolicy : public CallObjectManager {
public:
    BluetoothCallPolicy();
    ~BluetoothCallPolicy();

    int32_t AnswerCallPolicy(int32_t &callId);
    int32_t RejectCallPolicy(int32_t &callId);
    int32_t HoldCallPolicy(int32_t &callId);
    int32_t UnHoldCallPolicy(int32_t &callId);
    int32_t HangUpPolicy(int32_t &callId);
    int32_t SwitchCallPolicy(int32_t &callId);
    int32_t StartDtmfPolicy(int32_t &callId);
    int32_t CombineConferencePolicy(int32_t &callId);
    int32_t SeparateConferencePolicy(int32_t &callId);
};
} // namespace Telephony
} // namespace OHOS

#endif // BLUETOOTH_CALL_POLICY_H
