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

#define private public
#define protected public
#include "notification_slot.h"
#undef private
#undef protected
#include "notificationslot_fuzzer.h"

namespace OHOS {
    namespace {
        constexpr uint8_t ENABLE = 2;
    }
    bool DoSomethingInterestingWithMyAPI(const char* data, size_t size)
    {
        Notification::NotificationSlot notificationSlot;
        bool enabled = *data % ENABLE;
        notificationSlot.CanEnableLight();
        notificationSlot.CanVibrate();
        notificationSlot.GetDescription();
        notificationSlot.GetId();
        notificationSlot.GetLedLightColor();
        notificationSlot.GetLevel();
        notificationSlot.GetType();
        notificationSlot.GetLockScreenVisibleness();
        notificationSlot.GetName();
        notificationSlot.GetSound();
        notificationSlot.GetVibrationStyle();
        notificationSlot.IsEnableBypassDnd();
        notificationSlot.EnableBypassDnd(enabled);
        notificationSlot.IsShowBadge();
        notificationSlot.EnableBadge(enabled);
        notificationSlot.SetEnable(enabled);
        notificationSlot.GetEnable();
        Parcel parcel;
        notificationSlot.ReadFromParcel(parcel);
        notificationSlot.Unmarshalling(parcel);
        return notificationSlot.Marshalling(parcel);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    char *ch = ParseData(data, size);
    if (ch != nullptr && size >= GetU32Size()) {
        OHOS::DoSomethingInterestingWithMyAPI(ch, size);
        free(ch);
        ch = nullptr;
    }
    return 0;
}
