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

#include "mock_bundle_mgr.h"
#include "ability_info.h"
#include "application_info.h"
#include "hilog_wrapper.h"
#include "ability_config.h"
using namespace OHOS::AAFwk;

namespace OHOS {
namespace AppExecFwk {
namespace {
const int32_t ERROR_USER_ID_U256 = 256;
}

int BundleMgrProxy::QueryWantAbility(
    const AAFwk::Want &__attribute__((unused)) want, std::vector<AbilityInfo> &__attribute__((unused)) abilityInfos)
{
    return 0;
}

bool BundleMgrProxy::QueryAbilityInfo(const AAFwk::Want &want, AbilityInfo &abilityInfo)
{
    ElementName eleName = want.GetElement();
    if (eleName.GetBundleName().empty()) {
        return false;
    }
    abilityInfo.visible = true;
    abilityInfo.name = eleName.GetAbilityName();
    abilityInfo.bundleName = eleName.GetBundleName();
    abilityInfo.applicationName = eleName.GetAbilityName() + "App";
    if (abilityInfo.bundleName != "com.ix.hiworld") {
        abilityInfo.applicationInfo.isLauncherApp = false;
    }
    return true;
}

bool BundleMgrProxy::GetBundleInfo(
    const std::string &bundleName, const BundleFlag flag, BundleInfo &bundleInfo, int32_t userId)
{
    return true;
}

bool BundleMgrProxy::GetApplicationInfo(
    const std::string &appName, const ApplicationFlag flag, const int userId, ApplicationInfo &appInfo)
{
    if (appName.empty()) {
        return false;
    }
    appInfo.name = "Helloworld";
    appInfo.bundleName = "com.ix.hiworld";
    return true;
}

int BundleMgrStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    return 0;
}

bool BundleMgrService::GetBundleInfo(
    const std::string &bundleName, const BundleFlag flag, BundleInfo &bundleInfo, int32_t userId)
{
    bundleInfo.uid = 0;
    return true;
}

bool BundleMgrService::QueryAbilityInfo(const AAFwk::Want &want, int32_t flags, int32_t userId,
    AbilityInfo &abilityInfo)
{
    auto flag = QueryAbilityInfo(want, abilityInfo);
    if (userId == ERROR_USER_ID_U256) {
        abilityInfo.applicationInfo.singleton = false;
    }
    return flag;
}

bool BundleMgrService::QueryAbilityInfo(const AAFwk::Want &want, AbilityInfo &abilityInfo)
{
    if (CheckWantEntity(want, abilityInfo)) {
        return true;
    }

    ElementName elementTemp = want.GetElement();
    std::string abilityNameTemp = elementTemp.GetAbilityName();
    std::string bundleNameTemp = elementTemp.GetBundleName();
    abilityInfo.deviceId = elementTemp.GetDeviceID();
    abilityInfo.visible = true;

    if (bundleNameTemp.empty() || abilityNameTemp.empty()) {
        return false;
    }

    auto fun = abilityInfoMap_.find(bundleNameTemp);
    if (fun != abilityInfoMap_.end()) {
        auto call = fun->second;
        if (call) {
            call(bundleNameTemp, abilityInfo, elementTemp);
            return true;
        }
    }
    if (std::string::npos != elementTemp.GetBundleName().find("Service")) {
        abilityInfo.type = AppExecFwk::AbilityType::SERVICE;
    }
    if (std::string::npos != elementTemp.GetBundleName().find("Data")) {
        abilityInfo.type = AppExecFwk::AbilityType::DATA;
    }
    if (std::string::npos != elementTemp.GetBundleName().find("Extension")) {
        abilityInfo.type = AppExecFwk::AbilityType::EXTENSION;
    }
    abilityInfo.name = elementTemp.GetAbilityName();
    abilityInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationName = elementTemp.GetBundleName();
    abilityInfo.deviceId = elementTemp.GetDeviceID();
    abilityInfo.applicationInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.name = "hello";
    if (want.HasEntity(Want::ENTITY_HOME) && want.GetAction() == Want::ACTION_HOME) {
        abilityInfo.applicationInfo.isLauncherApp = true;
    } else {
        abilityInfo.applicationInfo.isLauncherApp = false;
        abilityInfo.applicationInfo.iconPath = "icon path";
        abilityInfo.applicationInfo.label = "app label";
    }
    return true;
}

bool BundleMgrService::GetApplicationInfo(
    const std::string &appName, const ApplicationFlag flag, const int userId, ApplicationInfo &appInfo)
{
    appInfo.name = appName;
    appInfo.bundleName = appName;
    appInfo.uid = userId * BASE_USER_RANGE;
    return true;
}

bool BundleMgrService::CheckWantEntity(const AAFwk::Want &want, AbilityInfo &abilityInfo)
{
    auto entityVector = want.GetEntities();
    ElementName element = want.GetElement();

    auto find = false;
    // filter ams onstart
    for (const auto &entity : entityVector) {
        if (entity == Want::FLAG_HOME_INTENT_FROM_SYSTEM && element.GetAbilityName().empty() &&
            element.GetBundleName().empty()) {
            find = true;
            break;
        }
    }

    auto bundleName = element.GetBundleName();
    auto abilityName = element.GetAbilityName();
    if (find || (bundleName == AbilityConfig::SYSTEM_UI_BUNDLE_NAME &&
                    (abilityName == AbilityConfig::SYSTEM_UI_STATUS_BAR ||
                        abilityName == AbilityConfig::SYSTEM_UI_NAVIGATION_BAR))) {
        return true;
    }

    return false;
}

bool BundleMgrService::ImplicitQueryInfos(const Want &want, int32_t flags, int32_t userId,
    std::vector<AbilityInfo> &abilityInfos, std::vector<ExtensionAbilityInfo> &extensionInfos)
{
    if (want.GetAction() == "ohos.want.action.viewData") {
        auto num = want.GetIntParam("numMock", 0);
        if (num == 0) {
            return true;
        }
        for (auto i = 0 ; i < num; i++) {
            AbilityInfo info;
            int labelId = 1;
            int iconId = 1;
            info.name = "abilitySelector" + std::to_string(i);
            info.bundleName = "com.test.selector";
            info.labelId = labelId++;
            info.iconId = iconId++;
            abilityInfos.push_back(info);
        }
    }
    return true;
}
}  // namespace AppExecFwk
}  // namespace OHOS
