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

#ifndef CALL_STATUS_CALLBACK_PROXY_H
#define CALL_STATUS_CALLBACK_PROXY_H

#include "iremote_proxy.h"
#include "i_call_status_callback.h"

namespace OHOS {
namespace Telephony {
class CallStatusCallbackProxy : public IRemoteProxy<ICallStatusCallback> {
public:
    explicit CallStatusCallbackProxy(const sptr<IRemoteObject> &impl);
    virtual ~CallStatusCallbackProxy() = default;

    int32_t UpdateCallReportInfo(const CallReportInfo &info) override;
    int32_t UpdateCallsReportInfo(const CallsReportInfo &info) override;
    int32_t UpdateDisconnectedCause(const DisconnectedDetails &details) override;
    int32_t UpdateEventResultInfo(const CellularCallEventInfo &info) override;
    int32_t UpdateRBTPlayInfo(const RBTPlayInfo info) override;
    int32_t UpdateGetWaitingResult(const CallWaitResponse &callWaitResponse) override;
    int32_t UpdateSetWaitingResult(const int32_t result) override;
    int32_t UpdateGetRestrictionResult(const CallRestrictionResponse &callRestrictionResult) override;
    int32_t UpdateSetRestrictionResult(const int32_t result) override;
    int32_t UpdateGetTransferResult(const CallTransferResponse &callTransferResponse) override;
    int32_t UpdateSetTransferResult(const int32_t result) override;
    int32_t UpdateGetCallClipResult(const ClipResponse &clipResponse) override;
    int32_t UpdateGetCallClirResult(const ClirResponse &clirResponse) override;
    int32_t UpdateSetCallClirResult(const int32_t result) override;
    int32_t StartRttResult(const int32_t result) override;
    int32_t StopRttResult(const int32_t result) override;
    int32_t GetImsConfigResult(const GetImsConfigResponse &response) override;
    int32_t SetImsConfigResult(const int32_t result) override;
    int32_t GetImsFeatureValueResult(const GetImsFeatureValueResponse &response) override;
    int32_t SetImsFeatureValueResult(const int32_t result) override;
    int32_t ReceiveUpdateCallMediaModeResponse(const CallMediaModeResponse &response) override;
    int32_t InviteToConferenceResult(const int32_t result) override;
    int32_t StartDtmfResult(const int32_t result) override;
    int32_t StopDtmfResult(const int32_t result) override;
    int32_t SendUssdResult(const int32_t result) override;
    int32_t GetImsCallDataResult(const int32_t result) override;
    int32_t SendMmiCodeResult(const MmiCodeInfo &info) override;

private:
    static inline BrokerDelegator<CallStatusCallbackProxy> delegator_;
};
} // namespace Telephony
} // namespace OHOS

#endif
