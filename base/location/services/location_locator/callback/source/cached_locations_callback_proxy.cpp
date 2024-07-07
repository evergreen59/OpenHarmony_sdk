/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "cached_locations_callback_proxy.h"

#include "message_option.h"

#include "location_log.h"

namespace OHOS {
namespace Location {
CachedLocationsCallbackProxy::CachedLocationsCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<ICachedLocationsCallback>(impl)
{
}

void CachedLocationsCallbackProxy::OnCacheLocationsReport(const std::vector<std::unique_ptr<Location>>& locations)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return;
    }
    for (unsigned int i = 0; i < locations.size(); i++) {
        locations[i]->Marshalling(data);
    }
    MessageOption option = { MessageOption::TF_ASYNC };
    int error = Remote()->SendRequest(RECEIVE_CACHED_LOCATIONS_EVENT, data, reply, option);
    LBSLOGI(CACHED_LOCATIONS_CALLBACK,
        "CachedLocationsCallbackProxy::OnCacheLocationsReport Transact ErrCode = %{public}d", error);
}
} // namespace Location
} // namespace OHOS
