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

#ifndef INPUTER_DATA_IMPL_H
#define INPUTER_DATA_IMPL_H

#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

#include "refbase.h"

#include "i_inputer_data.h"
#include "inputer_set_data.h"

namespace OHOS {
namespace UserIam {
namespace PinAuth {
class InputerDataImpl : public IInputerData {
public:
    explicit InputerDataImpl(const std::vector<uint8_t> &salt, const sptr<InputerSetData> &inputerSetData);
    ~InputerDataImpl() override = default;
    void OnSetData(int32_t authSubType, std::vector<uint8_t> data) override;

private:
    std::vector<uint8_t> salt_;
    sptr<InputerSetData> inputerSetData_ {nullptr};
};
} // namespace PinAuth
} // namespace UserIam
} // namespace OHOS
#endif // INPUTER_DATA_IMPL_H
