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

#ifndef HIVIEW_PLUGINS_USAGE_EVENT_REPORT_IDL_HIVIEW_SHUTDOWN_CALLBACK_H
#define HIVIEW_PLUGINS_USAGE_EVENT_REPORT_IDL_HIVIEW_SHUTDOWN_CALLBACK_H

#include "hiview_shutdown_callback_stub.h"

namespace OHOS {
namespace HiviewDFX {
class HiViewShutdownCallback : public HiViewShutdownCallbackStub {
public:
    HiViewShutdownCallback() {}
    virtual ~HiViewShutdownCallback() {}
    void ShutdownCallback() override;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIVIEW_PLUGINS_USAGE_EVENT_REPORT_IDL_HIVIEW_SHUTDOWN_CALLBACK_H
