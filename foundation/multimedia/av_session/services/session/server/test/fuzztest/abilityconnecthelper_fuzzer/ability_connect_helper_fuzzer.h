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

#ifndef OHOS_AVCONTROLLERCALLITEM_FUZZER_H
#define OHOS_AVCONTROLLERCALLITEM_FUZZER_H

#include <mutex>
#include <set>
#include "avsession_info.h"
#include "background_audio_controller.h"
#include "audio_adapter.h"
#include "app_manager_adapter.h"
#include "ability_connect_helper.h"
#include "iremote_proxy.h"


namespace OHOS::AVSession {
    void AbilityConnectHelperFuzzTest(uint8_t *data, size_t size);
    void AbilityConnectionStubFuzzTest(uint8_t* data, size_t size);
    void AbilityConnectCallbackFuzzTest(uint8_t* data, size_t size);
}
#endif