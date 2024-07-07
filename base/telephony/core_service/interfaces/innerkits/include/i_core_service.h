/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef I_BASE_PHONE_SERVICE_H
#define I_BASE_PHONE_SERVICE_H

#include "cell_information.h"
#include "dialling_numbers_info.h"
#include "i_network_search_callback.h"
#include "ims_reg_info_callback.h"
#include "network_search_result.h"
#include "network_state.h"
#include "operator_config_types.h"
#include "signal_information.h"
#include "sim_state_type.h"

namespace OHOS {
namespace Telephony {
class ICoreService : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.telephony.ICoreService");

public:
    virtual ~ICoreService() = default;
    virtual int32_t GetPsRadioTech(int32_t slotId, int32_t &psRadioTech) = 0;
    virtual int32_t GetCsRadioTech(int32_t slotId, int32_t &csRadioTech) = 0;
    virtual std::u16string GetOperatorNumeric(int32_t slotId) = 0;
    virtual int32_t GetOperatorName(int32_t slotId, std::u16string &operatorName) = 0;
    virtual int32_t GetSignalInfoList(int32_t slotId, std::vector<sptr<SignalInformation>> &signals) = 0;
    virtual int32_t GetNetworkState(int32_t slotId, sptr<NetworkState> &networkState) = 0;
    virtual int32_t SetRadioState(int32_t slotId, bool isOn, const sptr<INetworkSearchCallback> &callback) = 0;
    virtual int32_t GetRadioState(int32_t slotId, const sptr<INetworkSearchCallback> &callback) = 0;
    virtual int32_t GetImei(int32_t slotId, std::u16string &imei) = 0;
    virtual int32_t GetMeid(int32_t slotId, std::u16string &meid) = 0;
    virtual int32_t GetUniqueDeviceId(int32_t slotId, std::u16string &deviceId) = 0;
    virtual bool IsNrSupported(int32_t slotId) = 0;
    virtual int32_t GetNrOptionMode(int32_t slotId, NrMode &mode) = 0;
    virtual int32_t HasSimCard(int32_t slotId, bool &hasSimCard) = 0;
    virtual int32_t GetSimState(int32_t slotId, SimState &simState) = 0;
    virtual int32_t GetCardType(int32_t slotId, CardType &cardType) = 0;
    virtual int32_t UnlockPin(int32_t slotId, const std::u16string &pin, LockStatusResponse &response) = 0;
    virtual int32_t UnlockPuk(
        int32_t slotId, const std::u16string &newPin, const std::u16string &puk, LockStatusResponse &response) = 0;
    virtual int32_t AlterPin(
        int32_t slotId, const std::u16string &newPin, const std::u16string &oldPin, LockStatusResponse &response) = 0;
    virtual int32_t UnlockPin2(int32_t slotId, const std::u16string &pin2, LockStatusResponse &response) = 0;
    virtual int32_t UnlockPuk2(
        int32_t slotId, const std::u16string &newPin2, const std::u16string &puk2, LockStatusResponse &response) = 0;
    virtual int32_t AlterPin2(
        int32_t slotId, const std::u16string &newPin2, const std::u16string &oldPin2, LockStatusResponse &response) = 0;
    virtual int32_t SetLockState(int32_t slotId, const LockInfo &options, LockStatusResponse &response) = 0;
    virtual int32_t GetLockState(int32_t slotId, LockType lockType, LockState &lockState) = 0;
    virtual int32_t GetSimOperatorNumeric(int32_t slotId, std::u16string &operatorNumeric) = 0;
    virtual int32_t GetISOCountryCodeForSim(int32_t slotId, std::u16string &countryCode) = 0;
    virtual int32_t GetSimSpn(int32_t slotId, std::u16string &spn) = 0;
    virtual int32_t GetSimIccId(int32_t slotId, std::u16string &iccId) = 0;
    virtual int32_t GetIMSI(int32_t slotId, std::u16string &imsi) = 0;
    virtual bool IsSimActive(int32_t slotId) = 0;
    virtual int32_t GetSlotId(int32_t simId) = 0;
    virtual int32_t GetSimId(int32_t slotId) = 0;
    virtual int32_t GetNetworkSearchInformation(int32_t slotId, const sptr<INetworkSearchCallback> &callback) = 0;
    virtual int32_t GetNetworkSelectionMode(int32_t slotId, const sptr<INetworkSearchCallback> &callback) = 0;
    virtual std::u16string GetLocaleFromDefaultSim() = 0;
    virtual int32_t GetSimGid1(int32_t slotId, std::u16string &gid1) = 0;
    virtual std::u16string GetSimGid2(int32_t slotId) = 0;
    virtual std::u16string GetSimEons(int32_t slotId, const std::string &plmn, int32_t lac, bool longNameRequired) = 0;
    virtual int32_t SetNetworkSelectionMode(int32_t slotId, int32_t selectMode,
        const sptr<NetworkInformation> &networkInformation, bool resumeSelection,
        const sptr<INetworkSearchCallback> &callback) = 0;
    virtual int32_t GetIsoCountryCodeForNetwork(int32_t slotId, std::u16string &countryCode) = 0;
    virtual int32_t GetSimAccountInfo(int32_t slotId, IccAccountInfo &info) = 0;
    virtual int32_t SetDefaultVoiceSlotId(int32_t slotId) = 0;
    virtual int32_t GetDefaultVoiceSlotId() = 0;
    virtual int32_t SetPrimarySlotId(int32_t slotId) = 0;
    virtual int32_t GetPrimarySlotId(int32_t &slotId) = 0;
    virtual int32_t SetShowNumber(int32_t slotId, const std::u16string &number) = 0;
    virtual int32_t GetShowNumber(int32_t slotId, std::u16string &showNumber) = 0;
    virtual int32_t SetShowName(int32_t slotId, const std::u16string &name) = 0;
    virtual int32_t GetShowName(int32_t slotId, std::u16string &showName) = 0;
    virtual int32_t GetActiveSimAccountInfoList(std::vector<IccAccountInfo> &iccAccountInfoList) = 0;
    virtual int32_t GetOperatorConfigs(int32_t slotId, OperatorConfig &poc) = 0;
    virtual int32_t RefreshSimState(int32_t slotId) = 0;
    virtual int32_t SetActiveSim(int32_t slotId, int32_t enable) = 0;
    virtual int32_t GetPreferredNetwork(int32_t slotId, const sptr<INetworkSearchCallback> &callback) = 0;
    virtual int32_t SetPreferredNetwork(
        int32_t slotId, int32_t networkMode, const sptr<INetworkSearchCallback> &callback) = 0;
    virtual int32_t GetSimTelephoneNumber(int32_t slotId, std::u16string &telephoneNumber) = 0;
    virtual std::u16string GetSimTeleNumberIdentifier(const int32_t slotId) = 0;
    virtual int32_t GetVoiceMailIdentifier(int32_t slotId, std::u16string &voiceMailIdentifier) = 0;
    virtual int32_t GetVoiceMailNumber(int32_t slotId, std::u16string &voiceMailNumber) = 0;
    virtual int32_t QueryIccDiallingNumbers(
        int slotId, int type, std::vector<std::shared_ptr<DiallingNumbersInfo>> &result) = 0;
    virtual int32_t AddIccDiallingNumbers(
        int slotId, int type, const std::shared_ptr<DiallingNumbersInfo> &diallingNumber) = 0;
    virtual int32_t DelIccDiallingNumbers(
        int slotId, int type, const std::shared_ptr<DiallingNumbersInfo> &diallingNumber) = 0;
    virtual int32_t UpdateIccDiallingNumbers(
        int slotId, int type, const std::shared_ptr<DiallingNumbersInfo> &diallingNumber) = 0;
    virtual int32_t SetVoiceMailInfo(
        const int32_t slotId, const std::u16string &mailName, const std::u16string &mailNumber) = 0;
    virtual int32_t GetImsRegStatus(int32_t slotId, ImsServiceType imsSrvType, ImsRegInfo &info) = 0;
    virtual int32_t GetMaxSimCount() = 0;
    virtual int32_t GetOpKey(int32_t slotId, std::u16string &opkey) = 0;
    virtual int32_t GetOpKeyExt(int32_t slotId, std::u16string &opkeyExt) = 0;
    virtual int32_t GetOpName(int32_t slotId, std::u16string &opname) = 0;
    virtual int32_t SendEnvelopeCmd(int32_t slotId, const std::string &cmd) = 0;
    virtual int32_t SendTerminalResponseCmd(int32_t slotId, const std::string &cmd) = 0;
    virtual int32_t SendCallSetupRequestResult(int32_t slotId, bool accept) = 0;
    virtual int32_t UnlockSimLock(int32_t slotId, const PersoLockInfo &lockInfo, LockStatusResponse &response) = 0;
    virtual int32_t GetCellInfoList(int32_t slotId, std::vector<sptr<CellInformation>> &cellInfo) = 0;
    virtual int32_t SendUpdateCellLocationRequest(int32_t slotId) = 0;
    virtual int32_t HasOperatorPrivileges(const int32_t slotId, bool &hasOperatorPrivileges) = 0;
    virtual int32_t SimAuthentication(
        int32_t slotId, const std::string &aid, const std::string &authData, SimAuthenticationResponse &response) = 0;
    virtual int32_t RegisterImsRegInfoCallback(
        int32_t slotId, ImsServiceType imsSrvType, const sptr<ImsRegInfoCallback> &callback) = 0;
    virtual int32_t UnregisterImsRegInfoCallback(int32_t slotId, ImsServiceType imsSrvType) = 0;
    enum class InterfaceID {
        GET_PS_RADIO_TECH = 0,
        GET_CS_RADIO_TECH,
        GET_OPERATOR_NUMERIC,
        GET_OPERATOR_NAME,
        GET_SIGNAL_INFO_LIST,
        GET_NETWORK_STATE,
        GET_CELL_INFO_LIST,
        SET_RADIO_STATE,
        GET_RADIO_STATE,
        GET_IMEI,
        GET_MEID,
        GET_UNIQUE_DEVICE_ID,
        HAS_SIM_CARD,
        GET_SIM_STATE,
        GET_ISO_COUNTRY_CODE,
        GET_ISO_COUNTRY_CODE_FOR_NETWORK,
        SET_PS_ATTACH_STATUS,
        GET_SPN,
        GET_ICCID,
        GET_IMSI,
        IS_SIM_ACTIVE,
        UNLOCK_PIN,
        UNLOCK_PUK,
        ALTER_PIN,
        CHECK_LOCK,
        SWITCH_LOCK,
        UNLOCK_PIN2,
        UNLOCK_PUK2,
        ALTER_PIN2,
        GET_SIM_OPERATOR_NUMERIC,
        GET_NETWORK_SEARCH_RESULT,
        GET_NETWORK_SELECTION_MODE,
        GET_SIM_LANGUAGE,
        GET_SIM_GID1,
        GET_SIM_GID2,
        SET_NETWORK_SELECTION_MODE,
        GET_CELL_LOCATION,
        GET_SIM_SUB_INFO,
        SET_DEFAULT_VOICE_SLOTID,
        GET_DEFAULT_VOICE_SLOTID,
        SET_PRIMARY_SLOTID,
        GET_PRIMARY_SLOTID,
        SET_DEFAULT_DATA_SLOTID,
        GET_DEFAULT_DATA_SLOTID,
        SET_SHOW_NUMBER,
        GET_SHOW_NUMBER,
        SET_SHOW_NAME,
        GET_SHOW_NAME,
        GET_ACTIVE_ACCOUNT_INFO_LIST,
        GET_OPERATOR_CONFIG,
        REFRESH_SIM_STATE,
        SET_SIM_ACTIVE,
        GET_PREFERRED_NETWORK_MODE,
        SET_PREFERRED_NETWORK_MODE,
        GET_SIM_PHONE_NUMBER,
        GET_SIM_TELENUMBER_IDENTIFIER,
        GET_VOICE_MAIL_TAG,
        GET_VOICE_MAIL_NUMBER,
        ICC_DIALLING_NUMBERS_GET,
        ICC_DIALLING_NUMBERS_DELETE,
        ICC_DIALLING_NUMBERS_INSERT,
        ICC_DIALLING_NUMBERS_UPDATE,
        SET_VOICE_MAIL,
        GET_MAX_SIM_COUNT,
        GET_OPKEY,
        GET_OPKEY_EXT,
        GET_OPNAME,
        GET_IMS_REG_STATUS,
        STK_CMD_FROM_APP_ENVELOPE,
        STK_CMD_FROM_APP_TERMINAL_RESPONSE,
        STK_RESULT_FROM_APP_CALL_SETUP_REQUEST,
        GET_CARD_TYPE,
        UNLOCK_SIMLOCK,
        HAS_OPERATOR_PRIVILEGES,
        SIM_AUTHENTICATION,
        IS_NR_SUPPORTED,
        GET_NR_OPTION_MODE,
        REG_IMS_CALLBACK,
        UN_REG_IMS_CALLBACK,
        GET_SIM_EONS,
        GET_SIM_SLOTID,
        GET_SIM_SIMID,
    };

protected:
    const int32_t ERROR = -1;
    const int32_t MIN_STRING_LE = 1;
    const int32_t MAX_STRING_LE = 32;
    const int32_t MAX_VECTOR = 100;
};
} // namespace Telephony
} // namespace OHOS
#endif // I_BASE_PHONE_SERVICE_H
