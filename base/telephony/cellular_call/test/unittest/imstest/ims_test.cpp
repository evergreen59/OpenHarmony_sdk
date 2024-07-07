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

#include "ims_test.h"

#define private public
#define protected public
#include "cellular_call_register.h"
#include "hril_call_parcel.h"
#include "ims_control.h"
#include "securec.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;
const int32_t SIM1_SLOTID = 0;
const int32_t SIM2_SLOTID = 1;
const int32_t INVALID_HANG_UP_TYPE = -1;
const int32_t RESULT = 1;
const std::string PHONE_NUMBER = "0000000";
const std::string PHONE_NUMBER_SECOND = "1111111";
const std::string PHONE_NUMBER_THIRD = "2222222";
const std::string PHONE_NUMBER_FOUR = "3333333";
const int32_t CELLULAR_CALL_SUCCESS = 0;

void ImsTest::SetUpTestCase(void)
{
    // step 3: Set Up Test Case
}

void ImsTest::TearDownTestCase(void)
{
    // step 3: Tear Down Test Case
}

void ImsTest::SetUp(void)
{
    // step 3: input testcase setup step
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::SET_DOMAIN_PREFERENCE_MODE)] =
        &ImsTest::SetDomainPreferenceMode;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::GET_DOMAIN_PREFERENCE_MODE)] =
        &ImsTest::GetDomainPreferenceMode;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::SET_IMS_SWITCH_STATUS)] =
        &ImsTest::SetImsSwitchStatus;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::GET_IMS_SWITCH_STATUS)] =
        &ImsTest::GetImsSwitchStatus;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::SET_IMS_CONFIG_STRING)] =
        &ImsTest::SetImsConfigString;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::SET_IMS_CONFIG_INT)] =
        &ImsTest::SetImsConfigInt;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::GET_IMS_CONFIG)] =
        &ImsTest::GetImsConfig;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::SET_IMS_FEATURE)] =
        &ImsTest::SetImsFeatureValue;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::GET_IMS_FEATURE)] =
        &ImsTest::GetImsFeatureValue;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::CTRL_CAMERA)] = &ImsTest::CtrlCamera;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::SET_PREVIEW_WINDOW)] =
        &ImsTest::SetPreviewWindow;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::SET_DISPLAY_WINDOW)] =
        &ImsTest::SetDisplayWindow;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::SET_CAMERA_ZOOM)] =
        &ImsTest::SetCameraZoom;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::SET_PAUSE_IMAGE)] =
        &ImsTest::SetPauseImage;
    requestFuncMap_[static_cast<int32_t>(CellularCallInterface::OperationType::SET_DEVICE_DIRECTION)] =
        &ImsTest::SetDeviceDirection;
}

void ImsTest::TearDown(void)
{
    // step 3: input testcase teardown step
}

int32_t ImsTest::SetDomainPreferenceMode(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test SetDomainPreferenceMode entry." << std::endl;
    std::cout << "please enter the need mode:(1 - 4)";
    int32_t mode;
    std::cin >> mode;
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    return telephonyService->SetDomainPreferenceMode(slotId, mode);
}

int32_t ImsTest::GetDomainPreferenceMode(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test GetDomainPreferenceMode entry." << std::endl;
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    return telephonyService->GetDomainPreferenceMode(slotId);
}

int32_t ImsTest::SetImsSwitchStatus(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test SetImsSwitchStatus entry." << std::endl;
    std::cout << "please enter the switch state:";
    bool active;
    std::cin >> active;
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    return telephonyService->SetImsSwitchStatus(slotId, active);
}

int32_t ImsTest::GetImsSwitchStatus(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test GetImsSwitchStatus entry." << std::endl;
    int32_t slotId = 0;
    std::cout << "please enter the slotId:(0   1)";
    std::cin >> slotId;
    bool enabled;
    return telephonyService->GetImsSwitchStatus(slotId, enabled);
}

int32_t ImsTest::SetImsConfigString(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test SetImsConfigString entry." << std::endl;
    std::cout << "please enter the item id:";
    int32_t item = 0;
    std::cin >> item;
    std::cout << "please enter the item value:";
    std::string value;
    std::cin >> value;
    int32_t slotId = 0;
    return telephonyService->SetImsConfig(slotId, static_cast<ImsConfigItem>(item), value);
}

int32_t ImsTest::SetImsConfigInt(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test SetImsConfigInt entry." << std::endl;
    std::cout << "please enter the item id:";
    int32_t item = 0;
    std::cin >> item;
    std::cout << "please enter the item value:";
    int32_t value = 0;
    std::cin >> value;
    int32_t slotId = 0;
    return telephonyService->SetImsConfig(slotId, static_cast<ImsConfigItem>(item), value);
}

int32_t ImsTest::GetImsConfig(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test GetImsConfig entry." << std::endl;
    std::cout << "please enter the item id:";
    int32_t item = 0;
    std::cin >> item;
    int32_t slotId = 0;
    return telephonyService->GetImsConfig(slotId, static_cast<ImsConfigItem>(item));
}

int32_t ImsTest::SetImsFeatureValue(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test SetImsFeatureValue entry." << std::endl;
    std::cout << "please enter the feature type:";
    int32_t type = 0;
    std::cin >> type;
    std::cout << "please enter the feature value:";
    int32_t value = 0;
    std::cin >> value;
    int32_t slotId = 0;
    return telephonyService->SetImsFeatureValue(slotId, static_cast<FeatureType>(type), value);
}

int32_t ImsTest::GetImsFeatureValue(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test GetImsFeatureValue entry." << std::endl;
    std::cout << "please enter the feature type:";
    int32_t type = 0;
    std::cin >> type;
    int32_t slotId = 0;
    return telephonyService->GetImsFeatureValue(slotId, static_cast<FeatureType>(type));
}

int32_t ImsTest::CtrlCamera(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test CtrlCamera entry." << std::endl;
    std::u16string cameraId = u"cameraId";
    int32_t callingUid = 0;
    int32_t callingPid = 0;
    return telephonyService->CtrlCamera(cameraId, callingUid, callingPid);
}

int32_t ImsTest::SetPreviewWindow(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test SetPreviewWindow entry." << std::endl;
    int32_t x = 0;
    int32_t y = 0;
    int32_t z = 0;
    int32_t width = 0;
    int32_t height = 0;
    return telephonyService->SetPreviewWindow(x, y, z, width, height);
}

int32_t ImsTest::SetDisplayWindow(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test SetDisplayWindow entry." << std::endl;
    int32_t x = 0;
    int32_t y = 0;
    int32_t z = 0;
    int32_t width = 0;
    int32_t height = 0;
    return telephonyService->SetDisplayWindow(x, y, z, width, height);
}

int32_t ImsTest::SetCameraZoom(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test SetCameraZoom entry." << std::endl;
    float zoomRatio = 1.0;
    return telephonyService->SetCameraZoom(zoomRatio);
}

int32_t ImsTest::SetPauseImage(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test SetPauseImage entry." << std::endl;
    std::u16string path = u"path";
    return telephonyService->SetPauseImage(path);
}

int32_t ImsTest::SetDeviceDirection(const sptr<CellularCallInterface> &telephonyService) const
{
    std::cout << "test SetDeviceDirection entry." << std::endl;
    int32_t rotation = 0;
    return telephonyService->SetDeviceDirection(rotation);
}

int32_t ImsTest::InputNumForInterface(const sptr<CellularCallInterface> &telephonyService) const
{
    int32_t ret = -1;
    bool loopFlag = true;
    int32_t InputNum = 0;
    int32_t returnNum = 1000; // end loop variable value
    while (loopFlag) {
        std::cout << "\n**********Unit Test Start**********\n"
                     "Usage: please input a cmd num:\n"
                     "300:SetCallPreferenceMode\n"
                     "301:GetCallPreferenceMode\n"
                     "302:SetImsSwitchStatus\n"
                     "303:GetImsSwitchStatus\n"
                     "304:SetImsConfigString\n"
                     "305:SetImsConfigInt\n"
                     "306:GetImsConfig\n"
                     "307:SetImsFeatureValue\n"
                     "308:GetImsFeatureValue\n"
                     "400:CtrlCamera\n"
                     "401:SetPreviewWindow\n"
                     "402:SetDisplayWindow\n"
                     "403:SetCameraZoom\n"
                     "404:SetPauseImage\n"
                     "405:SetDeviceDirection\n"
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
 * @tc.number   cellular_call_ims_test_001
 * @tc.name     Test the corresponding functions by entering commands, such as 300 -- SetCallPreferenceMode, 301 --
 *              GetCallPreferenceMode, etc
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_ims_test_001, Function | MediumTest | Level0)
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    std::cout << "HWTEST_F cellular_call_ims_test_001";
}

/**
 * @tc.number   cellular_call_ims_test_002
 * @tc.name     Test the corresponding functions by entering commands, such as 300 -- SetCallPreferenceMode, 301 --
 *              GetCallPreferenceMode, etc
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_ims_test_002, Function | MediumTest | Level1)
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    auto telephonyService = iface_cast<CellularCallInterface>(remote);
    ASSERT_TRUE(telephonyService != nullptr);
    std::cout << "HWTEST_F cellular_call_ims_test_002";
}

/**
 * @tc.number   cellular_call_DialCall_0001
 * @tc.name     Test for SetClip function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_DialCall_0001, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM1_SLOTID, "*30#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM2_SLOTID, "*30#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0002
 * @tc.name     Test for SetClip function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_DialCall_0002, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM1_SLOTID, "#30#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM2_SLOTID, "#30#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0003
 * @tc.name     Test for getClip function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_DialCall_0003, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM1_SLOTID, "*#30#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM2_SLOTID, "*#30#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0004
 * @tc.name     Test for SetClir function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_DialCall_0004, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM1_SLOTID, "*31#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM2_SLOTID, "*31#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0005
 * @tc.name     Test for SetClir function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_DialCall_0005, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM1_SLOTID, "#31#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM2_SLOTID, "#31#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0006
 * @tc.name     Test for GetClir function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_DialCall_0006, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM1_SLOTID, "*#31#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM2_SLOTID, "*#31#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0007
 * @tc.name     Test for SetCallTransfer function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_DialCall_0007, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM1_SLOTID, "*21#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM2_SLOTID, "*21#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0008
 * @tc.name     Test for SetCallTransfer function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_DialCall_0008, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM1_SLOTID, "#21#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM2_SLOTID, "#21#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0009
 * @tc.name     Test for GetCallTransfer function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_DialCall_0009, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM1_SLOTID, "*#21#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM2_SLOTID, "*#21#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0010
 * @tc.name     Test for SetCallRestriction function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_DialCall_0010, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM1_SLOTID, "*33#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM2_SLOTID, "*33#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0011
 * @tc.name     Test for SetCallRestriction function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_DialCall_0011, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM1_SLOTID, "#33#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM2_SLOTID, "#33#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0012
 * @tc.name     Test for GetCallRestriction function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_DialCall_0012, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM1_SLOTID, "*#33#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM2_SLOTID, "*#33#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0013
 * @tc.name     Test for SetCallWaiting function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_DialCall_0013, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM1_SLOTID, "*43#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM2_SLOTID, "*43#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0014
 * @tc.name     Test for SetCallWaiting function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_DialCall_0014, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM1_SLOTID, "#43#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM2_SLOTID, "#43#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_DialCall_0015
 * @tc.name     Test for GetCallWaiting function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_DialCall_0015, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM1_SLOTID, "*#43#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = TestDialCallByIms(SIM1_SLOTID, "*#4@3#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_UT)) {
        int32_t ret = TestDialCallByIms(SIM2_SLOTID, "*#43#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = TestDialCallByIms(SIM2_SLOTID, "*#4@3#");
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_HangUpCall_0001
 * @tc.name     Test for HangUp function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_HangUpCall_0001, Function | MediumTest | Level2)
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
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_VOICE)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->HangUp(callInfo, CallSupplementType::TYPE_DEFAULT);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
        ret = telephonyService->HangUp(callInfo, CallSupplementType::TYPE_HANG_UP_ACTIVE);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_VOICE)) {
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
 * @tc.number   cellular_call_AnswerCall_0001
 * @tc.name     Test for answer function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_AnswerCall_0001, Function | MediumTest | Level2)
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
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_VOICE)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->Answer(callInfo);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_VOICE)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM2_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->Answer(callInfo);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
    }
}

/**
 * @tc.number   cellular_call_RejectCall_0001
 * @tc.name     Test for reject function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_RejectCall_0001, Function | MediumTest | Level2)
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
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_VOICE)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->Reject(callInfo);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_VOICE)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM2_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->Reject(callInfo);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
    }
}

/**
 * @tc.number   cellular_call_HoldCall_0001
 * @tc.name     Test for hold call function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_HoldCall_0001, Function | MediumTest | Level2)
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
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_VOICE)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->HoldCall(callInfo);
        EXPECT_EQ(ret, TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_VOICE)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM2_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->HoldCall(callInfo);
        EXPECT_EQ(ret, TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    }
}

/**
 * @tc.number   cellular_call_UnHoldCall_0001
 * @tc.name     Test for unhold call function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_UnHoldCall_0001, Function | MediumTest | Level2)
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
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_VOICE)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->UnHoldCall(callInfo);
        EXPECT_EQ(ret, TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_VOICE)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM2_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->UnHoldCall(callInfo);
        EXPECT_EQ(ret, TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    }
}

/**
 * @tc.number   cellular_call_SwitchCall_0001
 * @tc.name     Test for switch call function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_SwitchCall_0001, Function | MediumTest | Level2)
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
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_VOICE)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->SwitchCall(callInfo);
        EXPECT_EQ(ret, TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_VOICE)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM2_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->SwitchCall(callInfo);
        EXPECT_EQ(ret, TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    }
}

/**
 * @tc.number   cellular_call_CombineConference_0001
 * @tc.name     Test for combineConference function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_CombineConference_0001, Function | MediumTest | Level2)
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
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_VOICE)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->CombineConference(callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_VOICE)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM2_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->CombineConference(callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_SeparateConference_0001
 * @tc.name     Test for separateConference function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_SeparateConference_0001, Function | MediumTest | Level2)
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
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_VOICE)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->SeparateConference(callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_VOICE)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM2_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->SeparateConference(callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_StartDtmf_0001
 * @tc.name     Test for startDtmf function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_StartDtmf_0001, Function | MediumTest | Level2)
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
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_VOICE)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        char code = '1';
        ret = telephonyService->StartDtmf(code, callInfo);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_VOICE)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM2_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        char code = '1';
        ret = telephonyService->StartDtmf(code, callInfo);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
    }
}

/**
 * @tc.number   cellular_call_StopDtmf_0001
 * @tc.name     Test for stopDtmf function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_StopDtmf_0001, Function | MediumTest | Level2)
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
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_VOICE)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->StopDtmf(callInfo);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_VOICE)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM2_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        ret = telephonyService->StopDtmf(callInfo);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
    }
}

/**
 * @tc.number   cellular_call_SendDtmf_0001
 * @tc.name     Test for sendDtmf function by ims
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_SendDtmf_0001, Function | MediumTest | Level2)
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
    if (HasSimCard(SIM1_SLOTID) && CanUseImsService(SIM1_SLOTID, ImsServiceType::TYPE_VOICE)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM1_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        char code = '1';
        ret = telephonyService->SendDtmf(code, callInfo);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
    }
    if (HasSimCard(SIM2_SLOTID) && CanUseImsService(SIM2_SLOTID, ImsServiceType::TYPE_VOICE)) {
        CellularCallInfo callInfo;
        int32_t ret = InitCellularCallInfo(SIM2_SLOTID, PHONE_NUMBER, callInfo);
        EXPECT_EQ(ret, TELEPHONY_SUCCESS);
        char code = '1';
        ret = telephonyService->SendDtmf(code, callInfo);
        EXPECT_EQ(ret, CALL_ERR_CALL_CONNECTION_NOT_EXIST);
    }
}

/**
 * @tc.number   cellular_call_ImsControl_0001
 * @tc.name     Test for ImsControl
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_ImsControl_0001, Function | MediumTest | Level3)
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    auto imsControl = std::make_shared<IMSControl>();
    CellularCallInfo cellularCallInfo;
    for (int32_t slotId = 0; slotId < SIM_SLOT_COUNT; slotId++) {
        if (!HasSimCard(slotId) || !CanUseImsService(slotId, ImsServiceType::TYPE_VOICE)) {
            continue;
        }
        EXPECT_EQ(imsControl->UpdateImsCallMode(cellularCallInfo, ImsCallMode::CALL_MODE_AUDIO_ONLY),
            CALL_ERR_CALL_CONNECTION_NOT_EXIST);
        EXPECT_EQ(InitCellularCallInfo(slotId, PHONE_NUMBER, cellularCallInfo), TELEPHONY_SUCCESS);
        EXPECT_EQ(imsControl->Dial(cellularCallInfo), CALL_ERR_GET_RADIO_STATE_FAILED);
        ImsCurrentCallList callList;
        callList.callSize = 0;
        CallInfoList callInfoList;
        EXPECT_EQ(imsControl->ReportCallsData(slotId, callInfoList), TELEPHONY_ERROR);
        EXPECT_EQ(imsControl->ReportImsCallsData(slotId, callList), TELEPHONY_ERROR);
        ImsCurrentCall callInfo;
        callList.callSize = 1;
        callInfo.number = PHONE_NUMBER;
        callInfo.index = 1;
        callInfo.state = static_cast<int32_t>(TelCallState::CALL_STATUS_INCOMING);
        callList.calls.push_back(callInfo);
        EXPECT_EQ(imsControl->ReportImsCallsData(slotId, callList), TELEPHONY_SUCCESS);
        EXPECT_EQ(imsControl->Answer(cellularCallInfo), TELEPHONY_ERROR);
        EXPECT_EQ(imsControl->Reject(cellularCallInfo), TELEPHONY_ERROR);
        EXPECT_EQ(imsControl->HangUpAllConnection(slotId), TELEPHONY_ERROR);
        callList.callSize = 2;
        callInfo.index = 2;
        callInfo.state = static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE);
        callInfo.number = PHONE_NUMBER_SECOND;
        callList.calls.push_back(callInfo);
        callList.callSize = 3;
        callInfo.index = 3;
        callInfo.state = static_cast<int32_t>(TelCallState::CALL_STATUS_WAITING);
        callInfo.number = PHONE_NUMBER_THIRD;
        callList.calls.push_back(callInfo);
        callList.callSize = 4;
        callInfo.index = 4;
        callInfo.state = static_cast<int32_t>(TelCallState::CALL_STATUS_DISCONNECTED);
        callInfo.number = PHONE_NUMBER_FOUR;
        callList.calls.push_back(callInfo);
        EXPECT_EQ(imsControl->ReportImsCallsData(slotId, callList), TELEPHONY_SUCCESS);
        CLIRMode mode = CLIRMode::DEFAULT;
        EXPECT_EQ(imsControl->DialJudgment(slotId, PHONE_NUMBER_SECOND, mode, 0), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
        EXPECT_EQ(imsControl->DialJudgment(slotId, PHONE_NUMBER_THIRD, mode, 0), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
        EXPECT_EQ(InitCellularCallInfo(slotId, PHONE_NUMBER_SECOND, cellularCallInfo), TELEPHONY_SUCCESS);
        EXPECT_EQ(imsControl->Answer(cellularCallInfo), CALL_ERR_CALL_STATE);
        EXPECT_EQ(imsControl->UpdateImsCallMode(cellularCallInfo, ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERROR);
        EXPECT_EQ(InitCellularCallInfo(slotId, PHONE_NUMBER_THIRD, cellularCallInfo), TELEPHONY_SUCCESS);
        EXPECT_EQ(imsControl->Answer(cellularCallInfo), TELEPHONY_ERROR);
        EXPECT_EQ(InitCellularCallInfo(slotId, PHONE_NUMBER_FOUR, cellularCallInfo), TELEPHONY_SUCCESS);
        EXPECT_EQ(imsControl->Answer(cellularCallInfo), CALL_ERR_CALL_STATE);
        EXPECT_EQ(imsControl->Reject(cellularCallInfo), CALL_ERR_CALL_STATE);
        EXPECT_EQ(imsControl->HoldCall(slotId), CALL_ERR_CALL_STATE);
        std::vector<std::string> numberList;
        EXPECT_EQ(imsControl->KickOutFromConference(slotId, numberList), TELEPHONY_ERROR);
        EXPECT_EQ(imsControl->InviteToConference(slotId, numberList), TELEPHONY_ERROR);
        EXPECT_EQ(imsControl->StartRtt(slotId, PHONE_NUMBER), TELEPHONY_ERROR);
        EXPECT_EQ(imsControl->StopRtt(slotId), TELEPHONY_ERROR);
        EXPECT_EQ(
            imsControl->UpdateImsCallMode(cellularCallInfo, ImsCallMode::CALL_MODE_AUDIO_ONLY), CALL_ERR_CALL_STATE);
        EXPECT_EQ(imsControl->HangUp(cellularCallInfo, CallSupplementType::TYPE_DEFAULT), TELEPHONY_ERROR);
        EXPECT_EQ(imsControl->HangUp(cellularCallInfo, CallSupplementType::TYPE_HANG_UP_ACTIVE),
            CALL_ERR_RESOURCE_UNAVAILABLE);
        EXPECT_EQ(imsControl->HangUp(cellularCallInfo, CallSupplementType::TYPE_HANG_UP_ALL), TELEPHONY_ERROR);
        EXPECT_EQ(imsControl->HangUp(cellularCallInfo, static_cast<CallSupplementType>(INVALID_HANG_UP_TYPE)),
            TELEPHONY_ERR_ARGUMENT_INVALID);
        callList.callSize = 0;
        EXPECT_EQ(imsControl->ReportImsCallsData(slotId, callList), TELEPHONY_SUCCESS);
    }
}

/**
 * @tc.number   cellular_call_CellularCallConnectionIMS_0001
 * @tc.name     Test for CellularCallConnectionIMS
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_CellularCallConnectionIMS_0001, Function | MediumTest | Level3)
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(systemAbilityMgr != nullptr);
    auto remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    ASSERT_TRUE(remote != nullptr);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    for (int32_t slotId = 0; slotId < SIM_SLOT_COUNT; slotId++) {
        if (!HasSimCard(slotId) || !CanUseImsService(slotId, ImsServiceType::TYPE_VOICE)) {
            continue;
        }
        CellularCallConnectionIMS imsConnection;
        EXPECT_EQ(imsConnection.SendDtmfRequest(slotId, '1', 1), TELEPHONY_ERROR);
        EXPECT_EQ(imsConnection.StartDtmfRequest(slotId, '1', 1), TELEPHONY_ERROR);
        EXPECT_EQ(imsConnection.StopDtmfRequest(slotId, 1), TELEPHONY_ERROR);
        EXPECT_EQ(imsConnection.GetImsCallsDataRequest(slotId, 1), TELEPHONY_ERROR);
        EXPECT_EQ(imsConnection.GetCallFailReasonRequest(slotId), TELEPHONY_ERROR);
    }
}

/**
 * @tc.number   cellular_call_CellularCallRegister_0001
 * @tc.name     Test for CellularCallRegister
 * @tc.desc     Function test
 */
HWTEST_F(ImsTest, cellular_call_CellularCallRegister_0001, Function | MediumTest | Level3)
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
    GetImsConfigResponse imsConfigResponse;
    callRegister->ReportGetImsConfigResult(imsConfigResponse);
    callRegister->ReportSetImsConfigResult(RESULT);
    GetImsFeatureValueResponse imsFeatureValueResponse;
    callRegister->ReportGetImsFeatureResult(imsFeatureValueResponse);
    callRegister->ReportSetImsFeatureResult(RESULT);
    callRegister->ReportUpdateCallMediaModeResult(RESULT);
}
} // namespace Telephony
} // namespace OHOS
