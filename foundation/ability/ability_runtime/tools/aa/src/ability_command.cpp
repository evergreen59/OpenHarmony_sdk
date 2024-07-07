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
#include "ability_command.h"

#include <csignal>
#include <cstdlib>
#include <getopt.h>
#include <regex>
#include "ability_manager_client.h"
#include "hilog_wrapper.h"
#include "iservice_registry.h"
#include "mission_snapshot.h"
#include "bool_wrapper.h"
#include "sa_mgr_client.h"
#include "system_ability_definition.h"
#include "test_observer.h"

using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AAFwk {
namespace {
const std::string SHORT_OPTIONS = "ch:d:a:b:p:s:m:CD";
constexpr struct option LONG_OPTIONS[] = {
    {"help", no_argument, nullptr, 'h'},
    {"device", required_argument, nullptr, 'd'},
    {"ability", required_argument, nullptr, 'a'},
    {"bundle", required_argument, nullptr, 'b'},
    {"power", required_argument, nullptr, 'p'},
    {"setting", required_argument, nullptr, 's'},
    {"module", required_argument, nullptr, 'm'},
    {"cold-start", no_argument, nullptr, 'C'},
    {"debug", no_argument, nullptr, 'D'},
    {nullptr, 0, nullptr, 0},
};
const std::string SHORT_OPTIONS_APPLICATION_NOT_RESPONDING = "hp:";
#ifdef ABILITY_COMMAND_FOR_TEST
constexpr struct option LONG_OPTIONS_ApplicationNotResponding[] = {
    {"help", no_argument, nullptr, 'h'},
    {"pid", required_argument, nullptr, 'p'},
    {nullptr, 0, nullptr, 0},
};
#endif
const std::string SHORT_OPTIONS_DUMPSYS = "hal::i:e::p::r::d::u:c";
constexpr struct option LONG_OPTIONS_DUMPSYS[] = {
    {"help", no_argument, nullptr, 'h'},
    {"all", no_argument, nullptr, 'a'},
    {"mission-list", no_argument, nullptr, 'l'},
    {"ability", required_argument, nullptr, 'i'},
    {"extension", no_argument, nullptr, 'e'},
    {"pending", no_argument, nullptr, 'p'},
    {"process", no_argument, nullptr, 'r'},
    {"data", no_argument, nullptr, 'd'},
    {"userId", required_argument, nullptr, 'u'},
    {"client", no_argument, nullptr, 'c'},
    {nullptr, 0, nullptr, 0},
};
}  // namespace

AbilityManagerShellCommand::AbilityManagerShellCommand(int argc, char *argv[]) : ShellCommand(argc, argv, TOOL_NAME)
{
    for (int i = 0; i < argc_; i++) {
        HILOG_INFO("argv_[%{public}d]: %{public}s", i, argv_[i]);
    }
}

ErrCode AbilityManagerShellCommand::CreateCommandMap()
{
    commandMap_ = {
        {"help", std::bind(&AbilityManagerShellCommand::RunAsHelpCommand, this)},
        {"screen", std::bind(&AbilityManagerShellCommand::RunAsScreenCommand, this)},
        {"start", std::bind(&AbilityManagerShellCommand::RunAsStartAbility, this)},
        {"stop-service", std::bind(&AbilityManagerShellCommand::RunAsStopService, this)},
        {"dump", std::bind(&AbilityManagerShellCommand::RunAsDumpsysCommand, this)},
        {"force-stop", std::bind(&AbilityManagerShellCommand::RunAsForceStop, this)},
        {"test", std::bind(&AbilityManagerShellCommand::RunAsTestCommand, this)},
#ifdef ABILITY_COMMAND_FOR_TEST
        {"force-timeout", std::bind(&AbilityManagerShellCommand::RunForceTimeoutForTest, this)},
        {"ApplicationNotResponding", std::bind(&AbilityManagerShellCommand::RunAsSendAppNotRespondingProcessID, this)},
        {"block-ability", std::bind(&AbilityManagerShellCommand::RunAsBlockAbilityCommand, this)},
        {"block-ams-service", std::bind(&AbilityManagerShellCommand::RunAsBlockAmsServiceCommand, this)},
        {"block-app-service", std::bind(&AbilityManagerShellCommand::RunAsBlockAppServiceCommand, this)},
#endif
    };

    return OHOS::ERR_OK;
}

ErrCode AbilityManagerShellCommand::CreateMessageMap()
{
    messageMap_ = {
        //  code + message
        {
            RESOLVE_ABILITY_ERR,
            "error: resolve ability err.",
        },
        {
            GET_ABILITY_SERVICE_FAILED,
            "error: get ability service failed.",
        },
        {
            ABILITY_SERVICE_NOT_CONNECTED,
            "error: ability service not connected.",
        },
        {
            RESOLVE_APP_ERR,
            "error: resolve app err.",
        },
        {
            ABILITY_EXISTED,
            "error: ability existed.",
        },
        {
            CREATE_MISSION_STACK_FAILED,
            "error: create mission stack failed.",
        },
        {
            CREATE_ABILITY_RECORD_FAILED,
            "error: create ability record failed.",
        },
        {
            START_ABILITY_WAITING,
            "start ability successfully. waiting...",
        },
        {
            TERMINATE_LAUNCHER_DENIED,
            "error: terminate launcher denied.",
        },
        {
            CONNECTION_NOT_EXIST,
            "error: connection not exist.",
        },
        {
            INVALID_CONNECTION_STATE,
            "error: invalid connection state.",
        },
        {
            LOAD_ABILITY_TIMEOUT,
            "error: load ability timeout.",
        },
        {
            CONNECTION_TIMEOUT,
            "error: connection timeout.",
        },
        {
            GET_BUNDLE_MANAGER_SERVICE_FAILED,
            "error: get bundle manager service failed.",
        },
        {
            REMOVE_MISSION_FAILED,
            "error: remove mission failed.",
        },
        {
            INNER_ERR,
            "error: inner err.",
        },
        {
            GET_RECENT_MISSIONS_FAILED,
            "error: get recent missions failed.",
        },
        {
            REMOVE_STACK_LAUNCHER_DENIED,
            "error: remove stack launcher denied.",
        },
        {
            TARGET_ABILITY_NOT_SERVICE,
            "error: target ability not service.",
        },
        {
            TERMINATE_SERVICE_IS_CONNECTED,
            "error: terminate service is connected.",
        },
        {
            START_SERVICE_ABILITY_ACTIVATING,
            "error: start service ability activating.",
        },
        {
            KILL_PROCESS_FAILED,
            "error: kill process failed.",
        },
        {
            UNINSTALL_APP_FAILED,
            "error: uninstall app failed.",
        },
        {
            TERMINATE_ABILITY_RESULT_FAILED,
            "error: terminate ability result failed.",
        },
        {
            CHECK_PERMISSION_FAILED,
            "error: check permission failed.",
        },
        {
            NO_FOUND_ABILITY_BY_CALLER,
            "error: no found ability by caller.",
        },
        {
            ABILITY_VISIBLE_FALSE_DENY_REQUEST,
            "error: ability visible false deny request.",
        },
        {
            GET_BUNDLE_INFO_FAILED,
            "error: get bundle info failed.",
        },
        {
            KILL_PROCESS_KEEP_ALIVE,
            "error: keep alive process can not be killed.",
        },
    };

    return OHOS::ERR_OK;
}

ErrCode AbilityManagerShellCommand::init()
{
    return AbilityManagerClient::GetInstance()->Connect();
}

ErrCode AbilityManagerShellCommand::RunAsHelpCommand()
{
    resultReceiver_.append(HELP_MSG);

    return OHOS::ERR_OK;
}

ErrCode AbilityManagerShellCommand::RunAsScreenCommand()
{
    HILOG_INFO("enter");

    int result = OHOS::ERR_OK;

    int option = -1;
    int counter = 0;

    while (true) {
        counter++;

        option = getopt_long(argc_, argv_, SHORT_OPTIONS.c_str(), LONG_OPTIONS, nullptr);

        HILOG_INFO("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);

        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }

        if (option == -1) {
            // When scanning the first argument
            if (counter == 1 && strcmp(argv_[optind], cmd_.c_str()) == 0) {
                // 'aa screen' with no option: aa screen
                // 'aa screen' with a wrong argument: aa screen xxx
                HILOG_INFO("'aa %{public}s' %{public}s", HELP_MSG_NO_OPTION.c_str(), cmd_.c_str());
                resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                result = OHOS::ERR_INVALID_VALUE;
            }
            break;
        }

        if (option == '?') {
            switch (optopt) {
                case 'p': {
                    // 'aa screen -p' with no argument
                    HILOG_INFO("'aa %{public}s -p' with no argument.", cmd_.c_str());

                    resultReceiver_.append("error: option ");
                    resultReceiver_.append("requires a value.\n");

                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 0: {
                    // 'aa screen' with an unknown option: aa screen --x
                    // 'aa screen' with an unknown option: aa screen --xxx
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);

                    HILOG_INFO("'aa screen' with an unknown option.");

                    resultReceiver_.append(unknownOptionMsg);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                default: {
                    // 'aa screen' with an unknown option: aa screen -x
                    // 'aa screen' with an unknown option: aa screen -xxx
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);

                    HILOG_INFO("'aa screen' with an unknown option.");

                    resultReceiver_.append(unknownOptionMsg);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
            }
            break;
        }

        switch (option) {
            case 'h': {
                // 'aa screen -h'
                // 'aa screen --help'
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            case 0: {
                break;
            }
            default: {
                break;
            }
        }
    }

    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_SCREEN);
        result = OHOS::ERR_INVALID_VALUE;
    }

    return result;
}

ErrCode AbilityManagerShellCommand::RunAsStartAbility()
{
    Want want;
    std::string windowMode;
    ErrCode result = MakeWantFromCmd(want, windowMode);
    if (result == OHOS::ERR_OK) {
        int windowModeKey = std::atoi(windowMode.c_str());
        if (windowModeKey > 0) {
            auto setting = AbilityStartSetting::GetEmptySetting();
            if (setting != nullptr) {
                setting->AddProperty(AbilityStartSetting::WINDOW_MODE_KEY, windowMode);
                result = AbilityManagerClient::GetInstance()->StartAbility(want, *(setting.get()), nullptr, -1);
            }
        } else {
            result = AbilityManagerClient::GetInstance()->StartAbility(want);
        }
        if (result == OHOS::ERR_OK) {
            HILOG_INFO("%{public}s", STRING_START_ABILITY_OK.c_str());
            resultReceiver_ = STRING_START_ABILITY_OK + "\n";
        } else {
            HILOG_INFO("%{public}s result = %{public}d", STRING_START_ABILITY_NG.c_str(), result);
            if (result != START_ABILITY_WAITING) {
                resultReceiver_ = STRING_START_ABILITY_NG + "\n";
            }
            resultReceiver_.append(GetMessageFromCode(result));
        }
    } else {
        resultReceiver_.append(HELP_MSG_START);
        result = OHOS::ERR_INVALID_VALUE;
    }

    return result;
}

ErrCode AbilityManagerShellCommand::RunAsStopService()
{
    ErrCode result = OHOS::ERR_OK;

    Want want;
    std::string windowMode;
    result = MakeWantFromCmd(want, windowMode);
    if (result == OHOS::ERR_OK) {
        result = AbilityManagerClient::GetInstance()->StopServiceAbility(want);
        if (result == OHOS::ERR_OK) {
            HILOG_INFO("%{public}s", STRING_STOP_SERVICE_ABILITY_OK.c_str());
            resultReceiver_ = STRING_STOP_SERVICE_ABILITY_OK + "\n";
        } else {
            HILOG_INFO("%{public}s result = %{public}d", STRING_STOP_SERVICE_ABILITY_NG.c_str(), result);
            resultReceiver_ = STRING_STOP_SERVICE_ABILITY_NG + "\n";

            resultReceiver_.append(GetMessageFromCode(result));
        }
    } else {
        resultReceiver_.append(HELP_MSG_STOP_SERVICE);
        result = OHOS::ERR_INVALID_VALUE;
    }

    return result;
}

ErrCode AbilityManagerShellCommand::RunAsDumpsysCommand()
{
    ErrCode result = OHOS::ERR_OK;
    bool isUserID = false;
    bool isClient = false;
    int userID = DEFAULT_INVAL_VALUE;
    bool isfirstCommand = false;
    std::string args;
    for (auto it = argList_.begin(); it != argList_.end(); it++) {
        if (*it == "-c" || *it == "--client") {
            if (isClient == false) {
                isClient = true;
            } else {
                result = OHOS::ERR_INVALID_VALUE;
                resultReceiver_.append(HELP_MSG_DUMPSYS);
                return result;
            }
        } else if (*it == "-u" || *it == "--userId") {
            if (it + 1 == argList_.end()) {
                result = OHOS::ERR_INVALID_VALUE;
                resultReceiver_.append(HELP_MSG_DUMPSYS);
                return result;
            }
            (void)StrToInt(*(it + 1), userID);
            if (userID == DEFAULT_INVAL_VALUE) {
                result = OHOS::ERR_INVALID_VALUE;
                resultReceiver_.append(HELP_MSG_DUMPSYS);
                return result;
            }
            if (isUserID == false) {
                isUserID = true;
            } else {
                result = OHOS::ERR_INVALID_VALUE;
                resultReceiver_.append(HELP_MSG_DUMPSYS);
                return result;
            }
        } else if (*it == std::to_string(userID)) {
            continue;
        } else {
            args += *it;
            args += " ";
        }
    }

    while (true) {
        int option = getopt_long(argc_, argv_, SHORT_OPTIONS_DUMPSYS.c_str(), LONG_OPTIONS_DUMPSYS, nullptr);

        HILOG_INFO("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);

        if (optind < 0 || optind > argc_) {
            resultReceiver_.append(HELP_MSG_DUMPSYS);
            return OHOS::ERR_INVALID_VALUE;
        }

        if (option == -1) {
            break;
        }

        switch (option) {
            case 'h': {
                // 'aa dumpsys -h'
                // 'aa dumpsys --help'
                resultReceiver_.append(HELP_MSG_DUMPSYS);
                result = OHOS::ERR_INVALID_VALUE;
                return result;
            }
            case 'a': {
                if (isfirstCommand == false) {
                    isfirstCommand = true;
                } else {
                    result = OHOS::ERR_INVALID_VALUE;
                    resultReceiver_.append(HELP_MSG_DUMPSYS);
                    return result;
                }
                // 'aa dumpsys -a'
                // 'aa dumpsys --all'
                break;
            }
            case 'l': {
                if (isfirstCommand == false) {
                    isfirstCommand = true;
                } else {
                    // 'aa dump -i 10 -element -lastpage'
                    // 'aa dump -i 10 -render -lastpage'
                    // 'aa dump -i 10 -layer'
                    if ((optarg != nullptr) && strcmp(optarg, "astpage") && strcmp(optarg, "ayer")) {
                        result = OHOS::ERR_INVALID_VALUE;
                        resultReceiver_.append(HELP_MSG_DUMPSYS);
                        return result;
                    }
                }
                // 'aa dumpsys -l'
                // 'aa dumpsys --mission-list'
                break;
            }
            case 'i': {
                if (isfirstCommand == false) {
                    isfirstCommand = true;
                    int abilityRecordId = DEFAULT_INVAL_VALUE;
                    (void)StrToInt(optarg, abilityRecordId);
                    if (abilityRecordId == DEFAULT_INVAL_VALUE) {
                        result = OHOS::ERR_INVALID_VALUE;
                        resultReceiver_.append(HELP_MSG_DUMPSYS);
                        return result;
                    }
                } else {
                    // 'aa dumpsys -i 10 -inspector'
                    if ((optarg != nullptr) && strcmp(optarg, "nspector")) {
                        result = OHOS::ERR_INVALID_VALUE;
                        resultReceiver_.append(HELP_MSG_DUMPSYS);
                        return result;
                    }
                }
                // 'aa dumpsys -i'
                // 'aa dumpsys --ability'
                break;
            }
            case 'e': {
                if (isfirstCommand == false && optarg == nullptr) {
                    isfirstCommand = true;
                } else {
                    // 'aa dumpsys -i 10 -element'
                    if ((optarg != nullptr) && strcmp(optarg, "lement")) {
                        result = OHOS::ERR_INVALID_VALUE;
                        resultReceiver_.append(HELP_MSG_DUMPSYS);
                        return result;
                    }
                }
                // 'aa dumpsys -e'
                // 'aa dumpsys --extension'
                break;
            }
            case 'p': {
                if (isfirstCommand == false && optarg == nullptr) {
                    isfirstCommand = true;
                } else {
                    result = OHOS::ERR_INVALID_VALUE;
                    resultReceiver_.append(HELP_MSG_DUMPSYS);
                    return result;
                }
                // 'aa dumpsys -p'
                // 'aa dumpsys --pending'
                break;
            }
            case 'r': {
                if (isfirstCommand == false && optarg == nullptr) {
                    isfirstCommand = true;
                } else {
                    // 'aa dump -i 10 -render'
                    // 'aa dump -i 10 -rotation'
                    // 'aa dump -i 10 -frontend'
                    if ((optarg != nullptr) && strcmp(optarg, "ender") && strcmp(optarg, "otation") &&
                        strcmp(optarg, "ontend")) {
                        result = OHOS::ERR_INVALID_VALUE;
                        resultReceiver_.append(HELP_MSG_DUMPSYS);
                        return result;
                    }
                }
                // 'aa dumpsys -r'
                // 'aa dumpsys --process'
                break;
            }
            case 'd': {
                if (isfirstCommand == false && optarg == nullptr) {
                    isfirstCommand = true;
                } else {
                    result = OHOS::ERR_INVALID_VALUE;
                    resultReceiver_.append(HELP_MSG_DUMPSYS);
                    return result;
                }
                // 'aa dumpsys -d'
                // 'aa dumpsys --data'
                break;
            }
            case 'u': {
                // 'aa dumpsys -u'
                // 'aa dumpsys --userId'
                break;
            }
            case 'c': {
                // 'aa dumpsys -c'
                // 'aa dumpsys --client'
                break;
            }
            case '?': {
                if (!isfirstCommand) {
                    HILOG_INFO("'aa %{public}s' with an unknown option.", cmd_.c_str());
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
                    resultReceiver_.append(unknownOptionMsg);
                    resultReceiver_.append(HELP_MSG_DUMPSYS);
                    result = OHOS::ERR_INVALID_VALUE;
                    return result;
                }
                break;
            }
            default: {
                HILOG_INFO("'aa %{public}s' with an unknown option.", cmd_.c_str());
                std::string unknownOption = "";
                std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
                resultReceiver_.append(unknownOptionMsg);
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
        }
    }

    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_DUMPSYS);
    } else {
        if (isfirstCommand != true) {
            result = OHOS::ERR_INVALID_VALUE;
            resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
            resultReceiver_.append(HELP_MSG_DUMPSYS);
            return result;
        }

        std::vector<std::string> dumpResults;
        result = AbilityManagerClient::GetInstance()->DumpSysState(args, dumpResults, isClient, isUserID, userID);
        if (result == OHOS::ERR_OK) {
            for (auto it : dumpResults) {
                resultReceiver_ += it + "\n";
            }
        } else {
            HILOG_INFO("failed to dump state.");
        }
    }
    return result;
}

ErrCode AbilityManagerShellCommand::RunAsForceStop()
{
    HILOG_INFO("[%{public}s(%{public}s)] enter", __FILE__, __FUNCTION__);
    if (argList_.empty()) {
        resultReceiver_.append(HELP_MSG_FORCE_STOP + "\n");
        return OHOS::ERR_INVALID_VALUE;
    }
    HILOG_INFO("Bundle name : %{public}s", argList_[0].c_str());
    ErrCode result = OHOS::ERR_OK;
    result = AbilityManagerClient::GetInstance()->KillProcess(argList_[0]);
    if (result == OHOS::ERR_OK) {
        HILOG_INFO("%{public}s", STRING_FORCE_STOP_OK.c_str());
        resultReceiver_ = STRING_FORCE_STOP_OK + "\n";
    } else {
        HILOG_INFO("%{public}s result = %{public}d", STRING_FORCE_STOP_NG.c_str(), result);
        resultReceiver_ = STRING_FORCE_STOP_NG + "\n";
        resultReceiver_.append(GetMessageFromCode(result));
    }
    return result;
}

#ifdef ABILITY_COMMAND_FOR_TEST
ErrCode AbilityManagerShellCommand::RunForceTimeoutForTest()
{
    HILOG_INFO("[%{public}s(%{public}s)] enter", __FILE__, __FUNCTION__);
    if (argList_.empty()) {
        resultReceiver_.append(HELP_MSG_FORCE_TIMEOUT + "\n");
        return OHOS::ERR_INVALID_VALUE;
    }

    ErrCode result = OHOS::ERR_OK;
    if (argList_.size() == NUMBER_ONE && argList_[0] == HELP_MSG_FORCE_TIMEOUT_CLEAN) {
        HILOG_INFO("clear ability timeout flags.");
        result = AbilityManagerClient::GetInstance()->ForceTimeoutForTest(argList_[0], "");
    } else if (argList_.size() == NUMBER_TWO) {
        HILOG_INFO("Ability name : %{public}s, state: %{public}s", argList_[0].c_str(), argList_[1].c_str());
        result = AbilityManagerClient::GetInstance()->ForceTimeoutForTest(argList_[0], argList_[1]);
    } else {
        resultReceiver_.append(HELP_MSG_FORCE_TIMEOUT + "\n");
        return OHOS::ERR_INVALID_VALUE;
    }
    if (result == OHOS::ERR_OK) {
        HILOG_INFO("%{public}s", STRING_FORCE_TIMEOUT_OK.c_str());
        resultReceiver_ = STRING_FORCE_TIMEOUT_OK + "\n";
    } else {
        HILOG_INFO("%{public}s result = %{public}d", STRING_FORCE_TIMEOUT_NG.c_str(), result);
        resultReceiver_ = STRING_FORCE_TIMEOUT_NG + "\n";
        resultReceiver_.append(GetMessageFromCode(result));
    }
    return result;
}
#endif

ErrCode AbilityManagerShellCommand::MakeWantFromCmd(Want &want, std::string &windowMode)
{
    int result = OHOS::ERR_OK;

    int option = -1;
    int counter = 0;

    std::string deviceId = "";
    std::string bundleName = "";
    std::string abilityName = "";
    std::string moduleName;
    bool isColdStart = false;
    bool isDebugApp = false;
    bool isContinuation = false;

    while (true) {
        counter++;

        option = getopt_long(argc_, argv_, SHORT_OPTIONS.c_str(), LONG_OPTIONS, nullptr);

        HILOG_INFO("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);

        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }

        if (option == -1) {
            // When scanning the first argument
            if (counter == 1 && strcmp(argv_[optind], cmd_.c_str()) == 0) {
                // 'aa start' with no option: aa start
                // 'aa start' with a wrong argument: aa start xxx
                // 'aa stop-service' with no option: aa stop-service
                // 'aa stop-service' with a wrong argument: aa stop-service xxx
                HILOG_INFO("'aa %{public}s' %{public}s", HELP_MSG_NO_OPTION.c_str(), cmd_.c_str());

                resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                result = OHOS::ERR_INVALID_VALUE;
            }
            break;
        }

        if (option == '?') {
            switch (optopt) {
                case 'h': {
                    // 'aa start -h'
                    // 'aa stop-service -h'
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 'd': {
                    // 'aa start -d' with no argument
                    // 'aa stop-service -d' with no argument
                    HILOG_INFO("'aa %{public}s -d' with no argument.", cmd_.c_str());

                    resultReceiver_.append("error: option ");
                    resultReceiver_.append("requires a value.\n");

                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 'a': {
                    // 'aa start -a' with no argument
                    // 'aa stop-service -a' with no argument
                    HILOG_INFO("'aa %{public}s -a' with no argument.", cmd_.c_str());

                    resultReceiver_.append("error: option ");
                    resultReceiver_.append("requires a value.\n");

                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 'b': {
                    // 'aa start -b' with no argument
                    // 'aa stop-service -b' with no argument
                    HILOG_INFO("'aa %{public}s -b' with no argument.", cmd_.c_str());

                    resultReceiver_.append("error: option ");
                    resultReceiver_.append("requires a value.\n");

                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 's': {
                    // 'aa start -s' with no argument
                    // 'aa stop-service -s' with no argument
                    HILOG_INFO("'aa %{public}s -s' with no argument.", cmd_.c_str());

                    resultReceiver_.append("error: option ");
                    resultReceiver_.append(argv_[optind - 1]);
                    resultReceiver_.append("' requires a value.\n");

                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 'm': {
                    // 'aa start -m' with no argument
                    // 'aa stop-service -m' with no argument
                    HILOG_INFO("'aa %{public}s -m' with no argument.", cmd_.c_str());

                    resultReceiver_.append("error: option ");
                    resultReceiver_.append("requires a value.\n");

                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 0: {
                    // 'aa start' with an unknown option: aa start --x
                    // 'aa start' with an unknown option: aa start --xxx
                    // 'aa stop-service' with an unknown option: aa stop-service --x
                    // 'aa stop-service' with an unknown option: aa stop-service --xxx
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);

                    HILOG_INFO("'aa %{public}s' with an unknown option.", cmd_.c_str());

                    resultReceiver_.append(unknownOptionMsg);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                default: {
                    // 'aa start' with an unknown option: aa start -x
                    // 'aa start' with an unknown option: aa start -xxx
                    // 'aa stop-service' with an unknown option: aa stop-service -x
                    // 'aa stop-service' with an unknown option: aa stop-service -xxx
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);

                    HILOG_INFO("'aa %{public}s' with an unknown option.", cmd_.c_str());

                    resultReceiver_.append(unknownOptionMsg);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
            }
            break;
        }

        switch (option) {
            case 'h': {
                // 'aa start -h'
                // 'aa start --help'
                // 'aa stop-service -h'
                // 'aa stop-service --help'
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            case 'd': {
                // 'aa start -d xxx'
                // 'aa stop-service -d xxx'

                // save device ID
                if (optarg != nullptr) {
                    deviceId = optarg;
                }
                break;
            }
            case 'a': {
                // 'aa start -a xxx'
                // 'aa stop-service -a xxx'

                // save ability name
                abilityName = optarg;
                break;
            }
            case 'b': {
                // 'aa start -b xxx'
                // 'aa stop-service -b xxx'

                // save bundle name
                bundleName = optarg;
                break;
            }
            case 's': {
                // 'aa start -s xxx'
                // save windowMode
                windowMode = optarg;
                break;
            }
            case 'm': {
                // 'aa start -m xxx'
                // 'aa stop-service -m xxx'

                // save module name
                moduleName = optarg;
                break;
            }
            case 'C': {
                // 'aa start -C'
                // cold start app
                isColdStart = true;
                break;
            }
            case 'D': {
                // 'aa start -D'
                // debug app
                isDebugApp = true;
                break;
            }
            case 'c': {
                // 'aa start -c'
                // set ability launch reason = continuation
                isContinuation = true;
                break;
            }
            case 0: {
                break;
            }
            default: {
                break;
            }
        }
    }

    if (result == OHOS::ERR_OK) {
        if (abilityName.size() == 0 || bundleName.size() == 0) {
            // 'aa start [-d <device-id>] -a <ability-name> -b <bundle-name> [-D]'
            // 'aa stop-service [-d <device-id>] -a <ability-name> -b <bundle-name>'
            HILOG_INFO("'aa %{public}s' without enough options.", cmd_.c_str());

            if (abilityName.size() == 0) {
                resultReceiver_.append(HELP_MSG_NO_ABILITY_NAME_OPTION + "\n");
            }

            if (bundleName.size() == 0) {
                resultReceiver_.append(HELP_MSG_NO_BUNDLE_NAME_OPTION + "\n");
            }

            result = OHOS::ERR_INVALID_VALUE;
        } else {
            ElementName element(deviceId, bundleName, abilityName, moduleName);
            want.SetElement(element);

            if (isColdStart) {
                want.SetParam("coldStart", isColdStart);
            }
            if (isDebugApp) {
                want.SetParam("debugApp", isDebugApp);
            }
            if (isContinuation) {
                want.AddFlags(Want::FLAG_ABILITY_CONTINUATION);
            }
        }
    }

    return result;
}

ErrCode AbilityManagerShellCommand::RunAsTestCommand()
{
    HILOG_INFO("enter");
    std::map<std::string, std::string> params;

    for (int i = USER_TEST_COMMAND_START_INDEX; i < argc_; i++) {
        HILOG_INFO("argv_[%{public}d]: %{public}s", i, argv_[i]);
        std::string opt = argv_[i];
        if ((opt == "-h") || (opt == "--help")) {
            resultReceiver_.append(HELP_MSG_TEST);
            return OHOS::ERR_OK;
        } else if ((opt == "-b") || (opt == "-p") || (opt == "-m")) {
            if (i >= argc_ - 1) {
                return TestCommandError("error: option [" + opt + "] requires a value.\n");
            }
            std::string argv = argv_[++i];
            params[opt] = argv;
        } else if (opt == "-w") {
            if (i >= argc_ - 1) {
                return TestCommandError("error: option [" + opt + "] requires a value.\n");
            }

            std::string argv = argv_[++i];
            std::smatch sm;
            auto isNumber = std::regex_match(argv, sm, std::regex(STRING_TEST_REGEX_INTEGER_NUMBERS));
            if (!isNumber) {
                return TestCommandError("error: option [" + opt + "] only supports integer numbers.\n");
            }

            params[opt] = argv;
        } else if (opt == "-s") {
            if (i >= argc_ - USER_TEST_COMMAND_PARAMS_NUM) {
                return TestCommandError("error: option [-s] is incorrect.\n");
            }
            std::string argKey = argv_[++i];
            std::string argValue = argv_[++i];
            params[opt + " " + argKey] = argValue;
        } else if (opt == "-D") {
            params[opt] = DEBUG_VALUE;
        } else if (opt.at(0) == '-') {
            return TestCommandError("error: unknown option: " + opt + "\n");
        }
    }

    if (!IsTestCommandIntegrity(params)) {
        return OHOS::ERR_INVALID_VALUE;
    }

    return StartUserTest(params);
}

bool AbilityManagerShellCommand::IsTestCommandIntegrity(const std::map<std::string, std::string> &params)
{
    HILOG_INFO("enter");

    std::vector<std::string> opts = {"-b", "-s unittest"};
    for (auto opt : opts) {
        auto it = params.find(opt);
        if (it == params.end()) {
            TestCommandError("error: the option [" + opt + "] is expected.\n");
            return false;
        }
    }
    return true;
}

ErrCode AbilityManagerShellCommand::TestCommandError(const std::string &info)
{
    resultReceiver_.append(info);
    resultReceiver_.append(HELP_MSG_TEST);
    return OHOS::ERR_INVALID_VALUE;
}

ErrCode AbilityManagerShellCommand::StartUserTest(const std::map<std::string, std::string> &params)
{
    HILOG_INFO("enter");

    Want want;
    for (auto param : params) {
        want.SetParam(param.first, param.second);
    }

    auto dPos = params.find("-D");
    if (dPos != params.end() && dPos->second.compare(DEBUG_VALUE) == 0) {
        HILOG_INFO("Set Debug to want");
        want.SetParam("debugApp", true);
    }

    sptr<TestObserver> observer = new (std::nothrow) TestObserver();
    if (!observer) {
        HILOG_ERROR("Failed: the TestObserver is null");
        return OHOS::ERR_INVALID_VALUE;
    }

    int result = AbilityManagerClient::GetInstance()->StartUserTest(want, observer->AsObject());
    if (result != OHOS::ERR_OK) {
        HILOG_INFO("%{public}s result = %{public}d", STRING_START_USER_TEST_NG.c_str(), result);
        resultReceiver_ = STRING_START_USER_TEST_NG + "\n";
        resultReceiver_.append(GetMessageFromCode(result));
        return result;
    }
    HILOG_INFO("%{public}s", STRING_USER_TEST_STARTED.c_str());

    std::signal(SIGCHLD, SIG_DFL);

    int64_t timeMs = 0;
    if (!want.GetStringParam("-w").empty()) {
        auto time = std::stoi(want.GetStringParam("-w"));
        timeMs = time > 0 ? time * TIME_RATE_MS : 0;
    }
    if (!observer->WaitForFinish(timeMs)) {
        resultReceiver_ = "Timeout: user test is not completed within the specified time.\n";
        return OHOS::ERR_INVALID_VALUE;
    }

    HILOG_INFO("%{public}s", STRING_USER_TEST_FINISHED.c_str());
    resultReceiver_ = STRING_USER_TEST_FINISHED + "\n";

    return result;
}

sptr<IAbilityManager> AbilityManagerShellCommand::GetAbilityManagerService()
{
    sptr<ISystemAbilityManager> systemManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemManager == nullptr) {
        HILOG_ERROR("Fail to get registry.");
        return nullptr;
    }
    sptr<IRemoteObject> remoteObject = systemManager->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
    return iface_cast<IAbilityManager>(remoteObject);
}

#ifdef ABILITY_COMMAND_FOR_TEST
ErrCode AbilityManagerShellCommand::RunAsSendAppNotRespondingWithUnknownOption()
{
    switch (optopt) {
        case 'h': {
            break;
        }
        case 'p': {
            HILOG_INFO("'aa ApplicationNotResponding -p' with no argument.");
            resultReceiver_.append("error: option -p ");
            resultReceiver_.append("' requires a value.\n");
            break;
        }
        default: {
            std::string unknownOption;
            std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
            HILOG_INFO("'aa ApplicationNotResponding' with an unknown option.");
            resultReceiver_.append(unknownOptionMsg);
            break;
        }
    }
    return OHOS::ERR_INVALID_VALUE;
}

ErrCode AbilityManagerShellCommand::RunAsSendAppNotRespondingWithOption(int32_t option, std::string &pid)
{
    ErrCode result = ERR_OK;
    switch (option) {
        case 'h': {
            result = OHOS::ERR_INVALID_VALUE;
            break;
        }
        case 'p': {
            HILOG_INFO("aa ApplicationNotResponding 'aa %{public}s'  -p process.", cmd_.c_str());
            HILOG_INFO("aa ApplicationNotResponding 'aa optarg =  %{public}s'.", optarg);
            pid = optarg;
            HILOG_INFO("aa ApplicationNotResponding 'aa pid =  %{public}s'.", pid.c_str());
            break;
        }
        default: {
            HILOG_INFO("'aa %{public}s' with an unknown option.", cmd_.c_str());
            result = OHOS::ERR_INVALID_VALUE;
            break;
        }
    }
    return result;
}

ErrCode AbilityManagerShellCommand::RunAsSendAppNotRespondingProcessID()
{
    static sptr<IAbilityManager> abilityMs_;
    std::string pid = "";
    int option = -1;
    ErrCode result = OHOS::ERR_OK;
    option = getopt_long(argc_, argv_, SHORT_OPTIONS_APPLICATION_NOT_RESPONDING.c_str(),
        LONG_OPTIONS_ApplicationNotResponding, nullptr);
    HILOG_INFO("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
    if (optind < 0 || optind > argc_) {
        return OHOS::ERR_INVALID_VALUE;
    }
    if (option == -1) {
        if (strcmp(argv_[optind], cmd_.c_str()) == 0) {
            HILOG_INFO("'aa %{public}s' %{public}s", HELP_ApplicationNotResponding.c_str(), cmd_.c_str());
            result = OHOS::ERR_INVALID_VALUE;
        }
    } else if (option == '?') {
        result = RunAsSendAppNotRespondingWithUnknownOption();
    } else {
        result = RunAsSendAppNotRespondingWithOption(option, pid);
    }

    if (result == OHOS::ERR_OK) {
        HILOG_INFO("'aa pid = %{public}d'.", atoi(pid.c_str()));
        abilityMs_ = GetAbilityManagerService();
        if (abilityMs_ == nullptr) {
            std::cout << "abilityMsObj is nullptr";
        } else {
            abilityMs_->SendANRProcessID(atoi(pid.c_str()));
        }
    } else {
        resultReceiver_.append(HELP_ApplicationNotResponding+ "\n");
        result = OHOS::ERR_INVALID_VALUE;
    }
    return result;
}

ErrCode AbilityManagerShellCommand::RunAsBlockAbilityCommand()
{
    HILOG_INFO("[%{public}s(%{public}s)] enter", __FILE__, __FUNCTION__);
    ErrCode result = OHOS::ERR_OK;
    if (argList_.size() > 0) {
        result = AbilityManagerClient::GetInstance()->BlockAbility(atoi(argList_[0].c_str()));
    } else {
        result = OHOS::ERR_INVALID_VALUE;
    }

    if (result == OHOS::ERR_OK) {
        HILOG_INFO("%{public}s", STRING_BLOCK_ABILITY_OK.c_str());
        resultReceiver_ = STRING_BLOCK_ABILITY_OK + "\n";
    } else {
        HILOG_INFO("%{public}s result = %{public}d", STRING_BLOCK_ABILITY_NG.c_str(), result);
        resultReceiver_ = STRING_BLOCK_ABILITY_NG + "\n";
        resultReceiver_.append(GetMessageFromCode(result));
    }
    return result;
}

ErrCode AbilityManagerShellCommand::RunAsBlockAmsServiceCommand()
{
    HILOG_INFO("[%{public}s(%{public}s)] enter", __FILE__, __FUNCTION__);
    ErrCode result = OHOS::ERR_OK;
    result = AbilityManagerClient::GetInstance()->BlockAmsService();
    if (result == OHOS::ERR_OK) {
        HILOG_INFO("%{public}s", STRING_BLOCK_AMS_SERVICE_OK.c_str());
        resultReceiver_ = STRING_BLOCK_AMS_SERVICE_OK + "\n";
    } else {
        HILOG_INFO("%{public}s result = %{public}d", STRING_BLOCK_AMS_SERVICE_NG.c_str(), result);
        resultReceiver_ = STRING_BLOCK_AMS_SERVICE_NG + "\n";
        resultReceiver_.append(GetMessageFromCode(result));
    }
    return result;
}

ErrCode AbilityManagerShellCommand::RunAsBlockAppServiceCommand()
{
    HILOG_INFO("[%{public}s(%{public}s)] enter", __FILE__, __FUNCTION__);
    ErrCode result = OHOS::ERR_OK;
    result = AbilityManagerClient::GetInstance()->BlockAppService();
    if (result == OHOS::ERR_OK) {
        HILOG_INFO("%{public}s", STRING_BLOCK_APP_SERVICE_OK.c_str());
        resultReceiver_ = STRING_BLOCK_APP_SERVICE_OK + "\n";
    } else {
        HILOG_INFO("%{public}s result = %{public}d", STRING_BLOCK_APP_SERVICE_NG.c_str(), result);
        resultReceiver_ = STRING_BLOCK_APP_SERVICE_NG + "\n";
        resultReceiver_.append(GetMessageFromCode(result));
    }
    return result;
}
#endif
}  // namespace AAFwk
}  // namespace OHOS
