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

#include "shell_command.h"

#include <getopt.h>

#include "edm_log.h"

namespace OHOS {
namespace EDM {
ShellCommand::ShellCommand(int argc, char *argv[], std::string name)
{
    opterr = 0;
    argc_ = argc;
    argv_ = argv;
    name_ = name;

    if (argc < MIN_ARGUMENT_NUMBER) {
        cmd_ = "help";
        return;
    }
    cmd_ = argv[1];
    for (int i = 2; i < argc; i++) {
        argList_.push_back(argv[i]);
    }
}

ShellCommand::~ShellCommand()
{}

ErrCode ShellCommand::OnCommand()
{
    int result = OHOS::ERR_OK;
    if (commandMap_.find("help") == commandMap_.end()) {
        return OHOS::ERR_INVALID_VALUE;
    }

    auto respond = commandMap_["help"];
    if (commandMap_.find(cmd_) != commandMap_.end()) {
        respond = commandMap_[cmd_];
    }
    if (respond == nullptr) {
        resultReceiver_.append(GetCommandErrorMsg());
        respond = commandMap_["help"];
    }
    if (init() == OHOS::ERR_OK) {
        respond();
    } else {
        result = OHOS::ERR_INVALID_VALUE;
    }

    return result;
}

std::string ShellCommand::ExecCommand()
{
    int result = CreateCommandMap();
    if (result != OHOS::ERR_OK) {
        EDMLOGE("failed to create command map.\n");
    }

    result = CreateMessageMap();
    if (result != OHOS::ERR_OK) {
        EDMLOGE("failed to create message map.\n");
    }

    result = OnCommand();
    if (result != OHOS::ERR_OK) {
        EDMLOGE("failed to execute your command.\n");
        resultReceiver_ = "error: failed to execute your command.\n";
    }

    return resultReceiver_;
}

std::string ShellCommand::GetCommandErrorMsg() const
{
    std::string commandErrorMsg =
        name_ + ": '" + cmd_ + "' is not a valid " + name_ + " command. See '" + name_ + " help'.\n";

    return commandErrorMsg;
}

std::string ShellCommand::GetMessageFromCode(const int32_t code) const
{
    EDMLOGI("[%{public}s(%{public}s)] enter", __FILE__, __FUNCTION__);
    EDMLOGI("code = %{public}d", code);

    std::string result = "";
    if (messageMap_.find(code) != messageMap_.end()) {
        std::string message = messageMap_.at(code);
        if (message.size() != 0) {
            result.append(message + "\n");
        }
    }
    EDMLOGI("result = %{public}s", result.c_str());
    return result;
}
} // namespace EDM
} // namespace OHOS
