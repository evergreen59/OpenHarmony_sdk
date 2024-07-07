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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_QUICK_FIX_DEPLOYER_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_QUICK_FIX_DEPLOYER_H

#include "inner_bundle_info.h"
#include "quick_fix_checker.h"
#include "quick_fix_data_mgr.h"
#include "quick_fix_interface.h"
#include "quick_fix/quick_fix_status_callback_interface.h"

namespace OHOS {
namespace AppExecFwk {
class QuickFixDeployer final : public IQuickFix {
public:
    explicit QuickFixDeployer(const std::vector<std::string> &bundleFilePaths);

    virtual ~QuickFixDeployer() = default;

    virtual ErrCode Execute() override;

    DeployQuickFixResult GetDeployQuickFixResult() const;

    ErrCode ToDeployEndStatus(InnerAppQuickFix &newInnerAppQuickFix,
        const InnerAppQuickFix &oldInnerAppQuickFix);

private:
    ErrCode DeployQuickFix();

    ErrCode GetQuickFixDataMgr();

    ErrCode SaveToInnerBundleInfo(const InnerAppQuickFix &newInnerAppQuickFix);

    ErrCode ToDeployStartStatus(const std::vector<std::string> &bundleFilePaths,
        InnerAppQuickFix &newInnerAppQuickFix, InnerAppQuickFix &oldInnerAppQuickFix);

    ErrCode ParseAndCheckAppQuickFixInfos(
        const std::vector<std::string> &bundleFilePaths,
        std::unordered_map<std::string, AppQuickFix> &infos);

    ErrCode ToInnerAppQuickFix(const std::unordered_map<std::string, AppQuickFix> infos,
        const InnerAppQuickFix &oldInnerAppQuickFix, InnerAppQuickFix &newInnerAppQuickFix);

    ErrCode GetBundleInfo(const std::string &bundleName, BundleInfo &bundleInfo);

    ErrCode ProcessPatchDeployStart(
        const std::vector<std::string> bundleFilePaths,
        const BundleInfo &bundleInfo,
        std::unordered_map<std::string, AppQuickFix> &infos);

    ErrCode ProcessHotReloadDeployStart(const BundleInfo &bundleInfo, const AppQuickFix &appQuickFix);

    ErrCode ProcessPatchDeployEnd(const AppQuickFix &appQuickFix, std::string &patchPath);

    ErrCode ProcessHotReloadDeployEnd(const AppQuickFix &appQuickFix, std::string &patchPath);

    ErrCode CheckPatchVersionCode(
        const AppQuickFix &newAppQuickFix,
        const AppQuickFix &oldAppQuickFix);

    ErrCode SaveAppQuickFix(const InnerAppQuickFix &innerAppQuickFix);

    ErrCode MoveHqfFiles(InnerAppQuickFix &innerAppQuickFix, const std::string &targetPath);

    ErrCode ProcessBundleFilePaths(const std::vector<std::string> &bundleFilePaths,
        std::vector<std::string> &realFilePaths);

    void ToDeployQuickFixResult(const AppQuickFix &appQuickFix);

    void ProcessNativeLibraryPath(const std::string &patchPath, InnerAppQuickFix &innerAppQuickFix);

    void ProcessNativeLibraryPath(
        const std::string &patchPath, const InnerAppQuickFix &innerAppQuickFix, std::string &nativeLibraryPath);

    void ResetNativeSoAttrs(std::unordered_map<std::string, AppQuickFix> &infos);

    void ResetNativeSoAttrs(AppQuickFix &appQuickFix);

    bool IsLibIsolated(const std::string &bundleName, const std::string &moduleName);

    bool FetchInnerBundleInfo(const std::string &bundleName, InnerBundleInfo &innerBundleInfo);

    bool FetchPatchNativeSoAttrs(const AppqfInfo &appqfInfo,
        const HqfInfo hqfInfo, bool isLibIsolated, std::string &nativeLibraryPath, std::string &cpuAbi);

    bool HasNativeSoInBundle(const AppQuickFix &appQuickFix);

    std::vector<std::string> patchPaths_;
    std::shared_ptr<QuickFixDataMgr> quickFixDataMgr_ = nullptr;
    DeployQuickFixResult deployQuickFixResult_;
};
} // AppExecFwk
} // OHOS
#endif // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_QUICK_FIX_DEPLOYER_H