/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef IMS_TEST_HANDLER_H
#define IMS_TEST_HANDLER_H

#include <memory>

#include "event_handler.h"

namespace OHOS {
namespace Telephony {
class ImsTestHandler : public AppExecFwk::EventHandler {
public:
    void RegisterImsCall();
    void RegisterImsSms();
    void RegisterImsCoreService();
    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;

public:
    static const int32_t DEFAULT_SLOT_ID = 0;
};
} // namespace Telephony
} // namespace OHOS
#endif // IMS_TEST_HANDLER_H