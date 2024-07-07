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

#ifndef OHOS_ABILITY_RUNTIME_MISSION_LIST_H
#define OHOS_ABILITY_RUNTIME_MISSION_LIST_H

#include <list>
#include <memory>

#include "ability_record.h"
#include "iremote_object.h"
#include "mission.h"

using IRemoteObject = OHOS::IRemoteObject;

namespace OHOS {
namespace AAFwk {
enum MissionListType {
    CURRENT = 0,
    DEFAULT_STANDARD,
    DEFAULT_SINGLE,
    LAUNCHER
};

class MissionList : public std::enable_shared_from_this<MissionList> {
public:
    explicit MissionList(MissionListType type = MissionListType::CURRENT);
    explicit MissionList(const MissionList& missionList) : type_(missionList.type_), missions_(missionList.missions_) {}
    virtual ~MissionList();

    /**
     * Add mission to top of this mission list.
     *
     * @param mission target mission
     */
    void AddMissionToTop(const std::shared_ptr<Mission> &mission);

    /**
     * Remove mission from this mission list.
     *
     * @param mission target mission
     */
    void RemoveMission(const std::shared_ptr<Mission> &mission);

    /**
     * Get singleton mission by name.
     *
     * @param missionName target mission name.
     * @return founded mission.
     */
    std::shared_ptr<Mission> GetSingletonMissionByName(const std::string& missionName) const;

    /**
     * Get specified mission by name and flag.
     *
     * @param missionName target mission name.
     * @param flag target mission specified flag.
     * @return founded mission.
     */
    std::shared_ptr<Mission> GetSpecifiedMission(const std::string& missionName, const std::string& flag) const;

    /**
     * Get recent standard mission by name.
     *
     * @param missionName target mission name.
     * @return founded mission.
     */
    std::shared_ptr<Mission> GetRecentStandardMission(const std::string& missionName) const;

    /**
     * Get top mission of this mission list.
     *
     * @return founded mission.
     */
    std::shared_ptr<Mission> GetTopMission() const;

    /**
     * @brief Get the Ability Record By Token object
     *
     * @param token the ability to search
     * @return std::shared_ptr<AbilityRecord> the ability
     */
    std::shared_ptr<AbilityRecord> GetAbilityRecordByToken(const sptr<IRemoteObject> &token) const;

    /**
     * @brief remove mission by ability record
     *
     * @param abilityRecord the ability need to remove
     */
    void RemoveMissionByAbilityRecord(const std::shared_ptr<AbilityRecord> &abilityRecord);

    /**
     * whether the missionList contains mission.
     *
     * @return finded mission.
     */
    bool IsEmpty();

    /**
     * @brief Get the Top Ability object
     *
     * @return std::shared_ptr<AbilityRecord> the top ability
     */
    std::shared_ptr<AbilityRecord> GetTopAbility() const;

    /**
     * @brief Get the Mission By Id object
     *
     * @param missionId the given missionId
     * @return the mission of the given id
     */
    std::shared_ptr<Mission> GetMissionById(int missionId) const;

    /**
     * @brief Get the Mission By Id object
     *
     * @param missionId the given missionId
     * @return the mission of the given id
     */
    std::list<std::shared_ptr<Mission>>& GetAllMissions();

    /**
     * @brief Get the type of the missionList
     *
     * @return the mission list type
     */
    MissionListType GetType() const;

    /**
     * @brief Get the launcher root
     *
     * @return launcher root
     */
    std::shared_ptr<AbilityRecord> GetLauncherRoot() const;

    /**
     * @brief get ability record by id
     *
     * @param eventId event id
     * @return std::shared_ptr<AbilityRecord> return ability record
     */
    std::shared_ptr<AbilityRecord> GetAbilityRecordById(int64_t eventId) const;

    /**
     * @brief Get the Ability Record By Caller object
     *
     * @param caller the ability which call terminateAbility
     * @param requestCode startAbilityWithRequstCode
     * @return std::shared_ptr<AbilityRecord> the ability record which find
     */
    std::shared_ptr<AbilityRecord> GetAbilityRecordByCaller(
        const std::shared_ptr<AbilityRecord> &caller, int requestCode);

    /**
     * @brief Get the Ability Record By elementName
     *
     * @param element
     * @return std::shared_ptr<AbilityRecord> the ability record which find
     */
    std::shared_ptr<AbilityRecord> GetAbilityRecordByName(const AppExecFwk::ElementName &element);

    /**
     * Get ability token by target mission id.
     *
     * @param missionId target missionId.
     * @return the ability token of target mission.
     */
    sptr<IRemoteObject> GetAbilityTokenByMissionId(int32_t missionId);

    /**
     * Handle uninstall bundle.
     *
     * @param bundleName name of bundle.
     * @param uid the uid of bundle.
     */
    void HandleUnInstallApp(const std::string &bundleName, int32_t uid);

    /**
     * @brief dump mission
     *
     * @param info dump result.
     */
    void Dump(std::vector<std::string> &info);

    /**
     * @brief dump mission list info
     *
     * @param info dump result.
     */
    void DumpList(std::vector<std::string> &info, bool isClient);

    void DumpStateByRecordId(
        std::vector<std::string> &info, bool isClient, int32_t abilityRecordId, const std::vector<std::string> &params);

    std::shared_ptr<Mission> GetMissionBySpecifiedFlag(const AAFwk::Want &want, const std::string &flag) const;
    #ifdef ABILITY_COMMAND_FOR_TEST
    int BlockAbilityByRecordId(int32_t abilityRecordId);
    #endif

    int32_t GetMissionCountByUid(int32_t targetUid) const;

private:
    std::string GetTypeName();
    bool MatchedInitialMission(const std::shared_ptr<Mission>& mission, const std::string &bundleName, int32_t uid);

    MissionListType type_;
    std::list<std::shared_ptr<Mission>> missions_ {};
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_MISSION_LIST_H
