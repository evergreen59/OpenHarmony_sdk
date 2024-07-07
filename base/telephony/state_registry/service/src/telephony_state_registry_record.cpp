/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "telephony_state_registry_record.h"

#include "telephony_permission.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
bool TelephonyStateRegistryRecord::IsCanReadCallHistory()
{
    if (!TelephonyPermission::CheckPermission(Permission::READ_CALL_LOG)) {
        TELEPHONY_LOGE("Check permission failed,"
            " you must declare ohos.permission.READ_CALL_LOG permission for number");
        return false;
    }
    return true;
}

bool TelephonyStateRegistryRecord::IsExistStateListener(uint32_t mask) const
{
    return (telephonyObserver_ != nullptr) && ((mask_ & mask) != 0);
}
} // namespace Telephony
} // namespace OHOS
