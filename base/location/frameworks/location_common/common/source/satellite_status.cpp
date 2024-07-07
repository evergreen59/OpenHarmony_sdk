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

#include "satellite_status.h"

#include <parcel.h>

#include "common_utils.h"

namespace OHOS {
namespace Location {
SatelliteStatus::SatelliteStatus()
{
    satellitesNumber_ = 0;
}

SatelliteStatus::SatelliteStatus(SatelliteStatus& satelliteStatus)
{
    satellitesNumber_ = satelliteStatus.GetSatellitesNumber();
    satelliteIds_ = satelliteStatus.GetSatelliteIds();
    carrierToNoiseDensitys_ = satelliteStatus.GetCarrierToNoiseDensitys();
    altitudes_ = satelliteStatus.GetAltitudes();
    azimuths_ = satelliteStatus.GetAzimuths();
    carrierFrequencies_ = satelliteStatus.GetCarrierFrequencies();
}

void SatelliteStatus::ReadFromParcel(Parcel& parcel)
{
    satellitesNumber_ = parcel.ReadInt64();
    for (int i = 0; i < satellitesNumber_; i++) {
        satelliteIds_.push_back(parcel.ReadInt64());
        carrierToNoiseDensitys_.push_back(parcel.ReadDouble());
        altitudes_.push_back(parcel.ReadDouble());
        azimuths_.push_back(parcel.ReadDouble());
        carrierFrequencies_.push_back(parcel.ReadDouble());
    }
}

bool SatelliteStatus::Marshalling(Parcel& parcel) const
{
    CHK_PARCEL_RETURN_VALUE(parcel.WriteInt64(satellitesNumber_));
    for (int i = 0; i < satellitesNumber_; i++) {
        CHK_PARCEL_RETURN_VALUE(parcel.WriteInt64(satelliteIds_[i]));
        CHK_PARCEL_RETURN_VALUE(parcel.WriteDouble(carrierToNoiseDensitys_[i]));
        CHK_PARCEL_RETURN_VALUE(parcel.WriteDouble(altitudes_[i]));
        CHK_PARCEL_RETURN_VALUE(parcel.WriteDouble(azimuths_[i]));
        CHK_PARCEL_RETURN_VALUE(parcel.WriteDouble(carrierFrequencies_[i]));
    }
    return true;
}

std::unique_ptr<SatelliteStatus> SatelliteStatus::Unmarshalling(Parcel& parcel)
{
    std::unique_ptr<SatelliteStatus> status = std::make_unique<SatelliteStatus>();
    status->ReadFromParcel(parcel);
    return status;
}
} // namespace Location
} // namespace OHOS