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

#include <gtest/gtest.h>
#include <vector>

#include "ipc/file_stat.h"
#include "installd/installd_host_impl.h"
#include "installd/installd_operator.h"
#include "ipc/installd_proxy.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;
namespace OHOS {
namespace {
std::string TEST_STRING = "test.string";
const std::string TEST_CPU_ABI = "arm64";
const std::string HAP_FILE_PATH =
    "/data/app/el1/bundle/public/com.example.test/entry.hap";
const std::string TEST_PATH = "/data/app/el1/bundle/public/com.example.test/";
const std::string TEST_PATH_PHOTOS = "/data/app/el1/bundle/public/com.ohos.photos/entry";
const std::string TEST_PATH_TARGET = "/data/test";
const std::string OVER_MAX_PATH_SIZE(300, 'x');
}; // namespace
class BmsInstallDaemonHostImplTest : public testing::Test {
public:
    BmsInstallDaemonHostImplTest();
    ~BmsInstallDaemonHostImplTest();
    static void SetUpTestCase();
    static void TearDownTestCase();
    sptr<InstalldHostImpl> GetInstalldHostImpl();
    void SetUp();
    void TearDown();

private:
    sptr<InstalldHostImpl> hostImpl_ = nullptr;
};

BmsInstallDaemonHostImplTest::BmsInstallDaemonHostImplTest()
{}

BmsInstallDaemonHostImplTest::~BmsInstallDaemonHostImplTest()
{}

void BmsInstallDaemonHostImplTest::SetUpTestCase()
{
}

void BmsInstallDaemonHostImplTest::TearDownTestCase()
{}

void BmsInstallDaemonHostImplTest::SetUp()
{}

void BmsInstallDaemonHostImplTest::TearDown()
{}

sptr<InstalldHostImpl> BmsInstallDaemonHostImplTest::GetInstalldHostImpl()
{
    if (hostImpl_ != nullptr) {
        return hostImpl_;
    }
    hostImpl_ = new (std::nothrow) InstalldHostImpl();
    if (hostImpl_ == nullptr || hostImpl_->AsObject() == nullptr) {
        return nullptr;
    }
    return hostImpl_;
}

/**
 * @tc.number: InstalldHostImplTest_
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling CreateBundleDir of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->CreateBundleDir(TEST_STRING);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_0200
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling ExtractModuleFiles of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_0200, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->ExtractModuleFiles(TEST_STRING, TEST_STRING, TEST_STRING, TEST_STRING);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_0300
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling RenameModuleDir of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_0300, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->RenameModuleDir(TEST_STRING, TEST_STRING);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_0400
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling CreateBundleDataDir of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_0400, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->CreateBundleDataDir(TEST_STRING, 0, 0, 0, TEST_STRING);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_0500
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling RemoveBundleDataDir of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_0500, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->RemoveBundleDataDir(TEST_STRING, 0);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_0600
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling RemoveModuleDataDir of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_0600, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->RemoveModuleDataDir(TEST_STRING, 0);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_0700
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling RemoveDir of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_0700, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->RemoveDir(TEST_STRING);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_0800
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling CleanBundleDataDir of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_0800, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->CleanBundleDataDir(TEST_STRING);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_0900
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling GetBundleStats of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_0900, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    std::vector<int64_t> vec;
    auto ret = hostImpl->GetBundleStats(TEST_STRING, 0, vec);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_1000
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling SetDirApl of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_1000, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->SetDirApl(TEST_STRING, TEST_STRING, TEST_STRING);
#ifdef WITH_SELINUX
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
#else
    EXPECT_EQ(ret, ERR_OK);
#endif
}

/**
 * @tc.number: InstalldHostImplTest_1100
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling GetBundleCachePath of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_1100, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    std::vector<std::string> vec;
    auto ret = hostImpl->GetBundleCachePath(TEST_STRING, vec);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_1200
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling ScanDir of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_1200, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    std::vector<std::string> vec;
    auto ret = hostImpl->ScanDir(TEST_STRING, ScanMode::SUB_FILE_ALL, ResultMode::ABSOLUTE_PATH, vec);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_1300
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling MoveFile of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_1300, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->MoveFile(TEST_STRING, TEST_STRING);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_1400
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling CopyFile of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_1400, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->CopyFile(TEST_STRING, TEST_STRING);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_1500
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling Mkdir of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_1500, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->Mkdir(TEST_STRING, 0, 0, 0);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_1600
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling GetFileStat of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_1600, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    FileStat fileStat;
    auto ret = hostImpl->GetFileStat(TEST_STRING, fileStat);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_1700
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling ExtractDiffFiles of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_1700, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->ExtractDiffFiles(TEST_STRING, TEST_STRING, TEST_STRING);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_1800
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling ApplyDiffPatch of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_1800, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->ApplyDiffPatch(TEST_STRING, TEST_STRING, TEST_STRING);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_1900
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling IsExistDir of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_1900, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    bool isExist = true;
    auto ret = hostImpl->IsExistDir(TEST_STRING, isExist);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_2000
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling IsDirEmpty of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_2000, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    bool isDirEmpty = true;
    auto ret = hostImpl->IsDirEmpty(TEST_STRING, isDirEmpty);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_2100
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling ObtainQuickFixFileDir of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_2100, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    std::vector<std::string> vec;
    auto ret = hostImpl->ObtainQuickFixFileDir(TEST_STRING, vec);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_2200
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling CopyFiles of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_2200, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    std::vector<std::string> vec;
    auto ret = hostImpl->CopyFiles(TEST_STRING, TEST_STRING);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_2300
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling CopyFiles of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_2300, Function | SmallTest | Level0)
{
    sptr<InstalldProxy> installdProxy = new (std::nothrow) InstalldProxy(nullptr);
    EXPECT_NE(installdProxy, nullptr);

    auto ret = installdProxy->CopyFiles(TEST_STRING, TEST_STRING);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR);
}

/**
 * @tc.number: InstalldHostImplTest_2400
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling CopyFiles of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_2400, Function | SmallTest | Level0)
{
    sptr<InstalldProxy> installdProxy = new (std::nothrow) InstalldProxy(nullptr);
    EXPECT_NE(installdProxy, nullptr);

    std::vector<std::string> vec;
    auto ret = installdProxy->ObtainQuickFixFileDir(TEST_STRING, vec);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR);
}

/**
 * @tc.number: InstalldHostImplTest_2500
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling ExtractFiles of hostImpl
 * @tc.require: issueI5VW01
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_2500, Function | SmallTest | Level0)
{
    sptr<InstalldProxy> installdProxy = new (std::nothrow) InstalldProxy(nullptr);
    EXPECT_NE(installdProxy, nullptr);

    ExtractParam extractParam;
    ErrCode ret = installdProxy->ExtractFiles(extractParam);
    EXPECT_NE(ret, ERR_OK);

    extractParam.srcPath = HAP_FILE_PATH;
    ret = installdProxy->ExtractFiles(extractParam);
    EXPECT_NE(ret, ERR_OK);

    extractParam.targetPath = TEST_PATH;
    extractParam.cpuAbi = TEST_CPU_ABI;
    extractParam.extractFileType = ExtractFileType::AN;
    ret = installdProxy->ExtractFiles(extractParam);
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.number: InstalldHostImplTest_2600
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling ExtractDiffFiles of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_2600, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->ExtractDiffFiles(TEST_PATH_PHOTOS, TEST_PATH_TARGET, TEST_CPU_ABI);
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_2700
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling CreateBundleDir of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_2700, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->CreateBundleDir("");
    EXPECT_EQ(ret, ERR_APPEXECFWK_INSTALLD_PERMISSION_DENIED);
}

/**
 * @tc.number: InstalldHostImplTest_2800
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling ExtractModuleFiles of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_2800, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->ExtractModuleFiles("", TEST_STRING, TEST_STRING, TEST_STRING);
    EXPECT_NE(ret, ERR_OK);
    ret = hostImpl->ExtractModuleFiles(TEST_STRING, "", TEST_STRING, TEST_STRING);
    EXPECT_NE(ret, ERR_OK);
    ret = hostImpl->ExtractModuleFiles("", "", TEST_STRING, TEST_STRING);
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.number: InstalldHostImplTest_2900
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling ExtractFiles of hostImpl
 * @tc.require: issueI5VW01
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_2900, Function | SmallTest | Level0)
{
    sptr<InstalldProxy> installdProxy = new (std::nothrow) InstalldProxy(nullptr);
    EXPECT_NE(installdProxy, nullptr);

    ExtractParam extractParam;
    extractParam.srcPath = "";
    extractParam.targetPath = "";
    ErrCode ret = installdProxy->ExtractFiles(extractParam);
    EXPECT_NE(ret, ERR_OK);

    extractParam.targetPath = TEST_PATH;
    ret = installdProxy->ExtractFiles(extractParam);
    EXPECT_NE(ret, ERR_OK);

    extractParam.targetPath = "";
    extractParam.srcPath = HAP_FILE_PATH;
    ret = installdProxy->ExtractFiles(extractParam);
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.number: InstalldHostImplTest_3000
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling RenameModuleDir of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_3000, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->RenameModuleDir("", TEST_STRING);
    EXPECT_NE(ret, ERR_OK);
    ret = hostImpl->RenameModuleDir(TEST_STRING, "");
    EXPECT_NE(ret, ERR_OK);
    ret = hostImpl->RenameModuleDir("", "");
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.number: InstalldHostImplTest_3100
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling RenameModuleDir of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_3100, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->RenameModuleDir(OVER_MAX_PATH_SIZE, TEST_STRING);
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.number: InstalldHostImplTest_3200
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling CreateBundleDataDir of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_3200, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->CreateBundleDataDir(TEST_STRING, -1, -1, -1, TEST_STRING);
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.number: InstalldHostImplTest_0500
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling RemoveBundleDataDir of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_3300, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->RemoveBundleDataDir("", 0);
    EXPECT_NE(ret, ERR_OK);
    ret = hostImpl->RemoveBundleDataDir(TEST_STRING, -1);
    EXPECT_NE(ret, ERR_OK);
    ret = hostImpl->RemoveBundleDataDir("", -1);
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.number: InstalldHostImplTest_3400
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling RemoveDir of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_3400, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->RemoveDir("");
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.number: InstalldHostImplTest_3500
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling CleanBundleDataDir of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_3500, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->CleanBundleDataDir("");
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.number: InstalldHostImplTest_3600
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling GetBundleStats of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_3600, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    std::vector<int64_t> vec;
    auto ret = hostImpl->GetBundleStats("", 0, vec);
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.number: InstalldHostImplTest_3700
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling GetBundleCachePath of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_3700, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    std::vector<std::string> vec;
    auto ret = hostImpl->GetBundleCachePath("", vec);
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.number: InstalldHostImplTest_3800
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling ScanDir of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_3800, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    std::vector<std::string> vec;
    auto ret = hostImpl->ScanDir("", ScanMode::SUB_FILE_ALL, ResultMode::ABSOLUTE_PATH, vec);
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.number: InstalldHostImplTest_3900
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling MoveFile of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_3900, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->MoveFile("", "");
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.number: InstalldHostImplTest_4000
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling Mkdir of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_4000, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->Mkdir("", 0, 0, 0);
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.number: InstalldHostImplTest_4100
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling ExtractDiffFiles of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_4100, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->ExtractDiffFiles("", TEST_STRING, TEST_STRING);
    EXPECT_NE(ret, ERR_OK);
    ret = hostImpl->ExtractDiffFiles(TEST_STRING, "", TEST_STRING);
    EXPECT_NE(ret, ERR_OK);
    ret = hostImpl->ExtractDiffFiles("", "", TEST_STRING);
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.number: InstalldHostImplTest_4200
 * @tc.name: test function of InstallHostImpl
 * @tc.desc: 1. calling ApplyDiffPatch of hostImpl
 * @tc.require: issueI5T6P3
*/
HWTEST_F(BmsInstallDaemonHostImplTest, InstalldHostImplTest_4200, Function | SmallTest | Level0)
{
    auto hostImpl = GetInstalldHostImpl();
    EXPECT_NE(hostImpl, nullptr);

    auto ret = hostImpl->ApplyDiffPatch("", TEST_STRING, TEST_STRING);
    EXPECT_NE(ret, ERR_OK);
    ret = hostImpl->ApplyDiffPatch(TEST_STRING, "", TEST_STRING);
    EXPECT_NE(ret, ERR_OK);
    ret = hostImpl->ApplyDiffPatch("", "", TEST_STRING);
    EXPECT_NE(ret, ERR_OK);
}
} // OHOS