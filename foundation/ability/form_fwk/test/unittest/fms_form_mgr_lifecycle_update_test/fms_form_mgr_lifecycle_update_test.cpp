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

#include "accesstoken_kit.h"
#include "form_bms_helper.h"
#include "form_db_cache.h"
#include "form_host_interface.h"
#define private public
#include "form_data_mgr.h"
#include "form_mgr.h"
#include "form_mgr_errors.h"
#include "form_mgr_service.h"
#undef private
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"

#include "mock_bundle_manager.h"
#include "mock_form_host_client.h"
#include "remote_native_token.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;
using namespace OHOS::Security;

namespace {
const std::string PERMISSION_NAME_REQUIRE_FORM = "ohos.permission.REQUIRE_FORM";
const std::string FORM_PROVIDER_BUNDLE_NAME = "com.form.provider.service";
const std::string FORM_PROVIDER_ABILITY_NAME = "com.form.provider.app.test";
const std::string FORM_HOST_BUNDLE_NAME = "com.form.host.app";
const std::string DEVICE_ID = "ohos-phone1";

const std::string DEF_LABEL1 = "PermissionFormRequireGrant";

class FmsFormMgrLifecycleUpdateTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    void InitFormItemInfo(int64_t formId, FormItemInfo &formItemInfo) const;

protected:
    std::shared_ptr<FormMgrService> formyMgrServ_ = DelayedSingleton<FormMgrService>::GetInstance();
    sptr<MockFormHostClient> token_;
};

void FmsFormMgrLifecycleUpdateTest::SetUpTestCase()
{
    RemoteNativeToken::SetNativeToken();
    FormBmsHelper::GetInstance().SetBundleManager(new BundleMgrService());
}

void FmsFormMgrLifecycleUpdateTest::TearDownTestCase()
{}

void FmsFormMgrLifecycleUpdateTest::SetUp()
{
    formyMgrServ_->OnStart();
    // token
    token_ = new (std::nothrow) MockFormHostClient();

    // Permission install
    int userId = 0;
    auto tokenId = AccessToken::AccessTokenKit::GetHapTokenID(userId, FORM_PROVIDER_BUNDLE_NAME, 0);
    auto flag = OHOS::Security::AccessToken::PERMISSION_USER_FIXED;
    AccessToken::AccessTokenKit::GrantPermission(tokenId, PERMISSION_NAME_REQUIRE_FORM, flag);
}

void FmsFormMgrLifecycleUpdateTest::TearDown()
{}

void FmsFormMgrLifecycleUpdateTest::InitFormItemInfo(int64_t formId, FormItemInfo &formItemInfo) const
{
    // create hapSourceDirs
    std::vector<std::string> hapSourceDirs;
    std::string hapSourceDir = "1/2/3";
    hapSourceDirs.emplace_back(hapSourceDir);

    // create formItemInfo
    formItemInfo.SetFormId(formId);
    formItemInfo.SetTemporaryFlag(true);
    formItemInfo.SetEnableUpdateFlag(true);
    formItemInfo.SetUpdateDuration(Constants::MIN_CONFIG_DURATION);
    formItemInfo.SetScheduledUpdateTime("10:30");
    formItemInfo.SetHapSourceDirs(hapSourceDirs);
}

/**
 * @tc.number: FmsFormMgrLifecycleUpdateTest_LifecycleUpdate_002
 * @tc.name: LifecycleUpdate
 * @tc.desc: Verify that the return value is correct.
 * @tc.info: formIds is empty.
 */
HWTEST_F(FmsFormMgrLifecycleUpdateTest, FmsFormMgrLifecycleUpdateTest_LifecycleUpdate_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "FmsFormMgrLifecycleUpdateTest_LifecycleUpdate_002 start";

    std::vector<int64_t> formIds;

    EXPECT_EQ(ERR_APPEXECFWK_FORM_INVALID_PARAM, FormMgr::GetInstance().LifecycleUpdate(formIds, token_, true));

    GTEST_LOG_(INFO) << "FmsFormMgrLifecycleUpdateTest_LifecycleUpdate_002 end";
}

/**
 * @tc.number: FmsFormMgrLifecycleUpdateTest_LifecycleUpdate_003
 * @tc.name: LifecycleUpdate
 * @tc.desc: Verify that the return value is correct.
 * @tc.info: clientRecords_ is empty.
 */
HWTEST_F(FmsFormMgrLifecycleUpdateTest, FmsFormMgrLifecycleUpdateTest_LifecycleUpdate_003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "FmsFormMgrLifecycleUpdateTest_LifecycleUpdate_003 start";

    std::vector<int64_t> formIds;
    formIds.push_back(3);
    EXPECT_EQ(ERR_APPEXECFWK_FORM_OPERATION_NOT_SELF,
        FormMgr::GetInstance().LifecycleUpdate(formIds, token_, true));

    GTEST_LOG_(INFO) << "FmsFormMgrLifecycleUpdateTest_LifecycleUpdate_003 end";
}

/**
 * @tc.number: FmsFormMgrLifecycleUpdateTest_LifecycleUpdate_004
 * @tc.name: LifecycleUpdate
 * @tc.desc: Verify that the return value is correct.
 * @tc.info:
 *      clientRecords_ is exist, but no formRecords.
 *      set EnableRefresh,  and not pull up Provider.
 */
HWTEST_F(FmsFormMgrLifecycleUpdateTest, FmsFormMgrLifecycleUpdateTest_LifecycleUpdate_004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "FmsFormMgrLifecycleUpdateTest_LifecycleUpdate_004 start";

    std::vector<int64_t> formIds;
    int64_t formId = 4;
    formIds.push_back(formId);

    // create clientRecords_
    FormHostRecord formHostRecord;
    formHostRecord.SetFormHostClient(token_);
    formHostRecord.AddForm(formId);
    formHostRecord.SetNeedRefresh(formId, true);
    FormDataMgr::GetInstance().clientRecords_.push_back(formHostRecord);

    EXPECT_EQ(ERR_OK, FormMgr::GetInstance().LifecycleUpdate(formIds, token_, true));

    GTEST_LOG_(INFO) << "FmsFormMgrLifecycleUpdateTest_LifecycleUpdate_004 end";
}


/**
 * @tc.number: FmsFormMgrLifecycleUpdateTest_LifecycleUpdate_005
 * @tc.name: LifecycleUpdate
 * @tc.desc: Verify that the return value is correct.
 * @tc.info:
 *      clientRecords and formRecords(needRefresh:true) is exist.
 *      set EnableRefresh, and pull up Provider and update.
 */
HWTEST_F(FmsFormMgrLifecycleUpdateTest, FmsFormMgrLifecycleUpdateTest_LifecycleUpdate_005, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "FmsFormMgrLifecycleUpdateTest_LifecycleUpdate_005 start";

    std::vector<int64_t> formIds;
    int64_t formId = 5;
    formIds.push_back(formId);

    // create clientRecords_
    FormHostRecord formHostRecord;
    formHostRecord.SetFormHostClient(token_);
    formHostRecord.AddForm(formId);
    formHostRecord.SetNeedRefresh(formId, true);
    FormDataMgr::GetInstance().clientRecords_.push_back(formHostRecord);

    // create formRecords
    int callingUid = 0;
    FormItemInfo formItemInfo;
    InitFormItemInfo(formId, formItemInfo);
    FormRecord record = FormDataMgr::GetInstance().CreateFormRecord(formItemInfo, callingUid);
    // needRefresh:true
    record.needRefresh = true;
    FormDataMgr::GetInstance().formRecords_.emplace(formId, record);

    EXPECT_EQ(ERR_OK, FormMgr::GetInstance().LifecycleUpdate(formIds, token_, true));

    GTEST_LOG_(INFO) << "FmsFormMgrLifecycleUpdateTest_LifecycleUpdate_005 end";
}

/**
 * @tc.number: FmsFormMgrLifecycleUpdateTest_LifecycleUpdate_006
 * @tc.name: LifecycleUpdate
 * @tc.desc: Verify that the return value is correct.
 *
 * @tc.info:
 *      clientRecords(needRefresh:true) and formRecords(needRefresh & versionUpgrade:false) is exist.
 *      set EnableRefresh, and update hostRecord.
 */
HWTEST_F(FmsFormMgrLifecycleUpdateTest, FmsFormMgrLifecycleUpdateTest_LifecycleUpdate_006, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "FmsFormMgrLifecycleUpdateTest_LifecycleUpdate_006 start";

    std::vector<int64_t> formIds;
    int64_t formId = 6;
    formIds.push_back(formId);

    // create clientRecords_
    FormHostRecord formHostRecord;
    formHostRecord.SetFormHostClient(token_);
    formHostRecord.AddForm(formId);
    // needRefresh:true
    formHostRecord.SetNeedRefresh(formId, true);
    FormDataMgr::GetInstance().clientRecords_.push_back(formHostRecord);

    // create formRecords
    int callingUid = 0;
    FormItemInfo formItemInfo;
    InitFormItemInfo(formId, formItemInfo);
    FormRecord record = FormDataMgr::GetInstance().CreateFormRecord(formItemInfo, callingUid);
    // needRefresh:false
    record.needRefresh = false;
    // versionUpgrade:false
    record.versionUpgrade = false;
    FormDataMgr::GetInstance().formRecords_.emplace(formId, record);

    EXPECT_EQ(ERR_OK, FormMgr::GetInstance().LifecycleUpdate(formIds, token_, true));

    // judge hostrecord's needRefresh_ is false.
    EXPECT_EQ(false, FormDataMgr::GetInstance().clientRecords_.at(0).IsNeedRefresh(formId));

    GTEST_LOG_(INFO) << "FmsFormMgrLifecycleUpdateTest_LifecycleUpdate_006 end";
}
}