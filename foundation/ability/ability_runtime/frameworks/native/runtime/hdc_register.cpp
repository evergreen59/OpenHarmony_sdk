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

#include "hdc_register.h"

#include <dlfcn.h>
#include <unistd.h>

#include "hilog_wrapper.h"

namespace OHOS::AbilityRuntime {
using StartRegister = void (*)(const std::string& pkgName);
using StopRegister = void (*)();

HdcRegister::~HdcRegister()
{
    StopHdcRegister();
}

HdcRegister& HdcRegister::Get()
{
    static HdcRegister hdcRegister;
    return hdcRegister;
}

void HdcRegister::StartHdcRegister(const std::string& bundleName)
{
    HILOG_DEBUG("HdcRegister::StartHdcRegister begin");

    registerHandler_ = dlopen("libhdc_register.z.so", RTLD_LAZY);
    if (registerHandler_ == nullptr) {
        HILOG_ERROR("HdcRegister::StartHdcRegister failed to open register library");
        return;
    }
    auto startRegister = reinterpret_cast<StartRegister>(dlsym(registerHandler_, "StartConnect"));
    if (startRegister == nullptr) {
        HILOG_ERROR("HdcRegister::StartHdcRegister failed to find symbol 'StartConnect'");
        return;
    }
    startRegister(bundleName);
    HILOG_DEBUG("HdcRegister::StartHdcRegister end");
}

void HdcRegister::StopHdcRegister()
{
    HILOG_DEBUG("HdcRegister::StopHdcRegister begin");
    if (registerHandler_ == nullptr) {
        HILOG_ERROR("HdcRegister::StopHdcRegister registerHandler_ is nullptr");
        return;
    }
    auto stopRegister = reinterpret_cast<StopRegister>(dlsym(registerHandler_, "StopConnect"));
    if (stopRegister != nullptr) {
        stopRegister();
    } else {
        HILOG_ERROR("HdcRegister::StopHdcRegister failed to find symbol 'StopConnect'");
    }
    dlclose(registerHandler_);
    registerHandler_ = nullptr;
    HILOG_DEBUG("HdcRegister::StopHdcRegister end");
}
} // namespace OHOS::AbilityRuntime
