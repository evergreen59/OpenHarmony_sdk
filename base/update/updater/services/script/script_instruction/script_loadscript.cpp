/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "script_loadscript.h"
#include "script_instruction.h"
#include "script_instructionhelper.h"
#include "script_manager.h"
#include "script_utils.h"

using namespace Uscript;

namespace BasicInstruction {
int32_t ScriptLoadScript::Execute(UScriptEnv &env, UScriptContext &context)
{
    ScriptInstructionHelper* helper = ScriptInstructionHelper::GetBasicInstructionHelper();
    USCRIPT_CHECK(helper != nullptr, return USCRIPT_INVALID_PARAM, "Failed to get instruction helper");

    std::string scriptName;
    int32_t priority = 0;
    int32_t ret = context.GetParam(0, scriptName);
    USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to get param");
    ret = context.GetParam(1, priority);
    USCRIPT_CHECK(ret == USCRIPT_SUCCESS, return ret, "Failed to get param");
    USCRIPT_LOGI("ScriptLoadScript %s priority:%d", scriptName.c_str(), priority);
    return helper->AddScript(scriptName, priority);
}
} // namespace BasicInstruction
