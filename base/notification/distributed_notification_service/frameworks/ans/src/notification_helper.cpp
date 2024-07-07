/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "notification_helper.h"
#include "ans_notification.h"
#include "singleton.h"

namespace OHOS {
namespace Notification {
ErrCode NotificationHelper::AddNotificationSlot(const NotificationSlot &slot)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->AddNotificationSlot(slot);
}

ErrCode NotificationHelper::AddSlotByType(const NotificationConstant::SlotType &slotType)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->AddSlotByType(slotType);
}

ErrCode NotificationHelper::AddNotificationSlots(const std::vector<NotificationSlot> &slots)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->AddNotificationSlots(slots);
}

ErrCode NotificationHelper::RemoveNotificationSlot(const NotificationConstant::SlotType &slotType)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->RemoveNotificationSlot(slotType);
}

ErrCode NotificationHelper::RemoveAllSlots()
{
    return DelayedSingleton<AnsNotification>::GetInstance()->RemoveAllSlots();
}

ErrCode NotificationHelper::GetNotificationSlot(
    const NotificationConstant::SlotType &slotType, sptr<NotificationSlot> &slot)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->GetNotificationSlot(slotType, slot);
}

ErrCode NotificationHelper::GetNotificationSlots(std::vector<sptr<NotificationSlot>> &slots)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->GetNotificationSlots(slots);
}

ErrCode NotificationHelper::GetNotificationSlotNumAsBundle(const NotificationBundleOption &bundleOption, uint64_t &num)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->GetNotificationSlotNumAsBundle(bundleOption, num);
}

ErrCode NotificationHelper::PublishNotification(const NotificationRequest &request)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->PublishNotification(request);
}

ErrCode NotificationHelper::PublishNotification(const std::string &label, const NotificationRequest &request)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->PublishNotification(label, request);
}

ErrCode NotificationHelper::PublishNotification(const NotificationRequest &request, const std::string &deviceId)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->PublishNotification(request, deviceId);
}

ErrCode NotificationHelper::CancelNotification(int32_t notificationId)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->CancelNotification(notificationId);
}

ErrCode NotificationHelper::CancelNotification(const std::string &label, int32_t notificationId)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->CancelNotification(label, notificationId);
}

ErrCode NotificationHelper::CancelAllNotifications()
{
    return DelayedSingleton<AnsNotification>::GetInstance()->CancelAllNotifications();
}

ErrCode NotificationHelper::CancelAsBundle(
    int32_t notificationId, const std::string &representativeBundle, int32_t userId)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->CancelAsBundle(
        notificationId, representativeBundle, userId);
}

ErrCode NotificationHelper::GetActiveNotificationNums(uint64_t &num)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->GetActiveNotificationNums(num);
}

ErrCode NotificationHelper::GetActiveNotifications(std::vector<sptr<NotificationRequest>> &request)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->GetActiveNotifications(request);
}

ErrCode NotificationHelper::GetCurrentAppSorting(sptr<NotificationSortingMap> &sortingMap)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->GetCurrentAppSorting(sortingMap);
}

ErrCode NotificationHelper::SetNotificationAgent(const std::string &agent)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->SetNotificationAgent(agent);
}

ErrCode NotificationHelper::GetNotificationAgent(std::string &agent)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->GetNotificationAgent(agent);
}

ErrCode NotificationHelper::CanPublishNotificationAsBundle(const std::string &representativeBundle, bool &canPublish)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->CanPublishNotificationAsBundle(
        representativeBundle, canPublish);
}

ErrCode NotificationHelper::PublishNotificationAsBundle(
    const std::string &representativeBundle, const NotificationRequest &request)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->PublishNotificationAsBundle(representativeBundle, request);
}

ErrCode NotificationHelper::SetNotificationBadgeNum()
{
    return DelayedSingleton<AnsNotification>::GetInstance()->SetNotificationBadgeNum();
}

ErrCode NotificationHelper::SetNotificationBadgeNum(int32_t num)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->SetNotificationBadgeNum(num);
}

ErrCode NotificationHelper::IsAllowedNotify(bool &allowed)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->IsAllowedNotify(allowed);
}

ErrCode NotificationHelper::IsAllowedNotifySelf(bool &allowed)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->IsAllowedNotifySelf(allowed);
}

ErrCode NotificationHelper::RequestEnableNotification(std::string &deviceId)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->RequestEnableNotification(deviceId);
}

ErrCode NotificationHelper::AreNotificationsSuspended(bool &suspended)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->AreNotificationsSuspended(suspended);
}

ErrCode NotificationHelper::HasNotificationPolicyAccessPermission(bool &hasPermission)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->HasNotificationPolicyAccessPermission(hasPermission);
}

ErrCode NotificationHelper::GetBundleImportance(NotificationSlot::NotificationLevel &importance)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->GetBundleImportance(importance);
}

ErrCode NotificationHelper::SubscribeNotification(const NotificationSubscriber &subscriber)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->SubscribeNotification(subscriber);
}

ErrCode NotificationHelper::SubscribeNotification(
    const NotificationSubscriber &subscriber, const NotificationSubscribeInfo &subscribeInfo)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->SubscribeNotification(subscriber, subscribeInfo);
}

ErrCode NotificationHelper::UnSubscribeNotification(NotificationSubscriber &subscriber)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->UnSubscribeNotification(subscriber);
}

ErrCode NotificationHelper::UnSubscribeNotification(
    NotificationSubscriber &subscriber, NotificationSubscribeInfo subscribeInfo)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->UnSubscribeNotification(subscriber, subscribeInfo);
}

ErrCode NotificationHelper::RemoveNotification(const std::string &key, int32_t removeReason)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->RemoveNotification(key, removeReason);
}

ErrCode NotificationHelper::RemoveNotification(const NotificationBundleOption &bundleOption,
    const int32_t notificationId, const std::string &label, int32_t removeReason)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->RemoveNotification(bundleOption,
        notificationId, label, removeReason);
}

ErrCode NotificationHelper::RemoveAllNotifications(const NotificationBundleOption &bundleOption)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->RemoveAllNotifications(bundleOption);
}

ErrCode NotificationHelper::RemoveNotificationsByBundle(const NotificationBundleOption &bundleOption)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->RemoveNotificationsByBundle(bundleOption);
}

ErrCode NotificationHelper::RemoveNotifications()
{
    return DelayedSingleton<AnsNotification>::GetInstance()->RemoveNotifications();
}

ErrCode NotificationHelper::GetNotificationSlotsForBundle(
    const NotificationBundleOption &bundleOption, std::vector<sptr<NotificationSlot>> &slots)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->GetNotificationSlotsForBundle(bundleOption, slots);
}

ErrCode NotificationHelper::UpdateNotificationSlots(
    const NotificationBundleOption &bundleOption, const std::vector<sptr<NotificationSlot>> &slots)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->UpdateNotificationSlots(bundleOption, slots);
}

ErrCode NotificationHelper::GetAllActiveNotifications(std::vector<sptr<Notification>> &notification)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->GetAllActiveNotifications(notification);
}

ErrCode NotificationHelper::GetAllActiveNotifications(
    const std::vector<std::string> key, std::vector<sptr<Notification>> &notification)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->GetAllActiveNotifications(key, notification);
}

ErrCode NotificationHelper::IsAllowedNotify(const NotificationBundleOption &bundleOption, bool &allowed)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->IsAllowedNotify(bundleOption, allowed);
}

ErrCode NotificationHelper::SetNotificationsEnabledForAllBundles(const std::string &deviceId, bool enabled)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->SetNotificationsEnabledForAllBundles(deviceId, enabled);
}

ErrCode NotificationHelper::SetNotificationsEnabledForDefaultBundle(const std::string &deviceId, bool enabled)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->SetNotificationsEnabledForDefaultBundle(deviceId, enabled);
}

ErrCode NotificationHelper::SetNotificationsEnabledForSpecifiedBundle(
    const NotificationBundleOption &bundleOption, std::string &deviceId, bool enabled)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->SetNotificationsEnabledForSpecifiedBundle(
        bundleOption, deviceId, enabled);
}

ErrCode NotificationHelper::SetShowBadgeEnabledForBundle(const NotificationBundleOption &bundleOption, bool enabled)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->SetShowBadgeEnabledForBundle(bundleOption, enabled);
}

ErrCode NotificationHelper::GetShowBadgeEnabledForBundle(const NotificationBundleOption &bundleOption, bool &enabled)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->GetShowBadgeEnabledForBundle(bundleOption, enabled);
}

ErrCode NotificationHelper::GetShowBadgeEnabled(bool &enabled)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->GetShowBadgeEnabled(enabled);
}

ErrCode NotificationHelper::CancelGroup(const std::string &groupName)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->CancelGroup(groupName);
}

ErrCode NotificationHelper::RemoveGroupByBundle(
    const NotificationBundleOption &bundleOption, const std::string &groupName)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->RemoveGroupByBundle(bundleOption, groupName);
}

ErrCode NotificationHelper::SetDoNotDisturbDate(const NotificationDoNotDisturbDate &doNotDisturbDate)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->SetDoNotDisturbDate(doNotDisturbDate);
}

ErrCode NotificationHelper::GetDoNotDisturbDate(NotificationDoNotDisturbDate &doNotDisturbDate)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->GetDoNotDisturbDate(doNotDisturbDate);
}

ErrCode NotificationHelper::DoesSupportDoNotDisturbMode(bool &doesSupport)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->DoesSupportDoNotDisturbMode(doesSupport);
}

ErrCode NotificationHelper::IsDistributedEnabled(bool &enabled)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->IsDistributedEnabled(enabled);
}

ErrCode NotificationHelper::EnableDistributed(const bool enabled)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->EnableDistributed(enabled);
}

ErrCode NotificationHelper::EnableDistributedByBundle(const NotificationBundleOption &bundleOption, const bool enabled)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->EnableDistributedByBundle(bundleOption, enabled);
}

ErrCode NotificationHelper::EnableDistributedSelf(const bool enabled)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->EnableDistributedSelf(enabled);
}

ErrCode NotificationHelper::IsDistributedEnableByBundle(const NotificationBundleOption &bundleOption, bool &enabled)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->IsDistributedEnableByBundle(bundleOption, enabled);
}

ErrCode NotificationHelper::GetDeviceRemindType(NotificationConstant::RemindType &remindType)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->GetDeviceRemindType(remindType);
}

ErrCode NotificationHelper::PublishContinuousTaskNotification(const NotificationRequest &request)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->PublishContinuousTaskNotification(request);
}

ErrCode NotificationHelper::CancelContinuousTaskNotification(const std::string &label, int32_t notificationId)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->CancelContinuousTaskNotification(label, notificationId);
}

ErrCode NotificationHelper::IsSupportTemplate(const std::string &templateName, bool &support)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->IsSupportTemplate(templateName, support);
}

ErrCode NotificationHelper::IsAllowedNotify(const int32_t &userId, bool &allowed)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->IsAllowedNotify(userId, allowed);
}

ErrCode NotificationHelper::SetNotificationsEnabledForAllBundles(const int32_t &userId, bool enabled)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->SetNotificationsEnabledForAllBundles(
        userId, enabled);
}

ErrCode NotificationHelper::RemoveNotifications(const int32_t &userId)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->RemoveNotifications(userId);
}

ErrCode NotificationHelper::SetDoNotDisturbDate(const int32_t &userId,
    const NotificationDoNotDisturbDate &doNotDisturbDate)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->SetDoNotDisturbDate(userId, doNotDisturbDate);
}

ErrCode NotificationHelper::GetDoNotDisturbDate(const int32_t &userId, NotificationDoNotDisturbDate &doNotDisturbDate)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->GetDoNotDisturbDate(userId, doNotDisturbDate);
}

ErrCode NotificationHelper::SetEnabledForBundleSlot(
    const NotificationBundleOption &bundleOption, const NotificationConstant::SlotType &slotType, bool enabled)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->SetEnabledForBundleSlot(bundleOption, slotType, enabled);
}

ErrCode NotificationHelper::GetEnabledForBundleSlot(
    const NotificationBundleOption &bundleOption, const NotificationConstant::SlotType &slotType, bool &enabled)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->GetEnabledForBundleSlot(bundleOption, slotType, enabled);
}

ErrCode NotificationHelper::SetSyncNotificationEnabledWithoutApp(const int32_t userId, const bool enabled)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->SetSyncNotificationEnabledWithoutApp(
        userId, enabled);
}

ErrCode NotificationHelper::GetSyncNotificationEnabledWithoutApp(const int32_t userId, bool &enabled)
{
    return DelayedSingleton<AnsNotification>::GetInstance()->GetSyncNotificationEnabledWithoutApp(
        userId, enabled);
}
}  // namespace Notification
}  // namespace OHOS