/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_FORM_FWK_FORM_SYS_EVENT_RECEIVER_H
#define OHOS_FORM_FWK_FORM_SYS_EVENT_RECEIVER_H

#include "common_event_subscriber.h"
#include "common_event_subscribe_info.h"
#include "form_event_handler.h"
#include "form_event_util.h"

namespace OHOS {
namespace AppExecFwk {
/**
 * @class FormSysEventReceiver
 * Receive system common event.
 */
class FormSysEventReceiver : public EventFwk::CommonEventSubscriber,
    public std::enable_shared_from_this<FormSysEventReceiver> {
public:
    FormSysEventReceiver() = default;
    FormSysEventReceiver(const EventFwk::CommonEventSubscribeInfo &subscriberInfo);
    virtual ~FormSysEventReceiver() = default;
    /**
     * @brief System common event receiver.
     * @param eventData Common event data.
     */
    virtual void OnReceiveEvent(const EventFwk::CommonEventData &eventData) override;

    /**
     * @brief SetEventHandler.
     * @param handler event handler
     */
    inline void SetEventHandler(const std::shared_ptr<FormEventHandler> &handler)
    {
        eventHandler_ = handler;
    }
private:
    void HandleUserIdRemoved(const int32_t userId); // multiuser
    bool IsSameForm(const FormRecord &record, const FormInfo &formInfo);
    void HandleBundleScanFinished(int32_t userId);
private:
    FormEventUtil formEventHelper_;
    std::shared_ptr<FormEventHandler> eventHandler_ = nullptr;
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif // OHOS_FORM_FWK_FORM_SYS_EVENT_RECEIVER_H
