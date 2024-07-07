/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include "securec.h"

#include "rand.h"
#include "rand_openssl.h"

#include "log.h"
#include "memory.h"

using namespace std;
using namespace testing::ext;

namespace {
class CryptoRandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void CryptoRandTest::SetUpTestCase() {}
void CryptoRandTest::TearDownTestCase() {}

void CryptoRandTest::SetUp() // add init here, this will be called before test.
{
}

void CryptoRandTest::TearDown() // add destroy here, this will be called when test case done.
{
}

/**
 * @tc.name: CryptoFrameworkRandTest.CryptoFrameworkRandCreateTest001
 * @tc.desc: Verify that the creation of the random obj is normal.
 * @tc.type: FUNC
 * @tc.require: I5QWEN
 */
HWTEST_F(CryptoRandTest, CryptoFrameworkRandCreateTest001, TestSize.Level0)
{
    HcfResult ret = HcfRandCreate(nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

HWTEST_F(CryptoRandTest, CryptoFrameworkRandGenerateTest001, TestSize.Level0)
{
    // create a rand obj
    HcfRand *randObj = nullptr;
    HcfResult ret = HcfRandCreate(&randObj);
    ASSERT_EQ(ret, HCF_SUCCESS);
    // preset params
    int32_t randomLen = 0;
    // define randomBlob and seedBlob
    struct HcfBlob randomBlob = {0};
    // test generate random with length 0
    ret = randObj->generateRandom(randObj, randomLen, &randomBlob);
    EXPECT_NE(ret, HCF_SUCCESS);
    HcfObjDestroy(randObj);
}

HWTEST_F(CryptoRandTest, CryptoFrameworkRandGenerateTest002, TestSize.Level0)
{
    // create a rand obj
    HcfRand *randObj = nullptr;
    HcfResult ret = HcfRandCreate(&randObj);
    ASSERT_EQ(ret, HCF_SUCCESS);
    // preset params
    int32_t randomLen = 32;
    // define randomBlob and seedBlob
    struct HcfBlob randomBlob = {0};
    // test generate random
    ret = randObj->generateRandom(randObj, randomLen, &randomBlob);
    EXPECT_EQ(ret, HCF_SUCCESS);
    // destroy the API obj and blob data
    HcfBlobDataClearAndFree(&randomBlob);
    HcfObjDestroy(randObj);
}

HWTEST_F(CryptoRandTest, CryptoFrameworkRandGenerateTest003, TestSize.Level0)
{
    // create a rand obj
    HcfRand *randObj = nullptr;
    HcfResult ret = HcfRandCreate(&randObj);
    ASSERT_EQ(ret, HCF_SUCCESS);
    // preset params
    int32_t randomLen = 8192;
    // define randomBlob and seedBlob
    struct HcfBlob randomBlob = {0};
    // test generate random
    ret = randObj->generateRandom(randObj, randomLen, &randomBlob);
    EXPECT_EQ(ret, HCF_SUCCESS);
    // destroy the API obj and blob data
    HcfBlobDataClearAndFree(&randomBlob);
    HcfObjDestroy(randObj);
}

HWTEST_F(CryptoRandTest, CryptoFrameworkRandGenerateTest004, TestSize.Level0)
{
    // create a rand obj
    HcfRand *randObj = nullptr;
    HcfResult ret = HcfRandCreate(&randObj);
    ASSERT_EQ(ret, HCF_SUCCESS);
    // preset params
    int32_t randomLen = 8193;
    // define randomBlob and seedBlob
    struct HcfBlob randomBlob = {0};
    // test generate random
    ret = randObj->generateRandom(randObj, randomLen, &randomBlob);
    EXPECT_NE(ret, HCF_SUCCESS);
    // destroy the API obj and blob data
    HcfObjDestroy(randObj);
}

HWTEST_F(CryptoRandTest, CryptoFrameworkSetSeedTest001, TestSize.Level0)
{
    // create a rand obj
    HcfRand *randObj = nullptr;
    HcfResult ret = HcfRandCreate(&randObj);
    ASSERT_EQ(ret, HCF_SUCCESS);
    // define randomBlob and seedBlob
    struct HcfBlob *seedBlob = nullptr;
    // test set seed
    ret = randObj->setSeed(randObj, seedBlob);
    EXPECT_NE(ret, HCF_SUCCESS);
    // destroy the API obj and blob data
    HcfObjDestroy(randObj);
}

HWTEST_F(CryptoRandTest, CryptoFrameworkSetSeedTest002, TestSize.Level0)
{
    // create a rand obj
    HcfRand *randObj = nullptr;
    HcfResult ret = HcfRandCreate(&randObj);
    ASSERT_EQ(ret, HCF_SUCCESS);
    // preset params
    int32_t seedLen = 32;
    // define randomBlob and seedBlob
    struct HcfBlob seedBlob = { .data = nullptr, .len = 0 };
    // test generate seed
    ret = randObj->generateRandom(randObj, seedLen, &seedBlob);
    EXPECT_EQ(ret, HCF_SUCCESS);
    // test set seed
    ret = randObj->setSeed(randObj, &seedBlob);
    EXPECT_EQ(ret, HCF_SUCCESS);
    // destroy the API obj and blob data
    HcfBlobDataClearAndFree(&seedBlob);
    HcfObjDestroy(randObj);
}

HWTEST_F(CryptoRandTest, CryptoFrameworkSetSeedTest003, TestSize.Level0)
{
    // create a rand obj
    HcfRand *randObj = nullptr;
    HcfResult ret = HcfRandCreate(&randObj);
    ASSERT_EQ(ret, HCF_SUCCESS);
    // preset params
    int32_t seedLen = 1000;
    // define randomBlob and seedBlob
    struct HcfBlob seedBlob = { .data = nullptr, .len = 0 };
    // test generate seed
    ret = randObj->generateRandom(randObj, seedLen, &seedBlob);
    EXPECT_EQ(ret, HCF_SUCCESS);
    // test set seed
    ret = randObj->setSeed(randObj, &seedBlob);
    EXPECT_EQ(ret, HCF_SUCCESS);
    // destroy the API obj and blob data
    HcfBlobDataClearAndFree(&seedBlob);
    HcfObjDestroy(randObj);
}

static const char *GetInvalidRandClass(void)
{
    return "INVALID_RAND_CLASS";
}

HWTEST_F(CryptoRandTest, NullInputRandTest001, TestSize.Level0)
{
    HcfResult ret = HcfRandSpiCreate(nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

HWTEST_F(CryptoRandTest, InvalidSpiClassRandTest001, TestSize.Level0)
{
    HcfRandSpi *spiObj = nullptr;
    HcfRandSpi invalidSpi = {{0}};
    invalidSpi.base.getClass = GetInvalidRandClass;
    HcfResult ret = HcfRandSpiCreate(&spiObj);
    ASSERT_EQ(ret, HCF_SUCCESS);
    ASSERT_NE(spiObj, nullptr);
    (void)spiObj->base.destroy(nullptr);
    (void)spiObj->base.destroy(&(invalidSpi.base));
    HcfObjDestroy(spiObj);
}
}