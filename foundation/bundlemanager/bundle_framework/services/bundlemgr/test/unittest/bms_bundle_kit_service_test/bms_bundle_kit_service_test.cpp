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

#define private public
#define protected public

#include <chrono>
#include <fstream>
#include <thread>
#include <gtest/gtest.h>

#include "ability_manager_client.h"
#include "ability_info.h"
#include "bms_device_manager.h"
#include "bundle_data_mgr.h"
#include "bundle_info.h"
#include "bundle_permission_mgr.h"
#include "bundle_mgr_service.h"
#include "bundle_mgr_service_event_handler.h"
#include "bundle_mgr_host.h"
#include "bundle_mgr_proxy.h"
#include "bundle_status_callback_proxy.h"
#include "bundle_stream_installer_host_impl.h"
#include "clean_cache_callback_proxy.h"
#include "directory_ex.h"
#include "hidump_helper.h"
#include "install_param.h"
#include "extension_ability_info.h"
#include "installd/installd_service.h"
#include "installd_client.h"
#include "inner_bundle_info.h"
#include "launcher_service.h"
#include "mock_clean_cache.h"
#include "mock_bundle_status.h"
#include "nlohmann/json.hpp"
#include "perf_profile.h"
#include "service_control.h"
#include "system_ability_helper.h"
#include "want.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;
using OHOS::AAFwk::Want;

namespace OHOS {
namespace {
const std::string BUNDLE_NAME_TEST = "com.example.bundlekit.test";
const std::string MODULE_NAME_TEST = "com.example.bundlekit.test.entry";
const std::string MODULE_NAME_TEST_1 = "com.example.bundlekit.test.entry_A";
const std::string MODULE_NAME_TEST_2 = "com.example.bundlekit.test.entry_B";
const std::string MODULE_NAME_TEST_3 = "com.example.bundlekit.test.entry_C";
const std::string ABILITY_NAME_TEST1 = ".Reading1";
const std::string ABILITY_NAME_TEST = ".Reading";
const int32_t BASE_TEST_UID = 65535;
const int32_t TEST_UID = 20065535;
const std::string BUNDLE_LABEL = "Hello, OHOS";
const std::string BUNDLE_DESCRIPTION = "example helloworld";
const std::string BUNDLE_VENDOR = "example";
const std::string BUNDLE_VERSION_NAME = "1.0.0.1";
const std::string BUNDLE_MAIN_ABILITY = "com.example.bundlekit.test.entry";
const int32_t BUNDLE_MAX_SDK_VERSION = 0;
const int32_t BUNDLE_MIN_SDK_VERSION = 0;
const std::string BUNDLE_JOINT_USERID = "3";
const uint32_t BUNDLE_VERSION_CODE = 1001;
const std::string BUNDLE_NAME_TEST1 = "com.example.bundlekit.test1";
const std::string BUNDLE_NAME_DEMO = "com.example.bundlekit.demo";
const std::string MODULE_NAME_DEMO = "com.example.bundlekit.demo.entry";
const std::string ABILITY_NAME_DEMO = ".Writing";
const int DEMO_UID = 30001;
const std::string PACKAGE_NAME = "com.example.bundlekit.test.entry";
const std::string PROCESS_TEST = "test.process";
const std::string DEVICE_ID = "PHONE-001";
const int APPLICATION_INFO_FLAGS = 1;
const int DEFAULT_USER_ID_TEST = 100;
const int NEW_USER_ID_TEST = 200;
const std::string LABEL = "hello";
const std::string DESCRIPTION = "mainEntry";
const std::string THEME = "mytheme";
const std::string ICON_PATH = "/data/data/icon.png";
const std::string KIND = "test";
const std::string ACTION = "action.system.home";
const std::string ENTITY = "entity.system.home";
const std::string TARGET_ABILITY = "MockTargetAbility";
const AbilityType ABILITY_TYPE = AbilityType::PAGE;
const DisplayOrientation ORIENTATION = DisplayOrientation::PORTRAIT;
const LaunchMode LAUNCH_MODE = LaunchMode::SINGLETON;
const uint32_t FORM_ENTITY = 1;
const uint32_t BACKGROUND_MODES = 1;
const std::vector<std::string> CONFIG_CHANGES = {"locale"};
const int DEFAULT_FORM_HEIGHT = 100;
const int DEFAULT_FORM_WIDTH = 200;
const std::string META_DATA_DESCRIPTION = "description";
const std::string META_DATA_NAME = "name";
const std::string META_DATA_TYPE = "type";
const std::string META_DATA_VALUE = "value";
const std::string META_DATA_EXTRA = "extra";
const ModuleColorMode COLOR_MODE = ModuleColorMode::AUTO;
const std::string CODE_PATH = "/data/app/el1/bundle/public/com.example.bundlekit.test";
const std::string RESOURCE_PATH = "/data/app/el1/bundle/public/com.example.bundlekit.test/res";
const std::string LIB_PATH = "/data/app/el1/bundle/public/com.example.bundlekit.test/lib";
const bool VISIBLE = true;
const std::string MAIN_ENTRY = "com.example.bundlekit.test.entry";
const uint32_t ABILITY_SIZE_ZERO = 0;
const uint32_t ABILITY_SIZE_ONE = 1;
const uint32_t PERMISSION_SIZE_ZERO = 0;
const uint32_t PERMISSION_SIZE_TWO = 2;
const uint32_t META_DATA_SIZE_ONE = 1;
const uint32_t BUNDLE_NAMES_SIZE_ZERO = 0;
const uint32_t BUNDLE_NAMES_SIZE_ONE = 1;
const uint32_t MODULE_NAMES_SIZE_ONE = 1;
const uint32_t MODULE_NAMES_SIZE_TWO = 2;
const uint32_t MODULE_NAMES_SIZE_THREE = 3;
const std::string EMPTY_STRING = "";
const int INVALID_UID = -1;
const std::string ABILITY_URI = "dataability:///com.example.hiworld.himusic.UserADataAbility/person/10";
const std::string URI = "dataability://com.example.hiworld.himusic.UserADataAbility";
const std::string ERROR_URI = "dataability://";
const std::string HAP_FILE_PATH = "/data/test/resource/bms/bundle_kit/test.hap";
const std::string HAP_FILE_PATH1 = "/data/test/resource/bms/bundle_kit/test1.hap";
const std::string ERROR_HAP_FILE_PATH = "/data/test/resource/bms/bundle_kit/error.hap";
const std::string META_DATA = "name";
const std::string ERROR_META_DATA = "String";
const std::string BUNDLE_DATA_DIR = "/data/app/el2/100/base/com.example.bundlekit.test";
const std::string FILES_DIR = "/data/app/el2/100/base/com.example.bundlekit.test/files";
const std::string TEST_FILE_DIR = "/data/app/el2/100/base/com.example.bundlekit.test/files";
const std::string DATA_BASE_DIR = "/data/app/el2/100/database/com.example.bundlekit.test";
const std::string TEST_DATA_BASE_DIR = "/data/app/el2/100/database/com.example.bundlekit.test";
const std::string CACHE_DIR = "/data/app/el2/100/base/com.example.bundlekit.test/cache";
const std::string TEST_CACHE_DIR = "/data/app/el2/100/base/com.example.bundlekit.test/cache/cache";
const std::string SERVICES_NAME = "d-bms";
const std::string FORM_NAME = "form_js";
const std::string FORM_PATH = "data/app";
const std::string FORM_JS_COMPONENT_NAME = "JS";
const std::string FORM_DESCRIPTION = "description";
const std::string FORM_SCHEDULED_UPDATE_TIME = "11:00";
const std::string FORM_CUSTOMIZE_DATAS_NAME = "customizeDataName";
const std::string FORM_CUSTOMIZE_DATAS_VALUE = "customizeDataValue";
const std::string FORM_PORTRAIT_LAYOUTS1 = "port1";
const std::string FORM_PORTRAIT_LAYOUTS2 = "port2";
const std::string FORM_LANDSCAPE_LAYOUTS1 = "land1";
const std::string FORM_LANDSCAPE_LAYOUTS2 = "land2";
const std::string FORM_SRC = "page/card/index";
constexpr int32_t FORM_JS_WINDOW_DESIGNWIDTH = 720;
const std::string SHORTCUT_TEST_ID = "shortcutTestId";
const std::string SHORTCUT_DEMO_ID = "shortcutDemoId";
const std::string SHORTCUT_HOST_ABILITY = "hostAbility";
const std::string SHORTCUT_ICON = "/data/test/bms_bundle";
const std::string SHORTCUT_LABEL = "shortcutLabel";
const std::string SHORTCUT_DISABLE_MESSAGE = "shortcutDisableMessage";
const std::string SHORTCUT_INTENTS_TARGET_BUNDLE = "targetBundle";
const std::string SHORTCUT_INTENTS_TARGET_MODULE = "targetModule";
const std::string SHORTCUT_INTENTS_TARGET_CLASS = "targetClass";
const std::string COMMON_EVENT_NAME = ".MainAbililty";
const std::string COMMON_EVENT_PERMISSION = "permission";
const std::string COMMON_EVENT_DATA = "data";
const std::string COMMON_EVENT_TYPE = "type";
const std::string COMMON_EVENT_EVENT = "usual.event.PACKAGE_ADDED";
const std::string COMMON_EVENT_EVENT_ERROR_KEY = "usual.event.PACKAGE_ADDED_D";
const std::string COMMON_EVENT_EVENT_NOT_EXISTS_KEY = "usual.event.PACKAGE_REMOVED";
const int FORMINFO_DESCRIPTIONID = 123;
const std::string ACTION_001 = "action001";
const std::string ACTION_002 = "action002";
const std::string ENTITY_001 = "entity001";
const std::string ENTITY_002 = "entity002";
const std::string TYPE_001 = "type001";
const std::string TYPE_002 = "type002";
const std::string TYPE_IMG_REGEX = "img/*";
const std::string TYPE_IMG_JPEG = "img/jpeg";
const std::string SCHEME_SEPARATOR = "://";
const std::string PORT_SEPARATOR = ":";
const std::string PATH_SEPARATOR = "/";
const std::string SCHEME_001 = "scheme001";
const std::string SCHEME_002 = "scheme002";
const std::string HOST_001 = "host001";
const std::string HOST_002 = "host002";
const std::string PORT_001 = "port001";
const std::string PORT_002 = "port002";
const std::string PATH_001 = "path001";
const std::string PATH_REGEX_001 = ".*";
const std::string CONTROLMESSAGE = "controlMessage";
const std::string URI_PATH_001 = SCHEME_001 + SCHEME_SEPARATOR + HOST_001 +
    PORT_SEPARATOR + PORT_001 + PATH_SEPARATOR + PATH_001;
const std::string URI_PATH_DUPLICATE_001 = SCHEME_001 + SCHEME_SEPARATOR +
    HOST_001 + PORT_SEPARATOR + PORT_001 + PATH_SEPARATOR + PATH_001 + PATH_001;
const std::string URI_PATH_REGEX_001 = SCHEME_001 + SCHEME_SEPARATOR + HOST_001 +
    PORT_SEPARATOR + PORT_001 + PATH_SEPARATOR + PATH_REGEX_001;
const int32_t DEFAULT_USERID = 100;
const int32_t ALL_USERID = -3;
const int32_t WAIT_TIME = 5; // init mocked bms
constexpr int32_t DISPOSED_STATUS = 10;
const int32_t ICON_ID = 16777258;
const int32_t LABEL_ID = 16777257;
const std::string BUNDLE_NAME = "bundleName";
const std::string MODULE_NAME = "moduleName";
const std::string ABILITY_NAME = "abilityName";
const std::string SHORTCUT_ID_KEY = "shortcutId";
const std::string ICON_KEY = "icon";
const std::string ICON_ID_KEY = "iconId";
const std::string LABEL_KEY = "label";
const std::string LABEL_ID_KEY = "labelId";
const std::string SHORTCUT_WANTS_KEY = "wants";
const std::string SHORTCUTS_KEY = "shortcuts";
const size_t ZERO = 0;
}  // namespace

class BmsBundleKitServiceTest : public testing::Test {
public:
    using Want = OHOS::AAFwk::Want;
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::shared_ptr<BundleDataMgr> GetBundleDataMgr() const;
    std::shared_ptr<BundleDistributedManager> GetBundleDistributedManager() const;
    static sptr<BundleMgrProxy> GetBundleMgrProxy();
    std::shared_ptr<LauncherService> GetLauncherService() const;
    void MockInnerBundleInfo(const std::string &bundleName, const std::string &moduleName,
        const std::string &abilityName, const std::vector<Dependency> &dependencies,
        InnerBundleInfo &innerBundleInfo) const;
    void MockInstallBundle(
        const std::string &bundleName, const std::string &moduleName, const std::string &abilityName,
        bool userDataClearable = true, bool isSystemApp = false) const;
    void MockInstallExtension(
        const std::string &bundleName, const std::string &moduleName, const std::string &extensionName) const;
    void MockInstallBundle(
        const std::string &bundleName, const std::vector<std::string> &moduleNameList, const std::string &abilityName,
        bool userDataClearable = true, bool isSystemApp = false) const;
    void MockUninstallBundle(const std::string &bundleName) const;
    AbilityInfo MockAbilityInfo(
        const std::string &bundleName, const std::string &module, const std::string &abilityName) const;
    ExtensionAbilityInfo MockExtensionInfo(
        const std::string &bundleName, const std::string &module, const std::string &extensionName) const;
    InnerModuleInfo MockModuleInfo(const std::string &moduleName) const;
    FormInfo MockFormInfo(
        const std::string &bundleName, const std::string &module, const std::string &abilityName) const;
    ShortcutInfo MockShortcutInfo(const std::string &bundleName, const std::string &shortcutId) const;
    ShortcutIntent MockShortcutIntent() const;
    ShortcutWant MockShortcutWant() const;
    Shortcut MockShortcut() const;
    CommonEventInfo MockCommonEventInfo(const std::string &bundleName, const int uid) const;
    void CheckBundleInfo(const std::string &bundleName, const std::string &moduleName, const uint32_t abilitySize,
        const BundleInfo &bundleInfo) const;
    void CheckBundleArchiveInfo(const std::string &bundleName, const std::string &moduleName,
        const uint32_t abilitySize, const BundleInfo &bundleInfo) const;
    void CheckBundleList(const std::string &bundleName, const std::vector<std::string> &bundleList) const;
    void CheckApplicationInfo(
        const std::string &bundleName, const uint32_t permissionSize, const ApplicationInfo &appInfo) const;
    void CheckAbilityInfo(const std::string &bundleName, const std::string &abilityName, int32_t flags,
        const AbilityInfo &appInfo) const;
    void CheckAbilityInfos(const std::string &bundleName, const std::string &abilityName, int32_t flags,
        const std::vector<AbilityInfo> &appInfo) const;
    void CheckCompatibleApplicationInfo(
        const std::string &bundleName, const uint32_t permissionSize, const CompatibleApplicationInfo &appInfo) const;
    void CheckCompatibleAbilityInfo(
        const std::string &bundleName, const std::string &abilityName, const CompatibleAbilityInfo &appInfo) const;
    void CheckInstalledBundleInfos(const uint32_t abilitySize, const std::vector<BundleInfo> &bundleInfos) const;
    void CheckInstalledApplicationInfos(const uint32_t permsSize, const std::vector<ApplicationInfo> &appInfos) const;
    void CheckModuleInfo(const HapModuleInfo &hapModuleInfo) const;
    void CreateFileDir() const;
    void CleanFileDir() const;
    void CheckFileExist() const;
    void CheckFileNonExist() const;
    void CheckCacheExist() const;
    void CheckCacheNonExist() const;
    void CheckFormInfoTest(const std::vector<FormInfo> &forms) const;
    void CheckFormInfoDemo(const std::vector<FormInfo> &forms) const;
    void CheckShortcutInfoTest(std::vector<ShortcutInfo> &shortcutInfos) const;
    void CheckCommonEventInfoTest(std::vector<CommonEventInfo> &commonEventInfos) const;
    void CheckShortcutInfoDemo(std::vector<ShortcutInfo> &shortcutInfos) const;
    void AddBundleInfo(const std::string &bundleName, BundleInfo &bundleInfo) const;
    void AddApplicationInfo(const std::string &bundleName, ApplicationInfo &appInfo,
        bool userDataClearable = true, bool isSystemApp = false) const;
    void AddInnerBundleInfoByTest(const std::string &bundleName, const std::string &moduleName,
        const std::string &abilityName, InnerBundleInfo &innerBundleInfo) const;
    void SaveToDatabase(const std::string &bundleName, InnerBundleInfo &innerBundleInfo,
        bool userDataClearable, bool isSystemApp) const;
    void ShortcutWantToJson(nlohmann::json &jsonObject, const ShortcutWant &shortcutWant);
    void ClearDataMgr();
    void SetDataMgr();

public:
    std::shared_ptr<BundleMgrService> bundleMgrService_ = DelayedSingleton<BundleMgrService>::GetInstance();
    std::shared_ptr<InstalldService> service_ = std::make_shared<InstalldService>();
    std::shared_ptr<LauncherService> launcherService_ = std::make_shared<LauncherService>();
    std::shared_ptr<BundleCommonEventMgr> commonEventMgr_ = std::make_shared<BundleCommonEventMgr>();
    std::shared_ptr<BundleUserMgrHostImpl> bundleUserMgrHostImpl_ = std::make_shared<BundleUserMgrHostImpl>();
    NotifyBundleEvents installRes_;
    const std::shared_ptr<BundleDataMgr> dataMgrInfo_ =
        DelayedSingleton<BundleMgrService>::GetInstance()->dataMgr_;
};

void BmsBundleKitServiceTest::SetUpTestCase()
{}

void BmsBundleKitServiceTest::TearDownTestCase()
{}

void BmsBundleKitServiceTest::SetUp()
{
    installRes_ = {
        .bundleName = HAP_FILE_PATH,
        .modulePackage = HAP_FILE_PATH,
        .abilityName = ABILITY_NAME_DEMO,
        .resultCode = ERR_OK,
        .type = NotifyType::INSTALL,
        .uid = Constants::INVALID_UID,
    };
    if (!service_->IsServiceReady()) {
        service_->Start();
    }
    if (!bundleMgrService_->IsServiceReady()) {
        bundleMgrService_->OnStart();
        std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME));
    }
}

void BmsBundleKitServiceTest::TearDown()
{}

void BmsBundleKitServiceTest::ClearDataMgr()
{
    bundleMgrService_->dataMgr_ = nullptr;
}

void BmsBundleKitServiceTest::SetDataMgr()
{
    EXPECT_NE(dataMgrInfo_, nullptr);
    bundleMgrService_->dataMgr_ = dataMgrInfo_;
    EXPECT_NE(bundleMgrService_->dataMgr_, nullptr);
}

std::shared_ptr<BundleDataMgr> BmsBundleKitServiceTest::GetBundleDataMgr() const
{
    return bundleMgrService_->GetDataMgr();
}

std::shared_ptr<BundleDistributedManager> BmsBundleKitServiceTest::GetBundleDistributedManager() const
{
    return bundleMgrService_->GetBundleDistributedManager();
}

std::shared_ptr<LauncherService> BmsBundleKitServiceTest::GetLauncherService() const
{
    return launcherService_;
}

sptr<BundleMgrProxy> BmsBundleKitServiceTest::GetBundleMgrProxy()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        APP_LOGE("fail to get system ability mgr.");
        return nullptr;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        APP_LOGE("fail to get bundle manager proxy.");
        return nullptr;
    }

    APP_LOGI("get bundle manager proxy success.");
    return iface_cast<BundleMgrProxy>(remoteObject);
}

void BmsBundleKitServiceTest::AddBundleInfo(const std::string &bundleName, BundleInfo &bundleInfo) const
{
    bundleInfo.name = bundleName;
    bundleInfo.label = BUNDLE_LABEL;
    bundleInfo.description = BUNDLE_DESCRIPTION;
    bundleInfo.vendor = BUNDLE_VENDOR;
    bundleInfo.versionCode = BUNDLE_VERSION_CODE;
    bundleInfo.versionName = BUNDLE_VERSION_NAME;
    bundleInfo.minSdkVersion = BUNDLE_MIN_SDK_VERSION;
    bundleInfo.maxSdkVersion = BUNDLE_MAX_SDK_VERSION;
    bundleInfo.mainEntry = MAIN_ENTRY;
    bundleInfo.isKeepAlive = true;
    bundleInfo.isDifferentName = true;
    bundleInfo.jointUserId = BUNDLE_JOINT_USERID;
    bundleInfo.singleton = true;
}

void BmsBundleKitServiceTest::AddApplicationInfo(const std::string &bundleName, ApplicationInfo &appInfo,
    bool userDataClearable, bool isSystemApp) const
{
    appInfo.bundleName = bundleName;
    appInfo.name = bundleName;
    appInfo.deviceId = DEVICE_ID;
    appInfo.process = PROCESS_TEST;
    appInfo.label = BUNDLE_LABEL;
    appInfo.description = BUNDLE_DESCRIPTION;
    appInfo.codePath = CODE_PATH;
    appInfo.dataDir = FILES_DIR;
    appInfo.dataBaseDir = DATA_BASE_DIR;
    appInfo.cacheDir = CACHE_DIR;
    appInfo.flags = APPLICATION_INFO_FLAGS;
    appInfo.enabled = true;
    appInfo.userDataClearable = userDataClearable;
    appInfo.isSystemApp = isSystemApp;
}

void BmsBundleKitServiceTest::AddInnerBundleInfoByTest(const std::string &bundleName,
    const std::string &moduleName, const std::string &abilityName, InnerBundleInfo &innerBundleInfo) const
{
    std::string keyName = bundleName + "." + moduleName + "." + abilityName;
    FormInfo form = MockFormInfo(bundleName, moduleName, abilityName);
    std::vector<FormInfo> formInfos;
    formInfos.emplace_back(form);
    if (bundleName == BUNDLE_NAME_TEST) {
        ShortcutInfo shortcut = MockShortcutInfo(bundleName, SHORTCUT_TEST_ID);
        std::string shortcutKey = bundleName + moduleName + SHORTCUT_TEST_ID;
        innerBundleInfo.InsertShortcutInfos(shortcutKey, shortcut);
    } else {
        ShortcutInfo shortcut = MockShortcutInfo(bundleName, SHORTCUT_DEMO_ID);
        std::string shortcutKey = bundleName + moduleName + SHORTCUT_DEMO_ID;
        innerBundleInfo.InsertShortcutInfos(shortcutKey, shortcut);
    }
    innerBundleInfo.InsertFormInfos(keyName, formInfos);
    std::string commonEventKey = bundleName + moduleName + abilityName;
    CommonEventInfo eventInfo = MockCommonEventInfo(bundleName, innerBundleInfo.GetUid(DEFAULT_USERID));
    innerBundleInfo.InsertCommonEvents(commonEventKey, eventInfo);
}

void BmsBundleKitServiceTest::MockInstallBundle(
    const std::string &bundleName, const std::string &moduleName, const std::string &abilityName,
    bool userDataClearable, bool isSystemApp) const
{
    InnerModuleInfo moduleInfo = MockModuleInfo(moduleName);
    std::string keyName = bundleName + "." + moduleName + "." + abilityName;
    moduleInfo.entryAbilityKey = keyName;
    AbilityInfo abilityInfo = MockAbilityInfo(bundleName, moduleName, abilityName);
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.InsertAbilitiesInfo(keyName, abilityInfo);
    innerBundleInfo.InsertInnerModuleInfo(moduleName, moduleInfo);
    Skill skill {{ACTION}, {ENTITY}};
    std::vector<Skill> skills;
    skills.emplace_back(skill);
    innerBundleInfo.InsertSkillInfo(keyName, skills);
    SaveToDatabase(bundleName, innerBundleInfo, userDataClearable, isSystemApp);
}

void BmsBundleKitServiceTest::MockInstallExtension(const std::string &bundleName,
    const std::string &moduleName, const std::string &extensionName) const
{
    InnerModuleInfo moduleInfo = MockModuleInfo(moduleName);
    std::string keyName = bundleName + "." + moduleName + "." + extensionName;
    std::string keyName02 = bundleName + "." + moduleName + "." + extensionName + "02";
    ExtensionAbilityInfo extensionInfo = MockExtensionInfo(bundleName, moduleName, extensionName);
    ExtensionAbilityInfo extensionInfo02 = MockExtensionInfo(bundleName, moduleName, extensionName + "02");
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.InsertExtensionInfo(keyName, extensionInfo);
    innerBundleInfo.InsertExtensionInfo(keyName02, extensionInfo02);
    innerBundleInfo.InsertInnerModuleInfo(moduleName, moduleInfo);
    Skill skill {{ACTION}, {ENTITY}};
    std::vector<Skill> skills;
    skills.emplace_back(skill);
    innerBundleInfo.InsertExtensionSkillInfo(keyName, skills);
    innerBundleInfo.InsertExtensionSkillInfo(keyName02, skills);
    SaveToDatabase(bundleName, innerBundleInfo, false, false);
}

InnerModuleInfo BmsBundleKitServiceTest::MockModuleInfo(const std::string &moduleName) const
{
    InnerModuleInfo moduleInfo;
    RequestPermission reqPermission1;
    reqPermission1.name = "permission1";
    RequestPermission reqPermission2;
    reqPermission2.name = "permission2";
    moduleInfo.requestPermissions = {reqPermission1, reqPermission2};
    moduleInfo.name = MODULE_NAME_TEST;
    moduleInfo.icon = ICON_PATH;
    moduleInfo.modulePackage = PACKAGE_NAME;
    moduleInfo.moduleName = moduleName;
    moduleInfo.description = BUNDLE_DESCRIPTION;
    moduleInfo.colorMode = COLOR_MODE;
    moduleInfo.label = LABEL;

    AppExecFwk::CustomizeData customizeData {"name", "value", "extra"};
    MetaData metaData {{customizeData}};
    moduleInfo.metaData = metaData;
    return moduleInfo;
}

void BmsBundleKitServiceTest::SaveToDatabase(const std::string &bundleName,
    InnerBundleInfo &innerBundleInfo, bool userDataClearable, bool isSystemApp) const
{
    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);

    InnerBundleUserInfo innerBundleUserInfo;
    innerBundleUserInfo.bundleName = bundleName;
    innerBundleUserInfo.bundleUserInfo.enabled = true;
    innerBundleUserInfo.bundleUserInfo.userId = Constants::DEFAULT_USERID;
    innerBundleUserInfo.uid = BASE_TEST_UID;

    InnerBundleUserInfo innerBundleUserInfo1;
    innerBundleUserInfo1.bundleName = bundleName;
    innerBundleUserInfo1.bundleUserInfo.enabled = true;
    innerBundleUserInfo1.bundleUserInfo.userId = DEFAULT_USERID;
    innerBundleUserInfo1.uid = TEST_UID;

    ApplicationInfo appInfo;
    AddApplicationInfo(bundleName, appInfo, userDataClearable, isSystemApp);
    BundleInfo bundleInfo;
    AddBundleInfo(bundleName, bundleInfo);
    innerBundleInfo.SetBaseApplicationInfo(appInfo);
    innerBundleInfo.AddInnerBundleUserInfo(innerBundleUserInfo);
    innerBundleInfo.AddInnerBundleUserInfo(innerBundleUserInfo1);
    innerBundleInfo.SetBaseBundleInfo(bundleInfo);
    auto accessTokenId = BundlePermissionMgr::CreateAccessTokenId(innerBundleInfo, bundleName, DEFAULT_USERID);
    innerBundleInfo.SetAccessTokenId(accessTokenId, DEFAULT_USERID);
    auto moduleNameVec = innerBundleInfo.GetModuleNameVec();
    auto abilityNameVec = innerBundleInfo.GetAbilityNames();
    if (!moduleNameVec.empty() && !abilityNameVec.empty()) {
        AddInnerBundleInfoByTest(bundleName, moduleNameVec[0], abilityNameVec[0], innerBundleInfo);
    }
    bool startRet = dataMgr->UpdateBundleInstallState(bundleName, InstallState::INSTALL_START);
    bool addRet = dataMgr->AddInnerBundleInfo(bundleName, innerBundleInfo);
    bool endRet = dataMgr->UpdateBundleInstallState(bundleName, InstallState::INSTALL_SUCCESS);

    EXPECT_TRUE(startRet);
    EXPECT_TRUE(addRet);
    EXPECT_TRUE(endRet);
}

void BmsBundleKitServiceTest::MockInstallBundle(
    const std::string &bundleName, const std::vector<std::string> &moduleNameList, const std::string &abilityName,
    bool userDataClearable, bool isSystemApp) const
{
    if (moduleNameList.empty()) {
        return;
    }
    InnerBundleInfo innerBundleInfo;
    for (const auto &moduleName : moduleNameList) {
        InnerModuleInfo moduleInfo = MockModuleInfo(moduleName);
        std::string keyName = bundleName + "." + moduleName + "." + abilityName;
        AbilityInfo abilityInfo = MockAbilityInfo(bundleName, moduleName, abilityName);
        innerBundleInfo.InsertAbilitiesInfo(keyName, abilityInfo);
        innerBundleInfo.InsertInnerModuleInfo(moduleName, moduleInfo);
        Skill skill {{ACTION}, {ENTITY}};
        std::vector<Skill> skills;
        skills.emplace_back(skill);
        innerBundleInfo.InsertSkillInfo(keyName, skills);
    }
    SaveToDatabase(bundleName, innerBundleInfo, userDataClearable, isSystemApp);
}

FormInfo BmsBundleKitServiceTest::MockFormInfo(
    const std::string &bundleName, const std::string &moduleName, const std::string &abilityName) const
{
    FormInfo formInfo;
    formInfo.name = FORM_NAME;
    formInfo.bundleName = bundleName;
    formInfo.abilityName = abilityName;
    formInfo.moduleName = moduleName;
    formInfo.package = PACKAGE_NAME;
    formInfo.descriptionId = FORMINFO_DESCRIPTIONID;
    formInfo.formConfigAbility = FORM_PATH;
    formInfo.description = FORM_DESCRIPTION;
    formInfo.defaultFlag = false;
    formInfo.type = FormType::JS;
    formInfo.colorMode = FormsColorMode::LIGHT_MODE;
    formInfo.supportDimensions = {1, 2};
    formInfo.portraitLayouts = {FORM_PORTRAIT_LAYOUTS1, FORM_PORTRAIT_LAYOUTS2};
    formInfo.landscapeLayouts = {FORM_LANDSCAPE_LAYOUTS1, FORM_LANDSCAPE_LAYOUTS2};
    formInfo.defaultDimension = 1;
    formInfo.updateDuration = 0;
    formInfo.formVisibleNotify = true;
    formInfo.deepLink = FORM_PATH;
    formInfo.scheduledUpdateTime = FORM_SCHEDULED_UPDATE_TIME;
    formInfo.updateEnabled = true;
    formInfo.jsComponentName = FORM_JS_COMPONENT_NAME;
    formInfo.src = FORM_SRC;
    formInfo.window.autoDesignWidth = true;
    formInfo.window.designWidth = FORM_JS_WINDOW_DESIGNWIDTH;
    for (auto &info : formInfo.customizeDatas) {
        info.name = FORM_CUSTOMIZE_DATAS_NAME;
        info.value = FORM_CUSTOMIZE_DATAS_VALUE;
    }
    return formInfo;
}

ShortcutInfo BmsBundleKitServiceTest::MockShortcutInfo(
    const std::string &bundleName, const std::string &shortcutId) const
{
    ShortcutInfo shortcutInfos;
    shortcutInfos.id = shortcutId;
    shortcutInfos.bundleName = bundleName;
    shortcutInfos.hostAbility = SHORTCUT_HOST_ABILITY;
    shortcutInfos.icon = SHORTCUT_ICON;
    shortcutInfos.label = SHORTCUT_LABEL;
    shortcutInfos.disableMessage = SHORTCUT_DISABLE_MESSAGE;
    shortcutInfos.isStatic = true;
    shortcutInfos.isHomeShortcut = true;
    shortcutInfos.isEnables = true;
    ShortcutIntent shortcutIntent;
    shortcutIntent.targetBundle = SHORTCUT_INTENTS_TARGET_BUNDLE;
    shortcutIntent.targetModule = SHORTCUT_INTENTS_TARGET_MODULE;
    shortcutIntent.targetClass = SHORTCUT_INTENTS_TARGET_CLASS;
    shortcutInfos.intents.push_back(shortcutIntent);
    return shortcutInfos;
}

ShortcutIntent BmsBundleKitServiceTest::MockShortcutIntent() const
{
    ShortcutIntent shortcutIntent;
    shortcutIntent.targetBundle = SHORTCUT_INTENTS_TARGET_BUNDLE;
    shortcutIntent.targetModule = SHORTCUT_INTENTS_TARGET_MODULE;
    shortcutIntent.targetClass = SHORTCUT_INTENTS_TARGET_CLASS;
    return shortcutIntent;
}

ShortcutWant BmsBundleKitServiceTest::MockShortcutWant() const
{
    ShortcutWant shortcutWant;
    shortcutWant.bundleName = BUNDLE_NAME_DEMO;
    shortcutWant.moduleName = MODULE_NAME_DEMO;
    shortcutWant.abilityName = ABILITY_NAME_DEMO;
    return shortcutWant;
}

Shortcut BmsBundleKitServiceTest::MockShortcut() const
{
    Shortcut shortcut;
    shortcut.shortcutId = SHORTCUT_TEST_ID;
    shortcut.icon = SHORTCUT_ICON;
    shortcut.iconId = ICON_ID;
    shortcut.label = SHORTCUT_LABEL;
    shortcut.labelId = LABEL_ID;
    ShortcutWant want = MockShortcutWant();
    shortcut.wants.push_back(want);
    return shortcut;
}

CommonEventInfo BmsBundleKitServiceTest::MockCommonEventInfo(
    const std::string &bundleName, const int uid) const
{
    CommonEventInfo CommonEventInfo;
    CommonEventInfo.name = COMMON_EVENT_NAME;
    CommonEventInfo.bundleName = bundleName;
    CommonEventInfo.uid = uid;
    CommonEventInfo.permission = COMMON_EVENT_PERMISSION;
    CommonEventInfo.data.emplace_back(COMMON_EVENT_DATA);
    CommonEventInfo.type.emplace_back(COMMON_EVENT_TYPE);
    CommonEventInfo.events.emplace_back(COMMON_EVENT_EVENT);
    return CommonEventInfo;
}

void BmsBundleKitServiceTest::MockUninstallBundle(const std::string &bundleName) const
{
    auto dataMgr = GetBundleDataMgr();
    EXPECT_NE(dataMgr, nullptr);
    bool startRet = dataMgr->UpdateBundleInstallState(bundleName, InstallState::UNINSTALL_START);
    bool finishRet = dataMgr->UpdateBundleInstallState(bundleName, InstallState::UNINSTALL_SUCCESS);

    EXPECT_TRUE(startRet);
    EXPECT_TRUE(finishRet);
}

AbilityInfo BmsBundleKitServiceTest::MockAbilityInfo(
    const std::string &bundleName, const std::string &moduleName, const std::string &abilityName) const
{
    AbilityInfo abilityInfo;
    abilityInfo.package = PACKAGE_NAME;
    abilityInfo.name = abilityName;
    abilityInfo.bundleName = bundleName;
    abilityInfo.moduleName = moduleName;
    abilityInfo.deviceId = DEVICE_ID;
    abilityInfo.label = LABEL;
    abilityInfo.labelId = 0;
    abilityInfo.description = DESCRIPTION;
    abilityInfo.theme = THEME;
    abilityInfo.iconPath = ICON_PATH;
    abilityInfo.visible = VISIBLE;
    abilityInfo.kind = KIND;
    abilityInfo.type = ABILITY_TYPE;
    abilityInfo.orientation = ORIENTATION;
    abilityInfo.launchMode = LAUNCH_MODE;
    abilityInfo.configChanges = {"locale"};
    abilityInfo.backgroundModes = 1;
    abilityInfo.formEntity = 1;
    abilityInfo.defaultFormHeight = DEFAULT_FORM_HEIGHT;
    abilityInfo.defaultFormWidth = DEFAULT_FORM_WIDTH;
    abilityInfo.codePath = CODE_PATH;
    abilityInfo.resourcePath = RESOURCE_PATH;
    abilityInfo.libPath = LIB_PATH;
    abilityInfo.uri = URI;
    abilityInfo.enabled = true;
    abilityInfo.supportPipMode = false;
    abilityInfo.targetAbility = TARGET_ABILITY;
    AppExecFwk::CustomizeData customizeData {
        "name",
        "value",
        "extra"
    };
    MetaData metaData {
        {customizeData}
    };
    abilityInfo.metaData = metaData;
    abilityInfo.permissions = {"abilityPerm001", "abilityPerm002"};
    return abilityInfo;
}

ExtensionAbilityInfo BmsBundleKitServiceTest::MockExtensionInfo(
    const std::string &bundleName, const std::string &moduleName, const std::string &extensionName) const
{
    ExtensionAbilityInfo extensionInfo;
    extensionInfo.name = extensionName;
    extensionInfo.bundleName = bundleName;
    extensionInfo.moduleName = moduleName;
    return extensionInfo;
}

void BmsBundleKitServiceTest::MockInnerBundleInfo(const std::string &bundleName, const std::string &moduleName,
    const std::string &abilityName, const std::vector<Dependency> &dependencies,
    InnerBundleInfo &innerBundleInfo) const
{
    ApplicationInfo appInfo;
    appInfo.bundleName = bundleName;
    BundleInfo bundleInfo;
    bundleInfo.name = bundleName;
    innerBundleInfo.SetBaseBundleInfo(bundleInfo);
    InnerModuleInfo moduleInfo;
    moduleInfo.modulePackage = moduleName;
    moduleInfo.moduleName = moduleName;
    moduleInfo.description = BUNDLE_DESCRIPTION;
    moduleInfo.dependencies = dependencies;
    innerBundleInfo.InsertInnerModuleInfo(moduleName, moduleInfo);
    AbilityInfo abilityInfo = MockAbilityInfo(bundleName, moduleName, abilityName);
    std::string keyName = bundleName + "." + moduleName + "." + abilityName;
    innerBundleInfo.InsertAbilitiesInfo(keyName, abilityInfo);
    innerBundleInfo.SetBaseApplicationInfo(appInfo);
}

void BmsBundleKitServiceTest::CheckBundleInfo(const std::string &bundleName, const std::string &moduleName,
    const uint32_t abilitySize, const BundleInfo &bundleInfo) const
{
    EXPECT_EQ(bundleName, bundleInfo.name);
    EXPECT_EQ(BUNDLE_LABEL, bundleInfo.label);
    EXPECT_EQ(BUNDLE_DESCRIPTION, bundleInfo.description);
    EXPECT_EQ(BUNDLE_VENDOR, bundleInfo.vendor);
    EXPECT_EQ(BUNDLE_VERSION_CODE, bundleInfo.versionCode);
    EXPECT_EQ(BUNDLE_VERSION_NAME, bundleInfo.versionName);
    EXPECT_EQ(BUNDLE_MIN_SDK_VERSION, bundleInfo.minSdkVersion);
    EXPECT_EQ(BUNDLE_MAX_SDK_VERSION, bundleInfo.maxSdkVersion);
    EXPECT_EQ(BUNDLE_MAIN_ABILITY, bundleInfo.mainEntry);
    EXPECT_EQ(bundleName, bundleInfo.applicationInfo.name);
    EXPECT_EQ(bundleName, bundleInfo.applicationInfo.bundleName);
    EXPECT_EQ(abilitySize, static_cast<uint32_t>(bundleInfo.abilityInfos.size()));
    EXPECT_EQ(true, bundleInfo.isDifferentName);
    EXPECT_EQ(BUNDLE_JOINT_USERID, bundleInfo.jointUserId);
    EXPECT_TRUE(bundleInfo.isKeepAlive);
    EXPECT_TRUE(bundleInfo.singleton);
}

void BmsBundleKitServiceTest::CheckBundleArchiveInfo(const std::string &bundleName, const std::string &moduleName,
    const uint32_t abilitySize, const BundleInfo &bundleInfo) const
{
    EXPECT_EQ(bundleName, bundleInfo.name);
    EXPECT_EQ(BUNDLE_LABEL, bundleInfo.label);
    EXPECT_EQ(BUNDLE_DESCRIPTION, bundleInfo.description);
    EXPECT_EQ(BUNDLE_VENDOR, bundleInfo.vendor);
    EXPECT_EQ(BUNDLE_VERSION_CODE, bundleInfo.versionCode);
    EXPECT_EQ(BUNDLE_VERSION_NAME, bundleInfo.versionName);
    EXPECT_EQ(BUNDLE_MIN_SDK_VERSION, bundleInfo.minSdkVersion);
    EXPECT_EQ(BUNDLE_MAX_SDK_VERSION, bundleInfo.maxSdkVersion);
    EXPECT_EQ(BUNDLE_MAIN_ABILITY, bundleInfo.mainEntry);
    EXPECT_EQ(bundleName, bundleInfo.applicationInfo.name);
    EXPECT_EQ(bundleName, bundleInfo.applicationInfo.bundleName);
    EXPECT_EQ(abilitySize, static_cast<uint32_t>(bundleInfo.abilityInfos.size()));
}

void BmsBundleKitServiceTest::CheckBundleList(
    const std::string &bundleName, const std::vector<std::string> &bundleList) const
{
    EXPECT_TRUE(std::find(bundleList.begin(), bundleList.end(), bundleName) != bundleList.end());
}

void BmsBundleKitServiceTest::CheckApplicationInfo(
    const std::string &bundleName, const uint32_t permissionSize, const ApplicationInfo &appInfo) const
{
    EXPECT_EQ(bundleName, appInfo.name);
    EXPECT_EQ(bundleName, appInfo.bundleName);
    EXPECT_EQ(BUNDLE_LABEL, appInfo.label);
    EXPECT_EQ(BUNDLE_DESCRIPTION, appInfo.description);
    EXPECT_EQ(DEVICE_ID, appInfo.deviceId);
    EXPECT_EQ(PROCESS_TEST, appInfo.process);
    EXPECT_EQ(CODE_PATH, appInfo.codePath);
    EXPECT_EQ(permissionSize, static_cast<uint32_t>(appInfo.permissions.size()));
    EXPECT_EQ(APPLICATION_INFO_FLAGS, appInfo.flags);
}

void BmsBundleKitServiceTest::CheckAbilityInfo(
    const std::string &bundleName, const std::string &abilityName, int32_t flags, const AbilityInfo &abilityInfo) const
{
    EXPECT_EQ(abilityName, abilityInfo.name);
    EXPECT_EQ(bundleName, abilityInfo.bundleName);
    EXPECT_EQ(LABEL, abilityInfo.label);
    EXPECT_EQ(DESCRIPTION, abilityInfo.description);
    EXPECT_EQ(DEVICE_ID, abilityInfo.deviceId);
    EXPECT_EQ(THEME, abilityInfo.theme);
    EXPECT_EQ(ICON_PATH, abilityInfo.iconPath);
    EXPECT_EQ(CODE_PATH, abilityInfo.codePath);
    EXPECT_EQ(ORIENTATION, abilityInfo.orientation);
    EXPECT_EQ(LAUNCH_MODE, abilityInfo.launchMode);
    EXPECT_EQ(URI, abilityInfo.uri);
    EXPECT_EQ(false, abilityInfo.supportPipMode);
    EXPECT_EQ(TARGET_ABILITY, abilityInfo.targetAbility);
    EXPECT_EQ(CONFIG_CHANGES, abilityInfo.configChanges);
    EXPECT_EQ(BACKGROUND_MODES, abilityInfo.backgroundModes);
    EXPECT_EQ(FORM_ENTITY, abilityInfo.formEntity);
    EXPECT_EQ(DEFAULT_FORM_HEIGHT, abilityInfo.defaultFormHeight);
    EXPECT_EQ(DEFAULT_FORM_WIDTH, abilityInfo.defaultFormWidth);
    if ((flags & GET_ABILITY_INFO_WITH_METADATA) != GET_ABILITY_INFO_WITH_METADATA) {
        EXPECT_EQ(0, abilityInfo.metaData.customizeData.size());
    } else {
        for (auto &info : abilityInfo.metaData.customizeData) {
            EXPECT_EQ(info.name, META_DATA_NAME);
            EXPECT_EQ(info.value, META_DATA_VALUE);
            EXPECT_EQ(info.extra, META_DATA_EXTRA);
        }
    }
}

void BmsBundleKitServiceTest::CheckAbilityInfos(const std::string &bundleName, const std::string &abilityName,
    int32_t flags, const std::vector<AbilityInfo> &abilityInfos) const
{
    for (auto abilityInfo : abilityInfos) {
        EXPECT_EQ(abilityName, abilityInfo.name);
        EXPECT_EQ(bundleName, abilityInfo.bundleName);
        EXPECT_EQ(LABEL, abilityInfo.label);
        EXPECT_EQ(DESCRIPTION, abilityInfo.description);
        EXPECT_EQ(DEVICE_ID, abilityInfo.deviceId);
        EXPECT_EQ(THEME, abilityInfo.theme);
        EXPECT_EQ(ICON_PATH, abilityInfo.iconPath);
        EXPECT_EQ(CODE_PATH, abilityInfo.codePath);
        EXPECT_EQ(ORIENTATION, abilityInfo.orientation);
        EXPECT_EQ(LAUNCH_MODE, abilityInfo.launchMode);
        EXPECT_EQ(URI, abilityInfo.uri);
        EXPECT_EQ(false, abilityInfo.supportPipMode);
        EXPECT_EQ(TARGET_ABILITY, abilityInfo.targetAbility);
        EXPECT_EQ(CONFIG_CHANGES, abilityInfo.configChanges);
        EXPECT_EQ(BACKGROUND_MODES, abilityInfo.backgroundModes);
        EXPECT_EQ(FORM_ENTITY, abilityInfo.formEntity);
        EXPECT_EQ(DEFAULT_FORM_HEIGHT, abilityInfo.defaultFormHeight);
        EXPECT_EQ(DEFAULT_FORM_WIDTH, abilityInfo.defaultFormWidth);
        if ((flags & GET_ABILITY_INFO_WITH_METADATA) != GET_ABILITY_INFO_WITH_METADATA) {
            EXPECT_EQ(0, abilityInfo.metaData.customizeData.size());
        } else {
            for (auto &info : abilityInfo.metaData.customizeData) {
                EXPECT_EQ(info.name, META_DATA_NAME);
                EXPECT_EQ(info.value, META_DATA_VALUE);
                EXPECT_EQ(info.extra, META_DATA_EXTRA);
            }
        }
        if ((flags & GET_ABILITY_INFO_WITH_PERMISSION) != GET_ABILITY_INFO_WITH_PERMISSION) {
            EXPECT_EQ(0, abilityInfo.permissions.size());
        } else {
            EXPECT_EQ(PERMISSION_SIZE_TWO, abilityInfo.permissions.size());
        }
    }
}

void BmsBundleKitServiceTest::CheckCompatibleAbilityInfo(
    const std::string &bundleName, const std::string &abilityName, const CompatibleAbilityInfo &abilityInfo) const
{
    EXPECT_EQ(abilityName, abilityInfo.name);
    EXPECT_EQ(bundleName, abilityInfo.bundleName);
    EXPECT_EQ(LABEL, abilityInfo.label);
    EXPECT_EQ(DESCRIPTION, abilityInfo.description);
    EXPECT_EQ(DEVICE_ID, abilityInfo.deviceId);
    EXPECT_EQ(ICON_PATH, abilityInfo.iconPath);
    EXPECT_EQ(ORIENTATION, abilityInfo.orientation);
    EXPECT_EQ(LAUNCH_MODE, abilityInfo.launchMode);
    EXPECT_EQ(URI, abilityInfo.uri);
    EXPECT_EQ(false, abilityInfo.supportPipMode);
    EXPECT_EQ(TARGET_ABILITY, abilityInfo.targetAbility);
    EXPECT_EQ(FORM_ENTITY, abilityInfo.formEntity);
    EXPECT_EQ(DEFAULT_FORM_HEIGHT, abilityInfo.defaultFormHeight);
    EXPECT_EQ(DEFAULT_FORM_WIDTH, abilityInfo.defaultFormWidth);
}
void BmsBundleKitServiceTest::CheckCompatibleApplicationInfo(
    const std::string &bundleName, const uint32_t permissionSize, const CompatibleApplicationInfo &appInfo) const
{
    EXPECT_EQ(bundleName, appInfo.name);
    EXPECT_EQ(BUNDLE_LABEL, appInfo.label);
    EXPECT_EQ(BUNDLE_DESCRIPTION, appInfo.description);
    EXPECT_EQ(PROCESS_TEST, appInfo.process);
    EXPECT_EQ(permissionSize, static_cast<uint32_t>(appInfo.permissions.size()));
}

void BmsBundleKitServiceTest::CheckModuleInfo(const HapModuleInfo &hapModuleInfo) const
{
    EXPECT_EQ(MODULE_NAME_TEST, hapModuleInfo.name);
    EXPECT_EQ(MODULE_NAME_TEST, hapModuleInfo.moduleName);
    EXPECT_EQ(BUNDLE_DESCRIPTION, hapModuleInfo.description);
    EXPECT_EQ(ICON_PATH, hapModuleInfo.iconPath);
    EXPECT_EQ(LABEL, hapModuleInfo.label);
    EXPECT_EQ(COLOR_MODE, hapModuleInfo.colorMode);
}

void BmsBundleKitServiceTest::CheckInstalledBundleInfos(
    const uint32_t abilitySize, const std::vector<BundleInfo> &bundleInfos) const
{
    bool isContainsDemoBundle = false;
    bool isContainsTestBundle = false;
    bool checkDemoAppNameRet = false;
    bool checkTestAppNameRet = false;
    bool checkDemoAbilitySizeRet = false;
    bool checkTestAbilitySizeRet = false;
    for (auto item : bundleInfos) {
        if (item.name == BUNDLE_NAME_DEMO) {
            isContainsDemoBundle = true;
            checkDemoAppNameRet = item.applicationInfo.name == BUNDLE_NAME_DEMO;
            uint32_t num = static_cast<uint32_t>(item.abilityInfos.size());
            checkDemoAbilitySizeRet = num == abilitySize;
        }
        if (item.name == BUNDLE_NAME_TEST) {
            isContainsTestBundle = true;
            checkTestAppNameRet = item.applicationInfo.name == BUNDLE_NAME_TEST;
            uint32_t num = static_cast<uint32_t>(item.abilityInfos.size());
            checkTestAbilitySizeRet = num == abilitySize;
        }
    }
    EXPECT_TRUE(isContainsDemoBundle);
    EXPECT_TRUE(isContainsTestBundle);
    EXPECT_TRUE(checkDemoAppNameRet);
    EXPECT_TRUE(checkTestAppNameRet);
    EXPECT_TRUE(checkDemoAbilitySizeRet);
    EXPECT_TRUE(checkTestAbilitySizeRet);
}

void BmsBundleKitServiceTest::CheckInstalledApplicationInfos(
    const uint32_t permsSize, const std::vector<ApplicationInfo> &appInfos) const
{
    bool isContainsDemoBundle = false;
    bool isContainsTestBundle = false;
    bool checkDemoAppNameRet = false;
    bool checkTestAppNameRet = false;
    bool checkDemoAbilitySizeRet = false;
    bool checkTestAbilitySizeRet = false;
    for (auto item : appInfos) {
        if (item.name == BUNDLE_NAME_DEMO) {
            isContainsDemoBundle = true;
            checkDemoAppNameRet = item.bundleName == BUNDLE_NAME_DEMO;
            uint32_t num = static_cast<uint32_t>(item.permissions.size());
            checkDemoAbilitySizeRet = num == permsSize;
        }
        if (item.name == BUNDLE_NAME_TEST) {
            isContainsTestBundle = true;
            checkTestAppNameRet = item.bundleName == BUNDLE_NAME_TEST;
            uint32_t num = static_cast<uint32_t>(item.permissions.size());
            checkTestAbilitySizeRet = num == permsSize;
        }
    }
    EXPECT_TRUE(isContainsDemoBundle);
    EXPECT_TRUE(isContainsTestBundle);
    EXPECT_TRUE(checkDemoAppNameRet);
    EXPECT_TRUE(checkTestAppNameRet);
    EXPECT_TRUE(checkDemoAbilitySizeRet);
    EXPECT_TRUE(checkTestAbilitySizeRet);
}

void BmsBundleKitServiceTest::CreateFileDir() const
{
    if (!service_->IsServiceReady()) {
        service_->Start();
    }

    if (access(TEST_FILE_DIR.c_str(), F_OK) != 0) {
        bool result = OHOS::ForceCreateDirectory(TEST_FILE_DIR);
        EXPECT_TRUE(result) << "fail to create file dir";
    }

    if (access(TEST_CACHE_DIR.c_str(), F_OK) != 0) {
        bool result = OHOS::ForceCreateDirectory(TEST_CACHE_DIR);
        EXPECT_TRUE(result) << "fail to create cache dir";
    }
}

void BmsBundleKitServiceTest::CleanFileDir() const
{
    service_->Stop();
    InstalldClient::GetInstance()->ResetInstalldProxy();

    OHOS::ForceRemoveDirectory(BUNDLE_DATA_DIR);
}

void BmsBundleKitServiceTest::CheckFileExist() const
{
    int dataExist = access(TEST_FILE_DIR.c_str(), F_OK);
    EXPECT_EQ(dataExist, 0);
}

void BmsBundleKitServiceTest::CheckFileNonExist() const
{
    int dataExist = access(TEST_FILE_DIR.c_str(), F_OK);
    EXPECT_NE(dataExist, 0);
}

void BmsBundleKitServiceTest::CheckCacheExist() const
{
    int dataExist = access(TEST_CACHE_DIR.c_str(), F_OK);
    EXPECT_EQ(dataExist, 0);
}

void BmsBundleKitServiceTest::CheckCacheNonExist() const
{
    int dataExist = access(TEST_CACHE_DIR.c_str(), F_OK);
    EXPECT_NE(dataExist, 0);
}

void BmsBundleKitServiceTest::CheckFormInfoTest(const std::vector<FormInfo> &formInfos) const
{
    for (auto &formInfo : formInfos) {
        EXPECT_EQ(formInfo.name, FORM_NAME);
        EXPECT_EQ(formInfo.bundleName, BUNDLE_NAME_TEST);
        EXPECT_EQ(formInfo.moduleName, MODULE_NAME_TEST);
        EXPECT_EQ(formInfo.abilityName, ABILITY_NAME_TEST);
        EXPECT_EQ(formInfo.description, FORM_DESCRIPTION);
        EXPECT_EQ(formInfo.formConfigAbility, FORM_PATH);
        EXPECT_EQ(formInfo.defaultFlag, false);
        EXPECT_EQ(formInfo.type, FormType::JS);
        EXPECT_EQ(formInfo.colorMode, FormsColorMode::LIGHT_MODE);
        EXPECT_EQ(formInfo.descriptionId, FORMINFO_DESCRIPTIONID);
        EXPECT_EQ(formInfo.deepLink, FORM_PATH);
        EXPECT_EQ(formInfo.package, PACKAGE_NAME);
        EXPECT_EQ(formInfo.formVisibleNotify, true);
        unsigned i = 2;
        EXPECT_EQ(formInfo.supportDimensions.size(), i);
        EXPECT_EQ(formInfo.defaultDimension, 1);
        EXPECT_EQ(formInfo.updateDuration, 0);
        EXPECT_EQ(formInfo.scheduledUpdateTime, FORM_SCHEDULED_UPDATE_TIME);
        EXPECT_EQ(formInfo.jsComponentName, FORM_JS_COMPONENT_NAME);
        EXPECT_EQ(formInfo.updateEnabled, true);
        for (auto &info : formInfo.customizeDatas) {
            EXPECT_EQ(info.name, FORM_CUSTOMIZE_DATAS_NAME);
            EXPECT_EQ(info.value, FORM_CUSTOMIZE_DATAS_VALUE);
        }
        EXPECT_EQ(formInfo.src, FORM_SRC);
        EXPECT_EQ(formInfo.window.designWidth, FORM_JS_WINDOW_DESIGNWIDTH);
        EXPECT_EQ(formInfo.window.autoDesignWidth, true);
    }
}

void BmsBundleKitServiceTest::CheckFormInfoDemo(const std::vector<FormInfo> &formInfos) const
{
    for (const auto &formInfo : formInfos) {
        EXPECT_EQ(formInfo.name, FORM_NAME);
        EXPECT_EQ(formInfo.bundleName, BUNDLE_NAME_DEMO);
        EXPECT_EQ(formInfo.moduleName, MODULE_NAME_DEMO);
        EXPECT_EQ(formInfo.abilityName, ABILITY_NAME_TEST);
        EXPECT_EQ(formInfo.description, FORM_DESCRIPTION);
        EXPECT_EQ(formInfo.formConfigAbility, FORM_PATH);
        EXPECT_EQ(formInfo.defaultFlag, false);
        EXPECT_EQ(formInfo.type, FormType::JS);
        EXPECT_EQ(formInfo.colorMode, FormsColorMode::LIGHT_MODE);
        EXPECT_EQ(formInfo.descriptionId, FORMINFO_DESCRIPTIONID);
        EXPECT_EQ(formInfo.deepLink, FORM_PATH);
        EXPECT_EQ(formInfo.package, PACKAGE_NAME);
        EXPECT_EQ(formInfo.formVisibleNotify, true);
        unsigned i = 2;
        EXPECT_EQ(formInfo.supportDimensions.size(), i);
        EXPECT_EQ(formInfo.defaultDimension, 1);
        EXPECT_EQ(formInfo.updateDuration, 0);
        EXPECT_EQ(formInfo.scheduledUpdateTime, FORM_SCHEDULED_UPDATE_TIME);
        EXPECT_EQ(formInfo.jsComponentName, FORM_JS_COMPONENT_NAME);
        EXPECT_EQ(formInfo.updateEnabled, true);
        EXPECT_EQ(formInfo.src, FORM_SRC);
        EXPECT_EQ(formInfo.window.designWidth, FORM_JS_WINDOW_DESIGNWIDTH);
        for (auto &info : formInfo.customizeDatas) {
            EXPECT_EQ(info.name, FORM_CUSTOMIZE_DATAS_NAME);
            EXPECT_EQ(info.value, FORM_CUSTOMIZE_DATAS_VALUE);
        }
    }
}

void BmsBundleKitServiceTest::CheckShortcutInfoTest(std::vector<ShortcutInfo> &shortcutInfos) const
{
    for (const auto &shortcutInfo : shortcutInfos) {
        EXPECT_EQ(shortcutInfo.id, SHORTCUT_TEST_ID);
        EXPECT_EQ(shortcutInfo.bundleName, BUNDLE_NAME_TEST);
        EXPECT_EQ(shortcutInfo.hostAbility, SHORTCUT_HOST_ABILITY);
        EXPECT_EQ(shortcutInfo.icon, SHORTCUT_ICON);
        EXPECT_EQ(shortcutInfo.label, SHORTCUT_LABEL);
        EXPECT_EQ(shortcutInfo.disableMessage, SHORTCUT_DISABLE_MESSAGE);
        EXPECT_EQ(shortcutInfo.isStatic, true);
        EXPECT_EQ(shortcutInfo.isHomeShortcut, true);
        EXPECT_EQ(shortcutInfo.isEnables, true);
        for (auto &shortcutIntent : shortcutInfo.intents) {
            EXPECT_EQ(shortcutIntent.targetBundle, SHORTCUT_INTENTS_TARGET_BUNDLE);
            EXPECT_EQ(shortcutIntent.targetModule, SHORTCUT_INTENTS_TARGET_MODULE);
            EXPECT_EQ(shortcutIntent.targetClass, SHORTCUT_INTENTS_TARGET_CLASS);
        }
    }
}

void BmsBundleKitServiceTest::CheckCommonEventInfoTest(std::vector<CommonEventInfo> &commonEventInfos) const
{
    for (const auto &commonEventInfo : commonEventInfos) {

        EXPECT_EQ(commonEventInfo.name, COMMON_EVENT_NAME);
        EXPECT_EQ(commonEventInfo.bundleName, BUNDLE_NAME_TEST);
        EXPECT_EQ(commonEventInfo.permission, COMMON_EVENT_PERMISSION);
        for (auto item : commonEventInfo.data) {
            EXPECT_EQ(item, COMMON_EVENT_DATA);
        }
        for (auto item : commonEventInfo.type) {
            EXPECT_EQ(item, COMMON_EVENT_TYPE);
        }
        for (auto item : commonEventInfo.events) {
            EXPECT_EQ(item, COMMON_EVENT_EVENT);
        }
    }
}

void BmsBundleKitServiceTest::CheckShortcutInfoDemo(std::vector<ShortcutInfo> &shortcutInfos) const
{
    for (const auto &shortcutInfo : shortcutInfos) {
        EXPECT_EQ(shortcutInfo.id, SHORTCUT_DEMO_ID);
        EXPECT_EQ(shortcutInfo.bundleName, BUNDLE_NAME_DEMO);
        EXPECT_EQ(shortcutInfo.hostAbility, SHORTCUT_HOST_ABILITY);
        EXPECT_EQ(shortcutInfo.icon, SHORTCUT_ICON);
        EXPECT_EQ(shortcutInfo.label, SHORTCUT_LABEL);
        EXPECT_EQ(shortcutInfo.disableMessage, SHORTCUT_DISABLE_MESSAGE);
        EXPECT_EQ(shortcutInfo.isStatic, true);
        EXPECT_EQ(shortcutInfo.isHomeShortcut, true);
        EXPECT_EQ(shortcutInfo.isEnables, true);
        for (auto &shortcutIntent : shortcutInfo.intents) {
            EXPECT_EQ(shortcutIntent.targetBundle, SHORTCUT_INTENTS_TARGET_BUNDLE);
            EXPECT_EQ(shortcutIntent.targetModule, SHORTCUT_INTENTS_TARGET_MODULE);
            EXPECT_EQ(shortcutIntent.targetClass, SHORTCUT_INTENTS_TARGET_CLASS);
        }
    }
}

void BmsBundleKitServiceTest::ShortcutWantToJson(nlohmann::json &jsonObject, const ShortcutWant &shortcutWant)
{
    jsonObject = nlohmann::json {
        {BUNDLE_NAME, shortcutWant.bundleName},
        {MODULE_NAME, shortcutWant.moduleName},
        {ABILITY_NAME, shortcutWant.abilityName},
    };
}

/**
 * @tc.number: CheckModuleRemovable_0100
 * @tc.name: test can check module removable is enable by no setting
 * @tc.desc: 1.system run normally
 *           2.check the module removable successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckModuleRemovable_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    bool isRemovable = false;
    auto testRet = GetBundleDataMgr()->IsModuleRemovable(BUNDLE_NAME_TEST, MODULE_NAME_TEST, isRemovable);
    EXPECT_EQ(testRet, ERR_OK);
    EXPECT_FALSE(isRemovable);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckModuleRemovable_0200
 * @tc.name: test can check module removable is enable by setting
 * @tc.desc: 1.system run normally
 *           2.check the module removable successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckModuleRemovable_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    bool testRet = GetBundleDataMgr()->SetModuleRemovable(BUNDLE_NAME_TEST, MODULE_NAME_TEST, true);
    EXPECT_TRUE(testRet);
    bool isRemovable = false;
    auto testRet1 = GetBundleDataMgr()->IsModuleRemovable(BUNDLE_NAME_TEST, MODULE_NAME_TEST, isRemovable);
    EXPECT_EQ(testRet1, ERR_OK);
    EXPECT_TRUE(isRemovable);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckModuleRemovable_0300
 * @tc.name: test can check module removable is disable by setting
 * @tc.desc: 1.system run normally
 *           2.check the module removable successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckModuleRemovable_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    bool testRet = GetBundleDataMgr()->SetModuleRemovable(BUNDLE_NAME_TEST, MODULE_NAME_TEST, false);
    EXPECT_TRUE(testRet);
    bool isRemovable = false;
    auto testRet1 = GetBundleDataMgr()->IsModuleRemovable(BUNDLE_NAME_TEST, MODULE_NAME_TEST, isRemovable);
    EXPECT_EQ(testRet1, ERR_OK);
    EXPECT_FALSE(isRemovable);

    bool testRet2 = GetBundleDataMgr()->SetModuleRemovable(BUNDLE_NAME_TEST, MODULE_NAME_TEST, true);
    EXPECT_TRUE(testRet2);
    isRemovable = false;
    auto testRet3 = GetBundleDataMgr()->IsModuleRemovable(BUNDLE_NAME_TEST, MODULE_NAME_TEST, isRemovable);
    EXPECT_EQ(testRet3, ERR_OK);
    EXPECT_TRUE(isRemovable);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckModuleRemovable_0400
 * @tc.name: test can check module removable is disable by no install
 * @tc.desc: 1.system run normally
 *           2.check the module removable failed
 */
HWTEST_F(BmsBundleKitServiceTest, CheckModuleRemovable_0400, Function | SmallTest | Level1)
{
    bool isRemovable = false;
    auto testRet = GetBundleDataMgr()->IsModuleRemovable(BUNDLE_NAME_TEST, MODULE_NAME_TEST, isRemovable);
    EXPECT_EQ(testRet, ERR_BUNDLE_MANAGER_BUNDLE_NOT_EXIST);
    EXPECT_FALSE(isRemovable);
}

/**
 * @tc.number: CheckModuleRemovable_0500
 * @tc.name: test can check module removable is able by empty bundle name
 * @tc.desc: 1.system run normally
 *           2.check the module removable successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckModuleRemovable_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    bool testRet = GetBundleDataMgr()->SetModuleRemovable("", "", true);
    EXPECT_FALSE(testRet);
    bool isRemovable = false;
    auto testRet1 = GetBundleDataMgr()->IsModuleRemovable(BUNDLE_NAME_TEST, MODULE_NAME_TEST, isRemovable);
    EXPECT_EQ(testRet1, ERR_OK);
    EXPECT_FALSE(isRemovable);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckModuleRemovable_0600
 * @tc.name: test can check module removable is disable by empty bundle name
 * @tc.desc: 1.system run normally
 *           2.check the module removable failed
 */
HWTEST_F(BmsBundleKitServiceTest, CheckModuleRemovable_0600, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    bool testRet = GetBundleDataMgr()->SetModuleRemovable(BUNDLE_NAME_TEST, MODULE_NAME_TEST, true);
    EXPECT_TRUE(testRet);
    bool isRemovable = false;
    auto testRet1 = GetBundleDataMgr()->IsModuleRemovable("", "", isRemovable);
    EXPECT_EQ(testRet1, ERR_BUNDLE_MANAGER_PARAM_ERROR);
    EXPECT_FALSE(isRemovable);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckModuleRemovable_0700
 * @tc.name: test can check module removable is able by empty bundle name
 * @tc.desc: 1.system run normally
 *           2.check the module removable successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckModuleRemovable_0700, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bool testRet = GetBundleDataMgr()->SetModuleRemovable("", "", true);
    EXPECT_FALSE(testRet);
    bool isRemovable = false;
    auto testRet1 = GetBundleDataMgr()->IsModuleRemovable(BUNDLE_NAME_TEST, MODULE_NAME_TEST, isRemovable);
    EXPECT_EQ(testRet1, ERR_OK);
    EXPECT_FALSE(isRemovable);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetBundleInfo_0100
 * @tc.name: test can get the bundleName's bundle info
 * @tc.desc: 1.system run normal
 *           2.get bundle info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfo_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    BundleInfo testResult;
    bool testRet = GetBundleDataMgr()->GetBundleInfo(BUNDLE_NAME_TEST, BundleFlag::GET_BUNDLE_DEFAULT, testResult,
        DEFAULT_USERID);
    EXPECT_TRUE(testRet);
    CheckBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_SIZE_ZERO, testResult);
    BundleInfo demoResult;
    bool demoRet = GetBundleDataMgr()->GetBundleInfo(BUNDLE_NAME_DEMO, BundleFlag::GET_BUNDLE_WITH_ABILITIES,
        demoResult, DEFAULT_USERID);
    EXPECT_TRUE(demoRet);
    CheckBundleInfo(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_SIZE_ONE, demoResult);
    EXPECT_EQ(PERMISSION_SIZE_ZERO, demoResult.reqPermissions.size());

    BundleInfo bundleInfo;
    bool ret = GetBundleDataMgr()->GetBundleInfo(BUNDLE_NAME_DEMO, GET_BUNDLE_WITH_ABILITIES |
        GET_BUNDLE_WITH_REQUESTED_PERMISSION, bundleInfo, DEFAULT_USERID);
    EXPECT_TRUE(ret);
    CheckBundleInfo(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_SIZE_ONE, bundleInfo);
    EXPECT_EQ(PERMISSION_SIZE_TWO, bundleInfo.reqPermissions.size());

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetBundleInfo_0200
 * @tc.name: test can not get the bundleName's bundle info which not exist in system
 * @tc.desc: 1.system run normal
 *           2.get bundle info failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfo_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    BundleInfo result;
    bool ret = GetBundleDataMgr()->GetBundleInfo(BUNDLE_NAME_DEMO, BundleFlag::GET_BUNDLE_DEFAULT, result,
        DEFAULT_USERID);
    EXPECT_FALSE(ret);
    EXPECT_EQ(EMPTY_STRING, result.label);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetBundleInfo_0300
 * @tc.name: test can not get bundle info with empty bundle name
 * @tc.desc: 1.system run normal
 *           2.get bundle info failed with empty bundle name
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfo_0300, Function | SmallTest | Level0)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    BundleInfo result;
    bool ret = GetBundleDataMgr()->GetBundleInfo(EMPTY_STRING, BundleFlag::GET_BUNDLE_DEFAULT, result, DEFAULT_USERID);
    EXPECT_FALSE(ret);
    EXPECT_EQ(EMPTY_STRING, result.name);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetBundleInfo_0400
 * @tc.name: test can not get the bundleName's bundle info with no bundle in system
 * @tc.desc: 1.system run normally and without any bundle
 *           2.get bundle info failed with no bundle in system
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfo_0400, Function | SmallTest | Level0)
{
    BundleInfo bundleInfo;
    bool ret = GetBundleDataMgr()->GetBundleInfo(BUNDLE_NAME_TEST, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo,
        DEFAULT_USERID);
    EXPECT_FALSE(ret);
    EXPECT_EQ(EMPTY_STRING, bundleInfo.name);
    EXPECT_EQ(EMPTY_STRING, bundleInfo.label);
}

/**
 * @tc.number: GetBundleInfo_0500
 * @tc.name: test can get the bundleName's bundle info
 * @tc.desc: 1.system run normal
 *           2.get bundle info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfo_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    BundleInfo testResult;
    bool testRet = hostImpl->GetBundleInfo(BUNDLE_NAME_TEST, BundleFlag::GET_BUNDLE_DEFAULT, testResult,
        DEFAULT_USERID);
    EXPECT_TRUE(testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetBundleInfo_0600
 * @tc.name: test can get the bundleName's bundle info
 * @tc.desc: 1.system run normal
 *           2.get bundle info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfo_0600, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    BundleInfo testResult;
    bool testRet = hostImpl->GetBundleInfo(BUNDLE_NAME_TEST, GET_BUNDLE_WITH_ABILITIES |
        GET_BUNDLE_WITH_REQUESTED_PERMISSION, testResult, DEFAULT_USERID);

    EXPECT_TRUE(testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetBundleInfo_0700
 * @tc.name: test can get the bundleName's bundle info
 * @tc.desc: 1.system run normal
 *           2.get bundle info failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfo_0700, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    BundleInfo testResult;
    bool testRet = hostImpl->GetBundleInfo("", GET_BUNDLE_WITH_ABILITIES |
        GET_BUNDLE_WITH_REQUESTED_PERMISSION, testResult, DEFAULT_USERID);
    EXPECT_FALSE(testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetBundleInfos_0100
 * @tc.name: test can get the installed bundles's bundle info with nomal flag
 * @tc.desc: 1.system run normally
 *           2.get all installed bundle info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfos_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<BundleInfo> bundleInfos;
    bool ret = GetBundleDataMgr()->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos, DEFAULT_USERID);
    EXPECT_TRUE(ret);
    CheckInstalledBundleInfos(ABILITY_SIZE_ZERO, bundleInfos);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetBundleInfos_0200
 * @tc.name: test can get the installed bundles's bundle info with abilities
 * @tc.desc: 1.system run normally
 *           2.get all installed bundle info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfos_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<BundleInfo> bundleInfos;
    bool ret = GetBundleDataMgr()->GetBundleInfos(BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfos, DEFAULT_USERID);
    EXPECT_TRUE(ret);
    CheckInstalledBundleInfos(ABILITY_SIZE_ONE, bundleInfos);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetApplicationInfo_0100
 * @tc.name: test can get the appName's application info
 * @tc.desc: 1.system run normally
 *           2.get application info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetApplicationInfo_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);

    ApplicationInfo testResult;
    bool testRet = GetBundleDataMgr()->GetApplicationInfo(
        BUNDLE_NAME_TEST, ApplicationFlag::GET_BASIC_APPLICATION_INFO, DEFAULT_USER_ID_TEST, testResult);
    EXPECT_TRUE(testRet);
    CheckApplicationInfo(BUNDLE_NAME_TEST, PERMISSION_SIZE_ZERO, testResult);

    ApplicationInfo demoResult;
    bool demoRet = GetBundleDataMgr()->GetApplicationInfo(
        BUNDLE_NAME_DEMO, ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION, DEFAULT_USER_ID_TEST, demoResult);
    EXPECT_TRUE(demoRet);
    CheckApplicationInfo(BUNDLE_NAME_DEMO, PERMISSION_SIZE_TWO, demoResult);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetApplicationInfo_0200
 * @tc.name: test can not get the appName's application info which not exist in system
 * @tc.desc: 1.system run normally
 *           2.get application info failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetApplicationInfo_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    ApplicationInfo result;
    bool ret = GetBundleDataMgr()->GetApplicationInfo(
        BUNDLE_NAME_DEMO, ApplicationFlag::GET_BASIC_APPLICATION_INFO, DEFAULT_USER_ID_TEST, result);
    EXPECT_FALSE(ret);
    EXPECT_EQ(EMPTY_STRING, result.name);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetApplicationInfo_0300
 * @tc.name: test can not get application info with empty appName
 * @tc.desc: 1.system run normally
 *           2.get application info failed with empty appName
 */
HWTEST_F(BmsBundleKitServiceTest, GetApplicationInfo_0300, Function | SmallTest | Level0)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    ApplicationInfo result;
    bool ret = GetBundleDataMgr()->GetApplicationInfo(
        EMPTY_STRING, ApplicationFlag::GET_BASIC_APPLICATION_INFO, DEFAULT_USER_ID_TEST, result);
    EXPECT_FALSE(ret);
    EXPECT_EQ(EMPTY_STRING, result.name);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetApplicationInfo_0400
 * @tc.name: test can not get the appName's application info with no bundle in system
 * @tc.desc: 1.system run normally
 *           2.get application info failed with no bundle in system
 */
HWTEST_F(BmsBundleKitServiceTest, GetApplicationInfo_0400, Function | SmallTest | Level0)
{
    ApplicationInfo result;
    bool ret = GetBundleDataMgr()->GetApplicationInfo(
        BUNDLE_NAME_TEST, ApplicationFlag::GET_BASIC_APPLICATION_INFO, DEFAULT_USER_ID_TEST, result);
    EXPECT_FALSE(ret);
    EXPECT_EQ(EMPTY_STRING, result.name);
    EXPECT_EQ(EMPTY_STRING, result.label);
}

/**
 * @tc.number: GetApplicationInfo_0600
 * @tc.name: test can parceable application info(permissions and metaData)
 * @tc.desc: 1.system run normally
 *           2.get application info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetApplicationInfo_0600, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    ApplicationInfo testResult;
    bool testRet = GetBundleDataMgr()->GetApplicationInfo(BUNDLE_NAME_TEST,
        GET_APPLICATION_INFO_WITH_PERMISSION | GET_APPLICATION_INFO_WITH_METADATA, DEFAULT_USER_ID_TEST, testResult);
    EXPECT_TRUE(testRet);
    EXPECT_EQ(PERMISSION_SIZE_TWO, testResult.permissions.size());
    EXPECT_EQ(META_DATA_SIZE_ONE, testResult.metaData.size());

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetApplicationInfo_0700
 * @tc.name: test can not get application info with empty appName
 * @tc.desc: 1.system run normally
 *           2.get application info failed with empty appName
 */
HWTEST_F(BmsBundleKitServiceTest, GetApplicationInfo_0700, Function | SmallTest | Level0)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    ApplicationInfo result;
    bool ret = hostImpl->GetApplicationInfo(
        EMPTY_STRING, ApplicationFlag::GET_BASIC_APPLICATION_INFO, DEFAULT_USER_ID_TEST, result);
    EXPECT_FALSE(ret);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}


/**
 * @tc.number: GetApplicationInfos_0100
 * @tc.name: test can get the installed bundles's application info with basic info flag
 * @tc.desc: 1.system run normally
 *           2.get all installed application info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetApplicationInfos_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<ApplicationInfo> appInfos;
    bool ret = GetBundleDataMgr()->GetApplicationInfos(
        ApplicationFlag::GET_BASIC_APPLICATION_INFO, DEFAULT_USER_ID_TEST, appInfos);
    EXPECT_TRUE(ret);
    CheckInstalledApplicationInfos(PERMISSION_SIZE_ZERO, appInfos);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetApplicationInfos_0200
 * @tc.name: test can get the installed bundles's application info with permissions
 * @tc.desc: 1.system run normally
 *           2.get all installed application info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetApplicationInfos_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<ApplicationInfo> appInfos;
    bool ret = GetBundleDataMgr()->GetApplicationInfos(
        ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION, DEFAULT_USER_ID_TEST, appInfos);
    EXPECT_TRUE(ret);
    CheckInstalledApplicationInfos(PERMISSION_SIZE_TWO, appInfos);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetApplicationInfos_0300
 * @tc.name: test can get the installed bundles's application info with permissions
 * @tc.desc: 1.system run normally
 *           2.get all installed application info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetApplicationInfos_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<ApplicationInfo> appInfos;
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool ret = hostImpl->GetApplicationInfos(
        ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION, DEFAULT_USER_ID_TEST, appInfos);
    EXPECT_TRUE(ret);
    CheckInstalledApplicationInfos(PERMISSION_SIZE_TWO, appInfos);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetApplicationInfos_0400
 * @tc.name: test can get the installed bundles's application info with permissions
 * @tc.desc: 1.system run normally
 *           2.get all installed application info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetApplicationInfos_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<ApplicationInfo> appInfos;
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool ret = hostImpl->GetApplicationInfos(
        GET_ABILITY_INFO_DEFAULT, DEFAULT_USER_ID_TEST, appInfos);
    EXPECT_TRUE(ret);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetAbilityLabel_0100
 * @tc.name: test can get the ability's label by bundleName and abilityName
 * @tc.desc: 1.system run normally
 *           2.get ability label successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetAbilityLabel_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::string testRet;
    ErrCode ret = GetBundleDataMgr()->GetAbilityLabel(BUNDLE_NAME_TEST, EMPTY_STRING, ABILITY_NAME_TEST, testRet);
    EXPECT_EQ(0, ret);
    EXPECT_EQ(LABEL, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAbilityLabel_0200
 * @tc.name: test can not get the ability's label if bundle doesn't install
 * @tc.desc: 1.system run normally
 *           2.get empty ability label
 */
HWTEST_F(BmsBundleKitServiceTest, GetAbilityLabel_0200, Function | SmallTest | Level1)
{
    std::string testRet;
    ErrCode ret = GetBundleDataMgr()->GetAbilityLabel(BUNDLE_NAME_TEST, EMPTY_STRING, ABILITY_NAME_TEST, testRet);
    EXPECT_NE(0, ret);
    EXPECT_EQ(EMPTY_STRING, testRet);
}

/**
 * @tc.number: GetAbilityLabel_0300
 * @tc.name: test can not get the ability's label if bundle doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get empty ability label
 */
HWTEST_F(BmsBundleKitServiceTest, GetAbilityLabel_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::string testRet;
    ErrCode ret = GetBundleDataMgr()->GetAbilityLabel(BUNDLE_NAME_DEMO, EMPTY_STRING, ABILITY_NAME_TEST, testRet);
    EXPECT_NE(0, ret);
    EXPECT_EQ(EMPTY_STRING, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAbilityLabel_0400
 * @tc.name: test can not get the ability's label if ability doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get empty ability label
 */
HWTEST_F(BmsBundleKitServiceTest, GetAbilityLabel_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::string testRet;
    ErrCode ret = GetBundleDataMgr()->GetAbilityLabel(BUNDLE_NAME_TEST, EMPTY_STRING, ABILITY_NAME_DEMO, testRet);
    EXPECT_NE(0, ret);
    EXPECT_EQ(EMPTY_STRING, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAbilityLabel_0500
 * @tc.name: test can not get the ability's label if module doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get empty ability label
 */
HWTEST_F(BmsBundleKitServiceTest, GetAbilityLabel_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::string testRet;
    ErrCode ret = GetBundleDataMgr()->GetAbilityLabel(BUNDLE_NAME_TEST, MODULE_NAME_TEST_1, ABILITY_NAME_TEST, testRet);
    EXPECT_NE(0, ret);
    EXPECT_EQ(EMPTY_STRING, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAbilityLabel_0600
 * @tc.name: test can not get the ability's label if module and ability exist
 * @tc.desc: 1.system run normally
 *           2.get empty ability label
 */
HWTEST_F(BmsBundleKitServiceTest, GetAbilityLabel_0600, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::string testRet;
    ErrCode ret = GetBundleDataMgr()->GetAbilityLabel(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, testRet);
    EXPECT_EQ(0, ret);
    EXPECT_EQ(LABEL, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAbilityLabel_0700
 * @tc.name: test can not get the ability's label if module exist and ability doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get empty ability label
 */
HWTEST_F(BmsBundleKitServiceTest, GetAbilityLabel_0700, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::string testRet;
    ErrCode ret = GetBundleDataMgr()->GetAbilityLabel(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_DEMO, testRet);
    EXPECT_NE(0, ret);
    EXPECT_EQ(EMPTY_STRING, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAbilityLabel_0800
 * @tc.name: test can not get the ability's label if module exist and ability doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get empty ability label
 */
HWTEST_F(BmsBundleKitServiceTest, GetAbilityLabel_0800, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::string testRet;
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    ErrCode ret = hostImpl->GetAbilityLabel(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_DEMO, testRet);
    EXPECT_NE(ret, ERR_OK);
    EXPECT_EQ(EMPTY_STRING, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfo_0100
 * @tc.name: test can get the ability info by want
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfo_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo result;

    int32_t flags = GET_ABILITY_INFO_DEFAULT;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfo(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfo(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);

    flags = GET_ABILITY_INFO_WITH_METADATA;
    testRet = GetBundleDataMgr()->QueryAbilityInfo(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfo(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfo_0200
 * @tc.name: test can not get the ability info by want in which element name is wrong
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfo_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    Want want;
    want.SetElementName(BUNDLE_NAME_DEMO, ABILITY_NAME_TEST);
    AbilityInfo result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfo(want, 0, 0, result);
    EXPECT_EQ(false, testRet);

    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_DEMO);
    testRet = GetBundleDataMgr()->QueryAbilityInfo(want, 0, 0, result);
    EXPECT_EQ(false, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfo_0300
 * @tc.name: test can not get the ability info by want which bundle doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfo_0300, Function | SmallTest | Level1)
{
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfo(want, 0, 0, result);
    EXPECT_EQ(false, testRet);
}

/**
 * @tc.number: QueryAbilityInfo_0400
 * @tc.name: test can not get the ability info by want which bundle doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfo_0400, Function | SmallTest | Level1)
{
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    want.SetModuleName(MODULE_NAME_TEST);
    AbilityInfo result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfo(want, 0, 0, result);
    EXPECT_EQ(false, testRet);
}

/**
 * @tc.number: QueryAbilityInfo_0500
 * @tc.name: test can get the ability info by want with bundleName, moduleName, abilityName
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfo_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    want.SetModuleName(MODULE_NAME_TEST);
    AbilityInfo result;

    int32_t flags = GET_ABILITY_INFO_DEFAULT;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfo(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfo(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    EXPECT_EQ(MODULE_NAME_TEST, result.moduleName);

    flags = GET_ABILITY_INFO_WITH_METADATA;
    testRet = GetBundleDataMgr()->QueryAbilityInfo(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfo(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    EXPECT_EQ(MODULE_NAME_TEST, result.moduleName);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfo_0600
 * @tc.name: test can get the ability info by want with bundleName, moduleName, abilityName
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfo_0600, Function | SmallTest | Level1)
{
    std::vector<std::string> moduleList {MODULE_NAME_TEST, MODULE_NAME_TEST_1};
    MockInstallBundle(BUNDLE_NAME_TEST, moduleList, ABILITY_NAME_TEST);
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    want.SetModuleName(MODULE_NAME_TEST);
    AbilityInfo result;
    int32_t flags = GET_ABILITY_INFO_DEFAULT;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfo(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfo(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    EXPECT_EQ(MODULE_NAME_TEST, result.moduleName);

    want.SetModuleName(MODULE_NAME_TEST_1);
    testRet = GetBundleDataMgr()->QueryAbilityInfo(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfo(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    EXPECT_EQ(MODULE_NAME_TEST_1, result.moduleName);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfo_0700
 * @tc.name: test can get the ability info by want with implicit query
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfo_0700, Function | SmallTest | Level1)
{
    std::vector<std::string> moduleList {MODULE_NAME_TEST, MODULE_NAME_TEST_1, MODULE_NAME_TEST_2};
    MockInstallBundle(BUNDLE_NAME_TEST, moduleList, ABILITY_NAME_TEST);
    Want want;
    want.SetAction("action.system.home");
    want.AddEntity("entity.system.home");
    want.SetElementName("", BUNDLE_NAME_TEST, "", MODULE_NAME_TEST_1);
    AbilityInfo result;
    int32_t flags = GET_ABILITY_INFO_DEFAULT;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfo(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfo(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    EXPECT_EQ(MODULE_NAME_TEST_1, result.moduleName);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfo_0800
 * @tc.name: test can get the ability info by want
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfo_0800, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo result;

    int32_t flags = GET_ABILITY_INFO_DEFAULT;
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool testRet = hostImpl->QueryAbilityInfo(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfo(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);

    flags = GET_ABILITY_INFO_WITH_APPLICATION;
    testRet = hostImpl->QueryAbilityInfo(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfo(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfos_0100
 * @tc.name: test can get the ability info of list by want
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfos_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<AbilityInfo> result;

    int32_t flags = GET_ABILITY_INFO_DEFAULT;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfos(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfos(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    result.clear();

    flags = GET_ABILITY_INFO_WITH_METADATA;
    testRet = GetBundleDataMgr()->QueryAbilityInfos(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfos(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    result.clear();

    flags = GET_ABILITY_INFO_WITH_PERMISSION | GET_ABILITY_INFO_WITH_METADATA;
    testRet = GetBundleDataMgr()->QueryAbilityInfos(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfos(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfos_0200
 * @tc.name: test can not get the ability info by want in which bundle name is wrong
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfos_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    Want want;
    want.SetElementName(BUNDLE_NAME_DEMO, ABILITY_NAME_TEST);
    std::vector<AbilityInfo> result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfos(want, 0, 0, result);
    EXPECT_EQ(false, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfos_0300
 * @tc.name: test can not get the ability info by want which bundle doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfos_0300, Function | SmallTest | Level1)
{
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<AbilityInfo> result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfos(want, 0, 0, result);
    EXPECT_EQ(false, testRet);
}

/**
 * @tc.number: QueryAbilityInfos_0400
 * @tc.name: test can not get the ability info by want which bundle doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfos_0400, Function | SmallTest | Level1)
{
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    want.SetModuleName(MODULE_NAME_TEST);
    std::vector<AbilityInfo> result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfos(want, 0, 0, result);
    EXPECT_EQ(false, testRet);
}

/**
 * @tc.number: QueryAbilityInfos_0500
 * @tc.name: test can get the ability info of list by want with bundleName, moduleName and abilityName
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfos_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    want.SetModuleName(MODULE_NAME_TEST);
    std::vector<AbilityInfo> result;

    int32_t flags = GET_ABILITY_INFO_DEFAULT;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfos(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfos(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    for (const auto &ret : result) {
        EXPECT_EQ(MODULE_NAME_TEST, ret.moduleName);
    }
    result.clear();

    flags = GET_ABILITY_INFO_WITH_METADATA;
    testRet = GetBundleDataMgr()->QueryAbilityInfos(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfos(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    for (const auto &ret : result) {
        EXPECT_EQ(MODULE_NAME_TEST, ret.moduleName);
    }
    result.clear();

    flags = GET_ABILITY_INFO_WITH_PERMISSION | GET_ABILITY_INFO_WITH_METADATA;
    testRet = GetBundleDataMgr()->QueryAbilityInfos(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfos(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    for (const auto &ret : result) {
        EXPECT_EQ(MODULE_NAME_TEST, ret.moduleName);
    }
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfos_0600
 * @tc.name: test can get the ability info of list by want with bundleName, moduleName and abilityName
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfos_0600, Function | SmallTest | Level1)
{
    std::vector<std::string> moduleList {MODULE_NAME_TEST, MODULE_NAME_TEST_1};
    MockInstallBundle(BUNDLE_NAME_TEST, moduleList, ABILITY_NAME_TEST);
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    want.SetModuleName(MODULE_NAME_TEST);
    std::vector<AbilityInfo> result;

    int32_t flags = GET_ABILITY_INFO_DEFAULT;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfos(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfos(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    for (const auto &ret : result) {
        EXPECT_EQ(MODULE_NAME_TEST, ret.moduleName);
    }
    result.clear();

    want.SetModuleName(MODULE_NAME_TEST_1);
    testRet = GetBundleDataMgr()->QueryAbilityInfos(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    CheckAbilityInfos(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, flags, result);
    for (const auto &ret : result) {
        EXPECT_EQ(MODULE_NAME_TEST_1, ret.moduleName);
    }
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfos_0700
 * @tc.name: test can get the ability info by want with implicit query
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfos_0700, Function | SmallTest | Level1)
{
    std::vector<std::string> moduleList {MODULE_NAME_TEST, MODULE_NAME_TEST_1, MODULE_NAME_TEST_2};
    MockInstallBundle(BUNDLE_NAME_TEST, moduleList, ABILITY_NAME_TEST);
    Want want;
    want.SetAction("action.system.home");
    want.AddEntity("entity.system.home");
    want.SetElementName("", BUNDLE_NAME_TEST, "", "");
    std::vector<AbilityInfo> result;
    int32_t flags = GET_ABILITY_INFO_DEFAULT;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfos(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    EXPECT_EQ(result.size(), MODULE_NAMES_SIZE_THREE);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfos_0800
 * @tc.name: test can get the ability info by want with implicit query
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfos_0800, Function | SmallTest | Level1)
{
    std::vector<std::string> moduleList {MODULE_NAME_TEST, MODULE_NAME_TEST_1, MODULE_NAME_TEST_2};
    MockInstallBundle(BUNDLE_NAME_TEST, moduleList, ABILITY_NAME_TEST);
    Want want;
    want.SetAction("action.system.home");
    want.AddEntity("entity.system.home");
    want.SetElementName("", BUNDLE_NAME_TEST, "", MODULE_NAME_TEST_1);
    std::vector<AbilityInfo> result;
    int32_t flags = GET_ABILITY_INFO_DEFAULT;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfos(want, flags, 0, result);
    EXPECT_EQ(true, testRet);
    EXPECT_EQ(result.size(), MODULE_NAMES_SIZE_ONE);
    if (!result.empty()) {
        EXPECT_EQ(MODULE_NAME_TEST_1, result[0].moduleName);
    }
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetLaunchWantForBundle_0100
 * @tc.name: test can get the launch want of a bundle
 * @tc.desc: 1.system run normally
 *           2.get launch want successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetLaunchWantForBundle_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    ErrCode testRet = GetBundleDataMgr()->GetLaunchWantForBundle(BUNDLE_NAME_TEST, want);
    EXPECT_EQ(ERR_OK, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetLaunchWantForBundle_0200
 * @tc.name: test can not get the launch want of a bundle which is not exist
 * @tc.desc: 1.system run normally
 *           2.get launch want failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetLaunchWantForBundle_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    ErrCode testRet = GetBundleDataMgr()->GetLaunchWantForBundle(BUNDLE_NAME_DEMO, want);
    EXPECT_NE(ERR_OK, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetLaunchWantForBundle_0300
 * @tc.name: test can not get the launch want of a bundle which its mainability is not exist
 * @tc.desc: 1.system run normally
 *           2.get launch want failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetLaunchWantForBundle_0300, Function | SmallTest | Level1)
{
    std::string bundleName = BUNDLE_NAME_DEMO;
    std::string moduleName = MODULE_NAME_DEMO;
    std::string abilityName = ABILITY_NAME_DEMO;
    InnerModuleInfo moduleInfo = MockModuleInfo(moduleName);
    std::string keyName = bundleName + "." + moduleName + "." + abilityName;
    AbilityInfo abilityInfo = MockAbilityInfo(bundleName, moduleName, abilityName);
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.InsertAbilitiesInfo(keyName, abilityInfo);
    innerBundleInfo.InsertInnerModuleInfo(moduleName, moduleInfo);
    Skill skill;
    std::vector<Skill> skills;
    skills.emplace_back(skill);
    innerBundleInfo.InsertSkillInfo(keyName, skills);
    SaveToDatabase(bundleName, innerBundleInfo, true, false);

    Want want;
    ErrCode testRet = GetBundleDataMgr()->GetLaunchWantForBundle(BUNDLE_NAME_DEMO, want);
    EXPECT_NE(ERR_OK, testRet);

    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetLaunchWantForBundle_0400
 * @tc.name: test can not get the launch want of empty name
 * @tc.desc: 1.system run normally
 *           2.get launch want failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetLaunchWantForBundle_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    Want want;
    ErrCode testRet = GetBundleDataMgr()->GetLaunchWantForBundle(EMPTY_STRING, want);
    EXPECT_NE(ERR_OK, testRet);

    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetLaunchWantForBundle_0500
 * @tc.name: test can not get the launch want when no bundle installed
 * @tc.desc: 1.system run normally
 *           2.get launch want failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetLaunchWantForBundle_0500, Function | SmallTest | Level1)
{
    Want want;
    ErrCode testRet = GetBundleDataMgr()->GetLaunchWantForBundle(BUNDLE_NAME_TEST, want);
    EXPECT_NE(ERR_OK, testRet);
}

/**
 * @tc.number: GetLaunchWantForBundle_0600
 * @tc.name: test can not get the launch want of a bundle which is not exist
 * @tc.desc: 1.system run normally
 *           2.get launch want failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetLaunchWantForBundle_0600, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    ErrCode testRet = bundleMgrProxy->GetLaunchWantForBundle(BUNDLE_NAME_DEMO, want);
    EXPECT_NE(ERR_OK, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetBundleList_0100
 * @tc.name: test can get all installed bundle names
 * @tc.desc: 1.system run normally
 *           2.get installed bundle names successfully with correct names
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleList_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);

    std::vector<std::string> testResult;
    bool testRet = GetBundleDataMgr()->GetBundleList(testResult);
    EXPECT_TRUE(testRet);
    CheckBundleList(BUNDLE_NAME_TEST, testResult);
    CheckBundleList(BUNDLE_NAME_DEMO, testResult);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetBundleNameForUid_0100
 * @tc.name: test can get the bundle names with bundle installed
 * @tc.desc: 1.system run normally
 *           2.get installed bundle names successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleNameForUid_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::string testResult;
    bool testRet = GetBundleDataMgr()->GetBundleNameForUid(TEST_UID, testResult);
    EXPECT_TRUE(testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetBundleNameForUid_0200
 * @tc.name: test can not get not installed bundle name
 * @tc.desc: 1.system run normally
 *           2.get installed bundle name by uid failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleNameForUid_0200, Function | SmallTest | Level1)
{
    std::string testResult;
    bool testRet = GetBundleDataMgr()->GetBundleNameForUid(TEST_UID, testResult);
    EXPECT_FALSE(testRet);
}

/**
 * @tc.number: GetBundleNameForUid_0300
 * @tc.name: test can not get installed bundle name by incorrect uid
 * @tc.desc: 1.system run normally
 *           2.get installed bundle name by uid failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleNameForUid_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::string testResult;
    bool testRet = GetBundleDataMgr()->GetBundleNameForUid(DEMO_UID, testResult);
    EXPECT_FALSE(testRet);
    EXPECT_NE(BUNDLE_NAME_TEST, testResult);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetBundleNameForUid_0400
 * @tc.name: test can not get installed bundle name by invalid uid
 * @tc.desc: 1.system run normally
 *           2.get installed bundle name by uid failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleNameForUid_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);

    std::string testResult;
    bool testRet = GetBundleDataMgr()->GetBundleNameForUid(INVALID_UID, testResult);
    EXPECT_FALSE(testRet);
    EXPECT_NE(BUNDLE_NAME_TEST, testResult);
    EXPECT_NE(BUNDLE_NAME_DEMO, testResult);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: CheckIsSystemAppByUid_0100
 * @tc.name: test can check the installed bundle whether system app or not by uid
 * @tc.desc: 1.system run normally
 *           2.check the installed bundle successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckIsSystemAppByUid_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);

    bool testRet = GetBundleDataMgr()->CheckIsSystemAppByUid(TEST_UID);
    EXPECT_FALSE(testRet);

    testRet = GetBundleDataMgr()->CheckIsSystemAppByUid(DEMO_UID);
    EXPECT_FALSE(testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: CheckIsSystemAppByUid_0200
 * @tc.name: test can check the installed bundle whether system app or not by uid
 * @tc.desc: 1.system run normally
 *           2.check the installed bundle successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckIsSystemAppByUid_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    bool testRet = GetBundleDataMgr()->CheckIsSystemAppByUid(INVALID_UID);
    EXPECT_FALSE(testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckIsSystemAppByUid_0300
 * @tc.name: Test CheckIsSystemAppByUid
 * @tc.desc: 1.Test the CheckIsSystemAppByUid by BundleMgrHostImpl
 */
HWTEST_F(BmsBundleKitServiceTest, CheckIsSystemAppByUid_0300, Function | MediumTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    std::vector<int> gids;
    auto ret = hostImpl->CheckIsSystemAppByUid(TEST_UID);
    EXPECT_EQ(ret, false);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: DUMP_0100
 * @tc.name: Dump bundlelist, bundle info for bundleName
 * @tc.desc: 1.system run normally
 *           2.dump info with one mock installed bundles
 */
HWTEST_F(BmsBundleKitServiceTest, DUMP_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();

    std::string infoResult;
    bool infoRet = hostImpl->DumpInfos(
        DumpFlag::DUMP_BUNDLE_INFO, BUNDLE_NAME_TEST, DEFAULT_USERID, infoResult);
    EXPECT_TRUE(infoRet);
    EXPECT_NE(std::string::npos, infoResult.find(BUNDLE_NAME_TEST));
    EXPECT_NE(std::string::npos, infoResult.find(MODULE_NAME_TEST));
    EXPECT_NE(std::string::npos, infoResult.find(ABILITY_NAME_TEST));

    std::string bundleNames;
    bool listRet = hostImpl->DumpInfos(
        DumpFlag::DUMP_BUNDLE_LIST, EMPTY_STRING, DEFAULT_USERID, bundleNames);
    EXPECT_TRUE(listRet);
    EXPECT_NE(std::string::npos, bundleNames.find(BUNDLE_NAME_TEST));

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_TEST, ABILITY_NAME_DEMO);

    std::string names;
    bool namesRet = hostImpl->DumpInfos(
        DumpFlag::DUMP_BUNDLE_LIST, EMPTY_STRING, DEFAULT_USERID, names);
    EXPECT_TRUE(namesRet);
    EXPECT_NE(std::string::npos, names.find(BUNDLE_NAME_DEMO));

    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: DUMP_0200
 * @tc.name: Dump empty bundle info for empty bundle name
 * @tc.desc: 1.system run normally
 *           2.dump with empty bundle name
 */
HWTEST_F(BmsBundleKitServiceTest, DUMP_0200, Function | SmallTest | Level0)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    std::string emptyResult;
    bool emptyRet = hostImpl->DumpInfos(
        DumpFlag::DUMP_BUNDLE_INFO, EMPTY_STRING, DEFAULT_USERID, emptyResult);
    EXPECT_FALSE(emptyRet);
    emptyRet = hostImpl->DumpInfos(
        DumpFlag::DUMP_BUNDLE_INFO, BUNDLE_NAME_TEST, DEFAULT_USERID, emptyResult);
    EXPECT_TRUE(emptyRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: DUMP_0300
 * @tc.name: Dump bundlelist, bundle info for bundleName
 * @tc.desc: 1.system run normally
 *           2.dump info with 2 installed bundles
 */
HWTEST_F(BmsBundleKitServiceTest, DUMP_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    std::string bundleNames;
    bool listRet = hostImpl->DumpInfos(
        DumpFlag::DUMP_BUNDLE_LIST, EMPTY_STRING, DEFAULT_USERID, bundleNames);
    EXPECT_TRUE(listRet);
    EXPECT_NE(std::string::npos, bundleNames.find(BUNDLE_NAME_DEMO));
    EXPECT_NE(std::string::npos, bundleNames.find(BUNDLE_NAME_TEST));

    std::string bundleInfo;
    bool infoRet = hostImpl->DumpInfos(
        DumpFlag::DUMP_BUNDLE_INFO, BUNDLE_NAME_TEST, DEFAULT_USERID, bundleInfo);
    EXPECT_TRUE(infoRet);
    EXPECT_NE(std::string::npos, bundleInfo.find(BUNDLE_NAME_TEST));

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: DUMP_0400
 * @tc.name: Dump empty bundle info for empty bundle name
 * @tc.desc: 1.system run normally
 *           2.dump with empty bundle name
 */
HWTEST_F(BmsBundleKitServiceTest, DUMP_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    std::string emptyResult;
    bool emptyRet = bundleMgrProxy->DumpInfos(
        DumpFlag::DUMP_BUNDLE_INFO, EMPTY_STRING, DEFAULT_USERID, emptyResult);
    EXPECT_FALSE(emptyRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: DUMP_0500
 * @tc.name: Dump empty bundle info for empty bundle name
 * @tc.desc: 1.system run normally
 *           2.dump with empty bundle name
 */
HWTEST_F(BmsBundleKitServiceTest, DUMP_0500, Function | SmallTest | Level0)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    std::string emptyResult;
    bool emptyRet = hostImpl->DumpInfos(
        DumpFlag::DUMP_SHORTCUT_INFO, EMPTY_STRING, DEFAULT_USERID, emptyResult);
    EXPECT_FALSE(emptyRet);
    emptyRet = hostImpl->DumpInfos(
        DumpFlag::DUMP_SHORTCUT_INFO, BUNDLE_NAME_TEST, DEFAULT_USERID, emptyResult);
    EXPECT_TRUE(emptyRet);
    emptyRet = hostImpl->DumpInfos(
        DumpFlag::DUMP_SHORTCUT_INFO, BUNDLE_NAME_TEST, ALL_USERID, emptyResult);
    EXPECT_TRUE(emptyRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: DUMP_0600
 * @tc.name: Dump bundlelist, bundle info for bundleName
 * @tc.desc: 1.system run normally
 *           2.dump with empty bundle name
 */
HWTEST_F(BmsBundleKitServiceTest, DUMP_0600, Function | SmallTest | Level0)
{
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    std::string emptyResult;
    bool emptyRet = hostImpl->DumpInfos(
        DumpFlag::DUMP_BUNDLE_LIST, EMPTY_STRING, ALL_USERID, emptyResult);
    EXPECT_TRUE(emptyRet);
}

/**
 * @tc.number: GetSandboxBundleInfo_0100
 * @tc.name: GetSandboxBundleInfo
 * @tc.desc: 1.test GetSandboxBundleInfo
 */
HWTEST_F(BmsBundleKitServiceTest, GetSandboxBundleInfo_0100, Function | SmallTest | Level0)
{
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    std::string bundleName = "";
    int32_t appIndex = 100;
    std::string bundleName1 = BUNDLE_NAME_TEST;
    int32_t appIndex1 = 1000;
    int32_t appIndex2 = -1;
    BundleInfo info;
    bool emptyRet = hostImpl->GetSandboxBundleInfo(bundleName, appIndex, DEFAULT_USERID, info);
    EXPECT_NE(emptyRet, ERR_OK);
    emptyRet = hostImpl->GetSandboxBundleInfo(bundleName1, appIndex1, DEFAULT_USERID, info);
    EXPECT_NE(emptyRet, ERR_OK);
    emptyRet = hostImpl->GetSandboxBundleInfo(bundleName1, appIndex2, DEFAULT_USERID, info);
    EXPECT_NE(emptyRet, ERR_OK);
}

/**
 * @tc.number: QueryAbilityInfoByUri_0100
 * @tc.name: test can get the ability info by uri
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfoByUri_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfoByUri(
        ABILITY_URI, DEFAULT_USERID, result);
    EXPECT_EQ(true, testRet);
    EXPECT_EQ(ABILITY_NAME_TEST, result.name);
    EXPECT_NE(ABILITY_NAME_DEMO, result.name);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfoByUri_0200
 * @tc.name: test can get the ability infos by uri
 * @tc.desc: 1.system run normally
 *           2.get ability info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfoByUri_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfoByUri(
        ABILITY_URI, DEFAULT_USERID, result);
    EXPECT_EQ(true, testRet);
    EXPECT_EQ(ABILITY_NAME_TEST, result.name);
    EXPECT_NE(ABILITY_NAME_DEMO, result.name);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: QueryAbilityInfoByUri_0300
 * @tc.name: test can not get the ability info by uri which bundle doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */

HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfoByUri_0300, Function | SmallTest | Level1)
{
    AbilityInfo result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfoByUri(
        ABILITY_URI, DEFAULT_USERID, result);
    EXPECT_EQ(false, testRet);
}

/**
 * @tc.number: QueryAbilityInfoByUri_0400
 * @tc.name: test can not get the ability info by empty uri
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfoByUri_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfoByUri(
        "",  DEFAULT_USERID, result);
    EXPECT_EQ(false, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfoByUri_0500
 * @tc.name: test can not get the ability info by error uri
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfoByUri_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo result;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfoByUri(
        ERROR_URI,  DEFAULT_USERID, result);
    EXPECT_EQ(false, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfoByUri_0600
 * @tc.name: test can get the ability info by uri
 * @tc.desc: 1.system run normally
 *           2.get ability info failed by zero userid
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfoByUri_0600, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo result;
    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    bool testRet = bundleMgrProxy->QueryAbilityInfoByUri(
        ABILITY_URI, 0, result);
    EXPECT_EQ(false, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfoByUri_0700
 * @tc.name: test can get the ability info by uri
 * @tc.desc: 1.system run normally
 *           2.get ability info failed by empty ability uri
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfoByUri_0700, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo result;
    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    bool testRet = bundleMgrProxy->QueryAbilityInfoByUri(
        "", result);
    EXPECT_EQ(false, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfoByUri_0800
 * @tc.name: test can get the ability info by uri
 * @tc.desc: 1.system run normally
 *           2.get ability info failed by empty ability uri
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfoByUri_0800, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo result;
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool testRet = hostImpl->QueryAbilityInfoByUri(
        ABILITY_URI, DEFAULT_USERID, result);
    EXPECT_EQ(true, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfoByUri_0900
 * @tc.name: test can get the ability info by uri
 * @tc.desc: 1.system run normally
 *           2.get ability info failed by empty ability uri
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfoByUri_0900, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo result;
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool testRet = hostImpl->QueryAbilityInfoByUri(
        ABILITY_URI, DEFAULT_USERID, result);
    EXPECT_EQ(true, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfosByUri_0100
 * @tc.name: test QueryAbilityInfosByUri by BundleMgrHostImpl
 * @tc.desc: 1.system run normally
 *           2.ability not found
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfosByUri_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    std::string uri = "invalid";
    std::vector<AbilityInfo> abilityInfos;
    bool ret = hostImpl->QueryAbilityInfosByUri(uri, abilityInfos);
    EXPECT_FALSE(ret);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAbilityInfosByUri_0100
 * @tc.name: test QueryAbilityInfosByUri by BundleMgrHostImpl
 * @tc.desc: 1.system run normally
 *           2.ability not found
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfosByUri_0200, Function | SmallTest | Level1)
{
    auto dataMgr = GetBundleDataMgr();
    std::string uri = "dataability://com.example.hiworld.himusic.UserADataAbility";
    std::vector<AbilityInfo> abilityInfos;
    InnerBundleInfo innerBundleInfo;
    GetBundleDataMgr()->bundleInfos_.insert(
        pair<std::string, InnerBundleInfo>("moduleName", innerBundleInfo));
    EXPECT_FALSE(GetBundleDataMgr()->bundleInfos_.empty());
    bool ret = dataMgr->QueryAbilityInfosByUri(uri, abilityInfos);
    EXPECT_FALSE(ret);
}

/**
 * @tc.number: QueryExtensionAbilityInfoByUri_0100
 * @tc.name: test can get the extensio ability info by uri
 * @tc.desc: 1.system run normally
 *           2.uri not include :///, invalid
 */
HWTEST_F(BmsBundleKitServiceTest, QueryExtensionAbilityInfoByUri_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo result;
    ExtensionAbilityInfo extensionAbilityInfo;
    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    bool testRet = bundleMgrProxy->QueryExtensionAbilityInfoByUri(
        URI, DEFAULT_USERID, extensionAbilityInfo);
    EXPECT_EQ(false, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryExtensionAbilityInfoByUri_0100
 * @tc.name: test can get the extensio ability info by uri
 * @tc.desc: 1.system run normally
 *           2.query info failed by empty uri
 */
HWTEST_F(BmsBundleKitServiceTest, QueryExtensionAbilityInfoByUri_0200, Function | SmallTest | Level1)
{
    AbilityInfo result;
    ExtensionAbilityInfo extensionAbilityInfo;
    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    bool testRet = bundleMgrProxy->QueryExtensionAbilityInfoByUri(
        "", DEFAULT_USERID, extensionAbilityInfo);
    EXPECT_EQ(false, testRet);
}

/**
 * @tc.number: QueryKeepAliveBundleInfos_0100
 * @tc.name: test can get the keep alive bundle infos
 * @tc.desc: 1.system run normally
 *           2.get all keep alive bundle infos successfully
 */
HWTEST_F(BmsBundleKitServiceTest, QueryKeepAliveBundleInfos_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<BundleInfo> bundleInfos;
    bool ret = GetBundleDataMgr()->QueryKeepAliveBundleInfos(bundleInfos);
    EXPECT_EQ(true, ret);
    CheckInstalledBundleInfos(ABILITY_SIZE_ONE, bundleInfos);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: QueryKeepAliveBundleInfos_0200
 * @tc.name: test can not get the keep alive bundle info which bundle doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get bundle info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryKeepAliveBundleInfos_0200, Function | SmallTest | Level1)
{
    std::vector<BundleInfo> bundleInfos;
    bool ret = GetBundleDataMgr()->QueryKeepAliveBundleInfos(bundleInfos);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number: GetBundleArchiveInfo_0200
 * @tc.name: test can not get the bundle archive info by empty hap file path
 * @tc.desc: 1.system run normally
 *           2.get the bundle archive info failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleArchiveInfo_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    BundleInfo testResult;
    bool listRet = hostImpl->GetBundleArchiveInfo("", BundleFlag::GET_BUNDLE_DEFAULT, testResult);
    EXPECT_FALSE(listRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryKeepAliveBundleInfos_0300
 * @tc.name: test can not get the keep alive bundle info which bundle doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get bundle info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryKeepAliveBundleInfos_0300, Function | SmallTest | Level1)
{
    std::vector<BundleInfo> bundleInfos;
    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    bool ret = bundleMgrProxy->QueryKeepAliveBundleInfos(bundleInfos);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: QueryKeepAliveBundleInfos_0400
 * @tc.name: test can not get the keep alive bundle info which bundle doesn't exist
 * @tc.desc: 1.system run normally
 *           2.get result failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryKeepAliveBundleInfos_0400, Function | SmallTest | Level1)
{
    std::vector<BundleInfo> bundleInfos;
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool ret = hostImpl->QueryKeepAliveBundleInfos(bundleInfos);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number: GetBundleArchiveInfo_0300
 * @tc.name: test can not get the bundle archive info by no exist hap file path
 * @tc.desc: 1.system run normally
 *           2.get the bundle archive info failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleArchiveInfo_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    BundleInfo testResult;
    bool listRet = hostImpl->GetBundleArchiveInfo(ERROR_HAP_FILE_PATH, BundleFlag::GET_BUNDLE_DEFAULT, testResult);
    EXPECT_FALSE(listRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetHapModuleInfo_0100
 * @tc.name: test can get the hap module info
 * @tc.desc: 1.system run normally
 *           2.get the hap module info successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetHapModuleInfo_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo abilityInfo;
    abilityInfo.bundleName = BUNDLE_NAME_TEST;
    abilityInfo.package = PACKAGE_NAME;

    HapModuleInfo hapModuleInfo;
    bool ret = GetBundleDataMgr()->GetHapModuleInfo(abilityInfo, hapModuleInfo);
    EXPECT_EQ(true, ret);
    CheckModuleInfo(hapModuleInfo);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetHapModuleInfo_0200
 * @tc.name: test can not get the hap module info by no exist bundleName
 * @tc.desc: 1.system run normally
 *           2.get the hap module info failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetHapModuleInfo_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo abilityInfo;
    abilityInfo.bundleName = BUNDLE_NAME_DEMO;
    abilityInfo.package = PACKAGE_NAME;

    HapModuleInfo hapModuleInfo;
    bool ret = GetBundleDataMgr()->GetHapModuleInfo(abilityInfo, hapModuleInfo);
    EXPECT_EQ(false, ret);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetHapModuleInfo_0300
 * @tc.name: test can not get the hap module info by no exist package
 * @tc.desc: 1.system run normally
 *           2.get the hap module info failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetHapModuleInfo_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo abilityInfo;
    abilityInfo.bundleName = BUNDLE_NAME_TEST;
    abilityInfo.package = BUNDLE_NAME_DEMO;

    HapModuleInfo hapModuleInfo;
    bool ret = GetBundleDataMgr()->GetHapModuleInfo(abilityInfo, hapModuleInfo);
    EXPECT_EQ(false, ret);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetHapModuleInfo_0400
 * @tc.name: test get the hap module info by userId
 * @tc.desc: 1.system run normally
 *           2.get the hap module info failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetHapModuleInfo_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo abilityInfo;
    abilityInfo.bundleName = BUNDLE_NAME_TEST;
    abilityInfo.package = PACKAGE_NAME;
    HapModuleInfo hapModuleInfo;
    bool ret = GetBundleDataMgr()->GetHapModuleInfo(abilityInfo, hapModuleInfo, DEFAULT_USER_ID_TEST);
    EXPECT_EQ(true, ret);

    ret = GetBundleDataMgr()->GetHapModuleInfo(abilityInfo, hapModuleInfo, NEW_USER_ID_TEST);
    EXPECT_EQ(false, ret);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetHapModuleInfo_0500
 * @tc.name: test can get the hap module info
 * @tc.desc: 1.system run normally
 *           2.get the hap module info failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetHapModuleInfo_0500, Function | SmallTest | Level1)
{
    AbilityInfo abilityInfo1;
    abilityInfo1.bundleName = "";
    abilityInfo1.package = PACKAGE_NAME;

    AbilityInfo abilityInfo2;
    abilityInfo2.bundleName = BUNDLE_NAME_TEST;
    abilityInfo2.package = "";

    AbilityInfo abilityInfo3;
    abilityInfo3.bundleName = "";
    abilityInfo3.package = "";

    HapModuleInfo hapModuleInfo;
    bool ret = GetBundleDataMgr()->GetHapModuleInfo(abilityInfo1, hapModuleInfo);
    EXPECT_FALSE(ret);

    ret = GetBundleDataMgr()->GetHapModuleInfo(abilityInfo2, hapModuleInfo);
    EXPECT_FALSE(ret);

    ret = GetBundleDataMgr()->GetHapModuleInfo(abilityInfo3, hapModuleInfo);
    EXPECT_FALSE(ret);
}

/**
 * @tc.number: CheckApplicationEnabled_0100
 * @tc.name: test can check bundle status is enable by no setting
 * @tc.desc: 1.system run normally
 *           2.check the bundle status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckApplicationEnabled_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    bool isEnable = false;
    int32_t ret = GetBundleDataMgr()->IsApplicationEnabled(BUNDLE_NAME_TEST, isEnable);
    EXPECT_EQ(0, ret);
    EXPECT_TRUE(isEnable);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckApplicationEnabled_0200
 * @tc.name: test can check bundle status is enable by setting
 * @tc.desc: 1.system run normally
 *           2.check the bundle status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckApplicationEnabled_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    int32_t testRet = GetBundleDataMgr()->SetApplicationEnabled(BUNDLE_NAME_TEST, true, Constants::DEFAULT_USERID);
    EXPECT_EQ(0, testRet);
    bool isEnable = false;
    int32_t ret = GetBundleDataMgr()->IsApplicationEnabled(BUNDLE_NAME_TEST, isEnable);
    EXPECT_EQ(0, ret);
    EXPECT_TRUE(isEnable);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckApplicationEnabled_0300
 * @tc.name: test can check bundle status is disable by setting
 * @tc.desc: 1.system run normally
 *           2.check the bundle status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckApplicationEnabled_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    int32_t testRet = GetBundleDataMgr()->SetApplicationEnabled(BUNDLE_NAME_TEST, false, Constants::DEFAULT_USERID);
    EXPECT_EQ(0, testRet);
    bool isEnable = false;
    int32_t ret = GetBundleDataMgr()->IsApplicationEnabled(BUNDLE_NAME_TEST, isEnable);
    EXPECT_EQ(0, ret);
    EXPECT_FALSE(isEnable);

    int32_t testRet2 = GetBundleDataMgr()->SetApplicationEnabled(BUNDLE_NAME_TEST, true, Constants::DEFAULT_USERID);
    EXPECT_EQ(0, testRet2);
    ret = GetBundleDataMgr()->IsApplicationEnabled(BUNDLE_NAME_TEST, isEnable);
    EXPECT_EQ(0, ret);
    EXPECT_TRUE(isEnable);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckApplicationEnabled_0400
 * @tc.name: test can check bundle status is disable by no install
 * @tc.desc: 1.system run normally
 *           2.check the bundle status failed
 */
HWTEST_F(BmsBundleKitServiceTest, CheckApplicationEnabled_0400, Function | SmallTest | Level1)
{
    bool isEnable = false;
    int32_t ret = GetBundleDataMgr()->IsApplicationEnabled(BUNDLE_NAME_TEST, isEnable);
    EXPECT_NE(0, ret);
}

/**
 * @tc.number: CheckApplicationEnabled_0500
 * @tc.name: test can check bundle status is able by empty bundle name
 * @tc.desc: 1.system run normally
 *           2.check the bundle status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckApplicationEnabled_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    int32_t testRet = GetBundleDataMgr()->SetApplicationEnabled("", true, Constants::DEFAULT_USERID);
    EXPECT_NE(0, testRet);
    bool isEnable = false;
    int32_t ret = GetBundleDataMgr()->IsApplicationEnabled(BUNDLE_NAME_TEST, isEnable);
    EXPECT_EQ(0, ret);
    EXPECT_TRUE(isEnable);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckApplicationEnabled_0600
 * @tc.name: test can check bundle status is disable by empty bundle name
 * @tc.desc: 1.system run normally
 *           2.check the bundle status failed
 */
HWTEST_F(BmsBundleKitServiceTest, CheckApplicationEnabled_0600, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    int32_t testRet = GetBundleDataMgr()->SetApplicationEnabled(BUNDLE_NAME_TEST, true, Constants::DEFAULT_USERID);
    EXPECT_EQ(0, testRet);
    bool isEnable = false;
    int32_t testRet1 = GetBundleDataMgr()->IsApplicationEnabled("", isEnable);
    EXPECT_NE(0, testRet1);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckApplicationEnabled_0700
 * @tc.name: test can check bundle status is enable by setting
 * @tc.desc: 1.system run normally
 *           2.check the bundle status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckApplicationEnabled_0700, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    int32_t testRet = bundleMgrProxy->SetApplicationEnabled("", true, Constants::DEFAULT_USERID);
    EXPECT_NE(0, testRet);
    bool isEnable = false;
    int32_t testRet1 = bundleMgrProxy->IsApplicationEnabled("", isEnable);
    EXPECT_NE(0, testRet1);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckApplicationEnabled_0800
 * @tc.name: test can check bundle status is able by empty bundle name
 * @tc.desc: 1.system run normally
 *           2.check the bundle status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckApplicationEnabled_0800, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    ErrCode testRet = hostImpl->SetApplicationEnabled("", true, Constants::UNSPECIFIED_USERID);
    EXPECT_EQ(testRet, ERR_BUNDLE_MANAGER_BUNDLE_NOT_EXIST);
    bool isEnable = false;
    ErrCode ret = hostImpl->IsApplicationEnabled(BUNDLE_NAME_TEST, isEnable);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_TRUE(isEnable);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckApplicationEnabled_0900
 * @tc.name: test can check bundle status is able by empty bundle name
 * @tc.desc: 1.system run normally
 *           2.check the bundle status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckApplicationEnabled_0900, Function | SmallTest | Level1)
{
    ErrCode testRet = GetBundleDataMgr()->SetApplicationEnabled("", true, Constants::INVALID_USERID);
    EXPECT_EQ(testRet, ERR_BUNDLE_MANAGER_BUNDLE_NOT_EXIST);
}

/**
 * @tc.number: GetBundleInfosByMetaData_0100
 * @tc.name: test can get the bundle infos by metadata
 * @tc.desc: 1.system run normally
 *           2.get bundle infos successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfosByMetaData_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<BundleInfo> bundleInfos;
    bool testRet = GetBundleDataMgr()->GetBundleInfosByMetaData(META_DATA, bundleInfos);
    EXPECT_EQ(true, testRet);
    CheckInstalledBundleInfos(ABILITY_SIZE_ONE, bundleInfos);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetBundleInfosByMetaData_0200
 * @tc.name: test can not get the bundle infos by empty metadata
 * @tc.desc: 1.system run normally
 *           2.get bundle infos failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfosByMetaData_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<BundleInfo> bundleInfos;
    bool testRet = GetBundleDataMgr()->GetBundleInfosByMetaData("", bundleInfos);
    EXPECT_EQ(false, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetBundleInfosByMetaData_0300
 * @tc.name: test can not get the bundle infos by no exist metadata
 * @tc.desc: 1.system run normally
 *           2.get bundle infos failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfosByMetaData_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<BundleInfo> bundleInfos;
    bool testRet = GetBundleDataMgr()->GetBundleInfosByMetaData(ERROR_META_DATA, bundleInfos);
    EXPECT_EQ(false, testRet);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetBundleInfosByMetaData_0400
 * @tc.name: test can get the bundle infos by metadata
 * @tc.desc: 1.system run normally
 *           2.get bundle infos successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfosByMetaData_0400, Function | MediumTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    std::vector<BundleInfo> bundleInfos;
    auto ret = hostImpl->GetBundleInfosByMetaData(META_DATA, bundleInfos);
    EXPECT_EQ(ret, true);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanBundleDataFiles_0100
 * @tc.name: test can clean the bundle data files by bundle name
 * @tc.desc: 1.system run normally
 *           2.clean the bundle data files successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CleanBundleDataFiles_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    CreateFileDir();

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool testRet = hostImpl->CleanBundleDataFiles(BUNDLE_NAME_TEST, DEFAULT_USERID);
    EXPECT_TRUE(testRet);

    CleanFileDir();
    CheckFileNonExist();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanBundleDataFiles_0200
 * @tc.name: test can clean the bundle data files by empty bundle name
 * @tc.desc: 1.system run normally
 *           2.clean the bundle data files failed
 */
HWTEST_F(BmsBundleKitServiceTest, CleanBundleDataFiles_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    CreateFileDir();

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool testRet = hostImpl->CleanBundleDataFiles("", DEFAULT_USERID);
    EXPECT_FALSE(testRet);
    CheckFileExist();

    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanBundleDataFiles_0300
 * @tc.name: test can clean the bundle data files by no exist bundle name
 * @tc.desc: 1.system run normally
 *           2.clean the bundle data files failed
 */
HWTEST_F(BmsBundleKitServiceTest, CleanBundleDataFiles_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    CreateFileDir();

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool testRet = hostImpl->CleanBundleDataFiles(BUNDLE_NAME_DEMO, DEFAULT_USERID);
    EXPECT_FALSE(testRet);
    CheckFileExist();

    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanBundleDataFiles_0400
 * @tc.name: test can clean the bundle data files by bundle name
 * @tc.desc: 1.system run normally
 *           2.userDataClearable is false, isSystemApp is false
 *           3.clean the cache files succeed
 */
HWTEST_F(BmsBundleKitServiceTest, CleanBundleDataFiles_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, false, false);
    CreateFileDir();

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool testRet = hostImpl->CleanBundleDataFiles(BUNDLE_NAME_TEST, DEFAULT_USERID);
    EXPECT_TRUE(testRet);

    CleanFileDir();
    CheckFileNonExist();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanBundleDataFiles_0500
 * @tc.name: test can clean the bundle data files by bundle name
 * @tc.desc: 1.system run normally
 *           2.userDataClearable is true, isSystemApp is false
 *           3.clean the cache files succeed
 */
HWTEST_F(BmsBundleKitServiceTest, CleanBundleDataFiles_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, true, false);
    CreateFileDir();

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool testRet = hostImpl->CleanBundleDataFiles(BUNDLE_NAME_TEST, DEFAULT_USERID);
    EXPECT_TRUE(testRet);

    CleanFileDir();
    CheckFileNonExist();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanBundleDataFiles_0600
 * @tc.name: test can clean the bundle data files by bundle name
 * @tc.desc: 1.system run normally
 *           2.userDataClearable is false, isSystemApp is true
 *           3.clean the cache files failed
 */
HWTEST_F(BmsBundleKitServiceTest, CleanBundleDataFiles_0600, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, false, true);
    CreateFileDir();

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool testRet = hostImpl->CleanBundleDataFiles(BUNDLE_NAME_TEST, DEFAULT_USERID);
    EXPECT_FALSE(testRet);

    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanBundleDataFiles_0700
 * @tc.name: test can clean the bundle data files by bundle name
 * @tc.desc: 1.system run normally
 *           2.userDataClearable is true, isSystemApp is true
 *           3.clean the cache files failed
 */
HWTEST_F(BmsBundleKitServiceTest, CleanBundleDataFiles_0700, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, true, true);
    CreateFileDir();

    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bool testRet = bundleMgrProxy->CleanBundleDataFiles(BUNDLE_NAME_TEST, DEFAULT_USERID);
    EXPECT_FALSE(testRet);

    CleanFileDir();
    CheckFileNonExist();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanBundleDataFiles_0800
 * @tc.name: test can clean the bundle data files by bundle name
 * @tc.desc: 1.system run normally
 *           2.clean the bundle data files successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CleanBundleDataFiles_0800, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    CreateFileDir();

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool testRet = hostImpl->CleanBundleDataFiles(BUNDLE_NAME_TEST, DEFAULT_USERID);
    EXPECT_TRUE(testRet);

    CleanFileDir();
    CheckFileNonExist();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanBundleDataFiles_0900
 * @tc.name: test can clean the bundle data files by bundle name
 * @tc.desc: 1.system run normally
 *           2.clean the bundle data files failed by empty bundle name
 */
HWTEST_F(BmsBundleKitServiceTest, CleanBundleDataFiles_0900, Function | SmallTest | Level1)
{
    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bool testRet = bundleMgrProxy->CleanBundleDataFiles("", DEFAULT_USERID);
    EXPECT_FALSE(testRet);
}

/**
 * @tc.number: CleanBundleDataFiles_1000
 * @tc.name: test can clean the bundle data files by bundle name
 * @tc.desc: 1.system run normally
 *           2.userDataClearable is false, userId is false
 *           3.clean the cache files failed
 */
HWTEST_F(BmsBundleKitServiceTest, CleanBundleDataFiles_1000, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, false, false);
    CreateFileDir();

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool testRet = hostImpl->CleanBundleDataFiles(BUNDLE_NAME_TEST, 10001);
    EXPECT_FALSE(testRet);

    CleanFileDir();
    CheckFileNonExist();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanCache_0200
 * @tc.name: test can clean the cache files by empty bundle name
 * @tc.desc: 1.system run normally
 *           2.clean the cache files failed
 */
HWTEST_F(BmsBundleKitServiceTest, CleanCache_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    CreateFileDir();

    sptr<MockCleanCache> cleanCache = new (std::nothrow) MockCleanCache();
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    auto result = hostImpl->CleanBundleCacheFiles("", cleanCache);
    EXPECT_FALSE(result == ERR_OK);
    CheckCacheExist();

    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanCache_0300
 * @tc.name: test can clean the cache files by no exist bundle name
 * @tc.desc: 1.system run normally
 *           2.clean the cache files failed
 */
HWTEST_F(BmsBundleKitServiceTest, CleanCache_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    CreateFileDir();

    sptr<MockCleanCache> cleanCache = new (std::nothrow) MockCleanCache();
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    auto result = hostImpl->CleanBundleCacheFiles("wrong", cleanCache);
    EXPECT_FALSE(result == ERR_OK);
    CheckCacheExist();

    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanCache_0400
 * @tc.name: test can clean the cache files
 * @tc.desc: 1.system run normally
 *           2. userDataClearable is true, isSystemApp is false
 *           3.clean the cache files succeed
 */
HWTEST_F(BmsBundleKitServiceTest, CleanCache_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, true, false);
    CreateFileDir();

    sptr<MockCleanCache> cleanCache = new (std::nothrow) MockCleanCache();
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    auto result = hostImpl->CleanBundleCacheFiles(BUNDLE_NAME_TEST, cleanCache);
    EXPECT_TRUE(result == ERR_OK);

    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanCache_0500
 * @tc.name: test can clean the cache files
 * @tc.desc: 1.system run normally
 *           2. userDataClearable is false, isSystemApp is false
 *           3.clean the cache files succeed
 */
HWTEST_F(BmsBundleKitServiceTest, CleanCache_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, false, false);
    CreateFileDir();

    sptr<MockCleanCache> cleanCache = new (std::nothrow) MockCleanCache();
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    auto result = hostImpl->CleanBundleCacheFiles(BUNDLE_NAME_TEST, cleanCache);
    EXPECT_TRUE(result == ERR_OK);

    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanCache_0600
 * @tc.name: test can clean the cache files
 * @tc.desc: 1.system run normally
 *           2. userDataClearable is true, isSystemApp is true
 *           3.clean the cache files succeed
 */
HWTEST_F(BmsBundleKitServiceTest, CleanCache_0600, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, true, true);
    CreateFileDir();

    sptr<MockCleanCache> cleanCache = new (std::nothrow) MockCleanCache();
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    auto result = hostImpl->CleanBundleCacheFiles(BUNDLE_NAME_TEST, cleanCache);
    EXPECT_TRUE(result == ERR_OK);

    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanCache_0700
 * @tc.name: test can clean the cache files
 * @tc.desc: 1.system run normally
 *           2. userDataClearable is false, isSystemApp is true
 *           3.clean the cache files failed
 */
HWTEST_F(BmsBundleKitServiceTest, CleanCache_0700, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, false, true);
    CreateFileDir();

    sptr<MockCleanCache> cleanCache = new (std::nothrow) MockCleanCache();
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    auto result = hostImpl->CleanBundleCacheFiles(BUNDLE_NAME_TEST, cleanCache);
    EXPECT_FALSE(result == ERR_OK);
    CheckCacheExist();

    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanCache_0800
 * @tc.name: test can clean the cache files
 * @tc.desc: 1.system run normally
 *           2. userDataClearable is false, isSystemApp is false
 *           3.clean the cache files succeed
 */
HWTEST_F(BmsBundleKitServiceTest, CleanCache_0800, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, false, true);
    CreateFileDir();

    sptr<MockCleanCache> cleanCache = new (std::nothrow) MockCleanCache();
    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    auto result = bundleMgrProxy->CleanBundleCacheFiles(BUNDLE_NAME_TEST, cleanCache);
    EXPECT_FALSE(result == ERR_OK);

    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanCache_0900
 * @tc.name: test can clean the cache files
 * @tc.desc: 1.system run normally
 *           2.userDataClearable is false, isSystemApp is false
 *           3.clean the cache files failed by empty name
 */
HWTEST_F(BmsBundleKitServiceTest, CleanCache_0900, Function | SmallTest | Level1)
{
    sptr<MockCleanCache> cleanCache = new (std::nothrow) MockCleanCache();
    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    ErrCode result = bundleMgrProxy->CleanBundleCacheFiles("", cleanCache);
    EXPECT_NE(result, ERR_OK);
}

/**
 * @tc.number: CleanCache_1000
 * @tc.name: test can clean the cache files
 * @tc.desc: 1.system run normally
 *           2. userDataClearable is false, isSystemApp is false
 *           3.clean the cache files failed by nullptr cleaCache
 */
HWTEST_F(BmsBundleKitServiceTest, CleanCache_1000, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, false, true);
    CreateFileDir();

    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    ErrCode result = bundleMgrProxy->CleanBundleCacheFiles(BUNDLE_NAME_TEST, nullptr);
    EXPECT_NE(result, ERR_OK);

    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanCache_1100
 * @tc.name: test can clean the cache files with failed userId
 * @tc.desc: 1.system run normally
 *           2. userDataClearable is false, isSystemApp is false
 *           3. clean the cache files failed by failed userId
 */
HWTEST_F(BmsBundleKitServiceTest, CleanCache_1100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, false, true);
    CreateFileDir();

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    int32_t failedId = -100;
    sptr<MockCleanCache> cleanCache = new (std::nothrow) MockCleanCache();
    ErrCode result = hostImpl->CleanBundleCacheFiles(BUNDLE_NAME_TEST, cleanCache, failedId);
    EXPECT_EQ(result, ERR_BUNDLE_MANAGER_INVALID_USER_ID);

    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CleanCache_1200
 * @tc.name: test can clean the cache files
 * @tc.desc: 1.system run normally
 *           2. userDataClearable is false, dataMgr is nullptr
 *           3.clean the cache files failed
 */
HWTEST_F(BmsBundleKitServiceTest, CleanCache_1200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, false, false);
    CreateFileDir();

    sptr<MockCleanCache> cleanCache = new (std::nothrow) MockCleanCache();
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    ClearDataMgr();
    auto result = hostImpl->CleanBundleCacheFiles(BUNDLE_NAME_TEST, cleanCache);
    EXPECT_EQ(result, ERR_APPEXECFWK_SERVICE_INTERNAL_ERROR);

    SetDataMgr();
    CleanFileDir();
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: RegisterBundleStatus_0100
 * @tc.name: test can register the bundle status by bundle name
 * @tc.desc: 1.system run normally
 *           2.bundle status callback successfully
 */
HWTEST_F(BmsBundleKitServiceTest, RegisterBundleStatus_0100, Function | SmallTest | Level1)
{
    sptr<MockBundleStatus> bundleStatusCallback = new (std::nothrow) MockBundleStatus();
    bundleStatusCallback->SetBundleName(HAP_FILE_PATH);
    bool result = GetBundleDataMgr()->RegisterBundleStatusCallback(bundleStatusCallback);
    EXPECT_TRUE(result);
    EXPECT_NE(commonEventMgr_, nullptr);
    commonEventMgr_->NotifyBundleStatus(installRes_, GetBundleDataMgr());

    int32_t callbackResult = bundleStatusCallback->GetResultCode();
    EXPECT_EQ(callbackResult, ERR_OK);
}

/**
 * @tc.number: RegisterBundleStatus_0200
 * @tc.name: test can register the bundle status by bundle name
 * @tc.desc: 1.system run normally
 *           2.bundle status callback failed by empty bundle name
 */
HWTEST_F(BmsBundleKitServiceTest, RegisterBundleStatus_0200, Function | SmallTest | Level1)
{
    sptr<MockBundleStatus> bundleStatusCallback = new (std::nothrow) MockBundleStatus();
    bundleStatusCallback->SetBundleName(HAP_FILE_PATH);
    bool result = GetBundleDataMgr()->RegisterBundleStatusCallback(bundleStatusCallback);
    EXPECT_TRUE(result);
    installRes_.bundleName = "";
    EXPECT_NE(commonEventMgr_, nullptr);
    commonEventMgr_->NotifyBundleStatus(installRes_, GetBundleDataMgr());

    int32_t callbackResult = bundleStatusCallback->GetResultCode();
    EXPECT_EQ(callbackResult, ERR_TIMED_OUT);
}

/**
 * @tc.number: RegisterBundleStatus_0300
 * @tc.name: test can register the bundle status by bundle name
 * @tc.desc: 1.system run normally
 *           2.bundle status callback failed by no exist bundle name
 */
HWTEST_F(BmsBundleKitServiceTest, RegisterBundleStatus_0300, Function | SmallTest | Level1)
{
    sptr<MockBundleStatus> bundleStatusCallback = new (std::nothrow) MockBundleStatus();
    bundleStatusCallback->SetBundleName(HAP_FILE_PATH);
    bool result = GetBundleDataMgr()->RegisterBundleStatusCallback(bundleStatusCallback);
    EXPECT_TRUE(result);
    installRes_.bundleName = ERROR_HAP_FILE_PATH;
    EXPECT_NE(commonEventMgr_, nullptr);
    commonEventMgr_->NotifyBundleStatus(installRes_, GetBundleDataMgr());

    int32_t callbackResult = bundleStatusCallback->GetResultCode();
    EXPECT_EQ(callbackResult, ERR_TIMED_OUT);
}

/**
 * @tc.number: RegisterBundleStatus_0400
 * @tc.name: test can register the bundle status by empty bundle name
 * @tc.desc: 1.system run normally
 *           2.bundle status callback failed
 */
HWTEST_F(BmsBundleKitServiceTest, RegisterBundleStatus_0400, Function | SmallTest | Level1)
{
    sptr<MockBundleStatus> bundleStatusCallback = new (std::nothrow) MockBundleStatus();
    bundleStatusCallback->SetBundleName("");
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool result = hostImpl->RegisterBundleStatusCallback(bundleStatusCallback);
    EXPECT_FALSE(result);
}

/**
 * @tc.number: RegisterBundleStatus_0500
 * @tc.name: test can not register, the bundle status dataMgr is nullptr
 * @tc.desc: 1.system run normally
 *           2.bundle status callback failed
 */
HWTEST_F(BmsBundleKitServiceTest, RegisterBundleStatus_0500, Function | SmallTest | Level1)
{
    sptr<MockBundleStatus> bundleStatusCallback = new (std::nothrow) MockBundleStatus();
    bundleStatusCallback->SetBundleName(HAP_FILE_PATH);
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    ClearDataMgr();
    bool result = hostImpl->RegisterBundleStatusCallback(bundleStatusCallback);
    EXPECT_FALSE(result);
    SetDataMgr();
}

/**
 * @tc.number: RegisterBundleEventCallback_0100
 * @tc.name: test can not register, the bundle status bundleEventCallback is nullptr
 * @tc.desc: 1.system run normally
 *           2.bundle status callback failed
 */
HWTEST_F(BmsBundleKitServiceTest, RegisterBundleEventCallback_0100, Function | SmallTest | Level1)
{
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool result = hostImpl->RegisterBundleEventCallback(nullptr);
    EXPECT_FALSE(result);
}

/**
 * @tc.number: UnregisterBundleEventCallback_0100
 * @tc.name: test can not unregister, the bundle status bundleEventCallback is nullptr
 * @tc.desc: 1.system run normally
 *           2.bundle status callback failed
 */
HWTEST_F(BmsBundleKitServiceTest, UnregisterBundleEventCallback_0100, Function | SmallTest | Level1)
{
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool result = hostImpl->UnregisterBundleEventCallback(nullptr);
    EXPECT_FALSE(result);
}

/**
 * @tc.number: ClearBundleStatus_0100
 * @tc.name: test can clear the bundle status by bundle name
 * @tc.desc: 1.system run normally
 *           2.bundle status callback failed by cleared bundle name
 */
HWTEST_F(BmsBundleKitServiceTest, ClearBundleStatus_0100, Function | SmallTest | Level1)
{
    sptr<MockBundleStatus> bundleStatusCallback1 = new (std::nothrow) MockBundleStatus();
    bundleStatusCallback1->SetBundleName(HAP_FILE_PATH1);
    bool result1 = GetBundleDataMgr()->RegisterBundleStatusCallback(bundleStatusCallback1);
    EXPECT_TRUE(result1);

    bool result2 = GetBundleDataMgr()->ClearBundleStatusCallback(bundleStatusCallback1);
    EXPECT_TRUE(result2);

    sptr<MockBundleStatus> bundleStatusCallback = new (std::nothrow) MockBundleStatus();
    bundleStatusCallback->SetBundleName(HAP_FILE_PATH);
    bool result = GetBundleDataMgr()->RegisterBundleStatusCallback(bundleStatusCallback);
    EXPECT_TRUE(result);
    installRes_.bundleName = HAP_FILE_PATH;
    EXPECT_NE(commonEventMgr_, nullptr);
    commonEventMgr_->NotifyBundleStatus(installRes_, GetBundleDataMgr());

    int32_t callbackResult = bundleStatusCallback->GetResultCode();
    EXPECT_EQ(callbackResult, ERR_OK);

    installRes_.bundleName = HAP_FILE_PATH1;
    commonEventMgr_->NotifyBundleStatus(installRes_, GetBundleDataMgr());

    int32_t callbackResult1 = bundleStatusCallback1->GetResultCode();
    EXPECT_EQ(callbackResult1, ERR_TIMED_OUT);
}

/**
 * @tc.number: ClearBundleStatus_0200
 * @tc.name: test can not unregister, the bundle status dataMgr is nullptr
 * @tc.desc: 1.system run normally
 *           2.bundle status callback failed
 */
HWTEST_F(BmsBundleKitServiceTest, ClearBundleStatus_0200, Function | SmallTest | Level1)
{
    sptr<MockBundleStatus> bundleStatusCallback = new (std::nothrow) MockBundleStatus();
    bundleStatusCallback->SetBundleName(HAP_FILE_PATH);
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    ClearDataMgr();
    bool result = hostImpl->ClearBundleStatusCallback(bundleStatusCallback);
    EXPECT_FALSE(result);
    SetDataMgr();
}

/**
 * @tc.number: UnregisterBundleStatus_0100
 * @tc.name: test can unregister the bundle status by bundle name
 * @tc.desc: 1.system run normally
 *           2.bundle status callback failed by unregister bundle name
 */
HWTEST_F(BmsBundleKitServiceTest, UnregisterBundleStatus_0100, Function | SmallTest | Level1)
{
    sptr<MockBundleStatus> bundleStatusCallback = new (std::nothrow) MockBundleStatus();
    bundleStatusCallback->SetBundleName(HAP_FILE_PATH);
    bool result = GetBundleDataMgr()->RegisterBundleStatusCallback(bundleStatusCallback);
    EXPECT_TRUE(result);

    sptr<MockBundleStatus> bundleStatusCallback1 = new (std::nothrow) MockBundleStatus();
    bundleStatusCallback1->SetBundleName(HAP_FILE_PATH1);
    bool result1 = GetBundleDataMgr()->RegisterBundleStatusCallback(bundleStatusCallback1);
    EXPECT_TRUE(result1);

    bool result2 = GetBundleDataMgr()->UnregisterBundleStatusCallback();
    EXPECT_TRUE(result2);
    installRes_.bundleName = HAP_FILE_PATH;
    EXPECT_NE(commonEventMgr_, nullptr);
    commonEventMgr_->NotifyBundleStatus(installRes_, GetBundleDataMgr());

    int32_t callbackResult = bundleStatusCallback->GetResultCode();
    EXPECT_EQ(callbackResult, ERR_TIMED_OUT);

    installRes_.bundleName = HAP_FILE_PATH1;
    commonEventMgr_->NotifyBundleStatus(installRes_, GetBundleDataMgr());

    int32_t callbackResult1 = bundleStatusCallback1->GetResultCode();
    EXPECT_EQ(callbackResult1, ERR_TIMED_OUT);
}

/**
 * @tc.number: GetBundlesForUid_0100
 * @tc.name: test can get the bundle names with bundle installed
 * @tc.desc: 1.system run normally
 *           2.get installed bundle names successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundlesForUid_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<std::string> testResult;
    bool testRet = GetBundleDataMgr()->GetBundlesForUid(TEST_UID, testResult);
    EXPECT_TRUE(testRet);
    EXPECT_EQ(BUNDLE_NAMES_SIZE_ONE, testResult.size());
    if (testResult.size() > 0) {
        EXPECT_EQ(BUNDLE_NAME_TEST, testResult[0]);
    }

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetBundlesForUid_0200
 * @tc.name: test can not get not installed bundle names
 * @tc.desc: 1.system run normally
 *           2.get installed bundle names by uid failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundlesForUid_0200, Function | SmallTest | Level1)
{
    std::vector<std::string> testResult;
    bool testRet = GetBundleDataMgr()->GetBundlesForUid(TEST_UID, testResult);
    EXPECT_FALSE(testRet);
    EXPECT_EQ(BUNDLE_NAMES_SIZE_ZERO, testResult.size());
}

/**
 * @tc.number: GetBundlesForUid_0300
 * @tc.name: test can not get installed bundle names by incorrect uid
 * @tc.desc: 1.system run normally
 *           2.get installed bundle names by uid failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundlesForUid_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::vector<std::string> testResult;
    bool testRet = GetBundleDataMgr()->GetBundlesForUid(DEMO_UID, testResult);
    EXPECT_FALSE(testRet);
    EXPECT_EQ(BUNDLE_NAMES_SIZE_ZERO, testResult.size());

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetBundlesForUid_0400
 * @tc.name: test can not get installed bundle names by invalid uid
 * @tc.desc: 1.system run normally
 *           2.get installed bundle names by uid failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundlesForUid_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);

    std::vector<std::string> testResult;
    bool testRet = GetBundleDataMgr()->GetBundlesForUid(INVALID_UID, testResult);
    EXPECT_FALSE(testRet);
    EXPECT_EQ(BUNDLE_NAMES_SIZE_ZERO, testResult.size());

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetBundlesForUid_0500
 * @tc.name: test can get the bundle names with bundle installed
 * @tc.desc: 1.system run normally
 *           2.get installed bundle names successfully
 */
    HWTEST_F(BmsBundleKitServiceTest, GetBundlesForUid_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    std::vector<std::string> testResult;
    testResult.emplace_back(BUNDLE_NAME_DEMO);
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool testRet = hostImpl->GetBundlesForUid(TEST_UID, testResult);
    EXPECT_TRUE(testRet);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetNameForUid_0100
 * @tc.name: test can get the uid name with bundle installed
 * @tc.desc: 1.system run normally
 *           2.get installed uid name successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetNameForUid_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::string testResult;
    ErrCode testRet = GetBundleDataMgr()->GetNameForUid(TEST_UID, testResult);
    EXPECT_EQ(testRet, ERR_OK);
    EXPECT_EQ(BUNDLE_NAME_TEST, testResult);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetNameForUid_0200
 * @tc.name: test can not get not installed uid name
 * @tc.desc: 1.system run normally
 *           2.get installed uid name by uid failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetNameForUid_0200, Function | SmallTest | Level1)
{
    std::string testResult;
    ErrCode testRet = GetBundleDataMgr()->GetNameForUid(TEST_UID, testResult);
    EXPECT_NE(testRet, ERR_OK);
    EXPECT_NE(BUNDLE_NAME_TEST, testResult);
}

/**
 * @tc.number: GetNameForUid_0300
 * @tc.name: test can not get installed uid name by incorrect uid
 * @tc.desc: 1.system run normally
 *           2.get installed uid name by uid failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetNameForUid_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::string testResult;
    ErrCode testRet = GetBundleDataMgr()->GetNameForUid(DEMO_UID, testResult);
    EXPECT_NE(testRet, ERR_OK);
    EXPECT_NE(BUNDLE_NAME_TEST, testResult);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetNameForUid_0400
 * @tc.name: test can not get installed uid name by invalid uid
 * @tc.desc: 1.system run normally
 *           2.get installed uid name by uid failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetNameForUid_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);

    std::string testResult;
    ErrCode testRet = GetBundleDataMgr()->GetNameForUid(INVALID_UID, testResult);
    EXPECT_NE(testRet, ERR_OK);
    EXPECT_NE(BUNDLE_NAME_TEST, testResult);
    EXPECT_NE(BUNDLE_NAME_DEMO, testResult);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetNameForUid_0500
 * @tc.name: test can get the uid name with bundle installed
 * @tc.desc: 1.system run normally
 *           2.get installed uid name successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetNameForUid_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    std::string testResult;
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    ErrCode testRet = hostImpl->GetNameForUid(TEST_UID, testResult);
    EXPECT_EQ(testRet, ERR_OK);
    EXPECT_EQ(BUNDLE_NAME_TEST, testResult);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_0100
 * @tc.name: test can check ability status is enable by no setting
 * @tc.desc: 1.system run normally
 *           2.check the ability status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo = MockAbilityInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    bool isEnable = false;
    int32_t testRet = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo, isEnable);
    EXPECT_EQ(0, testRet);
    EXPECT_TRUE(isEnable);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_0200
 * @tc.name: test can check ability status is enable by setting
 * @tc.desc: 1.system run normally
 *           2.check the ability status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo = MockAbilityInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    int32_t testRet = GetBundleDataMgr()->SetAbilityEnabled(abilityInfo, true, Constants::DEFAULT_USERID);
    EXPECT_TRUE(testRet == 0);
    bool isEnable = false;
    int32_t testRet1 = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo, isEnable);
    EXPECT_EQ(0, testRet1);
    EXPECT_TRUE(isEnable);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_0300
 * @tc.name: test can check ability status is disable by setting and can be enabled again
 * @tc.desc: 1.system run normally
 *           2.check the ability status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo = MockAbilityInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    int32_t testRet = GetBundleDataMgr()->SetAbilityEnabled(abilityInfo, false, Constants::DEFAULT_USERID);
    EXPECT_EQ(0, testRet);
    bool isEnable = false;
    int32_t testRet1 = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo, isEnable);
    EXPECT_EQ(0, testRet1);
    EXPECT_FALSE(isEnable);
    int32_t testRet2 = GetBundleDataMgr()->SetAbilityEnabled(abilityInfo, true, Constants::DEFAULT_USERID);
    EXPECT_EQ(0, testRet2);
    int32_t testRet3 = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo, isEnable);
    EXPECT_EQ(0, testRet3);
    EXPECT_TRUE(isEnable);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_0400
 * @tc.name: test can check ability status is disable by no install
 * @tc.desc: 1.system run normally
 *           2.check the ability status failed
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_0400, Function | SmallTest | Level1)
{
    AbilityInfo abilityInfo = MockAbilityInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    bool isEnable = false;
    int32_t testRet = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo, isEnable);
    EXPECT_NE(0, testRet);
}

/**
 * @tc.number: CheckAbilityEnabled_0500
 * @tc.name: test can check ability status is able by empty AbilityInfo
 * @tc.desc: 1.system run normally
 *           2.check the ability status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    AbilityInfo abilityInfoEmpty;
    int32_t testRet = GetBundleDataMgr()->SetAbilityEnabled(abilityInfoEmpty, false, Constants::DEFAULT_USERID);
    EXPECT_NE(0, testRet);
    AbilityInfo abilityInfo = MockAbilityInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    bool isEnable = false;
    int32_t testRet1 = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo, isEnable);
    EXPECT_EQ(0, testRet1);
    EXPECT_TRUE(isEnable);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_0600
 * @tc.name: test can check ability status is disable by empty AbilityInfo
 * @tc.desc: 1.system run normally
 *           2.check the ability status failed
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_0600, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo = MockAbilityInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    int32_t testRet = GetBundleDataMgr()->SetAbilityEnabled(abilityInfo, true, Constants::DEFAULT_USERID);
    EXPECT_EQ(0, testRet);
    AbilityInfo abilityInfoEmpty;
    bool isEnable = false;
    int32_t testRet1 = GetBundleDataMgr()->IsAbilityEnabled(abilityInfoEmpty, isEnable);
    EXPECT_NE(0, testRet1);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_0700
 * @tc.name: test can check ability status is enable by empty moduleName
 * @tc.desc: 1.system run normally
 *           2.check the ability status successful
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_0700, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo;
    abilityInfo.name = ABILITY_NAME_TEST;
    abilityInfo.bundleName = BUNDLE_NAME_TEST;
    int32_t testRet = GetBundleDataMgr()->SetAbilityEnabled(abilityInfo, true, Constants::DEFAULT_USERID);
    EXPECT_EQ(0, testRet);
    bool isEnable = false;
    int32_t testRet1 = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo, isEnable);
    EXPECT_EQ(0, testRet1);
    EXPECT_TRUE(isEnable);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_0800
 * @tc.name: test can check ability status is disable by moduleName
 * @tc.desc: 1.system run normally
 *           2.check the ability status successful
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_0800, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo;
    abilityInfo.name = ABILITY_NAME_TEST;
    abilityInfo.bundleName = BUNDLE_NAME_TEST;
    abilityInfo.moduleName = MODULE_NAME_TEST;
    int32_t testRet = GetBundleDataMgr()->SetAbilityEnabled(abilityInfo, true, Constants::DEFAULT_USERID);
    EXPECT_EQ(0, testRet);
    bool isEnable = false;
    int32_t testRet1 = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo, isEnable);
    EXPECT_EQ(0, testRet1);
    EXPECT_TRUE(isEnable);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_0900
 * @tc.name: test can check ability status is disable by moduleName
 * @tc.desc: 1.system run normally
 *           2.check the ability status failed
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_0900, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo;
    abilityInfo.name = ABILITY_NAME_TEST;
    abilityInfo.bundleName = BUNDLE_NAME_TEST;
    abilityInfo.moduleName = MODULE_NAME_TEST_1;
    int32_t testRet = GetBundleDataMgr()->SetAbilityEnabled(abilityInfo, true, Constants::DEFAULT_USERID);
    EXPECT_FALSE(testRet == 0);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_1000
 * @tc.name: test can check ability status is enable by no setting
 * @tc.desc: 1.system run normally
 *           2.check the ability status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_1000, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo;
    abilityInfo.name = ABILITY_NAME_TEST;
    abilityInfo.bundleName = BUNDLE_NAME_TEST;
    bool isEnable = false;
    int32_t testRet = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo, isEnable);
    EXPECT_EQ(0, testRet);
    EXPECT_TRUE(isEnable);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_1100
 * @tc.name: test can check ability status is enable by no setting
 * @tc.desc: 1.system run normally
 *           2.check the ability status successfully
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_1100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo;
    abilityInfo.name = ABILITY_NAME_TEST;
    abilityInfo.bundleName = BUNDLE_NAME_TEST;
    abilityInfo.moduleName = MODULE_NAME_TEST;
    bool isEnable = false;
    int32_t testRet = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo, isEnable);
    EXPECT_EQ(0, testRet);
    EXPECT_TRUE(isEnable);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_1200
 * @tc.name: test can check ability status is enable by wrong moduleName
 * @tc.desc: 1.system run normally
 *           2.check the ability status failed
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_1200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo;
    abilityInfo.name = ABILITY_NAME_TEST;
    abilityInfo.bundleName = BUNDLE_NAME_TEST;
    abilityInfo.moduleName = MODULE_NAME_TEST_1;
    bool isEnable = false;
    int32_t testRet = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo, isEnable);
    EXPECT_NE(0, testRet);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_1300
 * @tc.name: test can check ability status is disable by empty AbilityInfo
 * @tc.desc: 1.system run normally
 *           2.check the ability status failed
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_1300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo = MockAbilityInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    int32_t testRet = GetBundleDataMgr()->SetAbilityEnabled(abilityInfo, true, Constants::DEFAULT_USERID);
    EXPECT_EQ(0, testRet);
    AbilityInfo abilityInfoEmpty;
    bool isEnable = false;
    int32_t testRet1 = GetBundleDataMgr()->IsAbilityEnabled(abilityInfoEmpty, isEnable);
    EXPECT_NE(0, testRet1);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_1400
 * @tc.name: test can check ability status is disable by empty AbilityInfo
 * @tc.desc: 1.system run normally
 *           2.check the ability status failed
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_1400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo = MockAbilityInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    int32_t testRet = hostImpl->SetAbilityEnabled(abilityInfo, true, Constants::DEFAULT_USERID);
    EXPECT_EQ(0, testRet);
    AbilityInfo abilityInfoEmpty;
    bool isEnable = false;
    int32_t testRet1 = hostImpl->IsAbilityEnabled(abilityInfoEmpty, isEnable);
    EXPECT_NE(0, testRet1);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: CheckAbilityEnabled_1500
 * @tc.name: test can check ability status is enable by empty moduleName
 * @tc.desc: 1.system run normally
 *           2.check the ability status successful
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAbilityEnabled_1500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo;
    abilityInfo.name = ABILITY_NAME_TEST;
    abilityInfo.bundleName = BUNDLE_NAME_TEST;
    int32_t testRet = GetBundleDataMgr()->SetAbilityEnabled(abilityInfo, true, Constants::UNSPECIFIED_USERID);
    EXPECT_NE(0, testRet);
    bool isEnable = false;
    int32_t testRet1 = GetBundleDataMgr()->IsAbilityEnabled(abilityInfo, isEnable);
    EXPECT_EQ(0, testRet1);
    EXPECT_TRUE(isEnable);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetFormInfoByModule_0100
 * @tc.name: test can  get the formInfo
 * @tc.desc: 1.system run normally
 *           2.get formInfo by moduleName successful
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByModule_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfos;
    auto result = GetBundleDataMgr()->GetFormsInfoByModule(BUNDLE_NAME_TEST, MODULE_NAME_TEST, formInfos);
    EXPECT_EQ(result, true);
    EXPECT_FALSE(formInfos.empty());
    CheckFormInfoTest(formInfos);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetFormInfoByModule_0200
 * @tc.name: test can get the formInfo
 * @tc.desc: 1.system run normally
 *           2.get forms info in different bundle
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByModule_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfo1;
    auto result = GetBundleDataMgr()->GetFormsInfoByModule(BUNDLE_NAME_TEST, MODULE_NAME_TEST, formInfo1);
    EXPECT_EQ(result, true);
    EXPECT_FALSE(formInfo1.empty());
    CheckFormInfoTest(formInfo1);

    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfo2;
    auto result1 = GetBundleDataMgr()->GetFormsInfoByModule(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, formInfo2);
    EXPECT_EQ(result1, true);
    EXPECT_FALSE(formInfo2.empty());
    CheckFormInfoDemo(formInfo2);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetFormInfoByModule_0400
 * @tc.name: test can not get the formInfo
 * @tc.desc:1.system run normally
 *          2.get form info with error moduleName
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByModule_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfos;
    GetBundleDataMgr()->GetFormsInfoByModule(BUNDLE_NAME_TEST, MODULE_NAME_DEMO, formInfos);
    EXPECT_TRUE(formInfos.empty());
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetFormInfoByModule_0400
 * @tc.name: test can not get the formInfo
 * @tc.desc: 1.system run normally
 *           2.get form info with empty moduleName
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByModule_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_DEMO);
    std::vector<FormInfo> formInfos;
    GetBundleDataMgr()->GetFormsInfoByModule(BUNDLE_NAME_TEST, EMPTY_STRING, formInfos);
    EXPECT_TRUE(formInfos.empty());
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetFormInfoByModule_0500
 * @tc.name: test can not get the formInfo
 * @tc.desc: 1.system run normally
 *           2.get form info with non bundle
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByModule_0500, Function | SmallTest | Level1)
{
    std::vector<FormInfo> formInfos;
    GetBundleDataMgr()->GetFormsInfoByModule("", EMPTY_STRING, formInfos);
    EXPECT_TRUE(formInfos.empty());
}

/**
 * @tc.number: GetFormInfoByModule_0600
 * @tc.name: test can not get the formInfo
 * @tc.desc:1.system run normally
 *          2.get form info with empty moduleName
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByModule_0600, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfos;
    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    auto result = bundleMgrProxy->GetFormsInfoByModule(BUNDLE_NAME_TEST, "", formInfos);
    EXPECT_EQ(result, false);
    EXPECT_TRUE(formInfos.empty());
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetFormInfoByModule_0900
 * @tc.name: test can  get the formInfo
 * @tc.desc: 1.system run normally
 *           2.get formInfo by moduleName successful
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByModule_0900, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfos;
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    auto result = hostImpl->GetFormsInfoByModule(BUNDLE_NAME_TEST, MODULE_NAME_TEST, formInfos);
    EXPECT_EQ(result, true);
    result = hostImpl->GetFormsInfoByModule("com.ohos.error", MODULE_NAME_TEST, formInfos);
    EXPECT_EQ(result, false);
    EXPECT_FALSE(formInfos.empty());
    CheckFormInfoTest(formInfos);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetFormInfoByApp_0100
 * @tc.name: test can get the formInfo by bundlename
 * @tc.desc: 1.system run normally
 *           2.get form info by bundleName
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByApp_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfos;
    auto result = GetBundleDataMgr()->GetFormsInfoByApp(BUNDLE_NAME_TEST, formInfos);
    EXPECT_EQ(result, true);
    EXPECT_FALSE(formInfos.empty());
    CheckFormInfoTest(formInfos);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetFormInfoByApp_0200
 * @tc.name: test can get the formInfo in different app by bundlename
 * @tc.desc: 1.system run normally
 *           2.get form info by different bundleName
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByApp_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfo1;
    auto result = GetBundleDataMgr()->GetFormsInfoByApp(BUNDLE_NAME_TEST, formInfo1);
    EXPECT_EQ(result, true);
    EXPECT_FALSE(formInfo1.empty());
    CheckFormInfoTest(formInfo1);

    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfo2;
    auto result1 = GetBundleDataMgr()->GetFormsInfoByApp(BUNDLE_NAME_DEMO, formInfo2);
    EXPECT_EQ(result1, true);
    EXPECT_FALSE(formInfo2.empty());
    CheckFormInfoDemo(formInfo2);

    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetFormInfoByApp_0300
 * @tc.name: test can't get the formInfo in app by error app name
 * @tc.desc: 1.system run normally
 *           2.get form info by error bundleName
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByApp_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfo1;
    GetBundleDataMgr()->GetFormsInfoByApp(BUNDLE_NAME_DEMO, formInfo1);
    EXPECT_TRUE(formInfo1.empty());
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetFormInfoByApp_0400
 * @tc.name: test can't get the formInfo in app by empty app name
 * @tc.desc: 1.system run normally
 *           2.get form info by empty bundleName
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByApp_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfo1;
    GetBundleDataMgr()->GetFormsInfoByApp(EMPTY_STRING, formInfo1);
    EXPECT_TRUE(formInfo1.empty());
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetFormInfoByApp_0500
 * @tc.name: test can't get the formInfo have no bundle
 * @tc.desc: 1.system run normally
 *           2.get form info with non bundle
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByApp_0500, Function | SmallTest | Level1)
{
    std::vector<FormInfo> formInfo1;
    GetBundleDataMgr()->GetFormsInfoByApp("", formInfo1);
    EXPECT_TRUE(formInfo1.empty());
}

/**
 * @tc.number: GetFormInfoByApp_0600
 * @tc.name: test can get the formInfo by bundlename
 * @tc.desc: 1.system run normally
 *           2.get form info fail by empty bundleName
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByApp_0600, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfos;
    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    auto result = bundleMgrProxy->GetFormsInfoByApp("", formInfos);
    EXPECT_EQ(result, false);
    EXPECT_TRUE(formInfos.empty());
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetFormInfoByApp_0700
 * @tc.name: test can't get the formInfo have no bundle
 * @tc.desc: 1.system run normally
 *           2.get form info with non bundle
 */
HWTEST_F(BmsBundleKitServiceTest, GetFormInfoByApp_0700, Function | SmallTest | Level1)
{
    std::vector<FormInfo> formInfo;
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    auto ret = hostImpl->GetFormsInfoByApp("", formInfo);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.number: GetDistributedBundleInfo_0100
 * @tc.name: GetDistributedBundleInfo
 * @tc.desc: 1.system run normally
 *           2.test GetDistributedBundleInfo
 */
HWTEST_F(BmsBundleKitServiceTest, GetDistributedBundleInfo_0100, Function | SmallTest | Level1)
{
    std::string networkId = "";
    const std::string bundleName = "";
    DistributedBundleInfo distributedBundleInfo;
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    auto ret = hostImpl->GetDistributedBundleInfo(bundleName, bundleName, distributedBundleInfo);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.number: GetBundleArchiveInfoBySandBoxPath_0100
 * @tc.name: Test GetBundleArchiveInfoBySandBoxPath
 * @tc.desc: 1.Test the GetBundleUserInfo by BundleMgrHostImpl
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleArchiveInfoBySandBoxPath_0100, Function | SmallTest | Level1)
{
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    std::string hapFilePath = "";
    int32_t flags = 1;
    BundleInfo bundleInfo;
    bool fromV9 = false;
    auto ret = hostImpl->GetBundleArchiveInfoBySandBoxPath(
        hapFilePath, flags, bundleInfo, fromV9);
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.number: GetAllFormInfo_0100
 * @tc.name: test can get all the formInfo
 * @tc.desc: 1.system run normally
 *           2.get forms by all the bundle
 */
HWTEST_F(BmsBundleKitServiceTest, GetAllFormInfo_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfos;
    auto result = GetBundleDataMgr()->GetAllFormsInfo(formInfos);
    EXPECT_EQ(result, true);
    EXPECT_FALSE(formInfos.empty());
    for (const auto &info : formInfos) {
        if (info.bundleName == BUNDLE_NAME_TEST) {
            std::vector<FormInfo> formInfo;
            formInfo.emplace_back(info);
            CheckFormInfoTest(formInfo);
            break;
        }
    }
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAllFormInfo_0200
 * @tc.name: test can get all the formInfo
 * @tc.desc: 1.system run normally
 *           2.get forms by all the bundle
 */
HWTEST_F(BmsBundleKitServiceTest, GetAllFormInfo_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfos;
    auto result = GetBundleDataMgr()->GetAllFormsInfo(formInfos);
    EXPECT_FALSE(formInfos.empty());
    EXPECT_EQ(result, true);
    for (const auto &info : formInfos) {
        if (info.bundleName == MODULE_NAME_DEMO) {
            std::vector<FormInfo> formInfo1;
            formInfo1.emplace_back(info);
            CheckFormInfoDemo(formInfo1);
        } else if (info.bundleName == MODULE_NAME_TEST){
            std::vector<FormInfo> formInfo2;
            formInfo2.emplace_back(info);
            CheckFormInfoTest(formInfo2);
        }
    }
    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetAllFormInfo_0300
 * @tc.name: test can get all the formInfo
 * @tc.desc: 1.system run normally
 *           2.get forms by all the bundle
 */
HWTEST_F(BmsBundleKitServiceTest, GetAllFormInfo_0301, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfos;
    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    auto result = bundleMgrProxy->GetAllFormsInfo(formInfos);
    EXPECT_EQ(result, true);
    EXPECT_TRUE(formInfos.empty());
    for (const auto &info : formInfos) {
        if (info.bundleName == BUNDLE_NAME_TEST) {
            std::vector<FormInfo> formInfo;
            formInfo.emplace_back(info);
            CheckFormInfoTest(formInfo);
            break;
        }
    }
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAllFormInfo_0400
 * @tc.name: test can get all the formInfo
 * @tc.desc: 1.system run normally
 *           2.get forms by all the bundle
 */
HWTEST_F(BmsBundleKitServiceTest, GetAllFormInfo_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<FormInfo> formInfos;
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    auto result = hostImpl->GetAllFormsInfo(formInfos);
    EXPECT_EQ(result, true);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetShortcutInfos_0100
 * @tc.name: test can get shortcutInfo by bundleName
 * @tc.desc: 1.can get shortcutInfo
 */
HWTEST_F(BmsBundleKitServiceTest, GetShortcutInfos_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<ShortcutInfo> shortcutInfos;
    auto result = GetBundleDataMgr()->GetShortcutInfos(
        BUNDLE_NAME_TEST,  DEFAULT_USERID, shortcutInfos);
    EXPECT_TRUE(result);
    CheckShortcutInfoTest(shortcutInfos);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetShortcutInfos_0200
 * @tc.name: test have two bundle can get shortcutInfo by bundleName
 * @tc.desc: 1.can get shortcutInfo
 */
HWTEST_F(BmsBundleKitServiceTest, GetShortcutInfos_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_TEST);
    std::vector<ShortcutInfo> shortcutInfo1;
    auto result1 = GetBundleDataMgr()->GetShortcutInfos(
        BUNDLE_NAME_TEST,  DEFAULT_USERID, shortcutInfo1);
    EXPECT_TRUE(result1);
    CheckShortcutInfoTest(shortcutInfo1);
    std::vector<ShortcutInfo> shortcutInfo2;
    auto result2 = GetBundleDataMgr()->GetShortcutInfos(
        BUNDLE_NAME_DEMO,  DEFAULT_USERID, shortcutInfo2);
    EXPECT_TRUE(result2);
    CheckShortcutInfoDemo(shortcutInfo2);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetShortcutInfos_0300
 * @tc.name: test can't get the shortcutInfo in app by error app name
 * @tc.desc: 1.have not get shortcutinfo by appName
 */
HWTEST_F(BmsBundleKitServiceTest, GetShortcutInfos_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<ShortcutInfo> shortcutInfos;
    GetBundleDataMgr()->GetShortcutInfos(
        BUNDLE_NAME_DEMO,  DEFAULT_USERID, shortcutInfos);
    EXPECT_TRUE(shortcutInfos.empty());
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetShortcutInfos_0400
 * @tc.name: test can't get the shortcutInfo in app by null app name
 * @tc.desc: 1.have not get shortcutinfo by null app name
 */
HWTEST_F(BmsBundleKitServiceTest, GetShortcutInfos_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<ShortcutInfo> shortcutInfos;
    GetBundleDataMgr()->GetShortcutInfos(
        EMPTY_STRING,  DEFAULT_USERID, shortcutInfos);
    EXPECT_TRUE(shortcutInfos.empty());
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetShortcutInfos_0500
 * @tc.name: test can't get the shortcutInfo have no bundle
 * @tc.desc: 1.have not get shortcutInfo by appName
 *           2.can't get shortcutInfo
 */
HWTEST_F(BmsBundleKitServiceTest, GetShortcutInfos_0500, Function | SmallTest | Level1)
{
    std::vector<ShortcutInfo> shortcutInfos;
    GetBundleDataMgr()->GetShortcutInfos(
        BUNDLE_NAME_TEST, DEFAULT_USERID, shortcutInfos);
    EXPECT_TRUE(shortcutInfos.empty());
}

/**
 * @tc.number: GetShortcutInfos_0600
 * @tc.name: test can get shortcutInfo by bundleName
 * @tc.desc: 1.can get shortcutInfo
 */
HWTEST_F(BmsBundleKitServiceTest, GetShortcutInfos_0600, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<ShortcutInfo> shortcutInfos;
    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bundleMgrProxy->GetShortcutInfos(
        BUNDLE_NAME_TEST, shortcutInfos);
    EXPECT_TRUE(shortcutInfos.empty());
    CheckShortcutInfoTest(shortcutInfos);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetShortcutInfos_0700
 * @tc.name: test can get shortcutInfo by bundleName
 * @tc.desc: 1.can not get shortcutInfo by empty bundle name
 */
HWTEST_F(BmsBundleKitServiceTest, GetShortcutInfos_0700, Function | SmallTest | Level1)
{
    std::vector<ShortcutInfo> shortcutInfos;
    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bundleMgrProxy->GetShortcutInfos(
        "", shortcutInfos);
    EXPECT_TRUE(shortcutInfos.empty());
}

/**
 * @tc.number: GetShortcutInfos_0800
 * @tc.name: test can get shortcutInfo by bundleName
 * @tc.desc: 1.can get shortcutInfo
 */
HWTEST_F(BmsBundleKitServiceTest, GetShortcutInfos_0800, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<ShortcutInfo> shortcutInfos;
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    auto result = hostImpl->GetShortcutInfos("", shortcutInfos);
    EXPECT_FALSE(result);
    result = hostImpl->GetShortcutInfos(
        BUNDLE_NAME_TEST,  DEFAULT_USERID, shortcutInfos);
    EXPECT_TRUE(result);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: Ability_0100
 * @tc.name: test can get the compatibleAbilityInfo by one bundle
 * @tc.desc: 1.can get compatibleAbilityInfo
 */
HWTEST_F(BmsBundleKitServiceTest, Ability_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfo(want, 0, 0, abilityInfo);
    EXPECT_TRUE(testRet);
    CompatibleAbilityInfo compatibleAbilityInfo;
    abilityInfo.ConvertToCompatiableAbilityInfo(compatibleAbilityInfo);
    CheckCompatibleAbilityInfo(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, compatibleAbilityInfo);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: Ability_0200
 * @tc.name: test can get the compatibleAbilityInfo by two bundle
 * @tc.desc: 1.can get compatibleAbilityInfo
 */
HWTEST_F(BmsBundleKitServiceTest, Ability_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    Want want1;
    want1.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo1;
    bool testRet1 = GetBundleDataMgr()->QueryAbilityInfo(want1, 0, 0, abilityInfo1);
    EXPECT_TRUE(testRet1);
    CompatibleAbilityInfo compatibleAbilityInfo1;
    abilityInfo1.ConvertToCompatiableAbilityInfo(compatibleAbilityInfo1);
    CheckCompatibleAbilityInfo(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, compatibleAbilityInfo1);
    Want want2;
    want2.SetElementName(BUNDLE_NAME_DEMO, ABILITY_NAME_DEMO);
    AbilityInfo abilityInfo2;
    bool testRet2 = GetBundleDataMgr()->QueryAbilityInfo(want2, 0, 0, abilityInfo2);
    EXPECT_TRUE(testRet2);
    CompatibleAbilityInfo compatibleAbilityInfo2;
    abilityInfo2.ConvertToCompatiableAbilityInfo(compatibleAbilityInfo2);
    CheckCompatibleAbilityInfo(BUNDLE_NAME_DEMO, ABILITY_NAME_DEMO, compatibleAbilityInfo2);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: Application_0100
 * @tc.name: test can get the compatibleApplicationInfo by one bundle
 * @tc.desc: 1.can get compatibleApplicationInfo
 */
HWTEST_F(BmsBundleKitServiceTest, Application_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    ApplicationInfo testResult;
    bool testRet = GetBundleDataMgr()->GetApplicationInfo(
        BUNDLE_NAME_TEST, ApplicationFlag::GET_BASIC_APPLICATION_INFO, DEFAULT_USER_ID_TEST, testResult);
    EXPECT_TRUE(testRet);
    CompatibleApplicationInfo appInfo;
    testResult.ConvertToCompatibleApplicationInfo(appInfo);
    CheckCompatibleApplicationInfo(BUNDLE_NAME_TEST, PERMISSION_SIZE_ZERO, appInfo);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: Application_0100
 * @tc.name: test can get the compatibleApplicationInfo by two bundle
 * @tc.desc: 1.can get compatibleApplicationInfo
 */
HWTEST_F(BmsBundleKitServiceTest, Application_0200, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);

    ApplicationInfo testResult1;
    bool testRet1 = GetBundleDataMgr()->GetApplicationInfo(
        BUNDLE_NAME_TEST, ApplicationFlag::GET_BASIC_APPLICATION_INFO, DEFAULT_USER_ID_TEST, testResult1);
    EXPECT_TRUE(testRet1);
    CompatibleApplicationInfo appInfo1;
    testResult1.ConvertToCompatibleApplicationInfo(appInfo1);
    CheckCompatibleApplicationInfo(BUNDLE_NAME_TEST, PERMISSION_SIZE_ZERO, appInfo1);

    ApplicationInfo testResult2;
    bool testRet2 = GetBundleDataMgr()->GetApplicationInfo(
        BUNDLE_NAME_DEMO, ApplicationFlag::GET_BASIC_APPLICATION_INFO, DEFAULT_USER_ID_TEST, testResult2);
    EXPECT_TRUE(testRet2);
    CompatibleApplicationInfo appInfo2;
    testResult2.ConvertToCompatibleApplicationInfo(appInfo2);
    CheckCompatibleApplicationInfo(BUNDLE_NAME_DEMO, PERMISSION_SIZE_ZERO, appInfo2);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: QueryAllAbilityInfos
 * @tc.name: test can get the All AbilityInfo
 * @tc.desc: 1.can get All AbilityInfo
 */
HWTEST_F(BmsBundleKitServiceTest, AllAbility_001, Function | SmallTest | Level1)
{
    std::vector<AbilityInfo> abilityInfos;
    Want want1;
    want1.SetElementName(BUNDLE_NAME_DEMO, ABILITY_NAME_DEMO);
    bool testRet1 = GetBundleDataMgr()->QueryLauncherAbilityInfos(want1, DEFAULT_USER_ID_TEST, abilityInfos);
    EXPECT_FALSE(testRet1);
}

/**
 * @tc.number: GetAllCommonEventInfo_0100
 * @tc.name: test can get CommonEventInfo by event key
 * @tc.desc: 1.can get CommonEventInfo
 */
HWTEST_F(BmsBundleKitServiceTest, GetAllCommonEventInfo_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<CommonEventInfo> commonEventInfos;
    auto result = GetBundleDataMgr()->GetAllCommonEventInfo(COMMON_EVENT_EVENT, commonEventInfos);
    EXPECT_TRUE(result);
    CheckCommonEventInfoTest(commonEventInfos);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAllCommonEventInfo_0200
 * @tc.name: test can't get the commonEventInfo have no bundle
 * @tc.desc: 1.have not get commonEventInfo by event key
 *           2.can't get commonEventInfo
 */
HWTEST_F(BmsBundleKitServiceTest, GetAllCommonEventInfo_0200, Function | SmallTest | Level1)
{
    std::vector<CommonEventInfo> commonEventInfos;
    GetBundleDataMgr()->GetAllCommonEventInfo(COMMON_EVENT_EVENT, commonEventInfos);
    EXPECT_TRUE(commonEventInfos.empty());
}

/**
 * @tc.number: GetAllCommonEventInfo_0300
 * @tc.name: test can't get the commonEventInfo in app by empty event key
 * @tc.desc: 1.have not get commonEventInfo by appName
 */
HWTEST_F(BmsBundleKitServiceTest, GetAllCommonEventInfo_0300, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<CommonEventInfo> commonEventInfos;
    GetBundleDataMgr()->GetAllCommonEventInfo(EMPTY_STRING, commonEventInfos);
    EXPECT_TRUE(commonEventInfos.empty());
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAllCommonEventInfo_0400
 * @tc.name: test can't get the commonEventInfo in app by error event key
 * @tc.desc: 1.have not get commonEventInfo by appName
 */
HWTEST_F(BmsBundleKitServiceTest, GetAllCommonEventInfo_0400, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<CommonEventInfo> commonEventInfos;
    GetBundleDataMgr()->GetAllCommonEventInfo(COMMON_EVENT_EVENT_ERROR_KEY, commonEventInfos);
    EXPECT_TRUE(commonEventInfos.empty());
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAllCommonEventInfo_0500
 * @tc.name: test can't get the commonEventInfo in app by not exists event key
 * @tc.desc: 1.have not get commonEventInfo by appName
 */
HWTEST_F(BmsBundleKitServiceTest, GetAllCommonEventInfo_0500, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<CommonEventInfo> commonEventInfos;
    GetBundleDataMgr()->GetAllCommonEventInfo(COMMON_EVENT_EVENT_NOT_EXISTS_KEY, commonEventInfos);
    EXPECT_TRUE(commonEventInfos.empty());
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAllCommonEventInfo_0600
 * @tc.name: test can get CommonEventInfo by event key
 * @tc.desc: 1.can get CommonEventInfo
 */
HWTEST_F(BmsBundleKitServiceTest, GetAllCommonEventInfo_0600, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<CommonEventInfo> commonEventInfos;
    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bundleMgrProxy->GetAllCommonEventInfo(COMMON_EVENT_EVENT_NOT_EXISTS_KEY, commonEventInfos);
    EXPECT_TRUE(commonEventInfos.empty());
    CheckCommonEventInfoTest(commonEventInfos);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAllCommonEventInfo_0700
 * @tc.name: test can get CommonEventInfo by event key
 * @tc.desc: 1.can not get CommonEventInfo by empty key
 */
HWTEST_F(BmsBundleKitServiceTest, GetAllCommonEventInfo_0700, Function | SmallTest | Level1)
{
    std::vector<CommonEventInfo> commonEventInfos;
    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    bool res = bundleMgrProxy->GetAllCommonEventInfo("", commonEventInfos);
    EXPECT_FALSE(res);
}

/**
 * @tc.number: GetAllCommonEventInfo_0800
 * @tc.name: test can't get the commonEventInfo have no bundle
 * @tc.desc: 1.have not get commonEventInfo by event key
 *           2.can't get commonEventInfo
 */
HWTEST_F(BmsBundleKitServiceTest, GetAllCommonEventInfo_0800, Function | SmallTest | Level1)
{
    std::vector<CommonEventInfo> commonEventInfos;
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    hostImpl->GetAllCommonEventInfo(COMMON_EVENT_EVENT, commonEventInfos);
    EXPECT_TRUE(commonEventInfos.empty());
}

/**
 * @tc.number: skill match rules
 * @tc.name: action match test: want empty; skill empty
 * @tc.desc: expect false
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Action_001, Function | SmallTest | Level1)
{
    struct Skill skill;
    Want want;
    bool ret = skill.Match(want);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: action match test: want not empty; skill empty
 * @tc.desc: expect false
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Action_002, Function | SmallTest | Level1)
{
    struct Skill skill;
    Want want;
    want.SetAction(ACTION_001);
    bool ret = skill.Match(want);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: action match test: want empty; skill not empty
 * @tc.desc: expect true
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Action_003, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    Want want;
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: action match test: want not empty; skill not empty; skill contains want
 * @tc.desc: expect true
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Action_004, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    Want want;
    want.SetAction(ACTION_001);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: action match test: want not empty; skill not empty; skill not contains want
 * @tc.desc: expect false
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Action_005, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    Want want;
    want.SetAction(ACTION_002);
    bool ret = skill.Match(want);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: entities match test: want empty; skill empty;
 * @tc.desc: expect true
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Entity_001, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    Want want;
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: entities match test: want not empty; skill empty;
 * @tc.desc: expect false
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Entity_002, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    Want want;
    want.AddEntity(ENTITY_001);
    bool ret = skill.Match(want);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: entities match test: want empty; skill not empty;
 * @tc.desc: expect true
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Entity_003, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    skill.entities.emplace_back(ENTITY_001);
    Want want;
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: entities match test: want not empty; skill not empty; skill contains want
 * @tc.desc: expect true
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Entity_004, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    skill.entities.emplace_back(ENTITY_001);
    Want want;
    want.AddEntity(ENTITY_001);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: entities match test: want not empty; skill not empty; skill contains want
 * @tc.desc: expect true
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Entity_005, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    skill.entities.emplace_back(ENTITY_001);
    skill.entities.emplace_back(ENTITY_002);
    Want want;
    want.AddEntity(ENTITY_001);
    want.AddEntity(ENTITY_002);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: entities match test: want not empty; skill not empty; skill not contains want
 * @tc.desc: expect false
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_Entity_006, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    skill.entities.emplace_back(ENTITY_001);
    Want want;
    want.AddEntity(ENTITY_002);
    bool ret = skill.Match(want);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: uri and type match test: want empty; skill empty
 * @tc.desc: expect true
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_UriAndType_001, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    Want want;
    want.SetUri("");
    want.SetType("");
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: uri and type match test: want uri empty, type not empty; skill uri empty, type not empty
 * @tc.desc: expect true
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_UriAndType_002, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    SkillUri skillUri;
    skillUri.type = TYPE_001;
    skill.uris.emplace_back(skillUri);
    Want want;
    want.SetType(TYPE_001);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: uri and type match test: want uri empty, type not empty; skill uri empty, type not empty; type not equal
 * @tc.desc: expect false
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_UriAndType_003, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    SkillUri skillUri;
    skillUri.type = TYPE_001;
    skill.uris.emplace_back(skillUri);
    Want want;
    want.SetType(TYPE_002);
    bool ret = skill.Match(want);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: uri and type match test: want uri not empty, type empty; skill uri not empty, type empty
 * @tc.desc: expect true
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_UriAndType_004, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    SkillUri skillUri;
    skillUri.scheme = SCHEME_001;
    skill.uris.emplace_back(skillUri);
    Want want;
    want.SetUri(SCHEME_001 + SCHEME_SEPARATOR);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: uri and type match test: want uri not empty, type empty; skill uri not empty, type empty; uri not equal
 * @tc.desc: expect false
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_UriAndType_005, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    SkillUri skillUri;
    skillUri.scheme = SCHEME_001;
    skill.uris.emplace_back(skillUri);
    Want want;
    want.SetUri(SCHEME_002 + SCHEME_SEPARATOR);
    bool ret = skill.Match(want);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: uri and type match test: want uri empty, type not empty; skill uri empty, type not empty; regex
 * @tc.desc: expect true
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_UriAndType_006, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    SkillUri skillUri;
    skillUri.type = TYPE_IMG_REGEX;
    skill.uris.emplace_back(skillUri);
    Want want;
    want.SetType(TYPE_IMG_JPEG);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: uri and type match test: want uri not empty, type empty; skill uri not empty, type empty
 *           uri path match.
 * @tc.desc: expect true
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_UriAndType_007, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    SkillUri skillUri;
    skillUri.scheme = SCHEME_001;
    skillUri.host = HOST_001;
    skillUri.port = PORT_001;
    skillUri.path = PATH_001;
    skill.uris.emplace_back(skillUri);
    Want want;
    want.SetUri(URI_PATH_001);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: uri and type match test: want uri not empty, type empty; skill uri not empty, type empty
 *           uri pathStartWith match.
 * @tc.desc: expect true
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_UriAndType_008, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    SkillUri skillUri;
    skillUri.scheme = SCHEME_001;
    skillUri.host = HOST_001;
    skillUri.port = PORT_001;
    skillUri.pathStartWith = PATH_001;
    skill.uris.emplace_back(skillUri);
    Want want;
    want.SetUri(URI_PATH_DUPLICATE_001);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: uri and type match test: want uri not empty, type empty; skill uri not empty, type empty
 *           uri pathRegex match.
 * @tc.desc: expect true
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_UriAndType_009, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    SkillUri skillUri;
    skillUri.scheme = SCHEME_001;
    skillUri.host = HOST_001;
    skillUri.port = PORT_001;
    skillUri.pathRegex = PATH_REGEX_001;
    skill.uris.emplace_back(skillUri);
    Want want;
    want.SetUri(URI_PATH_001);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: uri's scheme prefix match test
 * @tc.desc: config only has scheme, param has "scheme://" prefix then match, otherwise not match.
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_UriPrefix_001, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    SkillUri skillUri;
    skillUri.scheme = SCHEME_001;
    skill.uris.emplace_back(skillUri);
    // success testCase
    std::string uri = SCHEME_001 + SCHEME_SEPARATOR;
    Want want;
    want.SetUri(uri);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);

    uri.append(HOST_001);
    want.SetUri(uri);
    ret = skill.Match(want);
    EXPECT_EQ(true, ret);

    uri.append(PORT_SEPARATOR).append(PORT_001);
    want.SetUri(uri);
    ret = skill.Match(want);
    EXPECT_EQ(true, ret);

    uri.append(PATH_SEPARATOR).append(PATH_001);
    want.SetUri(uri);
    ret = skill.Match(want);
    EXPECT_EQ(true, ret);
    // fail testCase
    uri = SCHEME_002 + SCHEME_SEPARATOR;
    want.SetUri(uri);
    ret = skill.Match(want);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: uri's scheme prefix match test
 * @tc.desc: config only has scheme and host, param has "scheme://host" prefix then match, otherwise not match.
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_UriPrefix_002, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    SkillUri skillUri;
    skillUri.scheme = SCHEME_001;
    skillUri.host = HOST_001;
    skill.uris.emplace_back(skillUri);
    // success testCase
    std::string uri = SCHEME_001 + SCHEME_SEPARATOR + HOST_001;
    Want want;
    want.SetUri(uri);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);

    uri.append(PORT_SEPARATOR).append(PORT_001);
    want.SetUri(uri);
    ret = skill.Match(want);
    EXPECT_EQ(true, ret);

    uri.append(PATH_SEPARATOR).append(PATH_001);
    want.SetUri(uri);
    ret = skill.Match(want);
    EXPECT_EQ(true, ret);

    uri = SCHEME_001 + SCHEME_SEPARATOR + HOST_001 + PATH_SEPARATOR + PATH_001;
    want.SetUri(uri);
    ret = skill.Match(want);
    EXPECT_EQ(true, ret);
    // fail testCase
    uri = SCHEME_001 + SCHEME_SEPARATOR + HOST_002;
    want.SetUri(uri);
    ret = skill.Match(want);
    EXPECT_EQ(false, ret);

    uri = SCHEME_002 + SCHEME_SEPARATOR + HOST_001;
    want.SetUri(uri);
    ret = skill.Match(want);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: uri's scheme prefix match test
 * @tc.desc: config only has scheme and host and port,
 * param has "scheme://host:port" prefix then match, otherwise not match.
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_UriPrefix_003, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(ACTION_001);
    SkillUri skillUri;
    skillUri.scheme = SCHEME_001;
    skillUri.host = HOST_001;
    skillUri.port = PORT_001;
    skill.uris.emplace_back(skillUri);
    // success testCase
    std::string uri = SCHEME_001 + SCHEME_SEPARATOR + HOST_001 + PORT_SEPARATOR + PORT_001;
    Want want;
    want.SetUri(uri);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);

    uri.append(PATH_SEPARATOR).append(PATH_001);
    want.SetUri(uri);
    ret = skill.Match(want);
    EXPECT_EQ(true, ret);
    // fail testCase
    uri = SCHEME_001 + SCHEME_SEPARATOR + HOST_001 + PORT_SEPARATOR + PORT_002;
    want.SetUri(uri);
    ret = skill.Match(want);
    EXPECT_EQ(false, ret);

    uri = SCHEME_001 + SCHEME_SEPARATOR + HOST_002 + PORT_SEPARATOR + PORT_001;
    want.SetUri(uri);
    ret = skill.Match(want);
    EXPECT_EQ(false, ret);

    uri = SCHEME_002 + SCHEME_SEPARATOR + HOST_001 + PORT_SEPARATOR + PORT_001;
    want.SetUri(uri);
    ret = skill.Match(want);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number: skill match rules
 * @tc.name: action match test
 * @tc.desc: "action.system.home" is equal to "ohos.want.action.home"
 */
HWTEST_F(BmsBundleKitServiceTest, SkillMatch_HOME_ACTION_001, Function | SmallTest | Level1)
{
    struct Skill skill;
    skill.actions.emplace_back(Constants::ACTION_HOME);
    Want want;
    want.SetAction(Constants::WANT_ACTION_HOME);
    bool ret = skill.Match(want);
    EXPECT_EQ(true, ret);

    skill.actions.clear();
    skill.actions.emplace_back(Constants::WANT_ACTION_HOME);
    want.SetAction(Constants::ACTION_HOME);
    ret = skill.Match(want);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number: GetAlldependentModuleNames
 * @tc.name: no dependencies
 * @tc.desc: expect true
 */
HWTEST_F(BmsBundleKitServiceTest, GetAlldependentModuleNames_001, Function | SmallTest | Level1)
{
    InnerBundleInfo innerBundleInfo;
    std::vector<Dependency> dependencies;
    MockInnerBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, dependencies, innerBundleInfo);
    std::vector<std::string> dependentModuleName;
    auto res = innerBundleInfo.GetAllDependentModuleNames(MODULE_NAME_TEST, dependentModuleName);
    EXPECT_TRUE(res);
    EXPECT_TRUE(dependentModuleName.empty());
}

/**
 * @tc.number: GetAlldependentModuleNames
 * @tc.name: one dependent module
 * @tc.desc: expect true
 */
HWTEST_F(BmsBundleKitServiceTest, GetAlldependentModuleNames_002, Function | SmallTest | Level1)
{
    InnerBundleInfo innerBundleInfo;
    std::vector<Dependency> dependencies;
    Dependency dependency;
    dependency.moduleName = MODULE_NAME_TEST_1;
    dependencies.push_back(dependency);
    MockInnerBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, dependencies, innerBundleInfo);
    dependencies.clear();
    MockInnerBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST_1, ABILITY_NAME_TEST, dependencies, innerBundleInfo);

    std::vector<std::string> dependentModuleName;
    auto res = innerBundleInfo.GetAllDependentModuleNames(MODULE_NAME_TEST, dependentModuleName);
    EXPECT_TRUE(res);
    EXPECT_EQ(dependentModuleName.size(), MODULE_NAMES_SIZE_ONE);
    if (!dependentModuleName.empty()) {
        EXPECT_EQ(dependentModuleName[0], MODULE_NAME_TEST_1);
    }
}

/**
 * @tc.number: GetAlldependentModuleNames
 * @tc.name: more than one dependent module
 * @tc.desc: expect true
 */
HWTEST_F(BmsBundleKitServiceTest, GetAlldependentModuleNames_003, Function | SmallTest | Level1)
{
    InnerBundleInfo innerBundleInfo;
    std::vector<Dependency> dependencies;
    Dependency dependency_1;
    dependency_1.moduleName = MODULE_NAME_TEST_1;
    Dependency dependency_2;
    dependency_2.moduleName = MODULE_NAME_TEST_2;
    dependencies.push_back(dependency_1);
    dependencies.push_back(dependency_2);
    MockInnerBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, dependencies, innerBundleInfo);
    dependencies.clear();
    MockInnerBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST_1, ABILITY_NAME_TEST, dependencies, innerBundleInfo);
    MockInnerBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST_2, ABILITY_NAME_TEST, dependencies, innerBundleInfo);

    std::vector<std::string> dependentModuleName;
    auto res = innerBundleInfo.GetAllDependentModuleNames(MODULE_NAME_TEST, dependentModuleName);
    EXPECT_TRUE(res);
    EXPECT_EQ(dependentModuleName.size(), MODULE_NAMES_SIZE_TWO);
    if (dependentModuleName.size() == MODULE_NAMES_SIZE_TWO) {
        EXPECT_EQ(dependentModuleName[0], MODULE_NAME_TEST_1);
        EXPECT_EQ(dependentModuleName[MODULE_NAMES_SIZE_ONE], MODULE_NAME_TEST_2);
    }
}

/**
 * @tc.number: GetAlldependentModuleNames
 * @tc.name: Multiple dependent modules
 * @tc.desc: expect true
 */
HWTEST_F(BmsBundleKitServiceTest, GetAlldependentModuleNames_004, Function | SmallTest | Level1)
{
    InnerBundleInfo innerBundleInfo;
    std::vector<Dependency> dependencies;
    Dependency dependency_1;
    dependency_1.moduleName = MODULE_NAME_TEST_1;
    Dependency dependency_2;
    dependency_2.moduleName = MODULE_NAME_TEST_2;
    dependencies.push_back(dependency_1);
    dependencies.push_back(dependency_2);
    MockInnerBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST, dependencies, innerBundleInfo);
    dependencies.clear();
    MockInnerBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST_1, ABILITY_NAME_TEST, dependencies, innerBundleInfo);
    dependencies.clear();
    Dependency dependency_3;
    dependency_3.moduleName = MODULE_NAME_TEST_3;
    dependencies.push_back(dependency_3);
    MockInnerBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST_2, ABILITY_NAME_TEST, dependencies, innerBundleInfo);
    dependencies.clear();
    MockInnerBundleInfo(BUNDLE_NAME_TEST, MODULE_NAME_TEST_3, ABILITY_NAME_TEST, dependencies, innerBundleInfo);

    std::vector<std::string> dependentModuleName;
    auto res = innerBundleInfo.GetAllDependentModuleNames(MODULE_NAME_TEST, dependentModuleName);
    EXPECT_TRUE(res);
    EXPECT_EQ(dependentModuleName.size(), MODULE_NAMES_SIZE_THREE);
    if (dependentModuleName.size() == MODULE_NAMES_SIZE_THREE) {
        EXPECT_EQ(dependentModuleName[0], MODULE_NAME_TEST_1);
        EXPECT_EQ(dependentModuleName[MODULE_NAMES_SIZE_ONE], MODULE_NAME_TEST_2);
        EXPECT_EQ(dependentModuleName[MODULE_NAMES_SIZE_TWO], MODULE_NAME_TEST_3);
    }
}

/**
 * @tc.number: SetDisposedStatus_001
 * @tc.name: test SetDisposedStatus
 * @tc.desc: bundleName empty, expect false
 */
HWTEST_F(BmsBundleKitServiceTest, SetDisposedStatus_001, Function | SmallTest | Level1)
{
    bool result = GetBundleDataMgr()->SetDisposedStatus("", DISPOSED_STATUS);
    EXPECT_FALSE(result);
}

/**
 * @tc.number: SetDisposedStatus_002
 * @tc.name: test SetDisposedStatus
 * @tc.desc: wrong bundleName, expect false
 */
HWTEST_F(BmsBundleKitServiceTest, SetDisposedStatus_002, Function | SmallTest | Level1)
{
    bool result = GetBundleDataMgr()->SetDisposedStatus("wrong", DISPOSED_STATUS);
    EXPECT_FALSE(result);
}

/**
 * @tc.number: SetDisposedStatus_003
 * @tc.name: test SetDisposedStatus
 * @tc.desc: right bundleName, expect true
 */
HWTEST_F(BmsBundleKitServiceTest, SetDisposedStatus_003, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    bool result = GetBundleDataMgr()->SetDisposedStatus(BUNDLE_NAME_TEST, DISPOSED_STATUS);
    EXPECT_TRUE(result);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetDisposedStatus_001
 * @tc.name: test GetDisposedStatus
 * @tc.desc: empty bundleName, expect 0
 */
HWTEST_F(BmsBundleKitServiceTest, GetDisposedStatus_001, Function | SmallTest | Level1)
{
    int32_t status = GetBundleDataMgr()->GetDisposedStatus("");
    EXPECT_EQ(status, 0);
}

/**
 * @tc.number: GetDisposedStatus_002
 * @tc.name: test GetDisposedStatus
 * @tc.desc: wrong bundleName, expect 0
 */
HWTEST_F(BmsBundleKitServiceTest, GetDisposedStatus_002, Function | SmallTest | Level1)
{
    int32_t status = GetBundleDataMgr()->GetDisposedStatus("wrong");
    EXPECT_EQ(status, 0);
}

/**
 * @tc.number: GetDisposedStatus_003
 * @tc.name: test GetDisposedStatus
 * @tc.desc: right bundleName, expect true
 */
HWTEST_F(BmsBundleKitServiceTest, GetDisposedStatus_003, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    bool result = GetBundleDataMgr()->SetDisposedStatus(BUNDLE_NAME_TEST, DISPOSED_STATUS);
    EXPECT_TRUE(result);
    int32_t status = GetBundleDataMgr()->GetDisposedStatus(BUNDLE_NAME_TEST);
    EXPECT_EQ(status, DISPOSED_STATUS);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: Marshalling_001
 * @tc.name: BundleInfo Marshalling
 * @tc.desc: 1.Test the marshalling of BundleInfo
 */
HWTEST_F(BmsBundleKitServiceTest, Marshalling_001, Function | SmallTest | Level1)
{
    BundleInfo info;
    info.name = "com.ohos.contactsdataability";
    info.versionName = "1.0";
    info.vendor = "ohos";
    info.releaseType = "Release";
    info.mainEntry = "com.ohos.contactsdataability";
    info.entryModuleName = "entry";
    info.appId = "com.ohos.contactsdataability_BNtg4JBClbl92Rgc3jm"\
        "/RfcAdrHXaM8F0QOiwVEhnV5ebE5jNIYnAx+weFRT3QTyUjRNdhmc2aAzWyi+5t5CoBM=";
    info.cpuAbi = "armeabi";
    info.description = "dataability_description";
    info.applicationInfo.name = "com.ohos.contactsdataability";
    info.applicationInfo.bundleName = "com.ohos.contactsdataability";
    info.applicationInfo.versionName = "1.0";
    info.applicationInfo.iconPath = "$media:icon";
    info.applicationInfo.description = "dataability_description";
    info.applicationInfo.codePath = "/data/app/el1/budle/public/com.ohos.contactsdataability";
    info.applicationInfo.dataBaseDir = "/data/app/el2/database/com.ohos.contactsdataability";
    info.applicationInfo.apiReleaseType = "Release";
    info.applicationInfo.deviceId = "Id001";
    info.applicationInfo.entityType = "unsppecified";
    info.applicationInfo.vendor = "ohos";
    info.applicationInfo.nativeLibraryPath = "libs/arm";
    Parcel parcel;
    bool ret = info.Marshalling(parcel);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.number: Marshalling_002
 * @tc.name: RequestPermission Marshalling
 * @tc.desc: 1.Test the marshalling of RequestPermission
 */
HWTEST_F(BmsBundleKitServiceTest, Marshalling_002, Function | SmallTest | Level1)
{
    RequestPermission requestPermission;
    requestPermission.name = "ohos.global.systemres";
    requestPermission.reason = "1";
    Parcel parcel;
    bool ret = requestPermission.Marshalling(parcel);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.number: Marshalling_003
 * @tc.name: RequestPermissionUsedScene Marshalling
 * @tc.desc: 1.Test the marshalling of RequestPermissionUsedScene
 */
HWTEST_F(BmsBundleKitServiceTest, Marshalling_003, Function | SmallTest | Level1)
{
    RequestPermissionUsedScene usedScene;
    usedScene.abilities = {"ohos.global.systemres.MainAbility"};
    usedScene.when = "1";
    Parcel parcel;
    bool ret = usedScene.Marshalling(parcel);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.number: Marshalling_004
 * @tc.name: Metadata Marshalling
 * @tc.desc: 1.Test the marshalling of Metadata
 */
HWTEST_F(BmsBundleKitServiceTest, Marshalling_004, Function | SmallTest | Level1)
{
    Metadata metadata;
    metadata.name = "ohos.global.systemres";
    metadata.value = "1";
    metadata.resource = "/data/accounts/account_0/applications/ohos.global.systemres";
    Parcel parcel;
    bool ret = metadata.Marshalling(parcel);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.number: Marshalling_005
 * @tc.name: RequestPermissionUsedScene Marshalling
 * @tc.desc: 1.Test the marshalling of RequestPermissionUsedScene
 */
HWTEST_F(BmsBundleKitServiceTest, Marshalling_005, Function | SmallTest | Level1)
{
    CustomizeData customizeData;
    customizeData.name = "ohos.global.systemres";
    customizeData.value = "1";
    customizeData.extra = "/data/accounts/account_0/applications/ohos.global.systemres";
    Parcel parcel;
    bool ret = customizeData.Marshalling(parcel);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.number: Marshalling_006
 * @tc.name: AppRunningControlRule Marshalling
 * @tc.desc: 1.Test the marshalling of AppRunningControlRule
 */
HWTEST_F(BmsBundleKitServiceTest, Marshalling_006, Function | SmallTest | Level1)
{
    AppRunningControlRule param;
    param.appId = "appId";
    param.controlMessage = "Success";
    Parcel parcel;
    bool ret = param.Marshalling(parcel);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.number: Marshalling_007
 * @tc.name: HapModuleInfo Marshalling
 * @tc.desc: 1.Test the marshalling of HapModuleInfo
 */
HWTEST_F(BmsBundleKitServiceTest, Marshalling_007, Function | SmallTest | Level1)
{
    HapModuleInfo info;
    std::vector<std::string> reqCapabilities;
    std::vector<std::string> deviceTypes;
    std::vector<std::string> dependencies;
    std::vector<AbilityInfo> abilityInfos;
    AbilityInfo abilityInfo;
    std::map<std::string, bool> isRemovable;
    isRemovable["hap1"] = true;
    std::vector<ExtensionAbilityInfo> extensionInfos;
    ExtensionAbilityInfo extensionAbilityInfo;
    std::vector<Metadata> metadata;
    Metadata data;
    reqCapabilities.emplace_back("video_support");
    deviceTypes.emplace_back("default");
    dependencies.emplace_back("dependency");
    abilityInfos.emplace_back(abilityInfo);
    extensionInfos.emplace_back(extensionAbilityInfo);
    metadata.emplace_back(data);
    info.reqCapabilities = reqCapabilities;
    info.deviceTypes = reqCapabilities;
    info.abilityInfos = abilityInfos;
    info.isRemovable = isRemovable;
    info.extensionInfos = extensionInfos;
    info.metadata = metadata;
    Parcel parcel;
    bool ret = info.Marshalling(parcel);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.number: Marshalling_008
 * @tc.name: ApplicationInfo Marshalling
 * @tc.desc: 1.Test the marshalling of ApplicationInfo
 */
HWTEST_F(BmsBundleKitServiceTest, Marshalling_008, Function | SmallTest | Level1)
{
    ApplicationInfo info;
    std::vector<std::string> allowCommonEvent;
    std::vector<std::string> permissions;
    std::vector<std::string> moduleSourceDirs;
    std::vector<std::string> targetBundleList;
    std::vector<ModuleInfo> moduleInfos;
    ModuleInfo moduleInfo;
    Metadata data1("paramName", "paramValue", "paramResource");
    std::map<std::string, std::vector<Metadata>> metadata;
    std::vector<Metadata> vecMetaData;
    vecMetaData.emplace_back(data1);
    metadata["name"] = vecMetaData;
    std::map<std::string, std::vector<CustomizeData>> metaData;
    std::vector<CustomizeData> vecCustomizeData;
    CustomizeData customizeData;
    vecCustomizeData.emplace_back(customizeData);
    metaData["moduleName"] = vecCustomizeData;
    allowCommonEvent.emplace_back("allow1");
    permissions.emplace_back("permission1");
    moduleSourceDirs.emplace_back("dir1");
    targetBundleList.emplace_back("target");
    moduleInfos.emplace_back(moduleInfo);
    info.allowCommonEvent = allowCommonEvent;
    info.permissions = permissions;
    info.targetBundleList = targetBundleList;
    info.moduleSourceDirs = moduleSourceDirs;
    info.moduleInfos = moduleInfos;
    info.metadata = metadata;
    info.metaData = metaData;

    Parcel parcel;
    bool ret = info.Marshalling(parcel);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.number: Marshalling_009
 * @tc.name: HqfInfo Marshalling
 * @tc.desc: 1.Test the marshalling of HqfInfo
 */
HWTEST_F(BmsBundleKitServiceTest, Marshalling_009, Function | SmallTest | Level1)
{
    HqfInfo info;
    info.moduleName = "module";
    info.hapSha256 = "sha256";
    info.cpuAbi = "apu";
    info.nativeLibraryPath = "/data";

    Parcel parcel;
    bool ret = info.Marshalling(parcel);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.number: Marshalling_010
 * @tc.name: AppqfInfo Marshalling
 * @tc.desc: 1.Test the marshalling of AppqfInfo
 */
HWTEST_F(BmsBundleKitServiceTest, Marshalling_010, Function | SmallTest | Level1)
{
    AppqfInfo info;
    info.versionName = "1.0";
    info.cpuAbi = "apu";
    info.nativeLibraryPath = "/data";
    std::vector<HqfInfo> hqfInfos;
    HqfInfo hqfInfo;
    hqfInfos.emplace_back(hqfInfo);
    info.hqfInfos = hqfInfos;

    Parcel parcel;
    bool ret = info.Marshalling(parcel);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.number: Unmarshalling_001
 * @tc.name: RequestPermissionUsedScene Unmarshalling
 * @tc.desc: 1.Test the marshalling of RequestPermissionUsedScene
 */
HWTEST_F(BmsBundleKitServiceTest, Unmarshalling_001, Function | SmallTest | Level1)
{
    CompatibleAbilityInfo compatibleAbilityInfo;
    compatibleAbilityInfo.name = "com.ohos.contactsdataability.ability";;
    compatibleAbilityInfo.moduleName = "entry";
    compatibleAbilityInfo.bundleName = "com.ohos.contactsdataability";
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    AbilityInfo abilityInfo;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfo(want, 0, 0, abilityInfo);
    EXPECT_TRUE(testRet);
    abilityInfo.ConvertToCompatiableAbilityInfo(compatibleAbilityInfo);
    CheckCompatibleAbilityInfo(BUNDLE_NAME_TEST, ABILITY_NAME_TEST, compatibleAbilityInfo);
    Parcel parcel;
    bool ret = compatibleAbilityInfo.Unmarshalling(parcel);
    EXPECT_FALSE(ret);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: Unmarshalling_002
 * @tc.name: RequestPermissionUsedScene Unmarshalling
 * @tc.desc: 1.Test the marshalling of RequestPermissionUsedScene
 */
HWTEST_F(BmsBundleKitServiceTest, Unmarshalling_002, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    ApplicationInfo testResult;
    bool testRet = GetBundleDataMgr()->GetApplicationInfo(
        BUNDLE_NAME_TEST, ApplicationFlag::GET_BASIC_APPLICATION_INFO, DEFAULT_USER_ID_TEST, testResult);
    EXPECT_TRUE(testRet);
    CompatibleApplicationInfo appInfo;
    testResult.ConvertToCompatibleApplicationInfo(appInfo);
    Parcel parcel;
    bool ret = appInfo.Unmarshalling(parcel);
    EXPECT_FALSE(ret);
    CheckCompatibleApplicationInfo(BUNDLE_NAME_TEST, PERMISSION_SIZE_ZERO, appInfo);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: Unmarshalling_003
 * @tc.name: AppRunningControlRule Unmarshalling
 * @tc.desc: 1.Test the Unmarshalling of AppRunningControlRule
 */
HWTEST_F(BmsBundleKitServiceTest, Unmarshalling_003, Function | SmallTest | Level1)
{
    AppRunningControlRule param1;
    param1.appId = "appId";
    param1.controlMessage = "Success";
    Parcel parcel;
    AppRunningControlRule param2;
    auto ret1 = param1.Marshalling(parcel);
    EXPECT_EQ(ret1, true);
    auto ret2 = param2.Unmarshalling(parcel);
    EXPECT_NE(ret2, nullptr);
    EXPECT_EQ(param1.appId, ret2->appId);
    EXPECT_EQ(param1.controlMessage, ret2->controlMessage);
}

/**
 * @tc.number: ReadFromParcelOfAppqfInfo_001
 * @tc.name: ReadFromParcel
 * @tc.desc: 1.Test ReadFromParcel of AppqfInfo
 */
HWTEST_F(BmsBundleKitServiceTest, ReadFromParcelOfAppqfInfo_001, Function | SmallTest | Level1)
{
    AppqfInfo param1;
    AppqfInfo param2;
    param1.versionName = "1.0";
    param1.cpuAbi = "apu";
    param1.nativeLibraryPath = "/data";
    std::vector<HqfInfo> hqfInfos;
    HqfInfo info;
    hqfInfos.emplace_back(info);
    param1.hqfInfos = hqfInfos;
    Parcel parcel;
    auto ret1 = param1.Marshalling(parcel);
    EXPECT_TRUE(ret1);
    auto ret2 = param2.ReadFromParcel(parcel);
    EXPECT_TRUE(ret2);
}

/**
 * @tc.number: ReadFromParcelOfHapModuleInfo_001
 * @tc.name: ReadFromParcel
 * @tc.desc: 1.Test ReadFromParcel of HapModuleInfo
 */
HWTEST_F(BmsBundleKitServiceTest, ReadFromParcelOfHapModuleInfo_001, Function | SmallTest | Level1)
{
    HapModuleInfo info;
    std::vector<std::string> reqCapabilities;
    std::vector<std::string> dependencies;
    std::map<std::string, bool> isRemovable;
    isRemovable["hap1"] = true;
    reqCapabilities.emplace_back("video_support");
    dependencies.emplace_back("dependency");
    info.reqCapabilities = reqCapabilities;
    info.dependencies = dependencies;
    info.isRemovable = isRemovable;

    Parcel parcel;
    auto ret1 = info.Marshalling(parcel);
    EXPECT_EQ(ret1, true);
    auto ret2 = info.ReadFromParcel(parcel);
    EXPECT_TRUE(ret2);
}

/**
 * @tc.number: ReadFromParcelOfApplicationInfo_001
 * @tc.name: ReadFromParcel
 * @tc.desc: 1.Test ReadFromParcel of ApplicationInfo
 */
HWTEST_F(BmsBundleKitServiceTest, ReadFromParcelOfApplicationInfo_001, Function | SmallTest | Level1)
{
    ApplicationInfo info;
    Metadata data1("paramName", "paramValue", "paramResource");
    std::map<std::string, std::vector<Metadata>> metadata;
    std::vector<Metadata> vecMetaData;
    vecMetaData.emplace_back(data1);
    metadata["name"] = vecMetaData;
    std::map<std::string, std::vector<CustomizeData>> metaData;
    std::vector<CustomizeData> vecCustomizeData;
    CustomizeData customizeData;
    vecCustomizeData.emplace_back(customizeData);
    metaData["moduleName"] = vecCustomizeData;
    std::vector<std::string> targetBundleList;
    targetBundleList.emplace_back("target");
    info.metadata = metadata;
    info.metaData = metaData;
    info.targetBundleList = targetBundleList;
    Parcel parcel;
    auto ret1 = info.Marshalling(parcel);
    EXPECT_EQ(ret1, true);
    auto ret2 = info.ReadFromParcel(parcel);
    EXPECT_TRUE(ret2);
}

/**
 * @tc.number: ReadMetaDataFromParcel_001
 * @tc.name: ReadMetaDataFromParcel
 * @tc.desc: 1.Test ReadFromParcel of ApplicationInfo
 */
HWTEST_F(BmsBundleKitServiceTest, ReadMetaDataFromParcel_001, Function | SmallTest | Level1)
{
    ApplicationInfo info;
    std::map<std::string, std::vector<CustomizeData>> metaData;
    std::vector<CustomizeData> vecCustomizeData;
    CustomizeData customizeData;
    vecCustomizeData.emplace_back(customizeData);
    metaData["moduleName"] = vecCustomizeData;
    info.metaData = metaData;
    Parcel parcel;
    auto ret1 = info.Marshalling(parcel);
    EXPECT_EQ(ret1, true);
    auto ret2 = info.ReadMetaDataFromParcel(parcel);
    EXPECT_TRUE(ret2);
}

/**
 * @tc.number: CompatibleAbilityInfo_001
 * @tc.name: ReadFromParcel
 * @tc.desc: 1.Test ReadFromParcel of compatibleAbilityInfo success
 */
HWTEST_F(BmsBundleKitServiceTest, CompatibleAbilityInfo_001, Function | SmallTest | Level1)
{
    CompatibleAbilityInfo compatibleAbilityInfo;
    compatibleAbilityInfo.name = "com.ohos.contactsdata.ability";;
    compatibleAbilityInfo.moduleName = "entry";
    compatibleAbilityInfo.bundleName = "com.ohos.contactsdataability";
    Parcel parcel;
    auto ret = compatibleAbilityInfo.ReadFromParcel(parcel);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.number: CompatibleAbilityInfo_002
 * @tc.name: ConvertToAbilityInfo
 * @tc.desc: 1.Test ConvertToAbilityInfo of compatibleAbilityInfo success
 */
HWTEST_F(BmsBundleKitServiceTest, CompatibleAbilityInfo_002, Function | SmallTest | Level1)
{
    CompatibleAbilityInfo compatibleAbilityInfo;
    compatibleAbilityInfo.name = "com.ohos.contactsdata.ability";
    AbilityInfo info;
    compatibleAbilityInfo.ConvertToAbilityInfo(info);
    EXPECT_EQ(compatibleAbilityInfo.name, info.name);
}

/**
 * @tc.number: CompatibleApplicationInfo_001
 * @tc.name: ReadFromParcel
 * @tc.desc: 1.Test ReadFromParcel of CompatibleApplicationInfo success
 */
HWTEST_F(BmsBundleKitServiceTest, CompatibleApplicationInfo_001, Function | SmallTest | Level1)
{
    CompatibleApplicationInfo appInfo;
    appInfo.name = "com.ohos.contactsdata.ability";
    Parcel parcel;
    auto ret = appInfo.ReadFromParcel(parcel);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.number: CompatibleApplicationInfo_002
 * @tc.name: ConvertToApplicationInfo
 * @tc.desc: 1.Test ConvertToApplicationInfo of CompatibleApplicationInfo success
 */
HWTEST_F(BmsBundleKitServiceTest, CompatibleApplicationInfo_002, Function | SmallTest | Level1)
{
    CompatibleApplicationInfo appInfo;
    appInfo.name = "com.ohos.contactsdata.ability";
    ApplicationInfo info;
    appInfo.ConvertToApplicationInfo(info);
    EXPECT_EQ(appInfo.name, info.name);
}

/**
 * @tc.number: GetDisposedStatus_002
 * @tc.name: test GetDisposedStatus
 * @tc.desc: wrong bundleName, expect 0
 */
HWTEST_F(BmsBundleKitServiceTest, GetDisposedStatus_004, Function | SmallTest | Level1)
{
    sptr<BundleMgrProxy> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        APP_LOGE("bundle mgr proxy is nullptr.");
        EXPECT_EQ(bundleMgrProxy, nullptr);
    }
    int32_t status = bundleMgrProxy->GetDisposedStatus("wrong");
    EXPECT_EQ(status, 0);
}

/**
 * @tc.number: SeriviceStatusCallback_001
 * @tc.name: OnBundleStateChanged
 * @tc.desc: 1.Test StatusCallbackProxy
 */
HWTEST_F(BmsBundleKitServiceTest, SeriviceStatusCallback_001, Function | SmallTest | Level1)
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    APP_LOGI("get proxy success.");
    auto proxy = iface_cast<BundleStatusCallbackProxy>(remoteObject);
    int32_t resultCode = 0;
    uint8_t installType = static_cast<uint8_t>(InstallType::INSTALL_CALLBACK);
    std::string resultMsg = Constants::EMPTY_STRING;
    proxy->OnBundleStateChanged(installType, resultCode, resultMsg, BUNDLE_NAME_TEST);
    EXPECT_EQ(resultCode, 0);
    EXPECT_EQ(resultMsg, "");
}

/**
 * @tc.number: SeriviceStatusCallback_002
 * @tc.name: OnBundleAdded
 * @tc.desc: 1.Test StatusCallbackProxy
 */
HWTEST_F(BmsBundleKitServiceTest, SeriviceStatusCallback_002, Function | SmallTest | Level1)
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    APP_LOGI("get proxy success.");
    auto proxy = iface_cast<BundleStatusCallbackProxy>(remoteObject);
    std::string bundleName = "com.example.bundlekit.test";
    int32_t userId = 100;
    proxy->OnBundleAdded(bundleName, userId);
    EXPECT_EQ(bundleName, "com.example.bundlekit.test");
    EXPECT_EQ(userId, 100);
}

/**
 * @tc.number: SeriviceStatusCallback_003
 * @tc.name: OnBundleUpdated
 * @tc.desc: 1.Test StatusCallbackProxy
 */
HWTEST_F(BmsBundleKitServiceTest, SeriviceStatusCallback_003, Function | SmallTest | Level1)
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    APP_LOGI("get proxy success.");
    auto proxy = iface_cast<BundleStatusCallbackProxy>(remoteObject);
    std::string bundleName = "com.example.bundlekit.test";
    int32_t userId = 100;
    proxy->OnBundleUpdated(bundleName, userId);
    EXPECT_EQ(bundleName, "com.example.bundlekit.test");
    EXPECT_EQ(userId, 100);
}

/**
 * @tc.number: SeriviceStatusCallback_004
 * @tc.name: OnBundleRemoved
 * @tc.desc: 1.Test StatusCallbackProxy
 */
HWTEST_F(BmsBundleKitServiceTest, SeriviceStatusCallback_004, Function | SmallTest | Level1)
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    APP_LOGI("get proxy success.");
    auto proxy = iface_cast<BundleStatusCallbackProxy>(remoteObject);
    std::string bundleName = "com.example.bundlekit.test";
    int32_t userId = 100;
    proxy->OnBundleRemoved(bundleName, userId);
    EXPECT_EQ(bundleName, "com.example.bundlekit.test");
    EXPECT_EQ(userId, 100);
}

/**
 * @tc.number: SeriviceStatusCallback_005
 * @tc.name: OnCleanCacheFinished
 * @tc.desc: 1.Test CleanCacheCallbackProxy
 */
HWTEST_F(BmsBundleKitServiceTest, SeriviceStatusCallback_005, Function | SmallTest | Level1)
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    APP_LOGI("get proxy success.");
    auto proxy = iface_cast<CleanCacheCallbackProxy>(remoteObject);
    bool testRet = true;
    proxy->OnCleanCacheFinished(testRet);
    EXPECT_TRUE(testRet);
}

/**
 * @tc.number: SeriviceStatusCallback_006
 * @tc.name: OnBundleStateChanged
 * @tc.desc: 1.Test StatusCallbackProxy
 *           2.bundle state changed fail by wrong installType
 */
HWTEST_F(BmsBundleKitServiceTest, SeriviceStatusCallback_006, Function | SmallTest | Level1)
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    APP_LOGI("get proxy success.");
    auto proxy = iface_cast<BundleStatusCallbackProxy>(remoteObject);
    int32_t resultCode = 0;
    int32_t installType = 0;
    std::string resultMsg = Constants::EMPTY_STRING;
    proxy->OnBundleStateChanged(installType, resultCode, resultMsg, BUNDLE_NAME_TEST);
    EXPECT_EQ(resultCode, 0);
    EXPECT_EQ(installType, 0);
    EXPECT_EQ(resultMsg, Constants::EMPTY_STRING);
}

/**
 * @tc.number: SeriviceStatusCallback_007
 * @tc.name: OnBundleStateChanged
 * @tc.desc: 1.Test StatusCallbackProxy
 *           2.bundle state changed fail by wrong resultCode
 */
HWTEST_F(BmsBundleKitServiceTest, SeriviceStatusCallback_007, Function | SmallTest | Level1)
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    APP_LOGI("get proxy success.");
    auto proxy = iface_cast<BundleStatusCallbackProxy>(remoteObject);
    uint8_t resultCode = static_cast<uint8_t>(InstallType::INSTALL_CALLBACK);
    uint8_t installType = static_cast<uint8_t>(InstallType::INSTALL_CALLBACK);
    std::string resultMsg = Constants::EMPTY_STRING;
    proxy->OnBundleStateChanged(installType, resultCode, resultMsg, BUNDLE_NAME_TEST);
    EXPECT_EQ(resultCode, 0);
    EXPECT_EQ(installType, 0);
    EXPECT_EQ(resultMsg, Constants::EMPTY_STRING);
}

/**
 * @tc.number: SeriviceStatusCallback_008
 * @tc.name: OnBundleStateChanged
 * @tc.desc: 1.Test StatusCallbackProxy
 *           2.bundle state changed fail by wrong resultMsg
 */
HWTEST_F(BmsBundleKitServiceTest, SeriviceStatusCallback_008, Function | SmallTest | Level1)
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    APP_LOGI("get proxy success.");
    auto proxy = iface_cast<BundleStatusCallbackProxy>(remoteObject);
    int32_t resultCode = 0;
    uint8_t installType = static_cast<uint8_t>(InstallType::INSTALL_CALLBACK);
    std::string resultMsg = "";
    proxy->OnBundleStateChanged(installType, resultCode, resultMsg, BUNDLE_NAME_TEST);
    EXPECT_EQ(resultCode, 0);
    EXPECT_EQ(installType, 0);
    EXPECT_EQ(resultMsg, "");
}

/**
 * @tc.number: SeriviceStatusCallback_009
 * @tc.name: OnBundleStateChanged
 * @tc.desc: 1.Test StatusCallbackProxy
 *           2.bundle state changed fail by wrong bundle name
 */
HWTEST_F(BmsBundleKitServiceTest, SeriviceStatusCallback_009, Function | SmallTest | Level1)
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    APP_LOGI("get proxy success.");
    auto proxy = iface_cast<BundleStatusCallbackProxy>(remoteObject);
    int32_t resultCode = 0;
    uint8_t installType = static_cast<uint8_t>(InstallType::INSTALL_CALLBACK);
    std::string resultMsg = Constants::EMPTY_STRING;
    proxy->OnBundleStateChanged(installType, resultCode, resultMsg, "");
    EXPECT_EQ(resultCode, 0);
    EXPECT_EQ(installType, 0);
    EXPECT_EQ(resultMsg, Constants::EMPTY_STRING);
}

/**
 * @tc.number: SetDebugMode_0100
 * @tc.name: test SetDebugMode
 * @tc.desc: SetDebugMode
 */
HWTEST_F(BmsBundleKitServiceTest, SetDebugMode_0100, Function | SmallTest | Level1)
{
    bool isDebug = false;
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    int32_t result = hostImpl->SetDebugMode(isDebug);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.number: SetDebugMode_0200
 * @tc.name: test SetDebugMode
 * @tc.desc: SetDebugMode
 */
HWTEST_F(BmsBundleKitServiceTest, SetDebugMode_0200, Function | SmallTest | Level1)
{
    bool isDebug = true;
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    int32_t result = hostImpl->SetDebugMode(isDebug);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name: DynamicSystemProcess_0100
 * @tc.desc: Test start and stop d-bms process
 * @tc.type: FUNC
 * @tc.require: issueI56WFH
 */
HWTEST_F(BmsBundleKitServiceTest, DynamicSystemProcess_0100, Function | SmallTest | Level1)
{
    int32_t systemAbilityId = 402;
    std::string strExtra = std::to_string(systemAbilityId);
    auto extraArgv = strExtra.c_str();
    int ret = ServiceControlWithExtra(SERVICES_NAME.c_str(), START, &extraArgv, 1);
    EXPECT_EQ(ret, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    ret = ServiceControlWithExtra(SERVICES_NAME.c_str(), STOP, &extraArgv, 1);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number: QueryAbilityInfosV9_0100
 * @tc.name: test QueryAbilityInfosV9
 * @tc.desc: 1.get ability info failed
 * @tc.require: issueI56WFH
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfosV9_0100, Function | SmallTest | Level1)
{
    APP_LOGI("begin of QueryAbilityInfosV9_0100");
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<AbilityInfo> abilityInfos;

    int32_t flags = static_cast<int32_t>(GetAbilityInfoFlag::GET_ABILITY_INFO_ONLY_SYSTEM_APP);
    ErrCode ret = GetBundleDataMgr()->QueryAbilityInfosV9(want, flags, 0, abilityInfos);
    EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_ABILITY_NOT_EXIST);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    APP_LOGI("QueryAbilityInfosV9_0100 finish");
}

/**
 * @tc.number: QueryAbilityInfosV9_0200
 * @tc.name: test QueryAbilityInfosV9
 * @tc.desc: 1.when disable ability, get ability info failed
 * @tc.require: issueI56WFH
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfosV9_0200, Function | SmallTest | Level1)
{
    APP_LOGI("begin of QueryAbilityInfosV9_0200");
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<AbilityInfo> abilityInfos;

    int32_t flags = static_cast<int32_t>(GetAbilityInfoFlag::GET_ABILITY_INFO_DEFAULT);
    ErrCode ret = GetBundleDataMgr()->QueryAbilityInfosV9(want, flags, 0, abilityInfos);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_TRUE(abilityInfos.size() == 1);
    ret = GetBundleDataMgr()->SetAbilityEnabled(abilityInfos[0], false, 0);
    EXPECT_EQ(ret, ERR_OK);
    ret = GetBundleDataMgr()->QueryAbilityInfosV9(want, flags, 0, abilityInfos);
    EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_ABILITY_DISABLED);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    APP_LOGI("QueryAbilityInfosV9_0200 finish");
}

/**
 * @tc.number: QueryAbilityInfosV9_0300
 * @tc.name: test QueryAbilityInfosV9
 * @tc.desc: 1.implicit query cur bundle, get ability info failed
 * @tc.require: issueI56WFH
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfosV9_0300, Function | SmallTest | Level1)
{
    APP_LOGI("begin of QueryAbilityInfosV9_0300");
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetAction("action.not.extist");
    want.SetElementName("", BUNDLE_NAME_TEST, "", "");
    std::vector<AbilityInfo> abilityInfos;

    int32_t flags = static_cast<int32_t>(GetAbilityInfoFlag::GET_ABILITY_INFO_DEFAULT);
    ErrCode ret = GetBundleDataMgr()->QueryAbilityInfosV9(want, flags, 0, abilityInfos);
    EXPECT_NE(ret, ERR_OK);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    APP_LOGI("QueryAbilityInfosV9_0300 finish");
}

/**
 * @tc.number: QueryAbilityInfosV9_0400
 * @tc.name: test QueryAbilityInfosV9
 * @tc.desc: 1.implicit query cur bundle, get ability info failed
 * @tc.require: issueI56WFH
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfosV9_0400, Function | SmallTest | Level1)
{
    APP_LOGI("begin of QueryAbilityInfosV9_0400");
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetAction("action.not.extist");
    std::vector<AbilityInfo> abilityInfos;

    int32_t flags = static_cast<int32_t>(GetAbilityInfoFlag::GET_ABILITY_INFO_DEFAULT);
    ErrCode ret = GetBundleDataMgr()->QueryAbilityInfosV9(want, flags, 0, abilityInfos);
    EXPECT_NE(ret, ERR_OK);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    APP_LOGI("QueryAbilityInfosV9_0400 finish");
}

/**
 * @tc.number: QueryAbilityInfosV9_0500
 * @tc.name: test can get the ability info by want with implicit query
 * @tc.desc: 1.implicit query cur bundle only system, get ability info failed
 * @tc.require: issueI56WFH
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfosV9_0500, Function | SmallTest | Level1)
{
    APP_LOGI("begin of QueryAbilityInfosV9_0500");
    std::vector<std::string> moduleList {MODULE_NAME_TEST, MODULE_NAME_TEST_1, MODULE_NAME_TEST_2};
    MockInstallBundle(BUNDLE_NAME_TEST, moduleList, ABILITY_NAME_TEST);
    Want want;
    want.SetAction("action.system.home");
    want.AddEntity("entity.system.home");
    want.SetElementName("", BUNDLE_NAME_TEST, "", "");
    std::vector<AbilityInfo> abilityInfos;
    int32_t flags = static_cast<int32_t>(GetAbilityInfoFlag::GET_ABILITY_INFO_ONLY_SYSTEM_APP);

    ErrCode ret = GetBundleDataMgr()->QueryAbilityInfosV9(want, flags, 0, abilityInfos);
    EXPECT_NE(ret, ERR_OK);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    APP_LOGI("QueryAbilityInfosV9_0500 finish");
}

/**
 * @tc.number: QueryAbilityInfosV9_0600
 * @tc.name: test can get the ability info by want with implicit query
 * @tc.desc: 1.implicit query cur bundle when disable
 * @tc.require: issueI56WFH
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfosV9_0600, Function | SmallTest | Level1)
{
    APP_LOGI("begin of QueryAbilityInfosV9_0600");
    std::vector<std::string> moduleList {MODULE_NAME_TEST, MODULE_NAME_TEST_1, MODULE_NAME_TEST_2};
    MockInstallBundle(BUNDLE_NAME_TEST, moduleList, ABILITY_NAME_TEST);
    Want want;
    want.SetAction("action.system.home");
    want.AddEntity("entity.system.home");
    want.SetElementName("", BUNDLE_NAME_TEST, "", "");
    std::vector<AbilityInfo> abilityInfos;
    int32_t flags = static_cast<int32_t>(GetAbilityInfoFlag::GET_ABILITY_INFO_DEFAULT);

    ErrCode ret = GetBundleDataMgr()->QueryAbilityInfosV9(want, flags, 0, abilityInfos);
    EXPECT_EQ(ret, ERR_OK);
    for (const auto &abilityInfo : abilityInfos) {
        ret = GetBundleDataMgr()->SetAbilityEnabled(abilityInfo, false, 0);
        EXPECT_EQ(ret, ERR_OK);
    }
    abilityInfos.clear();
    ret = GetBundleDataMgr()->QueryAbilityInfosV9(want, flags, 0, abilityInfos);
    EXPECT_NE(ret, ERR_OK);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    APP_LOGI("QueryAbilityInfosV9_0600 finish");
}

/**
 * @tc.number: QueryAbilityInfosV9_0700
 * @tc.name: test QueryAbilityInfosV9
 * @tc.desc: 1.implicit query cur bundle, get ability info failed
 * @tc.require: issueI56WFH
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfosV9_0700, Function | SmallTest | Level1)
{
    APP_LOGI("begin of QueryAbilityInfosV9_0700");
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetAction("action.not.extist");
    std::vector<AbilityInfo> abilityInfos;

    int32_t flags = static_cast<int32_t>(GetAbilityInfoFlag::GET_ABILITY_INFO_DEFAULT);
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    ErrCode ret = hostImpl->QueryAbilityInfosV9(want, flags, 0, abilityInfos);
    EXPECT_NE(ret, ERR_OK);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    APP_LOGI("QueryAbilityInfosV9_0700 finish");
}

/**
 * @tc.number: QueryExtensionAbilityInfosV9_0100
 * @tc.name: test QueryExtensionAbilityInfosV9
 * @tc.desc: 1.explicit query extension info failed, bundle not exist, appIndex = 0
 * @tc.require: issueI56WFH
 */
HWTEST_F(BmsBundleKitServiceTest, QueryExtensionAbilityInfosV9_0100, Function | SmallTest | Level1)
{
    APP_LOGI("begin of QueryExtensionAbilityInfosV9_0100");
    Want want;
    want.SetElementName("invlaidBundleName", ABILITY_NAME_TEST);
    std::vector<ExtensionAbilityInfo> extensionInfos;
    int32_t flags = static_cast<int32_t>(GetExtensionAbilityInfoFlag::GET_EXTENSION_ABILITY_INFO_DEFAULT);
    ErrCode ret = GetBundleDataMgr()->QueryExtensionAbilityInfosV9(want, flags, 0, extensionInfos);
    EXPECT_NE(ret, ERR_OK);
    APP_LOGI("QueryExtensionAbilityInfosV9_0100 finish");
}

/**
 * @tc.number: QueryExtensionAbilityInfosV9_0200
 * @tc.name: test QueryExtensionAbilityInfosV9
 * @tc.desc: 1.explicit query extension info failed, bundle not exist, appIndex = 1
 * @tc.require: issueI56WFH
 */
HWTEST_F(BmsBundleKitServiceTest, QueryExtensionAbilityInfosV9_0200, Function | SmallTest | Level1)
{
    APP_LOGI("begin of QueryExtensionAbilityInfosV9_0200");
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<ExtensionAbilityInfo> extensionInfos;
    int32_t flags = static_cast<int32_t>(GetExtensionAbilityInfoFlag::GET_EXTENSION_ABILITY_INFO_DEFAULT);
    int32_t appIndex = 1;
    ErrCode ret = GetBundleDataMgr()->QueryExtensionAbilityInfosV9(want, flags, 0, extensionInfos, appIndex);
    EXPECT_NE(ret, ERR_OK);
    APP_LOGI("QueryExtensionAbilityInfosV9_0200 finish");
}

/**
 * @tc.number: QueryExtensionAbilityInfosV9_0300
 * @tc.name: test QueryExtensionAbilityInfosV9
 * @tc.desc: 1.implicit query extension info failed, scope in cur bundle, appIndex = 0, action not exist
 * @tc.require: issueI56WFH
 */
HWTEST_F(BmsBundleKitServiceTest, QueryExtensionAbilityInfosV9_0300, Function | SmallTest | Level1)
{
    APP_LOGI("begin of QueryExtensionAbilityInfosV9_0300");
    Want want;
    want.SetAction("action.not.exist");
    want.SetElementName("", BUNDLE_NAME_TEST, "", "");
    std::vector<ExtensionAbilityInfo> extensionInfos;
    int32_t flags = static_cast<int32_t>(GetExtensionAbilityInfoFlag::GET_EXTENSION_ABILITY_INFO_DEFAULT);
    ErrCode ret = GetBundleDataMgr()->QueryExtensionAbilityInfosV9(want, flags, 0, extensionInfos);
    EXPECT_NE(ret, ERR_OK);
    APP_LOGI("QueryExtensionAbilityInfosV9_0300 finish");
}

/**
 * @tc.number: QueryExtensionAbilityInfosV9_0400
 * @tc.name: test QueryExtensionAbilityInfosV9
 * @tc.desc: 1.implicit query extension info failed, scope in cur bundle, appIndex = 1
 * @tc.require: issueI56WFH
 */
HWTEST_F(BmsBundleKitServiceTest, QueryExtensionAbilityInfosV9_0400, Function | SmallTest | Level1)
{
    APP_LOGI("begin of QueryExtensionAbilityInfosV9_0400");
    Want want;
    want.SetAction("action.not.exist");
    want.SetElementName("", BUNDLE_NAME_TEST, "", "");
    std::vector<ExtensionAbilityInfo> extensionInfos;
    int32_t flags = static_cast<int32_t>(GetExtensionAbilityInfoFlag::GET_EXTENSION_ABILITY_INFO_DEFAULT);
    int32_t appIndex = 1;
    ErrCode ret = GetBundleDataMgr()->QueryExtensionAbilityInfosV9(want, flags, 0, extensionInfos, appIndex);
    EXPECT_NE(ret, ERR_OK);
    APP_LOGI("QueryExtensionAbilityInfosV9_0400 finish");
}

/**
 * @tc.number: QueryExtensionAbilityInfosV9_0500
 * @tc.name: test QueryExtensionAbilityInfosV9
 * @tc.desc: 1.implicit query extension info failed, scope in all bundle, appIndex = 1
 * @tc.require: issueI56WFH
 */
HWTEST_F(BmsBundleKitServiceTest, QueryExtensionAbilityInfosV9_0500, Function | SmallTest | Level1)
{
    APP_LOGI("begin of QueryExtensionAbilityInfosV9_0500");
    Want want;
    want.SetAction("action.not.exist");
    std::vector<ExtensionAbilityInfo> extensionInfos;
    int32_t flags = static_cast<int32_t>(GetExtensionAbilityInfoFlag::GET_EXTENSION_ABILITY_INFO_DEFAULT);
    int32_t appIndex = 1;
    ErrCode ret = GetBundleDataMgr()->QueryExtensionAbilityInfosV9(want, flags, 0, extensionInfos, appIndex);
    EXPECT_NE(ret, ERR_OK);
    APP_LOGI("QueryExtensionAbilityInfosV9_0500 finish");
}

/**
 * @tc.number: QueryExtensionAbilityInfosV9_0600
 * @tc.name: test QueryExtensionAbilityInfosV9
 * @tc.desc: 1.explicit query extension info success, get more than one extension
 * @tc.require: issueI56WFH
 */
HWTEST_F(BmsBundleKitServiceTest, QueryExtensionAbilityInfosV9_0600, Function | SmallTest | Level1)
{
    APP_LOGI("begin of QueryExtensionAbilityInfosV9_0600");
    std::string moduleName = "m1";
    std::string extension = "test-extension";
    MockInstallExtension(BUNDLE_NAME_TEST, moduleName, extension);
    Want want;
    want.SetAction("action.system.home");
    want.AddEntity("entity.system.home");
    want.SetElementName("", BUNDLE_NAME_TEST, "", "");
    std::vector<ExtensionAbilityInfo> extensionInfos;

    int32_t flags = static_cast<int32_t>(GetExtensionAbilityInfoFlag::GET_EXTENSION_ABILITY_INFO_DEFAULT);
    ErrCode ret = GetBundleDataMgr()->QueryExtensionAbilityInfosV9(want, flags, 0, extensionInfos);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(extensionInfos.size(), 2);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    APP_LOGI("QueryExtensionAbilityInfosV9_0600 finish");
}

/**
 * @tc.number: QueryExtensionAbilityInfosV9_0700
 * @tc.name: test QueryExtensionAbilityInfosV9 proxy
 * @tc.desc: 1.system run normally
 *           2.extension not found
 */
HWTEST_F(BmsBundleKitServiceTest, QueryExtensionAbilityInfosV9_0700, Function | SmallTest | Level1)
{
    Want want;
    int32_t flags = 0;
    int32_t userId = 100;
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    std::vector<ExtensionAbilityInfo> extensions;
    ErrCode ret = hostImpl->QueryExtensionAbilityInfosV9(want, flags, userId, extensions);
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.number: QueryExtensionAbilityInfosV9_0800
 * @tc.name: test QueryExtensionAbilityInfosV9
 * @tc.desc: 1.explicit query extension info failed, bundle not exist, appIndex = 1
 * @tc.require: issueI56WFH
 */
HWTEST_F(BmsBundleKitServiceTest, QueryExtensionAbilityInfosV9_0800, Function | SmallTest | Level1)
{
    Want want;
    want.SetElementName(BUNDLE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<ExtensionAbilityInfo> extensionInfos;
    int32_t userId = 100;
    int32_t flags =
        static_cast<int32_t>(GetExtensionAbilityInfoFlag::GET_EXTENSION_ABILITY_INFO_DEFAULT);
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    ErrCode ret = hostImpl->QueryExtensionAbilityInfosV9(want, flags, userId, extensionInfos);
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.number: TestAbleBundle_001
 * @tc.name: TestAbleBundle
 * @tc.desc: 1.Test the TestAbleBundle
 */
HWTEST_F(BmsBundleKitServiceTest, TestAbleBundle_001, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    bool testRet = GetBundleDataMgr()->DisableBundle(BUNDLE_NAME_TEST);
    EXPECT_EQ(testRet, true);
    bool testRet1 = GetBundleDataMgr()->EnableBundle(BUNDLE_NAME_TEST);
    EXPECT_EQ(testRet1, true);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: TestAbleBundle_002
 * @tc.name: TestAbleBundle
 * @tc.desc: 1.Test the TestAbleBundle
 *           2.query failed
 */
HWTEST_F(BmsBundleKitServiceTest, TestAbleBundle_002, Function | SmallTest | Level1)
{
    bool testRet = GetBundleDataMgr()->DisableBundle("");
    EXPECT_EQ(testRet, false);
    bool testRet1 = GetBundleDataMgr()->EnableBundle("");
    EXPECT_EQ(testRet1, false);
}

/**
 * @tc.number: GetProvisionId_001
 * @tc.name: GetProvisionId
 * @tc.desc: 1.Test the GetProvisionId success
 */
HWTEST_F(BmsBundleKitServiceTest, GetProvisionId_001, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::string provisionId;

    bool testRet = GetBundleDataMgr()->GetProvisionId(BUNDLE_NAME_TEST, provisionId);
    EXPECT_EQ(testRet, true);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetProvisionId_002
 * @tc.name: GetProvisionId
 * @tc.desc: 1.Test the GetProvisionId failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetProvisionId_002, Function | SmallTest | Level1)
{
    std::string provisionId;

    bool testRet = GetBundleDataMgr()->GetProvisionId("", provisionId);
    EXPECT_EQ(testRet, false);
}

/**
 * @tc.number: GetAppFeature_001
 * @tc.name: GetAppFeature
 * @tc.desc: 1.Test the GetAppFeature success
 */
HWTEST_F(BmsBundleKitServiceTest, GetAppFeature_001, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::string appFeature;

    bool testRet = GetBundleDataMgr()->GetAppFeature(BUNDLE_NAME_TEST, appFeature);
    EXPECT_EQ(testRet, true);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetAppFeature_002
 * @tc.name: GetAppFeature
 * @tc.desc: 1.Test the GetAppFeature failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetAppFeature_002, Function | SmallTest | Level1)
{
    std::string appFeature;

    bool testRet = GetBundleDataMgr()->GetAppFeature("", appFeature);
    EXPECT_EQ(testRet, false);
}

/**
 * @tc.number: QueryAbilityInfoWithFlagsV9_0100
 * @tc.name: test QueryAbilityInfoWithFlagsV9
 * @tc.desc: 1.explicit query Ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfoWithFlagsV9_0100, Function | SmallTest | Level1)
{
    APP_LOGI("begin of QueryAbilityInfoWithFlagsV9_0100");
    std::optional<AbilityInfo> option;
    AbilityInfo info;
    InnerBundleInfo innerBundleInfo;
    int32_t flags = static_cast<int32_t>(GetAbilityInfoFlag::GET_ABILITY_INFO_ONLY_SYSTEM_APP);
    ErrCode ret = GetBundleDataMgr()->QueryAbilityInfoWithFlagsV9(option, flags, 100, innerBundleInfo, info);
    EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_ABILITY_NOT_EXIST);
    APP_LOGI("QueryAbilityInfoWithFlagsV9_0100 finish");
}

/**
 * @tc.number: QueryAbilityInfoWithFlagsV9_0200
 * @tc.name: test QueryAbilityInfoWithFlagsV9
 * @tc.desc: 1.explicit query Ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfoWithFlagsV9_0200, Function | SmallTest | Level1)
{
    APP_LOGI("begin of QueryAbilityInfoWithFlagsV9_0200");
    std::optional<AbilityInfo> option;
    AbilityInfo info;
    InnerBundleInfo innerBundleInfo;
    int32_t flags = static_cast<int32_t>(GetAbilityInfoFlag::GET_ABILITY_INFO_DEFAULT);
    ErrCode ret = GetBundleDataMgr()->QueryAbilityInfoWithFlagsV9(option, flags, 100, innerBundleInfo, info);
    EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_ABILITY_DISABLED);
    APP_LOGI("QueryAbilityInfoWithFlagsV9_0200 finish");
}

/**
 * @tc.number: ImplicitQueryCurAbilityInfosV9_0100
 * @tc.name: test ImplicitQueryCurAbilityInfosV9
 * @tc.desc: 1.explicit query CurAbility info ok
 */
HWTEST_F(BmsBundleKitServiceTest, ImplicitQueryCurAbilityInfosV9_0100, Function | SmallTest | Level1)
{
    APP_LOGI("begin of ImplicitQueryCurAbilityInfosV9_0100");
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetAction("action.system.home");
    want.SetElementName("", BUNDLE_NAME_TEST, "", "");
    std::vector<AbilityInfo> abilityInfos;
    int32_t flags = static_cast<int32_t>(GetAbilityInfoFlag::GET_ABILITY_INFO_ONLY_SYSTEM_APP);
    int32_t appIndex = -1;
    ErrCode ret = GetBundleDataMgr()->ImplicitQueryCurAbilityInfosV9(want, flags, 0, abilityInfos, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    APP_LOGI("ImplicitQueryCurAbilityInfosV9_0100 finish");
}

/**
 * @tc.number: ImplicitQueryCurAbilityInfosV9_0200
 * @tc.name: test ImplicitQueryCurAbilityInfosV9
 * @tc.desc: 1.explicit query CurAbility info ok
 */
HWTEST_F(BmsBundleKitServiceTest, ImplicitQueryCurAbilityInfosV9_0200, Function | SmallTest | Level1)
{
    APP_LOGI("begin of ImplicitQueryCurAbilityInfosV9_0200");
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetAction("action.system.home");
    want.SetElementName("", BUNDLE_NAME_TEST, "", "");
    std::vector<AbilityInfo> abilityInfos;
    int32_t flags = static_cast<int32_t>(GetAbilityInfoFlag::GET_ABILITY_INFO_WITH_DISABLE);
    int32_t appIndex = -1;
    ErrCode ret = GetBundleDataMgr()->ImplicitQueryCurAbilityInfosV9(want, flags, 0, abilityInfos, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    APP_LOGI("ImplicitQueryCurAbilityInfosV9_0200 finish");
}

/**
 * @tc.number: ImplicitQueryCurAbilityInfosV9_0300
 * @tc.name: test ImplicitQueryCurAbilityInfosV9
 * @tc.desc: 1.explicit query CurAbility info ok
 */
HWTEST_F(BmsBundleKitServiceTest, ImplicitQueryCurAbilityInfosV9_0300, Function | SmallTest | Level1)
{
    APP_LOGI("begin of ImplicitQueryCurAbilityInfosV9_0300");
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetAction("action.system.home");
    want.SetElementName("", BUNDLE_NAME_TEST, "", "");
    std::vector<AbilityInfo> abilityInfos;
    int32_t flags = static_cast<int32_t>(GetAbilityInfoFlag::GET_ABILITY_INFO_WITH_APPLICATION);
    int32_t appIndex = -1;
    ErrCode ret = GetBundleDataMgr()->ImplicitQueryCurAbilityInfosV9(want, flags, 0, abilityInfos, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    APP_LOGI("ImplicitQueryCurAbilityInfosV9_0300 finish");
}

/**
 * @tc.number: ImplicitQueryCurAbilityInfosV9_0400
 * @tc.name: test ImplicitQueryCurAbilityInfosV9
 * @tc.desc: 1.explicit query CurAbility info ok
 */
HWTEST_F(BmsBundleKitServiceTest, ImplicitQueryCurAbilityInfosV9_0400, Function | SmallTest | Level1)
{
    APP_LOGI("begin of ImplicitQueryCurAbilityInfosV9_0400");
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetAction("action.system.home");
    want.SetElementName("", BUNDLE_NAME_TEST, "", "");
    std::vector<AbilityInfo> abilityInfos;
    int32_t flags = static_cast<int32_t>(GetAbilityInfoFlag::GET_ABILITY_INFO_WITH_METADATA);
    int32_t appIndex = -1;
    ErrCode ret = GetBundleDataMgr()->ImplicitQueryCurAbilityInfosV9(want, flags, 0, abilityInfos, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    APP_LOGI("ImplicitQueryCurAbilityInfosV9_0400 finish");
}

/**
 * @tc.number: ImplicitQueryCurAbilityInfosV9_0500
 * @tc.name: test ImplicitQueryCurAbilityInfosV9
 * @tc.desc: 1.explicit query CurAbility info ok
 */
HWTEST_F(BmsBundleKitServiceTest, ImplicitQueryCurAbilityInfosV9_0500, Function | SmallTest | Level1)
{
    APP_LOGI("begin of ImplicitQueryCurAbilityInfosV9_0500");
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    want.SetAction("action.system.home");
    want.SetElementName("", BUNDLE_NAME_TEST, "", "");
    std::vector<AbilityInfo> abilityInfos;
    int32_t flags = static_cast<int32_t>(GetAbilityInfoFlag::GET_ABILITY_INFO_WITH_PERMISSION);
    int32_t appIndex = -1;
    ErrCode ret = GetBundleDataMgr()->ImplicitQueryCurAbilityInfosV9(want, flags, 0, abilityInfos, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    APP_LOGI("ImplicitQueryCurAbilityInfosV9_0500 finish");
}

/**
 * @tc.number: ExplicitQueryExtensionInfoV9_0100
 * @tc.name: test ExplicitQueryExtensionInfoV9
 * @tc.desc: 1.explicit query extension info failed
 */
HWTEST_F(BmsBundleKitServiceTest, ExplicitQueryExtensionInfoV9_0100, Function | SmallTest | Level1)
{
    APP_LOGI("begin of ExplicitQueryExtensionInfoV9_0100");
    std::string moduleName = "m1";
    std::string extension = "test-extension";
    MockInstallExtension(BUNDLE_NAME_TEST, moduleName, extension);
    Want want;
    want.SetAction("action.system.home");
    want.AddEntity("entity.system.home");
    want.SetElementName("", BUNDLE_NAME_TEST, "", "");
    ExtensionAbilityInfo extensionInfo;
    int32_t flags = static_cast<int32_t>(GetExtensionAbilityInfoFlag::GET_EXTENSION_ABILITY_INFO_WITH_APPLICATION);
    int32_t appIndex = -1;
    ErrCode ret = GetBundleDataMgr()->ExplicitQueryExtensionInfoV9(want, flags, 0, extensionInfo, appIndex);
    EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_ABILITY_NOT_EXIST);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    APP_LOGI("ExplicitQueryExtensionInfoV9_0100 finish");
}

/**
 * @tc.number: ImplicitQueryCurExtensionInfosV9_0100
 * @tc.name: test ImplicitQueryCurExtensionInfosV9
 * @tc.desc: 1.explicit query curextension info success
 */
HWTEST_F(BmsBundleKitServiceTest, ImplicitQueryCurExtensionInfosV9_0100, Function | SmallTest | Level1)
{
    APP_LOGI("begin of ImplicitQueryCurExtensionInfosV9_0100");
    std::string moduleName = "m1";
    std::string extension = "test-extension";
    MockInstallExtension(BUNDLE_NAME_TEST, moduleName, extension);
    Want want;
    want.SetAction("action.system.home");
    want.AddEntity("entity.system.home");
    want.SetElementName("", BUNDLE_NAME_TEST, "", "");
    std::vector<ExtensionAbilityInfo> infos;
    int32_t flags = static_cast<int32_t>(GetExtensionAbilityInfoFlag::GET_EXTENSION_ABILITY_INFO_WITH_APPLICATION);
    int32_t appIndex = -1;
    ErrCode ret = GetBundleDataMgr()->ImplicitQueryCurExtensionInfosV9(want, flags, 0, infos, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    APP_LOGI("ImplicitQueryCurExtensionInfosV9_0100 finish");
}

/**
 * @tc.number: ImplicitQueryCurExtensionInfosV9_0200
 * @tc.name: test ImplicitQueryCurExtensionInfosV9
 * @tc.desc: 1.explicit query curextension info success
 */
HWTEST_F(BmsBundleKitServiceTest, ImplicitQueryCurExtensionInfosV9_0200, Function | SmallTest | Level1)
{
    APP_LOGI("begin of ImplicitQueryCurExtensionInfosV9_0200");
    std::string moduleName = "m1";
    std::string extension = "test-extension";
    MockInstallExtension(BUNDLE_NAME_TEST, moduleName, extension);
    Want want;
    want.SetAction("action.system.home");
    want.AddEntity("entity.system.home");
    want.SetElementName("", BUNDLE_NAME_TEST, "", "");
    std::vector<ExtensionAbilityInfo> infos;
    int32_t flags = static_cast<int32_t>(GetExtensionAbilityInfoFlag::GET_EXTENSION_ABILITY_INFO_DEFAULT);
    int32_t appIndex = -1;
    ErrCode ret = GetBundleDataMgr()->ImplicitQueryCurExtensionInfosV9(want, flags, 0, infos, appIndex);
    EXPECT_EQ(ret, ERR_OK);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    APP_LOGI("ImplicitQueryCurExtensionInfosV9_0200 finish");
}

/**
 * @tc.number: GetMediaData_0100
 * @tc.name: test GetMediaData
 * @tc.desc: 1.explicit get mediadata failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetMediaData_0100, Function | SmallTest | Level1)
{
    APP_LOGI("begin of GetMediaData_0100");
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::unique_ptr<uint8_t[]> mediaDataPtr;
    size_t len = 0;
    ErrCode ret = GetBundleDataMgr()->GetMediaData(BUNDLE_NAME_TEST1, MODULE_NAME_TEST, ABILITY_NAME_TEST,
        mediaDataPtr, len, 0);
    EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_BUNDLE_NOT_EXIST);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    APP_LOGI("GetMediaData_0100 finish");
}

/**
 * @tc.number: GetMediaData_0200
 * @tc.name: test GetMediaData
 * @tc.desc: 1.explicit get mediadata failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetMediaData_0200, Function | SmallTest | Level1)
{
    APP_LOGI("begin of GetMediaData_0200");
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::unique_ptr<uint8_t[]> mediaDataPtr;
    size_t len = 0;
    ErrCode ret = GetBundleDataMgr()->GetMediaData(BUNDLE_NAME_TEST, "", ABILITY_NAME_TEST,
        mediaDataPtr, len, 0);
    EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_INTERNAL_ERROR);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    APP_LOGI("GetMediaData_0200 finish");
}

/**
 * @tc.number: GetMediaData_0200
 * @tc.name: test GetMediaData
 * @tc.desc: 1.explicit get mediadata failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetMediaData_0300, Function | SmallTest | Level1)
{
    APP_LOGI("begin of GetMediaData_0300");
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::unique_ptr<uint8_t[]> mediaDataPtr;
    size_t len = 0;
    ErrCode ret = GetBundleDataMgr()->GetMediaData(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST1,
        mediaDataPtr, len, 0);
    EXPECT_EQ(ret, ERR_BUNDLE_MANAGER_ABILITY_NOT_EXIST);
    MockUninstallBundle(BUNDLE_NAME_TEST);
    APP_LOGI("GetMediaData_0300 finish");
}

/**
 * @tc.number: CheckAppInstallControl_0100
 * @tc.name: test CheckAppInstallControl
 * @tc.desc: 1.explicit check app install control success
 */
HWTEST_F(BmsBundleKitServiceTest, CheckAppInstallControl_0100, Function | SmallTest | Level1)
{
    APP_LOGI("begin of CheckAppInstallControl_0100");
    InnerBundleInfo info;
    bool ret = info.CheckAppInstallControl("", 0);
    EXPECT_TRUE(ret);
    APP_LOGI("CheckAppInstallControl_0100 finish");
}

/**
 * @tc.number: Hidump_001
 * @tc.name: Hidump
 * @tc.desc: 1.Returns whether the interface is called successfully
 */
HWTEST_F(BmsBundleKitServiceTest, Hidump_001, Function | SmallTest | Level1)
{
    std::vector<std::string> args;
    std::string result;
    bool testRet = bundleMgrService_->Hidump(args, result);
    EXPECT_EQ(testRet, true);
    args.emplace_back("1");
    testRet = bundleMgrService_->Hidump(args, result);
    EXPECT_EQ(testRet, true);
    args.emplace_back("2");
    testRet = bundleMgrService_->Hidump(args, result);
    EXPECT_EQ(testRet, true);
    args.emplace_back("3");
    testRet = bundleMgrService_->Hidump(args, result);
    EXPECT_EQ(testRet, true);
}

/**
 * @tc.number: CreateNewUser_0100
 * @tc.name: test new user can get shortcutInfo by bundleName
 * @tc.desc: 1.create new user
 *           2.get shortcutInfo success
 */
HWTEST_F(BmsBundleKitServiceTest, CreateNewUser_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<ShortcutInfo> shortcutInfos;

    int32_t userId = 101;
    bundleUserMgrHostImpl_->CreateNewUser(userId);
    auto result = GetBundleDataMgr()->GetShortcutInfos(
        BUNDLE_NAME_TEST,  userId, shortcutInfos);
    EXPECT_TRUE(result);
    CheckShortcutInfoTest(shortcutInfos);
    bundleUserMgrHostImpl_->RemoveUser(userId);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: AgingTest_0001
 * @tc.name: test Aging Start
 * @tc.desc: running is false
 */
HWTEST_F(BmsBundleKitServiceTest, AgingTest_0001, Function | SmallTest | Level0)
{
    BundleAgingMgr bundleAgingMgr;
    bundleAgingMgr.Start(
        OHOS::AppExecFwk::BundleAgingMgr::AgingTriggertype::PREIOD);
    EXPECT_FALSE(bundleAgingMgr.running);
}

/**
 * @tc.number: AgingTest_0002
 * @tc.name: test Aging Start
 * @tc.desc: running is false
 */
HWTEST_F(BmsBundleKitServiceTest, AgingTest_0002, Function | SmallTest | Level0)
{
    BundleAgingMgr bundleAgingMgr;
    bundleAgingMgr.Start(
        OHOS::AppExecFwk::BundleAgingMgr::AgingTriggertype::FREE_INSTALL);
    EXPECT_FALSE(bundleAgingMgr.running);
}

/**
 * @tc.number: AgingTest_0003
 * @tc.name: test Aging Start
 * @tc.desc: running is false
 */
HWTEST_F(BmsBundleKitServiceTest, AgingTest_0003, Function | SmallTest | Level0)
{
    BundleAgingMgr bundleAgingMgr;
    bundleAgingMgr.Start(
        OHOS::AppExecFwk::BundleAgingMgr::AgingTriggertype::UPDATE_REMOVABLE_FLAG);
    EXPECT_FALSE(bundleAgingMgr.running);
}

/**
 * @tc.number: AginTest_0004
 * @tc.name: test InitAgingtTimer
 * @tc.desc: agingTimerInterval is false
 */
HWTEST_F(BmsBundleKitServiceTest, AginTest_0004, Function | SmallTest | Level0)
{
    BundleAgingMgr bundleAgingMgr;
    bundleAgingMgr.InitAgingtTimer();
    bundleAgingMgr.InitAgingRunner();
    EXPECT_EQ(bundleAgingMgr.agingTimerInterval,
        AgingConstants::DEFAULT_AGING_TIMER_INTERVAL);
}

/**
 * @tc.number: AginTest_0005
 * @tc.name: test InitAgingtTimer
 * @tc.desc: agingTimerInterval is false
 */
HWTEST_F(BmsBundleKitServiceTest, AginTest_0005, Function | SmallTest | Level0)
{
    BundleAgingMgr bundleAgingMgr;
    std::shared_ptr<BundleDataMgr> dataMgr;
    bool res = bundleAgingMgr.ReInitAgingRequest(dataMgr);
    bundleAgingMgr.Process(dataMgr);
    EXPECT_EQ(res, false);
}

/**
 * @tc.number: AginTest_0006
 * @tc.name: test InitAgingtTimer
 * @tc.desc: agingTimerInterval is false
 */
HWTEST_F(BmsBundleKitServiceTest, AginTest_0006, Function | SmallTest | Level0)
{
    BundleAgingMgr bundleAgingMgr;
    bundleAgingMgr.Start(
        OHOS::AppExecFwk::BundleAgingMgr::AgingTriggertype::PREIOD);
    EXPECT_EQ(bundleAgingMgr.running, false);
}

/**
 * @tc.number: GetApplicationInfoV9_0100
 * @tc.name: Test GetApplicationInfoV9
 * @tc.desc: 1.Test the GetApplicationInfoV9 by BundleMgrHostImpl
 */
HWTEST_F(BmsBundleKitServiceTest, GetApplicationInfoV9_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    ApplicationInfo result;
    auto flag = ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION;
    ErrCode ret = hostImpl->GetApplicationInfoV9(
        BUNDLE_NAME_DEMO, flag, DEFAULT_USER_ID_TEST, result);
    EXPECT_EQ(ret, ERR_OK);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetApplicationInfosV9_0100
 * @tc.name: Test GetApplicationInfosV9
 * @tc.desc: 1.Test the GetApplicationInfosV9 by BundleMgrHostImpl
 */
HWTEST_F(BmsBundleKitServiceTest, GetApplicationInfosV9_0100, Function | SmallTest | Level1)
{
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    std::vector<ApplicationInfo> appInfos;
    auto flag = ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION;
    ErrCode ret = hostImpl->GetApplicationInfosV9(flag, DEFAULT_USER_ID_TEST, appInfos);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.number: GetBundleInfoV9_0100
 * @tc.name: Test GetBundleInfoV9
 * @tc.desc: 1.Test the GetBundleInfoV9 by BundleMgrHostImpl
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleInfoV9_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    BundleInfo result;
    ErrCode ret = hostImpl->GetBundleInfoV9(BUNDLE_NAME_DEMO, GET_BUNDLE_WITH_ABILITIES |
        GET_BUNDLE_WITH_REQUESTED_PERMISSION, result, DEFAULT_USERID);

    EXPECT_EQ(ret, ERR_OK);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetBundlePackInfo_0100
 * @tc.name: Test GetBundlePackInfo
 * @tc.desc: 1.Test the GetBundlePackInfo by BundleMgrHostImpl
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundlePackInfo_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    BundlePackInfo bundlePackInfo;
    auto ret = hostImpl->GetBundlePackInfo(
        BUNDLE_NAME_DEMO, GET_PACK_INFO_ALL, bundlePackInfo, DEFAULT_USERID);
    EXPECT_EQ(ret, ERR_OK);

    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetBundlePackInfo_0200
 * @tc.name: Test GetBundlePackInfo
 * @tc.desc: 1.Test the GetBundlePackInfo
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundlePackInfo_0200, Function | SmallTest | Level1)
{
    auto dataMgr = GetBundleDataMgr();
    BundlePackInfo bundlePackInfo;
    auto ret = dataMgr->GetBundlePackInfo(
        BUNDLE_NAME_DEMO, GET_PACK_INFO_ALL, bundlePackInfo, Constants::UNSPECIFIED_USERID);
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.number: GetBundleUserInfo_0100
 * @tc.name: Test GetBundleUserInfo
 * @tc.desc: 1.Test the GetBundleUserInfo by BundleMgrHostImpl
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleUserInfo_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    InnerBundleUserInfo innerBundleUserInfo;
    auto ret = hostImpl->GetBundleUserInfo(
        BUNDLE_NAME_DEMO, DEFAULT_USERID, innerBundleUserInfo);
    EXPECT_EQ(ret, true);

    std::vector<InnerBundleUserInfo> innerBundleUserInfos;
    ret = hostImpl->GetBundleUserInfos(
        BUNDLE_NAME_DEMO, innerBundleUserInfos);
    EXPECT_EQ(ret, true);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetBundleGids_0100
 * @tc.name: Test GetBundleGids
 * @tc.desc: 1.Test the GetBundleGids by BundleMgrHostImpl
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleGids_0100, Function | MediumTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_DEMO, MODULE_NAME_DEMO, ABILITY_NAME_DEMO);
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    std::vector<int> gids;
    auto ret = hostImpl->GetBundleGids(BUNDLE_NAME_DEMO, gids);
    EXPECT_EQ(ret, true);
    MockUninstallBundle(BUNDLE_NAME_DEMO);
}

/**
 * @tc.number: GetBundleGidsByUid_0100
 * @tc.name: Test GetBundleGidsByUid
 * @tc.desc: 1.Test the GetBundleGidsByUid by BundleMgrHostImpl
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleGidsByUid_0100, Function | MediumTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    std::vector<int> gids;
    auto ret = hostImpl->GetBundleGidsByUid(BUNDLE_NAME_TEST, TEST_UID, gids);
    EXPECT_EQ(ret, true);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryAllAbilityInfos_0100
 * @tc.name: Test QueryAllAbilityInfos
 * @tc.desc: 1.Test the QueryAllAbilityInfos by BundleMgrHostImpl
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAllAbilityInfos_0100, Function | MediumTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    Want want;
    ElementName name;
    name.SetAbilityName(ABILITY_NAME_TEST);
    name.SetBundleName(BUNDLE_NAME_TEST);
    want.SetElement(name);
    std::vector<AbilityInfo> AbilityInfo;
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    auto ret = hostImpl->QueryAllAbilityInfos(want, DEFAULT_USERID, AbilityInfo);
    EXPECT_EQ(ret, true);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: DumpShortcutInfo_0100
 * @tc.name: Test DumpShortcutInfo
 * @tc.desc: 1.Test the DumpShortcutInfo by BundleMgrHostImpl
 */
HWTEST_F(BmsBundleKitServiceTest, DumpShortcutInfo_0100, Function | MediumTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    int32_t userId = Constants::ALL_USERID;
    std::string result;
    auto ret = hostImpl->DumpShortcutInfo("ohos.test.error", userId, result);
    EXPECT_EQ(ret, false);
    EXPECT_NE(result.empty(), false);
    ret = hostImpl->DumpShortcutInfo(BUNDLE_NAME_TEST, userId, result);
    EXPECT_EQ(ret, true);
    EXPECT_NE(result.empty(), true);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: SetModuleRemovable_0100
 * @tc.name: Test SetModuleRemovable
 * @tc.desc: 1.Test the SetModuleRemovable by BundleMgrHostImpl
 */
HWTEST_F(BmsBundleKitServiceTest, SetModuleRemovable_0100, Function | MediumTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);

    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool isRemovable = true;
    auto ret1 = hostImpl->SetModuleRemovable(
        BUNDLE_NAME_TEST, MODULE_NAME_TEST, isRemovable);
    EXPECT_EQ(ret1, true);

    ErrCode ret2 = hostImpl->IsModuleRemovable(
        BUNDLE_NAME_TEST, MODULE_NAME_TEST, isRemovable);
    EXPECT_EQ(ret2, ERR_OK);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetModuleUpgradeFlag_0100
 * @tc.name: test can get the module upgrade flag
 * @tc.desc: 1.system run normally
 *           2.set module upgrade flag successfully
 *           3.get module upgrade flag successfully
 */
HWTEST_F(BmsBundleKitServiceTest, GetModuleUpgradeFlag_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    ErrCode result = hostImpl->SetModuleUpgradeFlag(BUNDLE_NAME_TEST, MODULE_NAME_TEST, 1);
    EXPECT_TRUE(result == ERR_OK);
    auto res = hostImpl->GetModuleUpgradeFlag(BUNDLE_NAME_TEST, MODULE_NAME_TEST);
    EXPECT_TRUE(res);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetShortcutInfoV9_0100
 * @tc.name: test query archive information
 * @tc.desc: 1.under '/data/test/bms_bundle',there is a hap
 *           2.query archive information without an ability information
 */
HWTEST_F(BmsBundleKitServiceTest, GetShortcutInfoV9_0100, Function | MediumTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::vector<ShortcutInfo> shortcutInfos;
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    ErrCode testRet = hostImpl->GetShortcutInfoV9(BUNDLE_NAME_TEST, shortcutInfos);
    EXPECT_EQ(testRet, ERR_OK);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: BundleStreamInstallerHostImplInit_0100
 * @tc.name: test Init
 * @tc.desc: Init is false
 */
HWTEST_F(BmsBundleKitServiceTest, BundleStreamInstallerHostImplInit_0100, Function | SmallTest | Level0)
{
    uint32_t installerId = 1;
    int32_t installedUid = 100;
    BundleStreamInstallerHostImpl impl(installerId, installedUid);
    InstallParam installParam;
    sptr<IStatusReceiver> statusReceiver;
    bool res = impl.Init(installParam, statusReceiver);
    EXPECT_TRUE(res);
}

/**
 * @tc.number: GetBundleDistributedManager_0001
 * @tc.name: test GetBundleDistributedManager
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test CheckAbilityEnableInstall
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleDistributedManager_0001, Function | SmallTest | Level0)
{
    auto bundleMgr = GetBundleDistributedManager();
    AAFwk::Want want;
    want.SetAction("action.system.home");
    want.AddEntity("entity.system.home");
    want.SetElementName("", "bundlename", "abilityname", "moudlename");
    bool ret = bundleMgr->CheckAbilityEnableInstall(want, 0, 100, nullptr);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.number: GetBundleDistributedManager_0001
 * @tc.name: test GetBundleDistributedManager
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test OnQueryRpcIdFinished
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleDistributedManager_0002, Function | SmallTest | Level0)
{
    auto bundleMgr = GetBundleDistributedManager();
    std::string queryRpcIdResult;
    bundleMgr->OnQueryRpcIdFinished(queryRpcIdResult);
    queryRpcIdResult = "[]";
    bundleMgr->OnQueryRpcIdFinished(queryRpcIdResult);
    queryRpcIdResult = "[0]";
    bundleMgr->OnQueryRpcIdFinished(queryRpcIdResult);
    EXPECT_EQ(queryRpcIdResult, "[0]");
}

/**
 * @tc.number: GetBundleDistributedManager_0001
 * @tc.name: test GetBundleDistributedManager
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test ComparePcIdString
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleDistributedManager_0003, Function | SmallTest | Level0)
{
    auto bundleMgr = GetBundleDistributedManager();
    AAFwk::Want want;
    want.SetAction("action.system.home");
    want.AddEntity("entity.system.home");
    want.SetElementName("", "bundlename", "abilityname", "moudlename");
    RpcIdResult rpcIdResult;
    int32_t res = bundleMgr->ComparePcIdString(want, rpcIdResult);
    EXPECT_EQ(res, ErrorCode::GET_DEVICE_PROFILE_FAILED);
}

/**
 * @tc.number: GetBundleDistributedManager_0001
 * @tc.name: test GetBundleDistributedManager
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test QueryRpcIdByAbilityToServiceCenter
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleDistributedManager_0004, Function | SmallTest | Level0)
{
    auto bundleMgr = GetBundleDistributedManager();
    TargetAbilityInfo targetAbilityInfo;
    bool res = bundleMgr->QueryRpcIdByAbilityToServiceCenter(targetAbilityInfo);
    EXPECT_EQ(res, false);
}

/**
 * @tc.number: GetBundleDistributedManager_0001
 * @tc.name: test GetBundleDistributedManager
 * @tc.require: issueI5MZ8V
 * @tc.desc: 1. system running normally
 *           2. test OutTimeMonitor
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleDistributedManager_0005, Function | SmallTest | Level0)
{
    auto bundleMgr = GetBundleDistributedManager();
    std::string transactId;
    QueryRpcIdParams queryRpcIdParams;
    bundleMgr->SendCallback(0, queryRpcIdParams);
    bundleMgr->OutTimeMonitor(transactId);
    EXPECT_EQ(transactId, "");
}

/**
 * @tc.number: Hidump_0001
 * @tc.name: test Hidump_0001
 * @tc.desc: Hidump is true
 */
HWTEST_F(BmsBundleKitServiceTest, Hidump_0001, Function | SmallTest | Level0)
{
    std::string arg = "-h";
    std::vector<std::string> args;
    args.push_back(arg);
    std::string result = "";
    auto res = bundleMgrService_->Hidump(args, result);
    EXPECT_TRUE(res);
}

/**
 * @tc.number: Hidump_0002
 * @tc.name: test Hidump
 * @tc.desc: Hidump is true
 */
HWTEST_F(BmsBundleKitServiceTest, Hidump_0002, Function | SmallTest | Level0)
{
    std::weak_ptr<BundleDataMgr> dataMgr;
    HidumpHelper dumpHelper(dataMgr);
    HidumpParam hidumpParam;
    hidumpParam.hidumpFlag = HidumpFlag::GET_ABILITY;
    std::string result = "";
    auto res1 = dumpHelper.ProcessDump(hidumpParam, result);
    EXPECT_EQ(res1, ERR_APPEXECFWK_HIDUMP_SERVICE_ERROR);

    std::string arg = "-ability";
    std::vector<std::string> args;
    args.push_back(arg);
    result.clear();
    auto res2 = bundleMgrService_->Hidump(args, result);
    EXPECT_TRUE(res2);
}

/**
 * @tc.number: Hidump_0003
 * @tc.name: test Hidump
 * @tc.desc: Hidump is true
 */
HWTEST_F(BmsBundleKitServiceTest, Hidump_0003, Function | SmallTest | Level0)
{
    std::weak_ptr<BundleDataMgr> dataMgr;
    HidumpHelper dumpHelper(dataMgr);
    HidumpParam hidumpParam;
    hidumpParam.hidumpFlag = HidumpFlag::GET_ABILITY_LIST;
    std::string result = "";
    auto res1 = dumpHelper.ProcessDump(hidumpParam, result);
    EXPECT_EQ(res1, ERR_APPEXECFWK_HIDUMP_SERVICE_ERROR);

    std::string arg = "-ability-list";
    std::vector<std::string> args;
    args.push_back(arg);
    result.clear();
    auto res2 = bundleMgrService_->Hidump(args, result);
    EXPECT_TRUE(res2);
}

/**
 * @tc.number: Hidump_0004
 * @tc.name: test Hidump
 * @tc.desc: Hidump is true
 */
HWTEST_F(BmsBundleKitServiceTest, Hidump_0004, Function | SmallTest | Level0)
{
    std::weak_ptr<BundleDataMgr> dataMgr;
    HidumpHelper dumpHelper(dataMgr);
    HidumpParam hidumpParam;
    hidumpParam.hidumpFlag = HidumpFlag::GET_BUNDLE;
    std::string result = "";
    auto res1 = dumpHelper.ProcessDump(hidumpParam, result);
    EXPECT_EQ(res1, ERR_APPEXECFWK_HIDUMP_SERVICE_ERROR);

    std::string arg = "-bundle";
    std::vector<std::string> args;
    args.push_back(arg);
    result.clear();
    auto res2 = bundleMgrService_->Hidump(args, result);
    EXPECT_TRUE(res2);
}

/**
 * @tc.number: Hidump_0005
 * @tc.name: test Hidump
 * @tc.desc: Hidump is true
 */
HWTEST_F(BmsBundleKitServiceTest, Hidump_0005, Function | SmallTest | Level0)
{
    std::weak_ptr<BundleDataMgr> dataMgr;
    HidumpHelper dumpHelper(dataMgr);
    HidumpParam hidumpParam;
    hidumpParam.hidumpFlag = HidumpFlag::GET_BUNDLE_LIST;
    std::string result = "";
    auto res1 = dumpHelper.ProcessDump(hidumpParam, result);
    EXPECT_EQ(res1, ERR_APPEXECFWK_HIDUMP_SERVICE_ERROR);

    std::string arg = "-bundle-list";
    std::vector<std::string> args;
    args.push_back(arg);
    result.clear();
    auto res2 = bundleMgrService_->Hidump(args, result);
    EXPECT_TRUE(res2);
}

/**
 * @tc.number: Hidump_0006
 * @tc.name: test Hidump
 * @tc.desc: Hidump is true
 */
HWTEST_F(BmsBundleKitServiceTest, Hidump_0006, Function | SmallTest | Level0)
{
    std::weak_ptr<BundleDataMgr> dataMgr;
    HidumpHelper dumpHelper(dataMgr);
    HidumpParam hidumpParam;
    hidumpParam.hidumpFlag = HidumpFlag::GET_DEVICEID;
    std::string result = "";
    auto res1 = dumpHelper.ProcessDump(hidumpParam, result);
    EXPECT_EQ(res1, ERR_APPEXECFWK_HIDUMP_SERVICE_ERROR);

    std::string arg = "-device";
    std::vector<std::string> args;
    args.push_back(arg);
    result.clear();
    auto res2 = bundleMgrService_->Hidump(args, result);
    EXPECT_TRUE(res2);
}

/**
 * @tc.number: Hidump_0007
 * @tc.name: test Hidump
 * @tc.desc: Hidump is true
 */
HWTEST_F(BmsBundleKitServiceTest, Hidump_0007, Function | SmallTest | Level0)
{
    std::weak_ptr<BundleDataMgr> dataMgr;
    HidumpHelper dumpHelper(dataMgr);
    HidumpParam hidumpParam;
    hidumpParam.hidumpFlag = HidumpFlag::GET_ABILITY_BY_NAME;
    std::string result = "";
    auto res2 = dumpHelper.ProcessDump(hidumpParam, result);
    EXPECT_EQ(res2, ERR_APPEXECFWK_HIDUMP_SERVICE_ERROR);

    std::string arg1 = "-ability";
    std::string arg2 = "-ability";
    std::vector<std::string> args;
    args.push_back(arg1);
    args.push_back(arg2);
    result.clear();
    auto res = bundleMgrService_->Hidump(args, result);
    EXPECT_FALSE(res);

    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::string arg3 = ABILITY_NAME_TEST;
    std::vector<std::string> args1;
    args1.push_back(arg1);
    args1.push_back(arg3);
    result.clear();
    auto res1 = bundleMgrService_->Hidump(args1, result);
    EXPECT_TRUE(res1);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: Hidump_0008
 * @tc.name: test Hidump
 * @tc.desc: Hidump is true
 */
HWTEST_F(BmsBundleKitServiceTest, Hidump_0008, Function | SmallTest | Level0)
{
    std::weak_ptr<BundleDataMgr> dataMgr;
    HidumpHelper dumpHelper(dataMgr);
    HidumpParam hidumpParam;
    hidumpParam.hidumpFlag = HidumpFlag::GET_BUNDLE_BY_NAME;
    std::string result = "";
    auto res2 = dumpHelper.ProcessDump(hidumpParam, result);
    EXPECT_EQ(res2, ERR_APPEXECFWK_HIDUMP_SERVICE_ERROR);

    std::string arg1 = "-bundle";
    std::string arg2 = "-bundle";
    std::vector<std::string> args;
    args.push_back(arg1);
    args.push_back(arg2);
    result.clear();
    auto res = bundleMgrService_->Hidump(args, result);
    EXPECT_FALSE(res);
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    std::string arg3 = BUNDLE_NAME_TEST;
    std::vector<std::string> args1;
    args1.push_back(arg1);
    args1.push_back(arg3);
    result.clear();
    auto res1 = bundleMgrService_->Hidump(args1, result);
    EXPECT_TRUE(res1);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: Hidump_0009
 * @tc.name: test Hidump
 * @tc.desc: Hidump is true
 */
HWTEST_F(BmsBundleKitServiceTest, Hidump_0009, Function | SmallTest | Level0)
{
    std::string arg1 = "-b";
    std::vector<std::string> args;
    args.push_back(arg1);
    std::string result = "";
    auto res = bundleMgrService_->Hidump(args, result);
    EXPECT_TRUE(res);
    std::string arg3 = ABILITY_NAME_TEST;
    std::vector<std::string> args1;
    args1.push_back(arg1);
    args1.push_back(arg3);
    result.clear();
    auto res1 = bundleMgrService_->Hidump(args1, result);
    EXPECT_TRUE(res1);
}

/**
 * @tc.number: LoadInstallInfosFromDb_0001
 * @tc.name: test LoadInstallInfosFromDb
 * @tc.desc: LoadInstallInfosFromDb is true
 */
HWTEST_F(BmsBundleKitServiceTest, LoadInstallInfosFromDb_0001, Function | SmallTest | Level0)
{
    std::shared_ptr<EventRunner> runner;
    BMSEventHandler handler(runner);
    bool res = handler.LoadInstallInfosFromDb();
    EXPECT_TRUE(res);
}

/**
 * @tc.number: LoadInstallInfosFromDb_0002
 * @tc.name: test LoadInstallInfosFromDb
 * @tc.desc: LoadInstallInfosFromDb is false
 */
HWTEST_F(BmsBundleKitServiceTest, LoadInstallInfosFromDb_0002, Function | SmallTest | Level0)
{
    std::shared_ptr<EventRunner> runner;
    BMSEventHandler handler(runner);
    InnerBundleInfo info;
    ClearDataMgr();
    handler.LoadInstallInfosFromDb();

    std::map<std::string, std::vector<InnerBundleUserInfo>> userMaps;
    ScanResultCode res = handler.ScanAndAnalyzeUserDatas(userMaps);
    EXPECT_EQ(res, ScanResultCode::SCAN_NO_DATA);

    std::map<std::string, std::vector<InnerBundleInfo>> installInfos;
    std::map<std::string, std::vector<InnerBundleUserInfo>> userInfoMaps;
    bool ret = handler.CombineBundleInfoAndUserInfo(
        installInfos, userInfoMaps);
    EXPECT_EQ(ret, false);

    handler.SaveInstallInfoToCache(info);
    handler.SetAllInstallFlag();
    handler.ProcessRebootBundleUninstall();
    bool bundleLevel = false;
    handler.DeletePreInfoInDb("", "", bundleLevel);

    std::list<std::string> scanPathList;
    handler.InnerProcessRebootBundleInstall(scanPathList, Constants::AppType::SYSTEM_APP);

    ret = handler.LoadAllPreInstallBundleInfos();
    EXPECT_EQ(ret, false);

    ret = handler.FetchInnerBundleInfo("", info);
    EXPECT_EQ(ret, false);

#ifdef USE_PRE_BUNDLE_PROFILE
    handler.UpdateRemovable("", bundleLevel);
    PreBundleConfigInfo preBundleConfigInfo;
    handler.UpdateTrustedPrivilegeCapability(preBundleConfigInfo);
#endif
    SetDataMgr();
}

/**
 * @tc.number: AnalyzeUserData_0001
 * @tc.name: test AnalyzeUserData
 * @tc.desc: AnalyzeUserData is false
 */
HWTEST_F(BmsBundleKitServiceTest, AnalyzeUserData_0001, Function | SmallTest | Level0)
{
    std::shared_ptr<EventRunner> runner;
    BMSEventHandler handler(runner);
    int32_t userId = 0;
    std::string userDataDir = "";
    std::string userDataBundleName = "";
    std::map<std::string, std::vector<InnerBundleUserInfo>> userMaps;
    bool res = handler.AnalyzeUserData(userId, userDataDir, userDataBundleName, userMaps);
    EXPECT_FALSE(res);
}

/**
 * @tc.number: AnalyzeUserData_0002
 * @tc.name: test AnalyzeUserData
 * @tc.desc: AnalyzeUserData is true
 */
HWTEST_F(BmsBundleKitServiceTest, AnalyzeUserData_0002, Function | SmallTest | Level0)
{
    std::shared_ptr<EventRunner> runner;
    BMSEventHandler handler(runner);
    int32_t userId = 0;
    std::string userDataDir = "/data/app/el2/100/base/";
    std::string userDataBundleName = BUNDLE_NAME_TEST;
    std::map<std::string, std::vector<InnerBundleUserInfo>> userMaps;
    bool res = handler.AnalyzeUserData(userId, userDataDir, userDataBundleName, userMaps);
    EXPECT_FALSE(res);
}

/**
 * @tc.number: RemoveSystemAbility
 * @tc.name: test RemoveSystemAbility
 * @tc.desc: RemoveSystemAbility is true
 */
HWTEST_F(BmsBundleKitServiceTest, RemoveSystemAbility_0001, Function | SmallTest | Level0)
{
    SystemAbilityHelper helper;
    int32_t systemAbilityId = 0;
    bool res = helper.RemoveSystemAbility(systemAbilityId);
    EXPECT_TRUE(res);
}

/**
 * @tc.number: AppRunningControlRuleResult_001
 * @tc.name: Marshalling and Unmarshalling
 * @tc.desc: 1.Test Marshalling and Unmarshalling successed
 */
HWTEST_F(BmsBundleKitServiceTest, AppRunningControlRuleResult_001, Function | SmallTest | Level1)
{
    AppRunningControlRuleResult result;
    result.controlMessage = CONTROLMESSAGE;
    result.controlWant = std::make_shared<AAFwk::Want>();
    Parcel parcel;
    auto ret1 = result.Marshalling(parcel);
    EXPECT_EQ(ret1, true);
    auto ret2 = result.Unmarshalling(parcel);
    EXPECT_NE(ret2, nullptr);
}


/**
 * @tc.number: CreateStream_0100
 * @tc.name: test CreateStream
 * @tc.desc: CreateStream is success
 */
HWTEST_F(BmsBundleKitServiceTest, CreateStream_0100, Function | SmallTest | Level0)
{
    uint32_t installerId = 1;
    int32_t installedUid = 0;
    BundleStreamInstallerHostImpl impl(installerId, installedUid);
    std::string hapName = "test.hap";
    auto res = impl.CreateStream(hapName);
    EXPECT_GE(res, 0);
}

/**
 * @tc.number: CreateStream_0200
 * @tc.name: test CreateStream
 * @tc.desc: CreateStream is false
 */
HWTEST_F(BmsBundleKitServiceTest, CreateStream_0200, Function | SmallTest | Level0)
{
    uint32_t installerId = 1;
    int32_t installedUid = 0;
    BundleStreamInstallerHostImpl impl(installerId, installedUid);
    std::string hapName = "123";
    auto res = impl.CreateStream(hapName);
    EXPECT_EQ(res, -1);
}

/**
 * @tc.number: CreateStream_0300
 * @tc.name: test CreateStream
 * @tc.desc: CreateStream is false
 */
HWTEST_F(BmsBundleKitServiceTest, CreateStream_0300, Function | SmallTest | Level0)
{
    uint32_t installerId = 1;
    int32_t installedUid = 100;
    BundleStreamInstallerHostImpl impl(installerId, installedUid);
    std::string hapName = "test.hap";
    auto res = impl.CreateStream(hapName);
    EXPECT_EQ(res, -1);
}

/**
 * @tc.number: Install_0100
 * @tc.name: test Install
 * @tc.desc: Install is false
 */
HWTEST_F(BmsBundleKitServiceTest, Install_0100, Function | SmallTest | Level0)
{
    uint32_t installerId = 1;
    int32_t installedUid = 100;
    BundleStreamInstallerHostImpl impl(installerId, installedUid);
    bool res = impl.Install();
    EXPECT_FALSE(res);
}

/**
 * @tc.number: GetAllDeviceList_0100
 * @tc.name: test GetAllDeviceList
 * @tc.desc: GetAllDeviceList is success
 */
HWTEST_F(BmsBundleKitServiceTest, GetAllDeviceList_0100, Function | SmallTest | Level0)
{
    BmsDeviceManager deviceManager;
    std::string deviceId = "100";
    std::vector<std::string> deviceIds;
    deviceIds.push_back(deviceId);
    bool res = deviceManager.GetAllDeviceList(deviceIds);
    EXPECT_TRUE(res);
}

/**
 * @tc.number: GetUdidByNetworkId_0100
 * @tc.name: test GetUdidByNetworkId
 * @tc.desc: GetUdidByNetworkId is false
 */
HWTEST_F(BmsBundleKitServiceTest, GetUdidByNetworkId_0100, Function | SmallTest | Level0)
{
    BmsDeviceManager deviceManager;
    std::string netWorkId = "100";
    std::string uid = "100";
    bool res = deviceManager.GetUdidByNetworkId(netWorkId, uid);
    EXPECT_FALSE(res);
}

/**
 * @tc.number: GetBundleDistributedManager_0100
 * @tc.name: test GetBundleDistributedManager
 * @tc.desc: 1.test ConvertTargetAbilityInfo
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleDistributedManager_0100, Function | SmallTest | Level1)
{
    Want want;
    TargetAbilityInfo targetAbilityInfo;
    bool ret = GetBundleDistributedManager()->ConvertTargetAbilityInfo(
        want, targetAbilityInfo);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.number: GetBundleDistributedManager_0200
 * @tc.name: test GetBundleDistributedManager
 * @tc.desc: 1.test ConvertTargetAbilityInfo
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleDistributedManager_0200, Function | SmallTest | Level1)
{
    TargetAbilityInfo targetAbilityInfo;
    bool ret = GetBundleDistributedManager()->QueryRpcIdByAbilityToServiceCenter(
        targetAbilityInfo);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.number: GetBundleDistributedManager_0300
 * @tc.name: test GetBundleDistributedManager
 * @tc.desc: 1.test CheckAbilityEnableInstall
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleDistributedManager_0300, Function | SmallTest | Level1)
{
    Want want;
    int32_t missionId = 0;
    GetBundleDistributedManager()->handler_ = nullptr;
    GetBundleDistributedManager()->OutTimeMonitor("");
    GetBundleDistributedManager()->OnQueryRpcIdFinished("");
    bool ret = GetBundleDistributedManager()->CheckAbilityEnableInstall(want, missionId, DEFAULT_USERID, nullptr);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.number: GetBundleDistributedManager_0400
 * @tc.name: test GetBundleDistributedManager
 * @tc.desc: 1.test CheckAbilityEnableInstall
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleDistributedManager_0400, Function | SmallTest | Level1)
{
    std::vector<std::string> moduleList {MODULE_NAME_TEST, MODULE_NAME_TEST_1, MODULE_NAME_TEST_2};
    MockInstallBundle(BUNDLE_NAME_TEST, moduleList, ABILITY_NAME_TEST);
    Want want;
    want.SetElementName("", BUNDLE_NAME_TEST, "", "");
    int32_t missionId = 0;
    bool ret = GetBundleDistributedManager()->CheckAbilityEnableInstall(want, missionId, DEFAULT_USERID, nullptr);
    EXPECT_EQ(ret, false);
    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: GetBundleDistributedManager_0500
 * @tc.name: test GetBundleDistributedManager
 * @tc.desc: 1.test CheckAbilityEnableInstall
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleDistributedManager_0500, Function | SmallTest | Level1)
{
    int32_t resultCode = 0;
    QueryRpcIdParams queryRpcIdParams;
    GetBundleDistributedManager()->SendCallback(resultCode, queryRpcIdParams);
    EXPECT_EQ(queryRpcIdParams.callback, nullptr);
}

/**
 * @tc.number: GetBundleDistributedManager_0600
 * @tc.name: test GetBundleDistributedManager
 * @tc.desc: 1.test CheckAbilityEnableInstall
 */
HWTEST_F(BmsBundleKitServiceTest, GetBundleDistributedManager_0600, Function | SmallTest | Level1)

{
    Want want;
    RpcIdResult rpcIdResult;
    int32_t res = GetBundleDistributedManager()->ComparePcIdString(want, rpcIdResult);
    EXPECT_EQ(res, ErrorCode::GET_DEVICE_PROFILE_FAILED);
}

/* @tc.number: ShortcutInfoBranchCover_001
 * @tc.name: Marshalling branch cover
 * @tc.desc: 1.Test Marshalling
 */
HWTEST_F(BmsBundleKitServiceTest, ShortcutInfoBranchCover_001, Function | SmallTest | Level1)
{
    ShortcutInfo shortcutInfo = MockShortcutInfo(BUNDLE_NAME_DEMO, SHORTCUT_TEST_ID);
    Parcel parcel;
    auto ret1 = shortcutInfo.Marshalling(parcel);
    EXPECT_EQ(ret1, true);
}

/**
 * @tc.number: ShortcutInfoBranchCover_002
 * @tc.name: shortcutIntent to_json and from_json branch cover
 * @tc.desc: 1.Test shortcutIntent to_json and from_json
 */
HWTEST_F(BmsBundleKitServiceTest, ShortcutInfoBranchCover_002, Function | SmallTest | Level1)
{
    ShortcutIntent shortcutIntent = MockShortcutIntent();
    nlohmann::json jsonObj;
    to_json(jsonObj, shortcutIntent);
    ShortcutIntent result;
    from_json(jsonObj, result);
    EXPECT_EQ(result.targetBundle, SHORTCUT_INTENTS_TARGET_BUNDLE);
    EXPECT_EQ(result.targetModule, SHORTCUT_INTENTS_TARGET_MODULE);
    EXPECT_EQ(result.targetClass, SHORTCUT_INTENTS_TARGET_CLASS);
}

/**
 * @tc.number: ShortcutInfoBranchCover_003
 * @tc.name: shortcutInfo to_json and from_json branch cover
 * @tc.desc: 1.Test shortcutInfo to_json and from_json
 */
HWTEST_F(BmsBundleKitServiceTest, ShortcutInfoBranchCover_003, Function | SmallTest | Level1)
{
    ShortcutInfo shortcutInfo = MockShortcutInfo(BUNDLE_NAME_DEMO, SHORTCUT_TEST_ID);
    nlohmann::json jsonObj;
    to_json(jsonObj, shortcutInfo);
    ShortcutInfo result;
    from_json(jsonObj, result);
    EXPECT_EQ(result.id, SHORTCUT_TEST_ID);
    EXPECT_EQ(result.bundleName, BUNDLE_NAME_DEMO);
    EXPECT_EQ(result.hostAbility, SHORTCUT_HOST_ABILITY);
    EXPECT_EQ(result.icon, SHORTCUT_ICON);
    EXPECT_EQ(result.label, SHORTCUT_LABEL);
    EXPECT_EQ(result.disableMessage, SHORTCUT_DISABLE_MESSAGE);
    EXPECT_EQ(result.isStatic, true);
    EXPECT_EQ(result.isHomeShortcut, true);
    EXPECT_EQ(result.isEnables, true);
}

/**
 * @tc.number: ShortcutInfoBranchCover_004
 * @tc.name: shortcutInfo to_json and from_json branch cover
 * @tc.desc: 1.Test shortcutInfo to_json and from_json
 */
HWTEST_F(BmsBundleKitServiceTest, ShortcutInfoBranchCover_004, Function | SmallTest | Level1)
{
    nlohmann::json jsonObject;
    jsonObject["id"] = false;
    jsonObject["label"] = "1";
    jsonObject["icon"] = "1";
    ShortcutInfo shortcutInfo;
    from_json(jsonObject, shortcutInfo);
    EXPECT_NE(shortcutInfo.icon, "1");
}

/**
 * @tc.number: ShortcutInfoBranchCover_005
 * @tc.name: shortcutWant to_json and from_json branch cover
 * @tc.desc: 1.Test shortcutWant to_json and from_json
 */
HWTEST_F(BmsBundleKitServiceTest, ShortcutInfoBranchCover_005, Function | SmallTest | Level1)
{
    ShortcutWant shortcutWant = MockShortcutWant();
    nlohmann::json jsonObj;
    ShortcutWantToJson(jsonObj, shortcutWant);
    ShortcutWant result;
    from_json(jsonObj, result);
    EXPECT_EQ(result.bundleName, BUNDLE_NAME_DEMO);
    EXPECT_EQ(result.moduleName, MODULE_NAME_DEMO);
    EXPECT_EQ(result.abilityName, ABILITY_NAME_DEMO);
}

/**
 * @tc.number: ShortcutInfoBranchCover_006
 * @tc.name: shortcutWant to_json and from_json branch cover
 * @tc.desc: 1.Test shortcutWant to_json and from_json
 */
HWTEST_F(BmsBundleKitServiceTest, ShortcutInfoBranchCover_006, Function | SmallTest | Level1)
{
    nlohmann::json jsonObject;
    jsonObject["bundleName"] = false;
    jsonObject["moduleName"] = "1";
    jsonObject["abilityName"] = "1";
    ShortcutWant shortcutWant;
    from_json(jsonObject, shortcutWant);
    EXPECT_NE(shortcutWant.moduleName, "1");
}

/**
 * @tc.number: ShortcutInfoBranchCover_007
 * @tc.name: shortcut from_json branch cover
 * @tc.desc: 1.Test shortcut from_json
 */
HWTEST_F(BmsBundleKitServiceTest, ShortcutInfoBranchCover_007, Function | SmallTest | Level1)
{
    nlohmann::json jsonObject;
    jsonObject["shortcutId"] = "1";
    jsonObject["label"] = "1";
    jsonObject["icon"] = "1";
    Shortcut shortcut;
    from_json(jsonObject, shortcut);
    EXPECT_EQ(shortcut.icon, "1");
}

/**
 * @tc.number: ShortcutInfoBranchCover_008
 * @tc.name: shortcut from_json branch cover
 * @tc.desc: 1.Test shortcut from_json
 */
HWTEST_F(BmsBundleKitServiceTest, ShortcutInfoBranchCover_008, Function | SmallTest | Level1)
{
    nlohmann::json jsonObject;
    jsonObject["shortcutId"] = false;
    jsonObject["label"] = "1";
    jsonObject["icon"] = "1";
    Shortcut shortcut;
    from_json(jsonObject, shortcut);
    EXPECT_NE(shortcut.icon, "1");
}

/**
 * @tc.number: ShortcutInfoBranchCover_009
 * @tc.name: shortcutJson from_json branch cover
 * @tc.desc: 1.Test shortcutJson from_json
 */
HWTEST_F(BmsBundleKitServiceTest, ShortcutInfoBranchCover_009, Function | SmallTest | Level1)
{
    nlohmann::json jsonObject = R"({"shortcuts":[{"shortcutId":"1","label":"1","icon":"1"}]})"_json;
    ShortcutJson shortcutJson;
    from_json(jsonObject, shortcutJson);
    Shortcut shortcut = shortcutJson.shortcuts.front();
    EXPECT_EQ(shortcut.shortcutId, "1");
    EXPECT_EQ(shortcut.label, "1");
    EXPECT_EQ(shortcut.icon, "1");
}

/**
 * @tc.number: ShortcutInfoBranchCover_0010
 * @tc.name: shortcutJson from_json branch cover
 * @tc.desc: 1.Test shortcutJson from_json
 */
HWTEST_F(BmsBundleKitServiceTest, ShortcutInfoBranchCover_0010, Function | SmallTest | Level1)
{
    nlohmann::json jsonObject;
    jsonObject["shortcuts"] = "1";
    ShortcutJson shortcutJson;
    from_json(jsonObject, shortcutJson);
    EXPECT_EQ(shortcutJson.shortcuts.size(), 0);
}

/**
 * @tc.number: DBMSBranchCover_0001
 * @tc.name: dbms Marshalling branch cover
 * @tc.desc: 1.Test dbms Marshalling and ReadFromParcel branch cover
 */
HWTEST_F(BmsBundleKitServiceTest, DBMSBranchCover_0001, Function | SmallTest | Level1)
{
    DistributedModuleInfo distributedModuleInfo1;
    DistributedModuleInfo distributedModuleInfo2;
    distributedModuleInfo1.moduleName = "testModuleName";
    DistributedAbilityInfo distributedAbioityInfo;
    distributedModuleInfo1.abilities.emplace_back(distributedAbioityInfo);
    Parcel parcel;
    auto ret1 = distributedModuleInfo1.Marshalling(parcel);
    EXPECT_TRUE(ret1);
    auto ret2 = distributedModuleInfo2.ReadFromParcel(parcel);
    EXPECT_TRUE(ret2);
}

/**
 * @tc.number: DBMSBranchCover_0002
 * @tc.name: dbms Unmarshalling branch cover
 * @tc.desc: 1.Test dbms Unmarshalling branch cover
 */
HWTEST_F(BmsBundleKitServiceTest, DBMSBranchCover_0002, Function | SmallTest | Level1)
{
    DistributedModuleInfo distributedModuleInfo1;
    distributedModuleInfo1.moduleName = "testModuleName";
    DistributedAbilityInfo distributedAbioityInfo;
    distributedModuleInfo1.abilities.emplace_back(distributedAbioityInfo);
    Parcel parcel;
    auto ret1 = distributedModuleInfo1.Marshalling(parcel);
    EXPECT_TRUE(ret1);
    DistributedModuleInfo distributedModuleInfo2;
    auto ret2 = distributedModuleInfo2.Unmarshalling(parcel);
    EXPECT_NE(ret2, nullptr);
    EXPECT_EQ(distributedModuleInfo1.moduleName, ret2->moduleName);
}

/**
 * @tc.number: DBMSBranchCover_0003
 * @tc.name: dbms Unmarshalling branch cover
 * @tc.desc: 1.Test dbms Unmarshalling branch cover
 */
HWTEST_F(BmsBundleKitServiceTest, DBMSBranchCover_0003, Function | SmallTest | Level1)
{
    DistributedModuleInfo distributedModuleInfo;
    std::string path = "/data/test/distributedModuleinfo.txt";
    std::ofstream file(path);
    file.close();
    int fd = -1;
    std::string prefix = "[ability]";
    distributedModuleInfo.Dump(prefix, fd);
    long length = lseek(fd, ZERO, SEEK_END);
    EXPECT_EQ(length, -1);
}

/**
 * @tc.number: DBMSAbilityInfoBranchCover_0001
 * @tc.name: DBMSAbilityInfo ReadFromParcel and marshalling test
 * @tc.desc: 1.DBMSAbilityInfo ReadFromParcel and marshalling test
 */
HWTEST_F(BmsBundleKitServiceTest, DBMSAbilityInfoBranchCover_0001, Function | SmallTest | Level1)
{
    DistributedAbilityInfo distributedAbilityInfo1;
    DistributedAbilityInfo distributedAbilityInfo2;
    distributedAbilityInfo1.abilityName = "testAbilityName";
    Parcel parcel;
    auto ret1 = distributedAbilityInfo1.Marshalling(parcel);
    EXPECT_TRUE(ret1);
    auto ret2 = distributedAbilityInfo2.ReadFromParcel(parcel);
    EXPECT_TRUE(ret2);
}

/**
 * @tc.number: DBMSAbilityInfoBranchCover_0002
 * @tc.name: DBMSAbilityInfo Unmarshalling test
 * @tc.desc: 1.DBMSAbilityInfo Unmarshalling test
 */
HWTEST_F(BmsBundleKitServiceTest, DBMSAbilityInfoBranchCover_0002, Function | SmallTest | Level1)
{
    DistributedAbilityInfo distributedAbilityInfo1;
    DistributedAbilityInfo distributedAbilityInfo2;
    distributedAbilityInfo1.abilityName = "testAbilityName";
    Parcel parcel;
    auto ret1 = distributedAbilityInfo1.Marshalling(parcel);
    EXPECT_TRUE(ret1);
    auto ret2 = distributedAbilityInfo2.Unmarshalling(parcel);
    EXPECT_NE(ret2, nullptr);
    EXPECT_EQ(distributedAbilityInfo1.abilityName, ret2->abilityName);
}

/**
 * @tc.number: DBMSAbilityInfoBranchCover_0003
 * @tc.name: DBMSAbilityInfo dump test
 * @tc.desc: 1.DBMSAbilityInfo dump test
 */
HWTEST_F(BmsBundleKitServiceTest, DBMSAbilityInfoBranchCover_0003, Function | SmallTest | Level1)
{
    DistributedAbilityInfo distributedAbilityInfo;
    std::string path = "/data/test/distributedAbilityInfo.txt";
    std::ofstream file(path);
    file.close();
    int fd = -1;
    std::string prefix = "[ability]";
    distributedAbilityInfo.Dump(prefix, fd);
    long length = lseek(fd, ZERO, SEEK_END);
    EXPECT_EQ(length, -1);
}

/**
 * @tc.number: PermissionDefBranchCover_0001
 * @tc.name: PermissionDef Marshalling test
 * @tc.desc: 1.PermissionDef Marshalling test
 */
HWTEST_F(BmsBundleKitServiceTest, PermissionDefBranchCover_0001, Function | SmallTest | Level1)
{
    PermissionDef param1;
    param1.permissionName = "testPermissioinName";
    param1.bundleName = "testBundleName";
    param1.label = "testLabel";
    param1.description = "testDescription";
    Parcel parcel;
    auto ret1 = param1.Marshalling(parcel);
    EXPECT_TRUE(ret1);
    PermissionDef param2;
    auto ret2 = param2.Unmarshalling(parcel);
    EXPECT_NE(ret2, nullptr);
    EXPECT_EQ(param1.permissionName, ret2->permissionName);
    EXPECT_EQ(param1.bundleName, ret2->bundleName);
    EXPECT_EQ(param1.grantMode, ret2->grantMode);
    EXPECT_EQ(param1.availableLevel, ret2->availableLevel);
    EXPECT_EQ(param1.provisionEnable, ret2->provisionEnable);
    EXPECT_EQ(param1.distributedSceneEnable, ret2->distributedSceneEnable);
    EXPECT_EQ(param1.label, ret2->label);
    EXPECT_EQ(param1.labelId, ret2->labelId);
    EXPECT_EQ(param1.description, ret2->description);
    EXPECT_EQ(param1.descriptionId, ret2->descriptionId);
}

/**
 * @tc.number: CommonEventInfoBranchCover_0001
 * @tc.name: CommonEventInfo Marshalling test
 * @tc.desc: 1.CommonEventInfo Marshalling test
 */
HWTEST_F(BmsBundleKitServiceTest, CommonEventInfoBranchCover_0001, Function | SmallTest | Level1)
{
    CommonEventInfo commonEventInfo;
    commonEventInfo.name = COMMON_EVENT_NAME;
    commonEventInfo.bundleName = BUNDLE_NAME;
    commonEventInfo.uid = 100;
    commonEventInfo.permission = COMMON_EVENT_PERMISSION;
    Parcel parcel;
    auto ret1 = commonEventInfo.Marshalling(parcel);
    EXPECT_TRUE(ret1);
    CommonEventInfo commonEventInfo2;
    auto ret2 = commonEventInfo2.Unmarshalling(parcel);
    EXPECT_NE(ret2, nullptr);
    EXPECT_EQ(commonEventInfo.name, ret2->name);
    EXPECT_EQ(commonEventInfo.bundleName, ret2->bundleName);
    EXPECT_EQ(commonEventInfo.uid, ret2->uid);
    EXPECT_EQ(commonEventInfo.permission, ret2->permission);
}

/**
 * @tc.number: CommonEventInfoBranchCover_0002
 * @tc.name: CommonEventInfo from_json test
 * @tc.desc: 1.CommonEventInfo from_json test
 */
HWTEST_F(BmsBundleKitServiceTest, CommonEventInfoBranchCover_0002, Function | SmallTest | Level1)
{
    CommonEventInfo commonEventInfo;
    commonEventInfo.name = COMMON_EVENT_NAME;
    commonEventInfo.bundleName = BUNDLE_NAME;
    commonEventInfo.uid = 100;
    commonEventInfo.permission = COMMON_EVENT_PERMISSION;
    nlohmann::json jsonObj;
    to_json(jsonObj, commonEventInfo);
    CommonEventInfo result;
    from_json(jsonObj, result);
    EXPECT_EQ(result.name, COMMON_EVENT_NAME);
    EXPECT_EQ(result.bundleName, BUNDLE_NAME);
    EXPECT_EQ(result.uid, 100);
    EXPECT_EQ(result.permission, COMMON_EVENT_PERMISSION);
}

/**
 * @tc.number: PerfProfileBranchCover_0001
 * @tc.name: PerfProfile GetBmsLoadStartTime test
 * @tc.desc: 1.PerfProfile GetBmsLoadStartTime test
 */
HWTEST_F(BmsBundleKitServiceTest, PerfProfileBranchCover_0001, Function | SmallTest | Level1)
{
    int64_t time = 100;
    PerfProfile perfProfile;
    perfProfile.SetBmsLoadStartTime(time);
    int64_t ret = perfProfile.GetBmsLoadStartTime();
    EXPECT_EQ(ret, time);
}

/**
 * @tc.number: PerfProfileBranchCover_0002
 * @tc.name: PerfProfile GetBmsLoadEndTime test
 * @tc.desc: 1.PerfProfile GetBmsLoadEndTime test
 */
HWTEST_F(BmsBundleKitServiceTest, PerfProfileBranchCover_0002, Function | SmallTest | Level1)
{
    int64_t time = 100;
    PerfProfile perfProfile;
    perfProfile.SetBmsLoadEndTime(time);
    int64_t ret = perfProfile.GetBmsLoadEndTime();
    EXPECT_EQ(ret, time);
}

/**
 * @tc.number: PerfProfileBranchCover_0003
 * @tc.name: PerfProfile GetBundleScanStartTime test
 * @tc.desc: 1.PerfProfile GetBundleScanStartTime test
 */
HWTEST_F(BmsBundleKitServiceTest, PerfProfileBranchCover_0003, Function | SmallTest | Level1)
{
    int64_t time = 100;
    PerfProfile perfProfile;
    perfProfile.SetBundleScanStartTime(time);
    int64_t ret = perfProfile.GetBundleScanStartTime();
    EXPECT_EQ(ret, time);
}

/**
 * @tc.number: PerfProfileBranchCover_0004
 * @tc.name: PerfProfile GetBundleScanEndTime test
 * @tc.desc: 1.PerfProfile GetBundleScanEndTime test
 */
HWTEST_F(BmsBundleKitServiceTest, PerfProfileBranchCover_0004, Function | SmallTest | Level1)
{
    int64_t time = 100;
    PerfProfile perfProfile;
    perfProfile.SetBundleScanEndTime(time);
    int64_t ret = perfProfile.GetBundleScanEndTime();
    EXPECT_EQ(ret, time);
}

/**
 * @tc.number: PerfProfileBranchCover_0005
 * @tc.name: PerfProfile GetBundleDownloadStartTime test
 * @tc.desc: 1.PerfProfile GetBundleDownloadStartTime test
 */
HWTEST_F(BmsBundleKitServiceTest, PerfProfileBranchCover_0005, Function | SmallTest | Level1)
{
    PerfProfile perfProfile;
    int64_t time = 100;
    perfProfile.SetBundleDownloadStartTime(time);
    int64_t ret = perfProfile.GetBundleDownloadStartTime();
    EXPECT_EQ(ret, time);
}

/**
 * @tc.number: PerfProfileBranchCover_0006
 * @tc.name: PerfProfile GetBundleDownloadStartTime test
 * @tc.desc: 1.PerfProfile GetBundleDownloadStartTime test
 */
HWTEST_F(BmsBundleKitServiceTest, PerfProfileBranchCover_0006, Function | SmallTest | Level1)
{
    PerfProfile perfProfile;
    int64_t time = 100;
    perfProfile.SetBundleDownloadEndTime(time);
    int64_t ret = perfProfile.GetBundleDownloadEndTime();
    EXPECT_EQ(ret, time);
}

/**
 * @tc.number: PerfProfileBranchCover_0007
 * @tc.name: PerfProfile GetBundleInstallStartTime test
 * @tc.desc: 1.PerfProfile GetBundleInstallStartTime test
 */
HWTEST_F(BmsBundleKitServiceTest, PerfProfileBranchCover_0007, Function | SmallTest | Level1)
{
    PerfProfile perfProfile;
    int64_t time = 100;
    perfProfile.SetBundleInstallStartTime(time);
    int64_t ret = perfProfile.GetBundleInstallStartTime();
    EXPECT_EQ(ret, time);
}

/**
 * @tc.number: PerfProfileBranchCover_0008
 * @tc.name: PerfProfile GetBundleTotalInstallTime test
 * @tc.desc: 1.PerfProfile GetBundleTotalInstallTime test
 */
HWTEST_F(BmsBundleKitServiceTest, PerfProfileBranchCover_0008, Function | SmallTest | Level1)
{
    PerfProfile perfProfile;
    int64_t ret = perfProfile.GetBundleTotalInstallTime();
    EXPECT_EQ(ret, ZERO);
}

/**
 * @tc.number: PerfProfileBranchCover_0009
 * @tc.name: PerfProfile GetBundleUninstallStartTime test
 * @tc.desc: 1.PerfProfile GetBundleUninstallStartTime test
 */
HWTEST_F(BmsBundleKitServiceTest, PerfProfileBranchCover_0009, Function | SmallTest | Level1)
{
    PerfProfile perfProfile;
    int64_t time = 100;
    perfProfile.SetBundleUninstallStartTime(time);
    int64_t ret = perfProfile.GetBundleUninstallStartTime();
    EXPECT_EQ(ret, time);
}

/**
 * @tc.number: PerfProfileBranchCover_0010
 * @tc.name: PerfProfile GetBundleUninstallEndTime test
 * @tc.desc: 1.PerfProfile GetBundleUninstallEndTime test
 */
HWTEST_F(BmsBundleKitServiceTest, PerfProfileBranchCover_0010, Function | SmallTest | Level1)
{
    PerfProfile perfProfile;
    int64_t time = 100;
    perfProfile.SetBundleUninstallEndTime(time);
    int64_t ret = perfProfile.GetBundleUninstallEndTime();
    EXPECT_EQ(ret, time);
}

/**
 * @tc.number: PerfProfileBranchCover_0011
 * @tc.name: PerfProfile GetBundleParseStartTime test
 * @tc.desc: 1.PerfProfile GetBundleParseStartTime test
 */
HWTEST_F(BmsBundleKitServiceTest, PerfProfileBranchCover_0011, Function | SmallTest | Level1)
{
    PerfProfile perfProfile;
    int64_t time = 100;
    perfProfile.SetBundleParseStartTime(time);
    int64_t ret = perfProfile.GetBundleParseStartTime();
    EXPECT_EQ(ret, time);
}

/**
 * @tc.number: PerfProfileBranchCover_0012
 * @tc.name: PerfProfile GetBundleParseEndTime test
 * @tc.desc: 1.PerfProfile GetBundleParseEndTime test
 */
HWTEST_F(BmsBundleKitServiceTest, PerfProfileBranchCover_0012, Function | SmallTest | Level1)
{
    PerfProfile perfProfile;
    int64_t time = 100;
    perfProfile.SetBundleParseEndTime(time);
    int64_t ret = perfProfile.GetBundleParseEndTime();
    EXPECT_EQ(ret, time);
}

/**
 * @tc.number: PerfProfileBranchCover_0013
 * @tc.name: PerfProfile GetAmsLoadStartTime test
 * @tc.desc: 1.PerfProfile GetAmsLoadStartTime test
 */
HWTEST_F(BmsBundleKitServiceTest, PerfProfileBranchCover_0013, Function | SmallTest | Level1)
{
    PerfProfile perfProfile;
    int64_t time = 100;
    perfProfile.SetAmsLoadStartTime(time);
    int64_t ret = perfProfile.GetAmsLoadStartTime();
    EXPECT_EQ(ret, time);
}

/**
 * @tc.number: PerfProfileBranchCover_0014
 * @tc.name: PerfProfile GetAbilityLoadStartTime test
 * @tc.desc: 1.PerfProfile GetAbilityLoadStartTime test
 */
HWTEST_F(BmsBundleKitServiceTest, PerfProfileBranchCover_0014, Function | SmallTest | Level1)
{
    PerfProfile perfProfile;
    int64_t time = 100;
    perfProfile.SetAbilityLoadStartTime(time);
    int64_t ret = perfProfile.GetAbilityLoadStartTime();
    EXPECT_EQ(ret, time);
}

/**
 * @tc.number: PerfProfileBranchCover_0015
 * @tc.name: PerfProfile GetAppForkStartTime test
 * @tc.desc: 1.PerfProfile GetAppForkStartTime test
 */
HWTEST_F(BmsBundleKitServiceTest, PerfProfileBranchCover_0015, Function | SmallTest | Level1)
{
    PerfProfile perfProfile;
    int64_t time = 100;
    perfProfile.SetAppForkStartTime(time);
    int64_t ret = perfProfile.GetAppForkStartTime();
    EXPECT_EQ(ret, time);
}

/**
 * @tc.number: PerfProfileBranchCover_0016
 * @tc.name: PerfProfile GetPerfProfileEnabled test
 * @tc.desc: 1.PerfProfile GetPerfProfileEnabled test
 */
HWTEST_F(BmsBundleKitServiceTest, PerfProfileBranchCover_0016, Function | SmallTest | Level1)
{
    PerfProfile perfProfile;
    bool enabled = true;
    perfProfile.SetPerfProfileEnabled(enabled);
    int64_t ret = perfProfile.GetPerfProfileEnabled();
    EXPECT_EQ(ret, enabled);
}

/**
 * @tc.number: PerfProfileBranchCover_0017
 * @tc.name: PerfProfile GetAmsLoadEndTime test
 * @tc.desc: 1.PerfProfile GetAmsLoadEndTime test
 */
HWTEST_F(BmsBundleKitServiceTest, PerfProfileBranchCover_0017, Function | SmallTest | Level1)
{
    PerfProfile perfProfile;
    int64_t time = 100;
    perfProfile.SetAmsLoadEndTime(time);
    int64_t ret = perfProfile.GetAmsLoadEndTime();
    EXPECT_EQ(ret, time);
}

/**
 * @tc.number: CompatibleAbilityBranchCover_0001
 * @tc.name: CompatibleAbility ReadFromParcel permission valid test
 * @tc.desc: 1.CompatibleAbility ReadFromParcel permission valid test
 */
HWTEST_F(BmsBundleKitServiceTest, CompatibleAbilityBranchCover_0001, Function | SmallTest | Level1)
{
    CompatibleAbilityInfo compatibleAbility;
    AbilityType type = AbilityType::UNKNOWN;
    DisplayOrientation orientation = DisplayOrientation::UNSPECIFIED;
    LaunchMode launchMode = LaunchMode::SINGLETON;
    std::vector<std::string> permissions;
    std::string permission = "permission";
    permissions.push_back(permission);
    compatibleAbility.type = type;
    compatibleAbility.orientation = orientation;
    compatibleAbility.launchMode = launchMode;
    compatibleAbility.permissions = permissions;
    Parcel parcel;
    bool ret1 = compatibleAbility.Marshalling(parcel);
    EXPECT_TRUE(ret1);
    CompatibleAbilityInfo compatibleAbilityInfo2;
    bool ret2 = compatibleAbilityInfo2.ReadFromParcel(parcel);
    EXPECT_TRUE(ret2);
    EXPECT_EQ(compatibleAbilityInfo2.type, compatibleAbility.type);
    EXPECT_EQ(compatibleAbilityInfo2.orientation, compatibleAbility.orientation);
    EXPECT_EQ(compatibleAbilityInfo2.launchMode, compatibleAbility.launchMode);
    EXPECT_EQ(compatibleAbilityInfo2.permissions, compatibleAbility.permissions);
}

/**
 * @tc.number: CompatibleAbilityBranchCover_0002
 * @tc.name: CompatibleAbility ReadFromParcel params  test
 * @tc.desc: 1.CompatibleAbility ReadFromParcel params  test
 */
HWTEST_F(BmsBundleKitServiceTest, CompatibleAbilityBranchCover_0002, Function | SmallTest | Level1)
{
    CompatibleAbilityInfo compatibleAbility;
    compatibleAbility.supportPipMode = false;
    compatibleAbility.grantPermission = false;
    compatibleAbility.readPermission = "readPermission";
    compatibleAbility.writePermission = "writePermisson";
    compatibleAbility.uriPermissionMode = "uriPermissionMode";
    compatibleAbility.uriPermissionPath = "uriPermissionPath";
    compatibleAbility.directLaunch = true;
    compatibleAbility.bundleName = "bundleName";
    compatibleAbility.className = "className";
    compatibleAbility.originalClassName = "originalClassName";
    compatibleAbility.deviceId = "deviceId";
    Parcel parcel;
    bool ret1 = compatibleAbility.Marshalling(parcel);
    EXPECT_TRUE(ret1);
    CompatibleAbilityInfo compatibleAbilityInfo2;
    bool ret2 = compatibleAbilityInfo2.ReadFromParcel(parcel);
    EXPECT_TRUE(ret2);
    EXPECT_EQ(compatibleAbility.supportPipMode, compatibleAbilityInfo2.supportPipMode);
    EXPECT_EQ(compatibleAbility.grantPermission, compatibleAbilityInfo2.grantPermission);
    EXPECT_EQ(compatibleAbility.readPermission, compatibleAbilityInfo2.readPermission);
    EXPECT_EQ(compatibleAbility.writePermission, compatibleAbilityInfo2.writePermission);
    EXPECT_EQ(compatibleAbility.uriPermissionMode, compatibleAbilityInfo2.uriPermissionMode);
    EXPECT_EQ(compatibleAbility.uriPermissionPath, compatibleAbilityInfo2.uriPermissionPath);
    EXPECT_EQ(compatibleAbility.directLaunch, compatibleAbilityInfo2.directLaunch);
    EXPECT_EQ(compatibleAbility.bundleName, compatibleAbilityInfo2.bundleName);
    EXPECT_EQ(compatibleAbility.className, compatibleAbilityInfo2.className);
    EXPECT_EQ(compatibleAbility.originalClassName, compatibleAbilityInfo2.originalClassName);
    EXPECT_EQ(compatibleAbility.deviceId, compatibleAbilityInfo2.deviceId);
    EXPECT_EQ(compatibleAbility.formEntity, compatibleAbilityInfo2.formEntity);
    EXPECT_EQ(compatibleAbility.minFormHeight, compatibleAbilityInfo2.minFormHeight);
    EXPECT_EQ(compatibleAbility.defaultFormHeight, compatibleAbilityInfo2.defaultFormHeight);
    EXPECT_EQ(compatibleAbility.minFormWidth, compatibleAbilityInfo2.minFormWidth);
    EXPECT_EQ(compatibleAbility.defaultFormWidth, compatibleAbilityInfo2.defaultFormWidth);
    EXPECT_EQ(compatibleAbility.iconId, compatibleAbilityInfo2.iconId);
    EXPECT_EQ(compatibleAbility.labelId, compatibleAbilityInfo2.labelId);
    EXPECT_EQ(compatibleAbility.descriptionId, compatibleAbilityInfo2.descriptionId);
    EXPECT_EQ(compatibleAbility.enabled, compatibleAbilityInfo2.enabled);
}

/**
 * @tc.number: FormInfoBranchCover_0001
 * @tc.name: FormInfo FormInfo params  test
 * @tc.desc: 1.FormInfo FormInfo params  test
 */
HWTEST_F(BmsBundleKitServiceTest, FormInfoBranchCover_0001, Function | SmallTest | Level1)
{
    ExtensionAbilityInfo extensionAbilityInfo;
    extensionAbilityInfo.bundleName = "bundleName";
    extensionAbilityInfo.moduleName = "moduleName";
    extensionAbilityInfo.name = "name";
    
    ExtensionFormInfo extensionFormInfo;
    extensionFormInfo.description = "description";
    extensionFormInfo.formConfigAbility = "formConfigAbility";
    extensionFormInfo.scheduledUpdateTime = "scheduledUpdateTime";
    extensionFormInfo.src = "src";
    extensionFormInfo.window.designWidth = 240;
    extensionFormInfo.window.autoDesignWidth = 240;
    extensionFormInfo.updateDuration = 60;
    extensionFormInfo.defaultDimension = 4;
    extensionFormInfo.isDefault = false;
    extensionFormInfo.formVisibleNotify = true;
    extensionFormInfo.updateEnabled = true;
    extensionFormInfo.type = FormType::JS;
    extensionFormInfo.colorMode = FormsColorMode::AUTO_MODE;

    FormInfo form(extensionAbilityInfo, extensionFormInfo);
    EXPECT_EQ(form.package, extensionAbilityInfo.bundleName + extensionAbilityInfo.moduleName);
    EXPECT_EQ(form.bundleName, extensionAbilityInfo.bundleName);
    EXPECT_EQ(form.originalBundleName, extensionAbilityInfo.bundleName);
    EXPECT_EQ(form.relatedBundleName, extensionAbilityInfo.bundleName);
    EXPECT_EQ(form.moduleName, extensionAbilityInfo.moduleName);
    EXPECT_EQ(form.abilityName, extensionAbilityInfo.name);
    EXPECT_EQ(form.name, extensionFormInfo.name);
    EXPECT_EQ(form.description, extensionFormInfo.description);
    EXPECT_EQ(form.jsComponentName, "");
    EXPECT_EQ(form.deepLink, "");
    EXPECT_EQ(form.formConfigAbility, extensionFormInfo.formConfigAbility);
    EXPECT_EQ(form.scheduledUpdateTime, extensionFormInfo.scheduledUpdateTime);
    EXPECT_EQ(form.src, extensionFormInfo.src);
    EXPECT_EQ(form.window.designWidth, extensionFormInfo.window.designWidth);
    EXPECT_EQ(form.window.autoDesignWidth, extensionFormInfo.window.autoDesignWidth);
    EXPECT_EQ(form.updateDuration, extensionFormInfo.updateDuration);
    EXPECT_EQ(form.defaultDimension, extensionFormInfo.defaultDimension);
    EXPECT_EQ(form.defaultFlag, extensionFormInfo.isDefault);
    EXPECT_EQ(form.formVisibleNotify, extensionFormInfo.formVisibleNotify);
    EXPECT_EQ(form.updateEnabled, extensionFormInfo.updateEnabled);
    EXPECT_EQ(form.type, extensionFormInfo.type);
    EXPECT_EQ(form.colorMode, extensionFormInfo.colorMode);
}

/**
 * @tc.number: SelfClean_0100
 * @tc.name: test SelfClean
 * @tc.desc: 1.system run normally
 */
HWTEST_F(BmsBundleKitServiceTest, SelfClean_0100, Function | SmallTest | Level0)
{
    std::shared_ptr<BundleMgrService> bundleMgrService = DelayedSingleton<BundleMgrService>::GetInstance();
    bundleMgrService -> ready_ = false;
    bundleMgrService->SelfClean();
    bool ret = bundleMgrService -> ready_;
    EXPECT_FALSE(ret);

    bundleMgrService -> ready_ = true;
    bundleMgrService -> registerToService_ = false;
    bundleMgrService->SelfClean();
    ret = bundleMgrService -> ready_;
    EXPECT_FALSE(ret);

    bundleMgrService -> registerToService_ = true;
    bundleMgrService->RegisterService();
    ret = bundleMgrService -> registerToService_;
    EXPECT_TRUE(ret);

    int32_t systemAbilityId = 0;
    const std::string deviceId = "";
    bundleMgrService->deviceManager_ = nullptr;
    bundleMgrService->OnRemoveSystemAbility(systemAbilityId, deviceId);

    bundleMgrService->deviceManager_ = std::make_shared<BmsDeviceManager>();
    bundleMgrService->OnRemoveSystemAbility(systemAbilityId, deviceId);
}

/**
 * @tc.number: VerifyPrivilegedPermission_0100
 * @tc.name: test VerifyPrivilegedPermission
 * @tc.desc: 1.system run normal
 */
HWTEST_F(BmsBundleKitServiceTest, VerifyPrivilegedPermission_0100, Function | SmallTest | Level1)
{
    MockInstallBundle(BUNDLE_NAME_TEST, MODULE_NAME_TEST, ABILITY_NAME_TEST);
    auto hostImpl = std::make_unique<BundleMgrHostImpl>();
    bool testRet = hostImpl->VerifyPrivilegedPermission(BUNDLE_NAME_TEST);
    EXPECT_TRUE(testRet);

    AbilityInfo abilityInfo;
    bool isEnabled = false;
    int32_t userId = -2;
    auto res = hostImpl->SetAbilityEnabled(abilityInfo, isEnabled, userId);
    EXPECT_EQ(res, ERR_BUNDLE_MANAGER_BUNDLE_NOT_EXIST);

    MockUninstallBundle(BUNDLE_NAME_TEST);
}

/**
 * @tc.number: QueryExtensionAbilityInfosV9_0900
 * @tc.name: 1.explicit query extension info failed, extensionInfos is empty
 * @tc.desc: 1.system run normal
 */
HWTEST_F(BmsBundleKitServiceTest, QueryExtensionAbilityInfosV9_0900, Function | SmallTest | Level1)
{
    std::shared_ptr<BundleMgrService> bundleMgrService = DelayedSingleton<BundleMgrService>::GetInstance();
    bundleMgrService->InitBundleMgrHost();
    auto hostImpl = bundleMgrService->host_;
    int32_t flags = 0;
    int32_t userId = DEFAULT_USERID;
    std::vector<ExtensionAbilityInfo> extensionInfos;
    Want want;
    ExtensionAbilityType extensionType = ExtensionAbilityType::FORM;
    auto testRet = hostImpl->QueryExtensionAbilityInfosV9(want, flags, userId, extensionInfos);
    EXPECT_EQ(testRet, ERR_BUNDLE_MANAGER_ABILITY_NOT_EXIST);

    extensionInfos.clear();
    testRet = hostImpl->QueryExtensionAbilityInfosV9(want, extensionType, flags, userId, extensionInfos);
    EXPECT_EQ(testRet, ERR_BUNDLE_MANAGER_ABILITY_NOT_EXIST);

    extensionInfos.clear();
    testRet = hostImpl->QueryExtensionAbilityInfosV9(want, flags, userId, extensionInfos);
    EXPECT_EQ(testRet, ERR_BUNDLE_MANAGER_ABILITY_NOT_EXIST);

    extensionInfos.clear();
    bool res = hostImpl->QueryExtensionAbilityInfos(want, extensionType, flags, userId, extensionInfos);
    EXPECT_FALSE(res);
}

/**
 * @tc.number: ImplicitQueryAbilityInfos_0100
 * @tc.name: test can get the ability infos
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, ImplicitQueryAbilityInfos_0100, Function | SmallTest | Level1)
{
    AAFwk::Want want;
    std::vector<AbilityInfo> abilityInfos;
    int32_t appIndex = 1;
    int32_t flags = 0;
    bool testRet = GetBundleDataMgr()->ImplicitQueryAbilityInfos(
        want, flags, DEFAULT_USERID, abilityInfos, appIndex);
    EXPECT_EQ(testRet, false);
    ErrCode testRet1 = GetBundleDataMgr()->ImplicitQueryAbilityInfosV9(
        want, flags, DEFAULT_USERID, abilityInfos, appIndex);
    EXPECT_EQ(testRet1, ERR_BUNDLE_MANAGER_ABILITY_NOT_EXIST);
}

/**
 * @tc.number: ImplicitQueryAbilityInfos_0200
 * @tc.name: test can get the ability infos
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, ImplicitQueryAbilityInfos_0200, Function | SmallTest | Level1)
{
    AAFwk::Want want;
    want.SetAction("action.system.home");
    want.AddEntity("entity.system.home");
    want.SetElementName("", "bundleName", "", "moduleName");
    std::vector<AbilityInfo> abilityInfos;
    int32_t appIndex = -1;
    int32_t flags = 0;
    ErrCode testRet = GetBundleDataMgr()->ImplicitQueryAbilityInfosV9(
        want, flags, DEFAULT_USERID, abilityInfos, appIndex);
    EXPECT_EQ(testRet, ERR_OK);
}

/**
 * @tc.number: ImplicitQueryAbilityInfos_0300
 * @tc.name: test can get the ability infos
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, ImplicitQueryAbilityInfos_0300, Function | SmallTest | Level1)
{
    AAFwk::Want want;
    std::vector<AbilityInfo> abilityInfos;
    int32_t appIndex = 0;
    int32_t flags = 0;
    GetBundleDataMgr()->ImplicitQueryAllAbilityInfos(
        want, flags, Constants::INVALID_USERID, abilityInfos, appIndex);
    ErrCode testRet = GetBundleDataMgr()->ImplicitQueryCurAbilityInfosV9(
        want, flags, DEFAULT_USERID, abilityInfos, appIndex);
    EXPECT_EQ(testRet, ERR_BUNDLE_MANAGER_BUNDLE_NOT_EXIST);

    appIndex = 1;
    want.SetAction("action.system.home");
    want.AddEntity("entity.system.home");
    want.SetElementName("", BUNDLE_NAME_TEST, "", MODULE_NAME_TEST);
    GetBundleDataMgr()->ImplicitQueryAllAbilityInfos(
        want, flags, DEFAULT_USERID, abilityInfos, appIndex);
    EXPECT_EQ(abilityInfos.size(), 0);
}

/**
 * @tc.number: ImplicitQueryAbilityInfos_0400
 * @tc.name: test can get the ability infos
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, ImplicitQueryAbilityInfos_0400, Function | SmallTest | Level1)
{
    AAFwk::Want want;
    std::vector<AbilityInfo> abilityInfos;
    int32_t appIndex = 1;
    int32_t flags = 0;
    want.SetAction("action.system.home");
    want.AddEntity("entity.system.home");
    want.SetElementName("", BUNDLE_NAME_TEST, "", MODULE_NAME_TEST);
    GetBundleDataMgr()->ImplicitQueryAllAbilityInfosV9(
        want, flags, DEFAULT_USERID, abilityInfos, appIndex);
    EXPECT_EQ(abilityInfos.size(), 0);
}

/**
 * @tc.number: ImplicitQueryExtensionInfos_0100
 * @tc.name: test can get the extension infos
 * @tc.desc: 1.system run normally
 *           2.get extension info failed
 */
HWTEST_F(BmsBundleKitServiceTest, ImplicitQueryExtensionInfos_0100, Function | SmallTest | Level1)
{
    AAFwk::Want want;
    std::vector<ExtensionAbilityInfo> extensionInfos;
    int32_t appIndex = 1;
    int32_t flags = 0;
    bool res = GetBundleDataMgr()->ImplicitQueryExtensionInfos(
        want, flags, DEFAULT_USERID, extensionInfos, appIndex);
    EXPECT_EQ(res, false);
    ErrCode res1 = GetBundleDataMgr()->ImplicitQueryExtensionInfosV9(
        want, flags, DEFAULT_USERID, extensionInfos, appIndex);
    EXPECT_EQ(res1, ERR_BUNDLE_MANAGER_ABILITY_NOT_EXIST);
}

/**
 * @tc.number: QueryAbilityInfoWithFlags_0100
 * @tc.name: test can get the ability infos
 * @tc.desc: 1.system run normally
 *           2.get ability info failed
 */
HWTEST_F(BmsBundleKitServiceTest, QueryAbilityInfoWithFlags_0100, Function | SmallTest | Level1)
{
    std::optional<AbilityInfo> option;
    InnerBundleInfo innerBundleInfo;
    AbilityInfo info;
    bool testRet = GetBundleDataMgr()->QueryAbilityInfoWithFlags(
        option, GET_ABILITY_INFO_SYSTEMAPP_ONLY, DEFAULT_USERID, innerBundleInfo, info);
    EXPECT_EQ(testRet, false);
    testRet = GetBundleDataMgr()->QueryAbilityInfoWithFlags(
        option, GET_ABILITY_INFO_WITH_PERMISSION, DEFAULT_USERID, innerBundleInfo, info);
    EXPECT_EQ(testRet, false);
}

/**
 * @tc.number: GetAllBundleInfos_0100
 * @tc.name: test can get the bundle infos
 * @tc.desc: 1.system run normally
 *           2.get bundle info failed
 */
HWTEST_F(BmsBundleKitServiceTest, GetAllBundleInfos_0100, Function | SmallTest | Level1)
{
    std::vector<BundleInfo> bundleInfos;
    int32_t flags = 0;
    InnerBundleInfo innerBundleInfo;
    GetBundleDataMgr()->bundleInfos_.insert(
        pair<std::string, InnerBundleInfo>("moduleName", innerBundleInfo));
    EXPECT_FALSE(GetBundleDataMgr()->bundleInfos_.empty());
    bool removable = false;
    GetBundleDataMgr()->UpdateRemovable("bundleName", removable);
    GetBundleDataMgr()->UpdateRemovable(BUNDLE_NAME_TEST, removable);
    bool testRet = GetBundleDataMgr()->GetAllBundleInfos(flags, bundleInfos);
    EXPECT_EQ(testRet, true);
    ErrCode testRet1 = GetBundleDataMgr()->GetAllBundleInfosV9(flags, bundleInfos);
    EXPECT_EQ(testRet1, ERR_OK);
}

/**
 * @tc.number: SetHideDesktopIcon_0001
 * @tc.name: test can SetHideDesktopIcon
 * @tc.desc: 1.system run normally
 *           2.SetHideDesktopIcon
 */
HWTEST_F(BmsBundleKitServiceTest, SetHideDesktopIcon_0001, Function | SmallTest | Level1)
{
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.baseApplicationInfo_->hideDesktopIcon = false;
    innerBundleInfo.baseApplicationInfo_->needAppDetail = true;
    innerBundleInfo.baseApplicationInfo_->appDetailAbilityLibraryPath = "path";
    innerBundleInfo.SetHideDesktopIcon(true);
    EXPECT_TRUE(innerBundleInfo.GetBaseApplicationInfo().hideDesktopIcon);
    EXPECT_FALSE(innerBundleInfo.GetBaseApplicationInfo().needAppDetail);
    EXPECT_TRUE(innerBundleInfo.GetBaseApplicationInfo().appDetailAbilityLibraryPath.empty());
}

/**
 * @tc.number: SetHideDesktopIcon_0002
 * @tc.name: test can SetHideDesktopIcon
 * @tc.desc: 1.system run normally
 *           2.SetHideDesktopIcon
 */
HWTEST_F(BmsBundleKitServiceTest, SetHideDesktopIcon_0002, Function | SmallTest | Level1)
{
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.baseApplicationInfo_->hideDesktopIcon = false;
    innerBundleInfo.baseApplicationInfo_->needAppDetail = true;
    innerBundleInfo.baseApplicationInfo_->appDetailAbilityLibraryPath = "path";
    innerBundleInfo.SetHideDesktopIcon(false);
    EXPECT_FALSE(innerBundleInfo.GetBaseApplicationInfo().hideDesktopIcon);
    EXPECT_TRUE(innerBundleInfo.GetBaseApplicationInfo().needAppDetail);
    EXPECT_FALSE(innerBundleInfo.GetBaseApplicationInfo().appDetailAbilityLibraryPath.empty());
}

/**
 * @tc.number: UpdateAppDetailAbilityAttrs_0001
 * @tc.name: test can UpdateAppDetailAbilityAttrs
 * @tc.desc: 1.system run normally
 *           2.UpdateAppDetailAbilityAttrs
 */
HWTEST_F(BmsBundleKitServiceTest, UpdateAppDetailAbilityAttrs_0001, Function | SmallTest | Level1)
{
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.baseApplicationInfo_->hideDesktopIcon = true;
    std::string keyName = Constants::APP_DETAIL_ABILITY;
    AbilityInfo abilityInfo;
    abilityInfo.name = Constants::APP_DETAIL_ABILITY;
    innerBundleInfo.InsertAbilitiesInfo(keyName, abilityInfo);

    innerBundleInfo.UpdateAppDetailAbilityAttrs();
    EXPECT_TRUE(innerBundleInfo.GetBaseApplicationInfo().hideDesktopIcon);
    EXPECT_FALSE(innerBundleInfo.GetBaseApplicationInfo().needAppDetail);
}

/**
 * @tc.number: UpdateAppDetailAbilityAttrs_0002
 * @tc.name: test can UpdateAppDetailAbilityAttrs
 * @tc.desc: 1.system run normally
 *           2.UpdateAppDetailAbilityAttrs
 */
HWTEST_F(BmsBundleKitServiceTest, UpdateAppDetailAbilityAttrs_0002, Function | SmallTest | Level1)
{
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.baseApplicationInfo_->hideDesktopIcon = false;
    innerBundleInfo.baseApplicationInfo_->needAppDetail = false;
    innerBundleInfo.UpdateAppDetailAbilityAttrs();
    EXPECT_FALSE(innerBundleInfo.GetBaseApplicationInfo().hideDesktopIcon);
    EXPECT_FALSE(innerBundleInfo.GetBaseApplicationInfo().needAppDetail);
}

/**
 * @tc.number: UpdateAppDetailAbilityAttrs_0003
 * @tc.name: test can UpdateAppDetailAbilityAttrs
 * @tc.desc: 1.system run normally
 *           2.UpdateAppDetailAbilityAttrs
 */
HWTEST_F(BmsBundleKitServiceTest, UpdateAppDetailAbilityAttrs_0003, Function | SmallTest | Level1)
{
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.baseApplicationInfo_->needAppDetail = true;
    Skill skill {{ACTION}, {ENTITY}};
    std::vector<Skill> skills;
    skills.emplace_back(skill);
    innerBundleInfo.InsertSkillInfo(BUNDLE_NAME, skills);
    AbilityInfo abilityInfo;
    abilityInfo.type = AbilityType::PAGE;
    innerBundleInfo.InsertAbilitiesInfo(BUNDLE_NAME, abilityInfo);
    innerBundleInfo.UpdateAppDetailAbilityAttrs();
    EXPECT_FALSE(innerBundleInfo.GetBaseApplicationInfo().hideDesktopIcon);
    EXPECT_FALSE(innerBundleInfo.GetBaseApplicationInfo().needAppDetail);
}

/**
 * @tc.number: UpdateAppDetailAbilityAttrs_0004
 * @tc.name: test can UpdateAppDetailAbilityAttrs
 * @tc.desc: 1.system run normally
 *           2.UpdateAppDetailAbilityAttrs
 */
HWTEST_F(BmsBundleKitServiceTest, UpdateAppDetailAbilityAttrs_0004, Function | SmallTest | Level1)
{
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.baseApplicationInfo_->needAppDetail = true;
    innerBundleInfo.UpdateAppDetailAbilityAttrs();
    EXPECT_FALSE(innerBundleInfo.GetBaseApplicationInfo().hideDesktopIcon);
    EXPECT_TRUE(innerBundleInfo.GetBaseApplicationInfo().needAppDetail);
}

/**
 * @tc.number: UpdateAppDetailAbilityAttrs_0005
 * @tc.name: test can UpdateAppDetailAbilityAttrs
 * @tc.desc: 1.system run normally
 *           2.UpdateAppDetailAbilityAttrs
 */
HWTEST_F(BmsBundleKitServiceTest, UpdateAppDetailAbilityAttrs_0005, Function | SmallTest | Level1)
{
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.baseApplicationInfo_->needAppDetail = true;
    Skill skill {{ACTION}, {ENTITY}};
    std::vector<Skill> skills;
    skills.emplace_back(skill);
    innerBundleInfo.InsertSkillInfo(BUNDLE_NAME, skills);
    AbilityInfo abilityInfo;
    abilityInfo.type = AbilityType::DATA;
    innerBundleInfo.InsertAbilitiesInfo(BUNDLE_NAME, abilityInfo);
    innerBundleInfo.UpdateAppDetailAbilityAttrs();
    EXPECT_FALSE(innerBundleInfo.GetBaseApplicationInfo().hideDesktopIcon);
    EXPECT_TRUE(innerBundleInfo.GetBaseApplicationInfo().needAppDetail);
}

/**
 * @tc.number: UpdateAppDetailAbilityAttrs_0006
 * @tc.name: test can UpdateAppDetailAbilityAttrs
 * @tc.desc: 1.system run normally
 *           2.UpdateAppDetailAbilityAttrs
 */
HWTEST_F(BmsBundleKitServiceTest, UpdateAppDetailAbilityAttrs_0006, Function | SmallTest | Level1)
{
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.baseApplicationInfo_->needAppDetail = true;
    Skill skill {{ACTION}, {ENTITY}};
    std::vector<Skill> skills;
    skills.emplace_back(skill);
    innerBundleInfo.InsertSkillInfo(BUNDLE_NAME, skills);
    AbilityInfo abilityInfo;
    innerBundleInfo.InsertAbilitiesInfo(MODULE_NAME, abilityInfo);

    abilityInfo.name = Constants::APP_DETAIL_ABILITY;
    abilityInfo.type = AbilityType::PAGE;
    innerBundleInfo.InsertAbilitiesInfo(BUNDLE_NAME, abilityInfo);

    innerBundleInfo.UpdateAppDetailAbilityAttrs();
    EXPECT_FALSE(innerBundleInfo.GetBaseApplicationInfo().hideDesktopIcon);
    EXPECT_FALSE(innerBundleInfo.GetBaseApplicationInfo().needAppDetail);
}

/**
 * @tc.number: UpdateAppDetailAbilityAttrs_0007
 * @tc.name: test can UpdateAppDetailAbilityAttrs
 * @tc.desc: 1.system run normally
 *           2.UpdateAppDetailAbilityAttrs
 */
HWTEST_F(BmsBundleKitServiceTest, UpdateAppDetailAbilityAttrs_0007, Function | SmallTest | Level1)
{
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.baseApplicationInfo_->needAppDetail = false;
    AbilityInfo abilityInfo;
    innerBundleInfo.InsertAbilitiesInfo(MODULE_NAME, abilityInfo);

    abilityInfo.name = Constants::APP_DETAIL_ABILITY;
    abilityInfo.type = AbilityType::PAGE;
    innerBundleInfo.InsertAbilitiesInfo(BUNDLE_NAME, abilityInfo);

    innerBundleInfo.UpdateAppDetailAbilityAttrs();
    EXPECT_FALSE(innerBundleInfo.GetBaseApplicationInfo().hideDesktopIcon);
    EXPECT_FALSE(innerBundleInfo.GetBaseApplicationInfo().needAppDetail);
}

/**
 * @tc.number: UpdateAppDetailAbilityAttrs_0008
 * @tc.name: test can UpdateAppDetailAbilityAttrs
 * @tc.desc: 1.system run normally
 *           2.UpdateAppDetailAbilityAttrs
 */
HWTEST_F(BmsBundleKitServiceTest, UpdateAppDetailAbilityAttrs_0008, Function | SmallTest | Level1)
{
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.baseApplicationInfo_->needAppDetail = true;
    AbilityInfo abilityInfo;
    innerBundleInfo.InsertAbilitiesInfo(ABILITY_NAME, abilityInfo);

    abilityInfo.name = Constants::APP_DETAIL_ABILITY;
    abilityInfo.type = AbilityType::PAGE;
    innerBundleInfo.InsertAbilitiesInfo(BUNDLE_NAME, abilityInfo);

    innerBundleInfo.SetIsNewVersion(true);
    innerBundleInfo.UpdateAppDetailAbilityAttrs();

    innerBundleInfo.baseApplicationInfo_->iconId = 1;
    innerBundleInfo.UpdateAppDetailAbilityAttrs();
    EXPECT_FALSE(innerBundleInfo.GetBaseApplicationInfo().hideDesktopIcon);
    EXPECT_TRUE(innerBundleInfo.GetBaseApplicationInfo().needAppDetail);
}
}