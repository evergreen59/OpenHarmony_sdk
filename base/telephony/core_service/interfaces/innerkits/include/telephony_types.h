/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef TELEPHONY_TELEPHONY_TYPES_H
#define TELEPHONY_TELEPHONY_TYPES_H

#include <string>
#include <vector>

#include "network_search_types.h"
#include "operator_config_types.h"
#include "parameter.h"

namespace OHOS {
namespace Telephony {
#define SIM_SLOT_COUNT GetMaxSlotCount<int32_t>()
#define PREFERRED_NETWORK_TYPE GetPreferredNetworkType<int32_t>()
inline const int32_t SYSPARA_SIZE = 128;
inline const int32_t DEFAULT_SIM_SLOT_ID = 0;
inline const int32_t DEFAULT_SIM_SLOT_ID_REMOVE = -1;
inline const int32_t INVALID_MAIN_CARD_SLOTID = -2;
inline const int32_t ERROR_SLOT_OPKEY = -2;
inline constexpr const char *DEFAULT_SLOT_COUNT = "1";
inline constexpr const char *TEL_SIM_SLOT_COUNT = "const.telephony.slotCount";
inline constexpr const char *DEFAULT_PREFERRED_NETWORK_TYPE = "5"; // CORE_NETWORK_MODE_LTE_WCDMA_GSM
inline constexpr const char *TEL_PREFERRED_NETWORK_TYPE = "const.telephony.preferredNetworkType";
inline constexpr const char *DEFAULT_OPERATOR_KEY = "";
inline constexpr const char *INITIAL_OPKEY = "-1";
inline constexpr const char *DEFAULT_OPERATOR_CONFIG = "default_operator_config.json";
inline constexpr const char *OPKEY_PROP_PREFIX = "telephony.sim.opkey";
inline constexpr const char *COUNTRY_CODE_KEY = "telephony.sim.countryCode";

template<typename T>
inline T GetMaxSlotCount()
{
    char simSlotCount[SYSPARA_SIZE] = { 0 };
    GetParameter(TEL_SIM_SLOT_COUNT, DEFAULT_SLOT_COUNT, simSlotCount, SYSPARA_SIZE);
    T slotCount = std::atoi(simSlotCount);
    return slotCount;
}

template<typename T>
inline T GetPreferredNetworkType()
{
    char preferredNetworkType[SYSPARA_SIZE] = { 0 };
    GetParameter(TEL_PREFERRED_NETWORK_TYPE, DEFAULT_PREFERRED_NETWORK_TYPE, preferredNetworkType, SYSPARA_SIZE);
    T networkType = std::atoi(preferredNetworkType);
    return networkType;
}

enum SimSlotId {
    SIM_SLOT_0 = 0,
    SIM_SLOT_1,
    SIM_SLOT_2,
    SIM_SLOT_3,
};

enum ModemPowerState { CORE_SERVICE_POWER_NOT_AVAILABLE = -1, CORE_SERVICE_POWER_OFF = 0, CORE_SERVICE_POWER_ON = 1 };

template<typename T>
struct TelRilResponseInfo {
    int32_t slotId;
    int32_t flag;
    int32_t errorNo;
    T object;
};

struct LinkBandwidthRule {
    int32_t rat;
    int32_t delayMs;
    int32_t delayUplinkKbps;
    int32_t delayDownlinkKbps;
    std::vector<int32_t> maximumUplinkKbps;
    std::vector<int32_t> maximumDownlinkKbps;
};

struct DataProfile {
    int profileId;
    /** (Access Point Name) a string parameter which is a logical name that is used to select the
     * GGSN or the external packet data network. from 3GPP TS 27.007 10.1 V4.3.0 (2001-12)
     */
    std::string apn;
    /** (Packet Data Protocol type) a string parameter which specifies the type of packet
     * data protocol from 3GPP TS 27.007 10.1 V4.3.0 (2001-12)
     */
    std::string protocol;
    int32_t verType;
    std::string userName;
    std::string password;
    std::string roamingProtocol;
};

struct ActivateDataParam {
    int32_t param; // eg:AppExecFwk::InnerEvent::Get(eventId, activateData.param);
    int32_t radioTechnology;
    struct DataProfile dataProfile;
    bool isRoaming;
    bool allowRoaming;
};

struct DeactivateDataParam {
    int32_t param; // eg:AppExecFwk::InnerEvent::Get(eventId, deactivateData.param);
    int32_t cid;
    int32_t reason;
};

struct CallTransferParam {
    int32_t mode;
    int32_t reason;
    int32_t classx;
    std::string number;
};

struct CallRestrictionParam {
    int32_t mode;
    std::string password;
    std::string fac;
};

struct DtmfParam {
    int32_t index;
    int32_t switchOn;
    int32_t switchOff;
    std::string sDTMFCode;
};

struct CBConfigParam {
    int32_t mode;
    std::string idList;
    std::string dcsList;
};

struct CdmaSimMessageParam {
    int32_t cdmaIndex;
    int32_t status;
    std::string pdu;
};

struct GsmSimMessageParam {
    int64_t refId;
    std::string smscPdu;
    std::string pdu;
};

struct SimMessageParam {
    int32_t gsmIndex;
    int32_t status;
    std::string smscPdu;
    std::string pdu;
};

struct SimLockParam {
    std::string fac;
    int32_t mode;
    std::string passwd;
};

struct SimPasswordParam {
    int32_t passwordLength;
    std::string fac;
    std::string oldPassword;
    std::string newPassword;
};

enum class EccType : int32_t {
    TYPE_CATEGORY = 0,
    TYPE_POLICE = 1,
    TYPE_AMBULANCE = 2,
    TYPE_FIRE = 4,
    TYPE_SEA = 8,
    TYPE_MOUNTAIN = 16,
};

enum class SimpresentType : int32_t {
    TYPE_NO_CARD = 0,
    TYPE_HAS_CARD = 1,
};

enum class AbnormalServiceType : int32_t {
    TYPE_ALL = 0,
    TYPE_ONLY_CS = 1,
};

struct EmergencyCall {
    std::string eccNum;
    std::string mcc;
    EccType eccType;
    SimpresentType simpresent;
    AbnormalServiceType abnormalService;
    bool operator==(const EmergencyCall &call)
    {
        return (eccNum == call.eccNum && mcc == call.mcc);
    }
};

struct SetEccListResponse {
    int32_t result;
    int32_t value;
};

struct PlmnNetworkName {
    std::string longName = "";
    std::string shortName = "";
};

struct OperatorPlmnInfo {
    std::string plmnNumeric = "";
    int32_t lacStart = 0;
    int32_t lacEnd = 0;
    int32_t pnnRecordId = 0;
};

enum class RadioProtocolPhase {
    RADIO_PROTOCOL_PHASE_INITIAL,
    RADIO_PROTOCOL_PHASE_CHECK,
    RADIO_PROTOCOL_PHASE_UPDATE,
    RADIO_PROTOCOL_PHASE_NOTIFY,
    RADIO_PROTOCOL_PHASE_COMPLETE,
};

enum class RadioProtocolStatus {
    RADIO_PROTOCOL_STATUS_NONE,
    RADIO_PROTOCOL_STATUS_SUCCESS,
    RADIO_PROTOCOL_STATUS_FAIL,
};

struct RadioProtocol {
    int32_t slotId;
    int32_t sessionId;
    RadioProtocolPhase phase;
    int32_t technology;
    int32_t modemId;
    RadioProtocolStatus status;
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_TELEPHONY_TYPES_H
