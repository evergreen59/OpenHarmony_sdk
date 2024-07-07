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

#include "passive_ability_test.h"

#include "accesstoken_kit.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "nativetoken_kit.h"
#include "system_ability_definition.h"
#include "token_setproc.h"

#include "common_utils.h"
#include "constant_definition.h"
#include "location_dumper.h"

using namespace testing::ext;

namespace OHOS {
namespace Location {
const int32_t LOCATION_PERM_NUM = 4;
const std::string ARGS_HELP = "-h";
void PassiveAbilityTest::SetUp()
{
    /*
     * @tc.setup: Get system ability's pointer and get sa proxy object.
     */
    MockNativePermission();
    ability_ = new (std::nothrow) PassiveAbility();
    EXPECT_NE(nullptr, ability_);
    proxy_ = new (std::nothrow) PassiveAbilityProxy(ability_);
    EXPECT_NE(nullptr, proxy_);
}

void PassiveAbilityTest::TearDown()
{
    /*
     * @tc.teardown: release memory.
     */
    proxy_ = nullptr;
}

void PassiveAbilityTest::MockNativePermission()
{
    const char *perms[] = {
        ACCESS_LOCATION.c_str(), ACCESS_APPROXIMATELY_LOCATION.c_str(),
        ACCESS_BACKGROUND_LOCATION.c_str(), MANAGE_SECURE_SETTINGS.c_str(),
    };
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = LOCATION_PERM_NUM,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "PassiveAbilityTest",
        .aplStr = "system_basic",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

/*
 * @tc.name: SendLocationRequest001
 * @tc.desc: Build Request, marshall and unmarshall data Then Send it
 * @tc.type: FUNC
 */
HWTEST_F(PassiveAbilityTest, SendLocationRequest001, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "PassiveAbilityStubTest, SendLocationRequest001, TestSize.Level1";
    LBSLOGI(PASSIVE_TEST, "[PassiveAbilityStubTest] SendLocationRequest001 begin");
    /*
     * @tc.steps: step1. build location request data.
     */
    std::unique_ptr<WorkRecord> workRecord = std::make_unique<WorkRecord>();
    int num = 2;
    for (int i = 0; i < num; i++) {
        int uid = i + 1;
        int pid = i + 2;
        int timeInterval = i;
        std::string name = "nameForTest";
        std::string uuid = std::to_string(CommonUtils::IntRandom(MIN_INT_RANDOM, MAX_INT_RANDOM));
        workRecord->Add(uid, pid, name, timeInterval, uuid);
    }
    /*
     * @tc.steps: step2. send location request
     * @tc.expected: step2. no exception happens.
     */
    EXPECT_EQ(ERRCODE_SUCCESS, proxy_->SendLocationRequest(*workRecord));
    LBSLOGI(PASSIVE_TEST, "[PassiveAbilityStubTest] SendLocationRequest001 begin");
}

/*
 * @tc.name: SetEnableAndDisable001
 * @tc.desc: Test disable and enable system ability
 * @tc.type: FUNC
 */
HWTEST_F(PassiveAbilityTest, SetEnableAndDisable001, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "PassiveAbilityStubTest, SetEnableAndDisable001, TestSize.Level1";
    LBSLOGI(PASSIVE_TEST, "[PassiveAbilityStubTest] SetEnableAndDisable001 begin");
    /*
     * @tc.steps: step1.remove SA
     * @tc.expected: step1. object1 is null.
     */
    EXPECT_EQ(ERRCODE_SUCCESS, proxy_->SetEnable(false)); // if the state is false
    sptr<ISystemAbilityManager> systemAbilityManager1 =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> object1 = systemAbilityManager1->GetSystemAbility(LOCATION_NOPOWER_LOCATING_SA_ID);
    EXPECT_EQ(nullptr, object1); // no SA can be given

    /*
     * @tc.steps: step2. test enable SA
     * @tc.expected: step2. object2 is not null.
     */
    EXPECT_EQ(ERRCODE_SUCCESS, proxy_->SetEnable(true)); // if the state is true
    sptr<ISystemAbilityManager> systemAbilityManager2 =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> object2 = systemAbilityManager2->GetSystemAbility(LOCATION_NOPOWER_LOCATING_SA_ID);
    EXPECT_NE(nullptr, object2); // SA can be given
    LBSLOGI(PASSIVE_TEST, "[PassiveAbilityStubTest] SetEnableAndDisable001 end");
}

HWTEST_F(PassiveAbilityTest, PassiveLocationMock001, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "PassiveAbilityStubTest, PassiveLocationMock001, TestSize.Level1";
    LBSLOGI(PASSIVE_TEST, "[PassiveAbilityStubTest] PassiveLocationMock001 begin");
    int timeInterval = 1;
    std::vector<std::shared_ptr<Location>> locations;
    EXPECT_EQ(ERRCODE_SUCCESS, proxy_->EnableMock());
    EXPECT_EQ(ERRCODE_SUCCESS, proxy_->SetMocked(timeInterval, locations));
    
    EXPECT_EQ(ERRCODE_SUCCESS, proxy_->DisableMock());
    EXPECT_EQ(ERRCODE_NOT_SUPPORTED, proxy_->SetMocked(timeInterval, locations));
    LBSLOGI(PASSIVE_TEST, "[PassiveAbilityStubTest] PassiveLocationMock001 end");
}

HWTEST_F(PassiveAbilityTest, PassiveOnStartAndOnStop001, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "PassiveAbilityStubTest, PassiveOnStartAndOnStop001, TestSize.Level1";
    LBSLOGI(PASSIVE_TEST, "[PassiveAbilityStubTest] PassiveOnStartAndOnStop001 begin");
    ability_->OnStart(); // start ability
    EXPECT_EQ(ServiceRunningState::STATE_RUNNING, ability_->QueryServiceState());
    ability_->OnStart(); // start ability again
    EXPECT_EQ(ServiceRunningState::STATE_RUNNING, ability_->QueryServiceState());

    ability_->OnStop(); // stop ability
    EXPECT_EQ(ServiceRunningState::STATE_NOT_START, ability_->QueryServiceState());
    ability_->OnStart(); // restart ability
    EXPECT_EQ(ServiceRunningState::STATE_RUNNING, ability_->QueryServiceState());
    LBSLOGI(PASSIVE_TEST, "[PassiveAbilityStubTest] PassiveOnStartAndOnStop001 end");
}

HWTEST_F(PassiveAbilityTest, PassiveDump001, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "PassiveAbilityStubTest, PassiveDump001, TestSize.Level1";
    LBSLOGI(PASSIVE_TEST, "[PassiveAbilityStubTest] PassiveDump001 begin");
    int32_t fd = 0;
    std::vector<std::u16string> args;
    std::u16string arg1 = Str8ToStr16("arg1");
    args.emplace_back(arg1);
    std::u16string arg2 = Str8ToStr16("arg2");
    args.emplace_back(arg2);
    std::u16string arg3 = Str8ToStr16("arg3");
    args.emplace_back(arg3);
    std::u16string arg4 = Str8ToStr16("arg4");
    args.emplace_back(arg4);
    EXPECT_EQ(ERR_OK, ability_->Dump(fd, args));

    std::vector<std::u16string> emptyArgs;
    EXPECT_EQ(ERR_OK, ability_->Dump(fd, emptyArgs));

    std::vector<std::u16string> helpArgs;
    std::u16string helpArg1 = Str8ToStr16(ARGS_HELP);
    helpArgs.emplace_back(helpArg1);
    EXPECT_EQ(ERR_OK, ability_->Dump(fd, helpArgs));
    LBSLOGI(PASSIVE_TEST, "[PassiveAbilityStubTest] PassiveDump001 end");
}

HWTEST_F(PassiveAbilityTest, PassiveSendReportMockLocationEvent001, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "PassiveAbilityTest, PassiveSendReportMockLocationEvent001, TestSize.Level1";
    LBSLOGI(PASSIVE_TEST, "[PassiveAbilityTest] PassiveSendReportMockLocationEvent001 begin");
    ability_->SendReportMockLocationEvent(); // clear location mock

    int timeInterval = 0;
    std::vector<std::shared_ptr<Location>> locations;
    Parcel parcel;
    parcel.WriteDouble(10.6); // latitude
    parcel.WriteDouble(10.5); // longitude
    parcel.WriteDouble(10.4); // altitude
    parcel.WriteDouble(1.0); // accuracy
    parcel.WriteDouble(5.0); // speed
    parcel.WriteDouble(10); // direction
    parcel.WriteInt64(1611000000); // timestamp
    parcel.WriteInt64(1611000000); // time since boot
    parcel.WriteString16(u"additions"); // additions
    parcel.WriteInt64(1); // additionSize
    parcel.WriteBool(true); // isFromMock is true
    parcel.WriteInt32(1); // source type
    parcel.WriteInt32(0); // floor no.
    parcel.WriteDouble(1000.0); // floor acc
    locations.push_back(Location::UnmarshallingShared(parcel));
    EXPECT_EQ(ERRCODE_SUCCESS, ability_->EnableMock());
    EXPECT_EQ(ERRCODE_SUCCESS, ability_->SetMocked(timeInterval, locations));
    sleep(2);
    LBSLOGI(PASSIVE_TEST, "[PassiveAbilityTest] PassiveSendReportMockLocationEvent001 end");
}

HWTEST_F(PassiveAbilityTest, PassiveSendReportMockLocationEvent002, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "PassiveAbilityTest, PassiveSendReportMockLocationEvent002, TestSize.Level1";
    LBSLOGI(PASSIVE_TEST, "[PassiveAbilityTest] PassiveSendReportMockLocationEvent002 begin");
    ability_->SendReportMockLocationEvent(); // clear location mock

    int timeInterval = 0;
    std::vector<std::shared_ptr<Location>> locations;
    Parcel parcel;
    parcel.WriteDouble(10.6); // latitude
    parcel.WriteDouble(10.5); // longitude
    parcel.WriteDouble(10.4); // altitude
    parcel.WriteDouble(1.0); // accuracy
    parcel.WriteDouble(5.0); // speed
    parcel.WriteDouble(10); // direction
    parcel.WriteInt64(1611000000); // timestamp
    parcel.WriteInt64(1611000000); // time since boot
    parcel.WriteString16(u"additions"); // additions
    parcel.WriteInt64(1); // additionSize
    parcel.WriteBool(false); // isFromMock is true
    parcel.WriteInt32(1); // source type
    parcel.WriteInt32(0); // floor no.
    parcel.WriteDouble(1000.0); // floor acc
    locations.push_back(Location::UnmarshallingShared(parcel));
    EXPECT_EQ(ERRCODE_SUCCESS, ability_->EnableMock());
    EXPECT_EQ(ERRCODE_SUCCESS, ability_->SetMocked(timeInterval, locations));
    sleep(2);
    LBSLOGI(PASSIVE_TEST, "[PassiveAbilityTest] PassiveSendReportMockLocationEvent002 end");
}
} // namespace Location
} // namespace OHOS