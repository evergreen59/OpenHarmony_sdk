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

#include "settemplate_fuzzer.h"
#define private public
#define protected public
#include "notification_request.h"
#undef private
#undef protected

namespace OHOS {
    namespace {
        constexpr uint8_t ENABLE = 2;
        constexpr uint8_t VISIBLENESS_TYPE_NUM = 4;
    }
    bool DoSomethingInterestingWithMyAPI(const char* data, size_t size)
    {
        std::string stringData(data);
        int32_t notificationId = static_cast<int32_t>(GetU32Data(data));
        Notification::NotificationRequest request(notificationId);
        bool enabled = *data % ENABLE;
        request.SetTapDismissed(enabled);
        uint8_t type = *data % VISIBLENESS_TYPE_NUM;
        Notification::NotificationConstant::VisiblenessType VisiblenessType =
            Notification::NotificationConstant::VisiblenessType(type);
        request.SetVisibleness(VisiblenessType);
        request.GetVisibleness();
        request.GetBadgeIconStyle();
        request.SetShortcutId(stringData);
        request.GetShortcutId();
        request.SetFloatingIcon(enabled);
        request.IsFloatingIcon();
        request.SetProgressBar(notificationId, notificationId, enabled);
        request.GetProgressMax();
        request.GetProgressValue();
        request.IsProgressIndeterminate();
        std::vector<std::string> text;
        text.emplace_back(stringData);
        request.SetNotificationUserInputHistory(text);
        request.GetNotificationUserInputHistory();
        request.GetNotificationHashCode();
        request.GetOwnerBundleName();
        request.GetCreatorBundleName();
        request.GetCreatorPid();
        request.SetCreatorUid(notificationId);
        request.GetCreatorUid();
        request.SetOwnerUid(notificationId);
        request.GetOwnerUid();
        request.GetLabel();
        request.SetDistributed(enabled);
        request.SetDevicesSupportDisplay(text);
        request.SetDevicesSupportOperate(text);
        request.GetNotificationDistributedOptions();
        request.SetCreatorUserId(notificationId);
        request.GetCreatorUserId();
        request.SetOwnerUserId(notificationId);
        request.GetOwnerUserId();
        request.GetNowSysTime();
        std::shared_ptr<Notification::NotificationTemplate> templ =
            std::make_shared<Notification::NotificationTemplate>();
        if (templ != nullptr) {
            templ->SetTemplateName(stringData);
        }
        request.SetTemplate(templ);
        request.GetTemplate();
        request.GetFlags();
        request.SetReceiverUserId(notificationId);
        return request.GetReceiverUserId();
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
