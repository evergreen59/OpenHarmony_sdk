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

#include "mission_list.h"

#include "hilog_wrapper.h"

namespace OHOS {
namespace AAFwk {
MissionList::MissionList(MissionListType type) : type_(type)
{
}

MissionList::~MissionList()
{
}

void MissionList::AddMissionToTop(const std::shared_ptr<Mission> &mission)
{
    if (!mission) {
        return;
    }

    if (!missions_.empty() && missions_.front() == mission) {
        HILOG_DEBUG("mission is already at the top of list");
        return;
    }

    missions_.remove(mission);
    missions_.push_front(mission);
    mission->SetMissionList(shared_from_this());
}

void MissionList::RemoveMission(const std::shared_ptr<Mission> &mission)
{
    for (auto iter = missions_.begin(); iter != missions_.end(); iter++) {
        if (*iter == mission) {
            missions_.erase(iter);
            return;
        }
    }
}

std::shared_ptr<Mission> MissionList::GetTopMission() const
{
    if (missions_.empty()) {
        return nullptr;
    }

    return missions_.front();
}

std::shared_ptr<Mission> MissionList::GetSingletonMissionByName(const std::string &missionName) const
{
    if (missionName.empty()) {
        return nullptr;
    }

    for (auto mission : missions_) {
        if (mission && mission->IsSingletonAbility() && mission->GetMissionName() == missionName) {
            return mission;
        }
    }

    return nullptr;
}

std::shared_ptr<Mission> MissionList::GetSpecifiedMission(
    const std::string &missionName, const std::string& flag) const
{
    if (missionName.empty() || flag.empty()) {
        return nullptr;
    }

    for (auto& mission : missions_) {
        if (mission && mission->IsSpecifiedAbility() && mission->GetMissionName() == missionName &&
            mission->GetSpecifiedFlag() == flag) {
            return mission;
        }
    }

    return nullptr;
}

std::shared_ptr<Mission> MissionList::GetRecentStandardMission(const std::string &missionName) const
{
    if (missionName.empty()) {
        return nullptr;
    }

    std::string missionTime = "0";
    std::shared_ptr<Mission> result = nullptr;
    for (auto& mission : missions_) {
        if (mission && mission->IsStandardAbility() && mission->GetMissionName() == missionName &&
            mission->GetMissionTime() >= missionTime) {
            result = mission;
            missionTime = mission->GetMissionTime();
        }
    }

    return result;
}

std::shared_ptr<AbilityRecord> MissionList::GetAbilityRecordByToken(const sptr<IRemoteObject> &token) const
{
    for (auto mission : missions_) {
        if (!mission) {
            continue;
        }
        std::shared_ptr<AbilityRecord> abilityRecord = mission->GetAbilityRecord();
        if (abilityRecord && token == abilityRecord->GetToken()->AsObject()) {
            return abilityRecord;
        }
    }

    return nullptr;
}

void MissionList::RemoveMissionByAbilityRecord(const std::shared_ptr<AbilityRecord> &abilityRecord)
{
    for (auto iter = missions_.begin(); iter != missions_.end(); iter++) {
        if ((*iter)->GetAbilityRecord() == abilityRecord) {
            missions_.erase(iter);
            return;
        }
    }
}

std::shared_ptr<Mission> MissionList::GetMissionById(int missionId) const
{
    for (auto mission : missions_) {
        if (mission && mission->GetMissionId() == missionId) {
            return mission;
        }
    }

    return nullptr;
}

std::shared_ptr<Mission> MissionList::GetMissionBySpecifiedFlag(const AAFwk::Want &want, const std::string &flag) const
{
    for (auto mission : missions_) {
        if (!mission) {
            return nullptr;
        }

        auto ability = mission->GetAbilityRecord();
        if (!ability) {
            return nullptr;
        }

        std::string srcAbilityName = ability->GetAbilityInfo().name;
        std::string srcBundleName = ability->GetApplicationInfo().bundleName;
        std::string tarAbilityName = want.GetElement().GetAbilityName();
        std::string tarBundleName = want.GetElement().GetBundleName();
        if ((srcBundleName == tarBundleName) &&
            (srcAbilityName == tarAbilityName) &&
            (ability->GetSpecifiedFlag() == flag)) {
            return mission;
        }
    }

    return nullptr;
}

bool MissionList::IsEmpty()
{
    return missions_.empty();
}

std::shared_ptr<AbilityRecord> MissionList::GetTopAbility() const
{
    if (missions_.empty()) {
        return nullptr;
    }

    auto& topMission = missions_.front();
    if (topMission) {
        return topMission->GetAbilityRecord();
    }
    return nullptr;
}

std::list<std::shared_ptr<Mission>>& MissionList::GetAllMissions()
{
    return missions_;
}

MissionListType MissionList::GetType() const
{
    return type_;
}

std::shared_ptr<AbilityRecord> MissionList::GetLauncherRoot() const
{
    for (auto mission : missions_) {
        if (!mission) {
            continue;
        }

        std::shared_ptr<AbilityRecord> ability = mission->GetAbilityRecord();
        if (ability && ability->IsLauncherRoot()) {
            return ability;
        }
    }
    return nullptr;
}

std::shared_ptr<AbilityRecord> MissionList::GetAbilityRecordById(int64_t eventId) const
{
    for (std::shared_ptr<Mission> mission : missions_) {
        if (mission && mission->GetAbilityRecord()->GetEventId() == eventId) {
            return mission->GetAbilityRecord();
        }
    }
    return nullptr;
}

std::shared_ptr<AbilityRecord> MissionList::GetAbilityRecordByCaller(
    const std::shared_ptr<AbilityRecord> &caller, int requestCode)
{
    for (auto mission : missions_) {
        auto callerList = mission->GetAbilityRecord()->GetCallerRecordList();
        if (callerList.empty()) {
            continue;
        }

        for (auto callerAbility : callerList) {
            if (callerAbility->GetCaller() == caller && callerAbility->GetRequestCode() == requestCode) {
                return mission->GetAbilityRecord();
            }
        }
    }
    return nullptr;
}

std::shared_ptr<AbilityRecord> MissionList::GetAbilityRecordByName(const AppExecFwk::ElementName &element)
{
    for (auto mission : missions_) {
        if (mission) {
            const AppExecFwk::AbilityInfo &abilityInfo = mission->GetAbilityRecord()->GetAbilityInfo();
            AppExecFwk::ElementName localElement(abilityInfo.deviceId, abilityInfo.bundleName,
                abilityInfo.name, abilityInfo.moduleName);
            AppExecFwk::ElementName localElementNoModuleName(abilityInfo.deviceId,
                abilityInfo.bundleName, abilityInfo.name); // note: moduleName of input param element maybe empty
            if (localElement == element || localElementNoModuleName == element) {
                return mission->GetAbilityRecord();
            }
        }
    }
    return nullptr;
}

sptr<IRemoteObject> MissionList::GetAbilityTokenByMissionId(int32_t missionId)
{
    for (auto mission : missions_) {
        if (mission && mission->GetMissionId() == missionId) {
            auto abilityRecord = mission->GetAbilityRecord();
            if (abilityRecord) {
                return abilityRecord->GetToken();
            }
        }
    }

    return nullptr;
}

std::string MissionList::GetTypeName()
{
    switch (type_) {
        case MissionListType::CURRENT: {
            return "NORMAL";
        }
        case MissionListType::DEFAULT_STANDARD: {
            return "DEFAULT_STANDARD";
        }
        case MissionListType::DEFAULT_SINGLE: {
            return "DEFAULT_SINGLE";
        }
        case MissionListType::LAUNCHER: {
            return "LAUNCHER";
        }
        default: {
            return "INVALID";
        }
    }
}

void MissionList::HandleUnInstallApp(const std::string &bundleName, int32_t uid)
{
    for (auto it = missions_.begin(); it != missions_.end();) {
        auto mission = *it;
        if (MatchedInitialMission(mission, bundleName, uid)) {
            missions_.erase(it++);
        } else {
            it++;
        }
    }
}

bool MissionList::MatchedInitialMission(const std::shared_ptr<Mission>& mission,
    const std::string &bundleName, int32_t uid)
{
    if (!mission) {
        return false;
    }

    auto abilityRecord = mission->GetAbilityRecord();
    if (!abilityRecord) {
        return false;
    }

    if (abilityRecord->GetAbilityInfo().bundleName == bundleName && abilityRecord->GetUid() == uid) {
        abilityRecord->SetIsUninstallAbility();
        if (abilityRecord->IsAbilityState(AbilityState::INITIAL)) {
            return true;
        }
    }

    return false;
}


void MissionList::Dump(std::vector<std::string>& info)
{
    std::string dumpInfo = "  MissionList Type #" + GetTypeName();
    info.push_back(dumpInfo);
    for (const auto& mission : missions_) {
        if (mission) {
            mission->Dump(info);
        }
    }
}

void MissionList::DumpStateByRecordId(
    std::vector<std::string> &info, bool isClient, int32_t abilityRecordId, const std::vector<std::string> &params)
{
    for (const auto& mission : missions_) {
        if (mission) {
            auto abilityRecord = mission->GetAbilityRecord();
            if (abilityRecord) {
                if (abilityRecord->GetRecordId() == abilityRecordId) {
                    HILOG_INFO("record begin to call DumpAbilityState %{public}s", __func__);
                    abilityRecord->DumpAbilityState(info, isClient, params);
                    return;
                }
            }
        }
    }
}
void MissionList::DumpList(std::vector<std::string> &info, bool isClient)
{
    std::string dumpInfo = "    MissionList Type #" + GetTypeName();
    info.push_back(dumpInfo);

    for (const auto& mission : missions_) {
        if (mission) {
            dumpInfo = "      Mission ID #" + std::to_string(mission->GetMissionId());
            dumpInfo += "  mission name #[" + mission->GetMissionName() + "]" +
                "  lockedState #" + std::to_string(mission->IsLockedState());
            info.push_back(dumpInfo);

            auto abilityRecord = mission->GetAbilityRecord();
            if (abilityRecord) {
                HILOG_INFO("record begin to call DumpAbilityState %{public}s", __func__);
                std::vector<std::string> params;
                abilityRecord->DumpAbilityState(info, isClient, params);
            }
        }
    }
}

#ifdef ABILITY_COMMAND_FOR_TEST
int MissionList::BlockAbilityByRecordId(int32_t abilityRecordId)
{
    int ret = -1;
    for (const auto& mission : missions_) {
        if (mission) {
            auto abilityRecord = mission->GetAbilityRecord();
            if (abilityRecord) {
                if (abilityRecord->GetRecordId() == abilityRecordId) {
                    HILOG_INFO("record begin to call BlockAbilityByRecordId %{public}s", __func__);
                    return abilityRecord->BlockAbility();
                }
            }
        }
    }
    return ret;
}
#endif

int32_t MissionList::GetMissionCountByUid(int32_t targetUid) const
{
    int32_t count = 0;
    for (const auto& mission : missions_) {
        if (!mission) {
            continue;
        }

        auto abilityRecord = mission->GetAbilityRecord();
        if (!abilityRecord) {
            continue;
        }

        if (abilityRecord->GetUid() == targetUid) {
            count++;
        }
    }
    return count;
}
}  // namespace AAFwk
}  // namespace OHOS
