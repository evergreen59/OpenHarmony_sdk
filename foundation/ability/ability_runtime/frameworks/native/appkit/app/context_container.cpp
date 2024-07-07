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
#include "context_container.h"

#include <regex>

#include "ability_constants.h"
#include "ability_manager_client.h"
#include "ability_manager_errors.h"
#include "app_context.h"
#include "bundle_constants.h"
#include "hilog_wrapper.h"
#include "parameters.h"

namespace OHOS {
namespace AppExecFwk {
// for api7 demo special
constexpr int CURRENT_ACCOUNT_ID = 100;
/**
 * Attaches a Context object to the current ability.
 * Generally, this method is called after Ability is loaded to provide the application context for the current ability.
 *
 * @param base Indicates a Context object.
 */
void ContextContainer::AttachBaseContext(const std::shared_ptr<Context> &base)
{
    if (base == nullptr) {
        HILOG_ERROR("ContextDeal::AttachBaseContext failed, base is nullptr");
        return;
    }
    baseContext_ = base;
}

void ContextContainer::DetachBaseContext()
{
    if (baseContext_ != nullptr) {
        baseContext_.reset();
    }
    baseContext_ = nullptr;
}

/**
 * Called when getting the ProcessInfo
 *
 * @return ProcessInfo
 */
std::shared_ptr<ProcessInfo> ContextContainer::GetProcessInfo() const
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetProcessInfo();
    }
    return nullptr;
}

/**
 * @brief Obtains information about the current application. The returned application information includes basic
 * information such as the application name and application permissions.
 *
 * @return Returns the ApplicationInfo for the current application.
 */
std::shared_ptr<ApplicationInfo> ContextContainer::GetApplicationInfo() const
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetApplicationInfo();
    } else {
        HILOG_ERROR("ContextContainer::GetApplicationInfo baseContext_ is nullptr");
        return nullptr;
    }
}

/**
 * @brief Obtains the Context object of the application.
 *
 * @return Returns the Context object of the application.
 */
std::shared_ptr<Context> ContextContainer::GetApplicationContext() const
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetApplicationContext();
    } else {
        HILOG_ERROR("ContextContainer::GetApplicationContext baseContext_ is nullptr");
        return nullptr;
    }
}

/**
 * @brief Obtains the path of the package containing the current ability. The returned path contains the resources,
 *  source code, and configuration files of a module.
 *
 * @return Returns the path of the package file.
 */
std::string ContextContainer::GetBundleCodePath()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetBundleCodePath();
    } else {
        HILOG_ERROR("ContextContainer::GetBundleCodePath baseContext_ is nullptr");
        return "";
    }
}

/**
 * @brief Obtains information about the current ability.
 * The returned information includes the class name, bundle name, and other information about the current ability.
 *
 * @return Returns the AbilityInfo object for the current ability.
 */
const std::shared_ptr<AbilityInfo> ContextContainer::GetAbilityInfo()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetAbilityInfo();
    } else {
        HILOG_ERROR("ContextContainer::GetAbilityInfo baseContext_ is nullptr");
        return nullptr;
    }
}

/**
 * @brief Obtains the Context object of the application.
 *
 * @return Returns the Context object of the application.
 */
std::shared_ptr<Context> ContextContainer::GetContext()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetContext();
    } else {
        HILOG_ERROR("ContextContainer::GetContext baseContext_ is nullptr");
        return nullptr;
    }
}

/**
 * @brief Obtains an IBundleMgr instance.
 * You can use this instance to obtain information about the application bundle.
 *
 * @return Returns an IBundleMgr instance.
 */
sptr<IBundleMgr> ContextContainer::GetBundleManager() const
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetBundleManager();
    } else {
        HILOG_ERROR("ContextContainer::GetBundleManager baseContext_ is nullptr");
        return nullptr;
    }
}

/**
 * @brief Obtains a resource manager.
 *
 * @return Returns a ResourceManager object.
 */
std::shared_ptr<Global::Resource::ResourceManager> ContextContainer::GetResourceManager() const
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetResourceManager();
    } else {
        HILOG_ERROR("ContextContainer::GetResourceManager baseContext_ is nullptr");
        return nullptr;
    }
}

/**
 * @brief Deletes the specified private file associated with the application.
 *
 * @param fileName Indicates the name of the file to delete. The file name cannot contain path separators.
 *
 * @return Returns true if the file is deleted successfully; returns false otherwise.
 */
bool ContextContainer::DeleteFile(const std::string &fileName)
{
    if (baseContext_ != nullptr) {
        return baseContext_->DeleteFile(fileName);
    } else {
        HILOG_ERROR("ContextContainer::DeleteFile baseContext_ is nullptr");
        return false;
    }
}

/**
 * @brief Obtains the application-specific cache directory on the device's internal storage. The system
 * automatically deletes files from the cache directory if disk space is required elsewhere on the device.
 * Older files are always deleted first.
 *
 * @return Returns the application-specific cache directory.
 */
std::string ContextContainer::GetCacheDir()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetCacheDir();
    } else {
        HILOG_ERROR("ContextContainer::GetCacheDir baseContext_ is nullptr");
        return "";
    }
}

/**
 * @brief Obtains the application-specific code-cache directory on the device's internal storage.
 * The system will delete any files stored in this location both when your specific application is upgraded,
 * and when the entire platform is upgraded.
 *
 * @return Returns the application-specific code-cache directory.
 */
std::string ContextContainer::GetCodeCacheDir()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetCodeCacheDir();
    } else {
        HILOG_ERROR("ContextContainer::GetCodeCacheDir baseContext_ is nullptr");
        return "";
    }
}

/**
 * @brief Obtains the local database path.
 * If the local database path does not exist, the system creates one and returns the created path.
 *
 * @return Returns the local database file.
 */
std::string ContextContainer::GetDatabaseDir()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetDatabaseDir();
    } else {
        HILOG_ERROR("ContextContainer::GetDatabaseDir baseContext_ is nullptr");
        return "";
    }
}

/**
 * @brief Obtains the absolute path where all private data files of this application are stored.
 *
 * @return Returns the absolute path storing all private data files of this application.
 */
std::string ContextContainer::GetDataDir()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetDataDir();
    } else {
        HILOG_ERROR("ContextContainer::GetDataDir baseContext_ is nullptr");
        return "";
    }
}

/**
 * @brief Obtains the directory for storing custom data files of the application.
 * You can use the returned File object to create and access files in this directory. The files
 * can be accessible only by the current application.
 *
 * @param name Indicates the name of the directory to retrieve. This directory is created as part
 * of your application data.
 * @param mode Indicates the file operating mode. The value can be 0 or a combination of MODE_PRIVATE.
 *
 * @return Returns a File object for the requested directory.
 */
std::string ContextContainer::GetDir(const std::string &name, int mode)
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetDir(name, mode);
    } else {
        HILOG_ERROR("ContextContainer::GetDir baseContext_ is nullptr");
        return "";
    }
}

/**
 * @brief Obtains the absolute path to the application-specific cache directory
 * on the primary external or shared storage device.
 *
 * @return Returns the absolute path to the application-specific cache directory on the external or
 * shared storage device; returns null if the external or shared storage device is temporarily unavailable.
 */
std::string ContextContainer::GetExternalCacheDir()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetExternalCacheDir();
    } else {
        HILOG_ERROR("ContextContainer::GetExternalCacheDir baseContext_ is nullptr");
        return "";
    }
}

/**
 * @brief Obtains the absolute path to the directory for storing files for the application on the
 * primary external or shared storage device.
 *
 * @param type Indicates the type of the file directory to return
 *
 * @return Returns the absolute path to the application file directory on the external or shared storage
 * device; returns null if the external or shared storage device is temporarily unavailable.
 */
std::string ContextContainer::GetExternalFilesDir(std::string &type)
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetExternalFilesDir(type);
    } else {
        HILOG_ERROR("ContextContainer::GetExternalFilesDir baseContext_ is nullptr");
        return "";
    }
}

/**
 * @brief Obtains the directory for storing files for the application on the device's internal storage.
 *
 * @return Returns the application file directory.
 */
std::string ContextContainer::GetFilesDir()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetFilesDir();
    } else {
        HILOG_ERROR("ContextContainer::GetFilesDir baseContext_ is nullptr");
        return "";
    }
}

/**
 * @brief Obtains the absolute path which app created and will be excluded from automatic backup to remote storage.
 * The returned path maybe changed if the application is moved to an adopted storage device.
 *
 * @return The path of the directory holding application files that will not be automatically backed up to remote
 * storage.
 */
std::string ContextContainer::GetNoBackupFilesDir()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetNoBackupFilesDir();
    } else {
        HILOG_ERROR("ContextContainer::GetNoBackupFilesDir baseContext_ is nullptr");
        return "";
    }
}

/**
 * @brief Checks whether the current process has the given permission.
 * You need to call requestPermissionsFromUser(std::vector<std::string>,std::vector<int>, int) to request a permission
 * only if the current process does not have the specific permission.
 *
 * @param permission Indicates the permission to check. This parameter cannot be null.
 *
 * @return Returns 0 (IBundleManager.PERMISSION_GRANTED) if the current process has the permission;
 * returns -1 (IBundleManager.PERMISSION_DENIED) otherwise.
 */
int ContextContainer::VerifySelfPermission(const std::string &permission)
{
    if (baseContext_ != nullptr) {
        return baseContext_->VerifySelfPermission(permission);
    } else {
        HILOG_ERROR("ContextContainer::VerifySelfPermission baseContext_ is nullptr");
        return AppExecFwk::Constants::PERMISSION_NOT_GRANTED;
    }
}

/**
 * @brief Obtains the bundle name of the current ability.
 *
 * @return Returns the bundle name of the current ability.
 */
std::string ContextContainer::GetBundleName() const
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetBundleName();
    } else {
        HILOG_ERROR("ContextContainer::GetBundleName baseContext_ is nullptr");
        return "";
    }
}

/**
 * @brief Obtains the path of the OHOS Ability Package (HAP} containing this ability.
 *
 * @return Returns the path of the HAP containing this ability.
 */
std::string ContextContainer::GetBundleResourcePath()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetBundleResourcePath();
    } else {
        HILOG_ERROR("ContextContainer::GetBundleResourcePath baseContext_ is nullptr");
        return "";
    }
}

/**
 * @brief Remove permissions for all users who have access to specific permissions
 *
 * @param permission Indicates the permission to unauth. This parameter cannot be null.
 * @param uri Indicates the URI to unauth. This parameter cannot be null.
 * @param uid Indicates the UID of the unauth to check.
 *
 */
void ContextContainer::UnauthUriPermission(const std::string &permission, const Uri &uri, int uid)
{
    if (baseContext_ != nullptr) {
        baseContext_->UnauthUriPermission(permission, uri, uid);
    } else {
        HILOG_ERROR("ContextContainer::UnauthUriPermission baseContext_ is nullptr");
    }
}

/**
 * @brief Obtains an ability manager.
 * The ability manager provides information about running processes and memory usage of an application.
 *
 * @return Returns an IAbilityManager instance.
 */
sptr<AAFwk::IAbilityManager> ContextContainer::GetAbilityManager()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetAbilityManager();
    } else {
        HILOG_ERROR("ContextContainer::GetAbilityManager baseContext_ is nullptr");
        return nullptr;
    }
}

/**
 * @brief Obtains the type of this application.
 *
 * @return Returns system if this application is a system application;
 * returns normal if it is released in OHOS AppGallery;
 * returns other if it is released by a third-party vendor;
 * returns an empty string if the query fails.
 */
std::string ContextContainer::GetAppType()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetAppType();
    } else {
        HILOG_ERROR("ContextContainer::GetAppType baseContext_ is nullptr");
        return "";
    }
}

/**
 * @brief Query whether the application of the specified PID and UID has been granted a certain permission
 *
 * @param permissions Indicates the list of permissions to be requested. This parameter cannot be null.
 * @param pid Process id
 * @param uid
 * @return Returns 0 (IBundleManager.PERMISSION_GRANTED) if the current process has the permission;
 * returns -1 (IBundleManager.PERMISSION_DENIED) otherwise.
 */
int ContextContainer::VerifyPermission(const std::string &permission, int pid, int uid)
{
    if (baseContext_ != nullptr) {
        return baseContext_->VerifyPermission(permission, pid, uid);
    } else {
        HILOG_ERROR("ContextContainer::VerifyPermission baseContext_ is nullptr");
        return AppExecFwk::Constants::PERMISSION_NOT_GRANTED;
    }
}

/**
 * @brief Obtains the distributed file path.
 * If the distributed file path does not exist, the system creates one and returns the created path. This method is
 * applicable only to the context of an ability rather than that of an application.
 *
 * @return Returns the distributed file.
 */
std::string ContextContainer::GetDistributedDir()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetDistributedDir();
    } else {
        HILOG_ERROR("ContextContainer::GetDistributedDir baseContext_ is nullptr");
        return "";
    }
}
/**
 * @brief Sets the pattern of this Context based on the specified pattern ID.
 *
 * @param patternId Indicates the resource ID of the pattern to set.
 */
void ContextContainer::SetPattern(int patternId)
{
    if (baseContext_ != nullptr) {
        baseContext_->SetPattern(patternId);
    } else {
        HILOG_ERROR("ContextContainer::SetPattern baseContext_ is nullptr");
    }
}

/**
 * @brief Obtains the Context object of this ability.
 *
 * @return Returns the Context object of this ability.
 */
std::shared_ptr<Context> ContextContainer::GetAbilityPackageContext()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetAbilityPackageContext();
    } else {
        HILOG_ERROR("ContextContainer::GetAbilityPackageContext baseContext_ is nullptr");
        return nullptr;
    }
}

/**
 * @brief Obtains the HapModuleInfo object of the application.
 *
 * @return Returns the HapModuleInfo object of the application.
 */
std::shared_ptr<HapModuleInfo> ContextContainer::GetHapModuleInfo()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetHapModuleInfo();
    } else {
        HILOG_ERROR("ContextContainer::GetHapModuleInfo baseContext_ is nullptr");
        return nullptr;
    }
}

/**
 * @brief Obtains the name of the current process.
 *
 * @return Returns the current process name.
 */
std::string ContextContainer::GetProcessName()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetProcessName();
    } else {
        HILOG_ERROR("ContextContainer::GetProcessName baseContext_ is nullptr");
        return "";
    }
}

/**
 * @brief Requests certain permissions from the system.
 * This method is called for permission request. This is an asynchronous method. When it is executed,
 * the task will be called back.
 *
 * @param permissions Indicates the list of permissions to be requested. This parameter cannot be null.
 * @param permissionsState Indicates the list of permissions' state to be requested. This parameter cannot be null.
 * @param task The callback or promise fo js interface.
 */
void ContextContainer::RequestPermissionsFromUser(std::vector<std::string> &permissions,
    std::vector<int> &permissionsState, PermissionRequestTask &&task)
{
    if (baseContext_ != nullptr) {
        baseContext_->RequestPermissionsFromUser(permissions, permissionsState, std::move(task));
    } else {
        HILOG_ERROR("ContextContainer::RequestPermissionsFromUser baseContext_ is nullptr");
    }
}

std::shared_ptr<Context> ContextContainer::CreateBundleContext(std::string bundleName, int flag, int accountId)
{
    if (bundleName.empty()) {
        HILOG_ERROR("ContextContainer::CreateBundleContext bundleName is empty");
        return nullptr;
    }

    if (strcmp(bundleName.c_str(), GetBundleName().c_str()) == 0) {
        return GetApplicationContext();
    }

    sptr<IBundleMgr> bundleMgr = GetBundleManager();
    if (bundleMgr == nullptr) {
        HILOG_ERROR("ContextContainer::CreateBundleContext GetBundleManager is nullptr");
        return nullptr;
    }

    BundleInfo bundleInfo;
    HILOG_INFO("CreateBundleContext length: %{public}zu, bundleName: %{public}s, accountId is %{public}d",
        bundleName.length(),
        bundleName.c_str(),
        accountId);
    int realAccountId = CURRENT_ACCOUNT_ID;
    if (accountId != DEFAULT_ACCOUNT_ID) {
        realAccountId = accountId;
    }
    bundleMgr->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo, realAccountId);

    if (bundleInfo.name.empty() || bundleInfo.applicationInfo.name.empty()) {
        HILOG_ERROR("ContextContainer::CreateBundleContext GetBundleInfo is error");
        return nullptr;
    }

    std::shared_ptr<AppContext> appContext = std::make_shared<AppContext>();
    std::shared_ptr<ContextDeal> deal = std::make_shared<ContextDeal>(true);

    // init resourceManager.
    InitResourceManager(bundleInfo, deal);

    deal->SetApplicationInfo(std::make_shared<ApplicationInfo>(bundleInfo.applicationInfo));
    appContext->AttachBaseContext(deal);
    return appContext;
}

void ContextContainer::InitResourceManager(BundleInfo &bundleInfo, std::shared_ptr<ContextDeal> &deal)
{
    std::shared_ptr<Global::Resource::ResourceManager> resourceManager(Global::Resource::CreateResourceManager());
    if (deal == nullptr || resourceManager == nullptr) {
        HILOG_ERROR("ContextContainer::InitResourceManager create resourceManager failed");
        return;
    }

    HILOG_DEBUG(
        "ContextContainer::InitResourceManager hapModuleInfos count: %{public}zu", bundleInfo.hapModuleInfos.size());
    std::regex pattern(AbilityRuntime::Constants::ABS_CODE_PATH);
    for (auto hapModuleInfo : bundleInfo.hapModuleInfos) {
        std::string loadPath;
        if (system::GetBoolParameter(AbilityRuntime::Constants::COMPRESS_PROPERTY, false) &&
            !hapModuleInfo.hapPath.empty()) {
            loadPath = hapModuleInfo.hapPath;
        } else {
            loadPath = hapModuleInfo.resourcePath;
        }
        if (loadPath.empty()) {
            continue;
        }
        loadPath = std::regex_replace(loadPath, pattern, AbilityRuntime::Constants::LOCAL_BUNDLES);
        HILOG_DEBUG("ContextContainer::InitResourceManager loadPath: %{public}s", loadPath.c_str());
        if (!resourceManager->AddResource(loadPath.c_str())) {
            HILOG_ERROR("ContextContainer::InitResourceManager AddResource failed");
        }
    }

    std::unique_ptr<Global::Resource::ResConfig> resConfig(Global::Resource::CreateResConfig());
    resConfig->SetLocaleInfo("zh", "Hans", "CN");
#ifdef SUPPORT_GRAPHICS
    if (resConfig->GetLocaleInfo() != nullptr) {
        HILOG_INFO(
            "ContextContainer::InitResourceManager language: %{public}s, script: %{public}s, region: %{public}s,",
            resConfig->GetLocaleInfo()->getLanguage(),
            resConfig->GetLocaleInfo()->getScript(),
            resConfig->GetLocaleInfo()->getCountry());
    } else {
        HILOG_INFO("ContextContainer::InitResourceManager language: GetLocaleInfo is null.");
    }
#endif
    resourceManager->UpdateResConfig(*resConfig);
    deal->initResourceManager(resourceManager);
}
/**
 * @brief Obtains information about the caller of this ability.
 *
 * @return Returns the caller information.
 */
Uri ContextContainer::GetCaller()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetCaller();
    } else {
        HILOG_ERROR("ContextContainer::GetCaller baseContext_ is nullptr");
        Uri uri("");
        return uri;
    }
}

/**
 * @brief Get the string of this Context based on the specified resource ID.
 *
 * @param resId Indicates the resource ID of the string to get.
 *
 * @return Returns the string of this Context.
 */
std::string ContextContainer::GetString(int resId)
{
    if (baseContext_ != nullptr) {
        std::string ret = baseContext_->GetString(resId);
        return ret;
    } else {
        HILOG_ERROR("ContextContainer::GetString baseContext_ is nullptr");
        return "";
    }
}

/**
 * @brief Get the string array of this Context based on the specified resource ID.
 *
 * @param resId Indicates the resource ID of the string array to get.
 *
 * @return Returns the string array of this Context.
 */
std::vector<std::string> ContextContainer::GetStringArray(int resId)
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetStringArray(resId);
    } else {
        HILOG_ERROR("ContextContainer::GetStringArray baseContext_ is nullptr");
        return std::vector<std::string>();
    }
}

/**
 * @brief Get the integer array of this Context based on the specified resource ID.
 *
 * @param resId Indicates the resource ID of the integer array to get.
 *
 * @return Returns the integer array of this Context.
 */
std::vector<int> ContextContainer::GetIntArray(int resId)
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetIntArray(resId);
    } else {
        HILOG_ERROR("ContextContainer::GetIntArray baseContext_ is nullptr");
        return std::vector<int>();
    }
}

/**
 * @brief Obtains the theme of this Context.
 *
 * @return theme Returns the theme of this Context.
 */
std::map<std::string, std::string> ContextContainer::GetTheme()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetTheme();
    } else {
        HILOG_ERROR("ContextContainer::GetTheme baseContext_ is nullptr");
        return std::map<std::string, std::string>();
    }
}

/**
 * @brief Sets the theme of this Context based on the specified theme ID.
 *
 * @param themeId Indicates the resource ID of the theme to set.
 */
void ContextContainer::SetTheme(int themeId)
{
    if (baseContext_ != nullptr) {
        baseContext_->SetTheme(themeId);
    } else {
        HILOG_ERROR("ContextContainer::SetTheme baseContext_ is nullptr");
    }
}

/**
 * @brief Obtains the pattern of this Context.
 *
 * @return getPattern in interface Context
 */
std::map<std::string, std::string> ContextContainer::GetPattern()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetPattern();
    } else {
        HILOG_ERROR("ContextContainer::GetPattern baseContext_ is nullptr");
        return std::map<std::string, std::string>();
    }
}

/**
 * @brief Get the color of this Context based on the specified resource ID.
 *
 * @param resId Indicates the resource ID of the color to get.
 *
 * @return Returns the color value of this Context.
 */
int ContextContainer::GetColor(int resId)
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetColor(resId);
    } else {
        HILOG_ERROR("ContextContainer::GetColor baseContext_ is nullptr");
        return INVALID_RESOURCE_VALUE;
    }
}

/**
 * @brief Obtains the theme id of this Context.
 *
 * @return int Returns the theme id of this Context.
 */
int ContextContainer::GetThemeId()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetThemeId();
    } else {
        HILOG_ERROR("ContextContainer::GetThemeId baseContext_ is nullptr");
        return -1;
    }
}

/**
 * @brief Obtains the current display orientation of this ability.
 *
 * @return Returns the current display orientation.
 */
int ContextContainer::GetDisplayOrientation()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetDisplayOrientation();
    } else {
        HILOG_ERROR("ContextContainer::GetDisplayOrientation baseContext_ is nullptr");
        return static_cast<int>(DisplayOrientation::UNSPECIFIED);
    }
}

/**
 * @brief Obtains the path storing the preference file of the application.
 *        If the preference file path does not exist, the system creates one and returns the created path.
 *
 * @return Returns the preference file path .
 */
std::string ContextContainer::GetPreferencesDir()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetPreferencesDir();
    } else {
        HILOG_ERROR("ContextContainer::GetPreferencesDir baseContext_ is nullptr");
        return "";
    }
}

/**
 * @brief Set color mode
 *
 * @param the value of color mode.
 */
void ContextContainer::SetColorMode(int mode)
{
    if (baseContext_ == nullptr) {
        HILOG_ERROR("ContextContainer::SetColorMode baseContext_ is nullptr");
        return;
    }

    baseContext_->SetColorMode(mode);
}

/**
 * @brief Obtains color mode.
 *
 * @return Returns the color mode value.
 */
int ContextContainer::GetColorMode()
{
    if (baseContext_ == nullptr) {
        HILOG_ERROR("ContextContainer::GetColorMode baseContext_ is nullptr");
        return -1;
    }

    return baseContext_->GetColorMode();
}

/**
 * @brief Obtains the unique ID of the mission containing this ability.
 *
 * @return Returns the unique mission ID.
 */
int ContextContainer::GetMissionId()
{
    if (baseContext_ != nullptr) {
        return baseContext_->GetMissionId();
    } else {
        HILOG_ERROR("ContextContainer::GetMissionId baseContext_ is nullptr");
        return -1;
    }
}

bool ContextContainer::IsUpdatingConfigurations()
{
    if (baseContext_ != nullptr) {
        return baseContext_->IsUpdatingConfigurations();
    }
    return false;
}

bool ContextContainer::PrintDrawnCompleted()
{
    if (baseContext_ != nullptr) {
        return baseContext_->PrintDrawnCompleted();
    }
    return false;
}
}  // namespace AppExecFwk
}  // namespace OHOS
