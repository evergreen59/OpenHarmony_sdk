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

#include "call_manager_gtest.h"

#include <cstring>
#include <gtest/gtest.h>
#include <string>

#include "bluetooth_hfp_ag.h"
#include "call_manager_service.h"
#include "telephony_types.h"

using namespace OHOS::Bluetooth;
namespace OHOS {
namespace Telephony {
using namespace testing::ext;
#ifndef TEL_TEST_UNSUPPORT
constexpr int16_t SIM1_SLOTID = 0;
constexpr int16_t SIM2_SLOTID = 1;
constexpr int16_t SIM1_SLOTID_NO_CARD = 0;
constexpr int16_t RETURN_VALUE_IS_ZERO = 0;
constexpr int16_t INVALID_NEGATIVE_ID = -100;
constexpr int16_t INVALID_POSITIVE_ID = 100;
#ifndef CALL_MANAGER_IMS_LITE_UNSUPPORT
constexpr int16_t CAMERA_ROTATION_90 = 90;
constexpr int16_t CAMERA_ROTATION_ERROR = 50;
#endif // CALL_MANAGER_IMS_LITE_UNSUPPORT
#endif // TEL_TEST_UNSUPPORT
constexpr int16_t SLEEP_1000_MS = 1000;
constexpr int BASE_TIME_MS = 1000;
constexpr int SLEEP_TIME_MS = 50;
constexpr int MAX_LIMIT_TIME = 18000;
constexpr int INVALID_DIAL_TYPE = 3;
const std::string PHONE_NUMBER = "xxxxx";

int32_t CallInfoManager::CallDetailsChange(const CallAttributeInfo &info)
{
    TELEPHONY_LOGI("CallDetailsChange Start");
    std::lock_guard<std::mutex> lock(mutex_);
    updateCallInfo_ = info;
    if (callIdSet_.find(updateCallInfo_.callId) == callIdSet_.end()) {
        TELEPHONY_LOGI("CallDetailsChange new call");
        callIdSet_.insert(updateCallInfo_.callId);
        newCallId_ = updateCallInfo_.callId;
        newCallState_ = (int32_t)updateCallInfo_.callState;
        std::unordered_set<int32_t> newSet;
        newSet.clear();
        g_callStateMap.insert(std::pair<int32_t, std::unordered_set<int32_t>>(newCallId_, newSet));
    }
    g_callStateMap[updateCallInfo_.callId].insert((int32_t)updateCallInfo_.callState);
    return TELEPHONY_SUCCESS;
}

bool CallInfoManager::HasActiveStatus()
{
    TELEPHONY_LOGI("Waiting for activation !");
    int sumUseTime = 0;
    int slipMs = SLEEP_TIME_MS;
    do {
        if (!(HasState(newCallId_, static_cast<int32_t>(TelCallState::CALL_STATUS_ACTIVE)))) {
            usleep(slipMs * BASE_TIME_MS);
            sumUseTime += slipMs;
        } else {
            TELEPHONY_LOGI("===========wait %d ms callStatus:%d==============", sumUseTime, newCallState_);
            return true;
        }
    } while (sumUseTime < MAX_LIMIT_TIME);
    TELEPHONY_LOGI("===========wait %d ms callStatus:%d=====not active=========", sumUseTime, newCallState_);
    return false;
}

bool CallInfoManager::HasState(int callId, int32_t callState)
{
    if (g_callStateMap.find(callId) == g_callStateMap.end()) {
        return false;
    }
    if (g_callStateMap[callId].find(callState) == g_callStateMap[callId].end()) {
        return false;
    }
    return true;
}

int32_t CallInfoManager::CallEventChange(const CallEventInfo &info)
{
    return TELEPHONY_SUCCESS;
}

void CallInfoManager::Init()
{
    newCallId_ = -1;
    updateCallInfo_.speakerphoneOn = -1;
    updateCallInfo_.startTime = -1;
    updateCallInfo_.isEcc = -1;
    updateCallInfo_.accountId = -1;
    updateCallInfo_.callType = CallType::TYPE_CS;
    updateCallInfo_.callId = -1;
    updateCallInfo_.callState = TelCallState::CALL_STATUS_IDLE;
    updateCallInfo_.videoState = VideoStateType::TYPE_VOICE;
}

void CallInfoManager::LockCallId(bool eq, int32_t targetId, int32_t slipMs, int32_t timeoutMs)
{
    int32_t useTimeMs = 0;
    std::cout << "wait for a few seconds......" << std::endl;
    while ((updateCallInfo_.callId != targetId) && (useTimeMs < timeoutMs)) {
        usleep(slipMs * SLEEP_1000_MS);
        useTimeMs += slipMs;
    }
    std::cout << "\n===========wait " << useTimeMs << " ms target:" << updateCallInfo_.callId << std::endl;
    if (eq) {
        EXPECT_EQ(updateCallInfo_.callId, targetId);
    } else {
        EXPECT_NE(updateCallInfo_.callId, targetId);
    }
}

void CallInfoManager::LockCallState(bool eq, int32_t targetState, int32_t slipMs, int32_t timeoutMs)
{
    if (CallManagerGtest::clientPtr_->GetCallState() == targetState) {
        return;
    }
    int32_t usedTimeMs = 0;
    std::cout << "wait for a few seconds......" << std::endl;
    while ((CallManagerGtest::clientPtr_->GetCallState() != targetState) && (usedTimeMs < timeoutMs)) {
        usleep(slipMs * SLEEP_1000_MS);
        usedTimeMs += slipMs;
    }
    int32_t callState = CallManagerGtest::clientPtr_->GetCallState();
    std::cout << "waited " << usedTimeMs << " seconds" << std::endl;
    std::cout << "target call state:" << targetState << std::endl;
    EXPECT_EQ(callState, targetState);
}

void CallManagerGtest::HangUpCall()
{
    clientPtr_->HangUpCall(newCallId_);
    int useTimeMs = 0;
    while ((CallInfoManager::HasState(newCallId_, (int32_t)TelCallState::CALL_STATUS_DISCONNECTED) != true) &&
        (useTimeMs < SLEEP_12000_MS)) {
        usleep((SLEEP_50_MS * SLEEP_1000_MS));
        useTimeMs += SLEEP_50_MS;
    }
    TELEPHONY_LOGI("===========wait %d ms target:%d==============", useTimeMs,
        CallInfoManager::HasState(newCallId_, (int32_t)TelCallState::CALL_STATUS_DISCONNECTED));
}

#ifndef TEL_TEST_UNSUPPORT
/********************************************* Test DialCall()***********************************************/
/**
 * @tc.number   Telephony_CallManager_DialCall_0100
 * @tc.name     make a normal phone call with card1, TYPE_VOICE
 * @tc.desc     Function test
 * @tc.require: I5P2WO
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "00000000000";
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
    }

    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_0200
 * @tc.name     make a normal phone call with null telephone numbers,
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 * @tc.require: I5P2WO
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "";
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_0300
 * @tc.name     make a normal phone call with telephone numbers is negative number
 * @tc.desc     Function test
 * @tc.require: I5P2WO
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "-12354785268";
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_0400
 * @tc.name     make a normal phone call with telephone numbers is too long
 * @tc.desc     Function test
 * @tc.require: I5P2WO
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_0400, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "19119080646435437102938190283912471651865810514786470168818468143768714648";
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);
    }

    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_0500
 * @tc.name     If an invalid number is dialed, the DialCall() interface succeeds, but callId is not generated
 * @tc.desc     Function test
 * @tc.require: I5P2WO
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_0500, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "19!@#$%^&*:";
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1000 to do ...
 * @tc.name     make a normal phone call with card1, TYPE_VOICE, import phonynumber 10086
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1000, Function | MediumTest | Level0)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = PHONE_NUMBER; // OPERATOR PHONY NUMBER
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1100
 * @tc.name     make a normal phone call with card1, TYPE_VOICE, slot id was invalid
 * @tc.desc     Function test
 * @tc.require: I5P2WA
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1100, Function | MediumTest | Level0)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = PHONE_NUMBER; // OPERATOR PHONY NUMBER
    InitDialInfo(INVALID_SLOT_ID, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1200
 * @tc.name     make a normal phone call with card1, TYPE_VOICE, slot id out of count
 * @tc.desc     Function test
 * @tc.require: I5P2WA
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1200, Function | MediumTest | Level0)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = PHONE_NUMBER; // OPERATOR PHONY NUMBER
    int32_t slotId = SIM_SLOT_COUNT;        // out of the count
    InitDialInfo(slotId, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1300
 * @tc.name     make a normal phone call with card1, TYPE_VOICE, video state was invalid
 * @tc.desc     Function test
 * @tc.require: I5P2WA
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1300, Function | MediumTest | Level0)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = PHONE_NUMBER; // OPERATOR PHONY NUMBER
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, INVALID_VIDEO_STATE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, CALL_ERR_INVALID_VIDEO_STATE);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, INVALID_VIDEO_STATE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, CALL_ERR_INVALID_VIDEO_STATE);
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1400
 * @tc.name     make a normal phone call without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1400, Function | MediumTest | Level1)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "00000000000";
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);
    }

    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1500
 * @tc.name     make a normal phone call with error dial type
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1500, Function | MediumTest | Level1)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "00000000000";
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, INVALID_DIAL_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, CALL_ERR_UNKNOW_DIAL_TYPE);
    }

    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, INVALID_DIAL_TYPE);
        int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, CALL_ERR_UNKNOW_DIAL_TYPE);
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1600
 * @tc.name     make a normal phone call with card1, TYPE_VOICE, video state was invalid
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1600, Function | MediumTest | Level0)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = PHONE_NUMBER; // OPERATOR PHONY NUMBER
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, INVALID_VIDEO_STATE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::blueToothClientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, CALL_ERR_INVALID_VIDEO_STATE);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, INVALID_VIDEO_STATE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::blueToothClientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, CALL_ERR_INVALID_VIDEO_STATE);
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1700
 * @tc.name     make a normal phone call with telephone numbers is too long
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1700, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    std::string phoneNumber =
        "19119080646435437102938190283912471651865851478647016881846814376871464810514786470168818468143768714648";
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
            (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::blueToothClientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, TELEPHONY_ERR_ARGUMENT_INVALID);
    }

    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
            (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::blueToothClientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_EQ(ret, TELEPHONY_ERR_ARGUMENT_INVALID);
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1800
 * @tc.name     make a normal phone call with null telephone numbers,
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1800, Function | MediumTest | Level1)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "";
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    if (HasSimCard(SIM1_SLOTID)) {
        InitDialInfo(
            SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::blueToothClientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_NE(ret, TELEPHONY_ERR_SUCCESS);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        InitDialInfo(
            SIM2_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, DIAL_SCENE_TEST, (int32_t)DialType::DIAL_CARRIER_TYPE);
        int32_t ret = CallManagerGtest::blueToothClientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
        EXPECT_NE(ret, TELEPHONY_ERR_SUCCESS);
    }
}

/**
 * @tc.number   Telephony_CallManager_DialCall_1900
 * @tc.name     make a normal phone call with card1, TYPE_VOICE, slot id out of count
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DialCall_1900, Function | MediumTest | Level0)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = PHONE_NUMBER; // OPERATOR PHONY NUMBER
    int32_t slotId = SIM_SLOT_COUNT;        // out of the count
    InitDialInfo(slotId, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::blueToothClientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, CALL_ERR_INVALID_SLOT_ID);
}

/********************************************* Test AnswerCall() ***********************************************/
/**
 * @tc.number   Telephony_CallManager_AnswerCall_0100
 * @tc.name     test AnswerCall with the callId does not exist
 * @tc.desc     Function test
 * @tc.require: I5P2WA
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_AnswerCall_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callId = INVALID_NEGATIVE_ID;
    int32_t videoState = (int32_t)VideoStateType::TYPE_VOICE;
    EXPECT_NE(CallManagerGtest::clientPtr_->AnswerCall(callId, videoState), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_AnswerCall_0200
 * @tc.name     test AnswerCall with the videoState does not exist
 * @tc.desc     Function test
 * @tc.require: I5P2WA
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_AnswerCall_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callId = INVALID_POSITIVE_ID;
    int32_t videoState = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->AnswerCall(callId, videoState), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_AnswerCall_0300
 * @tc.name     test AnswerCall without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_AnswerCall_0300, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callId = INVALID_POSITIVE_ID;
    int32_t videoState = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->AnswerCall(callId, videoState), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_AnswerCall_0400
 * @tc.name     test AnswerCall with the callId does not exist
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_AnswerCall_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_NE(CallManagerGtest::blueToothClientPtr_->AnswerCall(), RETURN_VALUE_IS_ZERO);
}

/********************************************* Test RejectCall() ***********************************************/
/**
 * @tc.number   Telephony_CallManager_RejectCall_0100
 * @tc.name     test RejectCall with the callId does not exist
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_RejectCall_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callId = INVALID_NEGATIVE_ID;
    std::u16string textMessage = Str8ToStr16("this is a test message");
    EXPECT_NE(CallManagerGtest::clientPtr_->RejectCall(callId, true, textMessage), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_RejectCall_0200
 * @tc.name     test RejectCall without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_RejectCall_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callId = INVALID_NEGATIVE_ID;
    std::u16string textMessage = Str8ToStr16("this is a test message");
    EXPECT_NE(CallManagerGtest::clientPtr_->RejectCall(callId, true, textMessage), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_RejectCall_0300
 * @tc.name     test RejectCall with the callId does not exist
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_RejectCall_0300, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_NE(CallManagerGtest::blueToothClientPtr_->RejectCall(), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test HangUpCall() *********************************************/
/**
 * @tc.number   Telephony_CallManager_HangUpCall_0100
 * @tc.name     test disconnect call with wrong callId
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HangUpCall_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->HangUpCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_HangUpCall_0200  to do ...
 * @tc.name     test ring disconnect call after DialCall,
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HangUpCall_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->HangUpCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_HangUpCall_0300  to do ...
 * @tc.name     test ring disconnect call after DialCall without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HangUpCall_0300, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    InitDialInfo(
        0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->HangUpCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_HangUpCall_0400  to do ...
 * @tc.name     test ring disconnect call after DialCall
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HangUpCall_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_NE(CallManagerGtest::blueToothClientPtr_->HangUpCall(), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test GetCallState() *********************************************/
/**
 * @tc.number   Telephony_CallManager_GetCallState_0100
 * @tc.name     test GetCallState() without call
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallState_0100, Function | MediumTest | Level1)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallState(), (int32_t)CallStateToApp::CALL_STATE_IDLE);
}

/**
 * @tc.number   Telephony_CallManager_GetCallState_0200 to do ...
 * @tc.name     test GetCallState() after call
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallState_0200, Function | MediumTest | Level1)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallState(), (int32_t)CallStateToApp::CALL_STATE_IDLE);
}

/**
 * @tc.number   Telephony_CallManager_GetCallState_0300
 * @tc.name     test GetCallState()
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallState_0300, Function | MediumTest | Level1)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_EQ(CallManagerGtest::blueToothClientPtr_->GetCallState(), TELEPHONY_SUCCESS);
}

/******************************************* Test HoldCall() *********************************************/
/**
 * @tc.number   Telephony_CallManager_HoldCall_0100
 * @tc.name     Hold calls for non-existent call ID
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HoldCall_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->HoldCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_HoldCall_0200 to do ...
 * @tc.name     coming call test hold call, return non 0
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HoldCall_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->HoldCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_HoldCall_0600 to do ...
 * @tc.name     coming an active call test hold call, return 0
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HoldCall_0600, Function | MediumTest | Level0)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->HoldCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_HoldCall_0300 to do ...
 * @tc.name     coming call test hold call without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HoldCall_0300, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    InitDialInfo(
        0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->HoldCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_HoldCall_0400
 * @tc.name     Hold calls for non-existent call ID
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HoldCall_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_NE(CallManagerGtest::blueToothClientPtr_->HoldCall(), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test UnHoldCall() *********************************************/
/**
 * @tc.number   Telephony_CallManager_UnHoldCall_0100
 * @tc.name     Replies calls to a call ID that does not exist, return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_UnHoldCall_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->UnHoldCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_UnHoldCall_0200 to do ...
 * @tc.name     Passing in the suspended call ID, test UnHoldCall() return non 0
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_UnHoldCall_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->UnHoldCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_UnHoldCall_0600 to do ...
 * @tc.name     coming a active call , holding the call ,and test unhold call , return 0
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_UnHoldCall_0600, Function | MediumTest | Level0)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->UnHoldCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_UnHoldCall_0300
 * @tc.name     Replies calls to a call ID that does not exist without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_UnHoldCall_0300, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->UnHoldCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_UnHoldCall_0400
 * @tc.name     Replies calls to a call ID that does not exist, return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_UnHoldCall_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_NE(CallManagerGtest::blueToothClientPtr_->UnHoldCall(), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SwitchCall() *********************************************/
/**
 * @tc.number   Telephony_CallManager_SwitchCall_0100
 * @tc.name     Test returns 0 after switching call
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SwitchCall_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->SwitchCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SwitchCall_0200
 * @tc.name     Test returns 0 after switching call without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SwitchCall_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->SwitchCall(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SwitchCall_0300
 * @tc.name     Test returns 0 after switching call
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SwitchCall_0300, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_NE(CallManagerGtest::blueToothClientPtr_->SwitchCall(), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SwitchCall_0400
 * @tc.name     Test returns 0 after switching call, and DialCall(), return true
 *              wait for the correct status of the callback to execute correctly
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SwitchCall_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    std::string phoneNumber = "00000000000";
    int32_t callId = INVALID_NEGATIVE_ID;
    InitDialInfo(
        0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL, (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
    EXPECT_NE(CallManagerGtest::clientPtr_->SwitchCall(callId), RETURN_VALUE_IS_ZERO);
}

/********************************************* Test HasCall() ***********************************************/
/**
 * @tc.number   Telephony_CallManager_HasCall_0100
 * @tc.name     in CALL_STATE_IDLE status, than test Hascall(), return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HasCall_0100, Function | MediumTest | Level1)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callState = CallManagerGtest::clientPtr_->GetCallState();
    int32_t idleState = (int32_t)CallStateToApp::CALL_STATE_IDLE;
    ASSERT_EQ(callState, idleState);
    EXPECT_EQ(CallManagerGtest::clientPtr_->HasCall(), false);
}

/**
 * @tc.number   Telephony_CallManager_HasCall_0200 to do ...
 * @tc.name     The wrong number is not on call, return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HasCall_0200, Function | MediumTest | Level1)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    std::string phoneNumber = "";
    InitDialInfo(0, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_NE(ret, RETURN_VALUE_IS_ZERO);
    bool isRet = CallManagerGtest::clientPtr_->HasCall();
    EXPECT_NE(isRet, true);
}

/**
 * @tc.number   Telephony_CallManager_HasCall_0300 to do ...
 * @tc.name     Free time test Hascall(), return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HasCall_0300, Function | MediumTest | Level1)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    bool isRet = CallManagerGtest::clientPtr_->HasCall();
    EXPECT_NE(isRet, true);
}

/**
 * @tc.number   Telephony_CallManager_HasCall_0400
 * @tc.name     in CALL_STATE_IDLE status, than test Hascall(), return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HasCall_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callState = CallManagerGtest::clientPtr_->GetCallState();
    int32_t idleState = (int32_t)CallStateToApp::CALL_STATE_IDLE;
    ASSERT_EQ(callState, idleState);
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_EQ(CallManagerGtest::blueToothClientPtr_->HasCall(), false);
}

/********************************* Test IsNewCallAllowed() ***************************************/
/**
 * @tc.number   Telephony_CallManager_IsNewCallAllowed_0100
 * @tc.name     The call failed after making a call without the card
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsNewCallAllowed_0100, Function | MediumTest | Level1)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string phoneNumber = "00000000000";
    InitDialInfo(SIM1_SLOTID, (int32_t)VideoStateType::TYPE_VOICE, (int32_t)DialScene::CALL_NORMAL,
        (int32_t)DialType::DIAL_CARRIER_TYPE);
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    ASSERT_EQ(ret, RETURN_VALUE_IS_ZERO);
    CallInfoManager::LockCallState(false, (int32_t)TelCallState::CALL_STATUS_ACTIVE, SLEEP_50_MS, SLEEP_12000_MS);
    bool enabled = false;
    EXPECT_EQ(CallManagerGtest::clientPtr_->IsNewCallAllowed(enabled), TELEPHONY_SUCCESS);
    EXPECT_EQ(enabled, true);
}

/**
 * @tc.number   Telephony_CallManager_IsNewCallAllowed_0200
 * @tc.name     The call failed after making a call without the card
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsNewCallAllowed_0200, Function | MediumTest | Level1)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    bool enabled = false;
    EXPECT_EQ(CallManagerGtest::blueToothClientPtr_->IsNewCallAllowed(enabled), TELEPHONY_SUCCESS);
    EXPECT_EQ(enabled, true);
}

/********************************************* Test IsRinging() ***********************************************/
/**
 * @tc.number   Telephony_CallManager_IsRinging_0100
 * @tc.name     There is no call, return true
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsRinging_0100, Function | MediumTest | Level1)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callState = CallManagerGtest::clientPtr_->GetCallState();
    int32_t idleState = (int32_t)CallStateToApp::CALL_STATE_IDLE;
    ASSERT_EQ(callState, idleState);
    bool enabled = false;
    EXPECT_EQ(CallManagerGtest::clientPtr_->IsRinging(enabled), TELEPHONY_SUCCESS);
    EXPECT_NE(enabled, true);
}

/**
 * @tc.number   Telephony_CallManager_IsRinging_0200
 * @tc.name     There is no call, return true
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsRinging_0200, Function | MediumTest | Level1)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallInfoManager::LockCallState(false, (int32_t)CallStateToApp::CALL_STATE_IDLE, SLEEP_200_MS, SLEEP_30000_MS);
    int32_t callState = CallManagerGtest::clientPtr_->GetCallState();
    int32_t idleState = (int32_t)CallStateToApp::CALL_STATE_IDLE;
    ASSERT_EQ(callState, idleState);
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    bool enabled = false;
    EXPECT_EQ(CallManagerGtest::blueToothClientPtr_->IsRinging(enabled), TELEPHONY_SUCCESS);
    EXPECT_NE(enabled, true);
}

/***************************************** Test CombineConference() ********************************************/
/**
 * @tc.number   Telephony_CallManager_CombineConference_0100
 * @tc.name     Import callId "@&%￥", test CombineConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_CombineConference_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->CombineConference(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_CombineConference_0200
 * @tc.name     Import callId "100", test CombineConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_CombineConference_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->CombineConference(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_CombineConference_0300
 * @tc.name     Import callId "@&%￥", test CombineConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_CombineConference_0300, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_NE(CallManagerGtest::blueToothClientPtr_->CombineConference(), RETURN_VALUE_IS_ZERO);
}

/***************************************** Test SeparateConference() ********************************************/
/**
 * @tc.number   Telephony_CallManager_SeparateConference_0100
 * @tc.name     Import callId " -100", test SeparateConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SeparateConference_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->SeparateConference(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SeparateConference_0200
 * @tc.name     Import callId "100", test SeparateConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SeparateConference_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->SeparateConference(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SeparateConference_0300
 * @tc.name     Import callId " -100", test SeparateConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SeparateConference_0300, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_NE(CallManagerGtest::blueToothClientPtr_->SeparateConference(), RETURN_VALUE_IS_ZERO);
}

/********************************************* Test GetMainCallId() ***********************************************/
/**
 * @tc.number   Telephony_CallManager_GetMainCallId_0100
 * @tc.name     Import callId "abcd", test GetMainCallId(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetMainCallId_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    int32_t mainCallId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->GetMainCallId(callId, mainCallId), TELEPHONY_ERR_SUCCESS);
    EXPECT_NE(mainCallId, RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_GetMainCallId_0200
 * @tc.name     Import callId "100", test GetMainCallId(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetMainCallId_0200, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    int32_t mainCallId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->GetMainCallId(callId, mainCallId), TELEPHONY_ERR_SUCCESS);
    EXPECT_NE(mainCallId, RETURN_VALUE_IS_ZERO);
}

/***************************************** Test GetSubCallIdList() ******************************************/

/**
 * @tc.number   Telephony_CallManager_GetSubCallIdList_0100
 * @tc.name     Import callId "abcd", test GetSubCallIdList(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetSubCallIdList_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    std::vector<std::u16string> ans;
    ans.clear();
    int32_t result = CallManagerGtest::clientPtr_->GetSubCallIdList(callId, ans);
    EXPECT_NE(result, TELEPHONY_ERR_SUCCESS);
    bool isEmpty = ans.empty();
    EXPECT_EQ(isEmpty, true);
    if (!ans.empty()) {
        ans.clear();
    }
}

/**
 * @tc.number   Telephony_CallManager_GetSubCallIdList_0200
 * @tc.name     Import callId "100", test GetSubCallIdList(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetSubCallIdList_0200, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    std::vector<std::u16string> ans;
    ans.clear();
    int32_t result = CallManagerGtest::clientPtr_->GetSubCallIdList(callId, ans);
    EXPECT_NE(result, TELEPHONY_ERR_SUCCESS);
    bool isEmpty = ans.empty();
    EXPECT_EQ(isEmpty, true);
    if (!ans.empty()) {
        ans.clear();
    }
}

/************************************ Test GetCallIdListForConference() ***************************************/

/**
 * @tc.number   Telephony_CallManager_GetCallIdListForConference_0100
 * @tc.name     Import callId "abcd", test GetCallIdListForConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallIdListForConference_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    std::vector<std::u16string> ans;
    ans.clear();
    int32_t result = CallManagerGtest::clientPtr_->GetCallIdListForConference(callId, ans);
    EXPECT_NE(result, TELEPHONY_ERR_SUCCESS);
    bool isEmpty = ans.empty();
    EXPECT_EQ(isEmpty, true);
    if (!ans.empty()) {
        ans.clear();
    }
}

/**
 * @tc.number   Telephony_CallManager_GetCallIdListForConference_0200
 * @tc.name     Import callId "100", test GetCallIdListForConference(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallIdListForConference_0200, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    std::vector<std::u16string> ans;
    ans.clear();
    int32_t result = CallManagerGtest::clientPtr_->GetCallIdListForConference(callId, ans);
    EXPECT_NE(result, TELEPHONY_ERR_SUCCESS);
    bool isEmpty = ans.empty();
    EXPECT_EQ(isEmpty, true);
    if (!ans.empty()) {
        ans.clear();
    }
}
/************************************* Test IsEmergencyPhoneNumber() ***************************************/
/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0100
 * @tc.name     Call one phonynumber "0-0-0", test IsEmergencyPhoneNumber(), return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "0-0-0";
    std::u16string phoneNumber = Str8ToStr16(number);
    if (HasSimCard(SIM1_SLOTID)) {
        bool enabled = false;
        int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, SIM1_SLOTID, enabled);
        EXPECT_EQ(result, TELEPHONY_ERR_SUCCESS);
        EXPECT_NE(enabled, true);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        bool enabled = false;
        int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, SIM2_SLOTID, enabled);
        EXPECT_EQ(result, TELEPHONY_ERR_SUCCESS);
        EXPECT_NE(enabled, true);
    }
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0200
 * @tc.name     Call one phonynumber "112", test IsEmergencyPhoneNumber(), return true
 * @tc.desc     Function test
 * @tc.require: I5O1OQ
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "112";
    std::u16string phoneNumber = Str8ToStr16(number);
    if (HasSimCard(SIM1_SLOTID)) {
        bool enabled = false;
        int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, SIM1_SLOTID, enabled);
        EXPECT_EQ(result, TELEPHONY_ERR_SUCCESS);
        EXPECT_EQ(enabled, true);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        bool enabled = false;
        int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, SIM2_SLOTID, enabled);
        EXPECT_EQ(result, TELEPHONY_ERR_SUCCESS);
        EXPECT_EQ(enabled, true);
    }
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0300
 * @tc.name     Call one phonynumber "911", test IsEmergencyPhoneNumber(), return true
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0300, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "911";
    std::u16string phoneNumber = Str8ToStr16(number);
    if (HasSimCard(SIM1_SLOTID)) {
        bool enabled = false;
        int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, SIM1_SLOTID, enabled);
        EXPECT_EQ(result, TELEPHONY_ERR_SUCCESS);
        EXPECT_EQ(enabled, true);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        bool enabled = false;
        int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, SIM2_SLOTID, enabled);
        EXPECT_EQ(result, TELEPHONY_ERR_SUCCESS);
        EXPECT_EQ(enabled, true);
    }
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0400
 * @tc.name     Call one phonynumber "08", test IsEmergencyPhoneNumber(), return true
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0400, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t slotId = SIM1_SLOTID_NO_CARD;
    std::string number = "08";
    std::u16string phoneNumber = Str8ToStr16(number);
    bool enabled = false;
    int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, slotId, enabled);
    EXPECT_EQ(result, TELEPHONY_ERR_SUCCESS);
    EXPECT_EQ(enabled, true);
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0500
 * @tc.name     Call one phonynumber "118", test IsEmergencyPhoneNumber(), return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0500, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t slotId = SIM1_SLOTID_NO_CARD;
    std::string number = "118";
    std::u16string phoneNumber = Str8ToStr16(number);
    bool enabled = false;
    int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, slotId, enabled);
    EXPECT_EQ(result, TELEPHONY_ERR_SUCCESS);
    EXPECT_EQ(enabled, true);
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0600
 * @tc.name     Call one phonynumber "119", test IsEmergencyPhoneNumber(), return true
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0600, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "119";
    std::u16string phoneNumber = Str8ToStr16(number);
    if (HasSimCard(SIM1_SLOTID)) {
        bool enabled = false;
        int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, SIM1_SLOTID, enabled);
        EXPECT_EQ(result, TELEPHONY_ERR_SUCCESS);
        EXPECT_EQ(enabled, true);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        bool enabled = false;
        int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, SIM2_SLOTID, enabled);
        EXPECT_EQ(result, TELEPHONY_ERR_SUCCESS);
        EXPECT_EQ(enabled, true);
    }
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0700
 * @tc.name     Call one phonynumber "999", test IsEmergencyPhoneNumber(), return true
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0700, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t slotId = SIM1_SLOTID_NO_CARD;
    std::string number = "999";
    std::u16string phoneNumber = Str8ToStr16(number);
    bool enabled = false;
    int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, slotId, enabled);
    EXPECT_EQ(result, TELEPHONY_ERR_SUCCESS);
    EXPECT_EQ(enabled, true);
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0800
 * @tc.name     Call one phonynumber "", test IsEmergencyPhoneNumber(), return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0800, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t slotId = SIM1_SLOTID_NO_CARD;
    std::string number = "";
    std::u16string phoneNumber = Str8ToStr16(number);
    bool enabled = false;
    int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, slotId, enabled);
    EXPECT_NE(result, TELEPHONY_ERR_SUCCESS);
    EXPECT_NE(enabled, true);
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_0900
 * @tc.name     Call one phonynumber "", test IsEmergencyPhoneNumber(), return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_0900, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t slotId = SIM1_SLOTID_NO_CARD;
    std::string number = "@123";
    std::u16string phoneNumber = Str8ToStr16(number);
    bool enabled = false;
    int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, slotId, enabled);
    EXPECT_NE(result, TELEPHONY_ERR_SUCCESS);
    EXPECT_NE(enabled, true);
}

/**
 * @tc.number   Telephony_CallManager_IsEmergencyPhoneNumber_1000
 * @tc.name     Call one phonynumber "", test IsEmergencyPhoneNumber(), return false
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsEmergencyPhoneNumber_1000, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t slotId = SIM1_SLOTID_NO_CARD;
    std::string number = "+1+1+9";
    std::u16string phoneNumber = Str8ToStr16(number);
    bool enabled = false;
    int32_t result = CallManagerGtest::clientPtr_->IsEmergencyPhoneNumber(phoneNumber, slotId, enabled);
    EXPECT_NE(result, TELEPHONY_ERR_SUCCESS);
    EXPECT_NE(enabled, true);
}

/********************************************* Test GetCallWaiting() ***********************************************/

/**
 * @tc.number   Telephony_CallManager_GetCallWaiting_0100
 * @tc.name     Import slotId 1, test GetCallWaiting(), return 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallWaiting_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallWaiting(SIM1_SLOTID), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallWaiting(SIM2_SLOTID), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetCallWaiting_0200
 * @tc.name     test GetCallWaiting without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallWaiting_0200, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallWaiting(SIM1_SLOTID), TELEPHONY_ERR_PERMISSION_ERR);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallWaiting(SIM2_SLOTID), TELEPHONY_ERR_PERMISSION_ERR);
    }
}

/********************************************* Test StartDtmf() ***********************************************/

/**
 * @tc.number   Telephony_CallManager_StartDtmf_0100
 * @tc.name     Import callId abcd, test StartDtmf(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_StartDtmf_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    char str = '1';
    EXPECT_NE(CallManagerGtest::clientPtr_->StartDtmf(callId, str), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_StartDtmf_0200
 * @tc.name     Import callId 100, test StartDtmf(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_StartDtmf_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    char str = '1';
    EXPECT_NE(CallManagerGtest::clientPtr_->StartDtmf(callId, str), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_StartDtmf_0300
 * @tc.name     Import callId 100, test StartDtmf(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_StartDtmf_0300, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    char str = '1';
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_NE(CallManagerGtest::blueToothClientPtr_->StartDtmf(str), RETURN_VALUE_IS_ZERO);
}

/********************************************* Test StopDtmf() ***********************************************/

/**
 * @tc.number   Telephony_CallManager_StopDtmf_0100
 * @tc.name     Import callId abcd, test StopDtmf(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_StopDtmf_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_NEGATIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->StopDtmf(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_StopDtmf_0200
 * @tc.name     Import callId 100, test StopDtmf(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_StopDtmf_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = INVALID_POSITIVE_ID;
    EXPECT_NE(CallManagerGtest::clientPtr_->StopDtmf(callId), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_StopDtmf_0300
 * @tc.name     Import callId, test StopDtmf(), return non 0
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_StopDtmf_0300, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    EXPECT_NE(CallManagerGtest::blueToothClientPtr_->StopDtmf(), RETURN_VALUE_IS_ZERO);
}

/******************************** Test FormatPhoneNumber() * **************************************/

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumber_0100
 * @tc.name     Import phonyNumber 01085198748, test FormatPhoneNumber(), return 010-8519-8748
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumber_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "01085198748";
    std::string Code = "Kr";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(
        CallManagerGtest::clientPtr_->FormatPhoneNumber(phonyNumber, countryCode, formatNumber), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumber_0200
 * @tc.name     Import countryCode KR, test FormatPhoneNumber(), return 010-8519-8748
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumber_0200, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "010-8519-8748";
    std::string Code = "KR";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_NE(
        CallManagerGtest::clientPtr_->FormatPhoneNumber(phonyNumber, countryCode, formatNumber), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumber_0300
 * @tc.name     Import phonyNumber (03)38122111, test FormatPhoneNumber(), return 03-3812-2111
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumber_0300, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "(03)38122111";
    std::string Code = "JP";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(
        CallManagerGtest::clientPtr_->FormatPhoneNumber(phonyNumber, countryCode, formatNumber), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumber_0400
 * @tc.name     Import phonyNumber 13888888888, test FormatPhoneNumber(), return 138 8888 8888
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumber_0400, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "13888888888";
    std::string Code = "CN";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(
        CallManagerGtest::clientPtr_->FormatPhoneNumber(phonyNumber, countryCode, formatNumber), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumber_0500
 * @tc.name     Import phonyNumber +81338122111, test FormatPhoneNumber(), return 03-3812-2111
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumber_0500, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "+81338122111";
    std::string Code = "jp";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(
        CallManagerGtest::clientPtr_->FormatPhoneNumber(phonyNumber, countryCode, formatNumber), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumber_0600
 * @tc.name     Import phonyNumber 666666999999, test FormatPhoneNumber(), return 666666999999
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumber_0600, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "666666999999";
    std::string Code = "CN";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_NE(
        CallManagerGtest::clientPtr_->FormatPhoneNumber(phonyNumber, countryCode, formatNumber), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumber_0700
 * @tc.name     Import countryCode abcdefg, test FormatPhoneNumber(), return 83886082
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumber_0700, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "13888888888";
    std::string Code = "abcdefg";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_NE(
        CallManagerGtest::clientPtr_->FormatPhoneNumber(phonyNumber, countryCode, formatNumber), RETURN_VALUE_IS_ZERO);
}

/******************************* Test FormatPhoneNumberToE164() ***************************************/

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumberToE164_0100
 * @tc.name     Import phonyNumber 01085198748, test FormatPhoneNumberToE164(), return +821085198748
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "01085198748";
    std::string Code = "Kr";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(CallManagerGtest::clientPtr_->FormatPhoneNumberToE164(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumberToE164_0200
 * @tc.name     Import countryCode KR, test FormatPhoneNumberToE164(), return +81338122111
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "(03)38122111";
    std::string Code = "JP";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(CallManagerGtest::clientPtr_->FormatPhoneNumberToE164(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumberToE164_0300
 * @tc.name     Import phonyNumber 13888888888, test FormatPhoneNumberToE164(), return +8613888888888
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0300, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "13888888888";
    std::string Code = "cn";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(CallManagerGtest::clientPtr_->FormatPhoneNumberToE164(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumberToE164_0400
 * @tc.name     Import phonyNumber +81338122111, test FormatPhoneNumberToE164(), return +81338122111
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0400, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "+81338122111";
    std::string Code = "jp";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_NE(CallManagerGtest::clientPtr_->FormatPhoneNumberToE164(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumberToE164_0500
 * @tc.name     Import phonyNumber 03-3812-2111, test FormatPhoneNumberToE164(), return +81338122111
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0500, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "03-3812-2111";
    std::string Code = "JP";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_EQ(CallManagerGtest::clientPtr_->FormatPhoneNumberToE164(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumberToE164_0600
 * @tc.name     Import phonyNumber 666666999999, test FormatPhoneNumberToE164(), return 83886082
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0600, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "666666999999";
    std::string Code = "CN";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_NE(CallManagerGtest::clientPtr_->FormatPhoneNumberToE164(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumberToE164_0700
 * @tc.name     Import countryCode abcdefg, test FormatPhoneNumberToE164(), return 83886082
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0700, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "13888888888";
    std::string Code = "abcdefg";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_NE(CallManagerGtest::clientPtr_->FormatPhoneNumberToE164(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_FormatPhoneNumberToE164_0800
 * @tc.name     Import countryCode abcdefg, test FormatPhoneNumberToE164(), return CALL_ERR_PHONE_NUMBER_EMPTY
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_FormatPhoneNumberToE164_0800, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string number = "";
    std::string Code = "abcdefg";
    std::string formatBefore = "";
    std::u16string phonyNumber = Str8ToStr16(number);
    std::u16string countryCode = Str8ToStr16(Code);
    std::u16string formatNumber = Str8ToStr16(formatBefore);
    EXPECT_NE(CallManagerGtest::clientPtr_->FormatPhoneNumberToE164(phonyNumber, countryCode, formatNumber),
        RETURN_VALUE_IS_ZERO);
}

/********************************************* Test SetCallWaiting() ***********************************************/
/**
 * @tc.number   Telephony_CallManager_SetCallWaiting_0100
 * @tc.name     input slotId 0, test SetCallWaiting() enable callWaiting
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallWaiting_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(SIM1_SLOTID, true), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(SIM2_SLOTID, true), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallWaiting_0200
 * @tc.name     input invalid slotId, test SetCallWaiting() enable callWaiting
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallWaiting_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(INVALID_SLOT_ID, true), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_SetCallWaiting_0300
 * @tc.name     input slotId was out of count, test SetCallWaiting() enable callWaiting
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallWaiting_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(slotId, true), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_SetCallWaiting_0400
 * @tc.name     input slotId 0, test SetCallWaiting() disable callWaiting
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallWaiting_0400, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(SIM1_SLOTID, false), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(SIM2_SLOTID, false), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallWaiting_0500
 * @tc.name     input invalid slotId, test SetCallWaiting() disable callWaiting
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallWaiting_0500, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(INVALID_SLOT_ID, false), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_SetCallWaiting_0600
 * @tc.name     input slotId was out of count, test SetCallWaiting() enable callWaiting
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallWaiting_0600, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(slotId, false), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_SetCallWaiting_0700
 * @tc.name     test SetCallWaiting without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallWaiting_0700, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(SIM1_SLOTID, true), TELEPHONY_ERR_PERMISSION_ERR);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallWaiting(SIM2_SLOTID, true), TELEPHONY_ERR_PERMISSION_ERR);
    }
}

/******************************************* Test GetCallRestriction() ********************************************/
/**
 * @tc.number   Telephony_CallManager_GetCallRestriction_0100
 * @tc.name     input slotId 0, CallRestrictionType RESTRICTION_TYPE_ALL_INCOMING, test GetCallRestriction()
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallRestriction_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallRestriction(
                      SIM1_SLOTID, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING),
            RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallRestriction(
                      SIM2_SLOTID, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING),
            RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetCallRestriction_0200
 * @tc.name     input invalid slotId, CallRestrictionType RESTRICTION_TYPE_ALL_INCOMING, test GetCallRestriction()
 *              return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallRestriction_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallRestriction(
                  INVALID_SLOT_ID, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING),
        CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_GetCallRestriction_0300
 * @tc.name     input invalid was out of count, CallRestrictionType RESTRICTION_TYPE_ALL_INCOMING,
 *              test GetCallRestriction() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallRestriction_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(
        CallManagerGtest::clientPtr_->GetCallRestriction(slotId, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING),
        CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_GetCallRestriction_0400
 * @tc.name     input slotId 0, CallRestrictionType RESTRICTION_TYPE_ALL_OUTGOING, test GetCallRestriction()
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallRestriction_0400, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallRestriction(
                      SIM1_SLOTID, CallRestrictionType::RESTRICTION_TYPE_ALL_OUTGOING),
            RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallRestriction(
                      SIM2_SLOTID, CallRestrictionType::RESTRICTION_TYPE_ALL_OUTGOING),
            RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetCallRestriction_0500
 * @tc.name     input invalid slotId, CallRestrictionType RESTRICTION_TYPE_ALL_OUTGOING, test GetCallRestriction()
 *              return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallRestriction_0500, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallRestriction(
                  INVALID_SLOT_ID, CallRestrictionType::RESTRICTION_TYPE_ALL_OUTGOING),
        CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_GetCallRestriction_0600
 * @tc.name     input slotId was out of count, CallRestrictionType RESTRICTION_TYPE_ALL_OUTGOING,
 *              test GetCallRestriction() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallRestriction_0600, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(
        CallManagerGtest::clientPtr_->GetCallRestriction(slotId, CallRestrictionType::RESTRICTION_TYPE_ALL_OUTGOING),
        CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_GetCallRestriction_0700
 * @tc.name     input slotId 0, CallRestrictionType RESTRICTION_TYPE_ALL_INCOMING, test GetCallRestriction()
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallRestriction_0700, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallRestriction(
                      SIM1_SLOTID, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING),
            TELEPHONY_ERR_PERMISSION_ERR);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallRestriction(
                      SIM2_SLOTID, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING),
            TELEPHONY_ERR_PERMISSION_ERR);
    }
}

/******************************************* Test SetCallRestriction() ********************************************/
/**
 * @tc.number   Telephony_CallManager_SetCallRestriction_0100
 * @tc.name     input slotId 0, CallRestrictionType RESTRICTION_TYPE_ALL_OUTGOING,
 *              CallRestrictionMode::RESTRICTION_MODE_ACTIVATION,
 *              test SetCallRestriction()
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallRestriction_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallRestrictionInfo info;
    info.fac = CallRestrictionType::RESTRICTION_TYPE_ALL_OUTGOING;
    info.mode = CallRestrictionMode::RESTRICTION_MODE_ACTIVATION;
    if (strcpy_s(info.password, kMaxNumberLen + 1, "123") != EOK) {
        TELEPHONY_LOGE("strcpy_s fail.");
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestriction(SIM1_SLOTID, info), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestriction(SIM2_SLOTID, info), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallRestriction_0200
 * @tc.name     input invalid slotId, CallRestrictionType RESTRICTION_TYPE_ALL_OUTGOING,
 *              CallRestrictionMode::RESTRICTION_MODE_ACTIVATION,
 *              test SetCallRestriction() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallRestriction_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallRestrictionInfo info;
    info.fac = CallRestrictionType::RESTRICTION_TYPE_ALL_OUTGOING;
    info.mode = CallRestrictionMode::RESTRICTION_MODE_ACTIVATION;
    if (strcpy_s(info.password, kMaxNumberLen + 1, "123") != EOK) {
        TELEPHONY_LOGE("strcpy_s fail.");
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestriction(INVALID_SLOT_ID, info), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_SetCallRestriction_0300
 * @tc.name     input slotId was out of count, CallRestrictionType RESTRICTION_TYPE_ALL_OUTGOING,
 *              CallRestrictionMode::RESTRICTION_MODE_ACTIVATION,
 *              test SetCallRestriction() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallRestriction_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallRestrictionInfo info;
    info.fac = CallRestrictionType::RESTRICTION_TYPE_ALL_OUTGOING;
    info.mode = CallRestrictionMode::RESTRICTION_MODE_ACTIVATION;
    if (strcpy_s(info.password, kMaxNumberLen + 1, "123") != EOK) {
        TELEPHONY_LOGE("strcpy_s fail.");
        return;
    }

    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestriction(slotId, info), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_SetCallRestriction_0400
 * @tc.name     test SetCallRestriction() without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallRestriction_0400, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallRestrictionInfo info;
    info.fac = CallRestrictionType::RESTRICTION_TYPE_ALL_OUTGOING;
    info.mode = CallRestrictionMode::RESTRICTION_MODE_ACTIVATION;
    if (strcpy_s(info.password, kMaxNumberLen + 1, "123") != EOK) {
        TELEPHONY_LOGE("strcpy_s fail.");
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestriction(SIM1_SLOTID, info), TELEPHONY_ERR_PERMISSION_ERR);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallRestriction(SIM2_SLOTID, info), TELEPHONY_ERR_PERMISSION_ERR);
    }
}

/******************************************* Test GetCallTransferInfo() ********************************************/
/**
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_0100
 * @tc.name     input slotId 0, CallTransferType TRANSFER_TYPE_BUSY, test GetCallTransferInfo()
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(SIM1_SLOTID, CallTransferType::TRANSFER_TYPE_BUSY),
            RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(SIM1_SLOTID, CallTransferType::TRANSFER_TYPE_BUSY),
            RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_0200
 * @tc.name     input invalid slotId, CallTransferType TRANSFER_TYPE_BUSY, test GetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(INVALID_SLOT_ID, CallTransferType::TRANSFER_TYPE_BUSY),
        CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_0300
 * @tc.name     input slotId was out of count, CallTransferType TRANSFER_TYPE_BUSY,
 *              test GetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(slotId, CallTransferType::TRANSFER_TYPE_BUSY),
        CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_0400
 * @tc.name     input slotId 0, CallTransferType TRANSFER_TYPE_NO_REPLY, test GetCallTransferInfo()
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_0400, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(
            CallManagerGtest::clientPtr_->GetCallTransferInfo(SIM1_SLOTID, CallTransferType::TRANSFER_TYPE_NO_REPLY),
            RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(
            CallManagerGtest::clientPtr_->GetCallTransferInfo(SIM2_SLOTID, CallTransferType::TRANSFER_TYPE_NO_REPLY),
            RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_0500
 * @tc.name     input invalid slotId, CallTransferType TRANSFER_TYPE_NO_REPLY, test GetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_0500, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(
        CallManagerGtest::clientPtr_->GetCallTransferInfo(INVALID_SLOT_ID, CallTransferType::TRANSFER_TYPE_NO_REPLY),
        CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_0600
 * @tc.name     input slotId was out of count, CallTransferType TRANSFER_TYPE_NO_REPLY,
 *              test GetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_0600, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(slotId, CallTransferType::TRANSFER_TYPE_NO_REPLY),
        CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_0700
 * @tc.name     input slotId 0, CallTransferType TRANSFER_TYPE_NOT_REACHABLE, test GetCallTransferInfo()
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_0700, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(
                      SIM1_SLOTID, CallTransferType::TRANSFER_TYPE_NOT_REACHABLE),
            RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(
                      SIM2_SLOTID, CallTransferType::TRANSFER_TYPE_NOT_REACHABLE),
            RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_0800
 * @tc.name     input invalid slotId, CallTransferType TRANSFER_TYPE_NOT_REACHABLE, test GetCallTransferInfo() return
 *              failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_0800, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(
                  INVALID_SLOT_ID, CallTransferType::TRANSFER_TYPE_NOT_REACHABLE),
        CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_0900
 * @tc.name     input slotId out of count, CallTransferType TRANSFER_TYPE_NOT_REACHABLE,
 *              test GetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_0900, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(slotId, CallTransferType::TRANSFER_TYPE_NOT_REACHABLE),
        CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_1000
 * @tc.name     input slotId 0, CallTransferType invalid, test GetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_1000, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(SIM1_SLOTID, (CallTransferType)FALSE_DEFAULT),
            CALL_ERR_INVALID_TRANSFER_TYPE);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(SIM2_SLOTID, (CallTransferType)FALSE_DEFAULT),
            CALL_ERR_INVALID_TRANSFER_TYPE);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetCallTransferInfo_1100
 * @tc.name     test GetCallTransferInfo() without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetCallTransferInfo_1100, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(SIM1_SLOTID, CallTransferType::TRANSFER_TYPE_BUSY),
            TELEPHONY_ERR_PERMISSION_ERR);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetCallTransferInfo(SIM1_SLOTID, CallTransferType::TRANSFER_TYPE_BUSY),
            TELEPHONY_ERR_PERMISSION_ERR);
    }
}

/******************************************* Test SetCallTransferInfo() ********************************************/
/**
 * @tc.number   Telephony_CallManager_SetCallTransferInfo_0100
 * @tc.name     input slotId 0, CallTransferSettingType CALL_TRANSFER_ENABLE, CallTransferType TRANSFER_TYPE_BUSY,
 *              test SetCallTransferInfo()
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallTransferInfo_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallTransferInfo info;
    info.settingType = CallTransferSettingType::CALL_TRANSFER_ENABLE;
    info.type = CallTransferType::TRANSFER_TYPE_BUSY;
    if (strcpy_s(info.transferNum, kMaxNumberLen + 1, "123") != EOK) {
        TELEPHONY_LOGE("strcpy_s fail.");
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(SIM1_SLOTID, info), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(SIM2_SLOTID, info), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallTransferInfo_0200
 * @tc.name     input invalid slotId, CallTransferSettingType CALL_TRANSFER_ENABLE, CallTransferType TRANSFER_TYPE_BUSY,
 *              test SetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallTransferInfo_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallTransferInfo info;
    info.settingType = CallTransferSettingType::CALL_TRANSFER_ENABLE;
    info.type = CallTransferType::TRANSFER_TYPE_BUSY;
    if (strcpy_s(info.transferNum, kMaxNumberLen + 1, "123") != EOK) {
        TELEPHONY_LOGE("strcpy_s fail.");
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(INVALID_SLOT_ID, info), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_SetCallTransferInfo_0300
 * @tc.name     input slotId was out of count, CallTransferSettingType CALL_TRANSFER_ENABLE,
 *              CallTransferType TRANSFER_TYPE_BUSY, test SetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallTransferInfo_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallTransferInfo info;
    info.settingType = CallTransferSettingType::CALL_TRANSFER_ENABLE;
    info.type = CallTransferType::TRANSFER_TYPE_BUSY;
    if (strcpy_s(info.transferNum, kMaxNumberLen + 1, "123") != EOK) {
        TELEPHONY_LOGE("strcpy_s fail.");
        return;
    }

    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(slotId, info), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_SetCallTransferInfo_0400
 * @tc.name     input slotId 0, CallTransferSettingType CALL_TRANSFER_ENABLE, CallTransferType invalid,
 *              test SetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallTransferInfo_0400, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallTransferInfo info;
    info.settingType = CallTransferSettingType::CALL_TRANSFER_ENABLE;
    info.type = (CallTransferType)FALSE_DEFAULT;
    if (strcpy_s(info.transferNum, kMaxNumberLen + 1, "123") != EOK) {
        TELEPHONY_LOGE("strcpy_s fail.");
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(SIM1_SLOTID, info), CALL_ERR_INVALID_TRANSFER_TYPE);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(SIM2_SLOTID, info), CALL_ERR_INVALID_TRANSFER_TYPE);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallTransferInfo_0500
 * @tc.name     input slotId 0, CallTransferSettingType invalid, CallTransferType TRANSFER_TYPE_BUSY,
 *              test SetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallTransferInfo_0500, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallTransferInfo info;
    info.settingType = (CallTransferSettingType)FALSE_DEFAULT;
    info.type = CallTransferType::TRANSFER_TYPE_BUSY;
    if (strcpy_s(info.transferNum, kMaxNumberLen + 1, "123") != EOK) {
        TELEPHONY_LOGE("strcpy_s fail.");
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(SIM1_SLOTID, info),
            CALL_ERR_INVALID_TRANSFER_SETTING_TYPE);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(SIM2_SLOTID, info),
            CALL_ERR_INVALID_TRANSFER_SETTING_TYPE);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallTransferInfo_0600
 * @tc.name     input slotId 0, CallTransferSettingType invalid, CallTransferType TRANSFER_TYPE_BUSY, transferNum was
 *              too long, test SetCallTransferInfo() return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallTransferInfo_0600, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallTransferInfo info;
    info.settingType = CallTransferSettingType::CALL_TRANSFER_ENABLE;
    info.type = CallTransferType::TRANSFER_TYPE_BUSY;
    if (strcpy_s(info.transferNum, kMaxNumberLen + 2,
            "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901") !=
        EOK) {
        TELEPHONY_LOGE("strcpy_s fail.");
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(SIM1_SLOTID, info), CALL_ERR_NUMBER_OUT_OF_RANGE);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(SIM2_SLOTID, info), CALL_ERR_NUMBER_OUT_OF_RANGE);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallTransferInfo_0700
 * @tc.name     input slotId 0, CallTransferSettingType CALL_TRANSFER_ENABLE, CallTransferType TRANSFER_TYPE_BUSY,
 *              test SetCallTransferInfo()
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallTransferInfo_0700, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    CallTransferInfo info;
    info.settingType = CallTransferSettingType::CALL_TRANSFER_ENABLE;
    info.type = CallTransferType::TRANSFER_TYPE_BUSY;
    if (strcpy_s(info.transferNum, kMaxNumberLen + 1, "123") != EOK) {
        TELEPHONY_LOGE("strcpy_s fail.");
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(SIM1_SLOTID, info), TELEPHONY_ERR_PERMISSION_ERR);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallTransferInfo(SIM2_SLOTID, info), TELEPHONY_ERR_PERMISSION_ERR);
    }
}

#ifndef CALL_MANAGER_IMS_LITE_UNSUPPORT
/******************************************* Test EnableImsSwitch() ********************************************/
/**
 * @tc.number   Telephony_CallManager_EnableImsSwitch_0100
 * @tc.name     input slotId 0, test EnableImsSwitch
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_EnableImsSwitch_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->EnableImsSwitch(SIM1_SLOTID), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->EnableImsSwitch(SIM2_SLOTID), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_EnableImsSwitch_0200
 * @tc.name     input invalid slotId, test EnableImsSwitch return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_EnableImsSwitch_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->EnableImsSwitch(INVALID_SLOT_ID), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_EnableImsSwitch_0300
 * @tc.name     input slotId was out of count, test EnableImsSwitch return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_EnableImsSwitch_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(CallManagerGtest::clientPtr_->EnableImsSwitch(slotId), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_EnableImsSwitch_0400
 * @tc.name     test EnableImsSwitch without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_EnableImsSwitch_0400, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->EnableImsSwitch(SIM1_SLOTID), TELEPHONY_ERR_PERMISSION_ERR);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->EnableImsSwitch(SIM2_SLOTID), TELEPHONY_ERR_PERMISSION_ERR);
    }
}

/******************************************* Test DisableImsSwitch() ********************************************/
/**
 * @tc.number   Telephony_CallManager_DisableImsSwitch_0100
 * @tc.name     input slotId 0, test DisableImsSwitch
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DisableImsSwitch_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->DisableImsSwitch(SIM1_SLOTID), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->DisableImsSwitch(SIM2_SLOTID), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_DisableImsSwitch_0200
 * @tc.name     input invalid slotId, test DisableImsSwitch return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DisableImsSwitch_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->DisableImsSwitch(INVALID_SLOT_ID), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_DisableImsSwitch_0300
 * @tc.name     input slotId was out of count, test DisableImsSwitch return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_DisableImsSwitch_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    EXPECT_EQ(CallManagerGtest::clientPtr_->DisableImsSwitch(slotId), CALL_ERR_INVALID_SLOT_ID);
}

/******************************************* Test IsImsSwitchEnabled() ********************************************/
/**
 * @tc.number   Telephony_CallManager_IsImsSwitchEnabled_0100
 * @tc.name     input slotId 0, test IsImsSwitchEnabled
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsImsSwitchEnabled_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    bool enabled;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->IsImsSwitchEnabled(SIM1_SLOTID, enabled), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->IsImsSwitchEnabled(SIM2_SLOTID, enabled), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_IsImsSwitchEnabled_0200
 * @tc.name     input invalid slotId, test IsImsSwitchEnabled return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsImsSwitchEnabled_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    bool enabled;
    EXPECT_EQ(CallManagerGtest::clientPtr_->IsImsSwitchEnabled(INVALID_SLOT_ID, enabled), CALL_ERR_INVALID_SLOT_ID);
}

/**
 * @tc.number   Telephony_CallManager_IsImsSwitchEnabled_0300
 * @tc.name     input slotId was out of count, test IsImsSwitchEnabled return failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsImsSwitchEnabled_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t slotId = SIM_SLOT_COUNT; // out of the count
    bool enabled;
    EXPECT_EQ(CallManagerGtest::clientPtr_->IsImsSwitchEnabled(slotId, enabled), CALL_ERR_INVALID_SLOT_ID);
}

/******************************************* Test ControlCamera() *********************************************/
/**
 * @tc.number   Telephony_CallManager_ControlCamera_0100
 * @tc.name     input camera id lcam001, Test ControlCamera, returns 0 if success
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_ControlCamera_0100, Function | MediumTest | Level0)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    // 3516DV300 camera id string is "lcam001"
    std::string id = "lcam001";

    std::u16string cameraID = Str8ToStr16(id);
    EXPECT_EQ(CallManagerGtest::clientPtr_->ControlCamera(cameraID), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_ControlCamera_0200
 * @tc.name     input error camera id xtsautotest123456, Test ControlCamera, return err code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_ControlCamera_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string id = "xtsautotest123456";

    std::u16string cameraID = Str8ToStr16(id);
    EXPECT_NE(CallManagerGtest::clientPtr_->ControlCamera(cameraID), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_ControlCamera_0300
 * @tc.name     input error camera id xtsautotest123456, Test ControlCamera,
 *              then input empty camera id, Test ControlCamera
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_ControlCamera_0300, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string id = "lcam001";

    std::u16string cameraID = Str8ToStr16(id);
    EXPECT_EQ(CallManagerGtest::clientPtr_->ControlCamera(cameraID), RETURN_VALUE_IS_ZERO);

    id = "";
    cameraID = Str8ToStr16(id);
    EXPECT_EQ(CallManagerGtest::clientPtr_->ControlCamera(cameraID), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_ControlCamera_0600
 * @tc.name     input empty camera id, Test ControlCamera, returns error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_ControlCamera_0600, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string id = "";

    std::u16string cameraID = Str8ToStr16(id);
    EXPECT_NE(CallManagerGtest::clientPtr_->ControlCamera(cameraID), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SetPreviewWindow() *********************************************/
/**
 * @tc.number   Telephony_CallManager_SetPreviewWindow_0100
 * @tc.name     input x:0, y:0, z:-1, width:200, height:200, Test set preview window
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetPreviewWindow_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    VideoWindow window;
    window.x = 0;
    window.y = 0;
    window.z = -1;
    window.width = 200;
    window.height = 200;
    EXPECT_NE(CallManagerGtest::clientPtr_->SetPreviewWindow(window), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetPreviewWindow_0200
 * @tc.name     input x:0, y:0, z:0, width:200, height:200, Test set preview window
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetPreviewWindow_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    VideoWindow window;
    window.x = 0;
    window.y = 0;
    window.z = 0;
    window.width = 200;
    window.height = 200;
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetPreviewWindow(window), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SetDisplayWindow() *********************************************/
/**
 * @tc.number   Telephony_CallManager_SetDisplayWindow_0100
 * @tc.name     input x:10, y:10, z:10, width:100, height:200, Test set display window
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetDisplayWindow_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    VideoWindow window;
    window.x = 10;
    window.y = 10;
    window.z = 10;
    window.width = 100;
    window.height = 200;
    EXPECT_NE(CallManagerGtest::clientPtr_->SetDisplayWindow(window), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetDisplayWindow_0200
 * @tc.name     input x:10, y:10, z:0, width:200, height:200, Test set display window
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetDisplayWindow_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    VideoWindow window;
    window.x = 10;
    window.y = 10;
    window.z = 0;
    window.width = 200;
    window.height = 200;
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetDisplayWindow(window), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SetDeviceDirection() *********************************************/
/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0100
 * @tc.name     input invalid rotation, Test SetDeviceDirection, return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetDeviceDirection_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t rotation = CAMERA_ROTATION_ERROR;
    EXPECT_NE(CallManagerGtest::clientPtr_->SetDeviceDirection(rotation), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0200
 * @tc.name     input rotation 0, Test SetDeviceDirection, return 0 if success
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetDeviceDirection_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t rotation = 0;
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetDeviceDirection(rotation), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0300
 * @tc.name     input rotation 90, Test SetDeviceDirection
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetDeviceDirection_0300, Function | MediumTest | Level1)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t rotation = CAMERA_ROTATION_90;
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetDeviceDirection(rotation), RETURN_VALUE_IS_ZERO);
}

/******************************************* Test SetCameraZoom() *********************************************/
/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0100
 * @tc.name     input zoomRatio 5.0, Test SetCameraZoom
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCameraZoom_0100, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    float zoomRatio = 5.0;
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCameraZoom(zoomRatio), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0200
 * @tc.name     input zoomRatio 0.1, Test SetCameraZoom
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCameraZoom_0200, Function | MediumTest | Level1)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    float zoomRatio = 0.1;
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCameraZoom(zoomRatio), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0300
 * @tc.name     input zoomRatio 10.0, Test SetCameraZoom
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCameraZoom_0300, Function | MediumTest | Level1)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    float zoomRatio = 10.0;
    EXPECT_EQ(CallManagerGtest::clientPtr_->SetCameraZoom(zoomRatio), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0400
 * @tc.name     input invalid para zoomRatio 15.0, Test SetCameraZoom, return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCameraZoom_0400, Function | MediumTest | Level1)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    float zoomRatio = 15.0;
    EXPECT_NE(CallManagerGtest::clientPtr_->SetCameraZoom(zoomRatio), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0500
 * @tc.name     input invalid para zoomRatio 0.0, Test SetCameraZoom, return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCameraZoom_0500, Function | MediumTest | Level1)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    float zoomRatio = 0.0;
    EXPECT_NE(CallManagerGtest::clientPtr_->SetCameraZoom(zoomRatio), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0600
 * @tc.name     input invalid para zoomRatio -1.0, Test SetCameraZoom, return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCameraZoom_0600, Function | MediumTest | Level1)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    float zoomRatio = -1.0;
    EXPECT_NE(CallManagerGtest::clientPtr_->SetCameraZoom(zoomRatio), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetDeviceDirection_0700
 * @tc.name     input invalid para zoomRatio 10.1, Test SetCameraZoom, return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCameraZoom_0700, Function | MediumTest | Level1)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    float zoomRatio = 10.1;
    EXPECT_NE(CallManagerGtest::clientPtr_->SetCameraZoom(zoomRatio), RETURN_VALUE_IS_ZERO);
}
/******************************************* Test SetPausePicture() *********************************************/
/**
 * @tc.number   Telephony_CallManager_SetPausePicture_0100
 * @tc.name     input locale picture path /system/bin/test.png that does not exist, Test SetPausePicture,
 *              return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetPausePicture_0100, Function | MediumTest | Level1)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string pathTemp = "/system/bin/test.png";
    std::u16string path = Str8ToStr16(pathTemp);
    EXPECT_NE(CallManagerGtest::clientPtr_->SetPausePicture(path), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetPausePicture_0200
 * @tc.name     input an invalid image format file path, Test SetPausePicture, return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetPausePicture_0200, Function | MediumTest | Level1)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string pathTemp = "/system/bin/test.jpg";
    std::u16string path = Str8ToStr16(pathTemp);
    EXPECT_NE(CallManagerGtest::clientPtr_->SetPausePicture(path), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetPausePicture_0300
 * @tc.name     input an invalid image format file path, Test SetPausePicture, return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetPausePicture_0300, Function | MediumTest | Level1)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string pathTemp = "/system/bin/test.bmp";
    std::u16string path = Str8ToStr16(pathTemp);
    EXPECT_NE(CallManagerGtest::clientPtr_->SetPausePicture(path), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetPausePicture_0400
 * @tc.name     input an invalid image format file path, Test SetPausePicture, return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetPausePicture_0400, Function | MediumTest | Level1)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string pathTemp = "/system/bin/test.webp";
    std::u16string path = Str8ToStr16(pathTemp);
    EXPECT_NE(CallManagerGtest::clientPtr_->SetPausePicture(path), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetPausePicture_0500
 * @tc.name     input remote picture path http://callManager.jpg that does not exist, Test SetPausePicture,
 *              return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetPausePicture_0500, Function | MediumTest | Level1)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string pathTemp = "http://callManager.jpg";
    std::u16string path = Str8ToStr16(pathTemp);
    EXPECT_NE(CallManagerGtest::clientPtr_->SetPausePicture(path), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_SetPausePicture_0600
 * @tc.name     input empty picture path, Test SetPausePicture, return error code if failed
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetPausePicture_0600, Function | MediumTest | Level1)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::string pathTemp = "";
    std::u16string path = Str8ToStr16(pathTemp);
    EXPECT_NE(CallManagerGtest::clientPtr_->SetPausePicture(path), RETURN_VALUE_IS_ZERO);
}
#endif

/********************************************* Test SetAudioDevic()***********************************************/
/**
 * @tc.number   Telephony_CallManager_SetAudioDevice_0100
 * @tc.name     make a normal buletoothAddress, set active bluetooth device
 * @tc.desc     Function test
 * @tc.require: issueI5JUAQ
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetAudioDevice_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "00000000000";
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);

    HandsFreeAudioGateway *profile = HandsFreeAudioGateway::GetProfile();
    vector<BluetoothRemoteDevice> devices = profile->GetConnectedDevices();
    int deviceNum = devices.size();
    std::string address = "";
    const int32_t zeroDevice = 0;
    if (deviceNum > zeroDevice) {
        address = devices[0].GetDeviceAddr();
    }

    if (CallInfoManager::HasActiveStatus()) {
        EXPECT_EQ(clientPtr_->SetAudioDevice(AudioDevice::DEVICE_BLUETOOTH_SCO, address), RETURN_VALUE_IS_ZERO);
    }

    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
        EXPECT_EQ(CallInfoManager::HasState(newCallId_, (int32_t)TelCallState::CALL_STATUS_DISCONNECTED), true);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetAudioDevice_0200
 * @tc.name     make EARPIECE device type, set active EARPIECE device
 * @tc.desc     Function test
 * @tc.require: issueI5JUAQ
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetAudioDevice_0200, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "00000000000";
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
    std::string address = "";
    if (CallInfoManager::HasActiveStatus()) {
        EXPECT_EQ(clientPtr_->SetAudioDevice(AudioDevice::DEVICE_EARPIECE, address), RETURN_VALUE_IS_ZERO);
    }

    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
        EXPECT_EQ(CallInfoManager::HasState(newCallId_, (int32_t)TelCallState::CALL_STATUS_DISCONNECTED), true);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetAudioDevice_0300
 * @tc.name     make SPEAKER device type, set active SPEAKER device
 * @tc.desc     Function test
 * @tc.require: issueI5JUAQ
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetAudioDevice_0300, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "00000000000";
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);

    std::string address = "";
    if (CallInfoManager::HasActiveStatus()) {
        EXPECT_EQ(clientPtr_->SetAudioDevice(AudioDevice::DEVICE_SPEAKER, address), RETURN_VALUE_IS_ZERO);
    }

    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
        EXPECT_EQ(CallInfoManager::HasState(newCallId_, (int32_t)TelCallState::CALL_STATUS_DISCONNECTED), true);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetAudioDevice_0400
 * @tc.name     make DEVICE_WIRED_HEADSET device type, set active DEVICE_WIRED_HEADSET device
 * @tc.desc     Function test
 * @tc.require: issueI5JUAQ
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetAudioDevice_0400, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "00000000000";
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);

    std::string address = "";
    if (CallInfoManager::HasActiveStatus()) {
        EXPECT_EQ(clientPtr_->SetAudioDevice(AudioDevice::DEVICE_WIRED_HEADSET, address), RETURN_VALUE_IS_ZERO);
    }

    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
        EXPECT_EQ(CallInfoManager::HasState(newCallId_, (int32_t)TelCallState::CALL_STATUS_DISCONNECTED), true);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetAudioDevice_0500
 * @tc.name     make a empty buletoothAddress, set active bluetooth device
 * @tc.desc     Function test
 * @tc.require: issueI5JUAQ
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetAudioDevice_0500, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "00000000000";
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);

    std::string address = "";
    if (CallInfoManager::HasActiveStatus()) {
        EXPECT_EQ(clientPtr_->SetAudioDevice(AudioDevice::DEVICE_BLUETOOTH_SCO, address), RETURN_VALUE_IS_ZERO);
    }

    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
        EXPECT_EQ(CallInfoManager::HasState(newCallId_, (int32_t)TelCallState::CALL_STATUS_DISCONNECTED), true);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetAudioDevice_0600
 * @tc.name     make SPEAKER device type, set disable device
 * @tc.desc     Function test
 * @tc.require: issueI5JUAQ
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetAudioDevice_0600, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "00000000000";
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);

    std::string address = "";
    if (CallInfoManager::HasActiveStatus()) {
        EXPECT_EQ(clientPtr_->SetAudioDevice(AudioDevice::DEVICE_DISABLE, address), RETURN_VALUE_IS_ZERO);
    }

    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
        EXPECT_EQ(CallInfoManager::HasState(newCallId_, (int32_t)TelCallState::CALL_STATUS_DISCONNECTED), true);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetAudioDevice_0700
 * @tc.name     make SPEAKER device type, set unknown device
 * @tc.desc     Function test
 * @tc.require: issueI5JUAQ
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetAudioDevice_0700, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "00000000000";
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);

    std::string address = "";
    if (CallInfoManager::HasActiveStatus()) {
        EXPECT_EQ(clientPtr_->SetAudioDevice(AudioDevice::DEVICE_UNKNOWN, address), RETURN_VALUE_IS_ZERO);
    }

    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
        EXPECT_EQ(CallInfoManager::HasState(newCallId_, (int32_t)TelCallState::CALL_STATUS_DISCONNECTED), true);
    }
}

/********************************************* Test SetMuted()***********************************************/
/**
 * @tc.number   Telephony_CallManager_SetMuted_0100
 * @tc.name     set muted true
 * @tc.desc     Function test
 * @tc.require: issueI5K59I
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetMuted_0100, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "00000000000";
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
    bool muted = true;
    if (CallInfoManager::HasActiveStatus()) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetMuted(muted), RETURN_VALUE_IS_ZERO);
    }

    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
        EXPECT_EQ(CallInfoManager::HasState(newCallId_, (int32_t)TelCallState::CALL_STATUS_DISCONNECTED), true);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetMuted_0200
 * @tc.name     without call, set muted failed
 * @tc.desc     Function test
 * @tc.require: issueI5K59I
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetMuted_0200, Function | MediumTest | Level2)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    bool muted = true;

    EXPECT_EQ(CallManagerGtest::clientPtr_->SetMuted(muted), CALL_ERR_AUDIO_SETTING_MUTE_FAILED);
}

/**
 * @tc.number   Telephony_CallManager_SetMuted_0300
 * @tc.name     set muted true
 * @tc.desc     Function test
 * @tc.require: issueI5K59I
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetMuted_0300, Function | MediumTest | Level2)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::IsServiceConnected(), true);
    std::string phoneNumber = "00000000000";
    int32_t ret = CallManagerGtest::clientPtr_->DialCall(Str8ToStr16(phoneNumber), dialInfo_);
    EXPECT_EQ(ret, RETURN_VALUE_IS_ZERO);
    bool muted = false;
    if (CallInfoManager::HasActiveStatus()) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetMuted(muted), RETURN_VALUE_IS_ZERO);
    }

    if (clientPtr_->GetCallState() == static_cast<int>(CallStateToApp::CALL_STATE_OFFHOOK)) {
        HangUpCall();
        EXPECT_EQ(CallInfoManager::HasState(newCallId_, (int32_t)TelCallState::CALL_STATUS_DISCONNECTED), true);
    }
}

/********************************************* Test RegisterCallBack() ***********************************************/
/**
 * @tc.number   Telephony_CallManager_RegisterCallBack_0100
 * @tc.name     test register callback
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_RegisterCallBack_0100, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    std::unique_ptr<CallManagerCallback> callManagerCallbackPtr = std::make_unique<CallManagerCallBackStub>();
    if (callManagerCallbackPtr == nullptr) {
        return;
    }
    EXPECT_EQ(CallManagerGtest::clientPtr_->RegisterCallBack(std::move(callManagerCallbackPtr)), RETURN_VALUE_IS_ZERO);
}

/*************************************** Test UnRegisterCallBack() ********************************************/
/**
 * @tc.number   Telephony_CallManager_UnRegisterCallBack_0100
 * @tc.name     test register callback
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_UnRegisterCallBack_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->UnRegisterCallBack(), RETURN_VALUE_IS_ZERO);
}

/************************************ Test SetCallPreferenceMode() *****************************************/
/**
 * @tc.number   Telephony_CallManager_SetCallPreferenceMode_0100
 * @tc.name     test set call preference mode cs voice only
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallPreferenceMode_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t mode = CS_VOICE_ONLY;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallPreferenceMode(SIM1_SLOTID, mode), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallPreferenceMode(SIM2_SLOTID, mode), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallPreferenceMode_0200
 * @tc.name     test set call preference mode with error mode
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallPreferenceMode_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t mode = 0;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallPreferenceMode(SIM1_SLOTID, mode), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallPreferenceMode(SIM2_SLOTID, mode), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallPreferenceMode_0300
 * @tc.name     test set call preference mode with cs voice prefered
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallPreferenceMode_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t mode = CS_VOICE_PREFERRED;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallPreferenceMode(SIM1_SLOTID, mode), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallPreferenceMode(SIM2_SLOTID, mode), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallPreferenceMode_0400
 * @tc.name     test set call preference mode with ims ps voice prefered
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallPreferenceMode_0400, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t mode = IMS_PS_VOICE_PREFERRED;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallPreferenceMode(SIM1_SLOTID, mode), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallPreferenceMode(SIM2_SLOTID, mode), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetCallPreferenceMode_0500
 * @tc.name     test set call preference mode with ims ps voice only
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetCallPreferenceMode_0500, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t mode = IMS_PS_VOICE_ONLY;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallPreferenceMode(SIM1_SLOTID, mode), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetCallPreferenceMode(SIM2_SLOTID, mode), RETURN_VALUE_IS_ZERO);
    }
}

/*************************************** Test IsInEmergencyCall() **************************************/
/**
 * @tc.number   Telephony_CallManager_IsInEmergencyCall_0100
 * @tc.name     test is in emergency call
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsInEmergencyCall_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    bool enabled = false;
    EXPECT_EQ(CallManagerGtest::clientPtr_->IsInEmergencyCall(enabled), TELEPHONY_SUCCESS);
    EXPECT_EQ(enabled, false);
}

/**
 * @tc.number   Telephony_CallManager_IsInEmergencyCall_0200
 * @tc.name     test is in emergency call
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsInEmergencyCall_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    ASSERT_TRUE(blueToothClientPtr_ != nullptr);
    bool enabled = false;
    EXPECT_EQ(CallManagerGtest::blueToothClientPtr_->IsInEmergencyCall(enabled), TELEPHONY_SUCCESS);
    EXPECT_EQ(enabled, false);
}

/**
 * @tc.number   Telephony_CallManager_IsInEmergencyCall_0300
 * @tc.name     test is in emergency call without permission
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_IsInEmergencyCall_0300, Function | MediumTest | Level3)
{
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    bool enabled = false;
    EXPECT_NE(CallManagerGtest::clientPtr_->IsInEmergencyCall(enabled), TELEPHONY_SUCCESS);
    EXPECT_EQ(enabled, false);
}

/****************************************** Test MuteRinger() *******************************************/
/**
 * @tc.number   Telephony_CallManager_MuteRinger_0100
 * @tc.name     test mute ringer
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_MuteRinger_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->MuteRinger(), RETURN_VALUE_IS_ZERO);
}

/********************************************* Test GetImsConfig() ************************************************/
/**
 * @tc.number   Telephony_CallManager_GetImsConfig_0100
 * @tc.name     test get ims config item video quality
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetImsConfig_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetImsConfig(SIM1_SLOTID, ITEM_VIDEO_QUALITY), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetImsConfig(SIM2_SLOTID, ITEM_VIDEO_QUALITY), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetImsConfig_0200
 * @tc.name     test get ims config item ims switch status
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetImsConfig_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(
            CallManagerGtest::clientPtr_->GetImsConfig(SIM1_SLOTID, ITEM_IMS_SWITCH_STATUS), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(
            CallManagerGtest::clientPtr_->GetImsConfig(SIM2_SLOTID, ITEM_IMS_SWITCH_STATUS), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetImsConfig_0300
 * @tc.name     test get ims config item with invalid item
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetImsConfig_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    ImsConfigItem item = static_cast<ImsConfigItem>(2);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(
            CallManagerGtest::clientPtr_->GetImsConfig(SIM1_SLOTID, item), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(
            CallManagerGtest::clientPtr_->GetImsConfig(SIM2_SLOTID, item), RETURN_VALUE_IS_ZERO);
    }
}

/********************************************* Test SetImsConfig() ************************************************/
/**
 * @tc.number   Telephony_CallManager_SetImsConfig_0100
 * @tc.name     test set ims config item video quality
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetImsConfig_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    std::u16string value = u"123";
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(
            CallManagerGtest::clientPtr_->SetImsConfig(SIM1_SLOTID, ITEM_VIDEO_QUALITY, value), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(
            CallManagerGtest::clientPtr_->SetImsConfig(SIM2_SLOTID, ITEM_VIDEO_QUALITY, value), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetImsConfig_0200
 * @tc.name     test set ims config item ims switch status
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetImsConfig_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    std::u16string value = u"123";
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetImsConfig(SIM1_SLOTID, ITEM_IMS_SWITCH_STATUS, value),
            RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetImsConfig(SIM2_SLOTID, ITEM_IMS_SWITCH_STATUS, value),
            RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetImsConfig_0300
 * @tc.name     test set ims config item with invalid item
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetImsConfig_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    std::u16string value = u"123";
    ImsConfigItem item = static_cast<ImsConfigItem>(2);
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetImsConfig(SIM1_SLOTID, item, value),
            RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetImsConfig(SIM2_SLOTID, item, value),
            RETURN_VALUE_IS_ZERO);
    }
}

/*********************************** Test GetImsFeatureValue() ****************************************/
/**
 * @tc.number   Telephony_CallManager_GetImsFeatureValue_0100
 * @tc.name     test get ims feature value type voice over lte
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetImsFeatureValue_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(
            CallManagerGtest::clientPtr_->GetImsFeatureValue(SIM1_SLOTID, TYPE_VOICE_OVER_LTE), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(
            CallManagerGtest::clientPtr_->GetImsFeatureValue(SIM2_SLOTID, TYPE_VOICE_OVER_LTE), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetImsFeatureValue_0200
 * @tc.name     test get ims feature value type video over lte
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetImsFeatureValue_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(
            CallManagerGtest::clientPtr_->GetImsFeatureValue(SIM1_SLOTID, TYPE_VIDEO_OVER_LTE), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(
            CallManagerGtest::clientPtr_->GetImsFeatureValue(SIM2_SLOTID, TYPE_VIDEO_OVER_LTE), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetImsFeatureValue_0300
 * @tc.name     test get ims feature value type ss over ut
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetImsFeatureValue_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetImsFeatureValue(SIM1_SLOTID, TYPE_SS_OVER_UT), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetImsFeatureValue(SIM2_SLOTID, TYPE_SS_OVER_UT), RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_GetImsFeatureValue_0400
 * @tc.name     test get ims feature value with invalid type
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_GetImsFeatureValue_0400, Function | MediumTest | Level3)
{
    AccessToken token;
    FeatureType type = static_cast<FeatureType>(3);
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetImsFeatureValue(SIM1_SLOTID, type), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->GetImsFeatureValue(SIM2_SLOTID, type), RETURN_VALUE_IS_ZERO);
    }
}

/***************************************** Test SetImsFeatureValue() ***************************************/
/**
 * @tc.number   Telephony_CallManager_SetImsFeatureValue_0100
 * @tc.name     test set ims feature value type voice over lte
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetImsFeatureValue_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t value = 1;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetImsFeatureValue(SIM1_SLOTID, TYPE_VOICE_OVER_LTE, value),
            RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetImsFeatureValue(SIM2_SLOTID, TYPE_VOICE_OVER_LTE, value),
            RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetImsFeatureValue_0200
 * @tc.name     test set ims feature value type video over lte
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetImsFeatureValue_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t value = 1;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetImsFeatureValue(SIM1_SLOTID, TYPE_VIDEO_OVER_LTE, value),
            RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetImsFeatureValue(SIM2_SLOTID, TYPE_VIDEO_OVER_LTE, value),
            RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetImsFeatureValue_0300
 * @tc.name     test set ims feature value type ss over ut
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetImsFeatureValue_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t value = 1;
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetImsFeatureValue(SIM1_SLOTID, TYPE_SS_OVER_UT, value),
            RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetImsFeatureValue(SIM2_SLOTID, TYPE_SS_OVER_UT, value),
            RETURN_VALUE_IS_ZERO);
    }
}

/**
 * @tc.number   Telephony_CallManager_SetImsFeatureValue_0400
 * @tc.name     test set ims feature value with invalid type
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_SetImsFeatureValue_0400, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t value = 1;
    FeatureType type = static_cast<FeatureType>(3);
    if (HasSimCard(SIM1_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetImsFeatureValue(SIM1_SLOTID, type, value), RETURN_VALUE_IS_ZERO);
    }
    if (HasSimCard(SIM2_SLOTID)) {
        EXPECT_EQ(CallManagerGtest::clientPtr_->SetImsFeatureValue(SIM2_SLOTID, type, value), RETURN_VALUE_IS_ZERO);
    }
}

/************************************** Test UpdateImsCallMode() ****************************************/
/**
 * @tc.number   Telephony_CallManager_UpdateImsCallMode_0100
 * @tc.name     test update ims call mode audio only
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_UpdateImsCallMode_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t callId = 1;
    EXPECT_NE(CallManagerGtest::clientPtr_->UpdateImsCallMode(callId, CALL_MODE_AUDIO_ONLY), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_UpdateImsCallMode_0200
 * @tc.name     test update ims call mode send only
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_UpdateImsCallMode_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t callId = 1;
    EXPECT_NE(CallManagerGtest::clientPtr_->UpdateImsCallMode(callId, CALL_MODE_SEND_ONLY), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_UpdateImsCallMode_0300
 * @tc.name     test update ims call mode recerve only
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_UpdateImsCallMode_0300, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t callId = 1;
    EXPECT_NE(CallManagerGtest::clientPtr_->UpdateImsCallMode(callId, CALL_MODE_RECEIVE_ONLY), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_UpdateImsCallMode_0400
 * @tc.name     test update ims call mode send receive
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_UpdateImsCallMode_0400, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t callId = 1;
    EXPECT_NE(CallManagerGtest::clientPtr_->UpdateImsCallMode(callId, CALL_MODE_SEND_RECEIVE), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_UpdateImsCallMode_0500
 * @tc.name     test update ims call mode video paused
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_UpdateImsCallMode_0500, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t callId = 1;
    EXPECT_NE(CallManagerGtest::clientPtr_->UpdateImsCallMode(callId, CALL_MODE_VIDEO_PAUSED), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_UpdateImsCallMode_0600
 * @tc.name     test update ims call with invalid mode
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_UpdateImsCallMode_0600, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    int32_t callId = 1;
    ImsCallMode mode = static_cast<ImsCallMode>(5);
    EXPECT_NE(CallManagerGtest::clientPtr_->UpdateImsCallMode(callId, mode), RETURN_VALUE_IS_ZERO);
}

/********************************************* Test StartRtt() ************************************************/
/**
 * @tc.number   Telephony_CallManager_StartRtt_0100
 * @tc.name     test start rtt
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_StartRtt_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    std::u16string msg = u"123";
    int32_t callId = 1;
    EXPECT_NE(CallManagerGtest::clientPtr_->StartRtt(callId, msg), RETURN_VALUE_IS_ZERO);
}

/********************************************* Test StopRtt() ************************************************/
/**
 * @tc.number   Telephony_CallManager_StopRtt_0100
 * @tc.name     test stop rtt
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_StopRtt_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    EXPECT_NE(CallManagerGtest::clientPtr_->StopRtt(callId), RETURN_VALUE_IS_ZERO);
}

/********************************************* Test JoinConference() *********************************************/
/**
 * @tc.number   Telephony_CallManager_JoinConference_0100
 * @tc.name     test join conference
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_JoinConference_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    std::vector<std::u16string> numberList { u"0000000000" };
    EXPECT_NE(CallManagerGtest::clientPtr_->JoinConference(callId, numberList), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_JoinConference_0200
 * @tc.name     test join conference with empty numberList
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_JoinConference_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    int32_t callId = 1;
    std::vector<std::u16string> numberList;
    EXPECT_NE(CallManagerGtest::clientPtr_->JoinConference(callId, numberList), CALL_ERR_PHONE_NUMBER_EMPTY);
}

/*********************************** Test ReportOttCallDetailsInfo() ***********************************/
/**
 * @tc.number   Telephony_CallManager_ReportOttCallDetailsInfo_0100
 * @tc.name     test report ott call details info
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_ReportOttCallDetailsInfo_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    OttCallDetailsInfo info;
    const char *number = "000000";
    memcpy_s(info.phoneNum, kMaxNumberLen, number, strlen(number));
    const char *bundleName = "com.ohos.tddtest";
    memcpy_s(info.bundleName, kMaxNumberLen, bundleName, strlen(bundleName));
    info.callState = TelCallState::CALL_STATUS_DIALING;
    info.videoState = VideoStateType::TYPE_VOICE;
    std::vector<OttCallDetailsInfo> ottVec { info };
    EXPECT_EQ(CallManagerGtest::clientPtr_->ReportOttCallDetailsInfo(ottVec), RETURN_VALUE_IS_ZERO);
}

/**
 * @tc.number   Telephony_CallManager_ReportOttCallDetailsInfo_0200
 * @tc.name     test report ott call details info
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_ReportOttCallDetailsInfo_0200, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    std::vector<OttCallDetailsInfo> ottVec;
    EXPECT_NE(CallManagerGtest::clientPtr_->ReportOttCallDetailsInfo(ottVec), RETURN_VALUE_IS_ZERO);
}

/************************************* Test ReportOttCallEventInfo() ************************************/
/**
 * @tc.number   Telephony_CallManager_ReportOttCallEventInfo_0100
 * @tc.name     test report ott call details info
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_ReportOttCallEventInfo_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }
    OttCallEventInfo info;
    info.ottCallEventId = OttCallEventId::OTT_CALL_EVENT_FUNCTION_UNSUPPORTED;
    char bundlename[] = "com.ohos.tddtest";
    memcpy_s(info.bundleName, kMaxNumberLen, bundlename, strlen(bundlename));
    EXPECT_NE(CallManagerGtest::clientPtr_->ReportOttCallEventInfo(info), RETURN_VALUE_IS_ZERO);
}

/*********************************** Test HasVoiceCapability() ***************************************/
/**
 * @tc.number   Telephony_CallManager_HasVoiceCapability_0100
 * @tc.name     test report ott call details info
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_HasVoiceCapability_0100, Function | MediumTest | Level3)
{
    AccessToken token;
    if (!HasSimCard(SIM1_SLOTID) && !HasSimCard(SIM2_SLOTID)) {
        return;
    }

    EXPECT_EQ(CallManagerGtest::clientPtr_->HasVoiceCapability(), true);
}

/*********************************** Test Dump() ***************************************/
/**
 * @tc.number   Telephony_CallManager_TestDump_0100
 * @tc.name     Test Dump
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_TestDump_0100, Function | MediumTest | Level3)
{
    std::vector<std::u16string> emptyArgs = {};
    std::vector<std::u16string> args = { u"test", u"test1" };
    EXPECT_EQ(DelayedSingleton<CallManagerService>::GetInstance()->Dump(-1, args), TELEPHONY_ERR_ARGUMENT_INVALID);
    EXPECT_EQ(DelayedSingleton<CallManagerService>::GetInstance()->Dump(0, emptyArgs), 0);
    EXPECT_EQ(DelayedSingleton<CallManagerService>::GetInstance()->Dump(0, args), 0);
}

#else // TEL_TEST_UNSUPPORT
/**************************************** Test For Unsupported Platform **********************************************/
/**
 * @tc.number   Telephony_CallManager_Mock_0100
 * @tc.name     test for unsupported platform
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerGtest, Telephony_CallManager_Mock_0100, Function | MediumTest | Level3)
{
    EXPECT_TRUE(true);
}

#endif // TEL_TEST_UNSUPPORT
} // namespace Telephony
} // namespace OHOS
