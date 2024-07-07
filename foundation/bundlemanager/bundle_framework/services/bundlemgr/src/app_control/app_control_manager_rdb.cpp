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

#include "app_control_manager_rdb.h"

#include "app_control_constants.h"
#include "app_log_wrapper.h"
#include "appexecfwk_errors.h"
#include "bundle_util.h"
#include "scope_guard.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
    const std::string APP_CONTROL_RDB_TABLE_NAME = "app_control";
    const std::string RUNNING_CONTROL = "RunningControl";
    const std::string APP_CONTROL_EDM_DEFAULT_MESSAGE = "The app has been disabled by EDM";
    const std::string DEFAULT = "default";
    const int32_t CALLING_NAME_INDEX = 1;
    const int32_t APP_ID_INDEX = 4;
    const int32_t CONTROL_MESSAGE_INDEX = 5;
    const int32_t DISPOSED_STATUS_INDEX = 6;
    // app control table key
    const std::string CALLING_NAME = "CALLING_NAME";
    const std::string APP_CONTROL_LIST = "APP_CONTROL_LIST";
    const std::string USER_ID = "USER_ID";
    const std::string APP_ID = "APP_ID";
    const std::string CONTROL_MESSAGE = "CONTROL_MESSAGE";
    const std::string DISPOSED_STATUS = "DISPOSED_STATUS";
    const std::string PRIORITY = "PRIORITY";
    const std::string TIME_STAMP = "TIME_STAMP";

    enum class PRIORITY {
        EDM = 100,
        APP_MARKET = 200,
    };
}
AppControlManagerRdb::AppControlManagerRdb()
{
    APP_LOGD("create AppControlManagerRdb.");
    BmsRdbConfig bmsRdbConfig;
    bmsRdbConfig.dbName = Constants::BUNDLE_RDB_NAME;
    bmsRdbConfig.tableName = APP_CONTROL_RDB_TABLE_NAME;
    bmsRdbConfig.createTableSql = std::string(
        "CREATE TABLE IF NOT EXISTS "
        + APP_CONTROL_RDB_TABLE_NAME
        + "(ID INTEGER PRIMARY KEY AUTOINCREMENT, CALLING_NAME TEXT NOT NULL, "
        + "APP_CONTROL_LIST TEXT, USER_ID INTEGER, APP_ID TEXT, CONTROL_MESSAGE TEXT, "
        + "DISPOSED_STATUS TEXT, PRIORITY INTEGER, TIME_STAMP INTEGER);");
    rdbDataManager_ = std::make_shared<RdbDataManager>(bmsRdbConfig);
    rdbDataManager_->CreateTable();
}

AppControlManagerRdb::~AppControlManagerRdb()
{
    APP_LOGD("destroy AppControlManagerRdb.");
}

ErrCode AppControlManagerRdb::AddAppInstallControlRule(const std::string &callingName,
    const std::vector<std::string> &appIds, const std::string &controlRuleType, int32_t userId)
{
    int64_t timeStamp = BundleUtil::GetCurrentTime();
    std::vector<NativeRdb::ValuesBucket> valuesBuckets;
    for (auto appId : appIds) {
        ErrCode result = DeleteOldControlRule(callingName, controlRuleType, appId, userId);
        if (result != ERR_OK) {
            return result;
        }
        NativeRdb::ValuesBucket valuesBucket;
        valuesBucket.PutString(CALLING_NAME, callingName);
        valuesBucket.PutString(APP_CONTROL_LIST, controlRuleType);
        valuesBucket.PutInt(USER_ID, static_cast<int>(userId));
        valuesBucket.PutString(APP_ID, appId);
        valuesBucket.PutInt(TIME_STAMP, timeStamp);
        valuesBuckets.emplace_back(valuesBucket);
    }
    int64_t insertNum = 0;
    bool ret = rdbDataManager_->BatchInsert(insertNum, valuesBuckets);
    if (!ret) {
        APP_LOGE("BatchInsert failed.");
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    if (valuesBuckets.size() != static_cast<uint64_t>(insertNum)) {
        APP_LOGE("BatchInsert size not expected.");
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    return ERR_OK;
}

ErrCode AppControlManagerRdb::DeleteAppInstallControlRule(const std::string &callingName,
    const std::string &controlRuleType, const std::vector<std::string> &appIds, int32_t userId)
{
    for (const auto &appId : appIds) {
        NativeRdb::AbsRdbPredicates absRdbPredicates(APP_CONTROL_RDB_TABLE_NAME);
        absRdbPredicates.EqualTo(CALLING_NAME, callingName);
        absRdbPredicates.EqualTo(APP_CONTROL_LIST, controlRuleType);
        absRdbPredicates.EqualTo(USER_ID, std::to_string(userId));
        absRdbPredicates.EqualTo(APP_ID, appId);
        bool ret = rdbDataManager_->DeleteData(absRdbPredicates);
        if (!ret) {
            APP_LOGE("DeleteAppInstallControlRule callingName:%{public}s appId:%{public}s userId:%{public}d failed.",
                callingName.c_str(), appId.c_str(), userId);
            return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode AppControlManagerRdb::DeleteAppInstallControlRule(const std::string &callingName,
    const std::string &controlRuleType, int32_t userId)
{
    NativeRdb::AbsRdbPredicates absRdbPredicates(APP_CONTROL_RDB_TABLE_NAME);
    absRdbPredicates.EqualTo(CALLING_NAME, callingName);
    absRdbPredicates.EqualTo(APP_CONTROL_LIST, controlRuleType);
    absRdbPredicates.EqualTo(USER_ID, std::to_string(userId));
    bool ret = rdbDataManager_->DeleteData(absRdbPredicates);
    if (!ret) {
        APP_LOGE("DeleteData callingName:%{public}s controlRuleType:%{public}s failed.",
            callingName.c_str(), controlRuleType.c_str());
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    return ERR_OK;
}

ErrCode AppControlManagerRdb::GetAppInstallControlRule(const std::string &callingName,
    const std::string &controlRuleType, int32_t userId, std::vector<std::string> &appIds)
{
    NativeRdb::AbsRdbPredicates absRdbPredicates(APP_CONTROL_RDB_TABLE_NAME);
    absRdbPredicates.EqualTo(CALLING_NAME, callingName);
    absRdbPredicates.EqualTo(APP_CONTROL_LIST, controlRuleType);
    absRdbPredicates.EqualTo(USER_ID, std::to_string(userId));
    auto absSharedResultSet = rdbDataManager_->QueryData(absRdbPredicates);
    if (absSharedResultSet == nullptr) {
        APP_LOGE("GetAppInstallControlRule failed.");
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    ScopeGuard stateGuard([&] { absSharedResultSet->Close(); });
    int32_t count;
    int ret = absSharedResultSet->GetRowCount(count);
    if (ret != NativeRdb::E_OK) {
        APP_LOGE("GetRowCount failed, ret: %{public}d", ret);
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    if (count == 0) {
        APP_LOGI("GetAppInstallControlRule size 0");
        return ERR_OK;
    }

    ret = absSharedResultSet->GoToFirstRow();
    if (ret != NativeRdb::E_OK) {
        APP_LOGE("GoToFirstRow failed, ret: %{public}d", ret);
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    do {
        std::string appId;
        ret = absSharedResultSet->GetString(APP_ID_INDEX, appId);
        if (ret != NativeRdb::E_OK) {
            APP_LOGE("GetString appId failed, ret: %{public}d", ret);
            return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
        }
        appIds.push_back(appId);
    } while (absSharedResultSet->GoToNextRow() == NativeRdb::E_OK);
    return ERR_OK;
}

ErrCode AppControlManagerRdb::AddAppRunningControlRule(const std::string &callingName,
    const std::vector<AppRunningControlRule> &controlRules, int32_t userId)
{
    int64_t timeStamp = BundleUtil::GetCurrentTime();
    std::vector<NativeRdb::ValuesBucket> valuesBuckets;
    for (auto &controlRule : controlRules) {
        ErrCode result = DeleteOldControlRule(callingName, RUNNING_CONTROL, controlRule.appId, userId);
        if (result != ERR_OK) {
            return result;
        }
        NativeRdb::ValuesBucket valuesBucket;
        valuesBucket.PutString(CALLING_NAME, callingName);
        valuesBucket.PutString(APP_CONTROL_LIST, RUNNING_CONTROL);
        valuesBucket.PutInt(USER_ID, static_cast<int>(userId));
        valuesBucket.PutString(APP_ID, controlRule.appId);
        valuesBucket.PutString(CONTROL_MESSAGE, controlRule.controlMessage);
        valuesBucket.PutInt(PRIORITY, static_cast<int>(PRIORITY::EDM));
        valuesBucket.PutInt(TIME_STAMP, timeStamp);
        valuesBuckets.emplace_back(valuesBucket);
    }
    int64_t insertNum = 0;
    bool ret = rdbDataManager_->BatchInsert(insertNum, valuesBuckets);
    if (!ret) {
        APP_LOGE("BatchInsert AddAppRunningControlRule failed.");
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    if (valuesBuckets.size() != static_cast<uint64_t>(insertNum)) {
        APP_LOGE("BatchInsert size not expected.");
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    return ERR_OK;
}

ErrCode AppControlManagerRdb::DeleteAppRunningControlRule(const std::string &callingName,
    const std::vector<AppRunningControlRule> &controlRules, int32_t userId)
{
    for (auto &rule : controlRules) {
        NativeRdb::AbsRdbPredicates absRdbPredicates(APP_CONTROL_RDB_TABLE_NAME);
        absRdbPredicates.EqualTo(CALLING_NAME, callingName);
        absRdbPredicates.EqualTo(APP_CONTROL_LIST, RUNNING_CONTROL);
        absRdbPredicates.EqualTo(USER_ID, std::to_string(userId));
        absRdbPredicates.EqualTo(APP_ID, rule.appId);
        bool ret = rdbDataManager_->DeleteData(absRdbPredicates);
        if (!ret) {
            APP_LOGE("DeleteAppInstallControlRule callingName:%{public}s appid:%{public}s userId:%{public}d failed.",
                callingName.c_str(), rule.appId.c_str(), userId);
            return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
        }
    }
    return ERR_OK;
}
ErrCode AppControlManagerRdb::DeleteAppRunningControlRule(const std::string &callingName, int32_t userId)
{
    NativeRdb::AbsRdbPredicates absRdbPredicates(APP_CONTROL_RDB_TABLE_NAME);
    absRdbPredicates.EqualTo(CALLING_NAME, callingName);
    absRdbPredicates.EqualTo(APP_CONTROL_LIST, RUNNING_CONTROL);
    absRdbPredicates.EqualTo(USER_ID, std::to_string(userId));
    bool ret = rdbDataManager_->DeleteData(absRdbPredicates);
    if (!ret) {
        APP_LOGE("DeleteAppRunningControlRule callingName:%{public}s userId:%{public}d failed.",
            callingName.c_str(), userId);
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    return ERR_OK;
}

ErrCode AppControlManagerRdb::GetAppRunningControlRule(const std::string &callingName,
    int32_t userId, std::vector<std::string> &appIds)
{
    NativeRdb::AbsRdbPredicates absRdbPredicates(APP_CONTROL_RDB_TABLE_NAME);
    absRdbPredicates.EqualTo(CALLING_NAME, callingName);
    absRdbPredicates.EqualTo(APP_CONTROL_LIST, RUNNING_CONTROL);
    absRdbPredicates.EqualTo(USER_ID, std::to_string(userId));
    auto absSharedResultSet = rdbDataManager_->QueryData(absRdbPredicates);
    if (absSharedResultSet == nullptr) {
        APP_LOGE("QueryData failed");
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    ScopeGuard stateGuard([&] { absSharedResultSet->Close(); });
    int32_t count;
    int ret = absSharedResultSet->GetRowCount(count);
    if (ret != NativeRdb::E_OK) {
        APP_LOGE("GetRowCount failed, ret: %{public}d", ret);
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    if (count == 0) {
        APP_LOGI("GetAppRunningControlRule size 0");
        return ERR_OK;
    }
    ret = absSharedResultSet->GoToFirstRow();
    if (ret != NativeRdb::E_OK) {
        APP_LOGE("GoToFirstRow failed, ret: %{public}d", ret);
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    do {
        std::string appId;
        ret = absSharedResultSet->GetString(APP_ID_INDEX, appId);
        if (ret != NativeRdb::E_OK) {
            APP_LOGE("GetString appId failed, ret: %{public}d", ret);
            return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
        }
        appIds.push_back(appId);
    } while (absSharedResultSet->GoToNextRow() == NativeRdb::E_OK);
    return ERR_OK;
}

ErrCode AppControlManagerRdb::GetAppRunningControlRule(const std::string &appId,
    int32_t userId, AppRunningControlRuleResult &controlRuleResult)
{
    NativeRdb::AbsRdbPredicates absRdbPredicates(APP_CONTROL_RDB_TABLE_NAME);
    absRdbPredicates.EqualTo(APP_ID, appId);
    absRdbPredicates.EqualTo(APP_CONTROL_LIST, RUNNING_CONTROL);
    absRdbPredicates.EqualTo(USER_ID, std::to_string(userId));
    absRdbPredicates.OrderByAsc(PRIORITY); // ascending
    auto absSharedResultSet = rdbDataManager_->QueryData(absRdbPredicates);
    if (absSharedResultSet == nullptr) {
        APP_LOGE("QueryData failed");
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    ScopeGuard stateGuard([&] { absSharedResultSet->Close(); });
    int32_t count;
    int ret = absSharedResultSet->GetRowCount(count);
    if (ret != NativeRdb::E_OK) {
        APP_LOGE("GetRowCount failed, ret: %{public}d", ret);
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    if (count == 0) {
        APP_LOGE("GetAppRunningControlRuleResult size 0");
        return ERR_BUNDLE_MANAGER_BUNDLE_NOT_SET_CONTROL;
    }
    ret = absSharedResultSet->GoToFirstRow();
    if (ret != NativeRdb::E_OK) {
        APP_LOGE("GoToFirstRow failed, ret: %{public}d", ret);
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    std::string callingName;
    if (absSharedResultSet->GetString(CALLING_NAME_INDEX, callingName) != NativeRdb::E_OK) {
        APP_LOGE("GetString callingName failed, ret: %{public}d", ret);
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    ret = absSharedResultSet->GetString(CONTROL_MESSAGE_INDEX, controlRuleResult.controlMessage);
    if (ret != NativeRdb::E_OK) {
        APP_LOGW("GetString controlMessage failed, ret: %{public}d", ret);
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    if (controlRuleResult.controlMessage.empty() && callingName == AppControlConstants::EDM_CALLING) {
        APP_LOGD("GetString controlMessage default");
        controlRuleResult.controlMessage = APP_CONTROL_EDM_DEFAULT_MESSAGE;
    }
    std::string wantString;
    if (absSharedResultSet->GetString(DISPOSED_STATUS_INDEX, wantString) != NativeRdb::E_OK) {
        APP_LOGE("GetString controlWant failed, ret: %{public}d", ret);
    }
    if (!wantString.empty()) {
        controlRuleResult.controlWant = std::make_shared<Want>(*Want::FromString(wantString));
    }
    return ERR_OK;
}

ErrCode AppControlManagerRdb::SetDisposedStatus(const std::string &callingName,
    const std::string &appId, const Want &want, int32_t userId)
{
    APP_LOGD("rdb begin to SetDisposedStatus");
    ErrCode code = DeleteDisposedStatus(callingName, appId, userId);
    if (code != ERR_OK) {
        APP_LOGE("DeleteDisposedStatus failed.");
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    int64_t timeStamp = BundleUtil::GetCurrentTime();
    NativeRdb::ValuesBucket valuesBucket;
    valuesBucket.PutString(CALLING_NAME, callingName);
    valuesBucket.PutString(APP_CONTROL_LIST, RUNNING_CONTROL);
    valuesBucket.PutString(APP_ID, appId);
    valuesBucket.PutString(DISPOSED_STATUS, want.ToString());
    valuesBucket.PutInt(PRIORITY, static_cast<int>(PRIORITY::APP_MARKET));
    valuesBucket.PutInt(TIME_STAMP, timeStamp);
    valuesBucket.PutString(USER_ID, std::to_string(userId));
    bool ret = rdbDataManager_->InsertData(valuesBucket);
    if (!ret) {
        APP_LOGE("SetDisposedStatus callingName:%{public}s appId:%{public}s failed.",
            callingName.c_str(), appId.c_str());
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    return ERR_OK;
}

ErrCode AppControlManagerRdb::DeleteDisposedStatus(const std::string &callingName,
    const std::string &appId, int32_t userId)
{
    APP_LOGD("rdb begin to DeleteDisposedStatus");
    NativeRdb::AbsRdbPredicates absRdbPredicates(APP_CONTROL_RDB_TABLE_NAME);
    absRdbPredicates.EqualTo(CALLING_NAME, callingName);
    absRdbPredicates.EqualTo(APP_CONTROL_LIST, RUNNING_CONTROL);
    absRdbPredicates.EqualTo(APP_ID, appId);
    absRdbPredicates.EqualTo(USER_ID, std::to_string(userId));
    bool ret = rdbDataManager_->DeleteData(absRdbPredicates);
    if (!ret) {
        APP_LOGE("DeleteDisposedStatus callingName:%{public}s appId:%{public}s failed.",
            callingName.c_str(), appId.c_str());
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    return ERR_OK;
}

ErrCode AppControlManagerRdb::GetDisposedStatus(const std::string &callingName,
    const std::string &appId, Want &want, int32_t userId)
{
    APP_LOGD("rdb begin to GetDisposedStatus");
    NativeRdb::AbsRdbPredicates absRdbPredicates(APP_CONTROL_RDB_TABLE_NAME);
    absRdbPredicates.EqualTo(CALLING_NAME, callingName);
    absRdbPredicates.EqualTo(APP_CONTROL_LIST, RUNNING_CONTROL);
    absRdbPredicates.EqualTo(APP_ID, appId);
    absRdbPredicates.EqualTo(USER_ID, std::to_string(userId));
    auto absSharedResultSet = rdbDataManager_->QueryData(absRdbPredicates);
    if (absSharedResultSet == nullptr) {
        APP_LOGE("GetAppInstallControlRule failed.");
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    ScopeGuard stateGuard([&] { absSharedResultSet->Close(); });
    int32_t count;
    int ret = absSharedResultSet->GetRowCount(count);
    if (ret != NativeRdb::E_OK) {
        APP_LOGE("GetRowCount failed, ret: %{public}d", ret);
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    if (count == 0) {
        APP_LOGI("GetAppRunningControlRule size 0");
        return ERR_OK;
    }
    ret = absSharedResultSet->GoToFirstRow();
    if (ret != NativeRdb::E_OK) {
        APP_LOGE("GoToFirstRow failed, ret: %{public}d", ret);
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    std::string wantString;
    ret = absSharedResultSet->GetString(DISPOSED_STATUS_INDEX, wantString);
    if (ret != NativeRdb::E_OK) {
        APP_LOGE("GetString DisposedStatus failed, ret: %{public}d", ret);
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    want = *Want::FromString(wantString);
    return ERR_OK;
}

ErrCode AppControlManagerRdb::DeleteOldControlRule(const std::string &callingName, const std::string &controlRuleType,
    const std::string &appId, int32_t userId)
{
    NativeRdb::AbsRdbPredicates absRdbPredicates(APP_CONTROL_RDB_TABLE_NAME);
    absRdbPredicates.EqualTo(CALLING_NAME, callingName);
    absRdbPredicates.EqualTo(APP_CONTROL_LIST, controlRuleType);
    absRdbPredicates.EqualTo(USER_ID, std::to_string(userId));
    absRdbPredicates.EqualTo(APP_ID, appId);
    bool ret = rdbDataManager_->DeleteData(absRdbPredicates);
    if (!ret) {
        APP_LOGE("DeleteOldControlRule %{public}s, %{public}s, %{public}s, %{public}d failed.",
            callingName.c_str(), appId.c_str(), controlRuleType.c_str(), userId);
        return ERR_BUNDLE_MANAGER_APP_CONTROL_INTERNAL_ERROR;
    }
    return ERR_OK;
}
}
}