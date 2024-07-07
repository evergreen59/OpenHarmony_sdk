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

#include "app_scheduler_proxy.h"
#include <unistd.h>
#include <gtest/gtest.h>
#include "errors.h"
#include "ipc_types.h"
#include "app_mgr_proxy.h"
#include "app_record_id.h"
#include "hilog_wrapper.h"
#include "mock_application.h"
#include "mock_app_mgr_service.h"
#include "application_state_observer_stub.h"

using namespace testing::ext;

using OHOS::iface_cast;
using OHOS::sptr;
using testing::_;
using testing::Invoke;
using testing::InvokeWithoutArgs;
using testing::Return;

namespace OHOS {
namespace AppExecFwk {
class AmsIpcAppMgrInterfaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AmsIpcAppMgrInterfaceTest::SetUpTestCase()
{}

void AmsIpcAppMgrInterfaceTest::TearDownTestCase()
{}

void AmsIpcAppMgrInterfaceTest::SetUp()
{}

void AmsIpcAppMgrInterfaceTest::TearDown()
{}

/*
 * Feature: AMS
 * Function: IPC
 * SubFunction: appmgr interface
 * FunctionPoints: interface
 * CaseDescription: test interface of AttachApplication
 */
HWTEST_F(AmsIpcAppMgrInterfaceTest, Interface_001, TestSize.Level1)
{
    HILOG_DEBUG("AppMgrIpcInterfaceTest_AppMgr_001 start");
    sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);
    sptr<MockApplication> app(new MockApplication());

    EXPECT_CALL(*mockAppMgr, AttachApplication(_))
        .WillOnce(InvokeWithoutArgs(mockAppMgr.GetRefPtr(), &MockAppMgrService::Post));
    appMgrClient->AttachApplication(app);
    mockAppMgr->Wait();
    HILOG_DEBUG("AppMgrIpcInterfaceTest_AppMgr_001 end");
}

/*
 * Feature: AMS
 * Function: IPC
 * SubFunction: appmgr interface
 * FunctionPoints: interface
 * CaseDescription: test interface of ApplicationForegrounded
 */
HWTEST_F(AmsIpcAppMgrInterfaceTest, Interface_002, TestSize.Level1)
{
    HILOG_DEBUG("AppMgrIpcInterfaceTest_AppMgr_002 start");
    sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);

    EXPECT_CALL(*mockAppMgr, ApplicationForegrounded(_))
        .WillOnce(InvokeWithoutArgs(mockAppMgr.GetRefPtr(), &MockAppMgrService::Post));
    auto recordId = AppRecordId::Create();
    appMgrClient->ApplicationForegrounded(recordId);
    mockAppMgr->Wait();
    HILOG_DEBUG("AppMgrIpcInterfaceTest_AppMgr_002 end");
}

/*
 * Feature: AMS
 * Function: IPC
 * SubFunction: appmgr interface
 * FunctionPoints: interface
 * CaseDescription: test interface of ApplicationBackgrounded
 */
HWTEST_F(AmsIpcAppMgrInterfaceTest, Interface_003, TestSize.Level1)
{
    HILOG_DEBUG("AppMgrIpcInterfaceTest_AppMgr_003 start");
    sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);

    EXPECT_CALL(*mockAppMgr, ApplicationBackgrounded(_))
        .WillOnce(InvokeWithoutArgs(mockAppMgr.GetRefPtr(), &MockAppMgrService::Post));
    auto recordId = AppRecordId::Create();
    appMgrClient->ApplicationBackgrounded(recordId);
    mockAppMgr->Wait();
    HILOG_DEBUG("AppMgrIpcInterfaceTest_AppMgr_003 end");
}

/*
 * Feature: AMS
 * Function: IPC
 * SubFunction: appmgr interface
 * FunctionPoints: interface
 * CaseDescription: test interface of ApplicationTerminated
 */
HWTEST_F(AmsIpcAppMgrInterfaceTest, Interface_004, TestSize.Level1)
{
    HILOG_DEBUG("AppMgrIpcInterfaceTest_AppMgr_004 start");
    sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);

    EXPECT_CALL(*mockAppMgr, ApplicationTerminated(_))
        .WillOnce(InvokeWithoutArgs(mockAppMgr.GetRefPtr(), &MockAppMgrService::Post));
    auto recordId = AppRecordId::Create();
    appMgrClient->ApplicationTerminated(recordId);
    mockAppMgr->Wait();
    HILOG_DEBUG("AppMgrIpcInterfaceTest_AppMgr_004 end");
}

/*
 * Feature: AMS
 * Function: IPC
 * SubFunction: appmgr interface
 * FunctionPoints: interface
 * CaseDescription: test interface of CheckPermission
 */
HWTEST_F(AmsIpcAppMgrInterfaceTest, Interface_005, TestSize.Level1)
{
    HILOG_DEBUG("AppMgrIpcInterfaceTest_AppMgr_005 start");
    sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);

    EXPECT_CALL(*mockAppMgr, CheckPermission(_, _)).Times(1).WillOnce(Return(OHOS::NO_ERROR));
    auto recordId = AppRecordId::Create();
    int ret = appMgrClient->CheckPermission(recordId, "write");
    EXPECT_EQ(OHOS::NO_ERROR, ret);

    EXPECT_CALL(*mockAppMgr, CheckPermission(_, _)).Times(1).WillOnce(Return(OHOS::NO_ERROR));
    ret = appMgrClient->CheckPermission(recordId, "read");
    EXPECT_EQ(OHOS::NO_ERROR, ret);

    EXPECT_CALL(*mockAppMgr, CheckPermission(_, _)).Times(1).WillOnce(Return(OHOS::ERR_INVALID_STATE));
    ret = appMgrClient->CheckPermission(recordId, "location");
    EXPECT_EQ(OHOS::ERR_INVALID_STATE, ret);

    HILOG_DEBUG("AppMgrIpcInterfaceTest_AppMgr_005 end");
}

/*
 * Feature: AMS
 * Function: IPC
 * SubFunction: appmgr interface
 * FunctionPoints: interface
 * CaseDescription: test IPC can transact data
 */
HWTEST_F(AmsIpcAppMgrInterfaceTest, Interface_006, TestSize.Level1)
{
    HILOG_DEBUG("AppMgrIpcInterfaceTest_AppMgr_006 start");
    sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);

    EXPECT_CALL(*mockAppMgr, CheckPermission(_, _))
        .Times(1)
        .WillOnce(Invoke(mockAppMgr.GetRefPtr(), &MockAppMgrService::CheckPermissionImpl));
    auto recordId = AppRecordId::Create();
    int ret = appMgrClient->CheckPermission(recordId, "write");
    EXPECT_EQ(0, ret);
    EXPECT_EQ("write", mockAppMgr->GetData());
    HILOG_DEBUG("AppMgrIpcInterfaceTest_AppMgr_006 end");
}

/*
 * Feature: AMS
 * Function: IPC
 * SubFunction: appmgr interface
 * FunctionPoints: KillApplication interface
 * CaseDescription: test IPC can transact data
 */
HWTEST_F(AmsIpcAppMgrInterfaceTest, ClearUpApplicationData_008, TestSize.Level1)
{
    HILOG_DEBUG("ClearUpApplicationData_008 start");

    sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);

    EXPECT_CALL(*mockAppMgr, ClearUpApplicationData(_)).Times(1);

    appMgrClient->ClearUpApplicationData("PROCESS");

    HILOG_DEBUG("ClearUpApplicationData_008 end");
}

/*
 * Feature: AMS
 * Function: IPC
 * SubFunction: appmgr interface
 * FunctionPoints: KillApplication interface
 * CaseDescription: test IPC can transact data
 */
HWTEST_F(AmsIpcAppMgrInterfaceTest, GetAllRunningProcesses_010, TestSize.Level1)
{
    HILOG_DEBUG("GetAllRunningProcesses_009 start");

    sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);

    EXPECT_CALL(*mockAppMgr, GetAllRunningProcesses(_)).Times(1).WillOnce(Return(OHOS::ERR_NULL_OBJECT));

    std::vector<RunningProcessInfo> runningProcessInfo;
    int32_t ret = appMgrClient->GetAllRunningProcesses(runningProcessInfo);
    EXPECT_EQ(ret, OHOS::ERR_NULL_OBJECT);

    EXPECT_CALL(*mockAppMgr, GetAllRunningProcesses(_)).Times(1).WillOnce(Return(OHOS::ERR_NONE));
    ret = appMgrClient->GetAllRunningProcesses(runningProcessInfo);
    EXPECT_EQ(ret, OHOS::ERR_NONE);

    HILOG_DEBUG("GetAllRunningProcesses_009 end");
}

/*
 * @tc.name: RegisterApplicationStateObserver_001
 * @tc.desc: Register application state observer test.
 * @tc.type: FUNC
 * @tc.require: issueI5822Q
 */
HWTEST_F(AmsIpcAppMgrInterfaceTest, RegisterApplicationStateObserver_001, TestSize.Level0)
{
    HILOG_DEBUG("RegisterApplicationStateObserver_001 start");

    sptr<IApplicationStateObserver> observer = new ApplicationStateObserverStub();
    std::vector<std::string> bundleNameList;
    sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);

    EXPECT_CALL(*mockAppMgr, RegisterApplicationStateObserver(_, _)).Times(1).WillOnce(Return(OHOS::NO_ERROR));

    int32_t err = appMgrClient->RegisterApplicationStateObserver(observer, bundleNameList);

    EXPECT_EQ(OHOS::NO_ERROR, err);

    HILOG_DEBUG("RegisterApplicationStateObserver_001 end");
}

/*
 * Feature: AMS
 * Function: IPC
 * SubFunction: appmgr interface
 * FunctionPoints: KillApplication interface
 * CaseDescription: test IPC can transact data
 */
HWTEST_F(AmsIpcAppMgrInterfaceTest, UnregisterApplicationStateObserver_001, TestSize.Level0)
{
    HILOG_DEBUG("UnregisterApplicationStateObserver_001 start");

    sptr<IApplicationStateObserver> observer = new ApplicationStateObserverStub();
    sptr<MockAppMgrService> mockAppMgr(new MockAppMgrService());
    sptr<IAppMgr> appMgrClient = iface_cast<IAppMgr>(mockAppMgr);

    EXPECT_CALL(*mockAppMgr, UnregisterApplicationStateObserver(_)).Times(1).WillOnce(Return(OHOS::NO_ERROR));

    int32_t err = appMgrClient->UnregisterApplicationStateObserver(observer);

    EXPECT_EQ(OHOS::NO_ERROR, err);

    HILOG_DEBUG("UnregisterApplicationStateObserver_001 end");
}
}  // namespace AppExecFwk
}  // namespace OHOS
