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
#include <string>
#include <thread>

#include "appexecfwk_errors.h"
#define private public
#include "form_bms_helper.h"
#include "form_constants.h"
#include "form_data_mgr.h"
#include "form_mgr_adapter.h"
#include "form_mgr_errors.h"
#include "form_mgr_service.h"
#include "form_timer_mgr.h"
#undef private
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "mock_bundle_manager.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace {
const std::string FORM_HOST_BUNDLE_NAME = "com.form.provider.service";
const std::string FORM_PROVIDER_ABILITY_NAME = "com.form.provider.app.test.ability";
const std::string PERMISSION_NAME_REQUIRE_FORM = "ohos.permission.REQUIRE_FORM";
const std::string DEF_LABEL1 = "PermissionFormRequireGrant";
constexpr int32_t UID_CALLINGUID_TRANSFORM_DIVISOR = 200000;

class FmsFormSetNextRefreshTest : public testing::Test {
public:
    FmsFormSetNextRefreshTest() : formSetNextRefresh_(nullptr)
    {}
    ~FmsFormSetNextRefreshTest()
    {}
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
protected:
    std::shared_ptr<FormMgrService> formSetNextRefresh_;
};

void FmsFormSetNextRefreshTest::SetUpTestCase(void)
{
    FormBmsHelper::GetInstance().SetBundleManager(new BundleMgrService());
}

void FmsFormSetNextRefreshTest::TearDownTestCase(void)
{}

void FmsFormSetNextRefreshTest::SetUp(void)
{
    formSetNextRefresh_ = DelayedSingleton<FormMgrService>::GetInstance();
    formSetNextRefresh_->OnStart();
}

void FmsFormSetNextRefreshTest::TearDown(void)
{
    formSetNextRefresh_->OnStop();
}

/**
 * @tc.number: FmsFormSetNextRefreshTest_SetNextRefreshTime_001
 * @tc.name: SetNextRefreshTime
 * @tc.desc: Verify that the return value is true.(formId is invalid)
 */
HWTEST_F(FmsFormSetNextRefreshTest, FmsFormSetNextRefreshTest_SetNextRefreshTime_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "FmsFormSetNextRefreshTest_SetNextRefreshTime_001 start";
    int64_t formId = 0; // invalid formId
    int64_t nextTime = Constants::MIN_NEXT_TIME;

    EXPECT_EQ(ERR_APPEXECFWK_FORM_INVALID_PARAM, formSetNextRefresh_->SetNextRefreshTime(formId, nextTime));
    GTEST_LOG_(INFO) << "FmsFormSetNextRefreshTest_SetNextRefreshTime_001 end";
}

/**
 * @tc.number: FmsFormSetNextRefreshTest_SetNextRefreshTime_002
 * @tc.name: SetNextRefreshTime
 * @tc.desc: Verify that the return value is true.(not found in form record)
 */
HWTEST_F(FmsFormSetNextRefreshTest, FmsFormSetNextRefreshTest_SetNextRefreshTime_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "FmsFormSetNextRefreshTest_SetNextRefreshTime_002 start";
    int64_t formId = 2;
    int64_t nextTime = Constants::MIN_NEXT_TIME;

    EXPECT_EQ(ERR_APPEXECFWK_FORM_NOT_EXIST_ID, formSetNextRefresh_->SetNextRefreshTime(formId, nextTime));
    GTEST_LOG_(INFO) << "FmsFormSetNextRefreshTest_SetNextRefreshTime_002 end";
}

/**
 * @tc.number: FmsFormSetNextRefreshTest_SetNextRefreshTime_003
 * @tc.name: SetNextRefreshTime
 * @tc.desc: Verify that the return value is true.(BundleName is found in form record, but no dynamicRefreshTask)
 */
HWTEST_F(FmsFormSetNextRefreshTest, FmsFormSetNextRefreshTest_SetNextRefreshTime_003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "FmsFormSetNextRefreshTest_SetNextRefreshTime_003 start";
    int64_t formId = 3;
    int64_t nextTime = Constants::MIN_NEXT_TIME;
    int callingUid = IPCSkeleton::GetCallingUid();
    int32_t userId = callingUid/UID_CALLINGUID_TRANSFORM_DIVISOR;
    GTEST_LOG_(INFO) << "callingUid:" << callingUid << " userId:" << userId;
    // check dynamicRefreshTasks_
    EXPECT_EQ(true, FormTimerMgr::GetInstance().dynamicRefreshTasks_.empty());

    // creat formRecords_
    FormItemInfo iteminfo;
    iteminfo.formId_ = formId;
    iteminfo.providerBundleName_ = FORM_HOST_BUNDLE_NAME;
    iteminfo.abilityName_ = FORM_PROVIDER_ABILITY_NAME;
    iteminfo.temporaryFlag_ = true;

    FormDataMgr::GetInstance().AllotFormRecord(iteminfo, callingUid, userId);
    EXPECT_EQ(ERR_OK, formSetNextRefresh_->SetNextRefreshTime(formId, nextTime));

    // check dynamicRefreshTasks_
    EXPECT_EQ(false, FormTimerMgr::GetInstance().dynamicRefreshTasks_.empty());

    GTEST_LOG_(INFO) << "FmsFormSetNextRefreshTest_SetNextRefreshTime_003 end";
}

/**
 * @tc.number: FmsFormSetNextRefreshTest_SetNextRefreshTime_004
 * @tc.name: SetNextRefreshTime
 * @tc.desc: Verify that the return value is true.(BundleName is not found in form record)
 */
HWTEST_F(FmsFormSetNextRefreshTest, FmsFormSetNextRefreshTest_SetNextRefreshTime_004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "FmsFormSetNextRefreshTest_SetNextRefreshTime_004 start";
    int64_t formId = 4;
    int64_t nextTime = Constants::MIN_NEXT_TIME;

    // creat formRecords_
    FormItemInfo iteminfo;
    iteminfo.formId_ = formId;
    iteminfo.providerBundleName_ = "other_bundleName";
    iteminfo.abilityName_ = FORM_PROVIDER_ABILITY_NAME;
    iteminfo.temporaryFlag_ = true;
    FormDataMgr::GetInstance().AllotFormRecord(iteminfo, 0);

    EXPECT_EQ(ERR_APPEXECFWK_FORM_OPERATION_NOT_SELF, formSetNextRefresh_->SetNextRefreshTime(formId, nextTime));
    GTEST_LOG_(INFO) << "FmsFormSetNextRefreshTest_SetNextRefreshTime_004 end";
}

/**
 * @tc.number: FmsFormSetNextRefreshTest_SetNextRefreshTime_005
 * @tc.name: SetNextRefreshTime
 * @tc.desc: Verify that the return value is true.(have dynamicRefreshTask , have IntervalTimerTasks)
 */
HWTEST_F(FmsFormSetNextRefreshTest, FmsFormSetNextRefreshTest_SetNextRefreshTime_005, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "FmsFormSetNextRefreshTest_SetNextRefreshTime_005 start";
    int64_t formId = 5;
    int64_t nextTime = Constants::MIN_NEXT_TIME;
    int callingUid = IPCSkeleton::GetCallingUid();
    int32_t userId = callingUid/UID_CALLINGUID_TRANSFORM_DIVISOR;
    // creat formRecords_
    FormItemInfo iteminfo;
    iteminfo.formId_ = formId;
    iteminfo.providerBundleName_ = FORM_HOST_BUNDLE_NAME;
    iteminfo.abilityName_ = FORM_PROVIDER_ABILITY_NAME;
    iteminfo.temporaryFlag_ = true;

    FormDataMgr::GetInstance().AllotFormRecord(iteminfo, callingUid, userId);

    // Creat dynamicRefreshTasks_
    DynamicRefreshItem theItem;
    theItem.formId = formId;
    theItem.userId = userId;
    theItem.settedTime = 1;
    FormTimerMgr::GetInstance().dynamicRefreshTasks_.clear();
    FormTimerMgr::GetInstance().dynamicRefreshTasks_.emplace_back(theItem);
    // check dynamicRefreshTasks_
    EXPECT_EQ(1, FormTimerMgr::GetInstance().dynamicRefreshTasks_.at(0).settedTime);

    // Create IntervalTimerTasks_
    FormTimer task(formId, 3 * Constants::MIN_PERIOD, userId);

    task.isEnable = true;
    FormTimerMgr::GetInstance().AddFormTimer(task);

    EXPECT_EQ(ERR_OK, formSetNextRefresh_->SetNextRefreshTime(formId, nextTime));
    // check dynamicRefreshTasks_
    EXPECT_EQ(true, FormTimerMgr::GetInstance().dynamicRefreshTasks_.at(0).settedTime != 1);

    GTEST_LOG_(INFO) << "FmsFormSetNextRefreshTest_SetNextRefreshTime_005 end";
}

/**
 * @tc.number: FmsFormSetNextRefreshTest_SetNextRefreshTime_006
 * @tc.name: SetNextRefreshTime
 * @tc.desc: Verify that the return value is true.(timerRefreshedCount >= 50)
 */
HWTEST_F(FmsFormSetNextRefreshTest, FmsFormSetNextRefreshTest_SetNextRefreshTime_006, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "FmsFormSetNextRefreshTest_SetNextRefreshTime_006 start";

    int64_t formId = 6;
    int64_t nextTime = Constants::MIN_NEXT_TIME;
    int32_t callingUid = IPCSkeleton::GetCallingUid();
    int32_t userId = callingUid/UID_CALLINGUID_TRANSFORM_DIVISOR;

    // creat formRecords_
    FormItemInfo iteminfo;
    iteminfo.formId_ = formId;
    iteminfo.providerBundleName_ = FORM_HOST_BUNDLE_NAME;
    iteminfo.abilityName_ = FORM_PROVIDER_ABILITY_NAME;
    iteminfo.temporaryFlag_ = true;
    FormDataMgr::GetInstance().AllotFormRecord(iteminfo, callingUid, userId);

    // set timerRefreshedCount
    FormTimerMgr::GetInstance().refreshLimiter_.AddItem(formId);
    auto iter = FormTimerMgr::GetInstance().refreshLimiter_.limiterMap_.find(formId);
    if (iter == FormTimerMgr::GetInstance().refreshLimiter_.limiterMap_.end()) {
        GTEST_LOG_(INFO) << "not found in limiterMap_!!!";
    } else {
        iter->second.refreshCount = Constants::LIMIT_COUNT;
    }
    EXPECT_EQ(ERR_APPEXECFWK_FORM_MAX_REFRESH, formSetNextRefresh_->SetNextRefreshTime(formId, nextTime));

    GTEST_LOG_(INFO) << "FmsFormSetNextRefreshTest_SetNextRefreshTime_006 end";
}
}
