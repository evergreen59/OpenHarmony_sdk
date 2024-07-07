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

#ifndef OHOS_ABILITY_RUNTIME_ABILITY_COMMAND_H
#define OHOS_ABILITY_RUNTIME_ABILITY_COMMAND_H

#include "shell_command.h"
#include "ability_manager_interface.h"

namespace OHOS {
namespace AAFwk {
namespace {
const std::string TOOL_NAME = "aa";

const std::string HELP_MSG = "usage: aa <command> <options>\n"
                             "These are common aa commands list:\n"
                             "  help                        list available commands\n"
                             "  start                       start ability with options\n"
                             "  stop-service                stop service with options\n"
                             "  dump                        dump the ability info\n"
                             "  force-stop <bundle-name>    force stop the process with bundle name\n"
#ifdef ABILITY_COMMAND_FOR_TEST
                             "  test                        start the test framework with options\n"
                             "  ApplicationNotResponding     Pass in pid with options\n"
                             "  block-ability <ability-record-id>       block ability with ability record id\n"
                             "  block-ams-service                       block ams service\n"
                             "  block-app-service                       block app service\n";
#else
                             "  test                        start the test framework with options\n";
#endif

const std::string HELP_MSG_SCREEN =
    "usage: aa screen <options>\n"
    "options list:\n"
    "  -h, --help                                                   list available commands\n"
    "  -p, --power <state>                                          power on or off with a state name\n";

const std::string HELP_ApplicationNotResponding =
    "usage: aa ApplicationNotResponding <options>\n"
    "options list:\n"
    "  -h, --help                   list available commands\n"
    "  -p, --pid                    Pass in pid with option\n";

const std::string HELP_MSG_START =
    "usage: aa start <options>\n"
    "options list:\n"
    "  -h, --help                                                   list available commands\n"
    "  [-d <device-id>] -a <ability-name> -b <bundle-name> [-m <module-name>] [-D] "
    "  start ability with an element name\n";

const std::string HELP_MSG_STOP_SERVICE =
    "usage: aa stop-service <options>\n"
    "options list:\n"
    "  -h, --help                                                   list available commands\n"
    "  [-d <device-id>] -a <ability-name> -b <bundle-name> [-m <module-name>] "
    "  stop service with an element name\n";

const std::string HELP_MSG_DUMPSYS = "usage: aa dump <options>\n"
                                  "options list:\n"
                                  "  -h, --help                   list available commands\n"
                                  "  -a, --all                    dump all abilities\n"
                                  "  -l, --mission-list           dump mission list\n"
                                  "  -i, --ability                dump abilityRecordId\n"
                                  "  -e, --extension              dump elementName (FA: serviceAbilityRecords,"
                                                                  "Stage: ExtensionRecords)\n"
                                  "  -p, --pending                dump pendingWantRecordId\n"
                                  "  -r, --process                dump process\n"
                                  "  -d, --data                   dump the data abilities\n"
                                  "  -u, --userId                 userId\n"
                                  "  -c, --client                 client\n"
                                  "  -c, -u are auxiliary parameters and cannot be used alone\n"
                                  "  The original -s parameter is invalid\n"
                                  "  The original -m parameter is invalid\n";

const std::string HELP_MSG_TEST =
    "usage: aa test <options>\n"
    "options list:\n"
    "  -h, --help                                             list available commands\n"
    "  -b <bundle-name> -s unittest <test-runner>             start the test framework with options\n"
    "                  [-p <package-name>]                    the name of package with test-runner, "
    "required for the FA model\n"
    "                  [-m <module-name>]                     the name of module with test-runner, "
    "required for the STAGE model\n"
    "                  [-s class <test-class>]\n"
    "                  [-s level <test-level>]\n"
    "                  [-s size <test-size>]\n"
    "                  [-s testType <test-testType>]\n"
    "                  [-s timeout <test-timeout>]\n"
    "                  [-s <any-key> <any-value>]\n"
    "                  [-w <wait-time>]\n"
    "                  [-D]\n";

const std::string HELP_MSG_FORCE_STOP = "usage: aa force-stop <bundle-name>\n";
const std::string HELP_MSG_BLOCK_ABILITY = "usage: aa block-ability <abilityrecordid>\n";
const std::string HELP_MSG_FORCE_TIMEOUT =
    "usage: aa force-timeout <ability-name> <INITIAL|INACTIVE|COMMAND|FOREGROUND|BACKGROUND|TERMINATING>\n"
    "usage: aa force-timeout clean.";
const std::string HELP_MSG_FORCE_TIMEOUT_CLEAN = "clean";

const std::string HELP_MSG_NO_ABILITY_NAME_OPTION = "error: -a <ability-name> is expected";
const std::string HELP_MSG_NO_BUNDLE_NAME_OPTION = "error: -b <bundle-name> is expected";

const std::string STRING_START_ABILITY_OK = "start ability successfully.";
const std::string STRING_START_ABILITY_NG = "error: failed to start ability.";

const std::string STRING_STOP_SERVICE_ABILITY_OK = "stop service ability successfully.";
const std::string STRING_STOP_SERVICE_ABILITY_NG = "error: failed to stop service ability.";

const std::string STRING_FORCE_STOP_OK = "force stop process successfully.";
const std::string STRING_FORCE_STOP_NG = "error: failed to force stop process.";

const std::string STRING_START_USER_TEST_NG = "error: failed to start user test.";
const std::string STRING_USER_TEST_STARTED = "user test started.";
const std::string STRING_USER_TEST_FINISHED = "user test finished.";

const std::string STRING_BLOCK_ABILITY_OK = "block ability successfully.";
const std::string STRING_BLOCK_ABILITY_NG = "error: failed to block stop ability.";

const std::string STRING_BLOCK_AMS_SERVICE_OK = "block ams service successfully.";
const std::string STRING_BLOCK_AMS_SERVICE_NG = "error: failed to block ams service.";

const std::string STRING_BLOCK_APP_SERVICE_OK = "block app service successfully.";
const std::string STRING_BLOCK_APP_SERVICE_NG = "error: failed to block app service.";

const int USER_TEST_COMMAND_START_INDEX = 2;
const int USER_TEST_COMMAND_PARAMS_NUM = 2;
const int TIME_RATE_MS = 1000;
const std::string STRING_FORCE_TIMEOUT_OK = "force ability timeout successfully.";
const std::string STRING_FORCE_TIMEOUT_NG = "error: failed to force ability timeout.";

const int NUMBER_TWO = 2;
const int NUMBER_ONE = 1;

const std::string DEBUG_VALUE = "true";

const std::string STRING_TEST_REGEX_INTEGER_NUMBERS = "^(0|[1-9][0-9]*|-[1-9][0-9]*)$";
}  // namespace

class AbilityManagerShellCommand : public ShellCommand {
public:
    AbilityManagerShellCommand(int argc, char *argv[]);
    ~AbilityManagerShellCommand() override
    {}

    ErrCode CreateMessageMap() override;
    bool IsTestCommandIntegrity(const std::map<std::string, std::string> &params);
    ErrCode StartUserTest(const std::map<std::string, std::string> &params);

private:
    ErrCode CreateCommandMap() override;
    ErrCode init() override;

    ErrCode RunAsHelpCommand();
    ErrCode RunAsScreenCommand();
    ErrCode RunAsStartAbility();
    ErrCode RunAsStopService();
    ErrCode RunAsDumpsysCommand();
    ErrCode RunAsForceStop();
    #ifdef ABILITY_COMMAND_FOR_TEST
    ErrCode RunForceTimeoutForTest();
    ErrCode RunAsSendAppNotRespondingProcessID();
    ErrCode RunAsBlockAbilityCommand();
    ErrCode RunAsBlockAmsServiceCommand();
    ErrCode RunAsBlockAppServiceCommand();
    ErrCode RunAsSendAppNotRespondingWithUnknownOption();
    ErrCode RunAsSendAppNotRespondingWithOption(int32_t option, std::string &pid);
    #endif
    sptr<IAbilityManager> GetAbilityManagerService();

    ErrCode MakeWantFromCmd(Want &want, std::string &windowMode);
    ErrCode RunAsTestCommand();
    ErrCode TestCommandError(const std::string &info);
};
}  // namespace AAFwk
}  // namespace OHOS

#endif  // OHOS_ABILITY_RUNTIME_ABILITY_COMMAND_H
