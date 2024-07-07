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

#ifndef INPUTER_GET_DATA_SERVICE_H
#define INPUTER_GET_DATA_SERVICE_H

#include <vector>

#include "i_inputer.h"
#include "inputer_get_data_stub.h"

namespace OHOS {
namespace UserIam {
namespace PinAuth {
class InputerGetDataService : public InputerGetDataStub {
public:
    explicit InputerGetDataService(const std::shared_ptr<IInputer> &inputer);
    ~InputerGetDataService() override = default;
    void OnGetData(int32_t authSubType, const std::vector<uint8_t> &salt,
        const sptr<InputerSetData> &inputerSetData) override;

private:
    std::shared_ptr<IInputer> inputer_ {nullptr};
};
} // namespace PinAuth
} // namespace UserIam
} // namespace OHOS
#endif // INPUTER_GET_DATA_SERVICE_H
