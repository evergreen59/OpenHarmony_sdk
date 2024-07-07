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
#include "bundle_test_tool.h"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <future>
#include <getopt.h>
#include <iostream>
#include <set>
#include <sstream>
#include <unistd.h>
#include <vector>

#include "app_log_wrapper.h"
#include "appexecfwk_errors.h"
#include "bundle_command_common.h"
#include "bundle_death_recipient.h"
#include "bundle_mgr_client.h"
#include "bundle_mgr_proxy.h"
#include "bundle_tool_callback_stub.h"
#include "directory_ex.h"
#include "parameter.h"
#ifdef BUNDLE_FRAMEWORK_QUICK_FIX
#include "quick_fix_status_callback_host_impl.h"
#endif
#include "status_receiver_impl.h"
#include "string_ex.h"
#include "json_util.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
// param
const int32_t INDEX_OFFSET = 2;
// quick fix error code
const int32_t ERR_BUNDLEMANAGER_FEATURE_IS_NOT_SUPPORTED = 801;
// quick fix error message
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_INTERNAL_ERROR = "error: quick fix internal error.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_PARAM_ERROR = "error: param error.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_PROFILE_PARSE_FAILED = "error: profile parse failed.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_BUNDLE_NAME_NOT_SAME = "error: not same bundle name.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_VERSION_CODE_NOT_SAME = "error: not same version code.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_VERSION_NAME_NOT_SAME = "error: not same version name.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_PATCH_VERSION_CODE_NOT_SAME =
    "error: not same patch version code.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_PATCH_VERSION_NAME_NOT_SAME =
    "error: not same patch version name.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_PATCH_TYPE_NOT_SAME = "error: not same patch type.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_UNKNOWN_QUICK_FIX_TYPE = "error: unknown quick fix type.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_SO_INCOMPATIBLE = "error: patch so incompatible.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_MODULE_NAME_SAME = "error: same moduleName.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_BUNDLE_NAME_NOT_EXIST = "error: bundle name is not existed.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_MODULE_NAME_NOT_EXIST = "error: module name is not existed.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_SIGNATURE_INFO_NOT_SAME = "error: signature is not existed.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_ADD_HQF_FAILED = "error: quick fix add hqf failed.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_SAVE_APP_QUICK_FIX_FAILED =
    "error: quick fix save innerAppQuickFix failed.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_VERSION_CODE_ERROR =
    "error: quick fix version code require greater than original hqf.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_NO_PATCH_IN_DATABASE = "error: no this quick fix info in database.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_INVALID_PATCH_STATUS = "error: wrong quick fix status.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_NOT_EXISTED_BUNDLE_INFO =
    "error: cannot obtain the bundleInfo from data mgr.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_REMOVE_PATCH_PATH_FAILED = "error: quick fix remove path failed.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_EXTRACT_DIFF_FILES_FAILED = "error: extract diff files failed.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_APPLY_DIFF_PATCH_FAILED = "error: apply diff patch failed.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_UNKOWN = "error: unknown.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_FEATURE_IS_NOT_SUPPORTED = "feature is not supported.\n";
const std::string MSG_ERR_BUNDLEMANAGER_OPERATION_TIME_OUT = "error: quick fix operation time out.\n";
const std::string MSG_ERR_BUNDLEMANAGER_FAILED_SERVICE_DIED = "error: bundleMgr service is dead.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_HOT_RELOAD_NOT_SUPPORT_RELEASE_BUNDLE =
    "error: hotreload not support release bundle.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_PATCH_ALREADY_EXISTED = "error: patch type already existed.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_HOT_RELOAD_ALREADY_EXISTED =
    "error: hotreload type already existed.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_NO_PATCH_INFO_IN_BUNDLE_INFO =
    "error: no patch info in bundleInfo.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_MOVE_PATCH_FILE_FAILED = "error: quick fix move hqf file failed.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_CREATE_PATCH_PATH_FAILED = "error: quick fix create path failed.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_OLD_PATCH_OR_HOT_RELOAD_IN_DB =
    "error: old patch or hot reload in db.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_SEND_REQUEST_FAILED = "error: send request failed.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_REAL_PATH_FAILED = "error: obtain realpath failed.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_INVALID_PATH = "error: input invalid path.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_OPEN_SOURCE_FILE_FAILED = "error: open source file failed.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_CREATE_FD_FAILED = "error: create file descriptor failed.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_INVALID_TARGET_DIR = "error: invalid designated target dir\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_CREATE_TARGET_DIR_FAILED = "error: create target dir failed.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_PERMISSION_DENIED = "error: quick fix permission denied.\n";
const std::string MSG_ERR_BUNDLEMANAGER_QUICK_FIX_WRITE_FILE_FAILED = "error: write file to target dir failed.\n";
const std::string MSG_ERR_BUNDLEMANAGER_SET_DEBUG_MODE_INVALID_PARAM =
    "error: invalid param for setting debug mode.\n";
const std::string MSG_ERR_BUNDLEMANAGER_SET_DEBUG_MODE_INTERNAL_ERROR =
    "error: internal error for setting debug mode.\n";
const std::string MSG_ERR_BUNDLEMANAGER_SET_DEBUG_MODE_PARCEL_ERROR = "error: parcel error for setting debug mode.\n";
const std::string MSG_ERR_BUNDLEMANAGER_SET_DEBUG_MODE_SEND_REQUEST_ERROR = "error: send request error.\n";
const std::string MSG_ERR_BUNDLEMANAGER_SET_DEBUG_MODE_UID_CHECK_FAILED = "error: uid check failed.\n";

static const std::string TOOL_NAME = "bundle_test_tool";
static const std::string HELP_MSG = "usage: bundle_test_tool <command> <options>\n"
                             "These are common bundle_test_tool commands list:\n"
                             "  help         list available commands\n"
                             "  setrm        set module isRemovable by given bundle name and module name\n"
                             "  getrm        obtain the value of isRemovable by given bundle name and module name\n"
                             "  installSandbox      indicates install sandbox\n"
                             "  uninstallSandbox    indicates uninstall sandbox\n"
                             "  dumpSandbox         indicates dump sandbox info\n"
                             "  getStr      obtain the value of label by given bundle name, module name and label id\n"
                             "  getIcon     obtain the value of icon by given bundle name, module name,\n"
                             "              density and icon id\n"
                             "  addAppInstallRule     obtain the value of install controlRule by given some app id\n"
                             "                        control rule type, user id and euid\n"
                             "  getAppInstallRule     obtain the value of install controlRule by given some app id\n"
                             "                        rule type, user id and euid\n"
                             "  deleteAppInstallRule  obtain the value of install controlRule by given some app id\n"
                             "                        user id and euid\n"
                             "  cleanAppInstallRule   obtain the value of install controlRule by given rule type\n"
                             "                        user id and euid\n"
                             "  addAppRunningRule     obtain the value of app running control rule\n"
                             "                        by given controlRule user id and euidn"
                             "  deleteAppRunningRule  obtain the value of app running control rule\n"
                             "                        by given controlRule user id and euid\n"
                             "  cleanAppRunningRule   obtain the value of app running control\n"
                             "                        rule by given user id and euid\n"
                             "  getAppRunningControlRule  obtain the value of app running control rule\n"
                             "                            by given user id and euid and some app id\n"
                             "  getAppRunningControlRuleResult     obtain the value of app running control rule\n"
                             "                      by given bundleName user id, euid and controlRuleResult\n"
                             "  deployQuickFix      deploy a quick fix patch of an already installed bundle\n"
                             "  switchQuickFix      switch a quick fix patch of an already installed bundle\n"
                             "  deleteQuickFix      delete a quick fix patch of an already installed bundle\n"
                             "  setDebugMode        enable signature debug mode\n"
                             "  getBundleStats        get bundle stats\n";

const std::string HELP_MSG_GET_REMOVABLE =
    "usage: bundle_test_tool getrm <options>\n"
    "eg:bundle_test_tool getrm -m <module-name> -n <bundle-name> \n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -n, --bundle-name  <bundle-name>       get isRemovable by moduleNmae and bundleName\n"
    "  -m, --module-name <module-name>        get isRemovable by moduleNmae and bundleName\n";

const std::string HELP_MSG_NO_REMOVABLE_OPTION =
    "error: you must specify a bundle name with '-n' or '--bundle-name' \n"
    "and a module name with '-m' or '--module-name' \n";

const std::string HELP_MSG_SET =
    "usage: bundle_test_tool setrm <options>\n"
    "eg:bundle_test_tool setrm -m <module-name> -n <bundle-name> -i 1\n"
    "options list:\n"
    "  -h, --help                               list available commands\n"
    "  -n, --bundle-name  <bundle-name>         set isRemovable by moduleNmae and bundleName\n"
    "  -i, --is-removable <is-removable>        set isRemovable  0 or 1\n"
    "  -m, --module-name <module-name>          set isRemovable by moduleNmae and bundleName\n";

const std::string HELP_MSG_INSTALL_SANDBOX =
    "usage: bundle_test_tool installSandbox <options>\n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -u, --user-id <user-id>                specify a user id\n"
    "  -n, --bundle-name <bundle-name>        install a sandbox of a bundle\n"
    "  -d, --dlp-type <dlp-type>              specify type of the sandbox application\n";

const std::string HELP_MSG_UNINSTALL_SANDBOX =
    "usage: bundle_test_tool uninstallSandbox <options>\n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -u, --user-id <user-id>                specify a user id\n"
    "  -a, --app-index <app-index>            specify a app index\n"
    "  -n, --bundle-name <bundle-name>        install a sandbox of a bundle\n";

const std::string HELP_MSG_DUMP_SANDBOX =
    "usage: bundle_test_tool dumpSandbox <options>\n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -u, --user-id <user-id>                specify a user id\n"
    "  -a, --app-index <app-index>            specify a app index\n"
    "  -n, --bundle-name <bundle-name>        install a sandbox of a bundle\n";

const std::string HELP_MSG_GET_STRING =
    "usage: bundle_test_tool getStr <options>\n"
    "eg:bundle_test_tool getStr -m <module-name> -n <bundle-name> -u <user-id> -i --id <id> \n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -n, --bundle-name <bundle-name>        specify bundle name of the application\n"
    "  -m, --module-name <module-name>        specify module name of the application\n"
    "  -u, --user-id <user-id>                specify a user id\n"
    "  -i, --id <id>                          specify a label id of the application\n";

const std::string HELP_MSG_GET_ICON =
    "usage: bundle_test_tool getIcon <options>\n"
    "eg:bundle_test_tool getIcon -m <module-name> -n <bundle-name> -u <user-id> -d --density <density> -i --id <id> \n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -n, --bundle-name  <bundle-name>       specify bundle name of the application\n"
    "  -m, --module-name <module-name>        specify module name of the application\n"
    "  -u, --user-id <user-id>                specify a user id\n"
    "  -d, --density <density>                specify a density\n"
    "  -i, --id <id>                          specify a icon id of the application\n";

const std::string HELP_MSG_NO_GETSTRING_OPTION =
    "error: you must specify a bundle name with '-n' or '--bundle-name' \n"
    "and a module name with '-m' or '--module-name' \n"
    "and a userid with '-u' or '--user-id' \n"
    "and a labelid with '-i' or '--id' \n";

const std::string HELP_MSG_NO_GETICON_OPTION =
    "error: you must specify a bundle name with '-n' or '--bundle-name' \n"
    "and a module name with '-m' or '--module-name' \n"
    "and a userid with '-u' or '--user-id' \n"
    "and a density with '-d' or '--density' \n"
    "and a iconid with '-i' or '--id' \n";

const std::string HELP_MSG_ADD_INSTALL_RULE =
    "usage: bundle_test_tool <options>\n"
    "eg:bundle_test_tool addAppInstallRule -a <app-id> -t <control-rule-type> -u <user-id> \n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -a, --app-id <app-id>                  specify app id of the application\n"
    "  -e, --euid <eu-id>                     default euid value is 537\n"
    "  -t, --control-rule-type                specify control type of the application\n"
    "  -u, --user-id <user-id>                specify a user id\n";

const std::string HELP_MSG_GET_INSTALL_RULE =
    "usage: bundle_test_tool <options>\n"
    "eg:bundle_test_tool getAppInstallRule -t <control-rule-type> -u <user-id> \n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -e, --euid <eu-id>                     default euid value is 537\n"
    "  -t, --control-rule-type                specify control type of the application\n"
    "  -u, --user-id <user-id>                specify a user id\n";

const std::string HELP_MSG_DELETE_INSTALL_RULE =
    "usage: bundle_test_tool <options>\n"
    "eg:bundle_test_tool deleteAppInstallRule -a <app-id> -t <control-rule-type> -u <user-id> \n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -e, --euid <eu-id>                     default euid value is 537\n"
    "  -a, --app-id <app-id>                  specify app id of the application\n"
    "  -t, --control-rule-type                specify control type of the application\n"
    "  -u, --user-id <user-id>                specify a user id\n";

const std::string HELP_MSG_CLEAN_INSTALL_RULE =
    "usage: bundle_test_tool <options>\n"
    "eg:bundle_test_tool cleanAppInstallRule -t <control-rule-type> -u <user-id> \n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -e, --euid <eu-id>                     default euid value is 537\n"
    "  -t, --control-rule-type                specify control type of the application\n"
    "  -u, --user-id <user-id>                specify a user id\n";

const std::string HELP_MSG_ADD_APP_RUNNING_RULE =
    "usage: bundle_test_tool <options>\n"
    "eg:bundle_test_tool addAppRunningRule -c <control-rule> -u <user-id> \n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -e, --euid <eu-id>                     default euid value is 537\n"
    "  -c, --control-rule                     specify control rule of the application\n"
    "  -u, --user-id <user-id>                specify a user id\n";

const std::string HELP_MSG_DELETE_APP_RUNNING_RULE =
    "usage: bundle_test_tool <options>\n"
    "eg:bundle_test_tool deleteAppRunningRule -c <control-rule> -u <user-id> \n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -e, --euid <eu-id>                     default euid value is 537\n"
    "  -c, --control-rule                     specify control rule of the application\n"
    "  -u, --user-id <user-id>                specify a user id\n";

const std::string HELP_MSG_CLEAN_APP_RUNNING_RULE =
    "usage: bundle_test_tool <options>\n"
    "eg:bundle_test_tool cleanAppRunningRule -u <user-id> \n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -e, --euid <eu-id>                     default euid value is 537\n"
    "  -u, --user-id <user-id>                specify a user id\n";

const std::string HELP_MSG_GET_APP_RUNNING_RULE =
    "usage: bundle_test_tool <options>\n"
    "eg:bundle_test_tool getAppRunningControlRule -u <user-id> \n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -e, --euid <eu-id>                     default euid value is 537\n"
    "  -u, --user-id <user-id>                specify a user id\n";

const std::string HELP_MSG_GET_APP_RUNNING_RESULT_RULE =
    "usage: bundle_test_tool <options>\n"
    "eg:bundle_test_tool getAppRunningControlRuleResult -n <bundle-name> \n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -e, --euid <eu-id>                     default euid value is 537\n"
    "  -n, --bundle-name  <bundle-name>       specify bundle name of the application\n"
    "  -u, --user-id <user-id>                specify a user id\n";

const std::string HELP_MSG_NO_ADD_INSTALL_RULE_OPTION =
    "error: you must specify a app id with '-a' or '--app-id' \n"
    "and a control type with '-t' or '--control-rule-type' \n"
    "and a userid with '-u' or '--user-id' \n";

const std::string HELP_MSG_NO_GET_INSTALL_RULE_OPTION =
    "error: you must specify a control type with '-t' or '--control-rule-type' \n"
    "and a userid with '-u' or '--user-id' \n";

const std::string HELP_MSG_NO_DELETE_INSTALL_RULE_OPTION =
    "error: you must specify a control type with '-a' or '--app-id' \n"
    "and a userid with '-u' or '--user-id' \n";

const std::string HELP_MSG_NO_CLEAN_INSTALL_RULE_OPTION =
    "error: you must specify a control type with '-t' or '--control-rule-type' \n"
    "and a userid with '-u' or '--user-id' \n";

const std::string HELP_MSG_NO_APP_RUNNING_RULE_OPTION =
    "error: you must specify a app running type with '-c' or '--control-rule' \n"
    "and a userid with '-u' or '--user-id' \n";

const std::string HELP_MSG_NO_CLEAN_APP_RUNNING_RULE_OPTION =
    "error: you must specify a app running type with a userid '-u' or '--user-id \n";

const std::string HELP_MSG_NO_GET_ALL_APP_RUNNING_RULE_OPTION =
    "error: you must specify a app running type with '-a' or '--app-id' \n"
    "and a userid with '-u' or '--user-id' \n";

const std::string HELP_MSG_NO_GET_APP_RUNNING_RULE_OPTION =
    "error: you must specify a app running type with '-n' or '--bundle-name' \n"
    "and a userid with '-u' or '--user-id' \n";

const std::string HELP_MSG_DEPLOY_QUICK_FIX =
    "usage: bundle_test_tool deploy quick fix <options>\n"
    "eg:bundle_test_tool deployQuickFix -p <quickFixPath> \n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -p, --patch-path  <patch-path>         specify patch path of the patch\n";

const std::string HELP_MSG_SWITCH_QUICK_FIX =
    "usage: bundle_test_tool switch quick fix <options>\n"
    "eg:bundle_test_tool switchQuickFix -n <bundle-name> \n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -n, --bundle-name  <bundle-name>       specify bundleName of the patch\n"
    "  -e, --enbale  <enable>                 enable a deployed patch of disable an under using patch,\n"
    "                                         1 represents enable and 0 represents disable\n";

const std::string HELP_MSG_DELETE_QUICK_FIX =
    "usage: bundle_test_tool delete quick fix <options>\n"
    "eg:bundle_test_tool deleteQuickFix -n <bundle-name> \n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -n, --bundle-name  <bundle-name>       specify bundleName of the patch\n";

const std::string HELP_MSG_SET_DEBUG_MODE =
    "usage: bundle_test_tool setDebugMode <options>\n"
    "eg:bundle_test_tool setDebugMode -e <0/1>\n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -e, --enable  <enable>                 enable signature debug mode, 1 represents enable debug mode and 0\n"
    "                                         represents disable debug mode\n";

const std::string HELP_MSG_GET_BUNDLE_STATS =
    "usage: bundle_test_tool getBundleStats <options>\n"
    "eg:bundle_test_tool getBundleStats -n <bundle-name>\n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -n, --bundle-name  <bundle-name>       specify bundle name of the application\n"
    "  -u, --user-id <user-id>                specify a user id\n";

const std::string HELP_MSG_NO_BUNDLE_NAME_OPTION =
    "error: you must specify a bundle name with '-n' or '--bundle-name' \n";

const std::string STRING_SET_REMOVABLE_OK = "set removable is ok \n";
const std::string STRING_SET_REMOVABLE_NG = "error: failed to set removable \n";
const std::string STRING_GET_REMOVABLE_OK = "get removable is ok \n";
const std::string STRING_GET_REMOVABLE_NG = "error: failed to get removable \n";
const std::string STRING_REQUIRE_CORRECT_VALUE =
    "error: option requires a correct value or note that\n"
    "the difference in expressions between short option and long option. \n";

const std::string STRING_INSTALL_SANDBOX_SUCCESSFULLY = "install sandbox app successfully \n";
const std::string STRING_INSTALL_SANDBOX_FAILED = "install sandbox app failed \n";

const std::string STRING_UNINSTALL_SANDBOX_SUCCESSFULLY = "uninstall sandbox app successfully\n";
const std::string STRING_UNINSTALL_SANDBOX_FAILED = "uninstall sandbox app failed\n";

const std::string STRING_DUMP_SANDBOX_FAILED = "dump sandbox app info failed\n";

const std::string STRING_GET_STRING_NG = "error: failed to get label \n";

const std::string STRING_GET_ICON_NG = "error: failed to get icon \n";

const std::string STRING_ADD_RULE_NG = "error: failed to add rule \n";
const std::string STRING_GET_RULE_NG = "error: failed to get rule \n";
const std::string STRING_DELETE_RULE_NG = "error: failed to delete rule \n";

const std::string STRING_DEPLOY_QUICK_FIX_OK = "deploy quick fix successfully\n";
const std::string STRING_DEPLOY_QUICK_FIX_NG = "deploy quick fix failed\n";
const std::string HELP_MSG_NO_QUICK_FIX_PATH_OPTION = "need a quick fix patch path\n";
const std::string STRING_SWITCH_QUICK_FIX_OK = "switch quick fix successfully\n";
const std::string STRING_SWITCH_QUICK_FIX_NG = "switch quick fix failed\n";
const std::string STRING_DELETE_QUICK_FIX_OK = "delete quick fix successfully\n";
const std::string STRING_DELETE_QUICK_FIX_NG = "delete quick fix failed\n";

const std::string STRING_SET_DEBUG_MODE_OK = "set debug mode successfully\n";
const std::string STRING_SET_DEBUG_MODE_NG = "set debug mode failed\n";

const std::string STRING_GET_BUNDLE_STATS_OK = "get bundle stats successfully\n";
const std::string STRING_GET_BUNDLE_STATS_NG = "get bundle stats failed\n";

const std::string GET_BUNDLE_STATS_ARRAY[] = {
    "app data size: ",
    "user data size: ",
    "distributed data size: ",
    "database size: ",
    "cache size: "
};

const std::string GET_RM = "getrm";
const std::string SET_RM = "setrm";
const std::string INSTALL_SANDBOX = "installSandbox";
const std::string UNINSTALL_SANDBOX = "uninstallSandbox";
const std::string DUMP_SANDBOX = "dumpSandbox";

const std::string SHORT_OPTIONS = "hn:m:a:d:u:i:";
const struct option LONG_OPTIONS[] = {
    {"help", no_argument, nullptr, 'h'},
    {"bundle-name", required_argument, nullptr, 'n'},
    {"module-name", required_argument, nullptr, 'm'},
    {"ability-name", required_argument, nullptr, 'a'},
    {"device-id", required_argument, nullptr, 'd'},
    {"user-id", required_argument, nullptr, 'u'},
    {"is-removable", required_argument, nullptr, 'i'},
    {nullptr, 0, nullptr, 0},
};

const std::string SHORT_OPTIONS_SANDBOX = "hn:d:u:a:";
const struct option LONG_OPTIONS_SANDBOX[] = {
    {"help", no_argument, nullptr, 'h'},
    {"bundle-name", required_argument, nullptr, 'n'},
    {"user-id", required_argument, nullptr, 'u'},
    {"dlp-type", required_argument, nullptr, 'd'},
    {"app-index", required_argument, nullptr, 'a'},
    {nullptr, 0, nullptr, 0},
};

const std::string SHORT_OPTIONS_GET = "hn:m:u:i:d:";
const struct option LONG_OPTIONS_GET[] = {
    {"help", no_argument, nullptr, 'h'},
    {"bundle-name", required_argument, nullptr, 'n'},
    {"module-name", required_argument, nullptr, 'm'},
    {"user-id", required_argument, nullptr, 'u'},
    {"id", required_argument, nullptr, 'i'},
    {"density", required_argument, nullptr, 'd'},
    {nullptr, 0, nullptr, 0},
};

const std::string SHORT_OPTIONS_RULE = "ha:c:n:e:r:t:u:";
const struct option LONG_OPTIONS_RULE[] = {
    {"help", no_argument, nullptr, 'h'},
    {"app-id", required_argument, nullptr, 'a'},
    {"control-rule", required_argument, nullptr, 'c'},
    {"bundle-name", required_argument, nullptr, 'n'},
    {"bundle-name", required_argument, nullptr, 'n'},
    {"euid", required_argument, nullptr, 'e'},
    {"control-rule-type", required_argument, nullptr, 't'},
    {"user-id", required_argument, nullptr, 'u'},
    {nullptr, 0, nullptr, 0},
};

const std::string SHORT_OPTIONS_QUICK_FIX = "hp:n:e:";
const struct option LONG_OPTIONS_QUICK_FIX[] = {
    {"help", no_argument, nullptr, 'h'},
    {"patch-path", required_argument, nullptr, 'p'},
    {"bundle-name", required_argument, nullptr, 'n'},
    {"enable", required_argument, nullptr, 'e'},
    {nullptr, 0, nullptr, 0},
};

const std::string SHORT_OPTIONS_DEBUG_MODE = "he:";
const struct option LONG_OPTIONS_DEBUG_MODE[] = {
    {"help", no_argument, nullptr, 'h'},
    {"enable", required_argument, nullptr, 'e'},
    {nullptr, 0, nullptr, 0},
};

const std::string SHORT_OPTIONS_GET_BUNDLE_STATS = "hn:u:";
const struct option LONG_OPTIONS_GET_BUNDLE_STATS[] = {
    {"help", no_argument, nullptr, 'h'},
    {"bundle-name", required_argument, nullptr, 'n'},
    {"user-id", required_argument, nullptr, 'u'},
    {nullptr, 0, nullptr, 0},
};
}  // namespace

BundleTestTool::BundleTestTool(int argc, char *argv[]) : ShellCommand(argc, argv, TOOL_NAME)
{}

BundleTestTool::~BundleTestTool()
{}

ErrCode BundleTestTool::CreateCommandMap()
{
    commandMap_ = {
        {"help", std::bind(&BundleTestTool::RunAsHelpCommand, this)},
        {"check", std::bind(&BundleTestTool::RunAsCheckCommand, this)},
        {"setrm", std::bind(&BundleTestTool::RunAsSetRemovableCommand, this)},
        {"getrm", std::bind(&BundleTestTool::RunAsGetRemovableCommand, this)},
        {"installSandbox", std::bind(&BundleTestTool::RunAsInstallSandboxCommand, this)},
        {"uninstallSandbox", std::bind(&BundleTestTool::RunAsUninstallSandboxCommand, this)},
        {"dumpSandbox", std::bind(&BundleTestTool::RunAsDumpSandboxCommand, this)},
        {"getStr", std::bind(&BundleTestTool::RunAsGetStringCommand, this)},
        {"getIcon", std::bind(&BundleTestTool::RunAsGetIconCommand, this)},
        {"addAppInstallRule", std::bind(&BundleTestTool::RunAsAddInstallRuleCommand, this)},
        {"getAppInstallRule", std::bind(&BundleTestTool::RunAsGetInstallRuleCommand, this)},
        {"deleteAppInstallRule", std::bind(&BundleTestTool::RunAsDeleteInstallRuleCommand, this)},
        {"cleanAppInstallRule", std::bind(&BundleTestTool::RunAsCleanInstallRuleCommand, this)},
        {"addAppRunningRule", std::bind(&BundleTestTool::RunAsAddAppRunningRuleCommand, this)},
        {"deleteAppRunningRule", std::bind(&BundleTestTool::RunAsDeleteAppRunningRuleCommand, this)},
        {"cleanAppRunningRule", std::bind(&BundleTestTool::RunAsCleanAppRunningRuleCommand, this)},
        {"getAppRunningControlRule", std::bind(&BundleTestTool::RunAsGetAppRunningControlRuleCommand, this)},
        {"getAppRunningControlRuleResult",
            std::bind(&BundleTestTool::RunAsGetAppRunningControlRuleResultCommand, this)},
        {"deployQuickFix", std::bind(&BundleTestTool::RunAsDeployQuickFix, this)},
        {"switchQuickFix", std::bind(&BundleTestTool::RunAsSwitchQuickFix, this)},
        {"deleteQuickFix", std::bind(&BundleTestTool::RunAsDeleteQuickFix, this)},
        {"setDebugMode", std::bind(&BundleTestTool::RunAsSetDebugMode, this)},
        {"getBundleStats", std::bind(&BundleTestTool::RunAsGetBundleStats, this)}
    };

    return OHOS::ERR_OK;
}

ErrCode BundleTestTool::CreateMessageMap()
{
    messageMap_ = BundleCommandCommon::bundleMessageMap_;

    return OHOS::ERR_OK;
}

ErrCode BundleTestTool::Init()
{
    APP_LOGI("BundleTestTool Init()");
    ErrCode result = OHOS::ERR_OK;
    if (bundleMgrProxy_ == nullptr) {
        bundleMgrProxy_ = BundleCommandCommon::GetBundleMgrProxy();
        if (bundleMgrProxy_ != nullptr) {
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

void BundleTestTool::CreateQuickFixMsgMap(std::unordered_map<int32_t, std::string> &quickFixMsgMap)
{
    quickFixMsgMap = {
        { ERR_OK, Constants::EMPTY_STRING },
        { ERR_BUNDLEMANAGER_QUICK_FIX_INTERNAL_ERROR, MSG_ERR_BUNDLEMANAGER_QUICK_FIX_INTERNAL_ERROR },
        { ERR_BUNDLEMANAGER_QUICK_FIX_PARAM_ERROR, MSG_ERR_BUNDLEMANAGER_QUICK_FIX_PARAM_ERROR },
        { ERR_BUNDLEMANAGER_QUICK_FIX_PROFILE_PARSE_FAILED, MSG_ERR_BUNDLEMANAGER_QUICK_FIX_PROFILE_PARSE_FAILED },
        { ERR_BUNDLEMANAGER_QUICK_FIX_BUNDLE_NAME_NOT_SAME, MSG_ERR_BUNDLEMANAGER_QUICK_FIX_BUNDLE_NAME_NOT_SAME },
        { ERR_BUNDLEMANAGER_QUICK_FIX_VERSION_CODE_NOT_SAME, MSG_ERR_BUNDLEMANAGER_QUICK_FIX_VERSION_CODE_NOT_SAME },
        { ERR_BUNDLEMANAGER_QUICK_FIX_VERSION_NAME_NOT_SAME, MSG_ERR_BUNDLEMANAGER_QUICK_FIX_VERSION_NAME_NOT_SAME },
        { ERR_BUNDLEMANAGER_QUICK_FIX_PATCH_VERSION_CODE_NOT_SAME,
            MSG_ERR_BUNDLEMANAGER_QUICK_FIX_PATCH_VERSION_CODE_NOT_SAME },
        { ERR_BUNDLEMANAGER_QUICK_FIX_PATCH_VERSION_NAME_NOT_SAME,
            MSG_ERR_BUNDLEMANAGER_QUICK_FIX_PATCH_VERSION_NAME_NOT_SAME },
        { ERR_BUNDLEMANAGER_QUICK_FIX_PATCH_TYPE_NOT_SAME, MSG_ERR_BUNDLEMANAGER_QUICK_FIX_PATCH_TYPE_NOT_SAME },
        { ERR_BUNDLEMANAGER_QUICK_FIX_UNKNOWN_QUICK_FIX_TYPE, MSG_ERR_BUNDLEMANAGER_QUICK_FIX_UNKNOWN_QUICK_FIX_TYPE },
        { ERR_BUNDLEMANAGER_QUICK_FIX_SO_INCOMPATIBLE, MSG_ERR_BUNDLEMANAGER_QUICK_FIX_SO_INCOMPATIBLE },
        { ERR_BUNDLEMANAGER_QUICK_FIX_BUNDLE_NAME_NOT_EXIST, MSG_ERR_BUNDLEMANAGER_QUICK_FIX_BUNDLE_NAME_NOT_EXIST },
        { ERR_BUNDLEMANAGER_QUICK_FIX_MODULE_NAME_NOT_EXIST, MSG_ERR_BUNDLEMANAGER_QUICK_FIX_MODULE_NAME_NOT_EXIST },
        { ERR_BUNDLEMANAGER_QUICK_FIX_SIGNATURE_INFO_NOT_SAME,
            MSG_ERR_BUNDLEMANAGER_QUICK_FIX_SIGNATURE_INFO_NOT_SAME },
        { ERR_BUNDLEMANAGER_QUICK_FIX_EXTRACT_DIFF_FILES_FAILED,
            MSG_ERR_BUNDLEMANAGER_QUICK_FIX_EXTRACT_DIFF_FILES_FAILED },
        { ERR_BUNDLEMANAGER_QUICK_FIX_APPLY_DIFF_PATCH_FAILED,
            MSG_ERR_BUNDLEMANAGER_QUICK_FIX_APPLY_DIFF_PATCH_FAILED },
        { ERR_BUNDLEMANAGER_FEATURE_IS_NOT_SUPPORTED, MSG_ERR_BUNDLEMANAGER_QUICK_FIX_FEATURE_IS_NOT_SUPPORTED },
        { ERR_APPEXECFWK_OPERATION_TIME_OUT, MSG_ERR_BUNDLEMANAGER_OPERATION_TIME_OUT },
        { ERR_APPEXECFWK_FAILED_SERVICE_DIED, MSG_ERR_BUNDLEMANAGER_FAILED_SERVICE_DIED },
        { ERR_BUNDLEMANAGER_QUICK_FIX_HOT_RELOAD_NOT_SUPPORT_RELEASE_BUNDLE,
            MSG_ERR_BUNDLEMANAGER_QUICK_FIX_HOT_RELOAD_NOT_SUPPORT_RELEASE_BUNDLE },
        { ERR_BUNDLEMANAGER_QUICK_FIX_PATCH_ALREADY_EXISTED, MSG_ERR_BUNDLEMANAGER_QUICK_FIX_PATCH_ALREADY_EXISTED },
        { ERR_BUNDLEMANAGER_QUICK_FIX_HOT_RELOAD_ALREADY_EXISTED,
            MSG_ERR_BUNDLEMANAGER_QUICK_FIX_HOT_RELOAD_ALREADY_EXISTED },
        { ERR_BUNDLEMANAGER_QUICK_FIX_MODULE_NAME_SAME, MSG_ERR_BUNDLEMANAGER_QUICK_FIX_MODULE_NAME_SAME },
        { ERR_BUNDLEMANAGER_QUICK_FIX_NO_PATCH_INFO_IN_BUNDLE_INFO,
            MSG_ERR_BUNDLEMANAGER_QUICK_FIX_NO_PATCH_INFO_IN_BUNDLE_INFO },
        { ERR_BUNDLEMANAGER_SET_DEBUG_MODE_INVALID_PARAM, MSG_ERR_BUNDLEMANAGER_SET_DEBUG_MODE_INVALID_PARAM },
        { ERR_BUNDLEMANAGER_SET_DEBUG_MODE_INTERNAL_ERROR, MSG_ERR_BUNDLEMANAGER_SET_DEBUG_MODE_INTERNAL_ERROR },
        { ERR_BUNDLEMANAGER_SET_DEBUG_MODE_PARCEL_ERROR, MSG_ERR_BUNDLEMANAGER_SET_DEBUG_MODE_PARCEL_ERROR },
        { ERR_BUNDLEMANAGER_SET_DEBUG_MODE_SEND_REQUEST_ERROR,
            MSG_ERR_BUNDLEMANAGER_SET_DEBUG_MODE_SEND_REQUEST_ERROR },
        { ERR_BUNDLEMANAGER_SET_DEBUG_MODE_UID_CHECK_FAILED, MSG_ERR_BUNDLEMANAGER_SET_DEBUG_MODE_UID_CHECK_FAILED },
        { ERR_BUNDLEMANAGER_QUICK_FIX_ADD_HQF_FAILED, MSG_ERR_BUNDLEMANAGER_QUICK_FIX_ADD_HQF_FAILED },
        { ERR_BUNDLEMANAGER_QUICK_FIX_SAVE_APP_QUICK_FIX_FAILED,
            MSG_ERR_BUNDLEMANAGER_QUICK_FIX_SAVE_APP_QUICK_FIX_FAILED },
        { ERR_BUNDLEMANAGER_QUICK_FIX_VERSION_CODE_ERROR, MSG_ERR_BUNDLEMANAGER_QUICK_FIX_VERSION_CODE_ERROR },
        { ERR_BUNDLEMANAGER_QUICK_FIX_NO_PATCH_IN_DATABASE, MSG_ERR_BUNDLEMANAGER_QUICK_FIX_NO_PATCH_IN_DATABASE },
        { ERR_BUNDLEMANAGER_QUICK_FIX_INVALID_PATCH_STATUS, MSG_ERR_BUNDLEMANAGER_QUICK_FIX_INVALID_PATCH_STATUS },
        { ERR_BUNDLEMANAGER_QUICK_FIX_NOT_EXISTED_BUNDLE_INFO,
            MSG_ERR_BUNDLEMANAGER_QUICK_FIX_NOT_EXISTED_BUNDLE_INFO },
        { ERR_BUNDLEMANAGER_QUICK_FIX_REMOVE_PATCH_PATH_FAILED,
            MSG_ERR_BUNDLEMANAGER_QUICK_FIX_REMOVE_PATCH_PATH_FAILED },
        { ERR_BUNDLEMANAGER_QUICK_FIX_MOVE_PATCH_FILE_FAILED,
            MSG_ERR_BUNDLEMANAGER_QUICK_FIX_MOVE_PATCH_FILE_FAILED },
        { ERR_BUNDLEMANAGER_QUICK_FIX_CREATE_PATCH_PATH_FAILED,
            MSG_ERR_BUNDLEMANAGER_QUICK_FIX_CREATE_PATCH_PATH_FAILED },
        { ERR_BUNDLEMANAGER_QUICK_FIX_OLD_PATCH_OR_HOT_RELOAD_IN_DB,
            MSG_ERR_BUNDLEMANAGER_QUICK_FIX_OLD_PATCH_OR_HOT_RELOAD_IN_DB }
    };
}

ErrCode BundleTestTool::RunAsHelpCommand()
{
    resultReceiver_.append(HELP_MSG);

    return OHOS::ERR_OK;
}

ErrCode BundleTestTool::CheckOperation(int userId, std::string deviceId, std::string bundleName,
    std::string moduleName, std::string abilityName)
{
    std::unique_lock<std::mutex> lock(mutex_);
    sptr<BundleToolCallbackStub> bundleToolCallbackStub =
        new(std::nothrow) BundleToolCallbackStub(cv_, mutex_, dataReady_);
    if (bundleToolCallbackStub == nullptr) {
        APP_LOGE("bundleToolCallbackStub is null");
        return OHOS::ERR_INVALID_VALUE;
    }
    APP_LOGI("CheckAbilityEnableInstall param: userId:%{public}d, bundleName:%{public}s, moduleName:%{public}s," \
        "abilityName:%{public}s", userId, bundleName.c_str(), moduleName.c_str(), abilityName.c_str());
    AAFwk::Want want;
    want.SetElementName(deviceId, bundleName, abilityName, moduleName);
    bool ret = bundleMgrProxy_->CheckAbilityEnableInstall(want, 1, userId, bundleToolCallbackStub);
    if (!ret) {
        APP_LOGE("CheckAbilityEnableInstall failed");
        return OHOS::ERR_OK;
    }
    APP_LOGI("CheckAbilityEnableInstall wait");
    cv_.wait(lock, [this] { return dataReady_; });
    dataReady_ = false;
    return OHOS::ERR_OK;
}

ErrCode BundleTestTool::RunAsCheckCommand()
{
    int counter = 0;
    int userId = 100;
    std::string deviceId = "";
    std::string bundleName = "";
    std::string moduleName = "";
    std::string abilityName = "";
    while (true) {
        counter++;
        int32_t option = getopt_long(argc_, argv_, SHORT_OPTIONS.c_str(), LONG_OPTIONS, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1) {
            // When scanning the first argument
            if ((counter == 1) && (strcmp(argv_[optind], cmd_.c_str()) == 0)) {
                // 'CheckAbilityEnableInstall' with no option: CheckAbilityEnableInstall
                // 'CheckAbilityEnableInstall' with a wrong argument: CheckAbilityEnableInstall
                APP_LOGD("'CheckAbilityEnableInstall' with no option.");
                return OHOS::ERR_INVALID_VALUE;
            }
            break;
        }
        switch (option) {
            case 'n': {
                bundleName = optarg;
                break;
            }
            case 'm': {
                moduleName = optarg;
                break;
            }
            case 'a': {
                abilityName = optarg;
                break;
            }
            case 'd': {
                deviceId = optarg;
                break;
            }
            case 'u': {
                userId = std::stoi(optarg);
                break;
            }
            default: {
                return OHOS::ERR_INVALID_VALUE;
            }
        }
    }
    return CheckOperation(userId, deviceId, bundleName, moduleName, abilityName);
}

bool BundleTestTool::SetIsRemovableOperation(
    const std::string &bundleName, const std::string &moduleName, int isRemovable) const
{
    bool enable = true;
    if (isRemovable == 0) {
        enable = false;
    }
    APP_LOGD("bundleName: %{public}s, moduleName:%{public}s, enable:%{public}d", bundleName.c_str(), moduleName.c_str(),
        enable);
    auto ret = bundleMgrProxy_->SetModuleRemovable(bundleName, moduleName, enable);
    APP_LOGD("SetModuleRemovable end bundleName: %{public}d", ret);
    if (!ret) {
        APP_LOGE("SetIsRemovableOperation failed");
        return false;
    }
    return ret;
}

bool BundleTestTool::GetIsRemovableOperation(
    const std::string &bundleName, const std::string &moduleName, std::string &result) const
{
    APP_LOGD("bundleName: %{public}s, moduleName:%{public}s", bundleName.c_str(), moduleName.c_str());
    bool isRemovable = false;
    auto ret = bundleMgrProxy_->IsModuleRemovable(bundleName, moduleName, isRemovable);
    APP_LOGD("IsModuleRemovable end bundleName: %{public}s, isRemovable:%{public}d", bundleName.c_str(), isRemovable);
    result.append("isRemovable: " + std::to_string(isRemovable) + "\n");
    if (ret != ERR_OK) {
        APP_LOGE("IsModuleRemovable failed, ret: %{public}d", ret);
        return false;
    }
    return true;
}

bool BundleTestTool::CheckRemovableErrorOption(int option, int counter, const std::string &commandName)
{
    if (option == -1) {
        if (counter == 1) {
            if (strcmp(argv_[optind], cmd_.c_str()) == 0) {
                // 'bundle_test_tool setrm/getrm' with no option: bundle_test_tool setrm/getrm
                // 'bundle_test_tool setrm/getrm' with a wrong argument: bundle_test_tool setrm/getrm xxx
                APP_LOGD("'bundle_test_tool %{public}s' with no option.", commandName.c_str());
                resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                return false;
            }
        }
        return true;
    } else if (option == '?') {
        switch (optopt) {
            case 'i': {
                if (commandName == GET_RM) {
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
                    APP_LOGD("'bundle_test_tool %{public}s' with an unknown option.", commandName.c_str());
                    resultReceiver_.append(unknownOptionMsg);
                } else {
                    APP_LOGD("'bundle_test_tool %{public}s -i' with no argument.", commandName.c_str());
                    resultReceiver_.append("error: -i option requires a value.\n");
                }
                break;
            }
            case 'm': {
                APP_LOGD("'bundle_test_tool %{public}s -m' with no argument.", commandName.c_str());
                resultReceiver_.append("error: -m option requires a value.\n");
                break;
            }
            case 'n': {
                APP_LOGD("'bundle_test_tool %{public}s -n' with no argument.", commandName.c_str());
                resultReceiver_.append("error: -n option requires a value.\n");
                break;
            }
            default: {
                std::string unknownOption = "";
                std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
                APP_LOGD("'bundle_test_tool %{public}s' with an unknown option.", commandName.c_str());
                resultReceiver_.append(unknownOptionMsg);
                break;
            }
        }
    }
    return false;
}

bool BundleTestTool::CheckRemovableCorrectOption(
    int option, const std::string &commandName, int &isRemovable, std::string &name)
{
    bool ret = true;
    switch (option) {
        case 'h': {
            APP_LOGD("'bundle_test_tool %{public}s %{public}s'", commandName.c_str(), argv_[optind - 1]);
            ret = false;
            break;
        }
        case 'n': {
            name = optarg;
            APP_LOGD("'bundle_test_tool %{public}s -n %{public}s'", commandName.c_str(), argv_[optind - 1]);
            break;
        }
        case 'i': {
            if (commandName == GET_RM) {
                std::string unknownOption = "";
                std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
                APP_LOGD("'bundle_test_tool %{public}s' with an unknown option.", commandName.c_str());
                resultReceiver_.append(unknownOptionMsg);
                ret = false;
            } else if (OHOS::StrToInt(optarg, isRemovable)) {
                APP_LOGD("'bundle_test_tool %{public}s -i isRemovable:%{public}d, %{public}s'",
                    commandName.c_str(), isRemovable, argv_[optind - 1]);
            } else {
                APP_LOGE("bundle_test_tool setrm with error %{private}s", optarg);
                resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                ret = false;
            }
            break;
        }
        case 'm': {
            name = optarg;
            APP_LOGD("'bundle_test_tool %{public}s -m module-name:%{public}s, %{public}s'",
                commandName.c_str(), name.c_str(), argv_[optind - 1]);
            break;
        }
        default: {
            std::string unknownOption = "";
            std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
            APP_LOGD("'bundle_test_tool %{public}s' with an unknown option.", commandName.c_str());
            resultReceiver_.append(unknownOptionMsg);
            ret = false;
            break;
        }
    }
    return ret;
}

ErrCode BundleTestTool::RunAsSetRemovableCommand()
{
    int result = OHOS::ERR_OK;
    int counter = 0;
    int isRemovable = 0;
    std::string commandName = SET_RM;
    std::string name = "";
    std::string bundleName = "";
    std::string moduleName = "";
    APP_LOGD("RunAsSetCommand is start");
    while (true) {
        counter++;
        int32_t option = getopt_long(argc_, argv_, SHORT_OPTIONS.c_str(), LONG_OPTIONS, nullptr);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d, argv_[optind - 1]:%{public}s", option,
            optopt, optind, argv_[optind - 1]);
        if (option == -1 || option == '?') {
            result = !CheckRemovableErrorOption(option, counter, commandName)? OHOS::ERR_INVALID_VALUE : result;
            break;
        }
        result = !CheckRemovableCorrectOption(option, commandName, isRemovable, name)
            ? OHOS::ERR_INVALID_VALUE : result;
        moduleName = option == 'm' ? name : moduleName;
        bundleName = option == 'n' ? name : bundleName;
    }
    if (result == OHOS::ERR_OK) {
        if (resultReceiver_ == "" && (bundleName.size() == 0 || moduleName.size() == 0)) {
            APP_LOGD("'bundle_test_tool setrm' with not enough option.");
            resultReceiver_.append(HELP_MSG_NO_REMOVABLE_OPTION);
            result = OHOS::ERR_INVALID_VALUE;
        }
    }
    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_SET);
    } else {
        bool setResult = false;
        setResult = SetIsRemovableOperation(bundleName, moduleName, isRemovable);
        APP_LOGD("'bundle_test_tool setrm' isRemovable is %{public}d", isRemovable);
        resultReceiver_ = setResult ? STRING_SET_REMOVABLE_OK : STRING_SET_REMOVABLE_NG;
    }
    return result;
}

ErrCode BundleTestTool::RunAsGetRemovableCommand()
{
    int result = OHOS::ERR_OK;
    int counter = 0;
    std::string commandName = GET_RM;
    std::string name = "";
    std::string bundleName = "";
    std::string moduleName = "";
    APP_LOGD("RunAsGetRemovableCommand is start");
    while (true) {
        counter++;
        int32_t option = getopt_long(argc_, argv_, SHORT_OPTIONS.c_str(), LONG_OPTIONS, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1 || option == '?') {
            result = !CheckRemovableErrorOption(option, counter, commandName) ? OHOS::ERR_INVALID_VALUE : result;
            break;
        }
        int tempIsRem = 0;
        result = !CheckRemovableCorrectOption(option, commandName, tempIsRem, name)
            ? OHOS::ERR_INVALID_VALUE : result;
        moduleName = option == 'm' ? name : moduleName;
        bundleName = option == 'n' ? name : bundleName;
    }

    if (result == OHOS::ERR_OK) {
        if (resultReceiver_ == "" && (bundleName.size() == 0 || moduleName.size() == 0)) {
            APP_LOGD("'bundle_test_tool getrm' with no option.");
            resultReceiver_.append(HELP_MSG_NO_REMOVABLE_OPTION);
            result = OHOS::ERR_INVALID_VALUE;
        }
    }

    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_GET_REMOVABLE);
    } else {
        std::string results = "";
        GetIsRemovableOperation(bundleName, moduleName, results);
        if (results.empty()) {
            resultReceiver_.append(STRING_GET_REMOVABLE_NG);
            return result;
        }
        resultReceiver_.append(results);
    }
    return result;
}

bool BundleTestTool::CheckSandboxErrorOption(int option, int counter, const std::string &commandName)
{
    if (option == -1) {
        if (counter == 1) {
            if (strcmp(argv_[optind], cmd_.c_str()) == 0) {
                APP_LOGD("'bundle_test_tool %{public}s' with no option.", commandName.c_str());
                resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                return false;
            }
        }
        return true;
    } else if (option == '?') {
        switch (optopt) {
            case 'n':
            case 'u':
            case 'd':
            case 'a': {
                if ((commandName != INSTALL_SANDBOX && optopt == 'd') ||
                    (commandName == INSTALL_SANDBOX && optopt == 'a')) {
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
                    APP_LOGD("'bundle_test_tool %{public}s' with an unknown option.", commandName.c_str());
                    resultReceiver_.append(unknownOptionMsg);
                    break;
                }
                APP_LOGD("'bundle_test_tool %{public}s' -%{public}c with no argument.", commandName.c_str(), optopt);
                resultReceiver_.append("error: option requires a value.\n");
                break;
            }
            default: {
                std::string unknownOption = "";
                std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
                APP_LOGD("'bundle_test_tool %{public}s' with an unknown option.", commandName.c_str());
                resultReceiver_.append(unknownOptionMsg);
                break;
            }
        }
    }
    return false;
}

bool BundleTestTool::CheckSandboxCorrectOption(
    int option, const std::string &commandName, int &data, std::string &bundleName)
{
    bool ret = true;
    switch (option) {
        case 'h': {
            APP_LOGD("'bundle_test_tool %{public}s %{public}s'", commandName.c_str(), argv_[optind - 1]);
            ret = false;
            break;
        }
        case 'n': {
            APP_LOGD("'bundle_test_tool %{public}s %{public}s'", commandName.c_str(), argv_[optind - 1]);
            bundleName = optarg;
            break;
        }
        case 'u':
        case 'a':
        case 'd': {
            if ((commandName != INSTALL_SANDBOX && option == 'd') ||
                (commandName == INSTALL_SANDBOX && option == 'a')) {
                std::string unknownOption = "";
                std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
                APP_LOGD("'bundle_test_tool %{public}s' with an unknown option.", commandName.c_str());
                resultReceiver_.append(unknownOptionMsg);
                ret = false;
                break;
            }

            APP_LOGD("'bundle_test_tool %{public}s %{public}s %{public}s'", commandName.c_str(),
                argv_[optind - OFFSET_REQUIRED_ARGUMENT], optarg);

            if (!OHOS::StrToInt(optarg, data)) {
                if (option == 'u') {
                    APP_LOGE("bundle_test_tool %{public}s with error -u %{private}s", commandName.c_str(), optarg);
                } else if (option == 'a') {
                    APP_LOGE("bundle_test_tool %{public}s with error -a %{private}s", commandName.c_str(), optarg);
                } else {
                    APP_LOGE("bundle_test_tool %{public}s with error -d %{private}s", commandName.c_str(), optarg);
                }
                resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                ret = false;
            }
            break;
        }
        default: {
            ret = false;
            break;
        }
    }
    return ret;
}

ErrCode BundleTestTool::InstallSandboxOperation(
    const std::string &bundleName, const int32_t userId, const int32_t dlpType, int32_t &appIndex) const
{
    APP_LOGD("InstallSandboxOperation of bundleName %{public}s, dipType is %{public}d", bundleName.c_str(), dlpType);
    return bundleInstallerProxy_->InstallSandboxApp(bundleName, dlpType, userId, appIndex);
}

ErrCode BundleTestTool::RunAsInstallSandboxCommand()
{
    int result = OHOS::ERR_OK;
    int counter = 0;
    std::string commandName = INSTALL_SANDBOX;
    std::string bundleName = "";
    int32_t userId = 100;
    int32_t dlpType = 0;
    while (true) {
        counter++;
        int32_t option = getopt_long(argc_, argv_, SHORT_OPTIONS_SANDBOX.c_str(), LONG_OPTIONS_SANDBOX, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1 || option == '?') {
            result = !CheckSandboxErrorOption(option, counter, commandName) ? OHOS::ERR_INVALID_VALUE : result;
            break;
        } else if (option == 'u') {
            result = !CheckSandboxCorrectOption(option, commandName, userId, bundleName) ?
                OHOS::ERR_INVALID_VALUE : result;
        } else {
            result = !CheckSandboxCorrectOption(option, commandName, dlpType, bundleName) ?
                OHOS::ERR_INVALID_VALUE : result;
        }
    }

    if (result == OHOS::ERR_OK && bundleName == "") {
        resultReceiver_.append(HELP_MSG_NO_BUNDLE_NAME_OPTION);
        result = OHOS::ERR_INVALID_VALUE;
    } else {
        APP_LOGD("installSandbox app bundleName is %{public}s", bundleName.c_str());
    }

    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_INSTALL_SANDBOX);
        return result;
    }

    int32_t appIndex = 0;
    auto ret = InstallSandboxOperation(bundleName, userId, dlpType, appIndex);
    if (ret == OHOS::ERR_OK) {
        resultReceiver_.append(STRING_INSTALL_SANDBOX_SUCCESSFULLY);
    } else {
        resultReceiver_.append(STRING_INSTALL_SANDBOX_FAILED + "errCode is "+ std::to_string(ret) + "\n");
    }
    return result;
}

ErrCode BundleTestTool::UninstallSandboxOperation(const std::string &bundleName,
    const int32_t appIndex, const int32_t userId) const
{
    APP_LOGD("UninstallSandboxOperation of bundleName %{public}s_%{public}d", bundleName.c_str(), appIndex);
    return bundleInstallerProxy_->UninstallSandboxApp(bundleName, appIndex, userId);
}

ErrCode BundleTestTool::RunAsUninstallSandboxCommand()
{
    int result = OHOS::ERR_OK;
    int counter = 0;
    std::string bundleName = "";
    std::string commandName = UNINSTALL_SANDBOX;
    int32_t userId = 100;
    int32_t appIndex = -1;
    while (true) {
        counter++;
        int32_t option = getopt_long(argc_, argv_, SHORT_OPTIONS_SANDBOX.c_str(), LONG_OPTIONS_SANDBOX, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }

        if (option == -1 || option == '?') {
            result = !CheckSandboxErrorOption(option, counter, commandName) ? OHOS::ERR_INVALID_VALUE : result;
            break;
        } else if (option == 'u') {
            result = !CheckSandboxCorrectOption(option, commandName, userId, bundleName) ?
                OHOS::ERR_INVALID_VALUE : result;
        } else {
            result = !CheckSandboxCorrectOption(option, commandName, appIndex, bundleName) ?
                OHOS::ERR_INVALID_VALUE : result;
        }
    }

    if (result == OHOS::ERR_OK && bundleName == "") {
        resultReceiver_.append(HELP_MSG_NO_BUNDLE_NAME_OPTION);
        result = OHOS::ERR_INVALID_VALUE;
    } else {
        APP_LOGD("uninstallSandbox app bundleName is %{private}s", bundleName.c_str());
    }

    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_UNINSTALL_SANDBOX);
        return result;
    }

    auto ret = UninstallSandboxOperation(bundleName, appIndex, userId);
    if (ret == ERR_OK) {
        resultReceiver_.append(STRING_UNINSTALL_SANDBOX_SUCCESSFULLY);
    } else {
        resultReceiver_.append(STRING_UNINSTALL_SANDBOX_FAILED + "errCode is " + std::to_string(ret) + "\n");
    }
    return result;
}

ErrCode BundleTestTool::DumpSandboxBundleInfo(const std::string &bundleName, const int32_t appIndex,
    const int32_t userId, std::string &dumpResults)
{
    APP_LOGD("DumpSandboxBundleInfo of bundleName %{public}s_%{public}d", bundleName.c_str(), appIndex);
    BundleInfo bundleInfo;
    BundleMgrClient client;
    auto dumpRet = client.GetSandboxBundleInfo(bundleName, appIndex, userId, bundleInfo);
    if (dumpRet == ERR_OK) {
        nlohmann::json jsonObject = bundleInfo;
        dumpResults= jsonObject.dump(Constants::DUMP_INDENT);
    }
    return dumpRet;
}

ErrCode BundleTestTool::RunAsDumpSandboxCommand()
{
    int result = OHOS::ERR_OK;
    int counter = 0;
    std::string bundleName = "";
    std::string commandName = DUMP_SANDBOX;
    int32_t userId = 100;
    int32_t appIndex = -1;
    while (true) {
        counter++;
        int32_t option = getopt_long(argc_, argv_, SHORT_OPTIONS_SANDBOX.c_str(), LONG_OPTIONS_SANDBOX, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1 || option == '?') {
            result = !CheckSandboxErrorOption(option, counter, commandName) ? OHOS::ERR_INVALID_VALUE : result;
            break;
        } else if (option == 'u') {
            result = !CheckSandboxCorrectOption(option, commandName, userId, bundleName) ?
                OHOS::ERR_INVALID_VALUE : result;
        } else {
            result = !CheckSandboxCorrectOption(option, commandName, appIndex, bundleName) ?
                OHOS::ERR_INVALID_VALUE : result;
        }
    }

    if (result == OHOS::ERR_OK && bundleName == "") {
        resultReceiver_.append(HELP_MSG_NO_BUNDLE_NAME_OPTION);
        result = OHOS::ERR_INVALID_VALUE;
    } else {
        APP_LOGD("dumpSandbox app bundleName is %{public}s", bundleName.c_str());
    }

    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_DUMP_SANDBOX);
        return result;
    }

    std::string dumpRes = "";
    ErrCode ret = DumpSandboxBundleInfo(bundleName, appIndex, userId, dumpRes);
    if (ret == ERR_OK) {
        resultReceiver_.append(dumpRes);
    } else {
        resultReceiver_.append(STRING_DUMP_SANDBOX_FAILED + "errCode is "+ std::to_string(ret) + "\n");
    }
    return result;
}

ErrCode BundleTestTool::StringToInt(
    std::string optarg, const std::string &commandName, int &temp, bool &result)
{
    try {
        temp = std::stoi(optarg);
        APP_LOGD("bundle_test_tool %{public}s -u user-id:%{public}d, %{public}s",
            commandName.c_str(), temp, argv_[optind - 1]);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        result = false;
    }
    return OHOS::ERR_OK;
}

bool BundleTestTool::CheckGetStringCorrectOption(
    int option, const std::string &commandName, int &temp, std::string &name)
{
    bool ret = true;
    switch (option) {
        case 'h': {
            APP_LOGD("bundle_test_tool %{public}s %{public}s", commandName.c_str(), argv_[optind - 1]);
            ret = false;
            break;
        }
        case 'n': {
            name = optarg;
            APP_LOGD("bundle_test_tool %{public}s -n %{public}s", commandName.c_str(), argv_[optind - 1]);
            break;
        }
        case 'm': {
            name = optarg;
            APP_LOGD("bundle_test_tool %{public}s -m module-name:%{public}s, %{public}s",
                commandName.c_str(), name.c_str(), argv_[optind - 1]);
            break;
        }
        case 'u': {
            StringToInt(optarg, commandName, temp, ret);
            break;
        }
        case 'i': {
            StringToInt(optarg, commandName, temp, ret);
            break;
        }
        default: {
            std::string unknownOption = "";
            std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
            APP_LOGD("bundle_test_tool %{public}s with an unknown option.", commandName.c_str());
            resultReceiver_.append(unknownOptionMsg);
            ret = false;
            break;
        }
    }
    return ret;
}

ErrCode BundleTestTool::RunAsGetStringCommand()
{
    int result = OHOS::ERR_OK;
    int counter = 0;
    std::string commandName = "getStr";
    std::string name = "";
    std::string bundleName = "";
    std::string moduleName = "";
    int userId = 100;
    int labelId = 0;
    APP_LOGD("RunAsGetStringCommand is start");
    while (true) {
        counter++;
        int32_t option = getopt_long(argc_, argv_, SHORT_OPTIONS_GET.c_str(), LONG_OPTIONS_GET, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1) {
            // When scanning the first argument
            if ((counter == 1) && (strcmp(argv_[optind], cmd_.c_str()) == 0)) {
                // 'GetStringById' with no option: GetStringById
                // 'GetStringById' with a wrong argument: GetStringById
                APP_LOGD("bundle_test_tool getStr with no option.");
                resultReceiver_.append(HELP_MSG_NO_GETSTRING_OPTION);
                return OHOS::ERR_INVALID_VALUE;
            }
            break;
        }
        int temp = 0;
        result = !CheckGetStringCorrectOption(option, commandName, temp, name)
            ? OHOS::ERR_INVALID_VALUE : result;
        moduleName = option == 'm' ? name : moduleName;
        bundleName = option == 'n' ? name : bundleName;
        userId = option == 'u' ? temp : userId;
        labelId = option == 'i' ? temp : labelId;
    }

    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_GET_STRING);
    } else {
        std::string results = "";
        results = bundleMgrProxy_->GetStringById(bundleName, moduleName, labelId, userId);
        if (results.empty()) {
            resultReceiver_.append(STRING_GET_STRING_NG);
            return result;
        }
        resultReceiver_.append(results);
    }
    return result;
}

bool BundleTestTool::CheckGetIconCorrectOption(
    int option, const std::string &commandName, int &temp, std::string &name)
{
    bool ret = true;
    switch (option) {
        case 'h': {
            APP_LOGD("bundle_test_tool %{public}s %{public}s", commandName.c_str(), argv_[optind - 1]);
            ret = false;
            break;
        }
        case 'n': {
            name = optarg;
            APP_LOGD("bundle_test_tool %{public}s -n %{public}s", commandName.c_str(), argv_[optind - 1]);
            break;
        }
        case 'm': {
            name = optarg;
            APP_LOGD("bundle_test_tool %{public}s -m module-name:%{public}s, %{public}s",
                commandName.c_str(), name.c_str(), argv_[optind - 1]);
            break;
        }
        case 'u': {
            StringToInt(optarg, commandName, temp, ret);
            break;
        }
        case 'i': {
            StringToInt(optarg, commandName, temp, ret);
            break;
        }
        case 'd': {
            StringToInt(optarg, commandName, temp, ret);
            break;
        }
        default: {
            std::string unknownOption = "";
            std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
            APP_LOGD("bundle_test_tool %{public}s with an unknown option.", commandName.c_str());
            resultReceiver_.append(unknownOptionMsg);
            ret = false;
            break;
        }
    }
    return ret;
}

ErrCode BundleTestTool::RunAsGetIconCommand()
{
    int result = OHOS::ERR_OK;
    int counter = 0;
    std::string commandName = "getIcon";
    std::string name = "";
    std::string bundleName = "";
    std::string moduleName = "";
    int userId = 100;
    int iconId = 0;
    int density = 0;
    APP_LOGD("RunAsGetIconCommand is start");
    while (true) {
        counter++;
        int32_t option = getopt_long(argc_, argv_, SHORT_OPTIONS_GET.c_str(), LONG_OPTIONS_GET, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1) {
            // When scanning the first argument
            if ((counter == 1) && (strcmp(argv_[optind], cmd_.c_str()) == 0)) {
                // 'GetIconById' with no option: GetStringById
                // 'GetIconById' with a wrong argument: GetStringById
                APP_LOGD("bundle_test_tool getIcon with no option.");
                resultReceiver_.append(HELP_MSG_NO_GETICON_OPTION);
                return OHOS::ERR_INVALID_VALUE;
            }
            break;
        }
        int temp = 0;
        result = !CheckGetIconCorrectOption(option, commandName, temp, name)
            ? OHOS::ERR_INVALID_VALUE : result;
        moduleName = option == 'm' ? name : moduleName;
        bundleName = option == 'n' ? name : bundleName;
        userId = option == 'u' ? temp : userId;
        iconId = option == 'i' ? temp : iconId;
        density = option == 'd' ? temp : density;
    }

    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_GET_ICON);
    } else {
        std::string results = "";
        results = bundleMgrProxy_->GetIconById(bundleName, moduleName, iconId, density, userId);
        if (results.empty()) {
            resultReceiver_.append(STRING_GET_ICON_NG);
            return result;
        }
        resultReceiver_.append(results);
    }
    return result;
}

ErrCode BundleTestTool::CheckAddInstallRuleCorrectOption(int option, const std::string &commandName,
    std::vector<std::string> &appIds, int &controlRuleType, int &userId, int &euid)
{
    bool ret = true;
    switch (option) {
        case 'h': {
            APP_LOGD("bundle_test_tool %{public}s %{public}s", commandName.c_str(), argv_[optind - 1]);
            return OHOS::ERR_INVALID_VALUE;
        }
        case 'a': {
            std::string arrayAppId = optarg;
            std::stringstream array(arrayAppId);
            std::string object;
            while (getline(array, object, ',')) {
                appIds.emplace_back(object);
            }
            APP_LOGD("bundle_test_tool %{public}s -a %{public}s", commandName.c_str(), argv_[optind - 1]);
            break;
        }
        case 'e': {
            StringToInt(optarg, commandName, euid, ret);
            break;
        }
        case 't': {
            StringToInt(optarg, commandName, controlRuleType, ret);
            break;
        }
        case 'u': {
            StringToInt(optarg, commandName, userId, ret);
            break;
        }
        default: {
            std::string unknownOption = "";
            std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
            APP_LOGD("bundle_test_tool %{public}s with an unknown option.", commandName.c_str());
            resultReceiver_.append(unknownOptionMsg);
            return OHOS::ERR_INVALID_VALUE;
        }
    }
    return OHOS::ERR_OK;
}

// bundle_test_tool addAppInstallRule -a test1,test2 -t 1 -u 101 -e 537
ErrCode BundleTestTool::RunAsAddInstallRuleCommand()
{
    ErrCode result = OHOS::ERR_OK;
    int counter = 0;
    std::string commandName = "addAppInstallRule";
    std::vector<std::string> appIds;
    int euid = 537;
    int userId = 100;
    int ruleType = 0;
    APP_LOGD("RunAsAddInstallRuleCommand is start");
    while (true) {
        counter++;
        int option = getopt_long(argc_, argv_, SHORT_OPTIONS_RULE.c_str(), LONG_OPTIONS_RULE, nullptr);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1) {
            if ((counter == 1) && (strcmp(argv_[optind], cmd_.c_str()) == 0)) {
                resultReceiver_.append(HELP_MSG_NO_ADD_INSTALL_RULE_OPTION);
                return OHOS::ERR_INVALID_VALUE;
            }
            break;
        }
        result = CheckAddInstallRuleCorrectOption(option, commandName, appIds, ruleType, userId, euid);
        if (result != OHOS::ERR_OK) {
            resultReceiver_.append(HELP_MSG_ADD_INSTALL_RULE);
            return OHOS::ERR_INVALID_VALUE;
        }
    }
    seteuid(euid);
    auto rule = static_cast<AppInstallControlRuleType>(ruleType);
    auto appControlProxy = bundleMgrProxy_->GetAppControlProxy();
    if (!appControlProxy) {
        APP_LOGE("fail to get app control proxy.");
        return OHOS::ERR_INVALID_VALUE;
    }
    std::string appIdParam = "";
    for (auto param : appIds) {
        appIdParam = appIdParam.append(param) + ";";
    }
    APP_LOGI("appIds: %{public}s, controlRuleType: %{public}d, userId: %{public}d",
        appIdParam.c_str(), ruleType, userId);
    int32_t res = appControlProxy->AddAppInstallControlRule(appIds, rule, userId);
    APP_LOGI("AddAppInstallControlRule return code: %{public}d", res);
    if (res != OHOS::ERR_OK) {
        resultReceiver_.append(STRING_ADD_RULE_NG);
        return res;
    }
    resultReceiver_.append(std::to_string(res) + "\n");
    return result;
}

ErrCode BundleTestTool::CheckGetInstallRuleCorrectOption(int option, const std::string &commandName,
    int &controlRuleType, int &userId, int &euid)
{
    bool ret = true;
    switch (option) {
        case 'h': {
            APP_LOGD("bundle_test_tool %{public}s %{public}s", commandName.c_str(), argv_[optind - 1]);
            return OHOS::ERR_INVALID_VALUE;
        }
        case 'e': {
            StringToInt(optarg, commandName, euid, ret);
            break;
        }
        case 't': {
            StringToInt(optarg, commandName, controlRuleType, ret);
            break;
        }
        case 'u': {
            StringToInt(optarg, commandName, userId, ret);
            break;
        }
        default: {
            std::string unknownOption = "";
            std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
            APP_LOGD("bundle_test_tool %{public}s with an unknown option.", commandName.c_str());
            resultReceiver_.append(unknownOptionMsg);
            return OHOS::ERR_INVALID_VALUE;
        }
    }
    return OHOS::ERR_OK;
}

// bundle_test_tool getAppInstallRule -t 1 -u 101 -e 537
ErrCode BundleTestTool::RunAsGetInstallRuleCommand()
{
    ErrCode result = OHOS::ERR_OK;
    int counter = 0;
    std::string commandName = "getAppInstallRule";
    int euid = 537;
    int userId = 100;
    int ruleType = 0;
    APP_LOGD("RunAsGetInstallRuleCommand is start");
    while (true) {
        counter++;
        int option = getopt_long(argc_, argv_, SHORT_OPTIONS_RULE.c_str(), LONG_OPTIONS_RULE, nullptr);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1) {
            if ((counter == 1) && (strcmp(argv_[optind], cmd_.c_str()) == 0)) {
                resultReceiver_.append(HELP_MSG_NO_GET_INSTALL_RULE_OPTION);
                return OHOS::ERR_INVALID_VALUE;
            }
            break;
        }
        result = CheckGetInstallRuleCorrectOption(option, commandName, ruleType, userId, euid);
        if (result != OHOS::ERR_OK) {
            resultReceiver_.append(HELP_MSG_GET_INSTALL_RULE);
            return OHOS::ERR_INVALID_VALUE;
        }
    }
    seteuid(euid);
    auto appControlProxy = bundleMgrProxy_->GetAppControlProxy();
    if (!appControlProxy) {
        APP_LOGE("fail to get app control proxy.");
        return OHOS::ERR_INVALID_VALUE;
    }
    APP_LOGI("controlRuleType: %{public}d, userId: %{public}d", ruleType, userId);
    auto rule = static_cast<AppInstallControlRuleType>(ruleType);
    std::vector<std::string> appIds;
    int32_t res = appControlProxy->GetAppInstallControlRule(rule, userId, appIds);
    APP_LOGI("GetAppInstallControlRule return code: %{public}d", res);
    if (res != OHOS::ERR_OK) {
        resultReceiver_.append(STRING_GET_RULE_NG);
        return res;
    }
    std::string appIdParam = "";
    for (auto param : appIds) {
        appIdParam = appIdParam.append(param) + "; ";
    }
    resultReceiver_.append("appId : " + appIdParam + "\n");
    return result;
}

ErrCode BundleTestTool::CheckDeleteInstallRuleCorrectOption(int option, const std::string &commandName,
    int &controlRuleType, std::vector<std::string> &appIds, int &userId, int &euid)
{
    bool ret = true;
    switch (option) {
        case 'h': {
            APP_LOGD("bundle_test_tool %{public}s %{public}s", commandName.c_str(), argv_[optind - 1]);
            return OHOS::ERR_INVALID_VALUE;
        }
        case 'a': {
            std::string arrayAppId = optarg;
            std::stringstream array(arrayAppId);
            std::string object;
            while (getline(array, object, ',')) {
                appIds.emplace_back(object);
            }
            APP_LOGD("bundle_test_tool %{public}s -a %{public}s", commandName.c_str(), argv_[optind - 1]);
            break;
        }
        case 'e': {
            StringToInt(optarg, commandName, euid, ret);
            break;
        }
        case 't': {
            StringToInt(optarg, commandName, controlRuleType, ret);
            break;
        }
        case 'u': {
            StringToInt(optarg, commandName, userId, ret);
            break;
        }
        default: {
            std::string unknownOption = "";
            std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
            APP_LOGD("bundle_test_tool %{public}s with an unknown option.", commandName.c_str());
            resultReceiver_.append(unknownOptionMsg);
            return OHOS::ERR_INVALID_VALUE;
        }
    }
    return OHOS::ERR_OK;
}

// bundle_test_tool deleteAppInstallRule -a test1 -t 1 -u 101 -e 537
ErrCode BundleTestTool::RunAsDeleteInstallRuleCommand()
{
    ErrCode result = OHOS::ERR_OK;
    int counter = 0;
    int euid = 537;
    std::string commandName = "deleteAppInstallRule";
    std::vector<std::string> appIds;
    int ruleType = 0;
    int userId = 100;
    APP_LOGD("RunAsDeleteInstallRuleCommand is start");
    while (true) {
        counter++;
        int option = getopt_long(argc_, argv_, SHORT_OPTIONS_RULE.c_str(), LONG_OPTIONS_RULE, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1) {
            if ((counter == 1) && (strcmp(argv_[optind], cmd_.c_str()) == 0)) {
                resultReceiver_.append(HELP_MSG_NO_DELETE_INSTALL_RULE_OPTION);
                return OHOS::ERR_INVALID_VALUE;
            }
            break;
        }
        result = CheckDeleteInstallRuleCorrectOption(option, commandName, ruleType, appIds, userId, euid);
        if (result != OHOS::ERR_OK) {
            resultReceiver_.append(HELP_MSG_DELETE_INSTALL_RULE);
            return OHOS::ERR_INVALID_VALUE;
        }
    }
    seteuid(euid);
    auto appControlProxy = bundleMgrProxy_->GetAppControlProxy();
    if (!appControlProxy) {
        APP_LOGE("fail to get app control proxy.");
        return OHOS::ERR_INVALID_VALUE;
    }
    std::string appIdParam = "";
    for (auto param : appIds) {
        appIdParam = appIdParam.append(param) + ";";
    }
    APP_LOGI("appIds: %{public}s, userId: %{public}d", appIdParam.c_str(), userId);
    auto rule = static_cast<AppInstallControlRuleType>(ruleType);
    int32_t res = appControlProxy->DeleteAppInstallControlRule(rule, appIds, userId);
    APP_LOGI("DeleteAppInstallControlRule return code: %{public}d", res);
    if (res != OHOS::ERR_OK) {
        resultReceiver_.append(STRING_DELETE_RULE_NG);
        return res;
    }
    resultReceiver_.append(std::to_string(res) + "\n");
    return result;
}

ErrCode BundleTestTool::CheckCleanInstallRuleCorrectOption(
    int option, const std::string &commandName, int &controlRuleType, int &userId, int &euid)
{
    bool ret = true;
    switch (option) {
        case 'h': {
            APP_LOGD("bundle_test_tool %{public}s %{public}s", commandName.c_str(), argv_[optind - 1]);
            return OHOS::ERR_INVALID_VALUE;
        }
        case 'e': {
            StringToInt(optarg, commandName, euid, ret);
            break;
        }
        case 't': {
            StringToInt(optarg, commandName, controlRuleType, ret);
            break;
        }
        case 'u': {
            StringToInt(optarg, commandName, userId, ret);
            break;
        }
        default: {
            std::string unknownOption = "";
            std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
            APP_LOGD("bundle_test_tool %{public}s with an unknown option.", commandName.c_str());
            resultReceiver_.append(unknownOptionMsg);
            return OHOS::ERR_INVALID_VALUE;
        }
    }
    return OHOS::ERR_OK;
}

// bundle_test_tool cleanAppInstallRule -t 1 -u 101 -e 537
ErrCode BundleTestTool::RunAsCleanInstallRuleCommand()
{
    ErrCode result = OHOS::ERR_OK;
    int counter = 0;
    int euid = 537;
    std::string commandName = "cleanAppInstallRule";
    int userId = 100;
    int ruleType = 0;
    APP_LOGD("RunAsCleanInstallRuleCommand is start");
    while (true) {
        counter++;
        int option = getopt_long(argc_, argv_, SHORT_OPTIONS_RULE.c_str(), LONG_OPTIONS_RULE, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1) {
            if ((counter == 1) && (strcmp(argv_[optind], cmd_.c_str()) == 0)) {
                APP_LOGD("bundle_test_tool getRule with no option.");
                resultReceiver_.append(HELP_MSG_NO_CLEAN_INSTALL_RULE_OPTION);
                return OHOS::ERR_INVALID_VALUE;
            }
            break;
        }
        result = CheckCleanInstallRuleCorrectOption(option, commandName, ruleType, userId, euid);
        if (result != OHOS::ERR_OK) {
            resultReceiver_.append(HELP_MSG_NO_CLEAN_INSTALL_RULE_OPTION);
            return OHOS::ERR_INVALID_VALUE;
        }
    }
    seteuid(euid);
    auto rule = static_cast<AppInstallControlRuleType>(ruleType);
    auto appControlProxy = bundleMgrProxy_->GetAppControlProxy();
    if (!appControlProxy) {
        APP_LOGE("fail to get app control proxy.");
        return OHOS::ERR_INVALID_VALUE;
    }
    APP_LOGI("controlRuleType: %{public}d, userId: %{public}d", ruleType, userId);
    int32_t res = appControlProxy->DeleteAppInstallControlRule(rule, userId);
    APP_LOGI("DeleteAppInstallControlRule clean return code: %{public}d", res);
    if (res != OHOS::ERR_OK) {
        resultReceiver_.append(STRING_DELETE_RULE_NG);
        return res;
    }
    resultReceiver_.append(std::to_string(res) + "\n");
    return result;
}

ErrCode BundleTestTool::CheckAppRunningRuleCorrectOption(int option, const std::string &commandName,
    std::vector<AppRunningControlRule> &controlRule, int &userId, int &euid)
{
    bool ret = true;
    switch (option) {
        case 'h': {
            APP_LOGD("bundle_test_tool %{public}s %{public}s", commandName.c_str(), argv_[optind - 1]);
            return OHOS::ERR_INVALID_VALUE;
        }
        case 'c': {
            std::string arrayJsonRule = optarg;
            std::stringstream array(arrayJsonRule);
            std::string object;
            while (getline(array, object, ';')) {
                size_t pos1 = object.find("appId");
                size_t pos2 = object.find("controlMessage");
                size_t pos3 = object.find(":", pos2);
                if ((pos1 == std::string::npos) || (pos2 == std::string::npos)) {
                    return OHOS::ERR_INVALID_VALUE;
                }
                std::string appId = object.substr(pos1+6, pos2-pos1-7);
                std::string controlMessage = object.substr(pos3+1);
                AppRunningControlRule rule;
                rule.appId = appId;
                rule.controlMessage = controlMessage;
                controlRule.emplace_back(rule);
            }
            break;
        }
        case 'e': {
            StringToInt(optarg, commandName, euid, ret);
            break;
        }
        case 'u': {
            StringToInt(optarg, commandName, userId, ret);
            break;
        }
        default: {
            std::string unknownOption = "";
            std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
            APP_LOGD("bundle_test_tool %{public}s with an unknown option.", commandName.c_str());
            resultReceiver_.append(unknownOptionMsg);
            return OHOS::ERR_INVALID_VALUE;
        }
    }
    return OHOS::ERR_OK;
}

// bundle_test_tool addAppRunningRule -c appId:id1,controlMessage:msg1;appId:id2,controlMessage:msg2
// -u 101 -e 537
ErrCode BundleTestTool::RunAsAddAppRunningRuleCommand()
{
    ErrCode result = OHOS::ERR_OK;
    int counter = 0;
    int euid = 537;
    std::string commandName = "addAppRunningRule";
    int userId = 100;
    std::vector<AppRunningControlRule> controlRule;
    APP_LOGD("RunAsAddAppRunningRuleCommand is start");
    while (true) {
        counter++;
        int option = getopt_long(argc_, argv_, SHORT_OPTIONS_RULE.c_str(), LONG_OPTIONS_RULE, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1) {
            if ((counter == 1) && (strcmp(argv_[optind], cmd_.c_str()) == 0)) {
                APP_LOGD("bundle_test_tool getRule with no option.");
                resultReceiver_.append(HELP_MSG_NO_APP_RUNNING_RULE_OPTION);
                return OHOS::ERR_INVALID_VALUE;
            }
            break;
        }
        result = CheckAppRunningRuleCorrectOption(option, commandName, controlRule, userId, euid);
        if (result != OHOS::ERR_OK) {
            resultReceiver_.append(HELP_MSG_ADD_APP_RUNNING_RULE);
            return OHOS::ERR_INVALID_VALUE;
        }
    }
    seteuid(euid);
    auto appControlProxy = bundleMgrProxy_->GetAppControlProxy();
    if (!appControlProxy) {
        APP_LOGE("fail to get app control proxy.");
        return OHOS::ERR_INVALID_VALUE;
    }
    std::string appIdParam = "";
    for (auto param : controlRule) {
        appIdParam = appIdParam.append("appId:"+ param.appId + ":" + "message" + param.controlMessage);
    }
    APP_LOGI("appRunningControlRule: %{public}s, userId: %{public}d", appIdParam.c_str(), userId);
    int32_t res = appControlProxy->AddAppRunningControlRule(controlRule, userId);
    if (res != OHOS::ERR_OK) {
        resultReceiver_.append(STRING_ADD_RULE_NG);
        return res;
    }
    resultReceiver_.append(std::to_string(res) + "\n");
    return result;
}

// bundle_test_tool deleteAppRunningRule -c appId:101,controlMessage:msg1 -u 101 -e 537
ErrCode BundleTestTool::RunAsDeleteAppRunningRuleCommand()
{
    ErrCode result = OHOS::ERR_OK;
    int counter = 0;
    int euid = 537;
    std::string commandName = "addAppRunningRule";
    int userId = 100;
    std::vector<AppRunningControlRule> controlRule;
    APP_LOGD("RunAsDeleteAppRunningRuleCommand is start");
    while (true) {
        counter++;
        int option = getopt_long(argc_, argv_, SHORT_OPTIONS_RULE.c_str(), LONG_OPTIONS_RULE, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1) {
            if ((counter == 1) && (strcmp(argv_[optind], cmd_.c_str()) == 0)) {
                APP_LOGD("bundle_test_tool getRule with no option.");
                resultReceiver_.append(HELP_MSG_NO_APP_RUNNING_RULE_OPTION);
                return OHOS::ERR_INVALID_VALUE;
            }
            break;
        }
        result = CheckAppRunningRuleCorrectOption(option, commandName, controlRule, userId, euid);
        if (result != OHOS::ERR_OK) {
            resultReceiver_.append(HELP_MSG_DELETE_APP_RUNNING_RULE);
            return OHOS::ERR_INVALID_VALUE;
        }
    }
    seteuid(euid);
    auto appControlProxy = bundleMgrProxy_->GetAppControlProxy();
    if (!appControlProxy) {
        APP_LOGE("fail to get app control proxy.");
        return OHOS::ERR_INVALID_VALUE;
    }
    std::string appIdParam = "";
    for (auto param : controlRule) {
        appIdParam = appIdParam.append("appId:"+ param.appId + ":" + "message" + param.controlMessage);
    }
    APP_LOGI("appRunningControlRule: %{public}s, userId: %{public}d", appIdParam.c_str(), userId);
    int32_t res = appControlProxy->DeleteAppRunningControlRule(controlRule, userId);
    if (res != OHOS::ERR_OK) {
        resultReceiver_.append(STRING_DELETE_RULE_NG);
        return res;
    }
    resultReceiver_.append(std::to_string(res) + "\n");
    return result;
}

ErrCode BundleTestTool::CheckCleanAppRunningRuleCorrectOption(
    int option, const std::string &commandName, int &userId, int &euid)
{
    bool ret = true;
    switch (option) {
        case 'h': {
            APP_LOGD("bundle_test_tool %{public}s %{public}s", commandName.c_str(), argv_[optind - 1]);
            return OHOS::ERR_INVALID_VALUE;
        }
        case 'e': {
            StringToInt(optarg, commandName, euid, ret);
            break;
        }
        case 'u': {
            StringToInt(optarg, commandName, userId, ret);
            break;
        }
        default: {
            std::string unknownOption = "";
            std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
            APP_LOGD("bundle_test_tool %{public}s with an unknown option.", commandName.c_str());
            resultReceiver_.append(unknownOptionMsg);
            return OHOS::ERR_INVALID_VALUE;
        }
    }
    return OHOS::ERR_OK;
}

// bundle_test_tool cleanAppRunningRule -u 101 -e 537
ErrCode BundleTestTool::RunAsCleanAppRunningRuleCommand()
{
    ErrCode result = OHOS::ERR_OK;
    int counter = 0;
    int euid = 537;
    std::string commandName = "addAppRunningRule";
    int userId = 100;
    APP_LOGD("RunAsCleanAppRunningRuleCommand is start");
    while (true) {
        counter++;
        int option = getopt_long(argc_, argv_, SHORT_OPTIONS_RULE.c_str(), LONG_OPTIONS_RULE, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1) {
            if ((counter == 1) && (strcmp(argv_[optind], cmd_.c_str()) == 0)) {
                APP_LOGD("bundle_test_tool getRule with no option.");
                resultReceiver_.append(HELP_MSG_NO_CLEAN_APP_RUNNING_RULE_OPTION);
                return OHOS::ERR_INVALID_VALUE;
            }
            break;
        }
        result = CheckCleanAppRunningRuleCorrectOption(option, commandName, userId, euid);
        if (result != OHOS::ERR_OK) {
            resultReceiver_.append(HELP_MSG_CLEAN_APP_RUNNING_RULE);
            return OHOS::ERR_INVALID_VALUE;
        }
    }
    seteuid(euid);
    auto appControlProxy = bundleMgrProxy_->GetAppControlProxy();
    if (!appControlProxy) {
        APP_LOGE("fail to get app control proxy.");
        return OHOS::ERR_INVALID_VALUE;
    }
    APP_LOGI("userId: %{public}d", userId);
    int32_t res = appControlProxy->DeleteAppRunningControlRule(userId);
    if (res != OHOS::ERR_OK) {
        resultReceiver_.append(STRING_DELETE_RULE_NG);
        return res;
    }
    resultReceiver_.append(std::to_string(res) + "\n");
    return result;
}

ErrCode BundleTestTool::CheckGetAppRunningRuleCorrectOption(int option, const std::string &commandName,
    int32_t &userId, int &euid)
{
    bool ret = true;
    switch (option) {
        case 'h': {
            APP_LOGD("bundle_test_tool %{public}s %{public}s", commandName.c_str(), argv_[optind - 1]);
            return OHOS::ERR_INVALID_VALUE;
        }
        case 'e': {
            StringToInt(optarg, commandName, euid, ret);
            break;
        }
        case 'u': {
            StringToInt(optarg, commandName, userId, ret);
            break;
        }
        default: {
            std::string unknownOption = "";
            std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
            APP_LOGD("bundle_test_tool %{public}s with an unknown option.", commandName.c_str());
            resultReceiver_.append(unknownOptionMsg);
            return OHOS::ERR_INVALID_VALUE;
        }
    }
    return OHOS::ERR_OK ;
}

// bundle_test_tool getAppRunningControlRule -u 101 -e 537
ErrCode BundleTestTool::RunAsGetAppRunningControlRuleCommand()
{
    ErrCode result = OHOS::ERR_OK;
    int counter = 0;
    int euid = 537;
    std::string commandName = "addAppRunningRule";
    int userId = 100;
    APP_LOGD("RunAsGetAppRunningControlRuleCommand is start");
    while (true) {
        counter++;
        int option = getopt_long(argc_, argv_, SHORT_OPTIONS_RULE.c_str(), LONG_OPTIONS_RULE, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1) {
            if ((counter == 1) && (strcmp(argv_[optind], cmd_.c_str()) == 0)) {
                APP_LOGD("bundle_test_tool getRule with no option.");
                resultReceiver_.append(HELP_MSG_NO_GET_ALL_APP_RUNNING_RULE_OPTION);
                return OHOS::ERR_INVALID_VALUE;
            }
            break;
        }
        result = CheckGetAppRunningRuleCorrectOption(option, commandName, userId, euid);
        if (result != OHOS::ERR_OK) {
            resultReceiver_.append(HELP_MSG_GET_APP_RUNNING_RULE);
            return OHOS::ERR_INVALID_VALUE;
        }
    }
    seteuid(euid);
    auto appControlProxy = bundleMgrProxy_->GetAppControlProxy();
    if (!appControlProxy) {
        APP_LOGE("fail to get app control proxy.");
        return OHOS::ERR_INVALID_VALUE;
    }
    APP_LOGI("userId: %{public}d", userId);
    std::vector<std::string> appIds;
    int32_t res = appControlProxy->GetAppRunningControlRule(userId, appIds);
    if (res != OHOS::ERR_OK) {
        resultReceiver_.append(STRING_GET_RULE_NG);
        return res;
    }
    std::string appIdParam = "";
    for (auto param : appIds) {
        appIdParam = appIdParam.append(param) + "; ";
    }
    resultReceiver_.append("appId : " + appIdParam + "\n");
    return result;
}

ErrCode BundleTestTool::CheckGetAppRunningRuleResultCorrectOption(int option, const std::string &commandName,
    std::string &bundleName, int32_t &userId, int &euid)
{
    bool ret = true;
    switch (option) {
        case 'h': {
            APP_LOGD("bundle_test_tool %{public}s %{public}s", commandName.c_str(), argv_[optind - 1]);
            return OHOS::ERR_INVALID_VALUE;
        }
        case 'e': {
            StringToInt(optarg, commandName, euid, ret);
            break;
        }
        case 'n': {
            APP_LOGD("'bundle_test_tool %{public}s %{public}s'", commandName.c_str(), argv_[optind - 1]);
            bundleName = optarg;
            break;
        }
        case 'u': {
            StringToInt(optarg, commandName, userId, ret);
            break;
        }
        default: {
            std::string unknownOption = "";
            std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
            APP_LOGD("bundle_test_tool %{public}s with an unknown option.", commandName.c_str());
            resultReceiver_.append(unknownOptionMsg);
            return OHOS::ERR_INVALID_VALUE;
        }
    }
    return OHOS::ERR_OK;
}

// bundle_test_tool getAppRunningControlRuleResult -n com.ohos.example -e 537
ErrCode BundleTestTool::RunAsGetAppRunningControlRuleResultCommand()
{
    ErrCode result = OHOS::ERR_OK;
    int counter = 0;
    int euid = 537;
    std::string commandName = "addAppRunningRule";
    int userId = 100;
    std::string bundleName;
    APP_LOGD("RunAsGetAppRunningControlRuleResultCommand is start");
    while (true) {
        counter++;
        int option = getopt_long(argc_, argv_, SHORT_OPTIONS_RULE.c_str(), LONG_OPTIONS_RULE, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1) {
            if ((counter == 1) && (strcmp(argv_[optind], cmd_.c_str()) == 0)) {
                APP_LOGD("bundle_test_tool getRule with no option.");
                resultReceiver_.append(HELP_MSG_NO_GET_APP_RUNNING_RULE_OPTION);
                return OHOS::ERR_INVALID_VALUE;
            }
            break;
        }
        result = CheckGetAppRunningRuleResultCorrectOption(option, commandName, bundleName, userId, euid);
        if (result != OHOS::ERR_OK) {
            resultReceiver_.append(HELP_MSG_GET_APP_RUNNING_RESULT_RULE);
            return OHOS::ERR_INVALID_VALUE;
        }
    }
    seteuid(euid);
    auto appControlProxy = bundleMgrProxy_->GetAppControlProxy();
    if (!appControlProxy) {
        APP_LOGE("fail to get app control proxy.");
        return OHOS::ERR_INVALID_VALUE;
    }
    AppRunningControlRuleResult ruleResult;
    APP_LOGI("bundleName: %{public}s, userId: %{public}d", bundleName.c_str(), userId);
    int32_t res = appControlProxy->GetAppRunningControlRule(bundleName, userId, ruleResult);
    if (res != OHOS::ERR_OK) {
        APP_LOGI("GetAppRunningControlRule result: %{public}d", res);
        resultReceiver_.append("message:" + ruleResult.controlMessage + " bundle:notFind" + "\n");
        return res;
    }
    resultReceiver_.append("message:" + ruleResult.controlMessage + "\n");
    if (ruleResult.controlWant != nullptr) {
        resultReceiver_.append("controlWant:" + ruleResult.controlWant->ToString() + "\n");
    } else {
        resultReceiver_.append("controlWant: nullptr \n");
    }
    return result;
}

ErrCode BundleTestTool::RunAsDeployQuickFix()
{
    int32_t result = OHOS::ERR_OK;
    int32_t counter = 0;
    int32_t index = 0;
    std::vector<std::string> quickFixPaths;
    while (true) {
        counter++;
        int32_t option = getopt_long(argc_, argv_, SHORT_OPTIONS_QUICK_FIX.c_str(), LONG_OPTIONS_QUICK_FIX, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }

        if (option == -1 || option == '?') {
            if (counter == 1 && strcmp(argv_[optind], cmd_.c_str()) == 0) {
                resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            if (optopt == 'p') {
                // 'bm deployQuickFix --patch-path' with no argument
                resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            break;
        }

        if (option == 'p') {
            APP_LOGD("'bm deployQuickFix -p %{public}s'", argv_[optind - 1]);
            quickFixPaths.emplace_back(optarg);
            index = optind;
            continue;
        }
        result = OHOS::ERR_INVALID_VALUE;
        break;
    }

    if (result != OHOS::ERR_OK || GetQuickFixPath(index, quickFixPaths) != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_DEPLOY_QUICK_FIX);
        return result;
    }

    std::shared_ptr<QuickFixResult> deployRes = nullptr;
    result = DeployQuickFix(quickFixPaths, deployRes);
    resultReceiver_ = (result == OHOS::ERR_OK) ? STRING_DEPLOY_QUICK_FIX_OK : STRING_DEPLOY_QUICK_FIX_NG;
    resultReceiver_ += GetResMsg(result, deployRes);

    return result;
}

ErrCode BundleTestTool::GetQuickFixPath(int32_t index, std::vector<std::string>& quickFixPaths) const
{
    APP_LOGI("GetQuickFixPath start");
    for (; index < argc_ && index >= INDEX_OFFSET; ++index) {
        if (argList_[index - INDEX_OFFSET] == "-p" || argList_[index - INDEX_OFFSET] == "--patch-path") {
            break;
        }

        std::string innerPath = argList_[index - INDEX_OFFSET];
        if (innerPath.empty() || innerPath == "-p" || innerPath == "--patch-path") {
            quickFixPaths.clear();
            return OHOS::ERR_INVALID_VALUE;
        }
        APP_LOGD("GetQuickFixPath is %{public}s'", innerPath.c_str());
        quickFixPaths.emplace_back(innerPath);
    }
    return OHOS::ERR_OK;
}

ErrCode BundleTestTool::RunAsSwitchQuickFix()
{
    int32_t result = OHOS::ERR_OK;
    int32_t counter = 0;
    int32_t enable = -1;
    std::string bundleName;
    while (true) {
        counter++;
        int32_t option = getopt_long(argc_, argv_, SHORT_OPTIONS_QUICK_FIX.c_str(), LONG_OPTIONS_QUICK_FIX, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }

        if (option == -1 || option == '?') {
            if (counter == 1 && strcmp(argv_[optind], cmd_.c_str()) == 0) {
                resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            if (optopt == 'n' || optopt == 'e') {
                // 'bm switchQuickFix -n -e' with no argument
                resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            break;
        }

        if (option == 'n') {
            APP_LOGD("'bm switchQuickFix -n %{public}s'", argv_[optind - 1]);
            bundleName = optarg;
            continue;
        }
        if (option == 'e' && OHOS::StrToInt(optarg, enable)) {
            APP_LOGD("'bm switchQuickFix -e %{public}s'", argv_[optind - 1]);
            continue;
        }
        result = OHOS::ERR_INVALID_VALUE;
        break;
    }

    if ((result != OHOS::ERR_OK) || (enable < 0) || (enable > 1)) {
        resultReceiver_.append(HELP_MSG_SWITCH_QUICK_FIX);
        return result;
    }
    std::shared_ptr<QuickFixResult> switchRes = nullptr;
    result = SwitchQuickFix(bundleName, enable, switchRes);
    resultReceiver_ = (result == OHOS::ERR_OK) ? STRING_SWITCH_QUICK_FIX_OK : STRING_SWITCH_QUICK_FIX_NG;
    resultReceiver_ += GetResMsg(result, switchRes);

    return result;
}

ErrCode BundleTestTool::RunAsDeleteQuickFix()
{
    int32_t result = OHOS::ERR_OK;
    int32_t counter = 0;
    std::string bundleName;
    while (true) {
        counter++;
        int32_t option = getopt_long(argc_, argv_, SHORT_OPTIONS_QUICK_FIX.c_str(), LONG_OPTIONS_QUICK_FIX, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }

        if (option == -1 || option == '?') {
            if (counter == 1 && strcmp(argv_[optind], cmd_.c_str()) == 0) {
                resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            if (optopt == 'n') {
                // 'bm deleteQuickFix -n' with no argument
                resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            break;
        }

        if (option == 'n') {
            APP_LOGD("'bm deleteQuickFix -n %{public}s'", argv_[optind - 1]);
            bundleName = optarg;
            continue;
        }
        result = OHOS::ERR_INVALID_VALUE;
        break;
    }

    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_SWITCH_QUICK_FIX);
        return result;
    }
    std::shared_ptr<QuickFixResult> deleteRes = nullptr;
    result = DeleteQuickFix(bundleName, deleteRes);
    resultReceiver_ = (result == OHOS::ERR_OK) ? STRING_DELETE_QUICK_FIX_OK : STRING_DELETE_QUICK_FIX_NG;
    resultReceiver_ += GetResMsg(result, deleteRes);

    return result;
}

ErrCode BundleTestTool::DeployQuickFix(const std::vector<std::string> &quickFixPaths,
    std::shared_ptr<QuickFixResult> &quickFixRes)
{
#ifdef BUNDLE_FRAMEWORK_QUICK_FIX
    std::set<std::string> realPathSet;
    for (const auto &quickFixPath : quickFixPaths) {
        std::string realPath;
        if (!PathToRealPath(quickFixPath, realPath)) {
            APP_LOGW("quickFixPath %{public}s is invalid", quickFixPath.c_str());
            continue;
        }
        APP_LOGD("realPath is %{public}s", realPath.c_str());
        realPathSet.insert(realPath);
    }
    std::vector<std::string> pathVec(realPathSet.begin(), realPathSet.end());

    sptr<QuickFixStatusCallbackHostlmpl> callback(new (std::nothrow) QuickFixStatusCallbackHostlmpl());
    if (callback == nullptr || bundleMgrProxy_ == nullptr) {
        APP_LOGE("callback or bundleMgrProxy is null");
        return ERR_BUNDLEMANAGER_QUICK_FIX_INTERNAL_ERROR;
    }
    sptr<BundleDeathRecipient> recipient(new (std::nothrow) BundleDeathRecipient(nullptr, callback));
    if (recipient == nullptr) {
        APP_LOGE("recipient is null");
        return ERR_BUNDLEMANAGER_QUICK_FIX_INTERNAL_ERROR;
    }
    bundleMgrProxy_->AsObject()->AddDeathRecipient(recipient);
    auto quickFixProxy = bundleMgrProxy_->GetQuickFixManagerProxy();
    if (quickFixProxy == nullptr) {
        APP_LOGE("quickFixProxy is null");
        return ERR_BUNDLEMANAGER_QUICK_FIX_INTERNAL_ERROR;
    }
    std::vector<std::string> destFiles;
    auto res = quickFixProxy->CopyFiles(pathVec, destFiles);
    if (res != ERR_OK) {
        APP_LOGE("Copy files failed with %{public}d.", res);
        return res;
    }
    res = quickFixProxy->DeployQuickFix(destFiles, callback);
    if (res != ERR_OK) {
        APP_LOGE("DeployQuickFix failed");
        return res;
    }

    return callback->GetResultCode(quickFixRes);
#else
    return ERR_BUNDLEMANAGER_FEATURE_IS_NOT_SUPPORTED;
#endif
}

ErrCode BundleTestTool::SwitchQuickFix(const std::string &bundleName, int32_t enable,
    std::shared_ptr<QuickFixResult> &quickFixRes)
{
    APP_LOGD("SwitchQuickFix bundleName: %{public}s, enable: %{public}d", bundleName.c_str(), enable);
#ifdef BUNDLE_FRAMEWORK_QUICK_FIX
    sptr<QuickFixStatusCallbackHostlmpl> callback(new (std::nothrow) QuickFixStatusCallbackHostlmpl());
    if (callback == nullptr || bundleMgrProxy_ == nullptr) {
        APP_LOGE("callback or bundleMgrProxy is null");
        return ERR_BUNDLEMANAGER_QUICK_FIX_INTERNAL_ERROR;
    }
    sptr<BundleDeathRecipient> recipient(new (std::nothrow) BundleDeathRecipient(nullptr, callback));
    if (recipient == nullptr) {
        APP_LOGE("recipient is null");
        return ERR_BUNDLEMANAGER_QUICK_FIX_INTERNAL_ERROR;
    }
    bundleMgrProxy_->AsObject()->AddDeathRecipient(recipient);
    auto quickFixProxy = bundleMgrProxy_->GetQuickFixManagerProxy();
    if (quickFixProxy == nullptr) {
        APP_LOGE("quickFixProxy is null");
        return ERR_BUNDLEMANAGER_QUICK_FIX_INTERNAL_ERROR;
    }
    bool enableFlag = (enable == 0) ? false : true;
    auto res = quickFixProxy->SwitchQuickFix(bundleName, enableFlag, callback);
    if (res != ERR_OK) {
        APP_LOGE("SwitchQuickFix failed");
        return res;
    }
    return callback->GetResultCode(quickFixRes);
#else
    return ERR_BUNDLEMANAGER_FEATURE_IS_NOT_SUPPORTED;
#endif
}

ErrCode BundleTestTool::DeleteQuickFix(const std::string &bundleName,
    std::shared_ptr<QuickFixResult> &quickFixRes)
{
    APP_LOGD("DeleteQuickFix bundleName: %{public}s", bundleName.c_str());
#ifdef BUNDLE_FRAMEWORK_QUICK_FIX
    sptr<QuickFixStatusCallbackHostlmpl> callback(new (std::nothrow) QuickFixStatusCallbackHostlmpl());
    if (callback == nullptr || bundleMgrProxy_ == nullptr) {
        APP_LOGE("callback or bundleMgrProxy is null");
        return ERR_BUNDLEMANAGER_QUICK_FIX_INTERNAL_ERROR;
    }
    sptr<BundleDeathRecipient> recipient(new (std::nothrow) BundleDeathRecipient(nullptr, callback));
    if (recipient == nullptr) {
        APP_LOGE("recipient is null");
        return ERR_BUNDLEMANAGER_QUICK_FIX_INTERNAL_ERROR;
    }
    bundleMgrProxy_->AsObject()->AddDeathRecipient(recipient);
    auto quickFixProxy = bundleMgrProxy_->GetQuickFixManagerProxy();
    if (quickFixProxy == nullptr) {
        APP_LOGE("quickFixProxy is null");
        return ERR_BUNDLEMANAGER_QUICK_FIX_INTERNAL_ERROR;
    }
    auto res = quickFixProxy->DeleteQuickFix(bundleName, callback);
    if (res != ERR_OK) {
        APP_LOGE("DeleteQuickFix failed");
        return res;
    }
    return callback->GetResultCode(quickFixRes);
#else
    return ERR_BUNDLEMANAGER_FEATURE_IS_NOT_SUPPORTED;
#endif
}

std::string BundleTestTool::GetResMsg(int32_t code)
{
    std::unordered_map<int32_t, std::string> quickFixMsgMap;
    CreateQuickFixMsgMap(quickFixMsgMap);
    if (quickFixMsgMap.find(code) != quickFixMsgMap.end()) {
        return quickFixMsgMap.at(code);
    }
    return MSG_ERR_BUNDLEMANAGER_QUICK_FIX_UNKOWN;
}

std::string BundleTestTool::GetResMsg(int32_t code, const std::shared_ptr<QuickFixResult> &quickFixRes)
{
    std::string resMsg;
    std::unordered_map<int32_t, std::string> quickFixMsgMap;
    CreateQuickFixMsgMap(quickFixMsgMap);
    if (quickFixMsgMap.find(code) != quickFixMsgMap.end()) {
        resMsg += quickFixMsgMap.at(code);
    } else {
        resMsg += MSG_ERR_BUNDLEMANAGER_QUICK_FIX_UNKOWN;
    }
    if (quickFixRes != nullptr) {
        resMsg += quickFixRes->ToString() + "\n";
    }
    return resMsg;
}

ErrCode BundleTestTool::RunAsSetDebugMode()
{
    int32_t result = OHOS::ERR_OK;
    int32_t counter = 0;
    int32_t enable = -1;
    while (true) {
        counter++;
        int32_t option = getopt_long(argc_, argv_, SHORT_OPTIONS_DEBUG_MODE.c_str(), LONG_OPTIONS_DEBUG_MODE, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }

        if (option == -1 || option == '?') {
            if (counter == 1 && strcmp(argv_[optind], cmd_.c_str()) == 0) {
                resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            if (optopt == 'e') {
                // 'bundle_test_tool setDebugMode -e' with no argument
                resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            break;
        }

        if (option == 'e' && OHOS::StrToInt(optarg, enable)) {
            APP_LOGD("'bundle_test_tool setDebugMode -e %{public}s'", argv_[optind - 1]);
            continue;
        }
        result = OHOS::ERR_INVALID_VALUE;
        break;
    }

    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_SET_DEBUG_MODE);
        return result;
    }
    ErrCode setResult = SetDebugMode(enable);
    if (setResult == OHOS::ERR_OK) {
        resultReceiver_ = STRING_SET_DEBUG_MODE_OK;
    } else {
        resultReceiver_ = STRING_SET_DEBUG_MODE_NG + GetResMsg(setResult);
    }
    return setResult;
}

ErrCode BundleTestTool::SetDebugMode(int32_t debugMode)
{
    if (debugMode != 0 && debugMode != 1) {
        APP_LOGE("SetDebugMode param is invalid");
        return ERR_BUNDLEMANAGER_SET_DEBUG_MODE_INVALID_PARAM;
    }
    bool enable = debugMode == 0 ? false : true;
    if (bundleMgrProxy_ == nullptr) {
        APP_LOGE("bundleMgrProxy_ is nullptr");
        return ERR_BUNDLEMANAGER_SET_DEBUG_MODE_INTERNAL_ERROR;
    }
    return bundleMgrProxy_->SetDebugMode(enable);
}

ErrCode BundleTestTool::RunAsGetBundleStats()
{
    int32_t result = OHOS::ERR_OK;
    int32_t counter = 0;
    std::string bundleName = "";
    int32_t userId = Constants::UNSPECIFIED_USERID;
    while (true) {
        counter++;
        int32_t option = getopt_long(argc_, argv_, SHORT_OPTIONS_GET_BUNDLE_STATS.c_str(),
            LONG_OPTIONS_GET_BUNDLE_STATS, nullptr);
        APP_LOGD("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        if (option == -1) {
            if (counter == 1) {
                // When scanning the first argument
                if (strcmp(argv_[optind], cmd_.c_str()) == 0) {
                    resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                    result = OHOS::ERR_INVALID_VALUE;
                }
            }
            break;
        }

        if (option == '?') {
            switch (optopt) {
                case 'n': {
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                case 'u': {
                    resultReceiver_.append(STRING_REQUIRE_CORRECT_VALUE);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                default: {
                    std::string unknownOption = "";
                    std::string unknownOptionMsg = GetUnknownOptionMsg(unknownOption);
                    resultReceiver_.append(unknownOptionMsg);
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
            }
            break;
        }

        switch (option) {
            case 'h': {
                result = OHOS::ERR_INVALID_VALUE;
                break;
            }
            case 'n': {
                bundleName = optarg;
                break;
            }
            case 'u': {
                if (!OHOS::StrToInt(optarg, userId) || userId < 0) {
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
            resultReceiver_.append(HELP_MSG_NO_BUNDLE_NAME_OPTION + "\n");
            result = OHOS::ERR_INVALID_VALUE;
        }
    }

    if (result != OHOS::ERR_OK) {
        resultReceiver_.append(HELP_MSG_GET_BUNDLE_STATS);
    } else {
        std::string msg;
        bool ret = GetBundleStats(bundleName, userId, msg);
        if (ret) {
            resultReceiver_ = STRING_GET_BUNDLE_STATS_OK + msg;
        } else {
            resultReceiver_ = STRING_GET_BUNDLE_STATS_NG + "\n";
        }
    }

    return result;
}

bool BundleTestTool::GetBundleStats(const std::string &bundleName, int32_t userId,
    std::string& msg)
{
    if (bundleMgrProxy_ == nullptr) {
        APP_LOGE("bundleMgrProxy_ is nullptr");
        return false;
    }
    userId = BundleCommandCommon::GetCurrentUserId(userId);
    std::vector<std::int64_t> bundleStats;
    bool ret = bundleMgrProxy_->GetBundleStats(bundleName, userId, bundleStats);
    if (ret) {
        for (size_t index = 0; index < bundleStats.size(); ++index) {
            msg += GET_BUNDLE_STATS_ARRAY[index] + std::to_string(bundleStats[index]) + "\n";
        }
    }
    return ret;
}
} // AppExecFwk
} // OHOS