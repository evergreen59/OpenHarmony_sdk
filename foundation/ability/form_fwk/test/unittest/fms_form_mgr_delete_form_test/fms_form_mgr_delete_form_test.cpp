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
#include "form_ams_helper.h"
#include "form_bms_helper.h"
#include "form_data_mgr.h"
#include "form_db_cache.h"
#include "form_host_interface.h"
#define private public
#include "form_mgr.h"
#undef private
#include "form_mgr_errors.h"
#include "form_mgr_service.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"

#include "mock_ability_manager.h"
#include "mock_bundle_manager.h"
#include "mock_form_host_client.h"
#include "remote_native_token.h"
#include "running_process_info.h"
#include "system_ability_definition.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;
using namespace OHOS::Security;

namespace {
const std::string PERMISSION_NAME_REQUIRE_FORM = "ohos.permission.REQUIRE_FORM";
const std::string PARAM_PROVIDER_PACKAGE_NAME = "com.form.provider.app.test.ability";
const std::string FORM_PROVIDER_BUNDLE_NAME = "com.form.provider.service";
const std::string PARAM_PROVIDER_MODULE_NAME = "com.form.provider.app.test.ability";
const std::string FORM_PROVIDER_ABILITY_NAME = "com.form.provider.app.test.ability";
const std::string PARAM_FORM_NAME = "com.form.name.test";

const std::string FORM_JS_COMPONENT_NAME = "jsComponentName";
const std::string FORM_PROVIDER_MODULE_SOURCE_DIR = "";

const std::string FORM_HOST_BUNDLE_NAME = "com.form.host.app";

const std::string DEVICE_ID = "ohos-phone1";
const std::string DEF_LABEL1 = "PermissionFormRequireGrant";

class FmsFormMgrDeleteFormTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    sptr<MockFormHostClient> token_;
    std::shared_ptr<FormMgrService> formyMgrServ_ = DelayedSingleton<FormMgrService>::GetInstance();
};

void FmsFormMgrDeleteFormTest::SetUpTestCase()
{
    RemoteNativeToken::SetNativeToken();
    FormBmsHelper::GetInstance().SetBundleManager(new BundleMgrService());
    FormAmsHelper::GetInstance().SetAbilityManager(new MockAbilityMgrService());
}

void FmsFormMgrDeleteFormTest::TearDownTestCase()
{}

void FmsFormMgrDeleteFormTest::SetUp()
{
    formyMgrServ_->OnStart();
    token_ = new (std::nothrow) MockFormHostClient();

    // Permission install
    int userId = 0;
    auto tokenId = AccessToken::AccessTokenKit::GetHapTokenID(userId, FORM_PROVIDER_BUNDLE_NAME, 0);
    auto flag = OHOS::Security::AccessToken::PERMISSION_USER_FIXED;
    AccessToken::AccessTokenKit::GrantPermission(tokenId, PERMISSION_NAME_REQUIRE_FORM, flag);
}

void FmsFormMgrDeleteFormTest::TearDown()
{}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: DeleteForm Function
 * FunctionPoints: FormMgr DeleteForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Quote of form is not 0 after delete form.
 */
HWTEST_F(FmsFormMgrDeleteFormTest, DeleteForm_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_delete_form_test_001 start";

    // clear old data
    FormDataMgr::GetInstance().ClearFormRecords();
    std::vector<FormDBInfo> oldFormDBInfos;
    FormDbCache::GetInstance().GetAllFormInfo(oldFormDBInfos);
    for (auto oldFormDBInfo: oldFormDBInfos) {
        FormDbCache::GetInstance().DeleteFormInfo(oldFormDBInfo.formId);
    }

    int64_t formId {12001};
    int callingUid = IPCSkeleton::GetCallingUid();
    // Create cache
    FormItemInfo record1;
    record1.SetFormId(formId);
    record1.SetProviderBundleName(FORM_PROVIDER_BUNDLE_NAME);
    record1.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    record1.SetTemporaryFlag(false);
    FormRecord retFormRec = FormDataMgr::GetInstance().AllotFormRecord(record1, callingUid);
    // User Uid Add some Useruids into a form in cache
    int formUserUid {1};
    FormDataMgr::GetInstance().AddFormUserUid(formId, formUserUid);
    // Set form host record
    retFormRec.formUserUids.emplace_back(formUserUid);
    FormDBInfo formDBInfo(formId, retFormRec);
    FormDbCache::GetInstance().SaveFormInfo(formDBInfo);
    // Set host record
    FormItemInfo info;
    FormDataMgr::GetInstance().AllotFormHostRecord(info, token_, formId, callingUid);

    EXPECT_EQ(ERR_OK, FormMgr::GetInstance().DeleteForm(formId, token_));
    token_->Wait();

    // Cache uid is not deleted yet.
    FormRecord formInfo;
    bool ret = FormDataMgr::GetInstance().GetFormRecord(formId, formInfo);
    EXPECT_TRUE(ret);
    size_t dataCnt {1};
    EXPECT_EQ(dataCnt, formInfo.formUserUids.size());
    // Database is not deleted yet.
    std::vector<FormDBInfo> formDBInfos;
    FormDbCache::GetInstance().GetAllFormInfo(formDBInfos);
    EXPECT_EQ(dataCnt, formDBInfos.size());
    FormDBInfo dbInfo {formDBInfos[0]};
    EXPECT_EQ(formId, dbInfo.formId);
    EXPECT_EQ(dataCnt, dbInfo.formUserUids.size());
    // Form host record is deleted.
    std::vector<FormHostRecord> hostRecords;
    FormDataMgr::GetInstance().GetFormHostRecord(formId, hostRecords);
    EXPECT_TRUE(hostRecords.empty());

    FormDataMgr::GetInstance().DeleteFormRecord(formId);
    FormDbCache::GetInstance().DeleteFormInfo(formId);
    FormDataMgr::GetInstance().DeleteHostRecord(token_, formId);
    GTEST_LOG_(INFO) << "fms_form_mgr_delete_form_test_001 end";
}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: DeleteForm Function
 * FunctionPoints: FormMgr DeleteForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Quote of form is 0 after delete form.
 */
HWTEST_F(FmsFormMgrDeleteFormTest, DeleteForm_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_delete_form_test_002 start";

    int64_t formId {12002};
    int callingUid = IPCSkeleton::GetCallingUid();
    // Create cache
    FormItemInfo record1;
    record1.SetFormId(formId);
    record1.SetProviderBundleName(FORM_PROVIDER_BUNDLE_NAME);
    record1.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    record1.SetTemporaryFlag(false);
    FormRecord retFormRec = FormDataMgr::GetInstance().AllotFormRecord(record1, callingUid);
    // User Uid Add some Useruids into a form in cache
    FormDBInfo formDBInfo(formId, retFormRec);
    FormDbCache::GetInstance().SaveFormInfo(formDBInfo);
    // Set form host record
    FormItemInfo info;
    FormDataMgr::GetInstance().AllotFormHostRecord(info, token_, formId, callingUid);

    EXPECT_EQ(ERR_OK, FormMgr::GetInstance().DeleteForm(formId, token_));
    token_->Wait();

    // Cache uid is deleted.
    FormRecord formInfo;
    bool ret = FormDataMgr::GetInstance().GetFormRecord(formId, formInfo);
    EXPECT_FALSE(ret);
    // Database is deleted.
    size_t dataCnt {0};
    std::vector<FormDBInfo> formDBInfos;
    FormDbCache::GetInstance().GetAllFormInfo(formDBInfos);
    EXPECT_EQ(dataCnt, formDBInfos.size());
    // Form host record is deleted.
    std::vector<FormHostRecord> hostRecords;
    FormDataMgr::GetInstance().GetFormHostRecord(formId, hostRecords);
    EXPECT_TRUE(hostRecords.empty());

    FormDataMgr::GetInstance().DeleteFormRecord(formId);
    FormDbCache::GetInstance().DeleteFormInfo(formId);
    FormDataMgr::GetInstance().DeleteHostRecord(token_, formId);
    GTEST_LOG_(INFO) << "fms_form_mgr_delete_form_test_002 end";
}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: DeleteForm Function
 * FunctionPoints: FormMgr DeleteForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Test cases when some paraments are invalid
 */
HWTEST_F(FmsFormMgrDeleteFormTest, DeleteForm_003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_delete_form_test_003 start";
    // case when formId<=0
    EXPECT_EQ(ERR_APPEXECFWK_FORM_INVALID_FORM_ID, FormMgr::GetInstance().DeleteForm(0, token_));
    // case when token is nullptr
    EXPECT_EQ(ERR_APPEXECFWK_FORM_INVALID_PARAM, FormMgr::GetInstance().DeleteForm(123L, nullptr));
    GTEST_LOG_(INFO) << "fms_form_mgr_delete_form_test_003 end";
}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: DeleteForm Function
 * FunctionPoints: FormMgr DeleteForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Case with no database info and form is not temporary.
 */
HWTEST_F(FmsFormMgrDeleteFormTest, DeleteForm_006, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_delete_form_test_006 start";

    int64_t formId {12006};
    int callingUid = IPCSkeleton::GetCallingUid();
    FormItemInfo record1;
    record1.SetFormId(formId);
    record1.SetProviderBundleName(FORM_PROVIDER_BUNDLE_NAME);
    record1.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    record1.SetTemporaryFlag(false);
    FormDataMgr::GetInstance().AllotFormRecord(record1, callingUid);
    // Set form host record
    FormItemInfo info;
    FormDataMgr::GetInstance().AllotFormHostRecord(info, token_, formId, callingUid);

    EXPECT_EQ(ERR_APPEXECFWK_FORM_NOT_EXIST_ID, FormMgr::GetInstance().DeleteForm(formId, token_));

    FormDataMgr::GetInstance().DeleteFormRecord(formId);
    FormDbCache::GetInstance().DeleteFormInfo(formId);
    FormDataMgr::GetInstance().DeleteHostRecord(token_, formId);
    GTEST_LOG_(INFO) << "fms_form_mgr_delete_form_test_006 end";
}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: DeleteForm Function
 * FunctionPoints: FormMgr DeleteForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Case when form with database info, not temporary form, without host record.
 */
HWTEST_F(FmsFormMgrDeleteFormTest, DeleteForm_007, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_delete_form_test_007 start";

    int64_t formId {12007};
    int callingUid = IPCSkeleton::GetCallingUid();
    // Create cache
    FormItemInfo record1;
    record1.SetFormId(formId);
    record1.SetProviderBundleName(FORM_PROVIDER_BUNDLE_NAME);
    record1.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    record1.SetTemporaryFlag(false);
    FormRecord retFormRec = FormDataMgr::GetInstance().AllotFormRecord(record1, callingUid);
    // Set database info.
    retFormRec.formUserUids.clear();
    FormDBInfo formDBInfo(formId, retFormRec);
    FormDbCache::GetInstance().SaveFormInfo(formDBInfo);
    // Set form host record
    FormItemInfo info;
    FormDataMgr::GetInstance().AllotFormHostRecord(info, token_, formId, callingUid);

    ASSERT_EQ(ERR_APPEXECFWK_FORM_OPERATION_NOT_SELF, FormMgr::GetInstance().DeleteForm(formId, token_));

    // Cache uid is not deleted yet.
    FormRecord formInfo;
    bool ret = FormDataMgr::GetInstance().GetFormRecord(formId, formInfo);
    EXPECT_TRUE(ret);
    size_t dataCnt {1};
    EXPECT_EQ(dataCnt, formInfo.formUserUids.size());
    // Database is not deleted yet.
    std::vector<FormDBInfo> formDBInfos;
    FormDbCache::GetInstance().GetAllFormInfo(formDBInfos);
    EXPECT_EQ(dataCnt, formDBInfos.size());
    FormDBInfo dbInfo {formDBInfos[0]};
    EXPECT_EQ(formId, dbInfo.formId);
    dataCnt = 0;
    EXPECT_EQ(dataCnt, dbInfo.formUserUids.size());
    // Form host record is not deleted.
    std::vector<FormHostRecord> hostRecords;
    FormDataMgr::GetInstance().GetFormHostRecord(formId, hostRecords);
    EXPECT_FALSE(hostRecords.empty());

    FormDataMgr::GetInstance().DeleteFormRecord(formId);
    FormDbCache::GetInstance().DeleteFormInfo(formId);
    FormDataMgr::GetInstance().DeleteHostRecord(token_, formId);
    GTEST_LOG_(INFO) << "fms_form_mgr_delete_form_test_007 end";
}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: DeleteForm Function
 * FunctionPoints: FormMgr DeleteForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Persistent data，abilityName or bundleName is empty.
 */
HWTEST_F(FmsFormMgrDeleteFormTest, DeleteForm_008, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_delete_form_test_008 start";

    int64_t formId {12008};
    int callingUid = IPCSkeleton::GetCallingUid();
    FormItemInfo record1;
    record1.SetFormId(formId);
    record1.SetProviderBundleName(FORM_PROVIDER_BUNDLE_NAME);
    record1.SetAbilityName("");
    record1.SetTemporaryFlag(false);
    FormRecord retFormRec = FormDataMgr::GetInstance().AllotFormRecord(record1, callingUid);
    // Set database info.
    FormDBInfo formDBInfo(formId, retFormRec);
    FormDbCache::GetInstance().SaveFormInfo(formDBInfo);
    // Set form host record
    FormItemInfo info;
    FormDataMgr::GetInstance().AllotFormHostRecord(info, token_, formId, callingUid);

    ASSERT_EQ(ERR_APPEXECFWK_FORM_INVALID_PARAM, FormMgr::GetInstance().DeleteForm(formId, token_));

    // Cache uid is not deleted yet.
    FormRecord formInfo;
    bool ret = FormDataMgr::GetInstance().GetFormRecord(formId, formInfo);
    EXPECT_TRUE(ret);
    size_t dataCnt {1};
    EXPECT_EQ(dataCnt, formInfo.formUserUids.size());
    // Database is not deleted yet.
    std::vector<FormDBInfo> formDBInfos;
    FormDbCache::GetInstance().GetAllFormInfo(formDBInfos);
    EXPECT_EQ(dataCnt, formDBInfos.size());
    FormDBInfo dbInfo {formDBInfos[0]};
    EXPECT_EQ(formId, dbInfo.formId);
    EXPECT_EQ(dataCnt, dbInfo.formUserUids.size());
    // form host record is not deleted yet.
    std::vector<FormHostRecord> hostRecords;
    FormDataMgr::GetInstance().GetFormHostRecord(formId, hostRecords);
    EXPECT_FALSE(hostRecords.empty());

    // Database info.
    FormDbCache::GetInstance().DeleteFormInfo(formId);
    retFormRec.bundleName = "";
    retFormRec.abilityName = FORM_PROVIDER_ABILITY_NAME;
    FormDBInfo formDBInfo1(formId, retFormRec);
    FormDbCache::GetInstance().SaveFormInfo(formDBInfo1);

    EXPECT_EQ(ERR_APPEXECFWK_FORM_INVALID_PARAM, FormMgr::GetInstance().DeleteForm(formId, token_));

    // Cache uid is not deleted yet.
    EXPECT_TRUE(FormDataMgr::GetInstance().GetFormRecord(formId, formInfo));
    EXPECT_EQ(dataCnt, formInfo.formUserUids.size());
    // Database is not deleted yet.
    formDBInfos.clear();
    FormDbCache::GetInstance().GetAllFormInfo(formDBInfos);
    EXPECT_EQ(dataCnt, formDBInfos.size());
    dbInfo = formDBInfos[0];
    EXPECT_EQ(formId, dbInfo.formId);
    EXPECT_EQ(dataCnt, dbInfo.formUserUids.size());
    // form host record is not deleted yet.
    FormDataMgr::GetInstance().GetFormHostRecord(formId, hostRecords);
    EXPECT_FALSE(hostRecords.empty());

    FormDataMgr::GetInstance().DeleteFormRecord(formId);
    FormDbCache::GetInstance().DeleteFormInfo(formId);
    FormDataMgr::GetInstance().DeleteHostRecord(token_, formId);
    GTEST_LOG_(INFO) << "fms_form_mgr_delete_form_test_008 end";
}

/**
 * @tc.name: DeleteForm_009
 * @tc.desc: Delete in application form
 * @tc.type: FUNC
 * @tc.require: issueI5OORU
 */
HWTEST_F(FmsFormMgrDeleteFormTest, DeleteForm_009, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_delete_form_test_009 start";

    // clear old data
    FormDataMgr::GetInstance().ClearFormRecords();
    std::vector<FormDBInfo> oldFormDBInfos;
    FormDbCache::GetInstance().GetAllFormInfo(oldFormDBInfos);
    for (auto oldFormDBInfo: oldFormDBInfos) {
        FormDbCache::GetInstance().DeleteFormInfo(oldFormDBInfo.formId);
    }

    int64_t formId {12001};
    int callingUid = IPCSkeleton::GetCallingUid();
    // Create cache
    FormItemInfo record1;
    record1.SetFormId(formId);
    record1.SetProviderBundleName(FORM_PROVIDER_BUNDLE_NAME);
    record1.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    record1.SetTemporaryFlag(false);
    FormRecord retFormRec = FormDataMgr::GetInstance().AllotFormRecord(record1, callingUid);
    // User Uid Add some Useruids into a form in cache
    int formUserUid {1};
    FormDataMgr::GetInstance().AddFormUserUid(formId, formUserUid);
    // Set form host record
    retFormRec.formUserUids.emplace_back(formUserUid);
    FormDBInfo formDBInfo(formId, retFormRec);
    FormDbCache::GetInstance().SaveFormInfo(formDBInfo);
    // Set host record
    FormItemInfo info;
    info.SetHostBundleName(FORM_PROVIDER_BUNDLE_NAME);
    FormDataMgr::GetInstance().AllotFormHostRecord(info, token_, formId, callingUid);

    EXPECT_EQ(ERR_OK, FormMgr::GetInstance().DeleteForm(formId, token_));
    token_->Wait();

    // Cache uid is not deleted yet.
    FormRecord formInfo;
    bool ret = FormDataMgr::GetInstance().GetFormRecord(formId, formInfo);
    EXPECT_TRUE(ret);
    size_t dataCnt {1};
    EXPECT_EQ(dataCnt, formInfo.formUserUids.size());
    // Database is not deleted yet.
    std::vector<FormDBInfo> formDBInfos;
    FormDbCache::GetInstance().GetAllFormInfo(formDBInfos);
    EXPECT_EQ(dataCnt, formDBInfos.size());
    FormDBInfo dbInfo {formDBInfos[0]};
    EXPECT_EQ(formId, dbInfo.formId);
    EXPECT_EQ(dataCnt, dbInfo.formUserUids.size());
    // Form host record is deleted.
    std::vector<FormHostRecord> hostRecords;
    FormDataMgr::GetInstance().GetFormHostRecord(formId, hostRecords);
    EXPECT_TRUE(hostRecords.empty());

    FormDataMgr::GetInstance().DeleteFormRecord(formId);
    FormDbCache::GetInstance().DeleteFormInfo(formId);
    FormDataMgr::GetInstance().DeleteHostRecord(token_, formId);
    GTEST_LOG_(INFO) << "fms_form_mgr_delete_form_test_009 end";
}
}