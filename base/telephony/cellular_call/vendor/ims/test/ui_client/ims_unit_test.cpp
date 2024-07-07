/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "ims_unit_test.h"

#include <iostream>

#include "ims_call_client.h"
#include "ims_core_service_client.h"
#include "ims_sms_client.h"
#include "ims_sms_types.h"
#include "ims_test_handler.h"
#include "iservice_registry.h"
#include "securec.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Telephony {
const int32_t IMS_TEST_ERROR = -1;
const int32_t IMS_TEST_SUCCESS = 0;
const int32_t IMS_TEST_MAX_PASSWORD = 6;

void ImsUnitTest::SetUpTestCase(void)
{
    // step 3: Set Up Test Case
}

void ImsUnitTest::TearDownTestCase(void)
{
    // step 3: Tear Down Test Case
}

void ImsUnitTest::SetUp(void)
{
    // step 3: input testcase setup step
    InitBasicFuncMap();
    InitConfigFuncMap();
    InitVideoCallFuncMap();
    InitSupplementFuncMap();
    InitSmsFuncMap();
    InitCoreServiceFuncMap();
}

void ImsUnitTest::InitBasicFuncMap(void)
{
    requestCallFuncMap_[ImsCallInterface::IMS_DIAL] = &ImsUnitTest::Dial;
    requestCallFuncMap_[ImsCallInterface::IMS_HANG_UP] = &ImsUnitTest::HangUp;
    requestCallFuncMap_[ImsCallInterface::IMS_REJECT_WITH_REASON] = &ImsUnitTest::RejectWithReason;
    requestCallFuncMap_[ImsCallInterface::IMS_ANSWER] = &ImsUnitTest::Answer;
    requestCallFuncMap_[ImsCallInterface::IMS_HOLD] = &ImsUnitTest::HoldCall;
    requestCallFuncMap_[ImsCallInterface::IMS_UN_HOLD] = &ImsUnitTest::UnHoldCall;
    requestCallFuncMap_[ImsCallInterface::IMS_SWITCH] = &ImsUnitTest::SwitchCall;
    requestCallFuncMap_[ImsCallInterface::IMS_COMBINE_CONFERENCE] = &ImsUnitTest::CombineConference;
    requestCallFuncMap_[ImsCallInterface::IMS_INVITE_TO_CONFERENCE] = &ImsUnitTest::InviteToConference;
    requestCallFuncMap_[ImsCallInterface::IMS_KICK_OUT_CONFERENCE] = &ImsUnitTest::KickOutFromConference;
    requestCallFuncMap_[ImsCallInterface::IMS_UPDATE_CALL_MEDIA_MODE] = &ImsUnitTest::UpdateImsCallMode;
    requestCallFuncMap_[ImsCallInterface::IMS_GET_CALL_DATA] = &ImsUnitTest::GetImsCallsDataRequest;
    requestCallFuncMap_[ImsCallInterface::IMS_GET_LAST_CALL_FAIL_REASON] = &ImsUnitTest::GetLastCallFailReason;
    requestCallFuncMap_[ImsCallInterface::IMS_START_DTMF] = &ImsUnitTest::StartDtmf;
    requestCallFuncMap_[ImsCallInterface::IMS_SEND_DTMF] = &ImsUnitTest::SendDtmf;
    requestCallFuncMap_[ImsCallInterface::IMS_STOP_DTMF] = &ImsUnitTest::StopDtmf;
    requestCallFuncMap_[ImsCallInterface::IMS_START_RTT] = &ImsUnitTest::StartRtt;
    requestCallFuncMap_[ImsCallInterface::IMS_STOP_RTT] = &ImsUnitTest::StopRtt;
}

void ImsUnitTest::InitConfigFuncMap(void)
{
    requestCallFuncMap_[ImsCallInterface::IMS_SET_DOMAIN_PREFERENCE_MODE] = &ImsUnitTest::SetDomainPreferenceMode;
    requestCallFuncMap_[ImsCallInterface::IMS_GET_DOMAIN_PREFERENCE_MODE] = &ImsUnitTest::GetDomainPreferenceMode;
    requestCallFuncMap_[ImsCallInterface::IMS_SET_SWITCH_STATUS] = &ImsUnitTest::SetImsSwitchStatus;
    requestCallFuncMap_[ImsCallInterface::IMS_GET_SWITCH_STATUS] = &ImsUnitTest::GetImsSwitchStatus;
    requestCallFuncMap_[ImsCallInterface::IMS_SET_IMS_CONFIG_STRING] = &ImsUnitTest::SetImsConfigString;
    requestCallFuncMap_[ImsCallInterface::IMS_SET_IMS_CONFIG_INT] = &ImsUnitTest::SetImsConfigInt;
    requestCallFuncMap_[ImsCallInterface::IMS_GET_IMS_CONFIG] = &ImsUnitTest::GetImsConfig;
    requestCallFuncMap_[ImsCallInterface::IMS_SET_IMS_FEATURE] = &ImsUnitTest::SetImsFeatureValue;
    requestCallFuncMap_[ImsCallInterface::IMS_GET_IMS_FEATURE] = &ImsUnitTest::GetImsFeatureValue;
    requestCallFuncMap_[ImsCallInterface::IMS_SET_MUTE] = &ImsUnitTest::SetMute;
    requestCallFuncMap_[ImsCallInterface::IMS_GET_MUTE] = &ImsUnitTest::GetMute;
}

void ImsUnitTest::InitVideoCallFuncMap(void)
{
    requestCallFuncMap_[ImsCallInterface::IMS_CTRL_CAMERA] = &ImsUnitTest::CtrlCamera;
    requestCallFuncMap_[ImsCallInterface::IMS_SET_PREVIEW_WINDOW] = &ImsUnitTest::SetPreviewWindow;
    requestCallFuncMap_[ImsCallInterface::IMS_SET_DISPLAY_WINDOW] = &ImsUnitTest::SetDisplayWindow;
    requestCallFuncMap_[ImsCallInterface::IMS_SET_CAMERA_ZOOM] = &ImsUnitTest::SetCameraZoom;
    requestCallFuncMap_[ImsCallInterface::IMS_SET_PAUSE_IMAGE] = &ImsUnitTest::SetPauseImage;
    requestCallFuncMap_[ImsCallInterface::IMS_SET_DEVICE_DIRECTION] = &ImsUnitTest::SetDeviceDirection;
}

void ImsUnitTest::InitSupplementFuncMap(void)
{
    requestCallFuncMap_[ImsCallInterface::IMS_GET_CLIP] = &ImsUnitTest::GetClip;
    requestCallFuncMap_[ImsCallInterface::IMS_SET_CLIR] = &ImsUnitTest::SetClir;
    requestCallFuncMap_[ImsCallInterface::IMS_GET_CLIR] = &ImsUnitTest::GetClir;
    requestCallFuncMap_[ImsCallInterface::IMS_SET_CALL_TRANSFER] = &ImsUnitTest::SetCallTransfer;
    requestCallFuncMap_[ImsCallInterface::IMS_GET_CALL_TRANSFER] = &ImsUnitTest::GetCallTransfer;
    requestCallFuncMap_[ImsCallInterface::IMS_SET_CALL_RESTRICTION] = &ImsUnitTest::SetCallRestriction;
    requestCallFuncMap_[ImsCallInterface::IMS_GET_CALL_RESTRICTION] = &ImsUnitTest::GetCallRestriction;
    requestCallFuncMap_[ImsCallInterface::IMS_SET_CALL_WAITING] = &ImsUnitTest::SetCallWaiting;
    requestCallFuncMap_[ImsCallInterface::IMS_GET_CALL_WAITING] = &ImsUnitTest::GetCallWaiting;
}

void ImsUnitTest::InitSmsFuncMap(void)
{
    requestSmsFuncMap_[ImsSmsInterface::IMS_SET_SMS_CONFIG] = &ImsUnitTest::SetSmsConfig;
    requestSmsFuncMap_[ImsSmsInterface::IMS_GET_SMS_CONFIG] = &ImsUnitTest::GetSmsConfig;
    requestSmsFuncMap_[ImsSmsInterface::IMS_SEND_MESSAGE] = &ImsUnitTest::SendMessage;
}

void ImsUnitTest::InitCoreServiceFuncMap(void)
{
    requestCoreServiceFuncMap_[ImsCoreServiceInterface::IMS_GET_REGISTRATION_STATUS] =
        &ImsUnitTest::GetImsRegistrationStatus;
}

void ImsUnitTest::TearDown(void)
{
    // step 3: input testcase teardown step
}

int32_t ImsUnitTest::Dial(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test Dial entry." << std::endl;
    ImsCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        std::cout << "Dial return, memset_s failed." << std::endl;
        return IMS_TEST_ERROR;
    }
    std::cout << "please enter the phone number:";
    std::cin >> callInfo.phoneNum;
    callInfo.videoState = 1;
    callInfo.slotId = 0;
    CLIRMode mode = CLIRMode::DEFAULT;
    imsCallPtr->Dial(callInfo, mode);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::HangUp(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test HangUp entry." << std::endl;
    ImsCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        std::cout << "HangUp return, memset_s failed." << std::endl;
        return IMS_TEST_ERROR;
    }
    std::cout << "please enter the phone number:";
    std::cin >> callInfo.phoneNum;
    callInfo.videoState = 1;
    callInfo.slotId = 0;
    std::cout << "please enter the connect index:";
    std::cin >> callInfo.index;
    imsCallPtr->HangUp(callInfo);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::RejectWithReason(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test RejectWithReason entry." << std::endl;
    ImsCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        std::cout << "RejectWithReason return, memset_s failed." << std::endl;
        return IMS_TEST_ERROR;
    }
    std::cout << "please enter the phone number:";
    std::cin >> callInfo.phoneNum;
    callInfo.videoState = 1;
    callInfo.slotId = 0;
    std::cout << "please enter the connect index:";
    std::cin >> callInfo.index;
    imsCallPtr->RejectWithReason(callInfo, ImsRejectReason::USER_DECLINE);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::Answer(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test Answer entry." << std::endl;
    ImsCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        std::cout << "Answer return, memset_s failed." << std::endl;
        return IMS_TEST_ERROR;
    }
    std::cout << "please enter the phone number:";
    std::cin >> callInfo.phoneNum;
    callInfo.videoState = 1;
    callInfo.slotId = 0;
    std::cout << "please enter the connect index:";
    std::cin >> callInfo.index;
    imsCallPtr->Answer(callInfo);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::HoldCall(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test HoldCall entry." << std::endl;
    int32_t slotId;
    int32_t callType;
    std::cout << "please enter the slot id:";
    std::cin >> slotId;
    std::cout << "please enter the callType(0->CS, 1->IMS, 2->OTT, 3->OTHER):";
    std::cin >> callType;
    imsCallPtr->HoldCall(slotId, callType);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::UnHoldCall(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test UnHoldCall entry." << std::endl;
    int32_t slotId;
    int32_t callType;
    std::cout << "please enter the slot id:";
    std::cin >> slotId;
    std::cout << "please enter the callType(0->CS, 1->IMS, 2->OTT, 3->OTHER):";
    std::cin >> callType;
    imsCallPtr->UnHoldCall(slotId, callType);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::SwitchCall(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test SwitchCall entry." << std::endl;
    int32_t slotId;
    int32_t callType;
    std::cout << "please enter the slot id:";
    std::cin >> slotId;
    std::cout << "please enter the callType(0->CS, 1->IMS, 2->OTT, 3->OTHER):";
    std::cin >> callType;
    imsCallPtr->SwitchCall(slotId, callType);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::CombineConference(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test CombineConference entry." << std::endl;
    std::cout << "please enter the slotId:";
    int32_t slotId = 0;
    imsCallPtr->CombineConference(slotId);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::InviteToConference(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test InviteToConference entry." << std::endl;
    std::vector<std::string> numberList;
    std::cout << "please enter the invite phone number:";
    std::string phoneNum;
    std::cin >> phoneNum;
    numberList.push_back(phoneNum);
    std::cout << "please enter the slotId:";
    int32_t slotId = 0;
    imsCallPtr->InviteToConference(slotId, numberList);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::KickOutFromConference(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test KickOutFromConference entry." << std::endl;
    std::vector<std::string> numberList;
    std::cout << "please enter the kick out phone number:";
    std::string phoneNum;
    std::cin >> phoneNum;
    numberList.push_back(phoneNum);
    std::cout << "please enter the slotId:";
    int32_t slotId = 0;
    imsCallPtr->KickOutFromConference(slotId, numberList);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::UpdateImsCallMode(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test UpdateImsCallMode entry." << std::endl;
    ImsCallInfo callInfo;
    if (memset_s(&callInfo, sizeof(callInfo), 0, sizeof(callInfo)) != EOK) {
        std::cout << "UpdateImsCallMode return, memset_s failed." << std::endl;
        return IMS_TEST_ERROR;
    }
    std::cout << "please enter the phone number:";
    std::cin >> callInfo.phoneNum;
    callInfo.videoState = 1;
    callInfo.slotId = 0;
    std::cout << "please enter the connect index:";
    std::cin >> callInfo.index;
    std::cout << "please enter the update call mode():";
    int32_t mode = 0;
    std::cin >> mode;
    imsCallPtr->UpdateImsCallMode(callInfo, static_cast<ImsCallMode>(mode));
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::GetImsCallsDataRequest(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "please enter the slot id:";
    int32_t slotId = 0;
    std::cin >> slotId;
    std::cout << "please enter the last calls data flag:";
    int64_t flag = 0;
    std::cin >> flag;
    imsCallPtr->GetImsCallsDataRequest(slotId, flag);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::GetLastCallFailReason(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "please enter the slot id:";
    int32_t slotId = 0;
    std::cin >> slotId;
    imsCallPtr->GetLastCallFailReason(slotId);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::StartDtmf(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test StartDtmf entry." << std::endl;
    std::cout << "please enter the dtmf code(0-9,#,*,A,D):";
    char dtmfCode;
    std::cin >> dtmfCode;
    std::cout << "please enter the index:";
    int32_t index = 0;
    std::cin >> index;
    std::cout << "please enter the slot id:";
    int32_t slotId = 0;
    std::cin >> slotId;
    imsCallPtr->StartDtmf(slotId, dtmfCode, index);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::SendDtmf(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test SendDtmf entry." << std::endl;
    std::cout << "please enter the dtmf code(0-9,#,*,A,D):";
    char dtmfCode;
    std::cin >> dtmfCode;
    std::cout << "please enter the index:";
    int32_t index = 0;
    std::cin >> index;
    std::cout << "please enter the slot id:";
    int32_t slotId = 0;
    std::cin >> slotId;
    imsCallPtr->SendDtmf(slotId, dtmfCode, index);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::StopDtmf(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test StopDtmf entry." << std::endl;
    std::cout << "please enter the index:";
    int32_t index = 0;
    std::cin >> index;
    std::cout << "please enter the slot id:";
    int32_t slotId = 0;
    std::cin >> slotId;
    imsCallPtr->StopDtmf(slotId, index);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::StartRtt(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test StartRtt entry." << std::endl;
    std::cout << "please enter the send msg:";
    std::string msg;
    std::cin >> msg;
    std::cout << "please enter the slot id:";
    int32_t slotId = 0;
    std::cin >> slotId;
    imsCallPtr->StartRtt(slotId, msg);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::StopRtt(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test StopRtt entry." << std::endl;
    std::cout << "please enter the slot id:";
    int32_t slotId = 0;
    std::cin >> slotId;
    imsCallPtr->StopRtt(slotId);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::SetDomainPreferenceMode(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test SetDomainPreferenceMode entry." << std::endl;
    std::cout << "please enter the need mode:(1 - 4)";
    int32_t mode;
    std::cin >> mode;
    std::cout << "please enter the slot id:";
    int32_t slotId = 0;
    std::cin >> slotId;
    imsCallPtr->SetDomainPreferenceMode(slotId, mode);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::GetDomainPreferenceMode(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test GetDomainPreferenceMode entry." << std::endl;
    std::cout << "please enter the slot id:";
    int32_t slotId = 0;
    std::cin >> slotId;
    imsCallPtr->GetDomainPreferenceMode(slotId);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::SetImsSwitchStatus(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test SetImsSwitchStatus entry." << std::endl;
    std::cout << "please enter the switch state:";
    bool active;
    std::cin >> active;
    std::cout << "please enter the slot id:";
    int32_t slotId = 0;
    std::cin >> slotId;
    imsCallPtr->SetImsSwitchStatus(slotId, active);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::GetImsSwitchStatus(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test GetImsSwitchStatus entry." << std::endl;
    std::cout << "please enter the slot id:";
    int32_t slotId = 0;
    std::cin >> slotId;
    imsCallPtr->GetImsSwitchStatus(slotId);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::SetImsConfigString(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test SetImsConfigString entry." << std::endl;
    std::cout << "please enter the item id:";
    int32_t item = 0;
    std::cin >> item;
    std::cout << "please enter the item value:";
    std::string value;
    std::cin >> value;
    imsCallPtr->SetImsConfig(static_cast<ImsConfigItem>(item), value);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::SetImsConfigInt(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test SetImsConfigInt entry." << std::endl;
    std::cout << "please enter the item id:";
    int32_t item = 0;
    std::cin >> item;
    std::cout << "please enter the item value:";
    int32_t value = 0;
    std::cin >> value;
    imsCallPtr->SetImsConfig(static_cast<ImsConfigItem>(item), value);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::GetImsConfig(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test GetImsConfig entry." << std::endl;
    std::cout << "please enter the item id:";
    int32_t item = 0;
    std::cin >> item;
    imsCallPtr->GetImsConfig(static_cast<ImsConfigItem>(item));
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::SetImsFeatureValue(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test SetImsFeatureValue entry." << std::endl;
    std::cout << "please enter the feature type:";
    int32_t type = 0;
    std::cin >> type;
    std::cout << "please enter the feature value:";
    int32_t value = 0;
    std::cin >> value;
    imsCallPtr->SetImsFeatureValue(static_cast<FeatureType>(type), value);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::GetImsFeatureValue(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test GetImsFeatureValue entry." << std::endl;
    std::cout << "please enter the feature type:";
    int32_t type = 0;
    std::cin >> type;
    int32_t value;
    imsCallPtr->GetImsFeatureValue(static_cast<FeatureType>(type), value);
    std::cout << "test GetImsFeatureValue value:" << value;
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::SetMute(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test SetMute entry.\n";
    int32_t slotId = 0;
    std::cout << "please enter the mute(0 - 1):";
    int32_t mute = -1;
    std::cin >> mute;
    return imsCallPtr->SetMute(slotId, mute);
}

int32_t ImsUnitTest::GetMute(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test GetMute entry.\n";
    int32_t slotId = 0;
    return imsCallPtr->GetMute(slotId);
}

int32_t ImsUnitTest::CtrlCamera(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test CtrlCamera entry." << std::endl;
    std::u16string cameraId = u"cameraId";
    int32_t callingUid = 0;
    int32_t callingPid = 0;
    imsCallPtr->CtrlCamera(cameraId, callingUid, callingPid);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::SetPreviewWindow(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test SetPreviewWindow entry." << std::endl;
    int32_t x = 0;
    int32_t y = 0;
    int32_t z = 0;
    int32_t width = 0;
    int32_t height = 0;
    imsCallPtr->SetPreviewWindow(x, y, z, width, height);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::SetDisplayWindow(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test SetDisplayWindow entry." << std::endl;
    int32_t x = 0;
    int32_t y = 0;
    int32_t z = 0;
    int32_t width = 0;
    int32_t height = 0;
    imsCallPtr->SetDisplayWindow(x, y, z, width, height);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::SetCameraZoom(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test SetCameraZoom entry." << std::endl;
    float zoomRatio = 1.0;
    imsCallPtr->SetCameraZoom(zoomRatio);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::SetPauseImage(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test SetPauseImage entry." << std::endl;
    std::u16string path = u"path";
    imsCallPtr->SetPauseImage(path);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::SetDeviceDirection(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test SetDeviceDirection entry." << std::endl;
    int32_t rotation = 0;
    imsCallPtr->SetDeviceDirection(rotation);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::GetClip(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "please enter the slot id:";
    int32_t slotId = 0;
    std::cin >> slotId;
    imsCallPtr->GetClip(slotId);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::SetClir(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "please enter the slot id:";
    int32_t slotId = 0;
    std::cin >> slotId;
    std::cout << "please enter the action:";
    int32_t action = 0;
    std::cin >> action;
    imsCallPtr->SetClir(slotId, action);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::GetClir(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "please enter the slot id:";
    int32_t slotId = 0;
    std::cin >> slotId;
    imsCallPtr->GetClir(slotId);
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::SetCallTransfer(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test SetCallTransfer entry.\n";
    int32_t slotId = 0;
    std::cout << "please enter the transfer type(0 - 5):";
    int32_t type = 0;
    std::cin >> type;
    std::cout << "please enter the transfer mode(0 - 4):";
    int32_t mode = 0;
    std::cin >> mode;
    std::cout << "please enter the phone number:";
    std::string transferNum;
    std::cin >> transferNum;
    int32_t classType = 1;
    return imsCallPtr->SetCallTransfer(slotId, type, mode, transferNum, classType);
}

int32_t ImsUnitTest::GetCallTransfer(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test GetCallTransfer entry.\n";
    int32_t slotId = 0;
    std::cout << "please enter the transfer type(0 - 5):";
    int32_t type = 0;
    std::cin >> type;
    return imsCallPtr->GetCallTransfer(slotId, type);
}

int32_t ImsUnitTest::SetCallRestriction(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test SetCallRestriction entry.\n";
    int32_t slotId = 0;
    std::cout << "please enter the fac:";
    std::string fac = "AO";
    std::cout << "please enter the call restriction password:";
    char password[IMS_TEST_MAX_PASSWORD] = { 0 };
    std::cin >> password;
    std::cout << "please enter the mode:";
    int32_t mode = 0;
    std::cin >> mode;
    int32_t ret = imsCallPtr->SetCallRestriction(slotId, fac, mode, password);
    memset_s(password, sizeof(password), 0, sizeof(password));
    return ret;
}

int32_t ImsUnitTest::GetCallRestriction(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test GetCallRestriction entry.\n";
    int32_t slotId = 0;
    std::cout << "please enter the fac:";
    std::string facType = "AO";
    return imsCallPtr->GetCallRestriction(slotId, facType);
}

int32_t ImsUnitTest::SetCallWaiting(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test SetCallWaiting entry.\n";
    bool activate = true;
    int32_t slotId = 0;
    return imsCallPtr->SetCallWaiting(slotId, activate, ServiceClassType::VOICE);
}

int32_t ImsUnitTest::GetCallWaiting(const sptr<ImsCallInterface> &imsCallPtr) const
{
    std::cout << "test GetCallWaiting entry.\n";
    int32_t slotId = 0;
    return imsCallPtr->GetCallWaiting(slotId);
}

int32_t ImsUnitTest::SetSmsConfig(const sptr<ImsSmsInterface> &imsSmsPtr) const
{
    std::cout << "test SetSmsConfig entry.\n";
    int32_t slotId = 0;
    int32_t imsSmsConfig = 1;
    return imsSmsPtr->ImsSetSmsConfig(slotId, imsSmsConfig);
}

int32_t ImsUnitTest::GetSmsConfig(const sptr<ImsSmsInterface> &imsSmsPtr) const
{
    std::cout << "test GetSmsConfig entry.\n";
    int32_t slotId = 0;
    return imsSmsPtr->ImsGetSmsConfig(slotId);
}

int32_t ImsUnitTest::SendMessage(const sptr<ImsSmsInterface> &imsSmsPtr) const
{
    std::cout << "test SendMessage entry.\n";
    int32_t slotId = 0;
    ImsMessageInfo imsMessageInfo;
    imsMessageInfo.refId = 1;
    return imsSmsPtr->ImsSendMessage(slotId, imsMessageInfo);
}

int32_t ImsUnitTest::GetImsRegistrationStatus(const sptr<ImsCoreServiceInterface> &imsCoreSerivcePtr) const
{
    std::cout << "test GetImsRegistrationStatus entry.\n";
    int32_t slotId = 0;
    return imsCoreSerivcePtr->GetImsRegistrationStatus(slotId);
}

int32_t ImsUnitTest::ImsCallTest() const
{
    auto imsCallPtr = DelayedSingleton<ImsCallClient>::GetInstance()->GetImsCallProxy();
    if (imsCallPtr == nullptr) {
        std::cout << "\nimsCallPtr is nullptr, Init ImsCall Failed!\n";
        return IMS_TEST_ERROR;
    }
    std::shared_ptr<ImsTestHandler> handler = std::make_shared<ImsTestHandler>();
    handler->RegisterImsCall();
    int32_t ret = -1;
    int32_t InputNum = 0;
    int32_t returnNum = 1000; //  end loop variable value
    bool loopFlag = true;
    while (loopFlag) {
        std::cout << "\n**********Unit Test Start**********\n"
                     "Usage: please input a cmd num:\n"
                     "1:Dial\n2:HangUp\n3:RejectWithReason\n4:Answer\n5:HoldCall\n6:UnHoldCall\n7:SwitchCall\n"
                     "8:CombineConference\n9:InviteToConference\n10:KickOutFromConference\n11:UpdateImsCallMode\n"
                     "13:GetImsCallsDataRequest\n14:GetLastCallFailReason\n100:StartDtmf\n101:SendDtmf\n102:StopDtmf\n"
                     "103:StartRtt\n104:StopRtt\n200:SetCallPreferenceMode\n201:GetCallPreferenceMode\n202:"
                     "SetImsSwitchStatus\n203:GetImsSwitchStatus\n204:SetImsConfigString\n205:SetImsConfigInt\n"
                     "206:GetImsConfig\n207:SetImsFeatureValue\n208:GetImsFeatureValue\n"
                     "209:SetMute\n210:GetMute\n300:CtrlCamera\n301:SetPreviewWindow\n"
                     "302:SetDisplayWindow\n303:SetCameraZoom\n304:SetPauseImage\n305:SetDeviceDirection\n"
                     "400:GetClip\n401:SetClir\n402:GetClir\n"
                     "403:SetCallTransfer\n404:GetCallTransfer\n405:SetCallRestriction\n406:GetCallRestriction\n"
                     "407:SetCallWaiting\n408:GetCallWaiting\n"
                     "500:RegisterCellularCallBack\n501:IsCallBackExists\n"
                     "1000:Exit\n"
                     "***********************************\n"
                     "Your choice: ";
        std::cin >> InputNum;
        if (InputNum == returnNum) {
            std::cout << "exiting...\n";
            break;
        }
        auto itFunc = requestCallFuncMap_.find(InputNum);
        if (itFunc != requestCallFuncMap_.end()) {
            auto requestFunc = itFunc->second;
            if (requestFunc != nullptr) {
                ret = (this->*requestFunc)(imsCallPtr);
            }
        }
        if (ret != IMS_TEST_SUCCESS) {
            std::cout << "this execution did not return a successful value, please check.\n";
            return ret;
        }
    }
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::ImsSmsTest() const
{
    auto imsSmsPtr = DelayedSingleton<ImsSmsClient>::GetInstance()->GetImsSmsProxy();
    if (imsSmsPtr == nullptr) {
        std::cout << "\nimsSmsPtr is nullptr, Init ImsSms Failed!\n";
        return IMS_TEST_ERROR;
    }
    std::shared_ptr<ImsTestHandler> handler = std::make_shared<ImsTestHandler>();
    handler->RegisterImsSms();
    int32_t ret = -1;
    int32_t InputNum = 0;
    int32_t returnNum = 1000; //  end loop variable value
    bool loopFlag = true;
    while (loopFlag) {
        std::cout << "\n**********Unit Test Start**********\n"
                     "Usage: please input a cmd num:\n"
                     "0:SendMessage\n"
                     "100:SetSmsConfig\n"
                     "101:GetSmsConfig\n"
                     "1000:Exit\n"
                     "***********************************\n"
                     "Your choice: ";
        std::cin >> InputNum;
        if (InputNum == returnNum) {
            std::cout << "exiting...\n";
            break;
        }
        auto itFunc = requestSmsFuncMap_.find(InputNum);
        if (itFunc != requestSmsFuncMap_.end()) {
            auto requestFunc = itFunc->second;
            if (requestFunc != nullptr) {
                ret = (this->*requestFunc)(imsSmsPtr);
            }
        }
        if (ret != IMS_TEST_SUCCESS) {
            std::cout << "this execution did not return a successful value, please check.\n";
            return ret;
        }
    }
    return IMS_TEST_SUCCESS;
}

int32_t ImsUnitTest::ImsCoreServiceTest() const
{
    auto imsCoreSerivcePtr = DelayedSingleton<ImsCoreServiceClient>::GetInstance()->GetImsCoreServiceProxy();
    if (imsCoreSerivcePtr == nullptr) {
        std::cout << "\nimsCoreSerivcePtr is nullptr, Init ImsCoreService Failed!\n";
        return IMS_TEST_ERROR;
    }
    std::shared_ptr<ImsTestHandler> handler = std::make_shared<ImsTestHandler>();
    handler->RegisterImsCoreService();
    int32_t ret = -1;
    int32_t InputNum = 0;
    int32_t returnNum = 1000; //  end loop variable value
    bool loopFlag = true;
    while (loopFlag) {
        std::cout << "\n**********Unit Test Start**********\n"
                     "Usage: please input a cmd num:\n"
                     "0:GetImsRegistrationStatus\n"
                     "1000:Exit\n"
                     "***********************************\n"
                     "Your choice: ";
        std::cin >> InputNum;
        if (InputNum == returnNum) {
            std::cout << "exiting...\n";
            break;
        }
        auto itFunc = requestCoreServiceFuncMap_.find(InputNum);
        if (itFunc != requestCoreServiceFuncMap_.end()) {
            auto requestFunc = itFunc->second;
            if (requestFunc != nullptr) {
                ret = (this->*requestFunc)(imsCoreSerivcePtr);
            }
        }
        if (ret != IMS_TEST_SUCCESS) {
            std::cout << "this execution did not return a successful value, please check.\n";
            return ret;
        }
    }
    return IMS_TEST_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS

using namespace OHOS::Telephony;
int32_t main()
{
    std::unique_ptr<ImsUnitTest> test = std::make_unique<ImsUnitTest>();
    const int32_t callTest = 1;
    const int32_t smsTest = 2;
    const int32_t coreServiceTest = 3;
    const int32_t codeExit = 1000;
    int32_t code;
    test->SetUp();
    std::cout << "Please select interface type...." << std::endl;
    while (true) {
        std::cout << "\n1:ims call interface\n"
                  << "2:ims sms interface\n"
                  << "3:ims core service interface\n"
                  << "1000:exit\n";
        std::cin >> code;
        switch (code) {
            case callTest:
                test->ImsCallTest();
                break;
            case smsTest:
                test->ImsSmsTest();
                break;
            case coreServiceTest:
                test->ImsCoreServiceTest();
                break;
            case codeExit:
                break;
            default:
                std::cout << "invalid input" << std::endl;
                break;
        }
        if (code == codeExit) {
            break;
        }
    }
    return OHOS::Telephony::TELEPHONY_SUCCESS;
}
