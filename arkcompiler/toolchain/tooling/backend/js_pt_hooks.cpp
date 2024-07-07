/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "backend/js_pt_hooks.h"

#include "agent/debugger_impl.h"

namespace panda::ecmascript::tooling {
void JSPtHooks::Breakpoint(const JSPtLocation &location)
{
    LOG_DEBUGGER(VERBOSE) << "JSPtHooks: Breakpoint => " << location.GetMethodId() << ": "
                         << location.GetBytecodeOffset();

    [[maybe_unused]] LocalScope scope(debugger_->vm_);
    debugger_->NotifyPaused(location, OTHER);
}

void JSPtHooks::Exception([[maybe_unused]] const JSPtLocation &location)
{
    LOG_DEBUGGER(VERBOSE) << "JSPtHooks: Exception";
    [[maybe_unused]] LocalScope scope(debugger_->vm_);

    debugger_->NotifyPaused({}, EXCEPTION);
}

bool JSPtHooks::SingleStep(const JSPtLocation &location)
{
    LOG_DEBUGGER(VERBOSE) << "JSPtHooks: SingleStep => " << location.GetBytecodeOffset();

    [[maybe_unused]] LocalScope scope(debugger_->vm_);
    if (UNLIKELY(firstTime_)) {
        firstTime_ = false;

        debugger_->NotifyPaused({}, BREAK_ON_START);
        return false;
    }

    // pause or step complete
    if (debugger_->NotifySingleStep(location)) {
        debugger_->NotifyPaused({}, OTHER);
        return true;
    }

    // temporary "safepoint" to handle possible protocol command
    debugger_->NotifyHandleProtocolCommand();

    return false;
}

void JSPtHooks::LoadModule(std::string_view pandaFileName, std::string_view entryPoint)
{
    LOG_DEBUGGER(VERBOSE) << "JSPtHooks: LoadModule: " << pandaFileName;

    [[maybe_unused]] LocalScope scope(debugger_->vm_);

    static uint32_t scriptId = 0;
    if (debugger_->NotifyScriptParsed(scriptId++, pandaFileName.data(), entryPoint)) {
        firstTime_ = true;
    }
}

void JSPtHooks::PendingJobEntry()
{
    LOG_DEBUGGER(VERBOSE) << "JSPtHooks: PendingJobEntry";

    [[maybe_unused]] LocalScope scope(debugger_->vm_);

    debugger_->NotifyPendingJobEntry();
}

void JSPtHooks::NativeCalling(const void *nativeAddress)
{
    LOG_DEBUGGER(INFO) << "JSPtHooks: NativeCalling, addr = " << nativeAddress;

    [[maybe_unused]] LocalScope scope(debugger_->vm_);

    debugger_->NotifyNativeCalling(nativeAddress);
}
}  // namespace panda::ecmascript::tooling
