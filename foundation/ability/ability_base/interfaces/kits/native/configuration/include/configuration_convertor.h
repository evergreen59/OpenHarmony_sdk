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

#ifndef OHOS_ABILITY_BASE_CONFIGURATION_CONVERTOR_H
#define OHOS_ABILITY_BASE_CONFIGURATION_CONVERTOR_H

#include <string>
#include "res_common.h"

namespace OHOS::AppExecFwk {
Global::Resource::ColorMode ConvertColorMode(std::string colormode);

Global::Resource::Direction ConvertDirection(int32_t height, int32_t width);
Global::Resource::Direction ConvertDirection(std::string direction);

Global::Resource::ScreenDensity ConvertDensity(float density);
Global::Resource::ScreenDensity ConvertDensity(std::string density);

Global::Resource::InputDevice ConvertHasPointerDevice(std::string hasPointerDevice);

Global::Resource::DeviceType ConvertDeviceType(std::string deviceType);

int32_t ConvertDisplayId(std::string displayId);

std::string GetColorModeStr(int32_t colormode);

std::string GetDirectionStr(int32_t height, int32_t width);

std::string GetDensityStr(float density);
} // namespace OHOS::AppExecFwk
#endif  // OHOS_ABILITY_BASE_CONFIGURATION_CONVERTOR_H