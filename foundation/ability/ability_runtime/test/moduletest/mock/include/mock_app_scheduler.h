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

#ifndef MODULETEST_OHOS_ABILITY_RUNTIME_MOCK_APP_SCHEDULER_H
#define MODULETEST_OHOS_ABILITY_RUNTIME_MOCK_APP_SCHEDULER_H

#include "gmock/gmock.h"
#include "refbase.h"
#include "iremote_object.h"
#include "app_scheduler.h"
#include "app_launch_data.h"
#include "configuration.h"

namespace OHOS {
namespace AAFwk {
class MockAppScheduler : public AppScheduler {
public:
    MockAppScheduler() = default;
    virtual ~MockAppScheduler() = default;

    MOCK_METHOD0(ScheduleForegroundApplication, void());
    MOCK_METHOD0(ScheduleBackgroundApplication, void());
    MOCK_METHOD0(ScheduleTerminateApplication, void());
    MOCK_METHOD2(ScheduleLaunchApplication, void(const AppExecFwk::AppLaunchData &, const Configuration &config));
    MOCK_METHOD3(ScheduleLaunchAbility, void(const AppExecFwk::AbilityInfo &, const sptr<IRemoteObject> &,
        const std::shared_ptr<AAFwk::Want> &));
    MOCK_METHOD1(ScheduleCleanAbility, void(const sptr<IRemoteObject> &));
    MOCK_METHOD1(ScheduleProfileChanged, void(const AppExecFwk::Profile &));
    MOCK_METHOD1(ScheduleConfigurationUpdated, void(const AppExecFwk::Configuration &config));
    MOCK_METHOD1(ScheduleShrinkMemory, void(const int));
    MOCK_METHOD1(ScheduleMemoryLevel, void(int32_t level));
    MOCK_METHOD0(ScheduleLowMemory, void());
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // MODULETEST_OHOS_ABILITY_RUNTIME_MOCK_APP_SCHEDULER_H
