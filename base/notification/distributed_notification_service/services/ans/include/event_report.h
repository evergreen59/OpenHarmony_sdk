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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_ANS_INCLUDE_EVENT_REPORT_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_ANS_INCLUDE_EVENT_REPORT_H

#include <unordered_map>

#ifdef HAS_HISYSEVENT_PART
#include "hisysevent.h"
#endif

namespace OHOS {
namespace Notification {
namespace {
// event name
constexpr char SUBSCRIBE_ERROR[] = "SUBSCRIBE_ERROR";
constexpr char ENABLE_NOTIFICATION_ERROR[] = "ENABLE_NOTIFICATION_ERROR";
constexpr char ENABLE_NOTIFICATION_SLOT_ERROR[] = "ENABLE_NOTIFICATION_SLOT_ERROR";
constexpr char PUBLISH_ERROR[] = "PUBLISH_ERROR";
constexpr char FLOW_CONTROL_OCCUR[] = "FLOW_CONTROL_OCCUR";

constexpr char SUBSCRIBE[] = "SUBSCRIBE";
constexpr char UNSUBSCRIBE[] = "UNSUBSCRIBE";
constexpr char ENABLE_NOTIFICATION[] = "ENABLE_NOTIFICATION";
constexpr char ENABLE_NOTIFICATION_SLOT[] = "ENABLE_NOTIFICATION_SLOT";

constexpr char PUBLISH[] = "PUBLISH";
constexpr char CANCEL[] = "CANCEL";
constexpr char REMOVE[] = "REMOVE";
} // namespace

struct EventInfo {
    int32_t notificationId = 0;
    int32_t contentType = 0;
    int32_t userId = -1;
    int32_t pid = 0;
    int32_t uid = 0;
    int32_t slotType = 0;
    int32_t errCode = 0;
    bool enable = false;
    std::string bundleName;
    std::string notificationLabel;
};

class EventReport {
public:
    /**
     * @brief send hisysevent
     *
     * @param eventName event name, corresponding to the document 'hisysevent.yaml'
     * @param eventInfo event info
     */
    static void SendHiSysEvent(const std::string &eventName, const EventInfo &eventInfo);

private:
#ifdef HAS_HISYSEVENT_PART
    // fault event
    static void InnerSendSubscribeErrorEvent(const EventInfo &eventInfo);
    static void InnerSendEnableNotificationErrorEvent(const EventInfo &eventInfo);
    static void InnerSendEnableNotificationSlotErrorEvent(const EventInfo &eventInfo);
    static void InnerSendPublishErrorEvent(const EventInfo &eventInfo);
    static void InnerSendFlowControlOccurEvent(const EventInfo &eventInfo);

    // behavior event
    static void InnerSendSubscribeEvent(const EventInfo &eventInfo);
    static void InnerSendUnSubscribeEvent(const EventInfo &eventInfo);
    static void InnerSendEnableNotificationEvent(const EventInfo &eventInfo);
    static void InnerSendEnableNotificationSlotEvent(const EventInfo &eventInfo);

    // statistic event
    static void InnerSendPublishEvent(const EventInfo &eventInfo);
    static void InnerSendCancelEvent(const EventInfo &eventInfo);
    static void InnerSendRemoveEvent(const EventInfo &eventInfo);

    template<typename... Types>
    static void InnerEventWrite(const std::string &eventName,
        HiviewDFX::HiSysEvent::EventType type, Types... keyValues);

    static std::unordered_map<std::string, void (*)(const EventInfo &eventInfo)> ansSysEventFuncMap_;
#endif
};
} // namespace Notification
} // namespace OHOS
#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_ANS_INCLUDE_EVENT_REPORT_H