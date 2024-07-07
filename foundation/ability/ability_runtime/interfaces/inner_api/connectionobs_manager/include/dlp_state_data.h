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

#ifndef ABILITY_RUNTIME_DLP_STATE_DATA_H
#define ABILITY_RUNTIME_DLP_STATE_DATA_H

#include "parcel.h"

namespace OHOS {
namespace AbilityRuntime {
struct DlpStateData : public Parcelable {
    /**
     * @brief read this Sequenceable object from a Parcel.
     *
     * @param parcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
     * @return Returns true if read successed; returns false otherwise.
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Marshals this Sequenceable object into a Parcel.
     *
     * @param parcel Indicates the Parcel object to which the Sequenceable object will be marshaled.
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Unmarshals this Sequenceable object from a Parcel.
     *
     * @param parcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
     */
    static DlpStateData *Unmarshalling(Parcel &parcel);

    int32_t targetPid = 0;
    int32_t targetUid = 0;
    std::string targetBundleName;
    std::string targetModuleName;
    std::string targetAbilityName;
    int32_t callerUid = 0;
    int32_t callerPid = 0;
    std::string callerName;
};
}  // namespace AbilityRuntime
}  // namespace OHOS
#endif  // ABILITY_RUNTIME_DLP_STATE_DATA_H
