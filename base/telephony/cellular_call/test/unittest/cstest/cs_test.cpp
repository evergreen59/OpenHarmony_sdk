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

#include "cs_test.h"

#define private public
#define protected public
#include "cellular_call_register.h"
#include "cellular_call_service.h"
#include "core_service_client.h"
#include "cs_control.h"
#include "hril_call_parcel.h"
#include "operator_config_types.h"
#include "securec.h"
#include "sim_state_type.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;
const int32_t SIM1_SLOTID = 0;
const int32_t SIM2_SLOTID = 1;
const int32_t INVALID_SLOTID = -1;
const int32_t INVALID_HANG_UP_TYPE = -1;
const int32_t RESULT = 1;
const std::string PHONE_NUMBER = "0000000";
const std::string PHONE_NUMBER_SECOND = "1111111";
const std::string PHONE_NUMBER_THIRD = "2222222";
const int32_t CELLULAR_CALL_SUCCESS = 0;

bool CsTest::HasSimCard(int32_t slotId)
{
    bool hasSimCard = false;
    DelayedRefSingleton<CoreServiceClient>::GetInstance().HasSimCard(slotId, hasSimCard);
    return hasSimCard;
}

void CsTest::SetUpTestCase(void)
{
    // step 3: Set Up Test Case
}

void CsTest::TearDownTestCase(void)
{
    // step 3: Tear Down Test Case
}

void CsTest::SetUp(void)
{
    // step 3: input testcase setup step
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::DIAL)] = &CsTest::Dial;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::HANG_UP)] = &CsTest::HangUp;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::REJECT)] = &CsTest::Reject;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::ANSWER)] = &CsTest::Answer;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::HOLD_CALL)] = &CsTest::HoldCall;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::UN_HOLD_CALL)] = &CsTest::UnHoldCall;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::SWITCH_CALL)] = &CsTest::SwitchCall;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::EMERGENCY_CALL)] =
        &CsTest::IsEmergencyPhoneNumber;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::COMBINE_CONFERENCE)] =
        &CsTest::CombineConference;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::SEPARATE_CONFERENCE)] =
        &CsTest::SeparateConference;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::INVITE_TO_CONFERENCE)] =
        &CsTest::InviteToConference;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::KICK_OUT_CONFERENCE)] =
        &CsTest::KickOutFromConference;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::HANG_UP_ALL_CONNECTION)] =
        &CsTest::HangUpAllConnection;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::UPDATE_CALL_MEDIA_MODE)] =
        &CsTest::UpdateImsCallMode;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::REGISTER_CALLBACK)] =
        &CsTest::RegisterCallBack;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::UNREGISTER_CALLBACK)] =
        &CsTest::UnRegisterCallBack;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::START_DTMF)] = &CsTest::StartDtmf;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::STOP_DTMF)] = &CsTest::StopDtmf;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::SEND_DTMF)] = &CsTest::SendDtmf;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::START_RTT)] = &CsTest::StartRtt;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::STOP_RTT)] = &CsTest::StopRtt;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::SET_CALL_TRANSFER)] =
        &CsTest::SetCallTransferInfo;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::GET_CALL_TRANSFER)] =
        &CsTest::GetCallTransferInfo;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::SET_CALL_WAITING)] =
        &CsTest::SetCallWaiting;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::GET_CALL_WAITING)] =
        &CsTest::GetCallWaiting;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::SET_CALL_RESTRICTION)] =
        &CsTest::SetCallRestriction;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::GET_CALL_RESTRICTION)] =
        &CsTest::GetCallRestriction;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::SET_MUTE)] = &CsTest::SetMute;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::GET_MUTE)] = &CsTest::GetMute;
}

void CsTest::TearDown(void)
{
    // step 3: input testcase teardown step
}

int32_t CsTest::Dial(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test Dial entry.\n";
    CellularCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        std::cout << "CellularCallService return, memset_s failed. \n";
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    std::cout << "please enter the phone number:";
    std::cin >> callInfo.phoneNum;
    callInfo.videoState = 1;
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    callInfo.slotId = slotId;
    return telephonyService->Dial(callInfo);
}

int32_t CsTest::HangUp(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test HangUp entry.\n";
    CellularCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        std::cout << "CellularCallService return, memset_s failed. \n";
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    std::cout << "please enter the phone number:";
    std::cin >> callInfo.phoneNum;
    callInfo.videoState = 1;
    std::cout << "please enter the call type(0:CS  1:IMS):";
    int32_t callType = 0;
    std::cin >> callType;
    callInfo.callType = static_cast<CallType>(callType);
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    callInfo.slotId = slotId;
    std::cout << "please enter the connect index:";
    std::cin >> callInfo.index;
    std::cout << "please enter the type:";
    int32_t type;
    std::cin >> type;
    return telephonyService->HangUp(callInfo, static_cast<CallSupplementType>(type));
}

int32_t CsTest::Answer(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test Answer entry.\n";
    CellularCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        std::cout << "CellularCallService return, memset_s failed. \n";
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    std::cout << "please enter the phone number:";
    std::cin >> callInfo.phoneNum;
    callInfo.videoState = 1;
    std::cout << "please enter the call type(0:CS  1:IMS):";
    int32_t callType = 0;
    std::cin >> callType;
    callInfo.callType = static_cast<CallType>(callType);
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    callInfo.slotId = slotId;
    return telephonyService->Answer(callInfo);
}

int32_t CsTest::Reject(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test Reject entry.\n";
    CellularCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        std::cout << "CellularCallService return, memset_s failed. \n";
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    std::cout << "please enter the phone number:";
    std::cin >> callInfo.phoneNum;
    callInfo.videoState = 1;
    std::cout << "please enter the call type(0:CS  1:IMS):";
    int32_t callType = 0;
    std::cin >> callType;
    callInfo.callType = static_cast<CallType>(callType);
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    callInfo.slotId = slotId;
    std::cout << "please enter the connect index:";
    std::cin >> callInfo.index;
    return telephonyService->Reject(callInfo);
}

int32_t CsTest::HoldCall(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test HoldCall entry.\n";
    CellularCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        std::cout << "CellularCallService return, memset_s failed. \n";
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    std::cout << "please enter the phone number:";
    std::cin >> callInfo.phoneNum;
    callInfo.videoState = 1;
    std::cout << "please enter the call type(0:CS  1:IMS):";
    int32_t callType = 0;
    std::cin >> callType;
    callInfo.callType = static_cast<CallType>(callType);
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    callInfo.slotId = slotId;
    return telephonyService->HoldCall(callInfo);
}

int32_t CsTest::UnHoldCall(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test UnHoldCall entry.\n";
    CellularCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        std::cout << "CellularCallService return, memset_s failed. \n";
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    std::cout << "please enter the phone number:";
    std::cin >> callInfo.phoneNum;
    callInfo.videoState = 1;
    std::cout << "please enter the call type(0:CS  1:IMS):";
    int32_t callType = 0;
    std::cin >> callType;
    callInfo.callType = static_cast<CallType>(callType);
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    callInfo.slotId = slotId;
    return telephonyService->UnHoldCall(callInfo);
}

int32_t CsTest::SwitchCall(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test SwitchCall entry.\n";
    CellularCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        std::cout << "CellularCallService return, memset_s failed. \n";
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    std::cout << "please enter the phone number:";
    std::cin >> callInfo.phoneNum;
    callInfo.videoState = 1;
    std::cout << "please enter the call type(0:CS  1:IMS):";
    int32_t callType = 0;
    std::cin >> callType;
    callInfo.callType = static_cast<CallType>(callType);
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    callInfo.slotId = slotId;
    return telephonyService->SwitchCall(callInfo);
}

int32_t CsTest::IsEmergencyPhoneNumber(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test IsEmergencyPhoneNumber entry.\n";
    std::string phoneNum;
    std::cout << "input phoneNum: ";
    std::cin >> phoneNum;
    std::cout << "input slotId: ";
    int32_t slotId = 0;
    std::cin >> slotId;
    bool enabled = false;
    telephonyService->IsEmergencyPhoneNumber(slotId, phoneNum, enabled);
    return enabled;
}

int32_t CsTest::CombineConference(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test CombineConference entry." << std::endl;
    CellularCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        std::cout << "CellularCallService return, memset_s failed. \n";
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    std::cout << "please enter the phone number:";
    std::cin >> callInfo.phoneNum;
    callInfo.videoState = 1;
    std::cout << "please enter the call type(0:CS  1:IMS):";
    int32_t callType = 0;
    std::cin >> callType;
    callInfo.callType = static_cast<CallType>(callType);
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    callInfo.slotId = slotId;
    return telephonyService->CombineConference(callInfo);
}

int32_t CsTest::SeparateConference(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test SeparateConference entry." << std::endl;
    CellularCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        std::cout << "CellularCallService return, memset_s failed. \n";
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    std::cout << "please enter the need split phone number:";
    std::cin >> callInfo.phoneNum;
    callInfo.videoState = 1;
    std::cout << "please enter the call type(0:CS  1:IMS):";
    int32_t callType = 0;
    std::cin >> callType;
    callInfo.callType = static_cast<CallType>(callType);
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    callInfo.slotId = slotId;
    std::cout << "please enter the need split index:";
    std::cin >> callInfo.index;
    return telephonyService->SeparateConference(callInfo);
}

int32_t CsTest::InviteToConference(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test InviteToConference entry." << std::endl;
    std::vector<std::string> numberList;
    std::cout << "please enter the invite phone number:";
    std::string phoneNum;
    std::cin >> phoneNum;
    numberList.push_back(phoneNum);
    std::cout << "please enter the slotId:";
    int32_t slotId = 0;
    return telephonyService->InviteToConference(slotId, numberList);
}

int32_t CsTest::KickOutFromConference(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test KickOutFromConference entry." << std::endl;
    std::vector<std::string> numberList;
    std::cout << "please enter the kick out phone number:";
    std::string phoneNum;
    std::cin >> phoneNum;
    numberList.push_back(phoneNum);
    std::cout << "please enter the slotId:";
    int32_t slotId = 0;
    return telephonyService->KickOutFromConference(slotId, numberList);
}

int32_t CsTest::HangUpAllConnection(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test HangUpAllConnection entry." << std::endl;
    return telephonyService->HangUpAllConnection();
}

int32_t CsTest::UpdateImsCallMode(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test UpdateImsCallMode entry." << std::endl;
    CellularCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        std::cout << "UpdateImsCallMode return, memset_s failed. \n";
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    std::cout << "please enter the need update phone number:";
    std::cin >> callInfo.phoneNum;
    callInfo.callType = CallType::TYPE_IMS;
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    callInfo.slotId = slotId;
    std::cout << "please enter the need update index:";
    std::cin >> callInfo.index;
    std::cout << "please enter the mode:";
    int32_t mode = 0;
    std::cin >> mode;
    return telephonyService->UpdateImsCallMode(callInfo, static_cast<ImsCallMode>(mode));
}

int32_t CsTest::RegisterCallBack(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test RegisterCallBack entry.\n";
    return CELLULAR_CALL_SUCCESS;
}

int32_t CsTest::UnRegisterCallBack(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test UnRegisterCallBack entry.\n";
    return telephonyService->UnRegisterCallManagerCallBack();
}

int32_t CsTest::StartDtmf(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test StartDtmf entry." << std::endl;
    CellularCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        std::cout << "CellularCallService return, memset_s failed. \n";
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    std::cout << "please enter the phone number:";
    std::cin >> callInfo.phoneNum;
    callInfo.videoState = 1;
    std::cout << "please enter the call type(0:CS  1:IMS):";
    int32_t callType = 0;
    std::cin >> callType;
    callInfo.callType = static_cast<CallType>(callType);
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    callInfo.slotId = slotId;
    std::cout << "please enter the index:";
    std::cin >> callInfo.index;
    char cDtmfCode;
    std::cout << "please enter the Dtmf code:";
    std::cin >> cDtmfCode;
    return telephonyService->StartDtmf(cDtmfCode, callInfo);
}

int32_t CsTest::StopDtmf(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test StopDtmf entry." << std::endl;
    CellularCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        std::cout << "CellularCallService return, memset_s failed. \n";
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    std::cout << "please enter the phone number:";
    std::cin >> callInfo.phoneNum;
    callInfo.videoState = 1;
    std::cout << "please enter the call type(0:CS  1:IMS):";
    int32_t callType = 0;
    std::cin >> callType;
    callInfo.callType = static_cast<CallType>(callType);
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    callInfo.slotId = slotId;
    std::cout << "please enter the index:";
    std::cin >> callInfo.index;
    return telephonyService->StopDtmf(callInfo);
}

int32_t CsTest::SendDtmf(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test SendDtmf entry." << std::endl;
    CellularCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        std::cout << "CellularCallService return, memset_s failed. \n";
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    std::cout << "please enter the phone number:";
    std::cin >> callInfo.phoneNum;
    callInfo.videoState = 1;
    std::cout << "please enter the call type(0:CS  1:IMS):";
    int32_t callType = 0;
    std::cin >> callType;
    callInfo.callType = static_cast<CallType>(callType);
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    callInfo.slotId = slotId;
    std::cout << "please enter the index:";
    std::cin >> callInfo.index;
    char cDtmfCode;
    std::cout << "please enter the Dtmf code:";
    std::cin >> cDtmfCode;
    return telephonyService->SendDtmf(cDtmfCode, callInfo);
}

int32_t CsTest::StartRtt(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test StartRtt entry." << std::endl;
    std::string msg;
    std::cout << "please enter the send msg:";
    std::cin >> msg;
    int32_t slotId;
    std::cout << "please enter the slotId:";
    std::cin >> slotId;
    return telephonyService->StartRtt(slotId, msg);
}

int32_t CsTest::StopRtt(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test StopRtt entry." << std::endl;
    int32_t slotId;
    std::cout << "please enter the slotId:";
    std::cin >> slotId;
    return telephonyService->StopRtt(slotId);
}

int32_t CsTest::SetCallTransferInfo(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test SetCallTransferInfoInfo entry.\n";
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    CallTransferInfo callTransferInfo;
    std::cout << "please enter the transfer type(0 - 5):";
    int32_t type = 0;
    std::cin >> type;
    callTransferInfo.type = static_cast<CallTransferType>(type);
    std::cout << "please enter the transfer setting type(0 - 4):";
    int32_t settingType = 0;
    std::cin >> settingType;
    callTransferInfo.settingType = static_cast<CallTransferSettingType>(settingType);
    std::cout << "please enter the phone number:";
    std::cin >> callTransferInfo.transferNum;
    return telephonyService->SetCallTransferInfo(slotId, callTransferInfo);
}

int32_t CsTest::GetCallTransferInfo(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test GetCallTransferInfo entry.\n";
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    std::cout << "please enter the transfer type(0 - 5):";
    int32_t type = 0;
    std::cin >> type;
    return telephonyService->GetCallTransferInfo(slotId, static_cast<CallTransferType>(type));
}

int32_t CsTest::SetCallWaiting(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test SetCallWaiting entry.\n";
    bool activate = true;
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    return telephonyService->SetCallWaiting(slotId, activate);
}

int32_t CsTest::GetCallWaiting(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test GetCallWaiting entry.\n";
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    return telephonyService->GetCallWaiting(slotId);
}

int32_t CsTest::SetCallRestriction(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test SetCallRestriction entry.\n";
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    CallRestrictionInfo info;
    std::cout << "please enter the fac(0 - 4):";
    int32_t fac = 0;
    std::cin >> fac;
    info.fac = static_cast<CallRestrictionType>(fac);
    std::cout << "please enter the phone password:";
    std::cin >> info.password;
    std::cout << "please enter the mode:";
    int32_t mode = 0;
    std::cin >> mode;
    info.mode = static_cast<CallRestrictionMode>(mode);
    return telephonyService->SetCallRestriction(slotId, info);
}

int32_t CsTest::GetCallRestriction(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test GetCallRestriction entry.\n";
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    std::cout << "please enter the fac(0 - 4):";
    int32_t facType = 0;
    std::cin >> facType;
    return telephonyService->GetCallRestriction(slotId, static_cast<CallRestrictionType>(facType));
}

int32_t CsTest::SetMute(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test SetMute entry.\n";
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    std::cout << "please enter the mute(0 - 1):";
    int32_t mute = -1;
    std::cin >> mute;
    return telephonyService->SetMute(slotId, mute);
}

int32_t CsTest::GetMute(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test GetMute entry.\n";
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    return telephonyService->GetMute(slotId);
}

void CsTest::JudgeIsEmergencyPhoneNumber()
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);

    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    bool enabled = false;
    int32_t successCode = 1;
    if (HasSimCard(SIM1_SLOTID)) {
        telephonyService->IsEmergencyPhoneNumber(SIM1_SLOTID, "499", enabled);
        EXPECT_EQ(enabled, successCode);
        telephonyService->IsEmergencyPhoneNumber(SIM1_SLOTID, "443", enabled);
        EXPECT_EQ(enabled, successCode);
        telephonyService->IsEmergencyPhoneNumber(SIM1_SLOTID, "356", enabled);
        EXPECT_EQ(enabled, successCode);
        telephonyService->IsEmergencyPhoneNumber(SIM1_SLOTID, "975", enabled);
        EXPECT_EQ(enabled, successCode);
        telephonyService->IsEmergencyPhoneNumber(SIM1_SLOTID, "783", enabled);
        EXPECT_EQ(enabled, successCode);
        telephonyService->IsEmergencyPhoneNumber(SIM1_SLOTID, "350", enabled);
        EXPECT_EQ(enabled, successCode);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        telephonyService->IsEmergencyPhoneNumber(SIM2_SLOTID, "499", enabled);
        EXPECT_EQ(enabled, successCode);
        telephonyService->IsEmergencyPhoneNumber(SIM2_SLOTID, "443", enabled);
        EXPECT_EQ(enabled, successCode);
        telephonyService->IsEmergencyPhoneNumber(SIM2_SLOTID, "356", enabled);
        EXPECT_EQ(enabled, successCode);
        telephonyService->IsEmergencyPhoneNumber(SIM2_SLOTID, "975", enabled);
        EXPECT_EQ(enabled, successCode);
        telephonyService->IsEmergencyPhoneNumber(SIM2_SLOTID, "783", enabled);
        EXPECT_EQ(enabled, successCode);
        telephonyService->IsEmergencyPhoneNumber(SIM2_SLOTID, "350", enabled);
        EXPECT_EQ(enabled, successCode);
    }
}

int32_t CsTest::InputNumForInterface(const sptr<CellularCallInterface> &telephonyService) const
{
    int32_t ret = -1;
    bool loopFlag = true;
    int32_t InputNum = 0;
    int32_t returnNum = 1000; // end loop variable value
    while (loopFlag) {
        std::cout << "\n**********Unit Test Start**********\n"
                     "Usage: please input a cmd num:\n"
                     "1:Dial\n2:HangUp\n3:Reject\n4:Answer\n5:HoldCall\n6:UnHoldCall\n7:SwitchCall\n"
                     "8:IsEmergencyPhoneNumber\n9:CombineConference\n10:SeparateConference\n11:InviteToConference\n"
                     "12:KickOutFromConference\n13:HangUpAllConnection\n14:UpdateImsCallMode\n"
                     "15:RegisterCallBack\n16:UnRegisterCallBack\n100:StartDtmf\n101:StopDtmf\n102:SendDtmf\n"
                     "103:StartRtt\n104:StopRtt\n200:SetCallTransferInfo\n201:GetCallTransferInfo\n"
                     "202:SetCallWaiting\n203:GetCallWaiting\n"
                     "204:SetCallRestriction\n205:GetCallRestriction\n311:SetMute\n312:GetMute\n"
                     "1000:Exit\n"
                     "***********************************\n"
                     "Your choice: ";
        std::cin >> InputNum;
        if (InputNum == returnNum) {
            std::cout << "exiting...\n";
            break;
        }
        auto itFunc = requestFuncMap_.find(InputNum);
        if (itFunc != requestFuncMap_.end()) {
            auto requestFunc = itFunc->second;
            if (requestFunc != nullptr) {
                ret = (this->*requestFunc)(telephonyService);
            }
        }
        if (ret != CELLULAR_CALL_SUCCESS) {
            std::cout << "this execution did not return a successful value, please check.\n";
            return ret;
        }
    }
    return CELLULAR_CALL_SUCCESS;
}

/**
 * @tc.number   cellular_call_cs_test_001
 * @tc.name     Test the corresponding functions by entering commands, such as 1 -- Dial, 2 -- Hangup, 3 -- Reject, etc
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_cs_test_001, Function | MediumTest | Level0)
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    std::cout << "HWTEST_F cellular_call_cs_test_001";
}

/**
 * @tc.number   cellular_call_cs_test_002
 * @tc.name     Test the corresponding functions by entering commands, such as 1 -- Dial, 2 -- Hangup, 3 -- Reject, etc
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_cs_test_002, Function | MediumTest | Level1)
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    std::cout << "HWTEST_F cellular_call_cs_test_002";
}

/**
 * @tc.number   Telephony_CallManager_SetEmergencyCallList_0101
 * @tc.name     SetEmergencyCallList "443,356,789",test SetEmergencyCallList(),return success
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_SetEmergencyCallList_0101, Function | MediumTest | Level3)
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);

    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    std::vector<EmergencyCall> eccVec;
    EmergencyCall temp0 = { "499", "460", EccType::TYPE_CATEGORY, SimpresentType::TYPE_NO_CARD,
        AbnormalServiceType::TYPE_ALL };
    EmergencyCall temp1 = { "443", "460", EccType::TYPE_POLICE, SimpresentType::TYPE_NO_CARD,
        AbnormalServiceType::TYPE_ALL };
    EmergencyCall temp2 = { "356", "460", EccType::TYPE_AMBULANCE, SimpresentType::TYPE_NO_CARD,
        AbnormalServiceType::TYPE_ALL };
    EmergencyCall temp3 = { "783", "460", EccType::TYPE_FIRE, SimpresentType::TYPE_NO_CARD,
        AbnormalServiceType::TYPE_ALL };
    EmergencyCall temp4 = { "975", "460", EccType::TYPE_SEA, SimpresentType::TYPE_HAS_CARD,
        AbnormalServiceType::TYPE_ONLY_CS };
    EmergencyCall temp5 = { "350", "460", EccType::TYPE_MOUNTAIN, SimpresentType::TYPE_HAS_CARD,
        AbnormalServiceType::TYPE_ALL };
    eccVec.push_back(temp0);
    eccVec.push_back(temp1);
    eccVec.push_back(temp2);
    eccVec.push_back(temp3);
    eccVec.push_back(temp4);
    eccVec.push_back(temp5);
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(telephonyService->SetEmergencyCallList(SIM1_SLOTID, eccVec), 0);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(telephonyService->SetEmergencyCallList(SIM2_SLOTID, eccVec), 0);
    }
    JudgeIsEmergencyPhoneNumber();
}

HWTEST_F(CsTest, cellular_call_IsOperatorConfigEmergencyCallList_0001, Function | MediumTest | Level3)
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);

    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    OperatorConfig opc;
    if (HasSimCard(SIM1_SLOTID)) {
        DelayedRefSingleton<CoreServiceClient>::GetInstance().GetOperatorConfigs(SIM1_SLOTID, opc);
        if (opc.stringArrayValue.find(KEY_EMERGENCY_CALL_STRING_ARRAY) != opc.stringArrayValue.end()) {
            for (auto number : opc.stringArrayValue[KEY_EMERGENCY_CALL_STRING_ARRAY]) {
                bool enabled = false;
                int32_t errorCode = telephonyService->IsEmergencyPhoneNumber(SIM1_SLOTID, number, enabled);
                EXPECT_EQ(TELEPHONY_SUCCESS, errorCode);
            }
        }
    }
    if (HasSimCard(SIM2_SLOTID)) {
        DelayedRefSingleton<CoreServiceClient>::GetInstance().GetOperatorConfigs(SIM2_SLOTID, opc);
        if (opc.stringArrayValue.find(KEY_EMERGENCY_CALL_STRING_ARRAY) != opc.stringArrayValue.end()) {
            for (auto number : opc.stringArrayValue[KEY_EMERGENCY_CALL_STRING_ARRAY]) {
                bool enabled = false;
                int32_t errorCode = telephonyService->IsEmergencyPhoneNumber(SIM2_SLOTID, number, enabled);
                EXPECT_EQ(TELEPHONY_SUCCESS, errorCode);
            }
        }
    }
}

/**
 * @tc.number   cellular_call_DialCall_0001
 * @tc.name     Test for SetClip function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_DialCall_0001, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM1_SLOTID, "*30#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM2_SLOTID, "*30#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0002
 * @tc.name     Test for SetClip function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_DialCall_0002, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM1_SLOTID, "#30#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM2_SLOTID, "#30#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0003
 * @tc.name     Test for getClip function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_DialCall_0003, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM1_SLOTID, "*#30#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM2_SLOTID, "*#30#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0004
 * @tc.name     Test for SetClir function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_DialCall_0004, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM1_SLOTID, "*31#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM2_SLOTID, "*31#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0005
 * @tc.name     Test for SetClir function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_DialCall_0005, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM1_SLOTID, "#31#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM2_SLOTID, "#31#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0006
 * @tc.name     Test for GetClir function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_DialCall_0006, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM1_SLOTID, "*#31#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM2_SLOTID, "*#31#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0007
 * @tc.name     Test for SetCallTransfer function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_DialCall_0007, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM1_SLOTID, "*21#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM2_SLOTID, "*21#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0008
 * @tc.name     Test for SetCallTransfer function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_DialCall_0008, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM1_SLOTID, "#21#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM2_SLOTID, "#21#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0009
 * @tc.name     Test for GetCallTransfer function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_DialCall_0009, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM1_SLOTID, "*#21#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM2_SLOTID, "*#21#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0010
 * @tc.name     Test for SetCallRestriction function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_DialCall_0010, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM1_SLOTID, "*33#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM2_SLOTID, "*33#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0011
 * @tc.name     Test for SetCallRestriction function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_DialCall_0011, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM1_SLOTID, "#33#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM2_SLOTID, "#33#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0012
 * @tc.name     Test for GetCallRestriction function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_DialCall_0012, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM1_SLOTID, "*#33#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM2_SLOTID, "*#33#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0013
 * @tc.name     Test for SetCallWaiting function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_DialCall_0013, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM1_SLOTID, "*43#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM2_SLOTID, "*43#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0014
 * @tc.name     Test for SetCallWaiting function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_DialCall_0014, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM1_SLOTID, "#43#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM2_SLOTID, "#43#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0015
 * @tc.name     Test for GetCallWaiting function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_DialCall_0015, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM1_SLOTID, "*#43#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM2_SLOTID, "*#43#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0016
 * @tc.name     Test for AlterPinPassword function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_DialCall_0016, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM1_SLOTID, "*04#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM2_SLOTID, "*04#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0017
 * @tc.name     Test for UnlockPuk function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_DialCall_0017, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM1_SLOTID, "*05#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM2_SLOTID, "*05#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0018
 * @tc.name     Test for AlterPin2Password function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_DialCall_0018, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM1_SLOTID, "*042#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM2_SLOTID, "*042#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0019
 * @tc.name     Test for UnlockPuk2 function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_DialCall_0019, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM1_SLOTID, "*052#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM2_SLOTID, "*052#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0020
 * @tc.name     Test for SendUssd function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_DialCall_0020, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM1_SLOTID, "*4444#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = TestDialCallByCs(SIM2_SLOTID, "*4444#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0021
 * @tc.name     Test for dial function with invalid slot by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_DialCall_0021, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = TestDialCallByCs(INVALID_SLOTID, PHONE_NUMBER);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = TestDialCallByCs(INVALID_SLOTID, PHONE_NUMBER);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
}

/**
 * @tc.number   cellular_call_HangUpCall_0001
 * @tc.name     Test for HangUp function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_HangUpCall_0001, Function | MediumTest | Level2)
{
    AccessToken token;
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->HangUp(callInfo, CallSupplementType::TYPE_DEFAULT);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
        ret = telephonyService->HangUp(callInfo, CallSupplementType::TYPE_HANG_UP_ACTIVE);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM2_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->HangUp(callInfo, CallSupplementType::TYPE_DEFAULT);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
        ret = telephonyService->HangUp(callInfo, CallSupplementType::TYPE_HANG_UP_ACTIVE);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_HangUpCall_0002
 * @tc.name     Test for HangUp function with invalid slot by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_HangUpCall_0002, Function | MediumTest | Level2)
{
    AccessToken token;
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->HangUp(callInfo, CallSupplementType::TYPE_DEFAULT);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->HangUp(callInfo, CallSupplementType::TYPE_DEFAULT);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
}

/**
 * @tc.number   cellular_call_AnswerCall_0001
 * @tc.name     Test for answer function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_AnswerCall_0001, Function | MediumTest | Level2)
{
    AccessToken token;
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->Answer(callInfo);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM2_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->Answer(callInfo);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
    }
}

/**
 * @tc.number   cellular_call_AnswerCall_0002
 * @tc.name     Test for answer function with invalid slot by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_AnswerCall_0002, Function | MediumTest | Level2)
{
    AccessToken token;
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->Answer(callInfo);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->Answer(callInfo);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
}

/**
 * @tc.number   cellular_call_RejectCall_0001
 * @tc.name     Test for reject function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_RejectCall_0001, Function | MediumTest | Level2)
{
    AccessToken token;
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->Reject(callInfo);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM2_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->Reject(callInfo);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
    }
}

/**
 * @tc.number   cellular_call_RejectCall_0002
 * @tc.name     Test for reject function with invalid slot by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_RejectCall_0002, Function | MediumTest | Level2)
{
    AccessToken token;
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->Reject(callInfo);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->Reject(callInfo);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
}

/**
 * @tc.number   cellular_call_HoldCall_0001
 * @tc.name     Test for hold call function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_HoldCall_0001, Function | MediumTest | Level2)
{
    AccessToken token;
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->HoldCall(callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM2_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->HoldCall(callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_HoldCall_0002
 * @tc.name     Test for hold function with invalid slot by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_HoldCall_0002, Function | MediumTest | Level2)
{
    AccessToken token;
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->HoldCall(callInfo);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->HoldCall(callInfo);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
}

/**
 * @tc.number   cellular_call_UnHoldCall_0001
 * @tc.name     Test for unhold call function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_UnHoldCall_0001, Function | MediumTest | Level2)
{
    AccessToken token;
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->UnHoldCall(callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM2_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->UnHoldCall(callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_UnHoldCall_0002
 * @tc.name     Test for unhold function with invalid slot by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_UnHoldCall_0002, Function | MediumTest | Level2)
{
    AccessToken token;
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->UnHoldCall(callInfo);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->UnHoldCall(callInfo);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
}

/**
 * @tc.number   cellular_call_SwitchCall_0001
 * @tc.name     Test for switch call function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_SwitchCall_0001, Function | MediumTest | Level2)
{
    AccessToken token;
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->SwitchCall(callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM2_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->SwitchCall(callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_SwitchCall_0002
 * @tc.name     Test for switch function with invalid slot by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_SwitchCall_0002, Function | MediumTest | Level2)
{
    AccessToken token;
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->SwitchCall(callInfo);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->SwitchCall(callInfo);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
}

/**
 * @tc.number   cellular_call_CombineConference_0001
 * @tc.name     Test for combineConference function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_CombineConference_0001, Function | MediumTest | Level2)
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->CombineConference(callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM2_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->CombineConference(callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_CombineConference_0002
 * @tc.name     Test for combineConference function with invalid slot by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_CombineConference_0002, Function | MediumTest | Level2)
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->CombineConference(callInfo);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->CombineConference(callInfo);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
}

/**
 * @tc.number   cellular_call_SeparateConference_0001
 * @tc.name     Test for separateConference function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_SeparateConference_0001, Function | MediumTest | Level2)
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->SeparateConference(callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM2_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->SeparateConference(callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_SeparateConference_0002
 * @tc.name     Test for separateConference function with invalid slot by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_SeparateConference_0002, Function | MediumTest | Level2)
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->SeparateConference(callInfo);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->SeparateConference(callInfo);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
}

/**
 * @tc.number   cellular_call_HangUpAllConnection_0001
 * @tc.name     Test for hangup all connection function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_HangUpAllConnection_0001, Function | MediumTest | Level2)
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = telephonyService->HangUpAllConnection();
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = telephonyService->HangUpAllConnection();
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_StartDtmf_0001
 * @tc.name     Test for startDtmf function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_StartDtmf_0001, Function | MediumTest | Level2)
{
    AccessToken token;
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        char code = '1';
        ret = telephonyService->StartDtmf(code, callInfo);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM2_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        char code = '1';
        ret = telephonyService->StartDtmf(code, callInfo);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
    }
}

/**
 * @tc.number   cellular_call_StartDtmf_0002
 * @tc.name     Test for startDtmf function with invalid slot by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_StartDtmf_0002, Function | MediumTest | Level2)
{
    AccessToken token;
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        char code = '1';
        ret = telephonyService->StartDtmf(code, callInfo);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        char code = '1';
        ret = telephonyService->StartDtmf(code, callInfo);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
}

/**
 * @tc.number   cellular_call_StopDtmf_0001
 * @tc.name     Test for stopDtmf function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_StopDtmf_0001, Function | MediumTest | Level2)
{
    AccessToken token;
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->StopDtmf(callInfo);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM2_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->StopDtmf(callInfo);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
    }
}

/**
 * @tc.number   cellular_call_StopDtmf_0002
 * @tc.name     Test for stopDtmf function with invalid slot by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_StopDtmf_0002, Function | MediumTest | Level2)
{
    AccessToken token;
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->StopDtmf(callInfo);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->StopDtmf(callInfo);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
}

/**
 * @tc.number   cellular_call_SendDtmf_0001
 * @tc.name     Test for sendDtmf function by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_SendDtmf_0001, Function | MediumTest | Level2)
{
    AccessToken token;
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        char code = '1';
        ret = telephonyService->SendDtmf(code, callInfo);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM2_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        char code = '1';
        ret = telephonyService->SendDtmf(code, callInfo);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
    }
}

/**
 * @tc.number   cellular_call_SendDtmf_0002
 * @tc.name     Test for sendDtmf function with invalid slot by cs
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_SendDtmf_0002, Function | MediumTest | Level2)
{
    AccessToken token;
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        char code = '1';
        ret = telephonyService->SendDtmf(code, callInfo);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        char code = '1';
        ret = telephonyService->SendDtmf(code, callInfo);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
}

/**
 * @tc.number   cellular_call_GetDomainPreferenceMode_0001
 * @tc.name     Test for GetDomainPreferenceMode function by invalid slotId
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_GetDomainPreferenceMode_0001, Function | MediumTest | Level3)
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = telephonyService->GetDomainPreferenceMode(INVALID_SLOTID);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = telephonyService->GetDomainPreferenceMode(INVALID_SLOTID);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
}

/**
 * @tc.number   cellular_call_GetDomainPreferenceMode_0002
 * @tc.name     Test for GetDomainPreferenceMode function by valid slotId
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_GetDomainPreferenceMode_0002, Function | MediumTest | Level3)
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = telephonyService->GetDomainPreferenceMode(SIM1_SLOTID);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = telephonyService->GetDomainPreferenceMode(SIM2_SLOTID);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_GetMute_0001
 * @tc.name     Test for GetMute function by invalid slotId
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_GetMute_0001, Function | MediumTest | Level3)
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = telephonyService->GetMute(INVALID_SLOTID);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = telephonyService->GetMute(INVALID_SLOTID);
        EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
    }
}

/**
 * @tc.number   cellular_call_GetMute_0002
 * @tc.name     Test for GetMute function by valid slotId
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_GetMute_0002, Function | MediumTest | Level3)
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID)) {
        int32_t ret = telephonyService->GetMute(SIM1_SLOTID);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        int32_t ret = telephonyService->GetMute(SIM2_SLOTID);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_CsControl_0001
 * @tc.name     Test for CsControl
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_CsControl_0001, Function | MediumTest | Level3)
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    auto csControl = std::make_shared<CSControl>();
    CellularCallInfo cellularCallInfo;
    EXPECT_EQ(InitCellularCallInfo(INVALID_SLOTID, PHONE_NUMBER, cellularCallInfo), TELEPHONY_SUCCESS);
    EXPECT_EQ(csControl->Dial(cellularCallInfo), CALL_ERR_GET_RADIO_STATE_FAILED);
    EXPECT_EQ(InitCellularCallInfo(INVALID_SLOTID, "", cellularCallInfo), TELEPHONY_SUCCESS);
    EXPECT_EQ(csControl->Dial(cellularCallInfo), CALL_ERR_PHONE_NUMBER_EMPTY);

    for (int32_t slotId = 0; slotId < SIM_SLOT_COUNT; slotId++) {
        if (!HasSimCard(slotId)) {
            continue;
        }
        EXPECT_EQ(InitCellularCallInfo(slotId, "*30#", cellularCallInfo), TELEPHONY_SUCCESS);
        EXPECT_EQ(csControl->DialCdma(cellularCallInfo), CALL_ERR_RESOURCE_UNAVAILABLE);
        EXPECT_EQ(csControl->DialGsm(cellularCallInfo), CALL_ERR_RESOURCE_UNAVAILABLE);
        EXPECT_EQ(InitCellularCallInfo(slotId, "#30#", cellularCallInfo), TELEPHONY_SUCCESS);
        EXPECT_EQ(csControl->DialGsm(cellularCallInfo), CALL_ERR_RESOURCE_UNAVAILABLE);
        EXPECT_EQ(InitCellularCallInfo(slotId, PHONE_NUMBER, cellularCallInfo), TELEPHONY_SUCCESS);
        EXPECT_EQ(csControl->DialCdma(cellularCallInfo), CALL_ERR_RESOURCE_UNAVAILABLE);
        EXPECT_EQ(csControl->Dial(cellularCallInfo), CALL_ERR_GET_RADIO_STATE_FAILED);
        ASSERT_FALSE(csControl->CalculateInternationalRoaming(slotId));
        CallInfoList callList;
        callList.callSize = 0;
        EXPECT_EQ(csControl->ReportCallsData(slotId, callList), TELEPHONY_ERROR);
        CallInfo callInfo;
        callList.callSize = 1;
        callInfo.number = PHONE_NUMBER;
        callInfo.index = 1;
        callInfo.state = static_cast<int32_t>(TelCallState::CALL_STATUS_INCOMING);
        callList.calls.push_back(callInfo);
        EXPECT_EQ(csControl->ReportCallsData(slotId, callList), TELEPHONY_SUCCESS);
        callList.callSize = 2;
        callInfo.state = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
        callInfo.number = PHONE_NUMBER_SECOND;
        callInfo.index = 2;
        callList.calls.push_back(callInfo);
        callList.callSize = 3;
        callInfo.state = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
        callInfo.number = PHONE_NUMBER_THIRD;
        callInfo.index = 3;
        callList.calls.push_back(callInfo);
        EXPECT_EQ(csControl->ReportCallsData(slotId, callList), TELEPHONY_SUCCESS);
        EXPECT_EQ(csControl->DialCdma(cellularCallInfo), TELEPHONY_SUCCESS);
        EXPECT_EQ(csControl->DialGsm(cellularCallInfo), CALL_ERR_RESOURCE_UNAVAILABLE);
        EXPECT_EQ(csControl->Answer(cellularCallInfo), CALL_ERR_RESOURCE_UNAVAILABLE);
        EXPECT_EQ(InitCellularCallInfo(slotId, PHONE_NUMBER_SECOND, cellularCallInfo), TELEPHONY_SUCCESS);
        EXPECT_EQ(csControl->Answer(cellularCallInfo), CALL_ERR_CALL_STATE);
        EXPECT_EQ(InitCellularCallInfo(slotId, PHONE_NUMBER_THIRD, cellularCallInfo), TELEPHONY_SUCCESS);
        EXPECT_EQ(csControl->Answer(cellularCallInfo), CALL_ERR_CALL_STATE);
        EXPECT_EQ(csControl->Reject(cellularCallInfo), CALL_ERR_CALL_STATE);
        EXPECT_EQ(InitCellularCallInfo(slotId, PHONE_NUMBER, cellularCallInfo), TELEPHONY_SUCCESS);
        EXPECT_EQ(csControl->Reject(cellularCallInfo), CALL_ERR_RESOURCE_UNAVAILABLE);
        EXPECT_EQ(csControl->HoldCall(slotId), CALL_ERR_CALL_STATE);
        EXPECT_EQ(csControl->UnHoldCall(slotId), CALL_ERR_CALL_STATE);
        EXPECT_EQ(csControl->SwitchCall(slotId), CALL_ERR_CALL_STATE);
        EXPECT_EQ(csControl->SeparateConference(slotId, PHONE_NUMBER, 1), CALL_ERR_RESOURCE_UNAVAILABLE);
        EXPECT_EQ(csControl->SeparateConference(slotId, "", 1), CALL_ERR_RESOURCE_UNAVAILABLE);
        EXPECT_EQ(csControl->HangUp(cellularCallInfo, CallSupplementType::TYPE_DEFAULT), CALL_ERR_RESOURCE_UNAVAILABLE);
        EXPECT_EQ(csControl->HangUp(cellularCallInfo, CallSupplementType::TYPE_HANG_UP_ACTIVE),
            CALL_ERR_RESOURCE_UNAVAILABLE);
        EXPECT_EQ(
            csControl->HangUp(cellularCallInfo, CallSupplementType::TYPE_HANG_UP_ALL), CALL_ERR_RESOURCE_UNAVAILABLE);
        EXPECT_EQ(csControl->HangUp(cellularCallInfo, static_cast<CallSupplementType>(INVALID_HANG_UP_TYPE)),
            TELEPHONY_ERR_ARGUMENT_INVALID);
        callList.callSize = 0;
        EXPECT_EQ(csControl->ReportCallsData(slotId, callList), TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_CellularCallConnectionCS_0001
 * @tc.name     Test for CellularCallConnectionCS
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_CellularCallConnectionCS_0001, Function | MediumTest | Level3)
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    for (int32_t slotId = 0; slotId < SIM_SLOT_COUNT; slotId++) {
        if (!HasSimCard(slotId)) {
            continue;
        }
        CellularCallConnectionCS csConnection;
        EXPECT_EQ(csConnection.SendDtmfRequest(slotId, '1', 1), CALL_ERR_RESOURCE_UNAVAILABLE);
        EXPECT_EQ(csConnection.StartDtmfRequest(slotId, '1', 1), CALL_ERR_RESOURCE_UNAVAILABLE);
        EXPECT_EQ(csConnection.StopDtmfRequest(slotId, 1), CALL_ERR_RESOURCE_UNAVAILABLE);
        EXPECT_EQ(csConnection.GetCsCallsDataRequest(slotId, 1), CALL_ERR_RESOURCE_UNAVAILABLE);
        EXPECT_EQ(csConnection.GetCallFailReasonRequest(slotId), CALL_ERR_RESOURCE_UNAVAILABLE);
        MMICodeUtils utils;
        ASSERT_FALSE(utils.IsNeedExecuteMmi(""));
        ASSERT_FALSE(utils.ExecuteMmiCode(slotId));
    }
}

/**
 * @tc.number   cellular_call_CellularCallRegister_0001
 * @tc.name     Test for CellularCallRegister
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_CellularCallRegister_0001, Function | MediumTest | Level3)
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    auto callRegister = DelayedSingleton<CellularCallRegister>::GetInstance();
    ASSERT_TRUE(callRegister != nullptr);
    CallReportInfo callRepotInfo;
    callRepotInfo.callType = CallType::TYPE_CS;
    callRepotInfo.accountId = INVALID_SLOTID;
    callRepotInfo.state = TelCallState::CALL_STATUS_INCOMING;
    callRepotInfo.callMode = VideoStateType::TYPE_VOICE;
    CallsReportInfo calls;
    calls.slotId = INVALID_SLOTID;
    calls.callVec.push_back(callRepotInfo);
    callRegister->ReportCallsInfo(calls);
    callRegister->ReportSingleCallInfo(callRepotInfo, TelCallState::CALL_STATUS_INCOMING);
    CellularCallEventInfo callEvent;
    callRegister->ReportEventResultInfo(callEvent);
    CallWaitResponse waitResponse;
    callRegister->ReportGetWaitingResult(waitResponse);
    callRegister->ReportSetWaitingResult(RESULT);
    CallRestrictionResponse restrictionResponse;
    callRegister->ReportGetRestrictionResult(restrictionResponse);
    callRegister->ReportSetRestrictionResult(RESULT);
    CallTransferResponse transferResponse;
    callRegister->ReportGetTransferResult(transferResponse);
    callRegister->ReportSetTransferResult(RESULT);
    ClipResponse clipResponse;
    callRegister->ReportGetClipResult(clipResponse);
    ClirResponse clirResponse;
    callRegister->ReportGetClirResult(clirResponse);
    callRegister->ReportSetClirResult(RESULT);
    callRegister->ReportCallRingBackResult(RESULT);
    DisconnectedDetails details;
    callRegister->ReportCallFailReason(details);
    MuteControlResponse muteResponse;
    callRegister->ReportSetMuteResult(muteResponse);
    callRegister->ReportGetMuteResult(muteResponse);
    callRegister->ReportInviteToConferenceResult(RESULT);
    callRegister->ReportGetCallDataResult(RESULT);
    callRegister->ReportStartDtmfResult(RESULT);
    callRegister->ReportStopDtmfResult(RESULT);
    callRegister->ReportStartRttResult(RESULT);
    callRegister->ReportStopRttResult(RESULT);
    callRegister->ReportSendUssdResult(RESULT);
    SetEccListResponse eccListResponse;
    callRegister->ReportSetEmergencyCallListResponse(eccListResponse);
    MmiCodeInfo mmiInfo;
    callRegister->ReportMmiCodeResult(mmiInfo);
    EXPECT_EQ(callRegister->RegisterCallManagerCallBack(nullptr), TELEPHONY_SUCCESS);
    EXPECT_EQ(callRegister->UnRegisterCallManagerCallBack(), TELEPHONY_SUCCESS);
    ASSERT_FALSE(callRegister->IsCallManagerCallBackRegistered());
}

/**
 * @tc.number   cellular_call_TestDump_0001
 * @tc.name    TestDump
 * @tc.desc     Function test
 */
HWTEST_F(CsTest, cellular_call_TestDump_0001, Function | MediumTest | Level3)
{
    std::vector<std::u16string> emptyArgs = {};
    std::vector<std::u16string> args = { u"test", u"test1" };
    EXPECT_EQ(DelayedSingleton<CellularCallService>::GetInstance()->Dump(-1, args), TELEPHONY_ERR_FAIL);
    EXPECT_EQ(DelayedSingleton<CellularCallService>::GetInstance()->Dump(0, emptyArgs), 0);
    EXPECT_EQ(DelayedSingleton<CellularCallService>::GetInstance()->Dump(0, args), 0);
}
} // namespace Telephony
} // namespace OHOS
