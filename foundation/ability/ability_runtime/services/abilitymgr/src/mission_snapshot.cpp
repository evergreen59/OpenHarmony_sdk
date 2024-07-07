/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "mission_snapshot.h"

#include "hilog_wrapper.h"
#include "nlohmann/json.hpp"
#include "string_ex.h"

namespace OHOS {
namespace AAFwk {
bool MissionSnapshot::ReadFromParcel(Parcel &parcel)
{
#ifdef SUPPORT_GRAPHICS
    std::unique_ptr<AppExecFwk::ElementName> ability(parcel.ReadParcelable<AppExecFwk::ElementName>());
    if (ability == nullptr) {
        return false;
    }
    topAbility = *ability;
    std::shared_ptr<Media::PixelMap> pixelMap(parcel.ReadParcelable<Media::PixelMap>());
    if (pixelMap == nullptr) {
        return false;
    }
    snapshot = pixelMap;
#endif
    return true;
}

MissionSnapshot *MissionSnapshot::Unmarshalling(Parcel &parcel)
{
    MissionSnapshot *info = new (std::nothrow) MissionSnapshot();

    if (!info->ReadFromParcel(parcel)) {
        delete info;
        info = nullptr;
    }
    return info;
}

bool MissionSnapshot::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteParcelable(&topAbility)) {
        return false;
    }
#ifdef SUPPORT_GRAPHICS
    if (!parcel.WriteParcelable(snapshot.get())) {
        return false;
    }
#endif
    return true;
}
}  // namespace AAFwk
}  // namespace OHOS
