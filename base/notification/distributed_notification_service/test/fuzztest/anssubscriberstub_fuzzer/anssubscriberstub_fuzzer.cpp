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
#include "ans_subscriber_stub.h"
#undef private
#undef protected
#include "anssubscriberstub_fuzzer.h"
#include "notification_request.h"

namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const char* data, size_t size)
    {
        Notification::AnsSubscriberStub ansSubscriberStub;
        uint32_t code = GetU32Data(data);
        MessageParcel datas;
        MessageParcel reply;
        MessageOption flags;
        // test OnRemoteRequest function
        ansSubscriberStub.OnRemoteRequest(code, datas, reply, flags);
        // test HandleOnConnected function
        ansSubscriberStub.HandleOnConnected(datas, reply);
        // test HandleOnDisconnected function
        ansSubscriberStub.HandleOnDisconnected(datas, reply);
        // test HandleOnConsumed function
        ansSubscriberStub.HandleOnConsumed(datas, reply);
        // test HandleOnConsumedMap function
        ansSubscriberStub.HandleOnConsumedMap(datas, reply);
        // test HandleOnCanceledMap function
        ansSubscriberStub.HandleOnCanceledMap(datas, reply);
        // test HandleOnUpdated function
        ansSubscriberStub.HandleOnUpdated(datas, reply);
        // test HandleOnDoNotDisturbDateChange function
        ansSubscriberStub.HandleOnDoNotDisturbDateChange(datas, reply);
        // test HandleOnEnabledNotificationChanged function
        ansSubscriberStub.HandleOnEnabledNotificationChanged(datas, reply);
        // test OnConnected function
        ansSubscriberStub.OnConnected();
        // test OnDisconnected function
        ansSubscriberStub.OnDisconnected();
        // test OnConsumed function
        sptr<Notification::Notification> notification = new Notification::Notification();
        ansSubscriberStub.OnConsumed(notification);
        // test OnConsumed function
        sptr<Notification::NotificationSortingMap> notificationMap = new Notification::NotificationSortingMap();
        ansSubscriberStub.OnConsumed(notification, notificationMap);
        // test OnCanceled function
        int32_t deleteReason = 1;
        ansSubscriberStub.OnCanceled(notification, notificationMap, deleteReason);
        // test OnUpdated function
        ansSubscriberStub.OnUpdated(notificationMap);
        // test OnDoNotDisturbDateChange function
        sptr<Notification::NotificationDoNotDisturbDate> date = new Notification::NotificationDoNotDisturbDate();
        ansSubscriberStub.OnDoNotDisturbDateChange(date);
        // test OnEnabledNotificationChanged function
        sptr<Notification::EnabledNotificationCallbackData> callbackData = new Notification::EnabledNotificationCallbackData();
        return true;
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
