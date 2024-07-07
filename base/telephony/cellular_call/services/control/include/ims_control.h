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

#ifndef CELLULAR_CALL_CONTROL_IMS_H
#define CELLULAR_CALL_CONTROL_IMS_H

#include "hril_call_parcel.h"
#include "cellular_call_data_struct.h"
#include "cellular_call_connection_ims.h"
#include "control_base.h"

namespace OHOS {
namespace Telephony {
using ImsConnectionMap = std::map<std::string, CellularCallConnectionIMS>;
class IMSControl : public ControlBase {
public:
    /**
     * constructor
     */
    IMSControl() = default;

    /**
     * destructor
     */
    ~IMSControl();

    /**
     * Dial
     *
     * @param CellularCallInfo
     * @return Error Code: Returns TELEPHONY_NO_ERROR on success, others on failure.
     */
    int32_t Dial(const CellularCallInfo &callInfo) override;

    /**
     * HangUp
     *
     * @param CellularCallInfo
     * @param CallSupplementType
     * @return Error Code: Returns TELEPHONY_NO_ERROR on success, others on failure.
     */
    int32_t HangUp(const CellularCallInfo &callInfo, CallSupplementType type) override;

    /**
     * Answer
     *
     * @param CellularCallInfo
     * @return Error Code: Returns TELEPHONY_NO_ERROR on success, others on failure.
     */
    int32_t Answer(const CellularCallInfo &callInfo) override;

    /**
     * Reject
     *
     * @param CellularCallInfo
     * @return Error Code: Returns TELEPHONY_NO_ERROR on success, others on failure.
     */
    int32_t Reject(const CellularCallInfo &callInfo) override;

    /**
     * HoldCall
     *
     * @param slotId
     * @return Error Code: Returns TELEPHONY_NO_ERROR on success, others on failure.
     */
    int32_t HoldCall(int32_t slotId) override;

    /**
     * UnHoldCall
     *
     * @param slotId
     * @return Error Code: Returns TELEPHONY_NO_ERROR on success, others on failure.
     */
    int32_t UnHoldCall(int32_t slotId) override;

    /**
     * SwitchCall
     *
     * @param slotId
     * @return Error Code: Returns TELEPHONY_NO_ERROR on success, others on failure.
     */
    int32_t SwitchCall(int32_t slotId) override;

    /**
     * CombineConference
     *
     * 22084-400_2001 1.3.8.2	Managing an active multiParty call
     * 3GPP TS 22.030
     *
     * Add another remote party
     * @param slotId
     * @return Error Code: Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t CombineConference(int32_t slotId) override;

    /**
     * InviteToConference
     *
     * @param slotId
     * @param numberList
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t InviteToConference(int32_t slotId, const std::vector<std::string> &numberList);

    /**
     * KickOutFromConference
     *
     * @param slotId
     * @param numberList
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t KickOutFromConference(int32_t slotId, const std::vector<std::string> &numberList);

    /**
     * HangUpAllConnection
     *
     * @return Error Code: Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t HangUpAllConnection(int32_t slotId) override;

    /**
     * Update Call Media Mode
     *
     * @param CellularCallInfo
     * @param ImsCallMode
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t UpdateImsCallMode(const CellularCallInfo &callInfo, ImsCallMode mode);

    /**
     * Start Rtt
     *
     * @param msg
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t StartRtt(int32_t slotId, const std::string &msg);

    /**
     * Stop Rtt
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t StopRtt(int32_t slotId);

    /**
     * Release all Connection
     */
    void ReleaseAllConnection();

    /**
     * GetConnectionMap
     *
     * @return ImsConnectionMap
     */
    ImsConnectionMap GetConnectionMap();

    /**
     * IMSControl ReportImsCallsData.
     *
     * @param slotId
     * @param CallInfoList
     * @returns Error Code: Returns TELEPHONY_NO_ERROR on success, others on failure.
     */
    int32_t ReportImsCallsData(int32_t slotId, const ImsCurrentCallList &callInfoList);

    /**
     * IMSControl ReportCallsData.
     *
     * @param slotId
     * @param CallInfoList
     * @returns Error Code: Returns TELEPHONY_NO_ERROR on success, others on failure.
     */
    int32_t ReportCallsData(int32_t slotId, const CallInfoList &callInfoList) override;

private:
    /**
     * handle dial judgment
     *
     * @param slotId
     * @param std::string phoneNum
     * @param CLIRMode clirMode
     * @param videoState  0: audio 1:video
     * @returns Error Code: Returns TELEPHONY_NO_ERROR on success, others on failure.
     */
    int32_t DialJudgment(int32_t slotId, const std::string &phoneNum, CLIRMode &clirMode, int32_t videoState);

    /**
     * Encapsulate Dial Common
     *
     * @param std::string phoneNum
     * @param CLIRMode clirMode
     * @param videoState  0: audio 1:video
     * @returns Error Code: Returns TELEPHONY_NO_ERROR on success, others on failure.
     */
    int32_t EncapsulateDial(int32_t slotId, const std::string &phoneNum, CLIRMode &clirMode, int32_t videoState) const;

    /**
     * Report being hung up data
     *
     * @return Error Code: Returns TELEPHONY_NO_ERROR on success, others on failure.
     */
    int32_t ReportHungUpInfo(int32_t slotId);

    /**
     * Report Incoming info
     *
     * @param CallInfoList
     * @return Error Code: Returns TELEPHONY_NO_ERROR on success, others on failure.
     */
    int32_t ReportIncomingInfo(int32_t slotId, const ImsCurrentCallList &callInfoList);

    /**
     * Report update info
     *
     * @param CallInfoList
     * @return Error Code: Returns TELEPHONY_NO_ERROR on success, others on failure.
     */
    int32_t ReportUpdateInfo(int32_t slotId, const ImsCurrentCallList &callInfoList);

    /**
     * Encapsulation CallReportInfo
     *
     * @param CallInfo
     * @return CallReportInfo
     */
    CallReportInfo EncapsulationCallReportInfo(int32_t slotId, const ImsCurrentCall &callInfo);

    /**
     * DeleteConnection Connection send
     *
     * @param CallsReportInfo
     * @param CallInfoList
     */
    void DeleteConnection(CallsReportInfo &callsReportInfo, const ImsCurrentCallList &callInfoList);

private:
    ImsConnectionMap connectionMap_; // save callConnection map
};
} // namespace Telephony
} // namespace OHOS

#endif // CELLULAR_CALL_CONTROL_IMS_H
