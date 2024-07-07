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

#ifndef OHOS_ABILITY_RUNTIME_URI_PERMISSION_MANAGER_STUB_IMPL_H
#define OHOS_ABILITY_RUNTIME_URI_PERMISSION_MANAGER_STUB_IMPL_H

#include <functional>
#include <map>

#include "bundlemgr/bundle_mgr_interface.h"
#include "foundation/filemanagement/storage_service/services/storage_manager/include/ipc/storage_manager_proxy.h"
#include "istorage_manager.h"
#include "uri.h"
#include "uri_permission_manager_stub.h"

namespace OHOS {
namespace AAFwk {
using ClearProxyCallback = std::function<void(const wptr<IRemoteObject>&)>;

struct GrantInfo {
    unsigned int flag;
    const unsigned int fromTokenId;
    const unsigned int targetTokenId;
};
class UriPermissionManagerStubImpl : public UriPermissionManagerStub,
                                     public std::enable_shared_from_this<UriPermissionManagerStubImpl> {
public:
    UriPermissionManagerStubImpl() = default;
    virtual ~UriPermissionManagerStubImpl() = default;

    bool GrantUriPermission(const Uri &uri, unsigned int flag, const Security::AccessToken::AccessTokenID fromTokenId,
        const Security::AccessToken::AccessTokenID targetTokenId) override;

    bool VerifyUriPermission(const Uri &uri, unsigned int flag,
        const Security::AccessToken::AccessTokenID tokenId) override;

    void RemoveUriPermission(const Security::AccessToken::AccessTokenID tokenId) override;

private:
    sptr<AppExecFwk::IBundleMgr> ConnectBundleManager();
    sptr<StorageManager::IStorageManager> ConnectStorageManager();
    int GetCurrentAccountId();
    void ClearBMSProxy();
    void ClearSMProxy();

    class BMSOrSMDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        BMSOrSMDeathRecipient(const ClearProxyCallback &proxy) : proxy_(proxy) {}
        ~BMSOrSMDeathRecipient() = default;
        virtual void OnRemoteDied([[maybe_unused]] const wptr<IRemoteObject>& remote) override;

    private:
        ClearProxyCallback proxy_;
    };

private:
    std::map<std::string, std::list<GrantInfo>> uriMap_;
    std::mutex mutex_;
    std::mutex bmsMutex_;
    std::mutex storageMutex_;
    sptr<AppExecFwk::IBundleMgr> bundleManager_ = nullptr;
    sptr<StorageManager::IStorageManager> storageManager_ = nullptr;
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_URI_PERMISSION_MANAGER_STUB_IMPL_H
