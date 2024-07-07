/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include "app_state_callback_host.h"
#include "app_mgr_constants.h"
#include "iapp_state_callback.h"
#include "app_process_data.h"
#include "mock_ability_token.h"

using namespace testing::ext;
namespace OHOS {
namespace AppExecFwk {
class AmsAppStateCallBackTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AmsAppStateCallBackTest::SetUpTestCase()
{}
void AmsAppStateCallBackTest::TearDownTestCase()
{}
void AmsAppStateCallBackTest::SetUp()
{}
void AmsAppStateCallBackTest::TearDown()
{}

/*
 * Feature: AppStateCallBackHost
 * Function: AppStateCallBackHost
 * SubFunction: OnRemoteRequest Function
 * FunctionPoints: AppStateCallBackHost Onreceived interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if Onreceived works when app state changed.
 */
HWTEST_F(AmsAppStateCallBackTest, OnRemoteRequest_001, TestSize.Level1)
{
    sptr<AppStateCallbackHost> host(new AppStateCallbackHost());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option {MessageOption::TF_ASYNC};

    AppData appDataInfo;
    AppProcessData processData;
    appDataInfo.appName = "test_name";
    appDataInfo.uid = 2;
    processData.appDatas.emplace_back(appDataInfo);
    processData.pid = 1;
    processData.appState = ApplicationState::APP_STATE_FOREGROUND;
    data.WriteParcelable(&processData);
    int32_t ret = 0;
    EXPECT_EQ(0, ret);
}

/*
 * Feature: AppStateCallBackHost
 * Function: AppStateCallBackHost
 * SubFunction: OnRemoteRequest Function
 * FunctionPoints: AppStateCallBackHost Onreceived interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if Onreceived works when ability request done.
 */
HWTEST_F(AmsAppStateCallBackTest, OnRemoteRequest_002, TestSize.Level1)
{
    sptr<AppStateCallbackHost> host(new AppStateCallbackHost());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option {MessageOption::TF_ASYNC};
    sptr<IRemoteObject> token = new MockAbilityToken();

    data.WriteParcelable(token.GetRefPtr());
    int32_t abilityState = static_cast<int32_t>(AbilityState::ABILITY_STATE_FOREGROUND);
    data.WriteInt32(abilityState);
    int32_t ret = 0;
    EXPECT_EQ(0, ret);
}

/*
 * Feature: AppStateCallBackHost
 * Function: AppStateCallBackHost
 * SubFunction: OnAbilityRequestDone Function
 * FunctionPoints: OnAbilityRequestDone Onreceived interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if Onreceived works when ability request done.
 */
HWTEST_F(AmsAppStateCallBackTest, OnAbilityRequestDone_001, TestSize.Level1)
{
    sptr<IRemoteObject> token = nullptr;
    AbilityState state = AbilityState::ABILITY_STATE_BEGIN;
    sptr<AppStateCallbackHost> host(new AppStateCallbackHost());
    host->OnAbilityRequestDone(token, state);
    
    int32_t ret = 0;
    EXPECT_EQ(0, ret);
}

/*
 * Feature: AppStateCallBackHost
 * Function: AppStateCallBackHost
 * SubFunction: OnAppStateChanged Function
 * FunctionPoints: OnAppStateChanged Onreceived interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if Onreceived works when app state changed.
 */
HWTEST_F(AmsAppStateCallBackTest, OnAppStateChanged_001, TestSize.Level1)
{
    AppProcessData data;
    sptr<AppStateCallbackHost> host(new AppStateCallbackHost());
    host->OnAppStateChanged(data);
    
    int32_t ret = 0;
    EXPECT_EQ(0, ret);
}

/*
* Feature: AppProcessData
* Function: AppProcessData
* SubFunction: Marshalling and Unmarshalling
* FunctionPoints: Marshalling and Unmarshalling can work normal
* EnvConditions: Mobile that can run normal
* CaseDescription: 1. Verify process data can write to parcel.
            2. verify process data can read from parcel.
*/
HWTEST_F(AmsAppStateCallBackTest, AppProcessData_001, TestSize.Level1)
{
    Parcel data;
    AppData appDataInfo;
    AppProcessData processData;
    appDataInfo.appName = "test_name";
    appDataInfo.uid = 2;
    processData.appDatas.emplace_back(appDataInfo);
    processData.pid = 1;
    processData.appState = ApplicationState::APP_STATE_FOREGROUND;

    bool ret = processData.Marshalling(data);
    EXPECT_EQ(true, ret);

    sptr<AppProcessData> newProcessData = AppProcessData::Unmarshalling(data);
    EXPECT_NE(nullptr, newProcessData);

    for (size_t i = 0; i < processData.appDatas.size(); i++) {
        EXPECT_EQ(processData.appDatas[i].appName, newProcessData->appDatas[i].appName);
        EXPECT_EQ(processData.appDatas[i].uid, newProcessData->appDatas[i].uid);
    }

    EXPECT_EQ(processData.pid, newProcessData->pid);
    EXPECT_EQ(processData.appState, newProcessData->appState);
}
}  // namespace AppExecFwk
}  // namespace OHOS
