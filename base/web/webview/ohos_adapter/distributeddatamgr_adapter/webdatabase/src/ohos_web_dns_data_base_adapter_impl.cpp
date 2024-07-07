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

#include "ohos_web_dns_data_base_adapter_impl.h"

#include <cinttypes>
#include <unistd.h>
#include "foundation/ability/ability_runtime/interfaces/kits/native/appkit/ability_runtime/context/application_context.h"
#include "nweb_log.h"
#include "sqlite_database_utils.h"

using namespace OHOS::NativeRdb;
using namespace OHOS::NWeb;

namespace {
    const int32_t RDB_VERSION = 1;
    const std::string WEB_DNS_DATABASE_FILE = "web_dns.db";

    const std::string ID_COL = "_id";
    const std::string DNS_TABLE_NAME = "dns";
    const std::string HOSTNAME_COL = "hostname";

    const std::string CREATE_TABLE = "CREATE TABLE " + DNS_TABLE_NAME + " (" + ID_COL + " INTEGER PRIMARY KEY, " +
                                     HOSTNAME_COL + " TEXT, " + " UNIQUE (" + HOSTNAME_COL + ") ON CONFLICT REPLACE);";

    const std::string WEB_PATH = "/web";
}

int32_t DnsDataBaseRdbOpenCallBack::OnCreate(OHOS::NativeRdb::RdbStore& rdbStore)
{
    WVLOG_I("web dns database opened, create table: %{public}s", CREATE_TABLE.c_str());
    return rdbStore.ExecuteSql(CREATE_TABLE);
}

int32_t DnsDataBaseRdbOpenCallBack::OnUpgrade(OHOS::NativeRdb::RdbStore& rdbStore,
                                              int32_t currentVersion, int32_t targetVersion)
{
    WVLOG_I("web dns database upgrade");
    return OHOS::NativeRdb::E_OK;
}

OhosWebDnsDataBaseAdapterImpl& OhosWebDnsDataBaseAdapterImpl::GetInstance()
{
    static OhosWebDnsDataBaseAdapterImpl instance;
    if (instance.rdbStore_ == nullptr) {
        WVLOG_I("webdatabase CreateDataBase");
        DnsDataBaseRdbOpenCallBack callBack;
        instance.rdbStore_ = instance.CreateDataBase(WEB_DNS_DATABASE_FILE, callBack);
    }
    return instance;
}

std::shared_ptr<OHOS::NativeRdb::RdbStore> OhosWebDnsDataBaseAdapterImpl::CreateDataBase(
    const std::string& dataBeseName, OHOS::NativeRdb::RdbOpenCallback& callBack)
{
    WVLOG_I("web dns database create rdb store");
    std::shared_ptr<OHOS::NativeRdb::RdbStore> rdbStore = nullptr;
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        OHOS::AbilityRuntime::ApplicationContext::GetApplicationContext();
    if (context == nullptr) {
        WVLOG_E("web dns database get context failed");
        return rdbStore;
    }

    std::string databaseDir = context->GetCacheDir() + WEB_PATH;
    if (access(databaseDir.c_str(), F_OK) != 0) {
        WVLOG_I("web dns database fail to access cache web dir:%{public}s", databaseDir.c_str());
        return rdbStore;
    }

    std::string bundleName = context->GetBundleName();
    std::string name = dataBeseName;
    int32_t errorCode = E_OK;
    std::string realPath = SqliteDatabaseUtils::GetDefaultDatabasePath(databaseDir, name, errorCode);
    RdbStoreConfig config("");
    config.SetPath(std::move(realPath));
    config.SetBundleName(bundleName);
    config.SetName(std::move(name));
    config.SetArea(context->GetArea());
    WVLOG_I("web dns database databaseDir=%{public}s", databaseDir.c_str());
    WVLOG_I("web dns database bundleName=%{public}s", bundleName.c_str());

    int32_t errCode = NativeRdb::E_OK;
    rdbStore = RdbHelper::GetRdbStore(config, RDB_VERSION, callBack, errCode);
    WVLOG_I("web dns database create rdb store end, errCode=%{public}d", errCode);
    return rdbStore;
}

OhosWebDnsDataBaseAdapterImpl::OhosWebDnsDataBaseAdapterImpl() {}

bool OhosWebDnsDataBaseAdapterImpl::ExistHostname(const std::string& hostname) const
{
    if (rdbStore_ == nullptr || hostname.empty()) {
        return false;
    }

    std::vector<std::string> hostInfo;
    GetHostnames(hostInfo);
    if (hostInfo.empty()) {
        WVLOG_I("web dns database get no hostinfo.");
        return false;
    }

    std::vector<std::string>::iterator it = find(hostInfo.begin(), hostInfo.end(), hostname);
    if (it != hostInfo.end()) {
        return true;
    }
    return false;
}

void OhosWebDnsDataBaseAdapterImpl::InsertHostname(const std::string& hostname)
{
    WVLOG_I("web dns database set info hostname:%{public}s", hostname.c_str());
    if (rdbStore_ == nullptr || hostname.empty()) {
        return;
    }

    int32_t errCode;
    int64_t outRowId;
    NativeRdb::ValuesBucket valuesBucket;
    valuesBucket.Clear();
    valuesBucket.PutString(HOSTNAME_COL, hostname);
    errCode = rdbStore_->Insert(outRowId, DNS_TABLE_NAME, valuesBucket);
    WVLOG_I("web dns database set info end, errCode=%{public}d", errCode);
}

void OhosWebDnsDataBaseAdapterImpl::GetHostnames(std::vector<std::string>& hostnames) const
{
    if (rdbStore_ == nullptr) {
        return;
    }

    std::vector<std::string> columns;
    NativeRdb::AbsRdbPredicates dirAbsPred(DNS_TABLE_NAME);
    std::unique_ptr<AbsSharedResultSet> resultSet = rdbStore_->Query(dirAbsPred, columns);
    if ((resultSet == nullptr) || (resultSet->GoToFirstRow() != NativeRdb::E_OK)) {
        WVLOG_E("web dns database rdb store query failed");
        return;
    }
    int32_t columnIndex;
    resultSet->GetColumnIndex(HOSTNAME_COL, columnIndex);
    do {
        std::string hostname;
        resultSet->GetString(columnIndex, hostname);
        hostnames.push_back(hostname);
    } while (resultSet->GoToNextRow() == NativeRdb::E_OK);
}

void OhosWebDnsDataBaseAdapterImpl::ClearAllHostname()
{
    if (rdbStore_ == nullptr) {
        return;
    }

    int32_t deletedRows = 0;
    NativeRdb::AbsRdbPredicates dirAbsPred(DNS_TABLE_NAME);
    int32_t ret = rdbStore_->Delete(deletedRows, dirAbsPred);
    WVLOG_I("web dns database clear all hostname: ret=%{public}d, deletedRows=%{public}d", ret, deletedRows);
}