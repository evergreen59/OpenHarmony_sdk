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
#ifndef FUSION_CONTROLLER_FUZZER_H
#define FUSION_CONTROLLER_FUZZER_H

#include "fusion_controller.h"

namespace OHOS {
namespace Location {
bool FusionControllerFuzzerTest(const uint8_t* data, size_t size);
} // namespace Location
} // namespace OHOS
#endif // FUSION_CONTROLLER_FUZZER_H