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

#ifndef TELEPHONY_IMS_FEATURE_H
#define TELEPHONY_IMS_FEATURE_H

#include <vector>

#include "ims_reg_types.h"

namespace OHOS {
namespace Telephony {
enum ImsCapabilityType {
    CAPABILITY_TYPE_VOICE = 0,
    CAPABILITY_TYPE_VIDEO = 1,
    CAPABILITY_TYPE_UT = 2,
};

struct ImsCapability {
    ImsCapabilityType imsCapabilityType;
    ImsRegTech imsRadioTech;
    bool enable;
};

struct ImsCapabilityList {
    std::vector<ImsCapability> imsCapabilities;
};

enum ImsFeatureIntResult {
    IMS_FEATURE_INT_RESULT_UNKNOWN = -1,
    IMS_FEATURE_INT_VALUE_DISABLED = 0,
    IMS_FEATURE_INT_VALUE_ENABLED = 1
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_IMS_FEATURE_H
