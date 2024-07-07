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
#include "asy_key_generator.h"
#include "blob.h"
#include "memory.h"
#include "signature.h"
#include "cstring"


using namespace std;
using namespace testing::ext;

namespace {
class CryptoRsaVerifyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void CryptoRsaVerifyTest::SetUp() {}
void CryptoRsaVerifyTest::TearDown() {}
void CryptoRsaVerifyTest::SetUpTestCase() {}
void CryptoRsaVerifyTest::TearDownTestCase() {}

HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest100, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("RSA1024|PSS|SHA256", &verify);
    EXPECT_NE(res, HCF_SUCCESS);
    ASSERT_EQ(verify, nullptr);

    HcfObjDestroy(verify);
}

HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest110, TestSize.Level0)
{
    HcfVerify *verify = NULL;
    int32_t res = HcfVerifyCreate("RSA1024|PSS|SHA256|MGF1_SHA256", &verify);
    EXPECT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(verify, nullptr);

    HcfObjDestroy(verify);
}

// incorrect case : init signer with NULL public key.
HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest120, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("RSA1024|PSS|SHA256|MGF1_SHA256", &verify);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->init(verify, NULL, NULL);
    EXPECT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

// incorrect case : init signer with private Key.
HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest130, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_2", &generator);
    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("RSA1024|PSS|SHA256|MGF1_SHA256", &verify);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->init(verify, NULL, (HcfPubKey *)keyPair->priKey);
    EXPECT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
    HcfObjDestroy(verify);
}

// incorrect case : init with other class (not cipher).
HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest140, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_2", &generator);
    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("RSA1024|PSS|SHA256|MGF1_SHA256", &verify);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->init((HcfVerify *)generator, NULL, keyPair->pubKey);
    EXPECT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
    HcfObjDestroy(verify);
}

// incorrect case : update with other class (not cipher).
HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest150, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_2", &generator);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("RSA1024|PSS|SHA256|MGF1_SHA256", &verify);
    EXPECT_EQ(res, HCF_SUCCESS);

    uint8_t plan[] = "this is rsa verify test.";
    HcfBlob input = {.data = plan, .len = strlen((char *)plan)};
    res = verify->update((HcfVerify *)generator, &input);
    EXPECT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
    HcfObjDestroy(generator);
}

// incorrect case : verify with other class (not cipher).
HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest160, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_2", &generator);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("RSA1024|PSS|SHA256|MGF1_SHA256", &verify);
    EXPECT_EQ(res, HCF_SUCCESS);

    uint8_t plan[] = "this is rsa verify test.";
    HcfBlob input = {.data = plan, .len = strlen((char *)plan)};
    bool result = verify->verify((HcfVerify *)generator, &input, &input);
    EXPECT_NE(result, true);

    HcfObjDestroy(verify);
    HcfObjDestroy(generator);
}

// incorrect case : use update function before intialize.
HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest170, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("RSA1024|PSS|SHA256|MGF1_SHA256", &verify);
    EXPECT_EQ(res, HCF_SUCCESS);

    uint8_t plan[] = "this is rsa verify test.";
    HcfBlob input = {.data = plan, .len = strlen((char *)plan)};
    res = verify->update(verify, &input);
    EXPECT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
}

// incorrect case : use verify function before intialize.
HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest180, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("RSA1024|PSS|SHA256|MGF1_SHA256", &verify);
    EXPECT_EQ(res, HCF_SUCCESS);

    uint8_t plan[] = "this is rsa verify test.";
    HcfBlob input = {.data = plan, .len = strlen((char *)plan)};
    bool result = verify->verify(verify, NULL, &input);
    EXPECT_NE(result, true);

    HcfObjDestroy(verify);
}

// incorrect case : update with NULL inputBlob.
HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest190, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfPubKey *pubkey = keyPair->pubKey;

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("RSA1024|PSS|SHA256|MGF1_SHA512", &verify);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->init(verify, NULL, pubkey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->update(verify, NULL);
    EXPECT_NE(res, 1);

    HcfObjDestroy(verify);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

// incorrect case : verify with NULL outputBlob.
HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest191, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfPubKey *pubkey = keyPair->pubKey;

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("RSA1024|PSS|SHA256|MGF1_SHA512", &verify);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->init(verify, NULL, pubkey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->verify(verify, NULL, NULL);
    EXPECT_NE(res, 1);

    HcfObjDestroy(verify);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

// incorrect case : init verify twice
HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest192, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_2", &generator);
    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfPubKey *pubkey = keyPair->pubKey;
    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("RSA1024|PSS|SHA256|MGF1_SHA256", &verify);
    EXPECT_EQ(res, HCF_SUCCESS);

    res = verify->init(verify, NULL, pubkey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->init(verify, NULL, pubkey);
    EXPECT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(verify);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest200, TestSize.Level0)
{
    uint8_t plan[] = "this is rsa verify test.";
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_2", &generator);
    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfPubKey *pubkey = keyPair->pubKey;
    HcfPriKey *prikey = keyPair->priKey;
    HcfBlob input = {.data = plan, .len = strlen((char *)plan)};
    HcfBlob verifyData = {.data = NULL, .len = 0};
    HcfSign *sign = NULL;
    res = HcfSignCreate("RSA1024|PSS|SHA256|MGF1_SHA256", &sign);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->init(sign, NULL, prikey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->sign(sign, &input, &verifyData);
    HcfObjDestroy(sign);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("RSA1024|PSS|SHA256|MGF1_SHA256", &verify);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->init(verify, NULL, pubkey);
    EXPECT_EQ(res, HCF_SUCCESS);
    bool result = verify->verify(verify, &input, &verifyData);
    EXPECT_EQ(result, true);
    HcfObjDestroy(verify);

    HcfFree(verifyData.data);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest210, TestSize.Level0)
{
    uint8_t plan[] = "this is rsa verify test aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.";
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfPubKey *pubkey = keyPair->pubKey;
    HcfPriKey *prikey = keyPair->priKey;

    HcfBlob input = {.data = plan, .len = strlen((char *)plan)};
    HcfBlob verifyData = {.data = NULL, .len = 0};
    HcfSign *sign = NULL;
    res = HcfSignCreate("RSA1024|PSS|SHA256|MGF1_SHA256", &sign);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->init(sign, NULL, prikey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->sign(sign, &input, &verifyData);
    HcfObjDestroy(sign);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("RSA1024|PSS|SHA256|MGF1_SHA256", &verify);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->init(verify, NULL, pubkey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->verify(verify, &input, &verifyData);
    EXPECT_EQ(res, 1);
    HcfObjDestroy(verify);

    HcfFree(verifyData.data);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest220, TestSize.Level0)
{
    uint8_t plan[] = "this is rsa verify test aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.";
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfPubKey *pubkey = keyPair->pubKey;
    HcfPriKey *prikey = keyPair->priKey;

    HcfBlob input = {.data = plan, .len = strlen((char *)plan)};
    HcfBlob verifyData = {.data = NULL, .len = 0};
    HcfSign *sign = NULL;
    res = HcfSignCreate("RSA1024|PSS|SHA256|MGF1_SHA512", &sign);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->init(sign, NULL, prikey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->sign(sign, &input, &verifyData);
    HcfObjDestroy(sign);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("RSA1024|PSS|SHA256|MGF1_SHA512", &verify);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->init(verify, NULL, pubkey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->verify(verify, &input, &verifyData);
    EXPECT_EQ(res, 1);
    HcfObjDestroy(verify);

    HcfFree(verifyData.data);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest230, TestSize.Level0)
{
    uint8_t plan[] = "this is rsa verify test aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.";
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfPubKey *pubkey = keyPair->pubKey;
    HcfPriKey *prikey = keyPair->priKey;

    HcfBlob input = {.data = plan, .len = strlen((char *)plan)};
    HcfBlob verifyData = {.data = NULL, .len = 0};
    HcfSign *sign = NULL;
    res = HcfSignCreate("RSA1024|PKCS1|SHA256", &sign);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->init(sign, NULL, prikey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->sign(sign, &input, &verifyData);
    HcfObjDestroy(sign);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("RSA1024|PKCS1|SHA256", &verify);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->init(verify, NULL, pubkey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->verify(verify, &input, &verifyData);
    EXPECT_EQ(res, 1);
    HcfObjDestroy(verify);

    HcfFree(verifyData.data);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest240, TestSize.Level0)
{
    uint8_t plan[] = "this is rsa verify test aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.";
    uint8_t errorverify[] = "asdfasdfasdfasf";
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfPubKey *pubkey = keyPair->pubKey;
    HcfPriKey *prikey = keyPair->priKey;

    HcfBlob input = {.data = plan, .len = strlen((char *)plan)};
    HcfBlob verifyData = {.data = NULL, .len = 0};
    HcfBlob invalidverifyData = {.data = errorverify, .len = strlen((char *)errorverify)};
    HcfSign *sign = NULL;
    res = HcfSignCreate("RSA1024|PKCS1|SHA256", &sign);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->init(sign, NULL, prikey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->sign(sign, &input, &verifyData);
    HcfObjDestroy(sign);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("RSA1024|PKCS1|SHA256", &verify);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->init(verify, NULL, pubkey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->verify(verify, &input, &invalidverifyData);
    EXPECT_EQ(res, 0);
    HcfObjDestroy(verify);

    HcfFree(verifyData.data);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

// Incorrect case: different mode
HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest250, TestSize.Level0)
{
    uint8_t plan[] = "this is rsa verify test aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.";
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfPubKey *pubkey = keyPair->pubKey;
    HcfPriKey *prikey = keyPair->priKey;

    HcfBlob input = {.data = plan, .len = strlen((char *)plan)};
    HcfBlob verifyData = {.data = NULL, .len = 0};
    HcfSign *sign = NULL;
    res = HcfSignCreate("RSA1024|PKCS1|SHA256", &sign);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->init(sign, NULL, prikey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->sign(sign, &input, &verifyData);
    HcfObjDestroy(sign);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("RSA1024|PSS|SHA256|MGF1_SHA512", &verify);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->init(verify, NULL, pubkey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->verify(verify, &input, &verifyData);
    EXPECT_EQ(res, 0);
    HcfObjDestroy(verify);

    HcfFree(verifyData.data);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

// Incorrect case: different mgf1md
HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest260, TestSize.Level0)
{
    uint8_t plan[] = "this is rsa verify test aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.";
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfPubKey *pubkey = keyPair->pubKey;
    HcfPriKey *prikey = keyPair->priKey;

    HcfBlob input = {.data = plan, .len = strlen((char *)plan)};
    HcfBlob verifyData = {.data = NULL, .len = 0};
    HcfSign *sign = NULL;
    res = HcfSignCreate("RSA1024|PSS|SHA256|MGF1_SHA256", &sign);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->init(sign, NULL, prikey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->sign(sign, &input, &verifyData);
    HcfObjDestroy(sign);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("RSA1024|PSS|SHA256|MGF1_SHA512", &verify);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->init(verify, NULL, pubkey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->verify(verify, &input, &verifyData);
    EXPECT_EQ(res, 0);
    HcfObjDestroy(verify);

    HcfFree(verifyData.data);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}


// Incorrect case: different pkcs1 md, verifu fail
HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest270, TestSize.Level0)
{
    uint8_t plan[] = "this is rsa verify test aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.";
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfPubKey *pubkey = keyPair->pubKey;
    HcfPriKey *prikey = keyPair->priKey;

    HcfBlob input = {.data = plan, .len = strlen((char *)plan)};
    HcfBlob verifyData = {.data = NULL, .len = 0};
    HcfSign *sign = NULL;
    res = HcfSignCreate("RSA1024|PKCS1|SHA256", &sign);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->init(sign, NULL, prikey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->sign(sign, &input, &verifyData);
    HcfObjDestroy(sign);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("RSA1024|PKCS1|SHA512", &verify);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->init(verify, NULL, pubkey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->verify(verify, &input, &verifyData);
    EXPECT_EQ(res, 0);
    HcfObjDestroy(verify);

    HcfFree(verifyData.data);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

// check update_func in PSS padding
HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest280, TestSize.Level0)
{
    uint8_t plan1[] = "this is rsa verify test plane1 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.";
    uint8_t plan2[] = "this is rsa verify test plane2 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.";
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfPubKey *pubkey = keyPair->pubKey;
    HcfPriKey *prikey = keyPair->priKey;

    HcfBlob input = {.data = plan1, .len = strlen((char *)plan1)};
    HcfBlob inputEx = {.data = plan2, .len = strlen((char *)plan2)};
    HcfBlob verifyData = {.data = NULL, .len = 0};
    HcfSign *sign = NULL;
    res = HcfSignCreate("RSA1024|PSS|SHA256|MGF1_SHA256", &sign);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->init(sign, NULL, prikey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->update(sign, &input);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->sign(sign, &inputEx, &verifyData);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfObjDestroy(sign);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("RSA1024|PSS|SHA256|MGF1_SHA256", &verify);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->init(verify, NULL, pubkey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->update(verify, &input);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->verify(verify, &inputEx, &verifyData);
    EXPECT_EQ(res, 1);
    HcfObjDestroy(verify);

    HcfFree(verifyData.data);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

// check update in PKCS1 padding
HWTEST_F(CryptoRsaVerifyTest, CryptoRsaVerifyTest290, TestSize.Level0)
{
    uint8_t plan1[] = "this is rsa verify test plane1 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.";
    uint8_t plan2[] = "this is rsa verify test plane2 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.";
    uint8_t plan3[] = "this is rsa verify test plane3 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.";

    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfPubKey *pubkey = keyPair->pubKey;
    HcfPriKey *prikey = keyPair->priKey;

    HcfBlob input1 = {.data = plan1, .len = strlen((char *)plan1)};
    HcfBlob input2 = {.data = plan2, .len = strlen((char *)plan2)};
    HcfBlob input3 = {.data = plan3, .len = strlen((char *)plan3)};
    HcfBlob verifyData = {.data = NULL, .len = 0};
    HcfSign *sign = NULL;
    res = HcfSignCreate("RSA1024|PKCS1|SHA256", &sign);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->init(sign, NULL, prikey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->update(sign, &input1);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->update(sign, &input2);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = sign->sign(sign, &input3, &verifyData);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfObjDestroy(sign);

    HcfVerify *verify = NULL;
    res = HcfVerifyCreate("RSA1024|PKCS1|SHA256", &verify);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->init(verify, NULL, pubkey);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->update(verify, &input1);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->update(verify, &input2);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = verify->verify(verify, &input3, &verifyData);
    EXPECT_EQ(res, 1);
    HcfObjDestroy(verify);

    HcfFree(verifyData.data);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}
}
