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

#ifndef MOCK_OHOS_EDM_MOCK_BUNDLE_MANAGER_H
#define MOCK_OHOS_EDM_MOCK_BUNDLE_MANAGER_H

#include <vector>
#include "gmock/gmock.h"
#include "bundle_info.h"
#include "want.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"
#include "bundle_mgr_interface.h"

namespace OHOS {
namespace AppExecFwk {
class BundleMgrProxy : public IRemoteProxy<IBundleMgr> {
public:
    explicit BundleMgrProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IBundleMgr>(impl) {}
    virtual ~BundleMgrProxy() {}

    bool QueryExtensionAbilityInfos(const Want &want, const ExtensionAbilityType &extensionType,
        const int32_t &flag, const int32_t &userId, std::vector<ExtensionAbilityInfo> &extensionInfos) override;

    ErrCode GetNameForUid(const int uid, std::string &name) override;

    bool GetBundleInfo(const std::string &bundleName, const BundleFlag flag,
        BundleInfo &bundleInfo, int32_t userId) override;
private:
    std::string bundleNameMock_;
};

class BundleMgrStub : public IRemoteStub<IBundleMgr> {
public:
    int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
};

class BundleMgrService : public BundleMgrStub {
public:
    BundleMgrService();
    virtual ~BundleMgrService() {}

    bool QueryExtensionAbilityInfos(const Want &want, const ExtensionAbilityType &extensionType,
        const int32_t &flag, const int32_t &userId, std::vector<ExtensionAbilityInfo> &extensionInfos) override;

    ErrCode GetNameForUid(const int uid, std::string &name) override;

    bool GetBundleInfo(const std::string &bundleName, const BundleFlag flag,
        BundleInfo &bundleInfo, int32_t userId) override;
private:
    std::string bundleNameMock_;
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // MOCK_OHOS_EDM_MOCK_BUNDLE_MANAGER_H
