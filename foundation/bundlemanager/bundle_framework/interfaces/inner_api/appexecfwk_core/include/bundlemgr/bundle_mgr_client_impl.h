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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_BUNDLE_MGR_CLIENT_IMPL_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_BUNDLE_MGR_CLIENT_IMPL_H

#include "appexecfwk_errors.h"
#include "application_info.h"
#include "bundle_info.h"
#include "bundle_pack_info.h"
#include "bundle_mgr_interface.h"
#include "extension_ability_info.h"
#include "hap_module_info.h"
#ifdef GLOBAL_RESMGR_ENABLE
#include "resource_manager.h"
#endif

namespace OHOS {
namespace AppExecFwk {
using Want = OHOS::AAFwk::Want;

class BundleMgrClientImpl : public std::enable_shared_from_this<BundleMgrClientImpl> {
public:
    BundleMgrClientImpl();
    virtual ~BundleMgrClientImpl();

    bool GetBundleNameForUid(const int uid, std::string &bundleName);
    bool GetBundleInfo(const std::string &bundleName, const BundleFlag flag, BundleInfo &bundleInfo, int32_t userId);
    ErrCode GetBundlePackInfo(
        const std::string &bundleName, const BundlePackFlag flag, BundlePackInfo &bundlePackInfo, int32_t userId);
    bool GetHapModuleInfo(const std::string &bundleName, const std::string &hapName, HapModuleInfo &hapModuleInfo);
    bool GetResConfigFile(const HapModuleInfo &hapModuleInfo, const std::string &metadataName,
        std::vector<std::string> &profileInfos) const;
    bool GetResConfigFile(const ExtensionAbilityInfo &extensionInfo, const std::string &metadataName,
        std::vector<std::string> &profileInfos) const;
    bool GetResConfigFile(const AbilityInfo &abilityInfo, const std::string &metadataName,
        std::vector<std::string> &profileInfos) const;
    std::vector<std::string> GetAccessibleAppCodePaths(int32_t userId);
    ErrCode InstallSandboxApp(const std::string &bundleName, int32_t dlpType, int32_t userId, int32_t &appIndex);
    ErrCode UninstallSandboxApp(const std::string &bundleName, int32_t appIndex, int32_t userId);
    ErrCode GetSandboxBundleInfo(const std::string &bundleName, int32_t appIndex, int32_t userId, BundleInfo &info);
    bool GetProfileFromExtension(const ExtensionAbilityInfo &extensionInfo, const std::string &metadataName,
        std::vector<std::string> &profileInfos) const;
    bool GetProfileFromAbility(const AbilityInfo &abilityInfo, const std::string &metadataName,
        std::vector<std::string> &profileInfos) const;
    bool GetProfileFromHap(const HapModuleInfo &hapModuleInfo, const std::string &metadataName,
        std::vector<std::string> &profileInfos) const;
    ErrCode GetSandboxAbilityInfo(const Want &want, int32_t appIndex, int32_t flags, int32_t userId,
        AbilityInfo &abilityInfo);
    ErrCode GetSandboxExtAbilityInfos(const Want &want, int32_t appIndex, int32_t flags, int32_t userId,
        std::vector<ExtensionAbilityInfo> &extensionInfos);
    ErrCode GetSandboxHapModuleInfo(const AbilityInfo &abilityInfo, int32_t appIndex, int32_t userId,
        HapModuleInfo &hapModuleInfo);

private:
    ErrCode Connect();
    void OnDeath();
    bool ConvertResourcePath(const std::string &bundleName, std::string &resPath, bool isCompressed) const;
    bool GetResProfileByMetadata(const std::vector<Metadata> &metadata, const std::string &metadataName,
        const std ::string &resourcePath, bool isCompressed, std::vector<std::string> &profileInfos) const;
#ifdef GLOBAL_RESMGR_ENABLE
    std::shared_ptr<Global::Resource::ResourceManager> InitResMgr(const std::string &resourcePath) const;
    bool GetResFromResMgr(const std::string &resName, const std::shared_ptr<Global::Resource::ResourceManager> &resMgr,
        bool isCompressed, std::vector<std::string> &profileInfos) const;
#endif
    bool IsFileExisted(const std::string &filePath) const;
    bool TransformFileToJsonString(const std::string &resPath, std::string &profile) const;

private:
    std::mutex mutex_;
    sptr<IBundleMgr> bundleMgr_;
    sptr<IBundleInstaller> bundleInstaller_;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_BUNDLE_MGR_CLIENT_H