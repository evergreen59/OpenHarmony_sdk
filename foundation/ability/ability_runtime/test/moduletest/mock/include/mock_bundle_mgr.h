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

#ifndef MODULETEST_OHOS_ABILITY_RUNTIME_MOCK_BUNDLE_MANAGER_H
#define MODULETEST_OHOS_ABILITY_RUNTIME_MOCK_BUNDLE_MANAGER_H

#include <vector>
#include <gmock/gmock.h>
#include "ability_info.h"
#include "application_info.h"
#include "bundlemgr/bundle_mgr_interface.h"
#include "want.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string COM_IX_HIWORLD = "com.ix.hiworld";
const std::string COM_IX_HIMUSIC = "com.ix.hiMusic";
const std::string COM_IX_HIRADIO = "com.ix.hiRadio";
const std::string COM_IX_HISERVICE = "com.ix.hiService";
const std::string COM_IX_MUSICSERVICE = "com.ix.musicService";
const std::string COM_IX_HIDATA = "com.ix.hiData";
const std::string COM_IX_PHONE = "com.ix.hiPhone";
const std::string COM_IX_TV = "com.ix.hiTV";
const std::string COM_IX_Film = "com.ix.hiFilm";
const std::string COM_IX_HIBACKGROUNDMUSIC = "com.ix.hiBackgroundMusic";
const std::string COM_IX_HIBACKGROUNDDATA = "com.ix.hiBackgroundData";
const std::string COM_IX_HISINGLEMUSIC = "com.ix.hiSingleMusicInfo";
const std::string COM_OHOS_Test = "com.ohos.test";
const std::string COM_IX_HIACCOUNT = "com.ix.hiAccount";
const std::string COM_IX_HIACCOUNTSERVICE = "com.ix.hiAccountService";
constexpr int32_t MAX_SYS_UID = 2899;
const int32_t BASE_USER_RANGE = 200000;
const int32_t MOCK_MAIN_UID = 20000000;

auto HiWordInfo = [](std::string bundleName, AbilityInfo &abilityInfo, ElementName &elementTemp) {
    abilityInfo.name = elementTemp.GetAbilityName();
    abilityInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationName = "Helloworld";
    abilityInfo.applicationInfo.name = "Helloworld";
    abilityInfo.type = AbilityType::PAGE;
    abilityInfo.applicationInfo.isLauncherApp = true;

    if (elementTemp.GetAbilityName() == "luncher") {
        abilityInfo.configChanges.push_back("fontSize");
    }
    return true;
};

auto HiMusicInfo = [](std::string bundleName, AbilityInfo &abilityInfo, ElementName &elementTemp) {
    abilityInfo.name = elementTemp.GetAbilityName();
    abilityInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationName = "hiMusic";
    abilityInfo.applicationInfo.name = "hiMusic";
    abilityInfo.type = AbilityType::PAGE;
    abilityInfo.applicationInfo.isLauncherApp = false;

    if (elementTemp.GetAbilityName() == "MusicAbility") {
        abilityInfo.process = "p1";
        abilityInfo.launchMode = LaunchMode::STANDARD;
    }
    if (elementTemp.GetAbilityName() == "MusicTopAbility") {
        abilityInfo.process = "p1";
        abilityInfo.launchMode = LaunchMode::STANDARD;
    }
    if (elementTemp.GetAbilityName() == "MusicSAbility") {
        abilityInfo.process = "p2";
        abilityInfo.launchMode = LaunchMode::SINGLETON;
    }
    if (elementTemp.GetAbilityName() == "Music") {
        abilityInfo.process = "p1";
        abilityInfo.launchMode = LaunchMode::STANDARD;
        abilityInfo.configChanges.push_back("layout");
    }

    if (elementTemp.GetAbilityName() == "MusicTon") {
        abilityInfo.process = "p1";
        abilityInfo.launchMode = LaunchMode::SINGLETON;
        abilityInfo.configChanges.push_back("layout");
    }
    return true;
};

auto HiRadioInfo = [](std::string bundleName, AbilityInfo &abilityInfo, ElementName &elementTemp) {
    abilityInfo.name = elementTemp.GetAbilityName();
    abilityInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationName = "hiRadio";
    abilityInfo.applicationInfo.name = "hiRadio";
    abilityInfo.type = AbilityType::PAGE;
    abilityInfo.process = "p3";
    if (elementTemp.GetAbilityName() == "RadioAbility") {
        abilityInfo.launchMode = LaunchMode::STANDARD;
    }
    if (elementTemp.GetAbilityName() == "RadioTopAbility") {
        abilityInfo.launchMode = LaunchMode::SINGLETON;
    }
    if (elementTemp.GetAbilityName() == "Radio") {
        abilityInfo.launchMode = LaunchMode::STANDARD;
        abilityInfo.configChanges.push_back("orientation");
    }
    return true;
};

auto HiServiceInfo = [](std::string bundleName, AbilityInfo &abilityInfo, ElementName &elementTemp) {
    abilityInfo.name = elementTemp.GetAbilityName();
    abilityInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationName = "hiService";
    abilityInfo.applicationInfo.name = "hiService";
    abilityInfo.type = AbilityType::SERVICE;
    abilityInfo.process = "p4";
    return true;
};

auto MusicServiceInfo = [](std::string bundleName, AbilityInfo &abilityInfo, ElementName &elementTemp) {
    abilityInfo.name = elementTemp.GetAbilityName();
    abilityInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationName = "musicService";
    abilityInfo.applicationInfo.name = "musicService";
    abilityInfo.type = AbilityType::SERVICE;
    abilityInfo.process = "p5";
    return true;
};

auto HiDataInfo = [](std::string bundleName, AbilityInfo &abilityInfo, ElementName &elementTemp) {
    abilityInfo.name = elementTemp.GetAbilityName();
    abilityInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationName = "hiData";
    abilityInfo.applicationInfo.name = "hiData";
    abilityInfo.type = AbilityType::DATA;
    abilityInfo.process = "p6";
    return true;
};

auto HiPhoneInfo = [](std::string bundleName, AbilityInfo &abilityInfo, ElementName &elementTemp) {
    abilityInfo.name = elementTemp.GetAbilityName();
    abilityInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationName = "hiPhone";
    abilityInfo.applicationInfo.name = "hiPhone";
    abilityInfo.type = AbilityType::PAGE;
    abilityInfo.applicationInfo.isLauncherApp = false;
    abilityInfo.process = "p7";
    if (elementTemp.GetAbilityName() == "PhoneAbility1") {
        abilityInfo.launchMode = LaunchMode::SINGLETON;
    }
    if (elementTemp.GetAbilityName() == "PhoneAbility2") {
        abilityInfo.launchMode = LaunchMode::SINGLETON;
    }
    return true;
};

auto HiTVInfo = [](std::string bundleName, AbilityInfo &abilityInfo, ElementName &elementTemp) {
    abilityInfo.name = elementTemp.GetAbilityName();
    abilityInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationName = "hiTV";
    abilityInfo.applicationInfo.name = "hiTV";
    abilityInfo.type = AbilityType::PAGE;
    abilityInfo.applicationInfo.isLauncherApp = false;
    if (elementTemp.GetAbilityName() == "TVAbility") {
        abilityInfo.process = "p8";
        abilityInfo.launchMode = LaunchMode::SINGLETON;
    }
    return true;
};

auto HiFilmInfo = [](std::string bundleName, AbilityInfo &abilityInfo, ElementName &elementTemp) {
    abilityInfo.name = elementTemp.GetAbilityName();
    abilityInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationName = "hiFilm";
    abilityInfo.applicationInfo.name = "hiFilm";
    abilityInfo.type = AbilityType::PAGE;
    abilityInfo.applicationInfo.isLauncherApp = false;
    if (elementTemp.GetAbilityName() == "FilmAbility") {
        abilityInfo.process = "p9";
        abilityInfo.launchMode = LaunchMode::SINGLETON;
    }
    return true;
};
auto HiBAckgroundMusicInfo = [](std::string bundleName, AbilityInfo &abilityInfo, ElementName &elementTemp) {
    abilityInfo.name = elementTemp.GetAbilityName();
    abilityInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.singleton = true;
    abilityInfo.applicationName = "hiBackgroundMusic";
    abilityInfo.applicationInfo.name = "hiBackgroundMusic";
    abilityInfo.type = AbilityType::SERVICE;
    abilityInfo.process = "p4";
    return true;
};

auto HiBAckgroundDataInfo = [](std::string bundleName, AbilityInfo &abilityInfo, ElementName &elementTemp) {
    abilityInfo.name = elementTemp.GetAbilityName();
    abilityInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.singleton = true;
    abilityInfo.applicationName = "hiBackgroundData";
    abilityInfo.applicationInfo.name = "hiBackgroundData";
    abilityInfo.type = AbilityType::SERVICE;
    abilityInfo.process = "p4";
    return true;
};

auto HiSingleMusicInfo = [](std::string bundleName, AbilityInfo &abilityInfo, ElementName &elementTemp) {
    abilityInfo.name = elementTemp.GetAbilityName();
    abilityInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.singleton = true;
    abilityInfo.applicationName = "hiSingleMusic";
    abilityInfo.applicationInfo.name = "hiSingleMusic";
    abilityInfo.type = AbilityType::PAGE;
    abilityInfo.process = "p3";
    if (elementTemp.GetAbilityName() == "SingleMusicAbility") {
        abilityInfo.launchMode = LaunchMode::STANDARD;
    }
    if (elementTemp.GetAbilityName() == "SingleMusicTopAbility") {
        abilityInfo.launchMode = LaunchMode::SINGLETON;
    }
    return true;
};

auto TestInfo = [](std::string bundleName, AbilityInfo &abilityInfo, ElementName &elementTemp) {
    abilityInfo.name = elementTemp.GetAbilityName();
    abilityInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationName = "com.ohos.test";
    abilityInfo.applicationInfo.name = "com.ohos.test";
    abilityInfo.moduleName = ".MyApplication";
    abilityInfo.package = "com.ohos.test";
    abilityInfo.type = AbilityType::PAGE;
    abilityInfo.applicationInfo.isLauncherApp = false;
    abilityInfo.kind = "page";
    abilityInfo.process = "p10";
    abilityInfo.labelId = 10001;
    abilityInfo.label = "$string:label";
    abilityInfo.descriptionId = 10002;
    abilityInfo.description = "$string:mainability_description";
    abilityInfo.iconId = 10003;
    abilityInfo.iconPath = "$media:icon";
    abilityInfo.theme = "mytheme";
    abilityInfo.readPermission = "readPermission";
    abilityInfo.writePermission = "writePermission";
    abilityInfo.resourcePath = "/data/app/com.ohos.test";
    abilityInfo.srcPath = "/resources/base/profile";
    abilityInfo.srcLanguage = "C++";
    abilityInfo.isLauncherAbility = false;
    abilityInfo.isNativeAbility = false;
    abilityInfo.enabled = false;
    abilityInfo.supportPipMode = false;
    abilityInfo.formEnabled = false;
    return true;
};
auto HiAccountInfo = [](std::string bundleName, AbilityInfo &abilityInfo, ElementName &elementTemp) {
    abilityInfo.name = elementTemp.GetAbilityName();
    abilityInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.uid = MOCK_MAIN_UID;
    abilityInfo.applicationName = "AccountTest";
    abilityInfo.applicationInfo.name = "AccountTest";
    abilityInfo.type = AbilityType::PAGE;
    abilityInfo.applicationInfo.isLauncherApp = true;
    return true;
};

auto HiAccountServiceInfo = [](std::string bundleName, AbilityInfo &abilityInfo, ElementName &elementTemp) {
    abilityInfo.name = elementTemp.GetAbilityName();
    abilityInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.bundleName = elementTemp.GetBundleName();
    abilityInfo.applicationInfo.uid = MOCK_MAIN_UID;
    abilityInfo.applicationName = "AccountServiceTest";
    abilityInfo.applicationInfo.name = "AccountServiceTest";
    abilityInfo.type = AbilityType::SERVICE;
    abilityInfo.applicationInfo.isLauncherApp = true;
    return true;
};
}  // namespace
class BundleMgrProxy : public IRemoteProxy<IBundleMgr> {
public:
    explicit BundleMgrProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IBundleMgr>(impl)
    {}

    int QueryWantAbility(const AAFwk::Want &want, std::vector<AbilityInfo> &abilityInfos);
    bool QueryAbilityInfo(const AAFwk::Want &want, AbilityInfo &abilityInfo) override;
    bool GetApplicationInfo(
        const std::string &appName, const ApplicationFlag flag, const int userId, ApplicationInfo &appInfo) override;
    bool GetBundleInfo(
        const std::string &bundleName, const BundleFlag flag, BundleInfo &bundleInfo, int32_t userId) override;
};

class BundleMgrStub : public IRemoteStub<IBundleMgr> {
public:
    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
};

class BundleMgrService : public BundleMgrStub {
public:
    bool GetBundleInfo(
        const std::string &bundleName, const BundleFlag flag, BundleInfo &bundleInfo, int32_t userId) override;
    bool QueryAbilityInfo(const AAFwk::Want &want, AbilityInfo &abilityInfo) override;
    bool QueryAbilityInfo(const AAFwk::Want &want, int32_t flags, int32_t userId, AbilityInfo &abilityInfo) override;

    bool GetApplicationInfo(
        const std::string &appName, const ApplicationFlag flag, const int userId, ApplicationInfo &appInfo) override;
    virtual bool GetBundleGidsByUid(const std::string &bundleName, const int &uid, std::vector<int> &gids) override
    {
        return true;
    }

    bool ImplicitQueryInfos(const Want &want, int32_t flags, int32_t userId,
        std::vector<AbilityInfo> &abilityInfos, std::vector<ExtensionAbilityInfo> &extensionInfos) override;

    BundleMgrService()
    {
        abilityInfoMap_.emplace(COM_IX_HIWORLD, HiWordInfo);
        abilityInfoMap_.emplace(COM_IX_HIMUSIC, HiMusicInfo);
        abilityInfoMap_.emplace(COM_IX_HIRADIO, HiRadioInfo);
        abilityInfoMap_.emplace(COM_IX_HISERVICE, HiServiceInfo);
        abilityInfoMap_.emplace(COM_IX_MUSICSERVICE, MusicServiceInfo);
        abilityInfoMap_.emplace(COM_IX_HIDATA, HiDataInfo);
        abilityInfoMap_.emplace(COM_IX_PHONE, HiPhoneInfo);
        abilityInfoMap_.emplace(COM_IX_TV, HiTVInfo);
        abilityInfoMap_.emplace(COM_IX_Film, HiFilmInfo);
        abilityInfoMap_.emplace(COM_IX_HIBACKGROUNDMUSIC, HiBAckgroundMusicInfo);
        abilityInfoMap_.emplace(COM_IX_HIBACKGROUNDDATA, HiBAckgroundDataInfo);
        abilityInfoMap_.emplace(COM_IX_HISINGLEMUSIC, HiSingleMusicInfo);
        abilityInfoMap_.emplace(COM_OHOS_Test, TestInfo);
        abilityInfoMap_.emplace(COM_IX_HIACCOUNT, HiAccountInfo);
        abilityInfoMap_.emplace(COM_IX_HIACCOUNTSERVICE, HiAccountServiceInfo);
    }

    virtual ~BundleMgrService()
    {}

    bool CheckWantEntity(const AAFwk::Want &, AbilityInfo &);

public:
    using QueryAbilityInfoFunType =
        std::function<bool(std::string bundleName, AbilityInfo &abilityInfo, ElementName &elementTemp)>;
    std::map<std::string, QueryAbilityInfoFunType> abilityInfoMap_;
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // MODULETEST_OHOS_ABILITY_RUNTIME_MOCK_BUNDLE_MANAGER_H
