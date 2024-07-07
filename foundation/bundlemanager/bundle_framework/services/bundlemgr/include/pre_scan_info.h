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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_PRE_SCAN_INFO_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_PRE_SCAN_INFO_H

#include <string>

namespace OHOS {
namespace AppExecFwk {
namespace {
    std::string GetBoolStrVal(bool val)
    {
        return val ? "true" : "false";
    }
}
struct PreBundleConfigInfo {
    std::string bundleName;
    bool keepAlive = false;
    bool singleton = false;
    bool runningResourcesApply = false;
    bool associatedWakeUp = false;
    bool allowUsePrivilegeExtension = false;
    bool allowMultiProcess = false;
    bool formVisibleNotify = false;
    bool allowQueryPriority = false;
    bool allowExcludeFromMissions = false;
    bool userDataClearable = true;
    bool hideDesktopIcon = false;
    std::vector<std::string> allowCommonEvent;
    std::vector<std::string> appSignature;
    std::vector<std::string> existInJsonFile;

    bool operator <(const PreBundleConfigInfo &preBundleConfigInfo) const
    {
        return bundleName < preBundleConfigInfo.bundleName;
    }

    void Reset()
    {
        bundleName.clear();
        keepAlive = false;
        singleton = false;
        runningResourcesApply = false;
        associatedWakeUp = false;
        allowUsePrivilegeExtension = false;
        allowMultiProcess = false;
        formVisibleNotify = false;
        allowQueryPriority = false;
        allowExcludeFromMissions = false;
        userDataClearable = true;
        hideDesktopIcon = false;
        allowCommonEvent.clear();
        appSignature.clear();
        existInJsonFile.clear();
    }

    std::string ToString() const
    {
        return "[ bundleName = " + bundleName
            + ", keepAlive = " + GetBoolStrVal(keepAlive)
            + ", singleton = " + GetBoolStrVal(singleton)
            + ", runningResourcesApply = " + GetBoolStrVal(runningResourcesApply)
            + ", associatedWakeUp = " + GetBoolStrVal(associatedWakeUp)
            + ", allowUsePrivilegeExtension = " + GetBoolStrVal(allowUsePrivilegeExtension)
            + ", allowMultiProcess = " + GetBoolStrVal(allowMultiProcess)
            + ", formVisibleNotify = " + GetBoolStrVal(formVisibleNotify)
            + ", allowQueryPriority = " + GetBoolStrVal(allowQueryPriority)
            + ", allowExcludeFromMissions = " + GetBoolStrVal(allowExcludeFromMissions)
            + ", userDataClearable = " + GetBoolStrVal(userDataClearable)
            + ", hideDesktopIcon = " + GetBoolStrVal(hideDesktopIcon) + "]";
    }
};

struct PreScanInfo {
    std::string bundleDir;
    bool removable = true;
    int32_t priority = 0;

    bool operator < (const PreScanInfo &preScanInfo) const
    {
        if (bundleDir == preScanInfo.bundleDir) {
            return false;
        }

        return priority >= preScanInfo.priority;
    }

    friend bool operator == (const PreScanInfo& oldPreScanInfo, const PreScanInfo& newPreScanInfo)
    {
        return oldPreScanInfo.bundleDir == newPreScanInfo.bundleDir;
    }

    void Reset()
    {
        bundleDir.clear();
        removable = true;
        priority = 0;
    }

    std::string ToString() const
    {
        return "[ bundleDir = " + bundleDir
            + ", removable = " + GetBoolStrVal(removable)
            + ", priority = " + std::to_string(priority) + "]";
    }
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_PRE_SCAN_INFO_H
