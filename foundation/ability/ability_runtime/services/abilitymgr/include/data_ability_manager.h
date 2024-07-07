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

#ifndef OHOS_ABILITY_RUNTIME_DATA_ABILITY_MANAGER_H
#define OHOS_ABILITY_RUNTIME_DATA_ABILITY_MANAGER_H

#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "ability_record.h"
#include "ability_running_info.h"
#include "data_ability_record.h"
#include "nocopyable.h"

namespace OHOS {
namespace AAFwk {
class DataAbilityManager : public NoCopyable {
public:
    DataAbilityManager();
    virtual ~DataAbilityManager();

public:
    sptr<IAbilityScheduler> Acquire(
        const AbilityRequest &abilityRequest, bool tryBind, const sptr<IRemoteObject> &client, bool isNotHap);
    int Release(const sptr<IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &client, bool isNotHap);
    int AttachAbilityThread(const sptr<IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &token);
    int AbilityTransitionDone(const sptr<IRemoteObject> &token, int state);
    void OnAbilityRequestDone(const sptr<IRemoteObject> &token, const int32_t state);
    void OnAppStateChanged(const AppInfo &info);
    void OnAbilityDied(const std::shared_ptr<AbilityRecord> &abilityRecord);
    std::shared_ptr<AbilityRecord> GetAbilityRecordById(int64_t id);
    std::shared_ptr<AbilityRecord> GetAbilityRecordByToken(const sptr<IRemoteObject> &token);
    std::shared_ptr<AbilityRecord> GetAbilityRecordByScheduler(const sptr<IAbilityScheduler> &scheduler);
    void Dump(const char *func, int line);
    void DumpState(std::vector<std::string> &info, const std::string &args = "") const;
    void DumpSysState(std::vector<std::string> &info, bool isClient = false, const std::string &args = "") const;
    bool ContainsDataAbility(const sptr<IAbilityScheduler> &scheduler);
    void GetAbilityRunningInfos(std::vector<AbilityRunningInfo> &info, bool isPerm);

private:
    using DataAbilityRecordPtr = std::shared_ptr<DataAbilityRecord>;
    using DataAbilityRecordPtrMap = std::map<std::string, DataAbilityRecordPtr>;

private:
    DataAbilityRecordPtr LoadLocked(const std::string &name, const AbilityRequest &req);
    void DumpLocked(const char *func, int line);
    void RestartDataAbility(const std::shared_ptr<AbilityRecord> &abilityRecord);
    void ReportDataAbilityAcquired(const sptr<IRemoteObject> &client, bool isNotHap,
        std::shared_ptr<DataAbilityRecord> &record);
    void ReportDataAbilityReleased(const sptr<IRemoteObject> &client, bool isNotHap,
        std::shared_ptr<DataAbilityRecord> &record);

private:
    std::mutex mutex_;
    DataAbilityRecordPtrMap dataAbilityRecordsLoaded_;
    DataAbilityRecordPtrMap dataAbilityRecordsLoading_;
};
}  // namespace AAFwk
}  // namespace OHOS

#endif  // OHOS_ABILITY_RUNTIME_DATA_ABILITY_MANAGER_H
