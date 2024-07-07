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
#include "form_host_interface.h"
#define private public
#include "form_mgr.h"
#undef private
#include "form_mgr_errors.h"
#include "form_mgr_adapter.h"
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

class FmsFormMgrUpdateFormTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    sptr<MockFormHostClient> token_;
    std::shared_ptr<FormMgrService> formyMgrServ_ = DelayedSingleton<FormMgrService>::GetInstance();
};

void FmsFormMgrUpdateFormTest::SetUpTestCase()
{
    RemoteNativeToken::SetNativeToken();
    FormBmsHelper::GetInstance().SetBundleManager(new BundleMgrService());
    FormAmsHelper::GetInstance().SetAbilityManager(new MockAbilityMgrService());
}

void FmsFormMgrUpdateFormTest::TearDownTestCase()
{}

void FmsFormMgrUpdateFormTest::SetUp()
{
    HILOG_INFO("fms_form_mgr_client_updateForm_test_001 setup");
    formyMgrServ_->OnStart();
    token_ = new (std::nothrow) MockFormHostClient();

    // Permission install
    int userId = 0;
    auto tokenId = AccessToken::AccessTokenKit::GetHapTokenID(userId, FORM_PROVIDER_BUNDLE_NAME, 0);
    auto flag = OHOS::Security::AccessToken::PERMISSION_USER_FIXED;
    AccessToken::AccessTokenKit::GrantPermission(tokenId, PERMISSION_NAME_REQUIRE_FORM, flag);
}

void FmsFormMgrUpdateFormTest::TearDown()
{}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: UpdateForm Function
 * FunctionPoints: FormMgr UpdateForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if FormMgr invoke UpdateForm works.
 */
HWTEST_F(FmsFormMgrUpdateFormTest, UpdateForm_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_client_updateForm_test_001 start";

    // param editor
    int64_t formId {100L};
    int32_t callingUid {20000001};
    std::string bandleName = FORM_PROVIDER_BUNDLE_NAME;
    FormProviderData formProviderData = FormProviderData(std::string("{\"city\": \"beijing001\"}"));

    // add formRecord
    FormItemInfo formItemInfo;
    formItemInfo.SetFormId(formId);
    formItemInfo.SetProviderBundleName(FORM_PROVIDER_BUNDLE_NAME);
    formItemInfo.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    formItemInfo.SetTemporaryFlag(false);
    FormDataMgr::GetInstance().AllotFormRecord(formItemInfo, callingUid);

    FormItemInfo formItemInfo1;
    formItemInfo1.SetFormId(1000L);
    formItemInfo1.SetProviderBundleName(FORM_PROVIDER_BUNDLE_NAME);
    formItemInfo1.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    formItemInfo1.SetTemporaryFlag(true);
    FormDataMgr::GetInstance().AllotFormRecord(formItemInfo1, callingUid);

    // add formHostRecord
    FormItemInfo itemInfo;
    FormDataMgr::GetInstance().AllotFormHostRecord(itemInfo, token_, formId, callingUid);

    // test exec
    EXPECT_EQ(ERR_OK, FormMgr::GetInstance().UpdateForm(formId, formProviderData));

    token_->Wait();

    GTEST_LOG_(INFO) << "fms_form_mgr_client_updateForm_test_001 end";
}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: UpdateForm Function
 * FunctionPoints: FormMgr UpdateForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if FormMgr invoke UpdateForm works when passing bandleName is empty.
 */
HWTEST_F(FmsFormMgrUpdateFormTest, UpdateForm_003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_client_updateForm_test_003 start";

    // param editor
    int64_t formId {300L};
    int32_t callingUid {0};
    std::string bandleName = "";
    FormProviderData formProviderData;

    // add formRecord
    FormItemInfo formItemInfo;
    formItemInfo.SetFormId(formId);
    formItemInfo.SetProviderBundleName(FORM_PROVIDER_BUNDLE_NAME);
    formItemInfo.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    formItemInfo.SetTemporaryFlag(false);
    FormDataMgr::GetInstance().AllotFormRecord(formItemInfo, callingUid);

    // add formHostRecord
    FormItemInfo itemInfo;
    FormDataMgr::GetInstance().AllotFormHostRecord(itemInfo, token_, formId, callingUid);

    // test exec
    EXPECT_EQ(ERR_APPEXECFWK_FORM_INVALID_PARAM,
        FormMgrAdapter::GetInstance().UpdateForm(formId, bandleName, formProviderData));

    GTEST_LOG_(INFO) << "fms_form_mgr_client_updateForm_test_003 end";
}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: UpdateForm Function
 * FunctionPoints: FormMgr UpdateForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if FormMgr invoke UpdateForm works when bandleName not match.
 */
HWTEST_F(FmsFormMgrUpdateFormTest, UpdateForm_004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_client_updateForm_test_004 start";

    // param editor
    int64_t formId {400L};
    int32_t callingUid {0};
    std::string bandleName = FORM_PROVIDER_BUNDLE_NAME;
    FormProviderData formProviderData = FormProviderData(std::string("{\"city\": \"beijing004\"}"));

    // add formRecord
    FormItemInfo formItemInfo;
    formItemInfo.SetFormId(formId);
    formItemInfo.SetProviderBundleName(FORM_HOST_BUNDLE_NAME);
    formItemInfo.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    formItemInfo.SetTemporaryFlag(false);
    FormDataMgr::GetInstance().AllotFormRecord(formItemInfo, callingUid);

    // add formHostRecord
    FormItemInfo itemInfo;
    FormDataMgr::GetInstance().AllotFormHostRecord(itemInfo, token_, formId, callingUid);

    // test exec
    EXPECT_EQ(ERR_APPEXECFWK_FORM_INVALID_PARAM, FormMgr::GetInstance().UpdateForm(formId, formProviderData));

    GTEST_LOG_(INFO) << "fms_form_mgr_client_updateForm_test_004 end";
}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: UpdateForm Function
 * FunctionPoints: FormMgr UpdateForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if FormMgr invoke UpdateForm works when not under current user.
 */
HWTEST_F(FmsFormMgrUpdateFormTest, UpdateForm_005, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_client_updateForm_test_005 start";

    // param editor
    int64_t formId {500L};
    int32_t callingUid {1};
    std::string bandleName = FORM_HOST_BUNDLE_NAME;
    FormProviderData formProviderData = FormProviderData(std::string("{\"city\": \"beijing005\"}"));

    // add formRecord
    FormItemInfo formItemInfo;
    formItemInfo.SetFormId(formId);
    formItemInfo.SetProviderBundleName(FORM_HOST_BUNDLE_NAME);
    formItemInfo.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    formItemInfo.SetTemporaryFlag(false);
    FormDataMgr::GetInstance().AllotFormRecord(formItemInfo, callingUid);

    // add formHostRecord
    FormItemInfo itemInfo;
    FormDataMgr::GetInstance().AllotFormHostRecord(itemInfo, token_, formId, callingUid);

    // test exec
    EXPECT_EQ(ERR_APPEXECFWK_FORM_INVALID_PARAM, FormMgr::GetInstance().UpdateForm(formId, formProviderData));

    GTEST_LOG_(INFO) << "fms_form_mgr_client_updateForm_test_005 end";
}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: UpdateForm Function
 * FunctionPoints: FormMgr UpdateForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if FormMgr invoke UpdateForm works when the updated form is not your own.
 */
HWTEST_F(FmsFormMgrUpdateFormTest, UpdateForm_006, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_client_updateForm_test_006 start";

    // param editor
    int64_t formId {600L};
    int32_t callingUid {0};
    std::string bandleName = "com.form.host.app600";
    FormProviderData formProviderData = FormProviderData(std::string("{\"city\": \"beijing006\"}"));

    // add formRecord
    FormItemInfo formItemInfo;
    formItemInfo.SetFormId(formId);
    formItemInfo.SetProviderBundleName(FORM_HOST_BUNDLE_NAME);
    formItemInfo.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    formItemInfo.SetTemporaryFlag(false);
    FormDataMgr::GetInstance().AllotFormRecord(formItemInfo, callingUid);

    // add formHostRecord
    FormItemInfo itemInfo;
    FormDataMgr::GetInstance().AllotFormHostRecord(itemInfo, token_, formId, callingUid);

    // test exec
    EXPECT_EQ(ERR_APPEXECFWK_FORM_INVALID_PARAM, FormMgr::GetInstance().UpdateForm(formId, formProviderData));

    GTEST_LOG_(INFO) << "fms_form_mgr_client_updateForm_test_006 end";
}

/*
 * Feature: FormMgrService
 * Function: FormMgr
 * SubFunction: UpdateForm Function
 * FunctionPoints: FormMgr UpdateForm interface
 * EnvConditions: Mobile that can run ohos test framework
 * CaseDescription: Verify if FormMgr invoke UpdateForm works.
 */
HWTEST_F(FmsFormMgrUpdateFormTest, UpdateForm_007, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "fms_form_mgr_client_updateForm_test_007 start";

    // param editor
    int64_t formId {700L};
    int32_t callingUid {0};
    std::string bandleName = FORM_PROVIDER_BUNDLE_NAME;
    std::string jsonData = std::string("{");
    for (int i = 0; i < 1024; i = i + 1) {
        jsonData = jsonData + std::string("\"city" + std::to_string(i) + "\"" + ":" + "\"beijing007\"");
        if (i != 1023) {
            jsonData = jsonData + std::string(", ");
        }
    }
    jsonData = jsonData + std::string("}");
    FormProviderData formProviderData = FormProviderData(jsonData);

    // add formRecord
    FormItemInfo formItemInfo;
    formItemInfo.SetFormId(formId);
    formItemInfo.SetProviderBundleName(FORM_PROVIDER_BUNDLE_NAME);
    formItemInfo.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    formItemInfo.SetTemporaryFlag(false);
    FormRecord formRecord = FormDataMgr::GetInstance().AllotFormRecord(formItemInfo, callingUid);
    formRecord.versionUpgrade = true;

    FormItemInfo formItemInfo1;
    formItemInfo1.SetFormId(7000L);
    formItemInfo1.SetProviderBundleName(FORM_HOST_BUNDLE_NAME);
    formItemInfo1.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    formItemInfo1.SetTemporaryFlag(true);
    FormRecord formRecord1 = FormDataMgr::GetInstance().AllotFormRecord(formItemInfo1, callingUid);

    // add formHostRecord
    FormItemInfo itemInfo;
    FormDataMgr::GetInstance().AllotFormHostRecord(itemInfo, token_, formId, callingUid);

    // test exec
    EXPECT_EQ(ERR_OK, FormMgr::GetInstance().UpdateForm(formId, formProviderData));

    token_->Wait();

    GTEST_LOG_(INFO) << "fms_form_mgr_client_updateForm_test_007 end";
}

/**
 * @tc.name: UpdateForm_008
 * @tc.desc: Verify linkage update
 * @tc.type: FUNC
 * @tc.require: issueI5KIZC
 */
HWTEST_F(FmsFormMgrUpdateFormTest, UpdateForm_008, TestSize.Level1) {
    GTEST_LOG_(INFO) << "UpdateForm_008 starts";
    // param editor
    int64_t formId {100L};
    int32_t callingUid {20000001};
    FormProviderData formProviderData = FormProviderData(std::string("{\"city\": \"beijing001\"}"));

    // add formRecord
    FormItemInfo formItemInfo;
    formItemInfo.SetFormId(formId);
    formItemInfo.SetProviderBundleName(FORM_PROVIDER_BUNDLE_NAME);
    formItemInfo.SetAbilityName(FORM_PROVIDER_ABILITY_NAME);
    formItemInfo.SetTemporaryFlag(false);
    FormDataMgr::GetInstance().AllotFormRecord(formItemInfo, callingUid);

    // add formHostRecord
    FormItemInfo itemInfo;
    itemInfo.SetHostBundleName(FORM_HOST_BUNDLE_NAME);
    FormDataMgr::GetInstance().AllotFormHostRecord(itemInfo, token_, formId, callingUid);
    FormItemInfo itemInfo1;
    itemInfo1.SetHostBundleName(FORM_PROVIDER_BUNDLE_NAME);
    FormDataMgr::GetInstance().AllotFormHostRecord(itemInfo, token_, formId, callingUid);

    // test exec
    EXPECT_EQ(ERR_OK, FormMgr::GetInstance().UpdateForm(formId, formProviderData));

    token_->Wait();

    GTEST_LOG_(INFO) << "UpdateForm_008 test ends";
}
}