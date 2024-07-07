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
#include "bundle_command.h"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <future>
#include <getopt.h>
#include <unistd.h>
#include <vector>

#include "app_log_wrapper.h"
#include "appexecfwk_errors.h"
#include "bundle_command_common.h"
#include "bundle_death_recipient.h"
#include "bundle_mgr_client.h"
#include "bundle_mgr_proxy.h"
#include "clean_cache_callback_host.h"
#include "parameter.h"
#include "quick_fix_command.h"
#include "status_receiver_impl.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string BUNDLE_NAME_EMPTY = "";
const int32_t INDEX_OFFSET = 2;
const int32_t MAX_WAITING_TIME = 3000;
const int32_t DEVICE_UDID_LENGTH = 65;
const int32_t MAX_ARGUEMENTS_NUMBER = 3;
const int32_t MINIMUM_WAITTING_TIME = 180; // 3 mins
const int32_t MAXIMUM_WAITTING_TIME = 600; // 10 mins

const std::string SHORT_OPTIONS = "hp:rn:m:a:cdu:w:";
const struct option LONG_OPTIONS[] = {
    {"help", no_argument, nullptr, 'h'},
    {"bundle-path", required_argument, nullptr, 'p'},
    {"replace", no_argument, nullptr, 'r'},
    {"bundle-name", required_argument, nullptr, 'n'},
    {"module-name", required_argument, nullptr, 'm'},
    {"ability-name", required_argument, nullptr, 'a'},
    {"bundle-info", no_argument, nullptr, 'i'},
    {"cache", no_argument, nullptr, 'c'},
    {"data", no_argument, nullptr, 'd'},
    {"is-removable", required_argument, nullptr, 'i'},
    {"user-id", required_argument, nullptr, 'u'},
    {"waitting-time", required_argument, nullptr, 'w'},
    {"keep-data", no_argument, nullptr, 'k'},
    {nullptr, 0, nullptr, 0},
};

const std::string SHORT_OPTIONS_DUMP = "hn:aisu:d:";
const struct option LONG_OPTIONS_DUMP[] = {
    {"help", no_argument, nullptr, 'h'},
    {"bundle-name", required_argument, nullptr, 'n'},
    {"all", no_argument, nullptr, 'a'},
    {"bundle-info", no_argument, nullptr, 'i'},
    {"shortcut-info", no_argument, nullptr, 's'},
    {"user-id", required_argument, nullptr, 'u'},
    {"device-id", required_argument, nullptr, 'd'},
    {nullptr, 0, nullptr, 0},
};

const std::string SHORT_OPTIONS_GET = "hu";
const struct option LONG_OPTIONS_GET[] = {
    {"help", no_argument, nullptr, 'h'},
    {"udid", no_argument, nullptr, 'u'},
    {nullptr, 0, nullptr, 0},
};
const std::string SHORT_OPTIONS_DUMP_DEPENDENCIES = "hn:m:";
const struct option LONG_OPTIONS_DUMP_DEPENDENCIES[] = {
    {"help", no_argument, nullptr, 'h'},
    {"bundle-name", required_argument, nullptr, 'n'},
    {"module-name", required_argument, nullptr, 'm'},
    {nullptr, 0, nullptr, 0},
};
}  // namespace

class CleanCacheCallbackImpl : public CleanCacheCallbackHost {
public:
    CleanCacheCallbackImpl() : signal_(std::make_shared<std::promise<bool>>())
    {}
    virtual ~CleanCacheCallbackImpl() override
    {}
    virtual void OnCleanCacheFinished(bool error) override;
    bool GetResultCode();
private:
    std::shared_ptr<std::promise<bool>> signal_;
    DISALLOW_COPY_AND_MOVE(CleanCacheCallbackImpl);
};

void CleanCacheCallbackImpl::OnCleanCacheFinished(bool error)
{
    if (signal_ != nullptr) {
        signal_->set_value(error);
    }
}

bool CleanCacheCallbackImpl::GetResultCode()
{
    if (signal_ != nullptr) {
        auto future = signal_->get_future();
        std::chrono::milliseconds span(MAX_WAITING_TIME);
        if (future.wait_for(span) == std::future_status::timeout) {
            return false;
        }
        return future.get();
    }
    return false;
}

BundleManagerShellCommand::BundleManagerShellCommand(int argc, char *argv[]) : ShellCommand(argc, argv, TOOL_NAME)
{}

ErrCode BundleManagerShellCommand::CreateCommandMap()
{
    commandMap_ = {
        {"help", std::bind(&BundleManagerShellCommand::RunAsHelpCommand, this)},
        {"install", std::bind(&BundleManagerShellCommand::RunAsInstallCommand, this)},
        {"uninstall", std::bind(&BundleManagerShellCommand::RunAsUninstallCommand, this)},
        {"dump", std::bind(&BundleManagerShellCommand::RunAsDumpCommand, this)},
        {"dump-dependencies", std::bind(&BundleManagerShellCommand::RunAsDumpDependenciesCommand, this)},
        {"clean", std::bind(&BundleManagerShellCommand::RunAsCleanCommand, this)},
        {"enable", std::bind(&BundleManagerShellCommand::RunAsEnableCommand, this)},
        {"disable", std::bind(&BundleManagerShellCommand::RunAsDisableCommand, this)},
        {"get", std::bind(&BundleManagerShellCommand::RunAsGetCommand, this)},
        {"quickfix", std::bind(&BundleManagerShellCommand::RunAsQuickFixCommand, this)},
    };

    return OHOS::ERR_OK;
}

ErrCode BundleManagerShellCommand::CreateMessageMap()
{
    messageMap_ = BundleCommandCommon::bundleMessageMap_;
    return OHOS::ERR_OK;
}

ErrCode BundleManagerShellCommand::Init()
{
    ErrCode result = OHOS::ERR_OK;

    if (bundleMgrProxy_ == nullptr) {
        bundleMgrProxy_ = BundleCommandCommon::GetBundleMgrProxy();
        if (bundleMgrProxy_) {
            if (bundleInstallerProxy_ == nullptr) {
                bundleInstallerProxy_ = bundleMgrProxy_->GetBundleInstaller();
            }
        }
    }

    if ((bundleMgrProxy_ == nullptr) || (bundleInstallerProxy_ == nullptr) ||
        (bundleInstallerProxy_->AsObject() == nullptr)) {
        result = OHOS::ERR_INVALID_VALUE;
    }

    return result;
}

ErrCode BundleManagerShellCommand::RunAsHelpCommand()
{
    resultReceiver_.append(HELP_MSG);

    return OHOS::ERR_OK;
}

ErrCode BundleManagerShellCommand::RunAsInstallCommand()
{
    int result = OHOS::ERR_OK;
    InstallFlag installFlag = InstallFlag::REPLACE_EXISTING;
    int counter = 0;
    std::vector<std::string> bundlePath;
    int index = 0;
    int32_t userId = Constants::ALL_USERID;
    int32_t waittingTime = MINIMUM_WAITTING_TIME;
    while (true) {
        counter++;
        int32_t option = getopt_long(argc_, argv_, SHORT_OPTIONS.c_str(), LONG_OPTIONS, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1) {
            if (counter == 1) {
                // When scanning the first argument
                if (strcmp(argv_[optind], cmd_.c_str()) == 0) {
                    // 'bm install' with no option: bm install
                    // 'bm install' with a wrong argument: bm install xxx
                    APP_LOGD("'bm install' with no option.");
                    resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                    result = OHOS::ERR_INVALID_VALUE;
                }
            }
            break;
        }

        if (option == '?') {
            switch (optopt) {
                case 'p': {
                    // 'bm install -p' with no argument: bm install -p
                    // 'bm install --bundle-path' with no argument: bm install --bundle-path
                    APP_LOGD("'bm install' with no argument.");
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 'u': {
                    // 'bm install -u' with no argument: bm install -u
                    // 'bm install --user-id' with no argument: bm install --user-id
                    APP_LOGD("'bm install -u' with no argument.");
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 'w': {
                    // 'bm install -w' with no argument: bm install -w
                    // 'bm install --waitting-time' with no argument: bm install --waitting-time
                    APP_LOGD("'bm install -w' with no argument.");
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                default: {
                    // 'bm install' with an unknown option: bm install -x
                    // 'bm install' with an unknown option: bm install -xxx
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
                    APP_LOGD("'bm install' with an unknown option.");
                    resultReceiver_.append(unknownOptionMsg);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
            }
            break;
        }

        switch (option) {
            case 'h': {
                // 'bm install -h'
                // 'bm install --help'
                APP_LOGD("'bm install %{public}s'", argv_[optind - 1]);
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            case 'p': {
                // 'bm install -p <bundle-file-path>'
                // 'bm install --bundle-path <bundle-file-path>'
                APP_LOGD("'bm install %{public}s'", argv_[optind - 1]);
                if (GetBundlePath(optarg, bundlePath) != OHOS::ERR_OK) {
                    APP_LOGD("'bm install' with no argument.");
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);

                    return OHOS::ERR_INVALID_VALUE;
                }
                index = optind;
                break;
            }
            case 'r': {
                // 'bm install -r'
                // 'bm install --replace'
                installFlag = InstallFlag::REPLACE_EXISTING;
                break;
            }
            case 'u': {
                // 'bm install -p <bundle-file-path> -u userId'
                // 'bm install --bundle-path <bundle-file-path> --user-id userId'
                APP_LOGD("'bm install %{public}s %{public}s'", argv_[optind - OFFSET_REQUIRED_ARGUMENT], optarg);
                if (!OHOS::StrToInt(optarg, userId) || userId < 0) {
                    APP_LOGE("bm install with error userId %{private}s", optarg);
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    return OHOS::ERR_INVALID_VALUE;
                }
                break;
            }
            case 'w': {
                APP_LOGD("'bm install %{public}s %{public}s'", argv_[optind - OFFSET_REQUIRED_ARGUMENT], optarg);
                if (!OHOS::StrToInt(optarg, waittingTime) || waittingTime < MINIMUM_WAITTING_TIME ||
                    waittingTime > MAXIMUM_WAITTING_TIME) {
                    APP_LOGE("bm install with error waittingTime %{private}s", optarg);
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    return OHOS::ERR_INVALID_VALUE;
                }
                break;
            }
            default: {
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
        }
    }

    for (; index < argc_ && index >= INDEX_OFFSET; ++index) {
        if (argList_[index - INDEX_OFFSET] == "-r" || argList_[index - INDEX_OFFSET] == "--replace" ||
            argList_[index - INDEX_OFFSET] == "-p" || argList_[index - INDEX_OFFSET] == "--bundle-path" ||
            argList_[index - INDEX_OFFSET] == "-u" || argList_[index - INDEX_OFFSET] == "--user-id" ||
            argList_[index - INDEX_OFFSET] == "-w" || argList_[index - INDEX_OFFSET] == "--waitting-time") {
            break;
        }
        if (GetBundlePath(argList_[index - INDEX_OFFSET], bundlePath) != OHOS::ERR_OK) {
            bundlePath.clear();
            APP_LOGD("'bm install' with error arguments.");
            resultReceiver_.append("error value for the chosen option");
            result = OHOS::ERR_INVALID_VALUE;
        }
    }

    for (auto &path : bundlePath) {
        APP_LOGD("install hap path %{private}s", path.c_str());
    }

    if (result == OHOS::ERR_OK) {
        if (resultReceiver_ == "" && bundlePath.empty()) {
            // 'bm install ...' with no bundle path option
            APP_LOGD("'bm install' with no bundle path option.");
            resultReceiver_.append(HELP_MSG_NO_BUNDLE_PATH_OPTION + "\n");
            result = OHOS::ERR_INVALID_VALUE;
        }
    }

    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_INSTALL);
    } else {
        InstallParam installParam;
        installParam.installFlag = installFlag;
        installParam.userId = userId;
        int32_t installResult = InstallOperation(bundlePath, installParam, waittingTime);
        if (installResult == OHOS::ERR_OK) {
            resultReceiver_ = STRING_INSTALL_BUNDLE_OK + "\n";
        } else {
            resultReceiver_ = STRING_INSTALL_BUNDLE_NG + "\n";
            resultReceiver_.append(GetMessageFromCode(installResult));
        }
    }

    return result;
}

ErrCode BundleManagerShellCommand::GetBundlePath(const std::string& param,
    std::vector<std::string>& bundlePaths) const
{
    if (param.empty()) {
        return OHOS::ERR_INVALID_VALUE;
    }
    if (param == "-r" || param == "--replace" || param == "-p" ||
        param == "--bundle-path" || param == "-u" || param == "--user-id" ||
        param == "-w" || param == "--waitting-time") {
        return OHOS::ERR_INVALID_VALUE;
    }
    bundlePaths.emplace_back(param);
    return OHOS::ERR_OK;
}

ErrCode BundleManagerShellCommand::RunAsUninstallCommand()
{
    int result = OHOS::ERR_OK;
    int counter = 0;
    std::string bundleName = "";
    std::string moduleName = "";
    int32_t userId = Constants::ALL_USERID;
    bool isKeepData = false;
    while (true) {
        counter++;
        int32_t option = getopt_long(argc_, argv_, SHORT_OPTIONS.c_str(), LONG_OPTIONS, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1) {
            if (counter == 1) {
                // When scanning the first argument
                if (strcmp(argv_[optind], cmd_.c_str()) == 0) {
                    // 'bm uninstall' with no option: bm uninstall
                    // 'bm uninstall' with a wrong argument: bm uninstall xxx
                    APP_LOGD("'bm uninstall' %{public}s", HELP_MSG_NO_OPTION.c_str());
                    resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                    result = OHOS::ERR_INVALID_VALUE;
                }
            }
            break;
        }

        if (option == '?') {
            switch (optopt) {
                case 'n': {
                    // 'bm uninstall -n' with no argument: bm uninstall -n
                    // 'bm uninstall --bundle-name' with no argument: bm uninstall --bundle-name
                    APP_LOGD("'bm uninstall -n' with no argument.");
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 'm': {
                    // 'bm uninstall -m' with no argument: bm uninstall -m
                    // 'bm uninstall --module-name' with no argument: bm uninstall --module-name
                    APP_LOGD("'bm uninstall -m' with no argument.");
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 'u': {
                    // 'bm uninstall -n <bundleName> -u userId'
                    // 'bm uninstall --bundle-name <bundleName> --user-id userId'
                    APP_LOGD("'bm uninstall -u' with no argument.");
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 'k': {
                    // 'bm uninstall -n <bundleName> -k'
                    // 'bm uninstall --bundle-name <bundleName> --keep-data'
                    APP_LOGD("'bm uninstall -k'");
                    isKeepData = true;
                    break;
                }
                default: {
                    // 'bm uninstall' with an unknown option: bm uninstall -x
                    // 'bm uninstall' with an unknown option: bm uninstall -xxx
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
                    APP_LOGD("'bm uninstall' with an unknown option.");
                    resultReceiver_.append(unknownOptionMsg);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
            }
            break;
        }

        switch (option) {
            case 'h': {
                // 'bm uninstall -h'
                // 'bm uninstall --help'
                APP_LOGD("'bm uninstall %{public}s'", argv_[optind - 1]);
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            case 'n': {
                // 'bm uninstall -n xxx'
                // 'bm uninstall --bundle-name xxx'
                APP_LOGD("'bm uninstall %{public}s %{public}s'", argv_[optind - OFFSET_REQUIRED_ARGUMENT], optarg);
                bundleName = optarg;
                break;
            }
            case 'm': {
                // 'bm uninstall -m xxx'
                // 'bm uninstall --module-name xxx'
                APP_LOGD("'bm uninstall %{public}s %{public}s'", argv_[optind - OFFSET_REQUIRED_ARGUMENT], optarg);
                moduleName = optarg;
                break;
            }
            case 'u': {
                // 'bm uninstall -n <bundleName> -u userId'
                // 'bm uninstall --bundle-name <bundleName> --user-id userId'
                APP_LOGD("'bm uninstall %{public}s %{public}s'", argv_[optind - OFFSET_REQUIRED_ARGUMENT], optarg);
                if (!OHOS::StrToInt(optarg, userId) || userId < 0) {
                    APP_LOGE("bm uninstall with error userId %{private}s", optarg);
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    return OHOS::ERR_INVALID_VALUE;
                }
                break;
            }
            case 'k': {
                // 'bm uninstall -n <bundleName> -k'
                // 'bm uninstall --bundle-name <bundleName> --keep-data'
                APP_LOGD("'bm uninstall %{public}s'", argv_[optind - OFFSET_REQUIRED_ARGUMENT]);
                isKeepData = true;
                break;
            }
            default: {
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
        }
    }

    if (result == OHOS::ERR_OK) {
        if (resultReceiver_ == "" && bundleName.size() == 0) {
            // 'bm uninstall ...' with no bundle name option
            APP_LOGD("'bm uninstall' with bundle name option.");
            resultReceiver_.append(HELP_MSG_NO_BUNDLE_NAME_OPTION + "\n");
            result = OHOS::ERR_INVALID_VALUE;
        }
    }

    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_UNINSTALL);
    } else {
        InstallParam installParam;
        installParam.userId = userId;
        installParam.isKeepData = isKeepData;
        int32_t uninstallResult = UninstallOperation(bundleName, moduleName, installParam);
        if (uninstallResult == OHOS::ERR_OK) {
            resultReceiver_ = STRING_UNINSTALL_BUNDLE_OK + "\n";
        } else {
            resultReceiver_ = STRING_UNINSTALL_BUNDLE_NG + "\n";
            resultReceiver_.append(GetMessageFromCode(uninstallResult));
        }
    }

    return result;
}

ErrCode BundleManagerShellCommand::RunAsDumpCommand()
{
    int result = OHOS::ERR_OK;
    int counter = 0;
    std::string bundleName = "";
    bool bundleDumpAll = false;
    bool bundleDumpInfo = false;
    bool bundleDumpShortcut = false;
    bool bundleDumpDistributedBundleInfo = false;
    std::string deviceId = "";
    int32_t userId = Constants::ALL_USERID;
    while (true) {
        counter++;
        int32_t option = getopt_long(argc_, argv_, SHORT_OPTIONS_DUMP.c_str(), LONG_OPTIONS_DUMP, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1) {
            if (counter == 1) {
                // When scanning the first argument
                if (strcmp(argv_[optind], cmd_.c_str()) == 0) {
                    // 'bm dump' with no option: bm dump
                    // 'bm dump' with a wrong argument: bm dump xxx
                    APP_LOGD("'bm dump' %{public}s", HELP_MSG_NO_OPTION.c_str());
                    resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                    result = OHOS::ERR_INVALID_VALUE;
                }
            }
            break;
        }
        if (option == '?') {
            switch (optopt) {
                case 'n': {
                    // 'bm dump -n' with no argument: bm dump -n
                    // 'bm dump --bundle-name' with no argument: bm dump --bundle-name
                    APP_LOGD("'bm dump -n' with no argument.");
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 'u': {
                    // 'bm dump -u' with no argument: bm dump -u
                    // 'bm dump --user-id' with no argument: bm dump --user-id
                    APP_LOGD("'bm dump -u' with no argument.");
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 'd': {
                    // 'bm dump -d' with no argument: bm dump -d
                    // 'bm dump --device-id' with no argument: bm dump --device-id
                    APP_LOGD("'bm dump -d' with no argument.");
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                default: {
                    // 'bm dump' with an unknown option: bm dump -x
                    // 'bm dump' with an unknown option: bm dump -xxx
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
                    APP_LOGD("'bm dump' with an unknown option.");
                    resultReceiver_.append(unknownOptionMsg);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
            }
            break;
        }
        switch (option) {
            case 'h': {
                // 'bm dump -h'
                // 'bm dump --help'
                APP_LOGD("'bm dump %{public}s'", argv_[optind - 1]);
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            case 'a': {
                // 'bm dump -a'
                // 'bm dump --all'
                APP_LOGD("'bm dump %{public}s'", argv_[optind - 1]);
                bundleDumpAll = true;
                break;
            }
            case 'n': {
                // 'bm dump -n xxx'
                // 'bm dump --bundle-name xxx'
                APP_LOGD("'bm dump %{public}s %{public}s'", argv_[optind - OFFSET_REQUIRED_ARGUMENT], optarg);
                bundleName = optarg;
                bundleDumpInfo = true;
                break;
            }
            case 's': {
                // 'bm dump -n xxx -s'
                // 'bm dump --bundle-name xxx --shortcut-info'
                APP_LOGD("'bm dump %{public}s %{public}s'", argv_[optind - OFFSET_REQUIRED_ARGUMENT], optarg);
                bundleDumpShortcut = true;
                break;
            }
            case 'u': {
                // 'bm dump -n <bundleName> -u userId'
                // 'bm dump --bundle-name <bundleName> --user-id userId'
                APP_LOGD("'bm dump %{public}s %{public}s'", argv_[optind - OFFSET_REQUIRED_ARGUMENT], optarg);
                if (!OHOS::StrToInt(optarg, userId) || userId < 0) {
                    APP_LOGE("bm dump with error userId %{private}s", optarg);
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    return OHOS::ERR_INVALID_VALUE;
                }
                break;
            }
            case 'd': {
                // 'bm dump -n <bundleName> -d deviceId'
                // 'bm dump --bundle-name <bundleName> --device-id deviceId'
                APP_LOGD("'bm dump %{public}s %{public}s'", argv_[optind - OFFSET_REQUIRED_ARGUMENT], optarg);
                deviceId = optarg;
                bundleDumpDistributedBundleInfo = true;
                break;
            }
            default: {
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
        }
    }
    if (result == OHOS::ERR_OK) {
        if ((resultReceiver_ == "") && bundleDumpShortcut && (bundleName.size() == 0)) {
            // 'bm dump -s ...' with no bundle name option
            APP_LOGD("'bm dump -s' with no bundle name option.");
            resultReceiver_.append(HELP_MSG_NO_BUNDLE_NAME_OPTION + "\n");
            result = OHOS::ERR_INVALID_VALUE;
        }
        if ((resultReceiver_ == "") && bundleDumpDistributedBundleInfo && (bundleName.size() == 0)) {
            // 'bm dump d ...' with no bundle name option
            APP_LOGD("'bm dump -d' with no bundle name option.");
            resultReceiver_.append(HELP_MSG_NO_BUNDLE_NAME_OPTION + "\n");
            result = OHOS::ERR_INVALID_VALUE;
        }
    }
    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_DUMP);
    } else {
        std::string dumpResults = "";
        APP_LOGD("dumpResults: %{public}s", dumpResults.c_str());
        if (bundleDumpShortcut) {
            dumpResults = DumpShortcutInfos(bundleName, userId);
        } else if (bundleDumpDistributedBundleInfo) {
            dumpResults = DumpDistributedBundleInfo(deviceId, bundleName);
        } else if (bundleDumpAll) {
            dumpResults = DumpBundleList(userId);
        } else if (bundleDumpInfo) {
            dumpResults = DumpBundleInfo(bundleName, userId);
        }
        if (dumpResults.empty() || (dumpResults == "")) {
            dumpResults = HELP_MSG_DUMP_FAILED + "\n";
        }
        resultReceiver_.append(dumpResults);
    }

    return result;
}

ErrCode BundleManagerShellCommand::RunAsDumpDependenciesCommand()
{
    int32_t result = OHOS::ERR_OK;
    int32_t counter = 0;
    std::string bundleName;
    std::string moduleName;
    while (true) {
        counter++;
        int32_t option = getopt_long(argc_, argv_, SHORT_OPTIONS_DUMP_DEPENDENCIES.c_str(),
            LONG_OPTIONS_DUMP_DEPENDENCIES, nullptr);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1) {
            if (counter == 1) {
                // When scanning the first argument
                if (strcmp(argv_[optind], cmd_.c_str()) == 0) {
                    // 'bm dump-dependencies' with no option: bm dump-dependencies
                    // 'bm dump-dependencies' with a wrong argument: bm dump-dependencies xxx
                    resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                    result = OHOS::ERR_INVALID_VALUE;
                }
            }
            break;
        }
        result = ParseDependenciesCommand(option, bundleName, moduleName);
        if (option == '?') {
            break;
        }
    }
    if (result == OHOS::ERR_OK) {
        if ((resultReceiver_ == "") && (bundleName.size() == 0 || moduleName.size() == 0)) {
            // 'bm dump-dependencies -n -m ...' with no bundle name option
            resultReceiver_.append(HELP_MSG_NO_REMOVABLE_OPTION);
            result = OHOS::ERR_INVALID_VALUE;
        }
    }
    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_DUMP_DEPENDENCIES);
    } else {
        std::string dumpResults = DumpDependentModuleNames(bundleName, moduleName);
        if (dumpResults.empty() || (dumpResults == "")) {
            dumpResults = HELP_MSG_DUMP_FAILED + "\n";
        }
        resultReceiver_.append(dumpResults);
    }
    return result;
}

ErrCode BundleManagerShellCommand::ParseDependenciesCommand(int32_t option, std::string &bundleName,
    std::string &moduleName)
{
    int32_t result = OHOS::ERR_OK;
    if (option == '?') {
        switch (optopt) {
            case 'n': {
                // 'bm dump-dependencies -n' with no argument: bm dump-dependencies -n
                // 'bm dump-dependencies --bundle-name' with no argument: bm dump-dependencies --bundle-name
                resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            case 'm': {
                // 'bm dump-dependencies -m' with no argument: bm dump-dependencies -m
                // 'bm dump-dependencies --module-name' with no argument: bm dump-dependencies --module-name
                resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            default: {
                // 'bm dump-dependencies' with an unknown option: bm dump-dependencies -x
                // 'bm dump-dependencies' with an unknown option: bm dump-dependencies -xxx
                std::string unknownOption = "";
                std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
                resultReceiver_.append(unknownOptionMsg);
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
        }
    } else {
        switch (option) {
            case 'h': {
                // 'bm dump-dependencies -h'
                // 'bm dump-dependencies --help'
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            case 'n': {
                // 'bm dump-dependencies -n xxx'
                // 'bm dump-dependencies --bundle-name xxx'
                bundleName = optarg;
                break;
            }
            case 'm': {
                // 'bm dump-dependencies -m xxx'
                // 'bm dump-dependencies --module-name xxx'
                moduleName = optarg;
                break;
            }
            default: {
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
        }
    }
    return result;
}

ErrCode BundleManagerShellCommand::RunAsCleanCommand()
{
    int32_t result = OHOS::ERR_OK;
    int32_t counter = 0;
    int32_t userId = Constants::UNSPECIFIED_USERID;
    bool cleanCache = false;
    bool cleanData = false;
    std::string bundleName = "";
    while (true) {
        counter++;
        int32_t option = getopt_long(argc_, argv_, SHORT_OPTIONS.c_str(), LONG_OPTIONS, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1) {
            if (counter == 1) {
                // When scanning the first argument
                if (strcmp(argv_[optind], cmd_.c_str()) == 0) {
                    // 'bm clean' with no option: bm clean
                    // 'bm clean' with a wrong argument: bm clean xxx
                    APP_LOGD("'bm clean' %{public}s", HELP_MSG_NO_OPTION.c_str());

                    resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                    result = OHOS::ERR_INVALID_VALUE;
                }
            }
            break;
        }

        if (option == '?') {
            switch (optopt) {
                case 'n': {
                    // 'bm clean -n' with no argument: bm clean -n
                    // 'bm clean --bundle-name' with no argument: bm clean --bundle-name
                    APP_LOGD("'bm clean -n' with no argument.");
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 'u': {
                    // 'bm clean -u' with no argument: bm clean -u
                    // 'bm clean --user-id' with no argument: bm clean --user-id
                    APP_LOGD("'bm clean -u' with no argument.");
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                default: {
                    // 'bm clean' with an unknown option: bm clear -x
                    // 'bm clean' with an unknown option: bm clear -xxx
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
                    APP_LOGD("'bm clean' with an unknown option.");
                    resultReceiver_.append(unknownOptionMsg);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
            }
            break;
        }

        switch (option) {
            case 'h': {
                // 'bm clean -h'
                // 'bm clean --help'
                APP_LOGD("'bm clean %{public}s'", argv_[optind - 1]);
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            case 'n': {
                // 'bm clean -n xxx'
                // 'bm clean --bundle-name xxx'
                APP_LOGD("'bm clean %{public}s %{public}s'", argv_[optind - OFFSET_REQUIRED_ARGUMENT], optarg);
                bundleName = optarg;
                break;
            }
            case 'c': {
                // 'bm clean -c'
                // 'bm clean --cache'
                APP_LOGD("'bm clean %{public}s'", argv_[optind - OFFSET_REQUIRED_ARGUMENT]);
                cleanCache = cleanData ? false : true;
                break;
            }
            case 'd': {
                // 'bm clean -d'
                // 'bm clean --data'
                APP_LOGD("'bm clean %{public}s '", argv_[optind - OFFSET_REQUIRED_ARGUMENT]);
                cleanData = cleanCache ? false : true;
                break;
            }
            case 'u': {
                // 'bm clean -u userId'
                // 'bm clean --user-id userId'
                APP_LOGD("'bm clean %{public}s %{public}s'", argv_[optind - OFFSET_REQUIRED_ARGUMENT], optarg);
                if (!OHOS::StrToInt(optarg, userId) || userId < 0) {
                    APP_LOGE("bm clean with error userId %{private}s", optarg);
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    return OHOS::ERR_INVALID_VALUE;
                }
                break;
            }
            default: {
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
        }
    }

    if (result == OHOS::ERR_OK) {
        if (resultReceiver_ == "" && bundleName.size() == 0) {
            // 'bm clean ...' with no bundle name option
            APP_LOGD("'bm clean' with no bundle name option.");
            resultReceiver_.append(HELP_MSG_NO_BUNDLE_NAME_OPTION + "\n");
            result = OHOS::ERR_INVALID_VALUE;
        }
        if (!cleanCache && !cleanData) {
            APP_LOGD("'bm clean' with no '-c' or '-d' option.");
            resultReceiver_.append(HELP_MSG_NO_DATA_OR_CACHE_OPTION + "\n");
            result = OHOS::ERR_INVALID_VALUE;
        }
    }

    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_CLEAN);
    } else {
        // bm clean -c
        if (cleanCache) {
            if (CleanBundleCacheFilesOperation(bundleName, userId)) {
                resultReceiver_ = STRING_CLEAN_CACHE_BUNDLE_OK + "\n";
            } else {
                resultReceiver_ = STRING_CLEAN_CACHE_BUNDLE_NG + "\n";
            }
        }
        // bm clean -d
        if (cleanData) {
            if (CleanBundleDataFilesOperation(bundleName, userId)) {
                resultReceiver_.append(STRING_CLEAN_DATA_BUNDLE_OK + "\n");
            } else {
                resultReceiver_.append(STRING_CLEAN_DATA_BUNDLE_NG + "\n");
            }
        }
    }
    return result;
}

ErrCode BundleManagerShellCommand::RunAsEnableCommand()
{
    int result = OHOS::ERR_OK;
    int counter = 0;
    std::string bundleName = "";
    std::string abilityName = "";
    int32_t userId = Constants::UNSPECIFIED_USERID;
    while (true) {
        counter++;
        int32_t option = getopt_long(argc_, argv_, SHORT_OPTIONS.c_str(), LONG_OPTIONS, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }

        if (option == -1) {
            if (counter == 1) {
                // When scanning the first argument
                if (strcmp(argv_[optind], cmd_.c_str()) == 0) {
                    // 'bm enable' with no option: bm enable
                    // 'bm enable' with a wrong argument: bm enable xxx
                    APP_LOGD("'bm enable' with no option.");
                    resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                    result = OHOS::ERR_INVALID_VALUE;
                }
            }
            break;
        }

        if (option == '?') {
            switch (optopt) {
                case 'n': {
                    // 'bm enable -n' with no argument: bm enable -n
                    // 'bm enable --bundle-name' with no argument: bm enable --bundle-name
                    APP_LOGD("'bm enable -n' with no argument.");
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 'a': {
                    // 'bm enable -a' with no argument: bm enable -a
                    // 'bm enable --ability-name' with no argument: bm enable --ability-name
                    APP_LOGD("'bm enable -a' with no argument.");
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 'u': {
                    // 'bm enable -u' with no argument: bm enable -u
                    // 'bm enable --user-id' with no argument: bm enable --user-id
                    APP_LOGD("'bm enable -u' with no argument.");
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                default: {
                    // 'bm enable' with an unknown option: bm enable -x
                    // 'bm enable' with an unknown option: bm enable -xxx
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
                    APP_LOGD("'bm enable' with an unknown option.");
                    resultReceiver_.append(unknownOptionMsg);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
            }
            break;
        }

        switch (option) {
            case 'h': {
                // 'bm enable-h'
                // 'bm enable --help'
                APP_LOGD("'bm enable %{public}s'", argv_[optind - 1]);
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            case 'n': {
                // 'bm enable -n <bundle-name>'
                // 'bm enable --bundle-name <bundle-name>'
                bundleName = optarg;
                break;
            }
            case 'a': {
                // 'bm enable -a <ability-name>'
                // 'bm enable --ability-name <ability-name>'
                abilityName = optarg;
                break;
            }
            case 'u': {
                // 'bm enable -u userId'
                // 'bm enable --user-id userId'
                APP_LOGD("'bm enable %{public}s %{public}s'", argv_[optind - OFFSET_REQUIRED_ARGUMENT], optarg);
                if (!OHOS::StrToInt(optarg, userId) || userId < 0) {
                    APP_LOGE("bm enable with error userId %{private}s", optarg);
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    return OHOS::ERR_INVALID_VALUE;
                }
                break;
            }
            default: {
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
        }
    }

    if (result == OHOS::ERR_OK) {
        if (resultReceiver_ == "" && bundleName.size() == 0) {
            // 'bm enable ...' with no bundle name option
            APP_LOGD("'bm enable' with no bundle name option.");

            resultReceiver_.append(HELP_MSG_NO_BUNDLE_NAME_OPTION + "\n");
            result = OHOS::ERR_INVALID_VALUE;
        }
    }

    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_ENABLE);
    } else {
        AbilityInfo abilityInfo;
        abilityInfo.name = abilityName;
        abilityInfo.bundleName = bundleName;
        bool enableResult = SetApplicationEnabledOperation(abilityInfo, true, userId);
        if (enableResult) {
            resultReceiver_ = STRING_ENABLE_BUNDLE_OK + "\n";
        } else {
            resultReceiver_ = STRING_ENABLE_BUNDLE_NG + "\n";
        }
    }
    return result;
}

ErrCode BundleManagerShellCommand::RunAsDisableCommand()
{
    int result = OHOS::ERR_OK;
    int counter = 0;
    std::string bundleName = "";
    std::string abilityName = "";
    int32_t userId = Constants::UNSPECIFIED_USERID;
    while (true) {
        counter++;
        int32_t option = getopt_long(argc_, argv_, SHORT_OPTIONS.c_str(), LONG_OPTIONS, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1) {
            if (counter == 1) {
                // When scanning the first argument
                if (strcmp(argv_[optind], cmd_.c_str()) == 0) {
                    // 'bm disable' with no option: bm disable
                    // 'bm disable' with a wrong argument: bm disable xxx
                    APP_LOGD("'bm disable' with no option.");
                    resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                    result = OHOS::ERR_INVALID_VALUE;
                }
            }
            break;
        }
        if (option == '?') {
            switch (optopt) {
                case 'n': {
                    // 'bm disable -n' with no argument: bm disable -n
                    // 'bm disable --bundle-name' with no argument: bm disable --bundle-name
                    APP_LOGD("'bm disable' with no argument.");
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 'a': {
                    // 'bm disable -a' with no argument: bm disable -a
                    // 'bm disable --ability-name' with no argument: bm disable --ability-name
                    APP_LOGD("'bm disable -a' with no argument.");
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 'u': {
                    // 'bm disable -u' with no argument: bm disable -u
                    // 'bm disable --user-id' with no argument: bm disable --user-id
                    APP_LOGD("'bm disable -u' with no argument.");
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                default: {
                    // 'bm disable' with an unknown option: bm disable -x
                    // 'bm disable' with an unknown option: bm disable -xxx
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
                    APP_LOGD("'bm disable' with an unknown option.");
                    resultReceiver_.append(unknownOptionMsg);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
            }
            break;
        }
        switch (option) {
            case 'h': {
                // 'bm disable -h'
                // 'bm disable --help'
                APP_LOGD("'bm disable %{public}s'", argv_[optind - 1]);
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            case 'n': {
                // 'bm disable -n <bundle-name>'
                // 'bm disable --bundle-name <bundle-name>'
                bundleName = optarg;
                break;
            }
            case 'a': {
                // 'bm disable -a <ability-name>'
                // 'bm disable --ability-name <ability-name>'
                abilityName = optarg;
                break;
            }
            case 'u': {
                // 'bm disable -u userId'
                // 'bm disable --user-id userId'
                APP_LOGD("'bm disable %{public}s %{public}s'", argv_[optind - OFFSET_REQUIRED_ARGUMENT], optarg);
                if (!OHOS::StrToInt(optarg, userId) || userId < 0) {
                    APP_LOGE("bm disable with error userId %{private}s", optarg);
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    return OHOS::ERR_INVALID_VALUE;
                }
                break;
            }
            default: {
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
        }
    }
    if (result == OHOS::ERR_OK) {
        if (resultReceiver_ == "" && bundleName.size() == 0) {
            // 'bm disable ...' with no bundle name option
            APP_LOGD("'bm disable' with no bundle name option.");
            resultReceiver_.append(HELP_MSG_NO_BUNDLE_NAME_OPTION + "\n");
            result = OHOS::ERR_INVALID_VALUE;
        }
    }
    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_DISABLE);
    } else {
        AbilityInfo abilityInfo;
        abilityInfo.name = abilityName;
        abilityInfo.bundleName = bundleName;
        bool enableResult = SetApplicationEnabledOperation(abilityInfo, false, userId);
        if (enableResult) {
            resultReceiver_ = STRING_DISABLE_BUNDLE_OK + "\n";
        } else {
            resultReceiver_ = STRING_DISABLE_BUNDLE_NG + "\n";
        }
    }
    return result;
}

ErrCode BundleManagerShellCommand::RunAsGetCommand()
{
    int result = OHOS::ERR_OK;
    int counter = 0;
    while (true) {
        counter++;
        if (argc_ > MAX_ARGUEMENTS_NUMBER) {
            resultReceiver_.append(HELP_MSG_GET);
            return OHOS::ERR_INVALID_VALUE;
        }
        int32_t option = getopt_long(argc_, argv_, SHORT_OPTIONS_GET.c_str(), LONG_OPTIONS_GET, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1) {
            if (counter == 1) {
                if (strcmp(argv_[optind], cmd_.c_str()) == 0) {
                    // 1.'bm get' with no option: bm get
                    // 2.'bm get' with a wrong argument: bm get -xxx
                    APP_LOGD("'bm get' %{public}s", HELP_MSG_NO_OPTION.c_str());
                    resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                    result = OHOS::ERR_INVALID_VALUE;
                }
            }
            break;
        }
        switch (option) {
            case 'h': {
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            case 'u': {
                break;
            }
            default: {
                result = OHOS::ERR_INVALID_VALUE;
                resultReceiver_.append(STRING_INCORRECT_OPTION + "\n");
                break;
            }
        }
    }
    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_GET);
        return result;
    }
    resultReceiver_.append(STRING_GET_UDID_OK + "\n");
    resultReceiver_.append(GetUdid() + "\n");
    return result;
}

ErrCode BundleManagerShellCommand::RunAsQuickFixCommand()
{
    for (auto index = INDEX_OFFSET; index < argc_; ++index) {
        APP_LOGD("argv_[%{public}d]: %{public}s", index, argv_[index]);
        std::string opt = argv_[index];
        if ((opt == "-h") || (opt == "--help")) {
            resultReceiver_.append(HELP_MSG_QUICK_FIX);
            return ERR_OK;
        } else if ((opt == "-a") || (opt == "--apply")) {
            if (index >= argc_ - INDEX_OFFSET) {
                resultReceiver_.append("error: option [--apply] is incorrect.\n");
                resultReceiver_.append(HELP_MSG_QUICK_FIX);
                return ERR_INVALID_VALUE;
            }

            std::string argKey = argv_[++index];
            index++;
            if (argKey == "-f" || argKey == "--file-path") {
                std::vector<std::string> quickFixFiles;
                // collect value of multi file-path.
                for (; index < argc_ && index >= INDEX_OFFSET; ++index) {
                    if (argList_[index - INDEX_OFFSET] == "-q" || argList_[index - INDEX_OFFSET] == "--query" ||
                        argList_[index - INDEX_OFFSET] == "-b" || argList_[index - INDEX_OFFSET] == "--bundle-name" ||
                        argList_[index - INDEX_OFFSET] == "-a" || argList_[index - INDEX_OFFSET] == "--apply" ||
                        argList_[index - INDEX_OFFSET] == "-f" || argList_[index - INDEX_OFFSET] == "--file-path") {
                        break;
                    }
                    quickFixFiles.emplace_back(argList_[index - INDEX_OFFSET]);
                }

                return QuickFixCommand::ApplyQuickFix(quickFixFiles, resultReceiver_);
            }
        } else if ((opt == "-q") || (opt == "--query")) {
            if (index >= argc_ - INDEX_OFFSET) {
                resultReceiver_.append("error: option [--query] is incorrect.\n");
                resultReceiver_.append(HELP_MSG_QUICK_FIX);
                return ERR_INVALID_VALUE;
            }

            std::string bundleName;
            std::string argKey = argv_[++index];
            std::string argValue = argv_[++index];
            if (argKey == "-b" || argKey == "--bundle-name") {
                bundleName = argValue;
            }

            return QuickFixCommand::GetApplyedQuickFixInfo(bundleName, resultReceiver_);
        } else {
            resultReceiver_.append("error: unknown option.\n");
            resultReceiver_.append(HELP_MSG_QUICK_FIX);
            return ERR_INVALID_VALUE;
        }
    }

    resultReceiver_.append("error: parameter is not enough.\n");
    resultReceiver_.append(HELP_MSG_QUICK_FIX);
    return ERR_INVALID_VALUE;
}

std::string BundleManagerShellCommand::GetUdid() const
{
    char innerUdid[DEVICE_UDID_LENGTH] = { 0 };
    int ret = GetDevUdid(innerUdid, DEVICE_UDID_LENGTH);
    if (ret != 0) {
        APP_LOGE("GetUdid failed! ret = %{public}d.", ret);
        return STRING_GET_UDID_NG;
    }
    std::string udid = innerUdid;
    return udid;
}

std::string BundleManagerShellCommand::DumpBundleList(int32_t userId) const
{
    std::string dumpResults;
    bool dumpRet = bundleMgrProxy_->DumpInfos(
        DumpFlag::DUMP_BUNDLE_LIST, BUNDLE_NAME_EMPTY, userId, dumpResults);
    if (!dumpRet) {
        APP_LOGE("failed to dump bundle list.");
    }
    return dumpResults;
}

std::string BundleManagerShellCommand::DumpBundleInfo(const std::string &bundleName, int32_t userId) const
{
    std::string dumpResults;
    bool dumpRet = bundleMgrProxy_->DumpInfos(
        DumpFlag::DUMP_BUNDLE_INFO, bundleName, userId, dumpResults);
    if (!dumpRet) {
        APP_LOGE("failed to dump bundle info.");
    }
    return dumpResults;
}

std::string BundleManagerShellCommand::DumpShortcutInfos(const std::string &bundleName, int32_t userId) const
{
    std::string dumpResults;
    bool dumpRet = bundleMgrProxy_->DumpInfos(
        DumpFlag::DUMP_SHORTCUT_INFO, bundleName, userId, dumpResults);
    if (!dumpRet) {
        APP_LOGE("failed to dump shortcut infos.");
    }
    return dumpResults;
}

std::string BundleManagerShellCommand::DumpDistributedBundleInfo(
    const std::string &deviceId, const std::string &bundleName)
{
    std::string dumpResults = "";
    DistributedBundleInfo distributedBundleInfo;
    bool dumpRet = bundleMgrProxy_->GetDistributedBundleInfo(deviceId, bundleName, distributedBundleInfo);
    if (!dumpRet) {
        APP_LOGE("failed to dump distributed bundleInfo.");
    } else {
        dumpResults.append("distributed bundleInfo");
        dumpResults.append(":\n");
        dumpResults.append(distributedBundleInfo.ToString());
        dumpResults.append("\n");
    }
    return dumpResults;
}

std::string BundleManagerShellCommand::DumpDependentModuleNames(
    const std::string &bundleName,
    const std::string &moduleName) const
{
    APP_LOGD("DumpDependentModuleNames bundleName: %{public}s, moduleName: %{public}s",
        bundleName.c_str(), moduleName.c_str());
    std::string dumpResults = "";
    std::vector<std::string> dependentModuleNames;
    bool dumpRet = bundleMgrProxy_->GetAllDependentModuleNames(bundleName, moduleName, dependentModuleNames);
    if (!dumpRet) {
        APP_LOGE("failed to dump dependent module name.");
    } else {
        dumpResults.append("dependent moduleNames:");
        for (const auto &name : dependentModuleNames) {
            dumpResults.append("\n");
            dumpResults.append(name);
        }
        dumpResults.append("\n");
    }
    return dumpResults;
}

int32_t BundleManagerShellCommand::InstallOperation(const std::vector<std::string> &bundlePaths,
    InstallParam &installParam, int32_t waittingTime) const
{
    std::vector<std::string> realPathVec;
    for (auto &bundlePath : bundlePaths) {
        std::string absoluteBundlePath = "";
        if (bundlePath.empty()) {
            continue;
        }
        if (bundlePath.at(0) == '/') {
            // absolute path
            absoluteBundlePath.append(bundlePath);
        } else {
            // relative path
            char *currentPathPtr = getcwd(nullptr, 0);

            if (currentPathPtr != nullptr) {
                absoluteBundlePath.append(currentPathPtr);
                absoluteBundlePath.append('/' + bundlePath);

                free(currentPathPtr);
                currentPathPtr = nullptr;
            }
        }
        realPathVec.emplace_back(absoluteBundlePath);
    }
    std::vector<std::string> pathVec;
    for (const auto &path : realPathVec) {
        if (std::find(pathVec.begin(), pathVec.end(), path) == pathVec.end()) {
            pathVec.emplace_back(path);
        }
    }
    sptr<StatusReceiverImpl> statusReceiver(new (std::nothrow) StatusReceiverImpl(waittingTime));
    if (statusReceiver == nullptr) {
        APP_LOGE("statusReceiver is null");
        return IStatusReceiver::ERR_UNKNOWN;
    }
    sptr<BundleDeathRecipient> recipient(new (std::nothrow) BundleDeathRecipient(statusReceiver));
    if (recipient == nullptr) {
        APP_LOGE("recipient is null");
        return IStatusReceiver::ERR_UNKNOWN;
    }
    bundleInstallerProxy_->AsObject()->AddDeathRecipient(recipient);
    ErrCode res = bundleInstallerProxy_->StreamInstall(pathVec, installParam, statusReceiver);
    APP_LOGD("StreamInstall result is %{public}d", res);
    if (res == ERR_OK) {
        return statusReceiver->GetResultCode();
    }
    if (res == ERR_APPEXECFWK_INSTALL_PARAM_ERROR) {
        APP_LOGE("install param error");
        return IStatusReceiver::ERR_INSTALL_PARAM_ERROR;
    }
    if (res == ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR) {
        APP_LOGE("install internal error");
        return IStatusReceiver::ERR_INSTALL_INTERNAL_ERROR;
    }
    if (res == ERR_APPEXECFWK_INSTALL_FILE_PATH_INVALID) {
        APP_LOGE("install invalid path");
        return IStatusReceiver::ERR_INSTALL_FILE_PATH_INVALID;
    }
    if (res == ERR_APPEXECFWK_INSTALL_DISK_MEM_INSUFFICIENT) {
        APP_LOGE("install failed due to no space left");
        return IStatusReceiver::ERR_INSTALL_DISK_MEM_INSUFFICIENT;
    }

    return res;
}

int32_t BundleManagerShellCommand::UninstallOperation(
    const std::string &bundleName, const std::string &moduleName, InstallParam &installParam) const
{
    sptr<StatusReceiverImpl> statusReceiver(new (std::nothrow) StatusReceiverImpl());
    if (statusReceiver == nullptr) {
        APP_LOGE("statusReceiver is null");
        return IStatusReceiver::ERR_UNKNOWN;
    }

    APP_LOGD("bundleName: %{public}s", bundleName.c_str());
    APP_LOGD("moduleName: %{public}s", moduleName.c_str());

    sptr<BundleDeathRecipient> recipient(new (std::nothrow) BundleDeathRecipient(statusReceiver));
    if (recipient == nullptr) {
        APP_LOGE("recipient is null");
        return IStatusReceiver::ERR_UNKNOWN;
    }
    bundleInstallerProxy_->AsObject()->AddDeathRecipient(recipient);
    if (moduleName.size() != 0) {
        bundleInstallerProxy_->Uninstall(bundleName, moduleName, installParam, statusReceiver);
    } else {
        bundleInstallerProxy_->Uninstall(bundleName, installParam, statusReceiver);
    }

    return statusReceiver->GetResultCode();
}

bool BundleManagerShellCommand::CleanBundleCacheFilesOperation(const std::string &bundleName, int32_t userId) const
{
    userId = BundleCommandCommon::GetCurrentUserId(userId);
    sptr<CleanCacheCallbackImpl> cleanCacheCallBack(new (std::nothrow) CleanCacheCallbackImpl());
    if (cleanCacheCallBack == nullptr) {
        APP_LOGE("cleanCacheCallBack is null");
        return false;
    }
    ErrCode cleanRet = bundleMgrProxy_->CleanBundleCacheFiles(bundleName, cleanCacheCallBack, userId);
    if (cleanRet == ERR_OK) {
        return cleanCacheCallBack->GetResultCode();
    }
    APP_LOGE("clean bundle cache files operation failed");
    return false;
}

bool BundleManagerShellCommand::CleanBundleDataFilesOperation(const std::string &bundleName, int32_t userId) const
{
    APP_LOGD("bundleName: %{public}s, userId:%{public}d", bundleName.c_str(), userId);
    userId = BundleCommandCommon::GetCurrentUserId(userId);
    bool cleanRet = bundleMgrProxy_->CleanBundleDataFiles(bundleName, userId);
    if (!cleanRet) {
        APP_LOGE("clean bundle data files operation failed");
    }
    return cleanRet;
}

bool BundleManagerShellCommand::SetApplicationEnabledOperation(const AbilityInfo &abilityInfo,
    bool isEnable, int32_t userId) const
{
    APP_LOGD("bundleName: %{public}s", abilityInfo.bundleName.c_str());
    userId = BundleCommandCommon::GetCurrentUserId(userId);
    int32_t ret;
    if (abilityInfo.name.size() == 0) {
        ret = bundleMgrProxy_->SetApplicationEnabled(abilityInfo.bundleName, isEnable, userId);
    } else {
        ret = bundleMgrProxy_->SetAbilityEnabled(abilityInfo, isEnable, userId);
    }
    if (ret != 0) {
        if (isEnable) {
            APP_LOGE("enable bundle failed");
        } else {
            APP_LOGE("disable bundle failed");
        }
        return false;
    }
    return true;
}
}  // namespace AppExecFwk
}  // namespace OHOS
