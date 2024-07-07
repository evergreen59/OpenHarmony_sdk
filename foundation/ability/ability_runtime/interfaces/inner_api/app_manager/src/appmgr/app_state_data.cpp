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

#include "app_state_data.h"

#include "hilog_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
bool AppStateData::Marshalling(Parcel &parcel) const
{
    return (parcel.WriteString(bundleName) && parcel.WriteInt32(uid) && parcel.WriteInt32(state)
        && parcel.WriteInt32(pid) && parcel.WriteInt32(accessTokenId) && parcel.WriteBool(isFocused));
}

bool AppStateData::ReadFromParcel(Parcel &parcel)
{
    bundleName = parcel.ReadString();
    uid = parcel.ReadInt32();
    state = parcel.ReadInt32();
    pid = parcel.ReadInt32();
    accessTokenId = parcel.ReadInt32();
    isFocused = parcel.ReadBool();

    return true;
}

AppStateData *AppStateData::Unmarshalling(Parcel &parcel)
{
    AppStateData *appStateData = new (std::nothrow) AppStateData();
    if (appStateData && !appStateData->ReadFromParcel(parcel)) {
        HILOG_WARN("appStateData failed, because ReadFromParcel failed");
        delete appStateData;
        appStateData = nullptr;
    }
    return appStateData;
}
}  // namespace AppExecFwk
}  // namespace OHOS
