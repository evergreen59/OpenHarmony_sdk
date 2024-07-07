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

#ifndef PINAUTH_IINPUTERDATA_H
#define PINAUTH_IINPUTERDATA_H

#include <memory>
#include <vector>
#include "nocopyable.h"

namespace OHOS {
namespace UserIam {
namespace PinAuth {
class IInputerData : public NoCopyable {
public:
    IInputerData() = default;
    ~IInputerData() override = default;
    virtual void OnSetData(int32_t authSubType, std::vector<uint8_t> data) = 0 ;
};
} // namespace PinAuth
} // namespace UserIam
} // namespace OHOS

#endif // PINAUTH_IINPUTERDATA_H
