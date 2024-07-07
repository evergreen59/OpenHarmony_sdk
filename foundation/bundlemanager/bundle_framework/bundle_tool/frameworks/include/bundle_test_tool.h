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

#ifndef FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_BUNDLE_TOOL_INCLUDE_BUNDLE_TEST_TOOL_H
#define FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_BUNDLE_TOOL_INCLUDE_BUNDLE_TEST_TOOL_H

#include "shell_command.h"
#include "bundle_mgr_interface.h"
#include "bundle_installer_interface.h"

namespace OHOS {
namespace AppExecFwk {
class BundleTestTool : public ShellCommand {
public:
    BundleTestTool(int argc, char *argv[]);
    ~BundleTestTool();

private:
    ErrCode CreateCommandMap() override;
    ErrCode CreateMessageMap() override;
    ErrCode Init() override;
    void CreateQuickFixMsgMap(std::unordered_map<int32_t, std::string> &quickFixMsgMap);
    std::string GetResMsg(int32_t code);
    std::string GetResMsg(int32_t code, const std::shared_ptr<QuickFixResult> &quickFixRes);

    ErrCode RunAsHelpCommand();
    ErrCode RunAsCheckCommand();
    ErrCode CheckOperation(int userId, std::string deviceId, std::string bundleName,
        std::string moduleName, std::string abilityName);
    ErrCode RunAsSetRemovableCommand();
    ErrCode RunAsGetRemovableCommand();
    ErrCode RunAsInstallSandboxCommand();
    ErrCode RunAsUninstallSandboxCommand();
    ErrCode RunAsDumpSandboxCommand();
    ErrCode RunAsGetStringCommand();
    ErrCode RunAsGetIconCommand();
    ErrCode RunAsAddInstallRuleCommand();
    ErrCode RunAsGetInstallRuleCommand();
    ErrCode RunAsDeleteInstallRuleCommand();
    ErrCode RunAsCleanInstallRuleCommand();
    ErrCode RunAsAddAppRunningRuleCommand();
    ErrCode RunAsDeleteAppRunningRuleCommand();
    ErrCode RunAsCleanAppRunningRuleCommand();
    ErrCode RunAsGetAppRunningControlRuleCommand();
    ErrCode RunAsGetAppRunningControlRuleResultCommand();
    ErrCode RunAsDeployQuickFix();
    ErrCode RunAsSwitchQuickFix();
    ErrCode RunAsDeleteQuickFix();
    ErrCode RunAsSetDebugMode();
    ErrCode RunAsGetBundleStats();

    std::condition_variable cv_;
    std::mutex mutex_;
    bool dataReady_ {false};

    sptr<IBundleMgr> bundleMgrProxy_;
    sptr<IBundleInstaller> bundleInstallerProxy_;

    bool CheckRemovableErrorOption(int option, int counter, const std::string &commandName);
    bool CheckRemovableCorrectOption(int option, const std::string &commandName, int &isRemovable, std::string &name);
    bool SetIsRemovableOperation(const std::string &bundleName, const std::string &moduleName, int isRemovable) const;
    bool GetIsRemovableOperation(
        const std::string &bundleName, const std::string &moduleName, std::string &result) const;
    bool CheckSandboxErrorOption(int option, int counter, const std::string &commandName);
    bool CheckGetStringCorrectOption(int option, const std::string &commandName, int &temp, std::string &name);
    bool CheckGetIconCorrectOption(int option, const std::string &commandName, int &temp, std::string &name);
    ErrCode CheckAddInstallRuleCorrectOption(int option, const std::string &commandName,
        std::vector<std::string> &appIds, int &controlRuleType, int &userId, int &euid);
    ErrCode CheckGetInstallRuleCorrectOption(int option, const std::string &commandName, int &controlRuleType,
        int &userId, int &euid);
    ErrCode CheckDeleteInstallRuleCorrectOption(int option, const std::string &commandName,
        int &controlRuleType, std::vector<std::string> &appIds, int &userId, int &euid);
    ErrCode CheckCleanInstallRuleCorrectOption(int option, const std::string &commandName,
        int &controlRuleType, int &userId, int &euid);
    ErrCode CheckAppRunningRuleCorrectOption(int option, const std::string &commandName,
        std::vector<AppRunningControlRule> &controlRule, int &userId, int &euid);
    ErrCode CheckCleanAppRunningRuleCorrectOption(int option, const std::string &commandName, int &userId, int &euid);
    ErrCode CheckGetAppRunningRuleCorrectOption(int option, const std::string &commandName,
        int32_t &userId, int &euid);
    ErrCode CheckGetAppRunningRuleResultCorrectOption(int option, const std::string &commandName,
        std::string &bundleName, int32_t &userId, int &euid);
    bool CheckSandboxCorrectOption(int option, const std::string &commandName, int &data, std::string &bundleName);
    ErrCode InstallSandboxOperation(
        const std::string &bundleName, const int32_t userId, const int32_t dlpType, int32_t &appIndex) const;
    ErrCode UninstallSandboxOperation(
        const std::string &bundleName, const int32_t appIndex, const int32_t userId) const;
    ErrCode DumpSandboxBundleInfo(const std::string &bundleName, const int32_t appIndex, const int32_t userId,
        std::string &dumpResults);
    ErrCode StringToInt(std::string option, const std::string &commandName, int &temp, bool &result);
    ErrCode DeployQuickFix(const std::vector<std::string> &quickFixPaths,
        std::shared_ptr<QuickFixResult> &quickFixRes);
    ErrCode SwitchQuickFix(const std::string &bundleName, int32_t enable,
        std::shared_ptr<QuickFixResult> &quickFixRes);
    ErrCode DeleteQuickFix(const std::string &bundleName, std::shared_ptr<QuickFixResult> &quickFixRes);
    ErrCode GetQuickFixPath(int32_t index, std::vector<std::string>& quickFixPaths) const;
    ErrCode SetDebugMode(int32_t debugMode);
    bool GetBundleStats(const std::string &bundleName, int32_t userId, std::string& msg);
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FFOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_BUNDLE_TOOL_INCLUDE_BUNDLE_TEST_TOOL_H