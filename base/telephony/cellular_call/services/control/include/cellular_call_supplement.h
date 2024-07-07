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

#ifndef CELLULAR_CALL_SUPPLEMENT_H
#define CELLULAR_CALL_SUPPLEMENT_H

#include <string>

#include "cellular_call_data_struct.h"
#include "hril_types.h"
#include "hril_call_parcel.h"
#include "module_service_utils.h"
#include "supplement_request.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
class CellularCallSupplement {
public:
    /**
     * Deal Clip
     *
     * 3GPP TS 22.030 V4.0.0 (2001-03)  6.5.6.2	Calling Line Identification Presentation (CLIP)
     * The CLIP Supplementary Service is defined in GSM 02.81[12]
     * 3GPP TS 22.030 V4.0.0 (2001-03) Annex B (normative):Codes for defined Supplementary Services
     *
     * @param slotId
     * @param MMIData
     */
    void GetClip(int32_t slotId, const MMIData &mmiData);

    /**
     * Deal Clir
     *
     * 3GPP TS 22.081 V4.0.0 (2001-03) 2 Calling Line Identification Restriction (CLIR)
     * 3GPP TS 22.030 V4.0.0 (2001-03) Annex B (normative):Codes for defined Supplementary Services
     *
     * @param slotId
     * @param MMIData
     */
    void GetClir(int32_t slotId, const MMIData &mmiData);

    /**
     * Deal Call Transfer
     *
     * 3GPP TS 22.030 V4.0.0 (2001-03)
     * 3GPP TS 22.030 V4.0.0 (2001-03) Annex B (normative):Codes for defined Supplementary Services
     *
     * @param slotId
     * @param MMIData
     */
    void DealCallTransfer(int32_t slotId, const MMIData &mmiData);

    /**
     * Set Call Transfer
     *
     * 27007-430_2001 7.11 Call forwarding number and conditions +CCFC
     * 3GPP TS 22.082 [4]
     * Control of the call forwarding supplementary service
     *
     * @param slotId
     * @param CallTransferInfo
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetCallTransferInfo(int32_t slotId, const CallTransferInfo &cTInfo);

    /**
     * Inquire Call Transfer
     *
     * 27007-430_2001 7.11 Call forwarding number and conditions +CCFC
     * 3GPP TS 22.082 [4]
     * Control of the call forwarding supplementary service
     *
     * @param slotId
     * @param CallTransferType
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t GetCallTransferInfo(int32_t slotId, CallTransferType type);

    /**
     * Deal Call Restriction
     *
     * 3GPP TS 22.088 [6] 1	Barring of outgoing calls
     * 3GPP TS 22.088 [6] 2	Barring of incoming calls
     *
     * @param slotId
     * @param MMIData
     */
    void DealCallRestriction(int32_t slotId, const MMIData &mmiData);

    /**
     * Set Call Restriction
     *
     * 27007-430_2001 7.4	Facility lock +CLCK
     * 3GPP TS 22.088 [6] 1	Barring of outgoing calls
     * 3GPP TS 22.088 [6] 2	Barring of incoming calls
     *
     * Control of the call barring supplementary service
     *
     * @param slotId
     * @param CallRestrictionInfo
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetCallRestriction(int32_t slotId, const CallRestrictionInfo &cRInfo);

    /**
     * Inquire Call Restriction
     *
     * 27007-430_2001 7.4	Facility lock +CLCK
     * 3GPP TS 22.088 [6] 1	Barring of outgoing calls
     * 3GPP TS 22.088 [6] 2	Barring of incoming calls
     *
     * Control of the call barring supplementary service
     *
     * @param slotId
     * @param CallRestrictionType
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t GetCallRestriction(int32_t slotId, CallRestrictionType facType);

    /**
     * Deal Call Waiting
     *
     * 27007-430_2001 7.12	Call waiting +CCWA
     * 3GPP TS 22.083 [5] 1	Call waiting (CW)
     *
     * @param slotId
     * Deal Call Waiting
     * @param MMIData
     */
    void DealCallWaiting(int32_t slotId, const MMIData &mmiData);

    /**
     * Set Call Waiting
     *
     * 27007-430_2001 7.12	Call waiting +CCWA
     * 3GPP TS 22.083 [5] 1	Call waiting (CW)
     *
     * Control of the Call Waiting supplementary service
     *
     * @param slotId
     * @param activate
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetCallWaiting(int32_t slotId, bool activate);

    /**
     * Inquire Call Waiting
     *
     * 27007-430_2001 7.12	Call waiting +CCWA
     * 3GPP TS 22.083 [5] 1	Call waiting (CW)
     *
     * Control of the Call Waiting supplementary service
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t GetCallWaiting(int32_t slotId);

    /**
     * Send Ussd msg
     *
     * 22.090 Unstructured Supplementary Service Data (USSD); Stage 1
     * 23.090 Unstructured Supplementary Service Data (USSD); Stage 2
     * 24.090 Unstructured Supplementary Service Data (USSD); Stage 3
     *
     * Control of the Ussd supplementary service
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SendUssd(int32_t slotId, const std::string &msg);

    /**
     * Event Set Pin Puk
     *
     * @param PinPukResponse
     */
    void EventSetPinPuk(const PinPukResponse &pinPukResponse);

    /**
     * SIM PIN unlock
     *
     * @param slotId
     * @param mmiData
     * @return Returns true on success.
     */
    void AlterPinPassword(int32_t slotId, const MMIData &mmiData);

    /**
     * SIM PUK unlock
     *
     * @param slotId
     * @param mmiData
     * @return Returns true on success.
     */
    void UnlockPuk(int32_t slotId, const MMIData &mmiData);

    /**
     * SIM PIN2 unlock
     *
     * @param slotId
     * @param mmiData
     * @return Returns true on success.
     */
    void AlterPin2Password(int32_t slotId, const MMIData &mmiData);

    /**
     * SIM PUK2 unlock
     *
     * @param slotId
     * @param mmiData
     * @return Returns true on success.
     */
    void UnlockPuk2(int32_t slotId, const MMIData &mmiData);

    /**
     * Event Deal clip Mmi
     *
     * 3GPP TS 27.007 V3.9.0 (2001-06) 7.6	Calling line identification presentation +CLIP
     *
     * @param GetClipResult
     */
    void EventGetClip(GetClipResult &getClipResult);

    /**
     * Event Deal Clir Mmi
     *
     * 3GPP TS 27.007 V3.9.0 (2001-06) 7.7	Calling line identification restriction +CLIR
     *
     * @param GetClirResult
     */
    void EventGetClir(GetClirResult &result);

    /**
     * Event set Clir Mmi
     *
     * 3GPP TS 27.007 V3.9.0 (2001-06) 7.7	Calling line identification restriction +CLIR
     *
     * @param HRilRadioResponseInfo
     */
    void EventSetClir(HRilRadioResponseInfo &info);

    /**
     * GetCallRestriction result
     *
     * 3GPP TS 27.007 V3.9.0 (2001-06) 7.4	Facility lock +CLCK
     *
     * @param CallRestrictionResult
     */
    void EventGetCallRestriction(const CallRestrictionResult &result);

    /**
     * SetCallRestriction result
     *
     * 3GPP TS 27.007 V3.9.0 (2001-06) 7.4	Facility lock +CLCK
     *
     * @param HRilRadioResponseInfo
     */
    void EventSetCallRestriction(HRilRadioResponseInfo &info);

    /**
     * Call Waiting result
     *
     * 3GPP TS 27.007 V3.9.0 (2001-06) 7.12	Call waiting +CCWA
     * status: 0	not active;  1	active
     *
     * @param CallWaitResult waitingInfo
     */
    void EventGetCallWaiting(CallWaitResult &waitingInfo);

    /**
     * Event Set CallWaiting
     *
     * 3GPP TS 27.007 V3.9.0 (2001-06) 7.12	Call waiting +CCWA
     *
     * @param HRilRadioResponseInfo
     */
    void EventSetCallWaiting(HRilRadioResponseInfo &responseInfo);

    /**
     * Event Inquire Call Transfer
     *
     * 3GPP TS 27.007 V3.9.0 (2001-06) 7.11	Call forwarding number and conditions +CCFC
     *
     * @param CallForwardQueryInfoList
     */
    void EventGetCallTransferInfo(CallForwardQueryInfoList &cFQueryList);

    /**
     * Event Set Call Transfer
     *
     * 3GPP TS 27.007 V3.9.0 (2001-06) 7.11	Call forwarding number and conditions +CCFC
     *
     * @param HRilRadioResponseInfo
     */
    void EventSetCallTransferInfo(HRilRadioResponseInfo &responseInfo);

    /**
     * Event Send Ussd
     *
     * 3GPP TS 3GPP TS 22.030 V16.0.0 (2020-07) 6.5	Supplementary Services Control
     *
     * @param HRilRadioResponseInfo
     */
    void EventSendUssd(HRilRadioResponseInfo &responseInfo);

    /**
     * Event Ss Notify
     *
     * @param SsNoticeInfo
     */
    void EventSsNotify(SsNoticeInfo &ssNoticeInfo);

    /**
     * Event Ussd Notify
     *
     * @param UssdNoticeInfo
     */
    void EventUssdNotify(UssdNoticeInfo &ussdNoticeInfo);

private:
    /**
     * Obtain ServiceCode
     *
     * @param serviceInfoB
     * @return ServiceClassType
     */
    int32_t ObtainServiceCode(const std::string &serviceInfoB);

    /**
     * Obtain Cause
     *
     * 3GPP TS 22.030 V4.0.0 (2001-03) Annex B (normative): Codes for defined Supplementary Services
     *
     * @param basicString
     * @return CallTransferType
     */
    int32_t ObtainCause(const std::string &serviceInfoC);

    /**
     * Obtain Barring Installation
     *
     * 27007-430_2001 7.4	Facility lock +CLCK
     * 3GPP TS 22.088 [6] 1	Barring of outgoing calls
     * 3GPP TS 22.088 [6] 2	Barring of incoming calls
     *
     * @param serviceInfoC
     * @return Barring type
     */
    std::string ObtainBarringInstallation(const std::string &serviceInfoC);

    /**
     * Is Phone Gsm Type
     *
     * @param slotId
     * @return bool
     */
    bool PhoneTypeGsmOrNot(int32_t slotId);

    /**
     * Get Message
     *
     * @param MmiCodeInfo
     * @param SsNoticeInfo
     */
    void GetMessage(MmiCodeInfo &mmiCodeInfo, const SsNoticeInfo &ssNoticeInfo);

private:
    void BuildCallForwardQueryInfo(const CallForwardQueryResult &queryResult);
    void ReportMmiCodeMessage(int32_t result, const std::string successMsg, const std::string failedMsg);
    bool IsVaildPinOrPuk(std::string newPinOrPuk, std::string newPinOrPukCheck);

private:
    SupplementRequest supplementRequest_;
    ModuleServiceUtils moduleServiceUtils_;
};
} // namespace Telephony
} // namespace OHOS
#endif // CELLULAR_CALL_SUPPLEMENT_H
