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

#include <cstring>
#include <unistd.h>

#include "ability_command.h"
#include "ability_tool_command.h"
#include "hilog_wrapper.h"
#ifdef A11Y_ENABLE
#include "accessibility_ability_command.h"
#endif // A11Y_ENABLE
int main(int argc, char *argv[])
{
    if (strstr(argv[0], "aa") != nullptr) {
        OHOS::AAFwk::AbilityManagerShellCommand cmd(argc, argv);
        std::cout << cmd.ExecCommand();
    } else if (strstr(argv[0], "ability_tool") != nullptr) {
        OHOS::AAFwk::AbilityToolCommand cmd(argc, argv);
        std::cout << cmd.ExecCommand();
    } else {
#ifdef A11Y_ENABLE
        if (strstr(argv[0], "accessibility") != nullptr) {
            OHOS::AAFwk::AccessibilityAbilityShellCommand cmd(argc, argv);
            std::cout << cmd.ExecCommand();
        }
#endif // A11Y_ENABLE
    }
    fflush(stdout);
    _exit(0);
}
