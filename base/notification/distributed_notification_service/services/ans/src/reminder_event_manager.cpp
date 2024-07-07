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

#include "reminder_event_manager.h"

#include "ans_log_wrapper.h"
#include "bundle_constants.h"
#include "bundle_mgr_interface.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

using namespace OHOS::EventFwk;
namespace OHOS {
namespace Notification {
ReminderEventManager::ReminderEventManager(std::shared_ptr<ReminderDataManager> &reminderDataManager)
{
    init(reminderDataManager);
}

void ReminderEventManager::init(std::shared_ptr<ReminderDataManager> &reminderDataManager) const
{
    MatchingSkills customMatchingSkills;
    customMatchingSkills.AddEvent(ReminderRequest::REMINDER_EVENT_ALARM_ALERT);
    customMatchingSkills.AddEvent(ReminderRequest::REMINDER_EVENT_ALERT_TIMEOUT);
    customMatchingSkills.AddEvent(ReminderRequest::REMINDER_EVENT_CLOSE_ALERT);
    customMatchingSkills.AddEvent(ReminderRequest::REMINDER_EVENT_SNOOZE_ALERT);
    customMatchingSkills.AddEvent(ReminderRequest::REMINDER_EVENT_REMOVE_NOTIFICATION);
    CommonEventSubscribeInfo customSubscriberInfo(customMatchingSkills);
    customSubscriberInfo.SetPermission("ohos.permission.GRANT_SENSITIVE_PERMISSIONS");
    auto customSubscriber = std::make_shared<ReminderEventCustomSubscriber>(customSubscriberInfo, reminderDataManager);

    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_BOOT_COMPLETED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_PACKAGE_DATA_CLEARED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_PACKAGE_RESTARTED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_TIMEZONE_CHANGED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_TIME_CHANGED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_USER_SWITCHED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_USER_REMOVED);
    CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    auto subscriber = std::make_shared<ReminderEventSubscriber>(subscriberInfo, reminderDataManager);

    std::string identity = IPCSkeleton::ResetCallingIdentity();
    if (CommonEventManager::SubscribeCommonEvent(subscriber) &&
        CommonEventManager::SubscribeCommonEvent(customSubscriber)) {
        ANSR_LOGD("SubscribeCommonEvent ok");
    } else {
        ANSR_LOGD("SubscribeCommonEvent fail");
    }
    IPCSkeleton::SetCallingIdentity(identity);

    sptr<SystemAbilityStatusChangeListener> statusChangeListener
        = new (std::nothrow) SystemAbilityStatusChangeListener(reminderDataManager);
    if (statusChangeListener == nullptr) {
        ANSR_LOGE("Failed to create statusChangeListener due to no memory.");
        return;
    }
    sptr<ISystemAbilityManager> samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        ANSR_LOGD("samgrProxy is null");
        return;
    }
    int32_t ret = samgrProxy->SubscribeSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, statusChangeListener);
    if (ret != ERR_OK) {
        ANSR_LOGE("subscribe system ability id: %{public}d failed", BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    }
}

ReminderEventManager::ReminderEventSubscriber::ReminderEventSubscriber(
    const CommonEventSubscribeInfo &subscriberInfo,
    std::shared_ptr<ReminderDataManager> &reminderDataManager) : CommonEventSubscriber(subscriberInfo)
{
    reminderDataManager_ = reminderDataManager;
}

ReminderEventManager::ReminderEventCustomSubscriber::ReminderEventCustomSubscriber(
    const CommonEventSubscribeInfo &subscriberInfo,
    std::shared_ptr<ReminderDataManager> &reminderDataManager) : CommonEventSubscriber(subscriberInfo)
{
    reminderDataManager_ = reminderDataManager;
}

void ReminderEventManager::ReminderEventCustomSubscriber::OnReceiveEvent(const EventFwk::CommonEventData &data)
{
    Want want = data.GetWant();
    std::string action = want.GetAction();
    ANSR_LOGI("Recieved common event:%{public}s", action.c_str());
    if (action == ReminderRequest::REMINDER_EVENT_ALARM_ALERT) {
        reminderDataManager_->ShowActiveReminder(want);
        return;
    }
    if (action == ReminderRequest::REMINDER_EVENT_ALERT_TIMEOUT) {
        reminderDataManager_->TerminateAlerting(want);
        return;
    }
    if (action == ReminderRequest::REMINDER_EVENT_CLOSE_ALERT) {
        reminderDataManager_->CloseReminder(want, true);
        return;
    }
    if (action == ReminderRequest::REMINDER_EVENT_SNOOZE_ALERT) {
        reminderDataManager_->SnoozeReminder(want);
        return;
    }
    if (action == ReminderRequest::REMINDER_EVENT_REMOVE_NOTIFICATION) {
        reminderDataManager_->CloseReminder(want, false);
        return;
    }
}

void ReminderEventManager::ReminderEventSubscriber::OnReceiveEvent(const EventFwk::CommonEventData &data)
{
    Want want = data.GetWant();
    std::string action = want.GetAction();
    ANSR_LOGI("Recieved common event:%{public}s", action.c_str());
    if (action == CommonEventSupport::COMMON_EVENT_BOOT_COMPLETED) {
        reminderDataManager_->Init(true);
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED) {
        HandlePackageRemove(want);
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_PACKAGE_DATA_CLEARED) {
        HandlePackageRemove(want);
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_PACKAGE_RESTARTED) {
        HandleProcessDied(want);
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_TIMEZONE_CHANGED) {
        reminderDataManager_->RefreshRemindersDueToSysTimeChange(ReminderDataManager::TIME_ZONE_CHANGE);
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_TIME_CHANGED) {
        reminderDataManager_->RefreshRemindersDueToSysTimeChange(ReminderDataManager::DATE_TIME_CHANGE);
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_USER_SWITCHED) {
        reminderDataManager_->OnUserSwitch(data.GetCode());
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_USER_REMOVED) {
        reminderDataManager_->OnUserRemove(data.GetCode());
        return;
    }
}

void ReminderEventManager::ReminderEventSubscriber::HandlePackageRemove(const EventFwk::Want &want) const
{
    OHOS::AppExecFwk::ElementName ele = want.GetElement();
    std::string bundleName = ele.GetBundleName();
    int32_t userId = want.GetIntParam(OHOS::AppExecFwk::Constants::USER_ID, -1);
    reminderDataManager_->CancelAllReminders(bundleName, userId);
}

void ReminderEventManager::ReminderEventSubscriber::HandleProcessDied(const EventFwk::Want &want) const
{
    sptr<NotificationBundleOption> bundleOption = GetBundleOption(want);
    if (bundleOption == nullptr) {
        ANSR_LOGE("Get bundle option error.");
        return;
    }
    reminderDataManager_->OnProcessDiedLocked(bundleOption);
}

sptr<NotificationBundleOption> ReminderEventManager::ReminderEventSubscriber::GetBundleOption(
    const OHOS::EventFwk::Want &want) const
{
    OHOS::AppExecFwk::ElementName ele = want.GetElement();
    std::string bundleName = ele.GetBundleName();
    int32_t userId = want.GetIntParam(OHOS::AppExecFwk::Constants::USER_ID, -1);
    int32_t uid = ReminderRequest::GetUid(userId, bundleName);
    ANSR_LOGD("bundleName=%{public}s, userId=%{public}d, uid=%{public}d", bundleName.c_str(), userId, uid);
    sptr<NotificationBundleOption> bundleOption = new (std::nothrow) NotificationBundleOption(bundleName, uid);
    if (bundleOption == nullptr) {
        ANSR_LOGE("new NotificationBundleOption fail due to no memory.");
    }
    return bundleOption;
}

ReminderEventManager::SystemAbilityStatusChangeListener::SystemAbilityStatusChangeListener(
    std::shared_ptr<ReminderDataManager> &reminderDataManager)
{
    reminderDataManager_ = reminderDataManager;
}

void ReminderEventManager::SystemAbilityStatusChangeListener::OnAddSystemAbility(
    int32_t systemAbilityId, const std::string& deviceId)
{
    ANSR_LOGD("OnAddSystemAbilityInner");
    reminderDataManager_->OnServiceStart();
}

void ReminderEventManager::SystemAbilityStatusChangeListener::OnRemoveSystemAbility(
    int32_t systemAbilityId, const std::string& deviceId)
{
    ANSR_LOGD("OnRemoveSystemAbilityInner");
}
}  // namespace OHOS
}  // namespace Notification
