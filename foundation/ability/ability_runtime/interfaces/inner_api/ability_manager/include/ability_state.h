/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ABILITY_RUNTIME_ABILITY_STATE_H
#define OHOS_ABILITY_RUNTIME_ABILITY_STATE_H

#include <string>

#include "parcel.h"

namespace OHOS {
namespace AAFwk {
/**
 * @enum AbilityState
 * AbilityState defines the life cycle state of ability.
 */
enum AbilityState {
    INITIAL = 0,
    INACTIVE,
    ACTIVE,
    INACTIVATING = 5,
    ACTIVATING,
    TERMINATING = 8,

    /**
     * State for api > 7.
     */
    FOREGROUND,
    BACKGROUND,
    FOREGROUNDING,
    BACKGROUNDING,
    FOREGROUND_FAILED,
    FOREGROUND_INVALID_MODE,
    FOREGROUND_WINDOW_FREEZED,
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_ABILITY_STATE_H
