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

#ifndef OHOS_ABILITY_RUNTIME_DUMMY_VALUES_BUCKET_H
#define OHOS_ABILITY_RUNTIME_DUMMY_VALUES_BUCKET_H

#include <string>
#include <unistd.h>

#include "nocopyable.h"
#include "parcel.h"

namespace OHOS {
namespace AppExecFwk {
class ValuesBucket : public Parcelable {
public:
    ValuesBucket() = default;
    ValuesBucket(const std::string &testInf);
    ~ValuesBucket() = default;
    ValuesBucket(const Parcel &parcel) {};

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static ValuesBucket *Unmarshalling(Parcel &parcel);

    bool IsEmpty()
    {
        return false;
    };

    void PutValues(std::shared_ptr<ValuesBucket> &other) {};

private:
    std::string testInf_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_DUMMY_VALUES_BUCKET_H
