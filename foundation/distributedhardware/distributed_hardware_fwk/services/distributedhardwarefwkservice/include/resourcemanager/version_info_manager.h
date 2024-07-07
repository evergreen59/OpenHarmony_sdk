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

#ifndef OHOS_DISTRIBUTED_HARDWARE_VERSION_INFO_MANAGER_H
#define OHOS_DISTRIBUTED_HARDWARE_VERSION_INFO_MANAGER_H

#include <condition_variable>
#include <map>
#include <set>

#include "kvstore_observer.h"

#include "db_adapter.h"
#include "event.h"
#include "eventbus_handler.h"
#include "event_bus.h"
#include "event_sender.h"
#include "impl_utils.h"
#include "single_instance.h"
#include "version_info.h"
#include "version_info_event.h"

class DBAdapter;
namespace OHOS {
namespace DistributedHardware {
class VersionInfoManager : public std::enable_shared_from_this<VersionInfoManager>,
                           public EventSender,
                           public DistributedKv::KvStoreObserver,
                           public EventBusHandler<VersionInfoEvent> {
public:
    VersionInfoManager(const VersionInfoManager &) = delete;
    VersionInfoManager &operator = (const VersionInfoManager &) = delete;
    VersionInfoManager(VersionInfoManager &&) = delete;
    VersionInfoManager &operator = (VersionInfoManager &&) = delete;
    static std::shared_ptr<VersionInfoManager> GetInstance();
    virtual ~VersionInfoManager();

    int32_t Init();
    int32_t UnInit();

    int32_t AddVersion(const VersionInfo &versionInfo);
    int32_t GetVersionInfoByDeviceId(const std::string &deviceId, VersionInfo &versionInfo);
    int32_t RemoveVersionInfoByDeviceId(const std::string &deviceId);
    int32_t SyncVersionInfoFromDB(const std::string &deviceId);
    int32_t SyncRemoteVersionInfos();

    void CreateManualSyncCount(const std::string &deviceId);
    void RemoveManualSyncCount(const std::string &deviceId);
    int32_t ManualSync(const std::string &networkId);

    void OnChange(const DistributedKv::ChangeNotification &changeNotification) override;
    void OnEvent(VersionInfoEvent &ev) override;

private:
    VersionInfoManager();
    void UpdateVersionCache(const VersionInfo &versionInfo);
    void HandleVersionAddChange(const std::vector<DistributedKv::Entry> &insertRecords);
    void HandleVersionUpdateChange(const std::vector<DistributedKv::Entry> &updateRecords);
    void HandleVersionDeleteChange(const std::vector<DistributedKv::Entry> &deleteRecords);

private:
    mutable std::mutex verInfoMgrMutex_;
    std::shared_ptr<DBAdapter> dbAdapterPtr_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif
