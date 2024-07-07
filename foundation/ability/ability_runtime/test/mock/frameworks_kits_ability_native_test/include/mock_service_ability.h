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

#ifndef MOCK_OHOS_ABILITY_RUNTIME_MOCK_SERVICE_ABILITY_H
#define MOCK_OHOS_ABILITY_RUNTIME_MOCK_SERVICE_ABILITY_H

#include "ability.h"
#include <gtest/gtest.h>

namespace OHOS {
namespace AppExecFwk {
using Want = OHOS::AAFwk::Want;

class MockServiceAbility : public Ability {
public:
    MockServiceAbility() = default;
    virtual ~MockServiceAbility() = default;

    void OnKeyDown(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
    {
        GTEST_LOG_(INFO) << "MockServiceAbility::OnKeyDown called";
    }

    void OnKeyUp(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
    {
        GTEST_LOG_(INFO) << "MockServiceAbility::OnKeyUp called";
    }

    void OnPointerEvent(std::shared_ptr<MMI::PointerEvent>& pointerEvent)
    {
        GTEST_LOG_(INFO) << "MockServiceAbility::OnTouchEvent called";
    }

    void OnAbilityResult(int requestCode, int resultCode, const AAFwk::Want &resultData)
    {
        GTEST_LOG_(INFO) << "MockServiceAbility::OnAbilityResult called";
    }

    void OnNewWant(const Want &want)
    {
        GTEST_LOG_(INFO) << "MockServiceAbility::OnNewWant called";
    }

    void OnBackground()
    {
        GTEST_LOG_(INFO) << "MockServiceAbility::OnBackground called";
    }

    void OnStop()
    {
        GTEST_LOG_(INFO) << "MockServiceAbility::OnStop called";
    }

    void OnStart(const Want &want)
    {
        GTEST_LOG_(INFO) << "MockServiceAbility::OnStart called";
    }
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // MOCK_OHOS_ABILITY_RUNTIME_MOCK_SERVICE_ABILITY_H
