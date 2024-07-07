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
#ifndef UNITTEST_OHOS_ABILITY_RUNTIME_MOCK_DATA_ABILITY_OBSERVER_STUB_H
#define UNITTEST_OHOS_ABILITY_RUNTIME_MOCK_DATA_ABILITY_OBSERVER_STUB_H
#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "data_ability_observer_stub.h"

namespace OHOS {
namespace AAFwk {
class MockDataAbilityObserverStub : public AAFwk::DataAbilityObserverStub {
public:
    MockDataAbilityObserverStub() = default;
    virtual ~MockDataAbilityObserverStub() = default;
    MOCK_METHOD0(OnChange, void(void));
};
}  // namespace AAFwk
}  // namespace OHOS
#endif /* UNITTEST_OHOS_ABILITY_RUNTIME_MOCK_DATA_ABILITY_OBSERVER_STUB_H */
