/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "locator_background_proxy.h"

#include <thread>

#include "common_event_manager.h"
#include "common_event_support.h"
#include "iservice_registry.h"
#include "os_account_manager.h"
#include "system_ability_definition.h"

#include "common_utils.h"
#include "constant_definition.h"
#include "location_config_manager.h"
#include "location_log.h"
#include "locator_ability.h"
#include "request_manager.h"

namespace OHOS {
namespace Location {
std::mutex LocatorBackgroundProxy::requestListMutex_;
std::mutex LocatorBackgroundProxy::locatorMutex_;
LocatorBackgroundProxy::LocatorBackgroundProxy()
{
    InitArgsFromProp();
    if (!featureSwitch_) {
        return;
    }
    requestsMap_ = std::make_shared<std::map<int32_t, std::shared_ptr<std::list<std::shared_ptr<Request>>>>>();
    requestsList_ = std::make_shared<std::list<std::shared_ptr<Request>>>();
    CommonUtils::GetCurrentUserId(curUserId_);
    requestsMap_->insert(make_pair(curUserId_, requestsList_));

    auto requestConfig = std::make_unique<RequestConfig>();
    requestConfig->SetPriority(PRIORITY_LOW_POWER);
    requestConfig->SetTimeInterval(timeInterval_);
    callback_ = sptr<mLocatorCallback>(new (std::nothrow) LocatorBackgroundProxy::mLocatorCallback());
    if (callback_ == nullptr) {
        return;
    }
    request_ = std::make_shared<Request>();
    if (request_ == nullptr) {
        return;
    }
    request_->SetUid(SYSTEM_UID);
    request_->SetPid(getpid());
    request_->SetPackageName(PROC_NAME);
    request_->SetRequestConfig(*requestConfig);
    request_->SetLocatorCallBack(callback_);
    SubscribeSaStatusChangeListerner();
    isUserSwitchSubscribed_ = LocatorBackgroundProxy::UserSwitchSubscriber::Subscribe();
    proxySwtich_ = (LocationConfigManager::GetInstance().GetLocationSwitchState() == ENABLED);
    RegisterAppStateObserver();
}

LocatorBackgroundProxy::~LocatorBackgroundProxy()
{
    UnregisterAppStateObserver();
}

// modify the parameters, in order to make the test easier
void LocatorBackgroundProxy::InitArgsFromProp()
{
    featureSwitch_ = 1;
    timeInterval_ = DEFAULT_TIME_INTERVAL;
}

void LocatorBackgroundProxy::SubscribeSaStatusChangeListerner()
{
    OHOS::EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(OHOS::EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED);
    OHOS::EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    if (subscriber_ == nullptr) {
        subscriber_ = std::make_shared<UserSwitchSubscriber>(subscriberInfo);
    }
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    statusChangeListener_ = new (std::nothrow) SystemAbilityStatusChangeListener(subscriber_);
    if (samgrProxy == nullptr || statusChangeListener_ == nullptr) {
        LBSLOGE(LOCATOR_BACKGROUND_PROXY,
            "SubscribeSaStatusChangeListerner samgrProxy or statusChangeListener_ is nullptr");
        return;
    }
    int32_t ret = samgrProxy->SubscribeSystemAbility(COMMON_EVENT_SERVICE_ID, statusChangeListener_);
    LBSLOGI(LOCATOR_BACKGROUND_PROXY,
        "SubscribeSaStatusChangeListerner SubscribeSystemAbility COMMON_EVENT_SERVICE_ID result:%{public}d", ret);
}

void LocatorBackgroundProxy::StartLocatorThread()
{
    auto requestManager = DelayedSingleton<RequestManager>::GetInstance();
    auto locatorAbility = DelayedSingleton<LocatorAbility>::GetInstance();
    if (requestManager == nullptr || locatorAbility == nullptr) {
        LBSLOGE(LOCATOR, "StartLocatorThread: RequestManager or LocatorAbility is nullptr");
        return;
    }
    std::this_thread::sleep_for(std::chrono::seconds(timeInterval_));
    std::unique_lock<std::mutex> lock(locatorMutex_, std::defer_lock);
    lock.lock();
    isWating_ = false;
    if (isLocating_ || !proxySwtich_ || requestsList_->empty()) {
        LBSLOGD(LOCATOR_BACKGROUND_PROXY, "cancel locating");
        lock.unlock();
        return;
    }
    isLocating_ = true;
    lock.unlock();
    LBSLOGI(LOCATOR_BACKGROUND_PROXY, "real start locating");
    requestManager.get()->HandleStartLocating(request_);
    locatorAbility.get()->ReportLocationStatus(callback_, SESSION_START);
}

void LocatorBackgroundProxy::StopLocatorThread()
{
    auto locatorAbility = DelayedSingleton<LocatorAbility>::GetInstance();
    if (locatorAbility == nullptr) {
        LBSLOGE(LOCATOR, "StopLocatorThread: LocatorAbility is nullptr.");
        return;
    }
    std::unique_lock<std::mutex> lock(locatorMutex_, std::defer_lock);
    lock.lock();
    if (!isLocating_) {
        lock.unlock();
        return;
    }
    isLocating_ = false;
    lock.unlock();
    locatorAbility.get()->StopLocating(callback_);
    LBSLOGI(LOCATOR_BACKGROUND_PROXY, "end locating");
}

void LocatorBackgroundProxy::StopLocator()
{
    std::lock_guard lock(locatorMutex_);
    if (!isLocating_) {
        return;
    }
    std::thread th(&LocatorBackgroundProxy::StopLocatorThread, this);
    th.detach();
}

void LocatorBackgroundProxy::StartLocator()
{
    std::lock_guard lock(locatorMutex_);
    if (isLocating_ || !proxySwtich_ || isWating_) {
        return;
    }
    isWating_ = true;
    LBSLOGI(LOCATOR_BACKGROUND_PROXY, "start locating");
    std::thread th(&LocatorBackgroundProxy::StartLocatorThread, this);
    th.detach();
}

void LocatorBackgroundProxy::UpdateListOnRequestChange(const std::shared_ptr<Request>& request)
{
    if (request == nullptr) {
        LBSLOGE(LOCATOR_BACKGROUND_PROXY, "request is null");
        return;
    }
    int32_t uid = request->GetUid();
    std::string bundleName;
    if (!CommonUtils::GetBundleNameByUid(uid, bundleName)) {
        LBSLOGE(LOCATOR_BACKGROUND_PROXY, "Fail to Get bundle name: uid = %{public}d.", uid);
        return;
    }
    UpdateListOnSuspend(request, !IsAppBackground(bundleName));
    if (requestsList_->empty()) {
        StopLocator();
    } else {
        StartLocator();
    }
}

// called when the app freezes or wakes up
// When the app enters frozen state, start proxy
// when the app wakes up, stop proxy
void LocatorBackgroundProxy::OnSuspend(const std::shared_ptr<Request>& request, bool active)
{
    if (!featureSwitch_) {
        return;
    }
    if (!isUserSwitchSubscribed_) {
        isUserSwitchSubscribed_ = LocatorBackgroundProxy::UserSwitchSubscriber::Subscribe();
    }
    UpdateListOnSuspend(request, active);
    if (requestsList_->empty()) {
        StopLocator();
    } else {
        StartLocator();
    }
}

// called when SA switch on or switch off
// when switch on, start proxy
// when switch off, stop proxy
void LocatorBackgroundProxy::OnSaStateChange(bool enable)
{
    if (proxySwtich_ == enable || !featureSwitch_) {
        return;
    }
    LBSLOGD(LOCATOR_BACKGROUND_PROXY, "OnSaStateChange %{public}d", enable);
    proxySwtich_ = enable;
    if (enable && !requestsList_->empty()) {
        StartLocator();
    } else {
        StopLocator();
    }
}

// called when deleteRequest called from locator ability (e.g. app stop locating)
void LocatorBackgroundProxy::OnDeleteRequestRecord(const std::shared_ptr<Request>& request)
{
    if (!featureSwitch_) {
        return;
    }
    bool listEmpty = false;
    std::unique_lock<std::mutex> lock(requestListMutex_, std::defer_lock);
    lock.lock();
    auto it = find(requestsList_->begin(), requestsList_->end(), request);
    if (it != requestsList_->end()) {
        requestsList_->remove(request);
        if (requestsList_->empty()) {
            listEmpty = true;
        }
    }
    lock.unlock();
    if (listEmpty) {
        StopLocator();
    }
}

bool LocatorBackgroundProxy::CheckPermission(const std::shared_ptr<Request>& request) const
{
    uint32_t tokenId = request->GetTokenId();
    uint32_t firstTokenId = request->GetFirstTokenId();
    return ((CommonUtils::CheckLocationPermission(tokenId, firstTokenId) ||
            CommonUtils::CheckApproximatelyPermission(tokenId, firstTokenId)) &&
            CommonUtils::CheckBackgroundPermission(tokenId, firstTokenId));
}

void LocatorBackgroundProxy::UpdateListOnSuspend(const std::shared_ptr<Request>& request, bool active)
{
    if (request == nullptr) {
        return;
    }
    auto requestManager = DelayedSingleton<RequestManager>::GetInstance();
    if (requestManager == nullptr) {
        LBSLOGE(LOCATOR_BACKGROUND_PROXY, "UpdateListOnSuspend: RequestManager is nullptr.");
        return;
    }
    requestManager->UpdateUsingPermission(request);
    std::lock_guard lock(requestListMutex_);
    auto userId = GetUserId(request->GetUid());
    auto iter = requestsMap_->find(userId);
    if (iter == requestsMap_->end()) {
        return;
    }
    auto requestsList = iter->second;
    auto it = find(requestsList->begin(), requestsList->end(), request);
    if (it != requestsList->end()) {
        if (active || !CheckPermission(request)) {
            LBSLOGD(LOCATOR_BACKGROUND_PROXY, "remove request:%{public}s from User:%{public}d",
                request->ToString().c_str(), userId);
            requestsList->remove(request);
        }
    } else {
        if (request->GetRequestConfig() == nullptr) {
            return;
        }
        if (!active && CheckPermission(request) && request->GetRequestConfig()->GetFixNumber() == 0
            && CheckMaxRequestNum(request->GetUid(), request->GetPackageName())) {
            LBSLOGD(LOCATOR_BACKGROUND_PROXY, "add request:%{public}s from User:%{public}d",
                request->ToString().c_str(), userId);
            requestsList->push_back(request);
        }
    }
}

void LocatorBackgroundProxy::UpdateListOnUserSwitch(int32_t userId)
{
    std::lock_guard lock(requestListMutex_);
    auto iter = requestsMap_->find(userId);
    if (iter == requestsMap_->end()) {
        auto mRequestsList = std::make_shared<std::list<std::shared_ptr<Request>>>();
        requestsMap_->insert(make_pair(userId, mRequestsList));
        LBSLOGD(LOCATOR_BACKGROUND_PROXY, "add requsetlist on user:%{public}d", userId);
    }
    // if change to another user, proxy requestList should change
    requestsList_ = (*requestsMap_)[userId];
    curUserId_ = userId;
}


const std::list<std::shared_ptr<Request>>& LocatorBackgroundProxy::GetRequestsInProxy() const
{
    return *requestsList_;
}

// called in LocatorCallbackProxy::OnLocationReport
// check if callback is from proxy
bool LocatorBackgroundProxy::IsCallbackInProxy(const sptr<ILocatorCallback>& callback) const
{
    if (!featureSwitch_) {
        return false;
    }
    for (auto request : *requestsList_) {
        if (request->GetLocatorCallBack() == callback) {
            return true;
        }
    }
    return false;
}

int32_t LocatorBackgroundProxy::GetUserId(int32_t uid) const
{
    int userId = 0;
    AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(uid, userId);
    return userId;
}

void LocatorBackgroundProxy::OnUserSwitch(int32_t userId)
{
    UpdateListOnUserSwitch(userId);
    if (!requestsList_->empty()) {
        StartLocator();
    } else {
        LBSLOGD(LOCATOR_BACKGROUND_PROXY, "OnUserSwitch stoplocator");
        StopLocator();
    }
}

void LocatorBackgroundProxy::OnUserRemove(int32_t userId)
{
    // if user is removed, remove the requestList from the user in requestsMap
    std::lock_guard lock(requestListMutex_);
    auto iter = requestsMap_->find(userId);
    if (iter != requestsMap_->end()) {
        requestsMap_->erase(iter);
        LBSLOGD(LOCATOR_BACKGROUND_PROXY, "erase requsetlist on user:%{public}d", userId);
    }
}

// limit the number of requests per app
bool LocatorBackgroundProxy::CheckMaxRequestNum(pid_t uid, const std::string& packageName) const
{
    int32_t num = 0;
    auto iter = requestsMap_->find(GetUserId(uid));
    if (iter == requestsMap_->end()) {
        return false;
    }
    for (auto request : *(iter->second)) {
        if (request->GetUid() == uid && packageName.compare(request->GetPackageName()) == 0) {
            if (++num >= REQUESTS_NUM_MAX) {
                return false;
            }
        }
    }
    return true;
}

void LocatorBackgroundProxy::mLocatorCallback::OnLocationReport(const std::unique_ptr<Location>& location)
{
    LBSLOGD(LOCATOR_BACKGROUND_PROXY, "locator background OnLocationReport");
    auto locatorBackgroundProxy = DelayedSingleton<LocatorBackgroundProxy>::GetInstance();
    if (locatorBackgroundProxy == nullptr) {
        LBSLOGE(LOCATOR_BACKGROUND_PROXY, "OnLocationReport: LocatorBackgroundProxy is nullptr.");
        return;
    }
    auto requestsList = locatorBackgroundProxy.get()->GetRequestsInProxy();
    if (requestsList.empty()) {
        locatorBackgroundProxy->StopLocator();
        return;
    }
    // call the callback of each proxy app
    for (auto request : requestsList) {
        request->GetLocatorCallBack()->OnLocationReport(location);
    }
}

void LocatorBackgroundProxy::mLocatorCallback::OnLocatingStatusChange(const int status)
{
}

void LocatorBackgroundProxy::mLocatorCallback::OnErrorReport(const int errorCode)
{
}

LocatorBackgroundProxy::UserSwitchSubscriber::UserSwitchSubscriber(
    const OHOS::EventFwk::CommonEventSubscribeInfo &info)
    : CommonEventSubscriber(info)
{
    LBSLOGD(LOCATOR_BACKGROUND_PROXY, "create UserSwitchEventSubscriber");
}

void LocatorBackgroundProxy::UserSwitchSubscriber::OnReceiveEvent(const OHOS::EventFwk::CommonEventData& event)
{
    int32_t userId = event.GetCode();
    const auto action = event.GetWant().GetAction();
    auto locatorProxy = DelayedSingleton<LocatorBackgroundProxy>::GetInstance();
    if (locatorProxy == nullptr) {
        LBSLOGE(LOCATOR_BACKGROUND_PROXY, "OnReceiveEvent: LocatorBackgroundProxy is nullptr.");
        return;
    }
    LBSLOGD(LOCATOR_BACKGROUND_PROXY, "action = %{public}s, userId = %{public}d", action.c_str(), userId);
    if (action == OHOS::EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED) {
        locatorProxy.get()->OnUserSwitch(userId);
    } else if (action == OHOS::EventFwk::CommonEventSupport::COMMON_EVENT_USER_REMOVED) {
        locatorProxy.get()->OnUserRemove(userId);
    }
}

bool LocatorBackgroundProxy::UserSwitchSubscriber::Subscribe()
{
    LBSLOGD(LOCATOR_BACKGROUND_PROXY, "subscribe common event");
    OHOS::EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(OHOS::EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED);
    OHOS::EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<UserSwitchSubscriber> subscriber = std::make_shared<UserSwitchSubscriber>(subscriberInfo);
    bool result = OHOS::EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber);
    if (result) {
    } else {
        LBSLOGE(LOCATOR_BACKGROUND_PROXY, "Subscribe service event error.");
    }
    return result;
}

LocatorBackgroundProxy::SystemAbilityStatusChangeListener::SystemAbilityStatusChangeListener(
    std::shared_ptr<UserSwitchSubscriber> &subscriber) : subscriber_(subscriber)
{}

void LocatorBackgroundProxy::SystemAbilityStatusChangeListener::OnAddSystemAbility(
    int32_t systemAbilityId, const std::string& deviceId)
{
    if (systemAbilityId != COMMON_EVENT_SERVICE_ID) {
        LBSLOGE(LOCATOR_BACKGROUND_PROXY, "systemAbilityId is not COMMON_EVENT_SERVICE_ID");
        return;
    }
    if (subscriber_ == nullptr) {
        LBSLOGE(LOCATOR_BACKGROUND_PROXY, "OnAddSystemAbility subscribeer is nullptr");
        return;
    }
    bool result = OHOS::EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber_);
    LBSLOGI(LOCATOR_BACKGROUND_PROXY, "SubscribeCommonEvent subscriber_ result = %{public}d", result);
}

void LocatorBackgroundProxy::SystemAbilityStatusChangeListener::OnRemoveSystemAbility(
    int32_t systemAbilityId, const std::string& deviceId)
{
    if (systemAbilityId != COMMON_EVENT_SERVICE_ID) {
        LBSLOGE(LOCATOR_BACKGROUND_PROXY, "systemAbilityId is not COMMON_EVENT_SERVICE_ID");
        return;
    }
    if (subscriber_ == nullptr) {
        LBSLOGE(LOCATOR_BACKGROUND_PROXY, "OnRemoveSystemAbility subscribeer is nullptr");
        return;
    }
    bool result = OHOS::EventFwk::CommonEventManager::UnSubscribeCommonEvent(subscriber_);
    LBSLOGE(LOCATOR_BACKGROUND_PROXY, "UnSubscribeCommonEvent subscriber_ result = %{public}d", result);
}

bool LocatorBackgroundProxy::IsAppBackground(std::string bundleName)
{
    sptr<ISystemAbilityManager> samgrClient = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrClient == nullptr) {
        LBSLOGE(REQUEST_MANAGER, "Get system ability manager failed.");
        return false;
    }
    sptr<AppExecFwk::IAppMgr> iAppManager =
        iface_cast<AppExecFwk::IAppMgr>(samgrClient->GetSystemAbility(APP_MGR_SERVICE_ID));
    if (iAppManager == nullptr) {
        LBSLOGE(REQUEST_MANAGER, "Failed to get ability manager service.");
        return false;
    }
    std::vector<AppExecFwk::AppStateData> foregroundAppList;
    iAppManager->GetForegroundApplications(foregroundAppList);
    auto it = std::find_if(foregroundAppList.begin(), foregroundAppList.end(), [bundleName] (auto foregroundApp) {
        return bundleName.compare(foregroundApp.bundleName) == 0;
    });
    if (it != foregroundAppList.end()) {
        LBSLOGE(REQUEST_MANAGER, "app : %{public}s is foreground.", bundleName.c_str());
        return false;
    }
    return true;
}

bool LocatorBackgroundProxy::RegisterAppStateObserver()
{
    if (appStateObserver_ != nullptr) {
        LBSLOGI(REQUEST_MANAGER, "app state observer exist.");
        return true;
    }
    appStateObserver_ = sptr<AppStateChangeCallback>(new (std::nothrow) AppStateChangeCallback());
    sptr<ISystemAbilityManager> samgrClient = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrClient == nullptr) {
        LBSLOGE(REQUEST_MANAGER, "Get system ability manager failed.");
        appStateObserver_ = nullptr;
        return false;
    }
    iAppMgr_ = iface_cast<AppExecFwk::IAppMgr>(samgrClient->GetSystemAbility(APP_MGR_SERVICE_ID));
    if (iAppMgr_ == nullptr) {
        LBSLOGE(REQUEST_MANAGER, "Failed to get ability manager service.");
        appStateObserver_ = nullptr;
        return false;
    }
    int32_t result = iAppMgr_->RegisterApplicationStateObserver(appStateObserver_);
    if (result != 0) {
        LBSLOGE(REQUEST_MANAGER, "Failed to Register app state observer.");
        iAppMgr_ = nullptr;
        appStateObserver_ = nullptr;
        return false;
    }
    return true;
}

bool LocatorBackgroundProxy::UnregisterAppStateObserver()
{
    if (iAppMgr_ != nullptr && appStateObserver_ != nullptr) {
        iAppMgr_->UnregisterApplicationStateObserver(appStateObserver_);
    }
    iAppMgr_ = nullptr;
    appStateObserver_ = nullptr;
    return true;
}

AppStateChangeCallback::AppStateChangeCallback()
{
}

AppStateChangeCallback::~AppStateChangeCallback()
{
}

void AppStateChangeCallback::OnForegroundApplicationChanged(const AppExecFwk::AppStateData& appStateData)
{
    auto requestManager = DelayedSingleton<RequestManager>::GetInstance();
    if (requestManager == nullptr) {
        LBSLOGE(REQUEST_MANAGER, "OnForegroundApplicationChanged: RequestManager is nullptr.");
        return;
    }
    int32_t pid = appStateData.pid;
    int32_t uid = appStateData.uid;
    int32_t state = appStateData.state;
    LBSLOGI(REQUEST_MANAGER,
        "The state of App changed, uid = %{public}d, pid = %{public}d, state = %{public}d", uid, pid, state);
    requestManager->HandlePowerSuspendChanged(pid, uid, state);
}
} // namespace OHOS
} // namespace Location
