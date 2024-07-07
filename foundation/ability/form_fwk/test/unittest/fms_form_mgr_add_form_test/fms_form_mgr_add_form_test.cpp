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
#define private public
#include "form_data_mgr.h"
#undef private
#include "form_db_cache.h"
#include "form_host_interface.h"
#define private public
#include "form_mgr.h"
#include "form_info.h"
#include "form_info_mgr.h"
#undef private
#include "form_mgr_errors.h"
#include "form_mgr_service.h"
#include "form_supply_callback.h"
#include "form_util.h"
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
using ::testing::SetArgReferee;
using ::testing::_;
using ::testing::Return;
using ::testing::DoAll;

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

const int32_t PARAM_FORM_DIMENSION_VALUE = 1;
const int32_t PARAM_FORM_DIMENSION_2_1 = 5;

const std::string DEVICE_ID = "ohos-phone1";
const std::string DEF_LABEL1 = "PermissionFormRequireGrant";

class FmsFormMgrAddFormTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    void CreateProviderData();
protected:
    sptr<MockFormHostClient> token_;
    std::shared_ptr<FormMgrService> formyMgrServ_ = DelayedSingleton<FormMgrService>::GetInstance();
    int32_t userId_ = FormUtil::GetCurrentAccountId();
};

void FmsFormMgrAddFormTest::SetUpTestCase()
{
    RemoteNativeToken::SetNativeToken();
    FormBmsHelper::GetInstance().SetBundleManager(new BundleMgrService());
    FormAmsHelper::GetInstance().SetAbilityManager(new MockAbilityMgrService());
}

void FmsFormMgrAddFormTest::TearDownTestCase()
{}

void FmsFormMgrAddFormTest::SetUp()
{
    formyMgrServ_->OnStart();
    token_ = new (std::nothrow) MockFormHostClient();

    // Permission install
    int userId = 0;
    auto tokenId = AccessToken::AccessTokenKit::GetHapTokenID(userId, FORM_PROVIDER_BUNDLE_NAME, 0);
    auto flag = OHOS::Security::AccessToken::PERMISSION_USER_FIXED;
    AccessToken::AccessTokenKit::GrantPermission(tokenId, PERMISSION_NAME_REQUIRE_FORM, flag);
}

void FmsFormMgrAddFormTest::TearDown()
{
    formyMgrServ_->OnStop();
}
void FmsFormMgrAddFormTest::CreateProviderData()
{
    std::unordered_map<std::string, std::shared_ptr<BundleFormInfo>> bundleFormInfoMap;
    std::shared_ptr<BundleFormInfo> bundleFormInfo = std::make_shared<BundleFormInfo>(FORM_PROVIDER_BUNDLE_NAME);
    FormInfo formInfo;
    formInfo.bundleName = FORM_PROVIDER_BUNDLE_NAME;
    formInfo.abilityName = FORM_PROVIDER_ABILITY_NAME;
    formInfo.moduleName = PARAM_PROVIDER_MODULE_NAME;
    formInfo.name = PARAM_FORM_NAME;
    formInfo.updateEnabled = true;
    formInfo.updateDuration = 1;
    formInfo.scheduledUpdateTime = "06:06";
    formInfo.jsComponentName = FORM_JS_COMPONENT_NAME;
    formInfo.formVisibleNotify = true;
    formInfo.supportDimensions = {1, 2, 5};
    formInfo.defaultDimension = 1;
    FormInfoStorage formInfoStorage;
    formInfoStorage.userId = userId_;
    formInfoStorage.formInfos.push_back(formInfo);
    bundleFormInfo->formInfoStorages_.emplace_back(formInfoStorage);
    bundleFormInfoMap.emplace(FORM_PROVIDER_BUNDLE_NAME, bundleFormInfo);

    FormInfoMgr::GetInstance().bundleFormInfoMap_ = bundleFormInfoMap;
}

class FmsFormMgrAddFormTestExt : public FmsFormMgrAddFormTest {
public:
    static void SetUpTestCase()
    {
        RemoteNativeToken::SetNativeToken();
        mockBundleMgrService = new MockBundleMgrService();
        FormBmsHelper::GetInstance().SetBundleManager(mockBundleMgrService);
        FormAmsHelper::GetInstance().SetAbilityManager(new MockAbilityMgrService());
    }

    void FillBundleInfo(const std::string &bundleName, BundleInfo &bundleInfo);
    static sptr<MockBundleMgrService> mockBundleMgrService;
};

sptr<MockBundleMgrService> FmsFormMgrAddFormTestExt::mockBundleMgrService = nullptr;

void FmsFormMgrAddFormTestExt::FillBundleInfo(const std::string &bundleName, BundleInfo &bundleInfo)
{
    std::vector<AbilityInfo> abilityInfos;
    ApplicationInfo applicationInfo;
    ModuleInfo moduleInfo;

    moduleInfo.moduleSourceDir = FORM_PROVIDER_MODULE_SOURCE_DIR;
    moduleInfo.moduleName = PARAM_PROVIDER_MODULE_NAME;
    bundleInfo.name = bundleName;
    applicationInfo.bundleName = bundleName;
    applicationInfo.moduleInfos.emplace_back(moduleInfo);
    bundleInfo.applicationInfo = applicationInfo;

    bundleInfo.moduleNames.emplace_back(PARAM_PROVIDER_MODULE_NAME);

    AbilityInfo abilityInfo;
    abilityInfo.name = FORM_PROVIDER_ABILITY_NAME;
    abilityInfo.package = PARAM_PROVIDER_PACKAGE_NAME;
    abilityInfo.bundleName = bundleName;
    abilityInfo.moduleName = PARAM_PROVIDER_MODULE_NAME;
    abilityInfo.deviceId = DEVICE_ID;
    abilityInfo.hapPath = Constants::ABS_CODE_PATH;
    bundleInfo.abilityInfos.emplace_back(abilityInfo);
}

/**
 * @tc.name: AddForm_001
 * @tc.desc: Add 2_1 form
 * @tc.type: FUNC
 * @tc.require: issueI5G2SH
 */
HWTEST_F(FmsFormMgrAddFormTest, AddForm_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_001 start";
    CreateProviderData();
    // No cache
    FormJsInfo formJsInfo;
    Want want;
    want.SetParam(Constants::PARAM_FORM_HOST_BUNDLENAME_KEY, FORM_HOST_BUNDLE_NAME);
    want.SetParam(Constants::PARAM_MODULE_NAME_KEY, PARAM_PROVIDER_MODULE_NAME);
    want.SetParam(Constants::PARAM_FORM_NAME_KEY, PARAM_FORM_NAME);
    want.SetParam(Constants::PARAM_FORM_DIMENSION_KEY, PARAM_FORM_DIMENSION_2_1);
    want.SetElementName(DEVICE_ID, FORM_PROVIDER_BUNDLE_NAME, FORM_PROVIDER_ABILITY_NAME);
    want.SetParam(Constants::PARAM_FORM_TEMPORARY_KEY, false);
    want.SetParam(Constants::ACQUIRE_TYPE, Constants::ACQUIRE_TYPE_CREATE_FORM);
    // clear old data
    FormDataMgr::GetInstance().ClearFormRecords();
    std::vector<FormDBInfo> oldFormDBInfos;
    FormDbCache::GetInstance().GetAllFormInfo(oldFormDBInfos);
    for (auto oldFormDBInfo: oldFormDBInfos) {
        FormDbCache::GetInstance().DeleteFormInfo(oldFormDBInfo.formId);
    }

    // add form
    EXPECT_EQ(ERR_OK, FormMgr::GetInstance().AddForm(0L, want, token_, formJsInfo));
    token_->Wait();

    size_t dataCnt {1};
    int64_t formId = formJsInfo.formId;
    // Form record alloted.
    FormRecord formInfo;
    bool ret = FormDataMgr::GetInstance().GetFormRecord(formId, formInfo);
    EXPECT_TRUE(ret);
    EXPECT_EQ(dataCnt, formInfo.formUserUids.size());
    // Database info alloted.
    std::vector<FormDBInfo> formDBInfos;
    FormDbCache::GetInstance().GetAllFormInfo(formDBInfos);
    EXPECT_EQ(dataCnt, formDBInfos.size());
    FormDBInfo dbInfo {formDBInfos[0]};
    EXPECT_EQ(formId, dbInfo.formId);
    EXPECT_EQ(dataCnt, dbInfo.formUserUids.size());
    // Form host record alloted.
    std::vector<FormHostRecord> hostRecords;
    FormDataMgr::GetInstance().GetFormHostRecord(formId, hostRecords);
    EXPECT_FALSE(hostRecords.empty());

    FormDataMgr::GetInstance().DeleteFormRecord(formId);
    FormDbCache::GetInstance().DeleteFormInfo(formId);
    FormDataMgr::GetInstance().DeleteHostRecord(token_, formId);
    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_001 end";
}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: AddForm Function
 * FunctionPoints: FormMgr AddForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Add form with cache info.
 */
HWTEST_F(FmsFormMgrAddFormTest, AddForm_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_002 start";
    CreateProviderData();

    int64_t formId = 0x0ffabcff00000000;
    int callingUid {0};
    // Set cache
    FormItemInfo record1;
    record1.SetFormId(formId);
    record1.SetProviderBundleName(FORM_PROVIDER_BUNDLE_NAME);
    record1.SetModuleName(PARAM_PROVIDER_MODULE_NAME);
    record1.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    record1.SetFormName(PARAM_FORM_NAME);
    record1.SetSpecificationId(PARAM_FORM_DIMENSION_VALUE);
    record1.SetTemporaryFlag(false);
    FormRecord retFormRec = FormDataMgr::GetInstance().AllotFormRecord(record1, callingUid);
    retFormRec.updateAtHour = 1;
    retFormRec.updateAtMin = 1;
    FormDataMgr::GetInstance().UpdateFormRecord(formId, retFormRec);
    // Set database info
    FormDBInfo formDBInfo(formId, retFormRec);
    FormDbCache::GetInstance().SaveFormInfo(formDBInfo);
    // Set form host record
    FormDataMgr::GetInstance().AllotFormHostRecord(record1, token_, formId, callingUid);

    FormJsInfo formJsInfo;
    Want want;
    want.SetParam(Constants::PARAM_FORM_HOST_BUNDLENAME_KEY, FORM_HOST_BUNDLE_NAME);
    want.SetParam(Constants::PARAM_MODULE_NAME_KEY, PARAM_PROVIDER_MODULE_NAME);
    want.SetParam(Constants::PARAM_FORM_NAME_KEY, PARAM_FORM_NAME);
    want.SetParam(Constants::PARAM_FORM_DIMENSION_KEY, PARAM_FORM_DIMENSION_VALUE);
    want.SetElementName(DEVICE_ID, FORM_PROVIDER_BUNDLE_NAME, FORM_PROVIDER_ABILITY_NAME);
    want.SetParam(Constants::PARAM_FORM_TEMPORARY_KEY, false);
    want.SetParam(Constants::ACQUIRE_TYPE, Constants::ACQUIRE_TYPE_CREATE_FORM);

    GTEST_LOG_(INFO) << "formId :"<<formId;
    EXPECT_EQ(ERR_OK, FormMgr::GetInstance().AddForm(formId, want, token_, formJsInfo));
    token_->Wait();

    size_t dataCnt {1};
    size_t formUserUidCnt {1};
    // Cache params updated.
    FormRecord formInfo;
    bool ret = FormDataMgr::GetInstance().GetFormRecord(formId, formInfo);
    EXPECT_TRUE(ret);
    EXPECT_EQ(formUserUidCnt, formInfo.formUserUids.size());
    // database info updated.
    std::vector<FormDBInfo> formDBInfos;
    FormDbCache::GetInstance().GetAllFormInfo(formDBInfos);
    EXPECT_EQ(dataCnt, formDBInfos.size());
    FormDBInfo dbInfo {formDBInfos[0]};
    EXPECT_EQ(formId, dbInfo.formId);
    EXPECT_EQ(formUserUidCnt, dbInfo.formUserUids.size());
    // Form host record not changed.
    std::vector<FormHostRecord> hostRecords;
    FormDataMgr::GetInstance().GetFormHostRecord(formId, hostRecords);
    EXPECT_FALSE(hostRecords.empty());

    FormDataMgr::GetInstance().DeleteFormRecord(formId);
    FormDbCache::GetInstance().DeleteFormInfo(formId);
    FormDataMgr::GetInstance().DeleteHostRecord(token_, formId);

    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_002 end";
}
/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: AddForm Function
 * FunctionPoints: FormMgr AddForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Add form with database info but without cache.
 */
HWTEST_F(FmsFormMgrAddFormTest, AddForm_003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_003 start";
    CreateProviderData();

    int64_t formId = 0x0ffabcdf00000000;
    int callingUid {0};
    // Set database info
    FormRecord record1;
    record1.formId = formId;
    record1.bundleName = FORM_PROVIDER_BUNDLE_NAME;
    record1.moduleName = PARAM_FORM_NAME;
    record1.abilityName = FORM_PROVIDER_ABILITY_NAME;
    record1.formName = PARAM_FORM_NAME;
    record1.specification = PARAM_FORM_DIMENSION_VALUE;
    record1.formUserUids.emplace_back(callingUid);
    record1.formTempFlag = false;
    FormDBInfo formDBInfo(formId, record1);
    FormDbCache::GetInstance().SaveFormInfo(formDBInfo);
    // Set form host record
    FormItemInfo info;
    FormDataMgr::GetInstance().AllotFormHostRecord(info, token_, formId, callingUid);
    FormJsInfo formJsInfo;
    Want want;
    want.SetParam(Constants::PARAM_FORM_HOST_BUNDLENAME_KEY, FORM_HOST_BUNDLE_NAME);
    want.SetParam(Constants::PARAM_MODULE_NAME_KEY, PARAM_PROVIDER_MODULE_NAME);
    want.SetParam(Constants::PARAM_FORM_NAME_KEY, PARAM_FORM_NAME);
    want.SetParam(Constants::PARAM_FORM_DIMENSION_KEY, PARAM_FORM_DIMENSION_VALUE);
    want.SetElementName(DEVICE_ID, FORM_PROVIDER_BUNDLE_NAME, FORM_PROVIDER_ABILITY_NAME);
    want.SetParam(Constants::PARAM_FORM_TEMPORARY_KEY, false);
    want.SetParam(Constants::ACQUIRE_TYPE, Constants::ACQUIRE_TYPE_CREATE_FORM);

    GTEST_LOG_(INFO) << "formId :"<<formId;
    EXPECT_EQ(ERR_OK, FormMgr::GetInstance().AddForm(formId, want, token_, formJsInfo));
    token_->Wait();

    size_t dataCnt {1};
    size_t formUserUidCnt {1};
    // Cache params updated.
    FormRecord formInfo;
    bool ret = FormDataMgr::GetInstance().GetFormRecord(formId, formInfo);
    EXPECT_TRUE(ret);
    EXPECT_EQ(formUserUidCnt, formInfo.formUserUids.size());
    // databse info updated.
    std::vector<FormDBInfo> formDBInfos;
    FormDbCache::GetInstance().GetAllFormInfo(formDBInfos);
    EXPECT_EQ(dataCnt, formDBInfos.size());
    FormDBInfo dbInfo {formDBInfos[0]};
    EXPECT_EQ(formId, dbInfo.formId);
    EXPECT_EQ(formUserUidCnt, dbInfo.formUserUids.size());
    // Form host record not changed.
    std::vector<FormHostRecord> hostRecords;
    FormDataMgr::GetInstance().GetFormHostRecord(formId, hostRecords);
    EXPECT_FALSE(hostRecords.empty());

    FormDataMgr::GetInstance().DeleteFormRecord(formId);
    FormDbCache::GetInstance().DeleteFormInfo(formId);
    FormDataMgr::GetInstance().DeleteHostRecord(token_, formId);

    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_003 end";
}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: AddForm Function
 * FunctionPoints: FormMgr AddForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Invalid case when callertoken is nullptr.
 */
HWTEST_F(FmsFormMgrAddFormTest, AddForm_004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_004 start";
    CreateProviderData();

    int64_t formId = 0x0ffabcde00000000;

    FormJsInfo formJsInfo;
    Want want;
    want.SetParam(Constants::PARAM_FORM_HOST_BUNDLENAME_KEY, FORM_HOST_BUNDLE_NAME);
    want.SetParam(Constants::PARAM_MODULE_NAME_KEY, PARAM_PROVIDER_MODULE_NAME);
    want.SetParam(Constants::PARAM_FORM_NAME_KEY, PARAM_FORM_NAME);
    want.SetParam(Constants::PARAM_FORM_DIMENSION_KEY, PARAM_FORM_DIMENSION_VALUE);
    want.SetElementName(DEVICE_ID, FORM_PROVIDER_BUNDLE_NAME, FORM_PROVIDER_ABILITY_NAME);
    want.SetParam(Constants::PARAM_FORM_TEMPORARY_KEY, false);
    want.SetParam(Constants::ACQUIRE_TYPE, Constants::ACQUIRE_TYPE_CREATE_FORM);

    EXPECT_EQ(ERR_APPEXECFWK_FORM_INVALID_PARAM, FormMgr::GetInstance().AddForm(formId, want, nullptr, formJsInfo));

    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_004 end";
}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: AddForm Function
 * FunctionPoints: FormMgr AddForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: BundleName,AbilityName,moduleName in Want is null separately.
 */
HWTEST_F(FmsFormMgrAddFormTest, AddForm_005, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_005 start";
    CreateProviderData();

    int64_t formId = 0x0ffabcdd00000000;
    FormJsInfo formJsInfo;
    Want want;
    want.SetParam(Constants::PARAM_FORM_HOST_BUNDLENAME_KEY, FORM_HOST_BUNDLE_NAME);
    want.SetParam(Constants::PARAM_MODULE_NAME_KEY, PARAM_PROVIDER_MODULE_NAME);
    want.SetParam(Constants::PARAM_FORM_NAME_KEY, PARAM_FORM_NAME);
    want.SetParam(Constants::PARAM_FORM_DIMENSION_KEY, PARAM_FORM_DIMENSION_VALUE);
    want.SetElementName(DEVICE_ID, "", FORM_PROVIDER_ABILITY_NAME);
    want.SetParam(Constants::PARAM_FORM_TEMPORARY_KEY, false);
    want.SetParam(Constants::ACQUIRE_TYPE, Constants::ACQUIRE_TYPE_CREATE_FORM);
    EXPECT_EQ(ERR_APPEXECFWK_FORM_INVALID_PARAM, FormMgr::GetInstance().AddForm(formId, want, nullptr, formJsInfo));

    want.SetElementName(DEVICE_ID, FORM_PROVIDER_BUNDLE_NAME, "");
    EXPECT_EQ(ERR_APPEXECFWK_FORM_INVALID_PARAM, FormMgr::GetInstance().AddForm(formId, want, nullptr, formJsInfo));

    want.SetElementName(DEVICE_ID, FORM_PROVIDER_BUNDLE_NAME, FORM_PROVIDER_ABILITY_NAME);
    std::string tmp = "";
    want.SetParam(Constants::PARAM_MODULE_NAME_KEY, tmp);
    EXPECT_EQ(ERR_APPEXECFWK_FORM_INVALID_PARAM, FormMgr::GetInstance().AddForm(formId, want, nullptr, formJsInfo));

    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_005 end";
}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: AddForm Function
 * FunctionPoints: FormMgr AddForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Case when cache info is not matched with form.
 */
HWTEST_F(FmsFormMgrAddFormTest, AddForm_006, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_006 start";
    CreateProviderData();

    int64_t formId = 0x0ababcff00000000;
    int callingUid {0};
    // Set cache info .
    FormItemInfo record1;
    record1.SetFormId(formId);
    record1.SetProviderBundleName(FORM_PROVIDER_ABILITY_NAME);
    record1.SetModuleName(PARAM_FORM_NAME);
    record1.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    record1.SetFormName(PARAM_FORM_NAME);
    record1.SetSpecificationId(PARAM_FORM_DIMENSION_VALUE);
    record1.SetTemporaryFlag(false);
    FormRecord retFormRec = FormDataMgr::GetInstance().AllotFormRecord(record1, callingUid);
    // Set database info.
    FormDBInfo formDBInfo(formId, retFormRec);
    FormDbCache::GetInstance().SaveFormInfo(formDBInfo);
    // Set form host record.
    FormItemInfo info;
    FormDataMgr::GetInstance().AllotFormHostRecord(info, token_, 111L, callingUid);

    FormJsInfo formJsInfo;
    Want want;
    want.SetParam(Constants::PARAM_FORM_HOST_BUNDLENAME_KEY, FORM_HOST_BUNDLE_NAME);
    want.SetParam(Constants::PARAM_MODULE_NAME_KEY, PARAM_PROVIDER_MODULE_NAME);
    want.SetParam(Constants::PARAM_FORM_NAME_KEY, PARAM_FORM_NAME);
    want.SetParam(Constants::PARAM_FORM_DIMENSION_KEY, PARAM_FORM_DIMENSION_VALUE);
    want.SetElementName(DEVICE_ID, FORM_PROVIDER_BUNDLE_NAME, FORM_PROVIDER_ABILITY_NAME);
    want.SetParam(Constants::PARAM_FORM_TEMPORARY_KEY, false);
    want.SetParam(Constants::ACQUIRE_TYPE, Constants::ACQUIRE_TYPE_CREATE_FORM);

    GTEST_LOG_(INFO) << "formId :"<<formId;
    EXPECT_EQ(ERR_APPEXECFWK_FORM_CFG_NOT_MATCH_ID, FormMgr::GetInstance().AddForm(formId, want, token_, formJsInfo));
    token_->Wait();

    FormDataMgr::GetInstance().DeleteFormRecord(formId);
    FormDbCache::GetInstance().DeleteFormInfo(formId);
    FormDataMgr::GetInstance().DeleteHostRecord(token_, formId);

    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_006 end";
}
/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: AddForm Function
 * FunctionPoints: FormMgr AddForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Case when temp form is out of limit.
 */
HWTEST_F(FmsFormMgrAddFormTest, AddForm_007, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_007 start";
    CreateProviderData();

    int64_t formId = 0x0ababc5f00000000;
    int callingUid {0}, tempCount = 0;
    // Set cache info .
    FormItemInfo record1[OHOS::AppExecFwk::Constants::MAX_TEMP_FORMS];
    for (; tempCount<OHOS::AppExecFwk::Constants::MAX_TEMP_FORMS; tempCount++) {
        record1[tempCount].SetFormId(formId+tempCount);
        record1[tempCount].SetProviderBundleName(FORM_PROVIDER_ABILITY_NAME);
        record1[tempCount].SetModuleName(PARAM_FORM_NAME);
        record1[tempCount].SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
        record1[tempCount].SetFormName(PARAM_FORM_NAME);
        record1[tempCount].SetSpecificationId(PARAM_FORM_DIMENSION_VALUE);
        record1[tempCount].SetTemporaryFlag(true);
        FormRecord retFormRec = FormDataMgr::GetInstance().AllotFormRecord(record1[tempCount], callingUid);
        // Set database info.
        FormDBInfo formDBInfo(formId+tempCount, retFormRec);
        FormDbCache::GetInstance().SaveFormInfo(formDBInfo);
        // Set form host record.
        FormItemInfo info;
        FormDataMgr::GetInstance().AllotFormHostRecord(info, token_, tempCount, callingUid);
    }

    FormJsInfo formJsInfo;
    Want want;
    want.SetParam(Constants::PARAM_FORM_HOST_BUNDLENAME_KEY, FORM_HOST_BUNDLE_NAME);
    want.SetParam(Constants::PARAM_MODULE_NAME_KEY, PARAM_PROVIDER_MODULE_NAME);
    want.SetParam(Constants::PARAM_FORM_NAME_KEY, PARAM_FORM_NAME);
    want.SetParam(Constants::PARAM_FORM_DIMENSION_KEY, PARAM_FORM_DIMENSION_VALUE);
    want.SetElementName(DEVICE_ID, FORM_PROVIDER_BUNDLE_NAME, FORM_PROVIDER_ABILITY_NAME);
    want.SetParam(Constants::PARAM_FORM_TEMPORARY_KEY, true);
    want.SetParam(Constants::ACQUIRE_TYPE, Constants::ACQUIRE_TYPE_CREATE_FORM);

    GTEST_LOG_(INFO) << "formId :"<<formId;
    EXPECT_EQ(ERR_APPEXECFWK_FORM_MAX_SYSTEM_TEMP_FORMS, FormMgr::GetInstance().AddForm(0, want, token_, formJsInfo));
    token_->Wait();

    FormDataMgr::GetInstance().DeleteFormRecord(formId);
    FormDbCache::GetInstance().DeleteFormInfo(formId);
    FormDataMgr::GetInstance().DeleteHostRecord(token_, formId);

    for (tempCount = 0; tempCount<OHOS::AppExecFwk::Constants::MAX_TEMP_FORMS; tempCount++) {
    FormDataMgr::GetInstance().DeleteFormRecord(formId + tempCount);
    FormDbCache::GetInstance().DeleteFormInfo(formId + tempCount);
    FormDataMgr::GetInstance().DeleteHostRecord(token_, tempCount);
    FormDataMgr::GetInstance().DeleteTempForm(formId + tempCount);
    }
    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_007 end";
}
/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: AddForm Function
 * FunctionPoints: FormMgr AddForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Case when normal form is out of limit.
 */
HWTEST_F(FmsFormMgrAddFormTest, AddForm_008, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_008 start";
    CreateProviderData();
    // clear old data
    FormDataMgr::GetInstance().formRecords_.clear();
    FormDataMgr::GetInstance().tempForms_.clear();
    std::vector<FormDBInfo> oldFormDBInfos;
    FormDbCache::GetInstance().GetAllFormInfo(oldFormDBInfos);
    for (auto oldFormDBInfo: oldFormDBInfos) {
        FormDbCache::GetInstance().DeleteFormInfo(oldFormDBInfo.formId);
    }

    int64_t formId = 0x0ab5bc5f00000000;
    int callingUid {0}, tempCount = 0;
    // Set cache info .
    FormItemInfo record1[Constants::MAX_FORMS+7];
    for (; tempCount<Constants::MAX_FORMS+7; tempCount++) {
        record1[tempCount].SetFormId(formId+tempCount);
        record1[tempCount].SetProviderBundleName(FORM_PROVIDER_BUNDLE_NAME);
        record1[tempCount].SetModuleName(PARAM_FORM_NAME);
        record1[tempCount].SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
        record1[tempCount].SetFormName(PARAM_FORM_NAME);
        record1[tempCount].SetSpecificationId(PARAM_FORM_DIMENSION_VALUE);
        record1[tempCount].SetTemporaryFlag(false);
        FormRecord retFormRec = FormDataMgr::GetInstance().AllotFormRecord(record1[tempCount], callingUid++);
        // Set database info.
        FormDBInfo formDBInfo(formId+tempCount, retFormRec);
        FormDbCache::GetInstance().SaveFormInfo(formDBInfo);
        // Set form host record.
        FormItemInfo info;
        FormDataMgr::GetInstance().AllotFormHostRecord(info, token_, tempCount, callingUid);
    }

    FormJsInfo formJsInfo;
    Want want;
    want.SetParam(Constants::PARAM_FORM_HOST_BUNDLENAME_KEY, FORM_HOST_BUNDLE_NAME);
    want.SetParam(Constants::PARAM_MODULE_NAME_KEY, PARAM_PROVIDER_MODULE_NAME);
    want.SetParam(Constants::PARAM_FORM_NAME_KEY, PARAM_FORM_NAME);
    want.SetParam(Constants::PARAM_FORM_DIMENSION_KEY, PARAM_FORM_DIMENSION_VALUE);
    want.SetElementName(DEVICE_ID, FORM_PROVIDER_BUNDLE_NAME, FORM_PROVIDER_ABILITY_NAME);
    want.SetParam(Constants::PARAM_FORM_TEMPORARY_KEY, false);
    want.SetParam(Constants::ACQUIRE_TYPE, Constants::ACQUIRE_TYPE_CREATE_FORM);

    GTEST_LOG_(INFO) << "formId :"<<formId;
    EXPECT_EQ(ERR_APPEXECFWK_FORM_MAX_SYSTEM_FORMS, FormMgr::GetInstance().AddForm(0, want, token_, formJsInfo));
    token_->Wait();

    FormDataMgr::GetInstance().DeleteFormRecord(formId);
    FormDbCache::GetInstance().DeleteFormInfo(formId);
    FormDataMgr::GetInstance().DeleteHostRecord(token_, formId);

    for (tempCount = 0; tempCount<Constants::MAX_FORMS+7; tempCount++) {
    FormDataMgr::GetInstance().DeleteFormRecord(formId + tempCount);
    FormDbCache::GetInstance().DeleteFormInfo(formId + tempCount);
    FormDataMgr::GetInstance().DeleteHostRecord(token_, tempCount);
    }
    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_008 end";
}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: AddForm Function
 * FunctionPoints: FormMgr AddForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Add form with cache info.
 */
HWTEST_F(FmsFormMgrAddFormTest, AddForm_009, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_009 start";
    CreateProviderData();

    std::vector<FormDBInfo> originDBInfos;
    FormDbCache::GetInstance().GetAllFormInfo(originDBInfos);
    size_t originDbInfoSize = originDBInfos.size();

    int64_t formId = 0x0abcdabc00000000;
    int callingUid {0};
    // Set cache
    FormItemInfo record1;
    record1.SetFormId(formId);
    record1.SetProviderBundleName(FORM_PROVIDER_BUNDLE_NAME);
    record1.SetModuleName(PARAM_PROVIDER_MODULE_NAME);
    record1.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    record1.SetFormName(PARAM_FORM_NAME);
    record1.SetSpecificationId(PARAM_FORM_DIMENSION_VALUE);
    record1.SetTemporaryFlag(false);

    FormInfoMgr::GetInstance().GetOrCreateBundleFromInfo(FORM_PROVIDER_BUNDLE_NAME);
    FormInfo formInfo1;
    formInfo1.moduleName = PARAM_PROVIDER_MODULE_NAME;
    formInfo1.name = PARAM_FORM_NAME;
    formInfo1.abilityName = FORM_PROVIDER_ABILITY_NAME;
    formInfo1.bundleName = FORM_PROVIDER_BUNDLE_NAME;
    formInfo1.supportDimensions.push_back(PARAM_FORM_DIMENSION_VALUE);
    FormInfoStorage formInfoStor;
    formInfoStor.userId = userId_;
    formInfoStor.formInfos.push_back(formInfo1);
    FormInfoMgr::GetInstance().bundleFormInfoMap_[FORM_PROVIDER_BUNDLE_NAME]->formInfoStorages_.push_back(formInfoStor);

    FormRecord retFormRec = FormDataMgr::GetInstance().AllotFormRecord(record1, callingUid);
    retFormRec.updateAtHour = 1;
    retFormRec.updateAtMin = 1;
    FormDataMgr::GetInstance().UpdateFormRecord(formId, retFormRec);
    // Set database info
    FormDBInfo formDBInfo(formId, retFormRec);
    FormDbCache::GetInstance().SaveFormInfo(formDBInfo);
    // Set form host record
    FormDataMgr::GetInstance().AllotFormHostRecord(record1, token_, formId, callingUid);

    FormJsInfo formJsInfo;
    Want want;
    want.SetParam(Constants::PARAM_FORM_HOST_BUNDLENAME_KEY, FORM_HOST_BUNDLE_NAME);
    want.SetParam(Constants::PARAM_MODULE_NAME_KEY, PARAM_PROVIDER_MODULE_NAME);
    want.SetParam(Constants::PARAM_FORM_NAME_KEY, PARAM_FORM_NAME);
    want.SetParam(Constants::PARAM_FORM_DIMENSION_KEY, PARAM_FORM_DIMENSION_VALUE);
    want.SetElementName(DEVICE_ID, FORM_PROVIDER_BUNDLE_NAME, FORM_PROVIDER_ABILITY_NAME);
    want.SetParam(Constants::PARAM_FORM_TEMPORARY_KEY, false);
    want.SetParam(Constants::ACQUIRE_TYPE, Constants::ACQUIRE_TYPE_CREATE_FORM);

    GTEST_LOG_(INFO) << "formId :"<<formId;
    EXPECT_EQ(ERR_OK, FormMgr::GetInstance().AddForm(formId, want, token_, formJsInfo));
    token_->Wait();

    size_t dataCnt {1};
    size_t formUserUidCnt {1};
    size_t zero {0};
    // Cache params updated.
    FormRecord formInfo;
    bool ret = FormDataMgr::GetInstance().GetFormRecord(formId, formInfo);
    EXPECT_TRUE(ret);
    EXPECT_EQ(formUserUidCnt, formInfo.formUserUids.size());
    // database info updated.
    std::vector<FormDBInfo> formDBInfos;
    FormDbCache::GetInstance().GetAllFormInfo(formDBInfos);
    EXPECT_EQ(originDbInfoSize + dataCnt, formDBInfos.size());
    bool formExist = false;
    for (size_t i = 0; i < formDBInfos.size(); i++) {
        if (formId == formDBInfos[i].formId) {
            EXPECT_EQ(formUserUidCnt, formDBInfos[i].formUserUids.size());
            formExist = true;
            break;
        }
    }
    EXPECT_TRUE(formExist);

    // Form host record not changed.
    FormDataMgr::GetInstance().ClearFormRecords();
    FormRecord formInfo2;
    ret = FormDataMgr::GetInstance().GetFormRecord(formId, formInfo2);
    EXPECT_FALSE(ret);
    EXPECT_EQ(zero, formInfo2.formUserUids.size());
    formDBInfos.clear();
    FormDbCache::GetInstance().GetAllFormInfo(formDBInfos);
    EXPECT_EQ(originDbInfoSize + dataCnt, formDBInfos.size());
    formExist = false;
    for (size_t i = 0; i < formDBInfos.size(); i++) {
        if (formId == formDBInfos[i].formId) {
            EXPECT_EQ(formUserUidCnt, formDBInfos[i].formUserUids.size());
            formExist = true;
            break;
        }
    }
    EXPECT_TRUE(formExist);

    EXPECT_EQ(ERR_OK, FormMgr::GetInstance().AddForm(formId, want, token_, formJsInfo));
    token_->Wait();
    FormRecord formInfo3;
    ret = FormDataMgr::GetInstance().GetFormRecord(formId, formInfo3);
    EXPECT_TRUE(ret);
    EXPECT_EQ(formUserUidCnt, formInfo3.formUserUids.size());
    formDBInfos.clear();
    FormDbCache::GetInstance().GetAllFormInfo(formDBInfos);
    EXPECT_EQ(originDbInfoSize + dataCnt, formDBInfos.size());
    formExist = false;
    for (size_t i = 0; i < formDBInfos.size(); i++) {
        if (formId == formDBInfos[i].formId) {
            EXPECT_EQ(formUserUidCnt, formDBInfos[i].formUserUids.size());
            formExist = true;
            break;
        }
    }
    EXPECT_TRUE(formExist);

    FormDataMgr::GetInstance().DeleteFormRecord(formId);
    FormDbCache::GetInstance().DeleteFormInfo(formId);
    FormDataMgr::GetInstance().DeleteHostRecord(token_, formId);

    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_009 end";
}

/**
 * @tc.name: AddForm_010
 * @tc.desc: Add in application form
 * @tc.type: FUNC
 * @tc.require: issueI5MVKZ
 */
HWTEST_F(FmsFormMgrAddFormTest, AddForm_010, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_010 start";
    CreateProviderData();
    // No cache
    FormJsInfo formJsInfo;
    Want want;
    want.SetParam(Constants::PARAM_FORM_HOST_BUNDLENAME_KEY, FORM_PROVIDER_BUNDLE_NAME);
    want.SetParam(Constants::PARAM_MODULE_NAME_KEY, PARAM_PROVIDER_MODULE_NAME);
    want.SetParam(Constants::PARAM_FORM_NAME_KEY, PARAM_FORM_NAME);
    want.SetParam(Constants::PARAM_FORM_DIMENSION_KEY, PARAM_FORM_DIMENSION_VALUE);
    want.SetElementName(DEVICE_ID, FORM_PROVIDER_BUNDLE_NAME, FORM_PROVIDER_ABILITY_NAME);
    want.SetParam(Constants::PARAM_FORM_TEMPORARY_KEY, false);
    want.SetParam(Constants::ACQUIRE_TYPE, Constants::ACQUIRE_TYPE_CREATE_FORM);
    // clear old data
    FormDataMgr::GetInstance().ClearFormRecords();
    std::vector<FormDBInfo> oldFormDBInfos;
    FormDbCache::GetInstance().GetAllFormInfo(oldFormDBInfos);
    for (auto oldFormDBInfo: oldFormDBInfos) {
        FormDbCache::GetInstance().DeleteFormInfo(oldFormDBInfo.formId);
    }

    // add form
    EXPECT_EQ(ERR_OK, FormMgr::GetInstance().AddForm(0L, want, token_, formJsInfo));
    token_->Wait();

    size_t dataCnt {1};
    int64_t formId = formJsInfo.formId;
    // Form record alloted.
    FormRecord formInfo;
    bool ret = FormDataMgr::GetInstance().GetFormRecord(formId, formInfo);
    EXPECT_TRUE(ret);
    EXPECT_EQ(dataCnt, formInfo.formUserUids.size());
    // Database info alloted.
    std::vector<FormDBInfo> formDBInfos;
    FormDbCache::GetInstance().GetAllFormInfo(formDBInfos);
    EXPECT_EQ(dataCnt, formDBInfos.size());
    FormDBInfo dbInfo {formDBInfos[0]};
    EXPECT_EQ(formId, dbInfo.formId);
    EXPECT_EQ(dataCnt, dbInfo.formUserUids.size());
    // Form host record alloted.
    std::vector<FormHostRecord> hostRecords;
    FormDataMgr::GetInstance().GetFormHostRecord(formId, hostRecords);
    EXPECT_FALSE(hostRecords.empty());

    FormSupplyCallback::GetInstance()->RemoveConnection(formId, token_);
    FormDataMgr::GetInstance().DeleteFormRecord(formId);
    FormDbCache::GetInstance().DeleteFormInfo(formId);
    FormDataMgr::GetInstance().DeleteHostRecord(token_, formId);
    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_010 end";
}

/**
 * @tc.name: AddForm_011
 * @tc.desc: Add form with unzipped app
 * @tc.type: FUNC
 * @tc.require: issueI5MVKZ
 */
HWTEST_F(FmsFormMgrAddFormTestExt, AddForm_011, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_011 start";

    CreateProviderData();
    // No cache
    FormJsInfo formJsInfo;
    Want want;
    want.SetParam(Constants::PARAM_FORM_HOST_BUNDLENAME_KEY, FORM_PROVIDER_BUNDLE_NAME);
    want.SetParam(Constants::PARAM_MODULE_NAME_KEY, PARAM_PROVIDER_MODULE_NAME);
    want.SetParam(Constants::PARAM_FORM_NAME_KEY, PARAM_FORM_NAME);
    want.SetParam(Constants::PARAM_FORM_DIMENSION_KEY, PARAM_FORM_DIMENSION_VALUE);
    want.SetElementName(DEVICE_ID, FORM_PROVIDER_BUNDLE_NAME, FORM_PROVIDER_ABILITY_NAME);
    want.SetParam(Constants::PARAM_FORM_TEMPORARY_KEY, false);
    want.SetParam(Constants::ACQUIRE_TYPE, Constants::ACQUIRE_TYPE_CREATE_FORM);
    // clear old data
    FormDataMgr::GetInstance().ClearFormRecords();
    std::vector<FormDBInfo> oldFormDBInfos;
    FormDbCache::GetInstance().GetAllFormInfo(oldFormDBInfos);
    for (auto oldFormDBInfo: oldFormDBInfos) {
        FormDbCache::GetInstance().DeleteFormInfo(oldFormDBInfo.formId);
    }

    BundleInfo bundleInfo;
    FillBundleInfo(FORM_PROVIDER_BUNDLE_NAME, bundleInfo);
    EXPECT_CALL(*mockBundleMgrService, GetBundleInfo(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<2>(bundleInfo), Return(true)));

    // add form
    EXPECT_EQ(ERR_OK, FormMgr::GetInstance().AddForm(0L, want, token_, formJsInfo));
    token_->Wait();

    size_t dataCnt {1};
    int64_t formId = formJsInfo.formId;
    // Form record alloted.
    FormRecord formInfo;
    bool ret = FormDataMgr::GetInstance().GetFormRecord(formId, formInfo);
    EXPECT_TRUE(ret);
    EXPECT_EQ(dataCnt, formInfo.formUserUids.size());
    // Database info alloted.
    std::vector<FormDBInfo> formDBInfos;
    FormDbCache::GetInstance().GetAllFormInfo(formDBInfos);
    EXPECT_EQ(dataCnt, formDBInfos.size());
    FormDBInfo dbInfo {formDBInfos[0]};
    EXPECT_EQ(formId, dbInfo.formId);
    EXPECT_EQ(dataCnt, dbInfo.formUserUids.size());
    // Form host record alloted.
    std::vector<FormHostRecord> hostRecords;
    FormDataMgr::GetInstance().GetFormHostRecord(formId, hostRecords);
    EXPECT_FALSE(hostRecords.empty());

    FormSupplyCallback::GetInstance()->RemoveConnection(formId, token_);
    FormDataMgr::GetInstance().DeleteFormRecord(formId);
    FormDbCache::GetInstance().DeleteFormInfo(formId);
    FormDataMgr::GetInstance().DeleteHostRecord(token_, formId);
    testing::Mock::AllowLeak(mockBundleMgrService);
    GTEST_LOG_(INFO) << "fms_form_mgr_add_form_test_011 end";
}
}