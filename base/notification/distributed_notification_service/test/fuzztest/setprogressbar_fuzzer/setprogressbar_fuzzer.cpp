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

#include "setprogressbar_fuzzer.h"
#include "notification_request.h"

namespace OHOS {
    namespace {
        constexpr uint8_t ENABLE = 2;
        constexpr uint8_t SLOT_TYPE_NUM = 5;
    }
    bool DoSomethingInterestingWithMyAPI(const char* data, size_t size)
    {
        std::string stringData(data);
        int32_t notificationId = static_cast<int32_t>(GetU32Data(data));
        Notification::NotificationRequest request(notificationId);
        bool enabled = *data % ENABLE;
        request.SetIsAgentNotification(enabled);
        std::shared_ptr<Notification::MessageUser> messageUser = nullptr;
        request.AddMessageUser(messageUser);
        request.IsAlertOneTime();
        uint64_t deletedTime = 1;
        request.SetAutoDeletedTime(deletedTime);
        request.GetAutoDeletedTime();
        std::shared_ptr<Media::PixelMap> icon = nullptr;
        request.SetLittleIcon(icon);
        request.SetBigIcon(icon);
        request.GetClassification();
        request.GetColor();
        request.IsColorEnabled();
        request.IsCountdownTimer();
        request.GetGroupAlertType();
        request.IsGroupOverview();
        request.GetGroupName();
        request.IsOnlyLocal();
        request.SetOnlyLocal(enabled);
        request.SetSettingsText(stringData);
        request.GetSettingsText();
        request.GetCreateTime();
        request.IsShowStopwatch();
        request.SetShowStopwatch(enabled);
        uint8_t type = *data % SLOT_TYPE_NUM;
        Notification::NotificationConstant::SlotType slotType = Notification::NotificationConstant::SlotType(type);
        request.SetSlotType(slotType);
        request.GetSlotType();
        request.SetSortingKey(stringData);
        request.GetSortingKey();
        request.SetStatusBarText(stringData);
        request.GetStatusBarText();
        return request.IsTapDismissed();
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
