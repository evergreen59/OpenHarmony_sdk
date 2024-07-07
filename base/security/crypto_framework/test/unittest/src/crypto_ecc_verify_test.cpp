/*
 * Copyright (C) 2022-2023 Huawei Device Co., Ltd.
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
#include <cstring>

#include "asy_key_generator.h"
#include "blob.h"
#include "memory.h"
#include "securec.h"
#include "signature.h"

using namespace std;
using namespace testing::ext;

namespace {
class CryptoEccVerifyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static HcfKeyPair *ecc224KeyPair_;
    static HcfKeyPair *ecc256KeyPair_;
    static HcfKeyPair *ecc384KeyPair_;
    static HcfKeyPair *ecc521KeyPair_;
};

HcfKeyPair *CryptoEccVerifyTest::ecc224KeyPair_ = nullptr;
HcfKeyPair *CryptoEccVerifyTest::ecc256KeyPair_ = nullptr;
HcfKeyPair *CryptoEccVerifyTest::ecc384KeyPair_ = nullptr;
HcfKeyPair *CryptoEccVerifyTest::ecc521KeyPair_ = nullptr;

static const char *g_mockMessage = "hello world";
static HcfBlob mockInput = {
    .data = (uint8_t *)g_mockMessage,
    .len = 12
};

void CryptoEccVerifyTest::SetUp() {}
void CryptoEccVerifyTest::TearDown() {}

void CryptoEccVerifyTest::SetUpTestCase()
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);
    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    ecc224KeyPair_ = keyPair;

    HcfObjDestroy(generator);

    HcfAsyKeyGenerator *generator2 = NULL;
    res = HcfAsyKeyGeneratorCreate("ECC256", &generator2);
    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator2, nullptr);

    HcfKeyPair *keyPair2 = NULL;
    res = generator2->generateKeyPair(generator2, NULL, &keyPair2);
    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair2, nullptr);

    ecc256KeyPair_ = keyPair2;

    HcfObjDestroy(generator2);

    HcfAsyKeyGenerator *generator3 = NULL;
    res = HcfAsyKeyGeneratorCreate("ECC384", &generator3);
    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator3, nullptr);

    HcfKeyPair *keyPair3 = NULL;
    res = generator3->generateKeyPair(generator3, NULL, &keyPair3);
    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair3, nullptr);

    ecc384KeyPair_ = keyPair3;

    HcfObjDestroy(generator3);

    HcfAsyKeyGenerator *generator4 = NULL;
    res = HcfAsyKeyGeneratorCreate("ECC521", &generator4);
    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator4, nullptr);

    HcfKeyPair *keyPair4 = NULL;
    res = generator4->generateKeyPair(generator4, NULL, &keyPair4);
    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair4, nullptr);

    ecc521KeyPair_ = keyPair4;

    HcfObjDestroy(generator4);
}

void CryptoEccVerifyTest::TearDownTestCase()
{
    HcfObjDestroy(ecc224KeyPair_);
    HcfObjDestroy(ecc256KeyPair_);
    HcfObjDestroy(ecc384KeyPair_);
    HcfObjDestroy(ecc521KeyPair_);
}

static const char *GetMockClass(void)
{
    return "HcfMock";
}

static HcfObjectBase obj = {
    .getClass = GetMockClass,
    .destroy = NULL
};

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest001, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC224|SHA1", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest002, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC224|SHA224", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest003, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC224|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest004, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC224|SHA384", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest005, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC224|SHA512", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest006, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA1", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest007, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA224", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest008, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest009, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA384", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest010, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA512", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest011, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC384|SHA1", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest012, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC384|SHA224", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest013, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC384|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest014, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC384|SHA384", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest015, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC384|SHA512", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest016, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC521|SHA1", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest017, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC521|SHA224", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest018, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC521|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest019, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC521|SHA384", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest020, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC521|SHA512", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest021, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate(NULL, &verify);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);
    ASSERT_EQ(verify, nullptr);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest022, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCD"
        "ABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCD", &verify);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);
    ASSERT_EQ(verify, nullptr);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest023, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC257|SHA256", &verify);

    ASSERT_NE(res, HCF_SUCCESS);
    ASSERT_EQ(verify, nullptr);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest024, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA257", &verify);

    ASSERT_NE(res, HCF_SUCCESS);
    ASSERT_EQ(verify, nullptr);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest025, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|MD5", &verify);

    ASSERT_NE(res, HCF_SUCCESS);
    ASSERT_EQ(verify, nullptr);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest026, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256SHA256", &verify);

    ASSERT_NE(res, HCF_SUCCESS);
    ASSERT_EQ(verify, nullptr);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest027, TestSize.Level0)
{
    int32_t res = HcfVerifyCreate("ECC256|SHA256", NULL);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest101, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    const char *className = verify->base.getClass();

    ASSERT_NE(className, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest102, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    verify->base.destroy((HcfObjectBase *)verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest103, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    verify->base.destroy(NULL);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest104, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    verify->base.destroy(&obj);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest105, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    const char *algName = verify->getAlgoName(verify);

    ASSERT_NE(algName, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest106, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    const char *algName = verify->getAlgoName(NULL);

    ASSERT_EQ(algName, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest107, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    const char *algName = verify->getAlgoName((HcfVerify *)(&obj));

    ASSERT_EQ(algName, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest201, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC224|SHA1", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc224KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest202, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC224|SHA224", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc224KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest203, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC224|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc224KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest204, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC224|SHA384", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc224KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest205, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC224|SHA512", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc224KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest206, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA1", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest207, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA224", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest208, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest209, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA384", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest210, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA512", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest211, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC384|SHA1", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc384KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest212, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC384|SHA224", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc384KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest213, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC384|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc384KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest214, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC384|SHA384", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc384KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest215, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC384|SHA512", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc384KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest216, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC521|SHA1", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc521KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest217, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC521|SHA224", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc521KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest218, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC521|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc521KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest219, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC521|SHA384", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc521KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest220, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC521|SHA512", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc521KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest221, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(NULL, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest222, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init((HcfVerify *)(&obj), NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest223, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, NULL);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest224, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, (HcfPubKey *)(&obj));

    ASSERT_EQ(res, HCF_INVALID_PARAMS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest301, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC224|SHA1", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc224KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest302, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC224|SHA224", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc224KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest303, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC224|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc224KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest304, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC224|SHA384", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc224KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest305, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC224|SHA512", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc224KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest306, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA1", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest307, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA224", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest308, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest309, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA384", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest310, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA512", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest311, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC384|SHA1", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc384KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest312, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC384|SHA224", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc384KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest313, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC384|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc384KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest314, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC384|SHA384", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc384KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest315, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC384|SHA512", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc384KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest316, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC521|SHA1", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc521KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest317, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC521|SHA224", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc521KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest318, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC521|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc521KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest319, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC521|SHA384", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc521KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest320, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC521|SHA512", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc521KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest321, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(NULL, &mockInput);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest322, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update((HcfVerify *)(&obj), &mockInput);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest323, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, NULL);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest324, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob input = {
        .data = NULL,
        .len = 1
    };
    res = verify->update(verify, &input);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest325, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob input = {
        .data = (uint8_t *)g_mockMessage,
        .len = 0
    };
    res = verify->update(verify, &input);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest401, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC224|SHA1", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc224KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC224|SHA1", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc224KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest402, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC224|SHA224", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc224KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC224|SHA224", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc224KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest403, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC224|SHA256", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc224KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC224|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc224KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest404, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC224|SHA384", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc224KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC224|SHA384", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc224KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest405, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC224|SHA512", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc224KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC224|SHA512", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc224KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest406, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC256|SHA1", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc256KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC256|SHA1", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest407, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC256|SHA224", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc256KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC256|SHA224", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest408, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC256|SHA256", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc256KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest409, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC256|SHA384", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc256KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC256|SHA384", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest410, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC256|SHA512", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc256KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC256|SHA512", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest411, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC384|SHA1", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc384KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC384|SHA1", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc384KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest412, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC384|SHA224", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc384KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC384|SHA224", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc384KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest413, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC384|SHA256", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc384KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC384|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc384KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest414, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC384|SHA384", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc384KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC384|SHA384", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc384KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest415, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC384|SHA512", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc384KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC384|SHA512", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc384KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest416, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC521|SHA1", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc521KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC521|SHA1", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc521KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest417, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC521|SHA224", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc521KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC521|SHA224", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc521KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest418, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC521|SHA256", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc521KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC521|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc521KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest419, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC521|SHA384", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc521KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC521|SHA384", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc521KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest420, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC521|SHA512", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc521KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC521|SHA512", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc521KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest421, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC224|SHA1", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc224KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, &mockInput, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC224|SHA1", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc224KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, &mockInput, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest422, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC224|SHA224", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc224KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, &mockInput, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC224|SHA224", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc224KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, &mockInput, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest423, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC224|SHA256", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc224KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, &mockInput, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC224|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc224KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, &mockInput, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest424, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC224|SHA384", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc224KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, &mockInput, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC224|SHA384", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc224KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, &mockInput, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest425, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC224|SHA512", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc224KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, &mockInput, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC224|SHA512", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc224KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, &mockInput, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest426, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC256|SHA1", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc256KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, &mockInput, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC256|SHA1", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, &mockInput, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest427, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC256|SHA224", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc256KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, &mockInput, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC256|SHA224", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, &mockInput, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest428, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC256|SHA256", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc256KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, &mockInput, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, &mockInput, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest429, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC256|SHA384", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc256KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, &mockInput, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC256|SHA384", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, &mockInput, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest430, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC256|SHA512", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc256KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, &mockInput, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC256|SHA512", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, &mockInput, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest431, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC384|SHA1", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc384KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, &mockInput, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC384|SHA1", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc384KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, &mockInput, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest432, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC384|SHA224", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc384KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, &mockInput, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC384|SHA224", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc384KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, &mockInput, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest433, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC384|SHA256", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc384KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, &mockInput, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC384|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc384KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, &mockInput, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest434, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC384|SHA384", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc384KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, &mockInput, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC384|SHA384", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc384KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, &mockInput, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest435, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC384|SHA512", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc384KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, &mockInput, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC384|SHA512", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc384KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, &mockInput, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest436, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC521|SHA1", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc521KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, &mockInput, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC521|SHA1", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc521KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, &mockInput, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest437, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC521|SHA224", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc521KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, &mockInput, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC521|SHA224", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc521KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, &mockInput, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest438, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC521|SHA256", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc521KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, &mockInput, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC521|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc521KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, &mockInput, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest439, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC521|SHA384", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc521KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, &mockInput, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC521|SHA384", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc521KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, &mockInput, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest440, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC521|SHA512", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc521KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, &mockInput, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC521|SHA512", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc521KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, &mockInput, &out);

    ASSERT_EQ(flag, true);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest441, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC256|SHA256", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc256KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(NULL, NULL, &out);

    ASSERT_EQ(flag, false);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest442, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC256|SHA256", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc256KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify((HcfVerify *)(&obj), NULL, &out);

    ASSERT_EQ(flag, false);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest443, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC256|SHA256", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc256KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, &out);

    ASSERT_EQ(flag, false);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest444, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC256|SHA256", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc256KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob input = {
        .data = NULL,
        .len = 1
    };
    bool flag = verify->verify(verify, &input, &out);

    ASSERT_EQ(flag, false);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest445, TestSize.Level0)
{
    HcfSign *sign = NULL;
    int32_t res = HcfSignCreate("ECC256|SHA256", &sign);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(sign, nullptr);

    res = sign->init(sign, NULL, ecc256KeyPair_->priKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = sign->update(sign, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = sign->sign(sign, NULL, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob input = {
        .data = (uint8_t *)g_mockMessage,
        .len = 0
    };
    bool flag = verify->verify(verify, &input, &out);

    ASSERT_EQ(flag, false);

    free(out.data);
    HcfObjDestroy(sign);
    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest446, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    bool flag = verify->verify(verify, NULL, NULL);

    ASSERT_EQ(flag, false);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest447, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob mockOut = {
        .data = NULL,
        .len = 1
    };
    bool flag = verify->verify(verify, NULL, &mockOut);

    ASSERT_EQ(flag, false);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoEccVerifyTest, CryptoEccVerifyTest448, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("ECC256|SHA256", &verify);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    res = verify->init(verify, NULL, ecc256KeyPair_->pubKey);

    ASSERT_EQ(res, HCF_SUCCESS);

    res = verify->update(verify, &mockInput);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob mockOut = {
        .data = (uint8_t *)g_mockMessage,
        .len = 0
    };
    bool flag = verify->verify(verify, NULL, &mockOut);

    ASSERT_EQ(flag, false);

    HcfObjDestroy(verify);
}
}
