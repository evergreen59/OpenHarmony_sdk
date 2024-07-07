/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#define private public

#include <gtest/gtest.h>

#include "bundle_constants.h"
#include "bundle_data_storage_rdb.h"
#ifdef BUNDLE_FRAMEWORK_DEFAULT_APP
#include "default_app_rdb.h"
#endif
#include "preinstall_data_storage_rdb.h"
#include "rdb_data_manager.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;

namespace {
const std::string DB_PATH = "/data/test/";
const std::string DB_NAME = "rdbTestDb.db";
const std::string TABLE_NAME = "rdbTestTable";
const std::string KEY_ONE = "KEY_ONE";
const std::string VALUE_ONE = "VALUE_ONE";
const std::string KEY_TWO = "KEY_TWO";
const std::string VALUE_TWO = "VALUE_TWO";
const std::string KEY_THREE = "KEY_THREE";
const std::string VALUE_THREE = "VALUE_THREE";
const std::string TEST_BUNDLE_NAME = "com.test.rdbone";
const std::string TEST_NAME = "NameOne";
const uint32_t TEST_VERSION = 1;
const std::string TEST_BUNDLE_NAME_TWO = "com.test.rdbtwo";
const std::string TEST_NAME_TWO = "NameTwo";
const uint32_t TEST_VERSION_TWO = 2;
#ifdef BUNDLE_FRAMEWORK_DEFAULT_APP
const int32_t TEST_USERID = 500;
const std::string TEST_DEFAULT_APP_TYPE = "IMAGE";
#endif

class BmsRdbDataManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::unique_ptr<RdbDataManager> OpenDbAndTable();
    void CloseDb();
};

void BmsRdbDataManagerTest::SetUpTestCase()
{}

void BmsRdbDataManagerTest::TearDownTestCase()
{}

void BmsRdbDataManagerTest::SetUp()
{}

void BmsRdbDataManagerTest::TearDown()
{}

std::unique_ptr<RdbDataManager> BmsRdbDataManagerTest::OpenDbAndTable()
{
    BmsRdbConfig bmsRdbConfig;
    bmsRdbConfig.dbPath = DB_PATH;
    bmsRdbConfig.dbName = DB_NAME;
    bmsRdbConfig.tableName = TABLE_NAME;
    auto rdbDataManager = std::make_unique<RdbDataManager>(bmsRdbConfig);
    rdbDataManager->CreateTable();
    return rdbDataManager;
}

void BmsRdbDataManagerTest::CloseDb()
{
    OHOS::NativeRdb::RdbHelper::DeleteRdbStore(DB_PATH + DB_NAME);
}

/**
 * @tc.number: RdbDataManager_0100
 * @tc.name: insert query delete
 * @tc.desc: 1.insert data
 *           2.query data
 *           3.delete data
 * @tc.require: issueI56W8B
 */
HWTEST_F(BmsRdbDataManagerTest, RdbDataManager_0100, Function | SmallTest | Level1)
{
    auto rdbDataManager = OpenDbAndTable();
    EXPECT_TRUE(rdbDataManager != nullptr);

    bool ret = rdbDataManager->InsertData(KEY_ONE, VALUE_ONE);
    EXPECT_TRUE(ret);

    std::string value;
    ret = rdbDataManager->QueryData(KEY_ONE, value);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(value == VALUE_ONE);

    ret = rdbDataManager->UpdateData(KEY_ONE, VALUE_TWO);
    EXPECT_TRUE(ret);

    ret = rdbDataManager->QueryData(KEY_ONE, value);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(value == VALUE_TWO);

    ret = rdbDataManager->DeleteData(KEY_ONE);
    EXPECT_TRUE(ret);

    ret = rdbDataManager->QueryData(KEY_ONE, value);
    EXPECT_FALSE(ret);
    CloseDb();
}

/**
 * @tc.number: RdbDataManager_0200
 * @tc.name: insert queryAll
 * @tc.desc: 1.insert data
 *           2.queryAll data
 * @tc.require: issueI56W8B
 */
HWTEST_F(BmsRdbDataManagerTest, RdbDataManager_0200, Function | SmallTest | Level1)
{
    auto rdbDataManager = OpenDbAndTable();
    EXPECT_TRUE(rdbDataManager != nullptr);

    bool ret = rdbDataManager->InsertData(KEY_ONE, VALUE_ONE);
    EXPECT_TRUE(ret);

    ret = rdbDataManager->InsertData(KEY_TWO, VALUE_TWO);
    EXPECT_TRUE(ret);

    ret = rdbDataManager->InsertData(KEY_THREE, VALUE_THREE);
    EXPECT_TRUE(ret);

    std::map<std::string, std::string> datas;
    ret = rdbDataManager->QueryAllData(datas);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(datas[KEY_ONE] == VALUE_ONE);
    EXPECT_TRUE(datas[KEY_TWO] == VALUE_TWO);
    EXPECT_TRUE(datas[KEY_THREE] == VALUE_THREE);
    CloseDb();
}

/**
 * @tc.number: BundleDataStorageRdb_0100
 * @tc.name: save and delete
 * @tc.desc: 1.SaveStorageBundleInfo
 *           2.DeleteStorageBundleInfo
 * @tc.require: issueI56W8B
 */
HWTEST_F(BmsRdbDataManagerTest, BundleDataStorageRdb_0100, Function | SmallTest | Level1)
{
    std::unique_ptr<IBundleDataStorage> dataStorage = std::make_unique<BundleDataStorageRdb>();
    ApplicationInfo applicationInfo;
    applicationInfo.bundleName = TEST_BUNDLE_NAME;
    applicationInfo.label = TEST_NAME;
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.SetBaseApplicationInfo(applicationInfo);
    bool ret = dataStorage->SaveStorageBundleInfo(innerBundleInfo);
    EXPECT_TRUE(ret);

    ret = dataStorage->DeleteStorageBundleInfo(innerBundleInfo);
    EXPECT_TRUE(ret);
}

/**
 * @tc.number: BundleDataStorageRdb_0200
 * @tc.name: LoadAllData
 * @tc.desc: 1.LoadAllData
 * @tc.require: issueI56W8B
 */
HWTEST_F(BmsRdbDataManagerTest, BundleDataStorageRdb_0200, Function | SmallTest | Level1)
{
    std::shared_ptr<IBundleDataStorage> dataStorage = std::make_shared<BundleDataStorageRdb>();
    ApplicationInfo applicationInfo;
    applicationInfo.bundleName = TEST_BUNDLE_NAME;
    applicationInfo.name = TEST_NAME;
    InnerBundleInfo innerBundleInfo1;
    innerBundleInfo1.SetBaseApplicationInfo(applicationInfo);
    bool ret = dataStorage->SaveStorageBundleInfo(innerBundleInfo1);
    EXPECT_TRUE(ret);

    applicationInfo.bundleName = TEST_BUNDLE_NAME_TWO;
    applicationInfo.name = TEST_NAME_TWO;
    InnerBundleInfo innerBundleInfo2;
    innerBundleInfo2.SetBaseApplicationInfo(applicationInfo);
    ret = dataStorage->SaveStorageBundleInfo(innerBundleInfo2);
    EXPECT_TRUE(ret);

    std::map<std::string, InnerBundleInfo> infos;
    ret = dataStorage->LoadAllData(infos);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(infos.empty());
    EXPECT_TRUE(infos[TEST_BUNDLE_NAME].GetApplicationName() == TEST_NAME);
    EXPECT_TRUE(infos[TEST_BUNDLE_NAME_TWO].GetApplicationName() == TEST_NAME_TWO);
    dataStorage->DeleteStorageBundleInfo(innerBundleInfo1);
    dataStorage->DeleteStorageBundleInfo(innerBundleInfo2);
}

/**
 * @tc.number: PreInstallDataStorageRdb_0100
 * @tc.name: save and delete
 * @tc.desc: 1.SavePreInstallStorageBundleInfo
 *           2.DeletePreInstallStorageBundleInfo
 * @tc.require: issueI56W8B
 */
HWTEST_F(BmsRdbDataManagerTest, PreInstallDataStorageRdb_0100, Function | SmallTest | Level1)
{
    std::unique_ptr<IPreInstallDataStorage> preInstallDataStorage =
        std::make_unique<PreInstallDataStorageRdb>();
    PreInstallBundleInfo preInstallBundleInfo;
    preInstallBundleInfo.SetBundleName(TEST_BUNDLE_NAME);
    preInstallBundleInfo.SetVersionCode(TEST_VERSION);
    bool ret = preInstallDataStorage->SavePreInstallStorageBundleInfo(preInstallBundleInfo);
    EXPECT_TRUE(ret);
    ret = preInstallDataStorage->DeletePreInstallStorageBundleInfo(preInstallBundleInfo);
    EXPECT_TRUE(ret);
}

/**
 * @tc.number: PreInstallDataStorageRdb_0200
 * @tc.name: LoadAllPreInstallBundleInfos
 * @tc.desc: 1.LoadAllPreInstallBundleInfos
 * @tc.require: issueI56WB8
 */
HWTEST_F(BmsRdbDataManagerTest, PreInstallDataStorageRdb_0200, Function | SmallTest | Level1)
{
    std::unique_ptr<IPreInstallDataStorage> preInstallDataStorage =
        std::make_unique<PreInstallDataStorageRdb>();
    PreInstallBundleInfo preInstallBundleInfo1;
    preInstallBundleInfo1.SetBundleName(TEST_BUNDLE_NAME);
    preInstallBundleInfo1.SetVersionCode(TEST_VERSION);
    bool ret = preInstallDataStorage->SavePreInstallStorageBundleInfo(preInstallBundleInfo1);
    EXPECT_TRUE(ret);

    PreInstallBundleInfo preInstallBundleInfo2;
    preInstallBundleInfo2.SetBundleName(TEST_BUNDLE_NAME_TWO);
    preInstallBundleInfo2.SetVersionCode(TEST_VERSION_TWO);
    ret = preInstallDataStorage->SavePreInstallStorageBundleInfo(preInstallBundleInfo2);
    EXPECT_TRUE(ret);

    std::vector<PreInstallBundleInfo> preInstallBundleInfos;
    ret = preInstallDataStorage->LoadAllPreInstallBundleInfos(preInstallBundleInfos);
    EXPECT_TRUE(ret);
}

/**
 * @tc.number: DefaultAppRdb_0100
 * @tc.name: save get and delete
 * @tc.desc: 1.SetDefaultApplicationInfo
 *           2.GetDefaultApplicationInfo
 *           3.DeleteDefaultApplicationInfos
 * @tc.require: issueI56W8B
 */
HWTEST_F(BmsRdbDataManagerTest, DefaultAppRdb_0100, Function | SmallTest | Level1)
{
#ifdef BUNDLE_FRAMEWORK_DEFAULT_APP
    std::unique_ptr<IDefaultAppDb> defaultAppDb = std::make_unique<DefaultAppRdb>();
    Element element1;
    element1.bundleName = TEST_BUNDLE_NAME;
    bool ret = defaultAppDb->SetDefaultApplicationInfo(TEST_USERID, TEST_DEFAULT_APP_TYPE, element1);
    EXPECT_TRUE(ret);

    Element element2;
    ret = defaultAppDb->GetDefaultApplicationInfo(TEST_USERID, TEST_DEFAULT_APP_TYPE, element2);
    EXPECT_TRUE(ret);

    ret = defaultAppDb->DeleteDefaultApplicationInfos(TEST_USERID);
    EXPECT_TRUE(ret);
#endif
}

/**
 * @tc.number: DefaultAppRdb_0200
 * @tc.name: save get and delete
 * @tc.desc: 1.SetDefaultApplicationInfos
 * @tc.require: issueI56W8B
 */
HWTEST_F(BmsRdbDataManagerTest, DefaultAppRdb_0200, Function | SmallTest | Level1)
{
    DefaultAppRdb defaultAppRdb;
    std::map<std::string, Element> infos;
    defaultAppRdb.rdbDataManager_ = nullptr;
    bool ret = defaultAppRdb.SetDefaultApplicationInfos(Constants::INVALID_UID, infos);
    EXPECT_FALSE(ret);
    ret = defaultAppRdb.DeleteDefaultApplicationInfos(Constants::INVALID_UID);
    EXPECT_FALSE(ret);
    ret = defaultAppRdb.DeleteDefaultApplicationInfo(Constants::INVALID_UID, "");
    EXPECT_TRUE(ret);
}

/**
 * @tc.number: DefaultAppRdb_0300
 * @tc.name: save get and delete
 * @tc.desc: 1.SetDefaultApplicationInfos
 * @tc.require: issueI56W8B
 */
HWTEST_F(BmsRdbDataManagerTest, DefaultAppRdb_0300, Function | SmallTest | Level1)
{
    DefaultAppRdb defaultAppRdb;
    std::map<std::string, Element> infos;
    Element element;
    element.bundleName = TEST_BUNDLE_NAME;
    defaultAppRdb.rdbDataManager_ = nullptr;
    bool ret = defaultAppRdb.SetDefaultApplicationInfo(Constants::INVALID_UID, "", element);
    EXPECT_FALSE(ret);
}
}  // namespace