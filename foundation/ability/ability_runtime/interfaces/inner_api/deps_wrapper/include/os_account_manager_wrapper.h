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

#ifndef OHOS_ABILITY_RUNTIME_OS_ACCOUNT_MANAGER_WRAPPER_H
#define OHOS_ABILITY_RUNTIME_OS_ACCOUNT_MANAGER_WRAPPER_H

#include <vector>

#include "errors.h"
#include "singleton.h"

namespace OHOS {
namespace AppExecFwk {
class OsAccountManagerWrapper : public DelayedSingleton<OsAccountManagerWrapper> {
public:
    OsAccountManagerWrapper() {}
    virtual ~OsAccountManagerWrapper() {}

    ErrCode QueryActiveOsAccountIds(std::vector<int32_t>& ids);

    ErrCode GetOsAccountLocalIdFromUid(const int32_t uid, int32_t &id);

    ErrCode GetOsAccountLocalIdFromProcess(int &id);

    ErrCode IsOsAccountExists(const int id, bool &isOsAccountExists);

    ErrCode CreateOsAccount(const std::string &name, int32_t &osAccountUserId);

    ErrCode RemoveOsAccount(const int id);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_OS_ACCOUNT_MANAGER_WRAPPER_H