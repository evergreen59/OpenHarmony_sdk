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
#include "appexecfwk_errors.h"
#include "form_bms_helper.h"
#include "form_cache_mgr.h"
#include "form_data_mgr.h"
#include "form_db_cache.h"
#include "form_db_info.h"
#define private public
#include "form_mgr.h"
#undef private
#include "form_mgr_errors.h"
#include "form_mgr_service.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "mock_bundle_manager.h"
#include "mock_form_host_client.h"
#include "mock_form_token.h"
#include "remote_native_token.h"
#include "running_process_info.h"
#include "system_ability_definition.h"

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
constexpr int32_t UID_CALLINGUID_TRANSFORM_DIVISOR = 200000;

class FmsFormMgrReleaseFormTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    sptr<MockFormHostClient> token_;
    std::shared_ptr<FormMgrService> formyMgrServ_ = DelayedSingleton<FormMgrService>::GetInstance();
};

void FmsFormMgrReleaseFormTest::SetUpTestCase()
{
    RemoteNativeToken::SetNativeToken();
    FormBmsHelper::GetInstance().SetBundleManager(new BundleMgrService());
}

void FmsFormMgrReleaseFormTest::TearDownTestCase()
{}

void FmsFormMgrReleaseFormTest::SetUp()
{
    formyMgrServ_->OnStart();
    token_ = new (std::nothrow) MockFormHostClient();

    // Permission install
    int userId = 0;
    auto tokenId = AccessToken::AccessTokenKit::GetHapTokenID(userId, FORM_PROVIDER_BUNDLE_NAME, 0);
    auto flag = OHOS::Security::AccessToken::PERMISSION_USER_FIXED;
    AccessToken::AccessTokenKit::GrantPermission(tokenId, PERMISSION_NAME_REQUIRE_FORM, flag);
}

void FmsFormMgrReleaseFormTest::TearDown()
{}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: ReleaseForm Function
 * FunctionPoints: FormMgr ReleaseForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Quote of form is not 0 after release form.
 */
HWTEST_F(FmsFormMgrReleaseFormTest, ReleaseForm_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_release_test_001 start";

    int64_t formId = FormDataMgr::GetInstance().GenerateFormId();
    int callingUid = IPCSkeleton::GetCallingUid();
    int32_t userId = callingUid/UID_CALLINGUID_TRANSFORM_DIVISOR;
    FormItemInfo record1;
    record1.SetFormId(formId);
    record1.SetProviderBundleName(FORM_PROVIDER_BUNDLE_NAME);
    record1.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    record1.SetTemporaryFlag(false);
    FormRecord retFormRec = FormDataMgr::GetInstance().AllotFormRecord(record1, callingUid, userId);
    // Set database info.
    FormDBInfo formDBInfo(formId, retFormRec);
    FormDbCache::GetInstance().SaveFormInfo(formDBInfo);

    int64_t formId2 = FormDataMgr::GetInstance().GenerateFormId();
    FormItemInfo record2;
    record2.SetFormId(formId2);
    record2.SetProviderBundleName(FORM_PROVIDER_BUNDLE_NAME);
    record2.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    record2.SetTemporaryFlag(false);
    FormDataMgr::GetInstance().AllotFormRecord(record2, 1, userId);
    FormItemInfo itemInfo;
    FormDataMgr::GetInstance().AllotFormHostRecord(itemInfo, token_, formId, callingUid);

    EXPECT_EQ(ERR_OK, FormMgr::GetInstance().ReleaseForm(formId, token_, true));

    // Cache uid is not deleted yet.
    FormRecord formInfo;
    bool ret = FormDataMgr::GetInstance().GetFormRecord(formId, formInfo);
    EXPECT_FALSE(ret);
    ret = FormDataMgr::GetInstance().GetFormRecord(formId2, formInfo);
    EXPECT_TRUE(ret);
    // Form host record is deleted.
    std::vector<FormHostRecord> hostRecords;
    FormDataMgr::GetInstance().GetFormHostRecord(formId, hostRecords);
    EXPECT_TRUE(hostRecords.empty());

    FormDataMgr::GetInstance().DeleteFormRecord(formId);
    FormDataMgr::GetInstance().DeleteFormRecord(formId2);
    FormDbCache::GetInstance().DeleteFormInfo(formId);
    FormDbCache::GetInstance().DeleteFormInfo(formId2);
    FormDataMgr::GetInstance().DeleteHostRecord(token_, formId2);
    GTEST_LOG_(INFO) << "fms_form_mgr_release_test_001 end";
}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: ReleaseForm Function
 * FunctionPoints: FormMgr ReleaseForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Quote of form is 0 after delete form.
 */
HWTEST_F(FmsFormMgrReleaseFormTest, ReleaseForm_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_release_test_002 start";

    int64_t formId1 = FormDataMgr::GetInstance().GenerateFormId();
    int64_t formId2 = FormDataMgr::GetInstance().GenerateFormId();
    FormItemInfo record1;
    record1.SetFormId(formId1);
    record1.SetProviderBundleName(FORM_PROVIDER_BUNDLE_NAME);
    record1.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    record1.SetTemporaryFlag(false);
    FormRecord retFormRec = FormDataMgr::GetInstance().AllotFormRecord(record1, 0);
    // Set database info.
    FormDBInfo formDBInfo(formId1, retFormRec);
    FormDbCache::GetInstance().SaveFormInfo(formDBInfo);

    FormItemInfo record2;
    record2.SetFormId(formId2);
    record2.SetProviderBundleName(FORM_PROVIDER_BUNDLE_NAME);
    record2.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    record2.SetTemporaryFlag(false);
    FormDataMgr::GetInstance().AllotFormRecord(record2, 0);

    FormItemInfo itemInfo;
    FormDataMgr::GetInstance().AllotFormHostRecord(itemInfo, token_, formId1, 0);

    FormCacheMgr::GetInstance().AddData(formId1, "test data 1");
    FormCacheMgr::GetInstance().AddData(formId2, "test data 2");

    EXPECT_EQ(ERR_OK, FormMgr::GetInstance().ReleaseForm(formId1, token_, true));

    FormDataMgr::GetInstance().DeleteFormRecord(formId1);
    FormDataMgr::GetInstance().DeleteFormRecord(formId2);
    FormDbCache::GetInstance().DeleteFormInfo(formId1);
    FormDbCache::GetInstance().DeleteFormInfo(formId2);
    FormDataMgr::GetInstance().DeleteHostRecord(token_, formId1);
    GTEST_LOG_(INFO) << "fms_form_mgr_release_test_002 end";
}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: ReleaseForm Function
 * FunctionPoints: FormMgr ReleaseForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: An exception tests if formId is not exist in cache.
 */
HWTEST_F(FmsFormMgrReleaseFormTest, ReleaseForm_004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_release_test_004 start";
    int64_t formId = FormDataMgr::GetInstance().GenerateFormId();
    EXPECT_EQ(ERR_APPEXECFWK_FORM_NOT_EXIST_ID, FormMgr::GetInstance().ReleaseForm(formId, token_, true));
    GTEST_LOG_(INFO) << "fms_form_mgr_release_test_004 end";
}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: ReleaseForm Function
 * FunctionPoints: FormMgr ReleaseForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: An exception tests if formId is not exist in host.
 */
HWTEST_F(FmsFormMgrReleaseFormTest, ReleaseForm_005, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_release_test_005 start";
    int64_t formId = FormDataMgr::GetInstance().GenerateFormId();
    FormItemInfo record;
    record.SetFormId(formId);
    record.SetProviderBundleName(FORM_PROVIDER_BUNDLE_NAME);
    record.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    record.SetTemporaryFlag(false);
    FormDataMgr::GetInstance().AllotFormRecord(record, 0);
    EXPECT_EQ(ERR_APPEXECFWK_FORM_NOT_EXIST_ID, FormMgr::GetInstance().ReleaseForm(formId, token_, true));
    GTEST_LOG_(INFO) << "fms_form_mgr_release_test_005 end";
}
}