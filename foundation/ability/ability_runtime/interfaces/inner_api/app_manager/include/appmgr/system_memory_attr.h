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

#ifndef OHOS_ABILITY_RUNTIME_SYSTEM_MEMORY_ATTR_H
#define OHOS_ABILITY_RUNTIME_SYSTEM_MEMORY_ATTR_H

#include "parcel.h"

namespace OHOS {
namespace AppExecFwk {
/**
 * @class SystemMemoryAttr
 * SystemMemoryAttr is used to query system memory info.
 */
class SystemMemoryAttr : public Parcelable {
public:
    SystemMemoryAttr();
    virtual ~SystemMemoryAttr() = default;
    /**
     * @default The available memory on the system
     */
    int64_t availSysMem_;

    /**
     * @default The total memory on the system
     */
    int64_t totalSysMem_;

    /**
     * @default The low memory threshold under which the system
     * will kill background processes
     */
    int64_t threshold_;

    /**
     * @default Whether the system is in low memory status
     */
    bool isSysInlowMem_;

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static SystemMemoryAttr *Unmarshalling(Parcel &parcel);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_SYSTEM_MEMORY_ATTR_H
