/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef VOLTAGE_ACTION_H
#define VOLTAGE_ACTION_H

#include <cstdint>
#include "ithermal_action.h"

namespace OHOS {
namespace PowerMgr {
class VoltageAction : public IThermalAction {
public:
    VoltageAction() = default;
    virtual ~VoltageAction() = default;
    bool AddActionValue(uint32_t value) override;
    void Execute() override;
    int32_t BatteryVoltageActionRequest(uint32_t voltage);
private:
    uint32_t latestvalue_;
};
} // namespace PowerMgr
} // namespace OHOS
#endif // VOLTAGE_ACTION_H