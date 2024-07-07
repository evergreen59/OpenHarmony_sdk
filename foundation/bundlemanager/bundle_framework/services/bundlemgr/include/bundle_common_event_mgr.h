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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_COMMON_EVENT_MGR_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_COMMON_EVENT_MGR_H

#include <functional>
#include <unordered_map>

#include "appexecfwk_errors.h"
#include "bundle_data_mgr.h"
#include "inner_bundle_info.h"

namespace OHOS {
namespace AppExecFwk {
enum class NotifyType {
    INSTALL = 1,
    UPDATE,
    UNINSTALL_BUNDLE,
    UNINSTALL_MODULE,
    ABILITY_ENABLE,
    APPLICATION_ENABLE,
    BUNDLE_DATA_CLEARED,
    BUNDLE_CACHE_CLEARED,
};

enum class SandboxInstallType : uint32_t {
    INSTALL = 0,
    UNINSTALL,
};

struct NotifyBundleEvents {
    std::string bundleName = "";
    std::string modulePackage = "";
    std::string abilityName = "";
    ErrCode resultCode = ERR_OK;
    NotifyType type = NotifyType::INSTALL;
    int32_t uid = 0;
    uint32_t accessTokenId = 0;
    bool isAgingUninstall = false;
};

class BundleCommonEventMgr {
public:
    BundleCommonEventMgr();
    virtual ~BundleCommonEventMgr() = default;
    void NotifyBundleStatus(const NotifyBundleEvents &installResult,
        const std::shared_ptr<BundleDataMgr> &dataMgr);
    ErrCode NotifySandboxAppStatus(const InnerBundleInfo &info, int32_t uid, int32_t userId,
        const SandboxInstallType &type);

private:
    std::string GetCommonEventData(const NotifyType &type);
    void Init();

    std::unordered_map<NotifyType, std::string> commonEventMap_;
};
} // AppExecFwk
} // OHOS
#endif // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_COMMON_EVENT_MGR_H