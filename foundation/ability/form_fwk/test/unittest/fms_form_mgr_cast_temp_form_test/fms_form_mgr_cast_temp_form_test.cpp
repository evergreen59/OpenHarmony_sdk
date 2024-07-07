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

const int32_t PARAM_FORM_DIMENSION_VALUE = 1;

const std::string DEVICE_ID = "ohos-phone1";
const std::string DEF_LABEL1 = "PermissionFormRequireGrant";

class FmsFormMgrCastTempFormTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    sptr<MockFormHostClient> token_;
    std::shared_ptr<FormMgrService> formyMgrServ_ = DelayedSingleton<FormMgrService>::GetInstance();
};

void FmsFormMgrCastTempFormTest::SetUpTestCase()
{
    RemoteNativeToken::SetNativeToken();
    FormBmsHelper::GetInstance().SetBundleManager(new BundleMgrService());
    FormAmsHelper::GetInstance().SetAbilityManager(new MockAbilityMgrService());
}

void FmsFormMgrCastTempFormTest::TearDownTestCase()
{}

void FmsFormMgrCastTempFormTest::SetUp()
{
    formyMgrServ_->OnStart();
    token_ = new (std::nothrow) MockFormHostClient();

    // Permission install
    int userId = 0;
    auto tokenId = AccessToken::AccessTokenKit::GetHapTokenID(userId, FORM_PROVIDER_BUNDLE_NAME, 0);
    auto flag = OHOS::Security::AccessToken::PERMISSION_USER_FIXED;
    AccessToken::AccessTokenKit::GrantPermission(tokenId, PERMISSION_NAME_REQUIRE_FORM, flag);
}

void FmsFormMgrCastTempFormTest::TearDown()
{}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: CastTempForm Function
 * FunctionPoints: FormMgr CastTempForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if FormMgr invoke CastTempForm works.
 */
HWTEST_F(FmsFormMgrCastTempFormTest, CastTempForm_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_cast_temp_form_test_001 start";

    std::vector<FormDBInfo> originDBInfos;
    FormDbCache::GetInstance().GetAllFormInfo(originDBInfos);
    size_t originDbInfoSize = originDBInfos.size();

    int64_t formId {FormDataMgr::GetInstance().GenerateFormId()};
    int callingUid {1};
    // cache info
    FormItemInfo record1;
    record1.SetFormId(formId);
    record1.SetProviderBundleName(FORM_PROVIDER_BUNDLE_NAME);
    record1.SetModuleName(PARAM_FORM_NAME);
    record1.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    record1.SetFormName(PARAM_FORM_NAME);
    record1.SetSpecificationId(PARAM_FORM_DIMENSION_VALUE);
    record1.SetTemporaryFlag(true);
    FormRecord retFormRec = FormDataMgr::GetInstance().AllotFormRecord(record1, callingUid);
    // host object
    FormItemInfo info;
    FormDataMgr::GetInstance().AllotFormHostRecord(info, token_, formId, callingUid);

    ASSERT_EQ(ERR_OK, FormMgr::GetInstance().CastTempForm(formId, token_));
    token_->Wait();

    size_t dataCnt {1};
    // form record is updated
    FormRecord formInfo;
    bool ret = FormDataMgr::GetInstance().GetFormRecord(formId, formInfo);
    EXPECT_TRUE(ret);
    size_t userUidCnt {2};
    EXPECT_EQ(userUidCnt, formInfo.formUserUids.size());
    // db data is added
    std::vector<FormDBInfo> formDBInfos;
    FormDbCache::GetInstance().GetAllFormInfo(formDBInfos);
    EXPECT_EQ(originDbInfoSize + dataCnt, formDBInfos.size());
    bool formExist = false;
    for (size_t i = 0; i < formDBInfos.size(); i++) {
        if (formId == formDBInfos[i].formId) {
            EXPECT_EQ(userUidCnt, formDBInfos[i].formUserUids.size());
            formExist = true;
            break;
        }
    }
    EXPECT_TRUE(formExist);
    // host is added
    std::vector<FormHostRecord> hostRecords;
    FormDataMgr::GetInstance().GetFormHostRecord(formId, hostRecords);
    EXPECT_FALSE(hostRecords.empty());

    FormDataMgr::GetInstance().DeleteFormRecord(formId);
    FormDbCache::GetInstance().DeleteFormInfo(formId);
    FormDataMgr::GetInstance().DeleteHostRecord(token_, formId);
    GTEST_LOG_(INFO) << "fms_form_mgr_cast_temp_form_test_001 end";
}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: CastTempForm Function
 * FunctionPoints: FormMgr CastTempForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: form id <= 0 or Caller ability token is nullptr.
 */
HWTEST_F(FmsFormMgrCastTempFormTest, CastTempForm_002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_cast_temp_form_test_002 start";
    // form id <= 0
    ASSERT_EQ(ERR_APPEXECFWK_FORM_INVALID_FORM_ID, FormMgr::GetInstance().CastTempForm(0L, token_));
    // Caller ability token is nullptr
    int64_t formId {FormDataMgr::GetInstance().GenerateFormId()};
    ASSERT_EQ(ERR_APPEXECFWK_FORM_INVALID_PARAM, FormMgr::GetInstance().CastTempForm(formId, nullptr));
    GTEST_LOG_(INFO) << "fms_form_mgr_cast_temp_form_test_002 end";
}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: CastTempForm Function
 * FunctionPoints: FormMgr CastTempForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Temp form is not in form cache.
 */
HWTEST_F(FmsFormMgrCastTempFormTest, CastTempForm_003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_cast_temp_form_test_003 start";
    int64_t formId {FormDataMgr::GetInstance().GenerateFormId()};
    // form is not exist in cache
    ASSERT_EQ(ERR_APPEXECFWK_FORM_NOT_EXIST_ID, FormMgr::GetInstance().CastTempForm(formId, token_));
    GTEST_LOG_(INFO) << "fms_form_mgr_cast_temp_form_test_003 end";
}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: CastTempForm Function
 * FunctionPoints: FormMgr CastTempForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Temp form is not in host.
 */
HWTEST_F(FmsFormMgrCastTempFormTest, CastTempForm_004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_cast_temp_form_test_004 start";

    int64_t formId {FormDataMgr::GetInstance().GenerateFormId()};
    int callingUid {1};
    // cache data
    FormItemInfo record1;
    record1.SetFormId(formId);
    record1.SetProviderBundleName(FORM_PROVIDER_BUNDLE_NAME);
    record1.SetModuleName(PARAM_FORM_NAME);
    record1.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    record1.SetFormName(PARAM_FORM_NAME);
    record1.SetSpecificationId(PARAM_FORM_DIMENSION_VALUE);
    record1.SetTemporaryFlag(true);
    FormRecord retFormRec = FormDataMgr::GetInstance().AllotFormRecord(record1, callingUid);

    ASSERT_EQ(ERR_APPEXECFWK_FORM_OPERATION_NOT_SELF, FormMgr::GetInstance().CastTempForm(formId, token_));

    FormDataMgr::GetInstance().DeleteFormRecord(formId);
    FormDbCache::GetInstance().DeleteFormInfo(formId);
    FormDataMgr::GetInstance().DeleteHostRecord(token_, formId);
    GTEST_LOG_(INFO) << "fms_form_mgr_cast_temp_form_test_004 end";
}
}
