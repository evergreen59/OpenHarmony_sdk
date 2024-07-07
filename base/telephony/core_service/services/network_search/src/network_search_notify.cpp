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

#include "network_search_notify.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
NetworkSearchNotify::NetworkSearchNotify() {}
NetworkSearchNotify::~NetworkSearchNotify() {}

void NetworkSearchNotify::NotifyNetworkStateUpdated(int32_t slotId, const sptr<NetworkState> &networkState)
{
    if (networkState == nullptr) {
        TELEPHONY_LOGE("NotifyNetworkStateUpdated networkState is nullptr");
        return;
    }
    int32_t result =
        DelayedRefSingleton<TelephonyStateRegistryClient>::GetInstance().UpdateNetworkState(slotId, networkState);
    TELEPHONY_LOGD("NotifyNetworkStateUpdated ret %{public}s", networkState->ToString().c_str());
    if (result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("NotifyNetworkStateUpdated TELEPHONY_STATE_REGISTRY_SYS_ABILITY_ID not found");
    }
}

void NetworkSearchNotify::NotifySignalInfoUpdated(
    int32_t slotId, const std::vector<sptr<SignalInformation>> &signalInfos)
{
    TELEPHONY_LOGD("NotifySignalInfoUpdated~~~ signalInfos size=%{public}zu", signalInfos.size());
    int32_t result =
        DelayedRefSingleton<TelephonyStateRegistryClient>::GetInstance().UpdateSignalInfo(slotId, signalInfos);
    TELEPHONY_LOGD("NotifySignalInfoUpdated ret %{public}d", result);
    if (result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("NotifySignalInfoUpdated TELEPHONY_STATE_REGISTRY_SYS_ABILITY_ID not found");
    }
}

void NetworkSearchNotify::NotifyCellInfoUpdated(int32_t slotId, const std::vector<sptr<CellInformation>> &cellInfos)
{
    TELEPHONY_LOGD("NotifyCellInfoUpdated~~~ cell size=%{public}zu", cellInfos.size());
    int32_t result =
        DelayedRefSingleton<TelephonyStateRegistryClient>::GetInstance().UpdateCellInfo(slotId, cellInfos);
    TELEPHONY_LOGD("NotifyCellInfoUpdated ret %{public}d", result);
    if (result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("NotifyCellInfoUpdated TELEPHONY_STATE_REGISTRY_SYS_ABILITY_ID not found.");
    }
}
} // namespace Telephony
} // namespace OHOS
