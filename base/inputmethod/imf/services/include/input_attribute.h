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

#ifndef SERVICES_INCLUDE_INPUT_ATTRIBUTE_H
#define SERVICES_INCLUDE_INPUT_ATTRIBUTE_H

#include <cstdint>

#include "message_parcel.h"

namespace OHOS {
namespace MiscServices {
struct InputAttribute {
    static const int32_t PATTERN_TEXT = 0x00000001;
    static const int32_t PATTERN_PASSWORD = 0x00000007;
    int32_t inputPattern = 0;
    int32_t enterKeyType = 0;
    int32_t inputOption = 0;

    static bool Marshalling(const InputAttribute &in, MessageParcel &data)
    {
        return data.WriteInt32(in.inputPattern) && data.WriteInt32(in.enterKeyType) && data.WriteInt32(in.inputOption);
    }

    static bool Unmarshalling(InputAttribute &out, MessageParcel &data)
    {
        return data.ReadInt32(out.inputPattern) && data.ReadInt32(out.enterKeyType) && data.ReadInt32(out.inputOption);
    }

    bool GetSecurityFlag()
    {
        return inputPattern == PATTERN_PASSWORD;
    }
};
} // namespace MiscServices
} // namespace OHOS

#endif // SERVICES_INCLUDE_INPUT_ATTRIBUTE_H
