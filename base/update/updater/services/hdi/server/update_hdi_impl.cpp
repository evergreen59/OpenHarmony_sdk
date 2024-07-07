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

#include "update_hdi_impl.h"

namespace Updater {
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif
EXTERNC __attribute__((visibility("default"))) IUpdateInterface *GetInterfaceInstance()
{
    static UpdateHdiImpl impl;
    return &impl;
}

int32_t UpdateHdiImpl::GetLockStatus(bool &status)
{
    status = false;
    return 0;
}
} // updater