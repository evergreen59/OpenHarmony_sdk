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

#include "app_state_data.h"

namespace OHOS {
namespace Security {
namespace AccessToken {
bool AppStateData::Marshalling(Parcel &parcel) const
{
    return (parcel.WriteString(bundleName) && parcel.WriteInt32(uid) && parcel.WriteInt32(state)
        && parcel.WriteInt32(pid) && parcel.WriteInt32(accessTokenId) && parcel.WriteBool(isFocused));
}

AppStateData *AppStateData::Unmarshalling(Parcel &parcel)
{
    AppStateData *appStateData = new (std::nothrow) AppStateData();
    if (appStateData == nullptr) {
        return nullptr;
    }
    appStateData->bundleName = parcel.ReadString();
    appStateData->uid = parcel.ReadInt32();
    appStateData->state = parcel.ReadInt32();
    appStateData->pid = parcel.ReadInt32();
    appStateData->accessTokenId = parcel.ReadInt32();
    appStateData->isFocused = parcel.ReadBool();
    return appStateData;
}
}  // namespace AccessToken
}  // namespace Security
}  // namespace OHOS
