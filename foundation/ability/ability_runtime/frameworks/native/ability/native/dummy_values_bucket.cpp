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

#include "hilog_wrapper.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
ValuesBucket::ValuesBucket(const std::string &testInf) : testInf_(testInf)
{}

/**
 * @brief read this Sequenceable object from a Parcel.
 *
 * @param inParcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
 * @return Returns true if read succeeded; returns false otherwise.
 */
bool ValuesBucket::ReadFromParcel(Parcel &parcel)
{
    testInf_ = Str16ToStr8(parcel.ReadString16());
    return true;
}

/**
 * @brief Unmarshals this Sequenceable object from a Parcel.
 *
 * @param inParcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
 */
ValuesBucket *ValuesBucket::Unmarshalling(Parcel &parcel)
{
    ValuesBucket *valuesBucket = new (std::nothrow) ValuesBucket();
    if (valuesBucket && !valuesBucket->ReadFromParcel(parcel)) {
        HILOG_ERROR("ValuesBucket::Unmarshalling ReadFromParcel failed");
        delete valuesBucket;
        valuesBucket = nullptr;
    }
    return valuesBucket;
}

/**
 * @brief Marshals this Sequenceable object into a Parcel.
 *
 * @param outParcel Indicates the Parcel object to which the Sequenceable object will be marshaled.
 */
bool ValuesBucket::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString16(Str8ToStr16(testInf_))) {
        HILOG_ERROR("valuesBucket::Marshalling WriteString16 failed");
        return false;
    }
    return true;
}
}  // namespace AppExecFwk
}  // namespace OHOS
