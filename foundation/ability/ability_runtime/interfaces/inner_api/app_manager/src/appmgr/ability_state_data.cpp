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

#include "ability_state_data.h"

#include "hilog_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
bool AbilityStateData::Marshalling(Parcel &parcel) const
{
    return (parcel.WriteString(moduleName) && parcel.WriteString(bundleName) &&
        parcel.WriteString(abilityName) && parcel.WriteInt32(abilityState) &&
        parcel.WriteInt32(pid) && parcel.WriteInt32(uid) &&
        (static_cast<MessageParcel*>(&parcel))->WriteRemoteObject(token) &&
        parcel.WriteInt32(abilityType) && parcel.WriteBool(isFocused));
}

bool AbilityStateData::ReadFromParcel(Parcel &parcel)
{
    moduleName = parcel.ReadString();

    bundleName = parcel.ReadString();

    abilityName = parcel.ReadString();

    abilityState = parcel.ReadInt32();

    pid = parcel.ReadInt32();

    uid = parcel.ReadInt32();

    token = (static_cast<MessageParcel*>(&parcel))->ReadRemoteObject();

    abilityType = parcel.ReadInt32();

    isFocused = parcel.ReadBool();
    return true;
}

AbilityStateData *AbilityStateData::Unmarshalling(Parcel &parcel)
{
    AbilityStateData *abilityStateData = new (std::nothrow) AbilityStateData();
    if (abilityStateData && !abilityStateData->ReadFromParcel(parcel)) {
        HILOG_WARN("AbilityStateData failed, because ReadFromParcel failed");
        delete abilityStateData;
        abilityStateData = nullptr;
    }
    return abilityStateData;
}
}  // namespace AppExecFwk
}  // namespace OHOS
