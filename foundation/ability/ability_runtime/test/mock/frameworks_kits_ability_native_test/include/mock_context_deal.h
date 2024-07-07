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
#ifndef MOCK_OHOS_ABILITY_RUNTIME_MOCK_CONTEXT_DEAL_H
#define MOCK_OHOS_ABILITY_RUNTIME_MOCK_CONTEXT_DEAL_H

#include "gmock/gmock.h"
#include "context_deal.h"

namespace OHOS {
namespace AppExecFwk {
class MockContextDeal : public ContextDeal {
public:
    MockContextDeal() = default;
    virtual ~MockContextDeal() = default;
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif /* MOCK_OHOS_ABILITY_RUNTIME_MOCK_CONTEXT_DEAL_H */
