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

#ifndef ARKCOMPILER_TOOLCHAIN_INSPECTOR_INSPECTOR_H
#define ARKCOMPILER_TOOLCHAIN_INSPECTOR_INSPECTOR_H

#include "ws_server.h"

#include <string>

namespace panda::ecmascript {
class EcmaVM;
}  // namespace panda::ecmascript

namespace OHOS::ArkCompiler::Toolchain {
using EcmaVM = panda::ecmascript::EcmaVM;
using DebuggerPostTask = std::function<void(std::function<void()>&&)>;

#if __cplusplus
extern "C" {
#endif

bool StartDebug(const std::string& componentName, void* vm, bool isDebugMode, int32_t instanceId,
    const DebuggerPostTask& debuggerPostTask);

void StopDebug(const std::string& componentName);

#if __cplusplus
}
#endif

class Inspector {
public:
    Inspector() = default;
    ~Inspector() = default;

    void OnMessage(std::string&& msg);

    static constexpr int32_t DELAY_CHECK_DISPATCH_STATUS = 100;

    pthread_t tid_ = 0;
    void* vm_ = nullptr;
    std::unique_ptr<WsServer> websocketServer_;
    DebuggerPostTask debuggerPostTask_;
};
} // namespace OHOS::ArkCompiler::Toolchain

#endif // ARKCOMPILER_TOOLCHAIN_INSPECTOR_INSPECTOR_H