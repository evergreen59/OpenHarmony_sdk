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
#include "securec.h"

#include "asy_key_generator.h"
#include "blob.h"
#include "memory.h"

using namespace std;
using namespace testing::ext;

namespace {
class CryptoRsaAsyKeyGeneratorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void CryptoRsaAsyKeyGeneratorTest::SetUpTestCase() {}
void CryptoRsaAsyKeyGeneratorTest::TearDownTestCase() {}
void CryptoRsaAsyKeyGeneratorTest::SetUp() {}
void CryptoRsaAsyKeyGeneratorTest::TearDown() {}

// HcfAsyKeyGeneratorCreate correct case: no primes
HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest100, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA512", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);
    EXPECT_NE(generator->base.getClass(), nullptr);
    EXPECT_NE(generator->base.destroy, nullptr);
    EXPECT_NE(generator->generateKeyPair, nullptr);
    EXPECT_NE(generator->getAlgoName, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest110, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA768", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);
    EXPECT_NE(generator->base.getClass(), nullptr);
    EXPECT_NE(generator->base.destroy, nullptr);
    EXPECT_NE(generator->generateKeyPair, nullptr);
    EXPECT_NE(generator->getAlgoName, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest120, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);
    EXPECT_NE(generator->base.getClass(), nullptr);
    EXPECT_NE(generator->base.destroy, nullptr);
    EXPECT_NE(generator->generateKeyPair, nullptr);
    EXPECT_NE(generator->getAlgoName, nullptr);
    HcfObjDestroy(generator);
}


HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest130, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA2048", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);
    EXPECT_NE(generator->base.getClass(), nullptr);
    EXPECT_NE(generator->base.destroy, nullptr);
    EXPECT_NE(generator->generateKeyPair, nullptr);
    EXPECT_NE(generator->getAlgoName, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest140, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA3072", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);
    EXPECT_NE(generator->base.getClass(), nullptr);
    EXPECT_NE(generator->base.destroy, nullptr);
    EXPECT_NE(generator->generateKeyPair, nullptr);
    EXPECT_NE(generator->getAlgoName, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest150, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA4096", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);
    EXPECT_NE(generator->base.getClass(), nullptr);
    EXPECT_NE(generator->base.destroy, nullptr);
    EXPECT_NE(generator->generateKeyPair, nullptr);
    EXPECT_NE(generator->getAlgoName, nullptr);
    HcfObjDestroy(generator);
}

// HcfAsyKeyGeneratorCreate correct case: with primes
HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest200, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA512|PRIMES_2", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);
    EXPECT_NE(generator->base.getClass(), nullptr);
    EXPECT_NE(generator->base.destroy, nullptr);
    EXPECT_NE(generator->generateKeyPair, nullptr);
    EXPECT_NE(generator->getAlgoName, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest210, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA768|PRIMES_2", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);
    EXPECT_NE(generator->base.getClass(), nullptr);
    EXPECT_NE(generator->base.destroy, nullptr);
    EXPECT_NE(generator->generateKeyPair, nullptr);
    EXPECT_NE(generator->getAlgoName, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest220, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);
    EXPECT_NE(generator->base.getClass(), nullptr);
    EXPECT_NE(generator->base.destroy, nullptr);
    EXPECT_NE(generator->generateKeyPair, nullptr);
    EXPECT_NE(generator->getAlgoName, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest230, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_3", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);
    EXPECT_NE(generator->base.getClass(), nullptr);
    EXPECT_NE(generator->base.destroy, nullptr);
    EXPECT_NE(generator->generateKeyPair, nullptr);
    EXPECT_NE(generator->getAlgoName, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest240, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_2", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);
    EXPECT_NE(generator->base.getClass(), nullptr);
    EXPECT_NE(generator->base.destroy, nullptr);
    EXPECT_NE(generator->generateKeyPair, nullptr);
    EXPECT_NE(generator->getAlgoName, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest250, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_3", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);
    EXPECT_NE(generator->base.getClass(), nullptr);
    EXPECT_NE(generator->base.destroy, nullptr);
    EXPECT_NE(generator->generateKeyPair, nullptr);
    EXPECT_NE(generator->getAlgoName, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest260, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA3072|PRIMES_3", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);
    EXPECT_NE(generator->base.getClass(), nullptr);
    EXPECT_NE(generator->base.destroy, nullptr);
    EXPECT_NE(generator->generateKeyPair, nullptr);
    EXPECT_NE(generator->getAlgoName, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest270, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA4096|PRIMES_4", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);
    EXPECT_NE(generator->base.getClass(), nullptr);
    EXPECT_NE(generator->base.destroy, nullptr);
    EXPECT_NE(generator->generateKeyPair, nullptr);
    EXPECT_NE(generator->getAlgoName, nullptr);
    HcfObjDestroy(generator);
}

// HcfAsyKeyGeneratorCreate Incorrect case : algname is null
HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest300, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate(NULL, &generator);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(generator, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest301, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("", &generator);
    EXPECT_NE(res, HCF_SUCCESS);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest310, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("111111111111111111111111111111111111111111111111111111111111111111111111"
        "111111111111111111111111111111111111111111111111111111111111111111111", &generator);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(generator, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest320, TestSize.Level0)
{
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024", NULL);
    EXPECT_NE(res, HCF_SUCCESS);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest330, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA12315", &generator);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(generator, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest340, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA512|PRIMES_777", &generator);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(generator, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest350, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA512|PRIMES_3", &generator);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(generator, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest360, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA768|PRIMES_3", &generator);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(generator, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest370, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_4", &generator);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(generator, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest380, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA3072|PRIMES_4", &generator);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(generator, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest381, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA3072|PRIMES_3", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest390, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA4096|PRIMES_5", &generator);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(generator, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest391, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA8192|PRIMES_5", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest400, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);
    HcfObjDestroy(generator);
    generator = NULL;
    HcfObjDestroy(generator);
    HcfObjDestroy(NULL);
}

// generateKeyPair correct case
HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest500, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(keyPair, nullptr);
    EXPECT_NE(keyPair->priKey, nullptr);
    EXPECT_NE(keyPair->pubKey, nullptr);
    EXPECT_NE(keyPair->base.getClass(), nullptr);
    EXPECT_NE(keyPair->base.destroy, nullptr);

    HcfPubKey *pubkey = keyPair->pubKey;
    EXPECT_NE(pubkey->base.getAlgorithm((HcfKey *)pubkey), nullptr);
    EXPECT_NE(pubkey->base.getFormat((HcfKey *)pubkey), nullptr);
    EXPECT_NE(pubkey->base.base.getClass(), nullptr);
    EXPECT_NE(pubkey->base.base.destroy, nullptr);

    HcfPriKey *prikey = keyPair->priKey;
    EXPECT_NE(prikey->base.getAlgorithm((HcfKey *)prikey), nullptr);
    EXPECT_NE(prikey->base.getFormat((HcfKey *)prikey), nullptr);
    EXPECT_NE(prikey->base.base.getClass(), nullptr);
    EXPECT_NE(prikey->base.base.destroy, nullptr);
    EXPECT_NE(prikey->clearMem, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest510, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(keyPair, nullptr);
    EXPECT_NE(keyPair->priKey, nullptr);
    EXPECT_NE(keyPair->pubKey, nullptr);
    EXPECT_NE(keyPair->base.getClass(), nullptr);
    EXPECT_NE(keyPair->base.destroy, nullptr);

    HcfPubKey *pubkey = keyPair->pubKey;
    EXPECT_NE(pubkey->base.getAlgorithm((HcfKey *)pubkey), nullptr);
    EXPECT_NE(pubkey->base.getFormat((HcfKey *)pubkey), nullptr);
    EXPECT_NE(pubkey->base.base.getClass(), nullptr);
    EXPECT_NE(pubkey->base.base.destroy, nullptr);

    HcfPriKey *prikey = keyPair->priKey;
    EXPECT_NE(prikey->base.getAlgorithm((HcfKey *)prikey), nullptr);
    EXPECT_NE(prikey->base.getFormat((HcfKey *)prikey), nullptr);
    EXPECT_NE(prikey->base.base.getClass(), nullptr);
    EXPECT_NE(prikey->base.base.destroy, nullptr);
    EXPECT_NE(prikey->clearMem, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest511, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_3", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    HcfBlob pubKeyBlob = { .data = nullptr, .len = 0 };
    HcfBlob priKeyBlob = { .data = nullptr, .len = 0 };
    HcfPubKey *pubKey = keyPair->pubKey;
    HcfPriKey *priKey = keyPair->priKey;

    res = pubKey->base.getEncoded((HcfKey *)priKey, &pubKeyBlob);
    EXPECT_NE(res, HCF_SUCCESS);
    res = priKey->base.getEncoded((HcfKey *)pubKey, &priKeyBlob);
    EXPECT_NE(res, HCF_SUCCESS);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
    HcfFree(pubKeyBlob.data);
    HcfFree(priKeyBlob.data);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest512, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA2048|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    HcfPubKey *pubKey = keyPair->pubKey;
    HcfPriKey *priKey = keyPair->priKey;

    res = pubKey->base.getEncoded((HcfKey *)priKey, NULL);
    EXPECT_NE(res, HCF_SUCCESS);
    res = priKey->base.getEncoded((HcfKey *)pubKey, NULL);
    EXPECT_NE(res, HCF_SUCCESS);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

// generateKeyPair conrrect case: use getEncode encode pubkey and prikey
HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest520, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfBlob pubKeyBlob = {.data = NULL, .len = 0};
    HcfBlob priKeyBlob = {.data = NULL, .len = 0};
    res = keyPair->pubKey->base.getEncoded((HcfKey *)keyPair->pubKey, &pubKeyBlob);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = keyPair->priKey->base.getEncoded((HcfKey *)keyPair->priKey, &priKeyBlob);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfKeyPair *dupKeyPair = NULL;
    res = generator->convertKey(generator, NULL, &pubKeyBlob, &priKeyBlob, &dupKeyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfPubKey *pubkey = dupKeyPair->pubKey;
    EXPECT_NE(pubkey->base.getAlgorithm((HcfKey *)pubkey), nullptr);
    EXPECT_NE(pubkey->base.getFormat((HcfKey *)pubkey), nullptr);
    EXPECT_NE(pubkey->base.base.getClass(), nullptr);
    EXPECT_NE(pubkey->base.base.destroy, nullptr);

    HcfPriKey *prikey = dupKeyPair->priKey;
    EXPECT_NE(prikey->base.getAlgorithm((HcfKey *)prikey), nullptr);
    EXPECT_NE(prikey->base.getFormat((HcfKey *)prikey), nullptr);
    EXPECT_NE(prikey->base.base.getClass(), nullptr);
    EXPECT_NE(prikey->base.base.destroy, nullptr);
    EXPECT_NE(prikey->clearMem, nullptr);

    HcfFree(pubKeyBlob.data);
    HcfFree(priKeyBlob.data);
    HcfObjDestroy(generator);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(dupKeyPair);
}

// Test muliti getEncoded and convertKey
HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest521, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfBlob pubKeyBlob1 = {.data = NULL, .len = 0};
    HcfBlob priKeyBlob1 = {.data = NULL, .len = 0};
    res = keyPair->pubKey->base.getEncoded((HcfKey *)keyPair->pubKey, &pubKeyBlob1);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = keyPair->priKey->base.getEncoded((HcfKey *)keyPair->priKey, &priKeyBlob1);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfKeyPair *dupKeyPair1 = NULL;
    res = generator->convertKey(generator, NULL, &pubKeyBlob1, &priKeyBlob1, &dupKeyPair1);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfKeyPair *dupKeyPair2 = NULL;
    HcfBlob pubKeyBlob2 = {.data = NULL, .len = 0};
    HcfBlob priKeyBlob2 = {.data = NULL, .len = 0};
    res = keyPair->pubKey->base.getEncoded((HcfKey *)keyPair->pubKey, &pubKeyBlob2);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = keyPair->priKey->base.getEncoded((HcfKey *)keyPair->priKey, &priKeyBlob2);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = generator->convertKey(generator, NULL, &pubKeyBlob2, &priKeyBlob2, &dupKeyPair2);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfFree(pubKeyBlob1.data);
    HcfFree(priKeyBlob1.data);
    HcfFree(pubKeyBlob2.data);
    HcfFree(priKeyBlob2.data);
    HcfObjDestroy(generator);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(dupKeyPair1);
    HcfObjDestroy(dupKeyPair2);
}

// generateKeyPair correct case: getEncode encode pubkey
HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest530, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA3072|PRIMES_3", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfBlob pubKeyBlob = {.data = NULL, .len = 0};
    res = keyPair->pubKey->base.getEncoded((HcfKey *)keyPair->pubKey, &pubKeyBlob);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfKeyPair *dupKeyPair = NULL;
    res = generator->convertKey(generator, NULL, &pubKeyBlob, NULL, &dupKeyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfPubKey *pubkey = dupKeyPair->pubKey;
    EXPECT_NE(pubkey->base.getAlgorithm((HcfKey *)pubkey), nullptr);
    EXPECT_NE(pubkey->base.getFormat((HcfKey *)pubkey), nullptr);
    EXPECT_NE(pubkey->base.base.getClass(), nullptr);
    EXPECT_NE(pubkey->base.base.destroy, nullptr);

    HcfFree(pubKeyBlob.data);
    HcfObjDestroy(generator);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(dupKeyPair);
}

// generateKeyPair correct case: getEncode encode prikey
HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest540, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfBlob priKeyBlob = {.data = NULL, .len = 0};
    res = keyPair->priKey->base.getEncoded((HcfKey *)keyPair->priKey, &priKeyBlob);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfKeyPair *dupKeyPair = NULL;
    res = generator->convertKey(generator, NULL, NULL, &priKeyBlob, &dupKeyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfPriKey *prikey = dupKeyPair->priKey;
    EXPECT_NE(prikey->base.getAlgorithm((HcfKey *)prikey), nullptr);
    EXPECT_NE(prikey->base.getFormat((HcfKey *)prikey), nullptr);
    EXPECT_NE(prikey->base.base.getClass(), nullptr);
    EXPECT_NE(prikey->base.base.destroy, nullptr);
    EXPECT_NE(prikey->clearMem, nullptr);

    HcfFree(priKeyBlob.data);
    HcfObjDestroy(generator);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(dupKeyPair);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest550, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA3072", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfBlob pubKeyBlob = {.data = NULL, .len = 0};
    HcfBlob priKeyBlob = {.data = NULL, .len = 0};
    res = keyPair->pubKey->base.getEncoded((HcfKey *)keyPair->pubKey, &pubKeyBlob);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = keyPair->priKey->base.getEncoded((HcfKey *)keyPair->priKey, &priKeyBlob);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfKeyPair *dupKeyPair = NULL;
    res = generator->convertKey(generator, NULL, &pubKeyBlob, &priKeyBlob, &dupKeyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfPubKey *pubkey = dupKeyPair->pubKey;
    EXPECT_NE(pubkey->base.getAlgorithm((HcfKey *)pubkey), nullptr);
    EXPECT_NE(pubkey->base.getFormat((HcfKey *)pubkey), nullptr);
    EXPECT_NE(pubkey->base.base.getClass(), nullptr);
    EXPECT_NE(pubkey->base.base.destroy, nullptr);

    HcfPriKey *prikey = dupKeyPair->priKey;
    EXPECT_NE(prikey->base.getAlgorithm((HcfKey *)prikey), nullptr);
    EXPECT_NE(prikey->base.getFormat((HcfKey *)prikey), nullptr);
    EXPECT_NE(prikey->base.base.getClass(), nullptr);
    EXPECT_NE(prikey->base.base.destroy, nullptr);
    EXPECT_NE(prikey->clearMem, nullptr);

    HcfFree(pubKeyBlob.data);
    HcfFree(priKeyBlob.data);
    HcfObjDestroy(generator);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(dupKeyPair);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest560, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA4096", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfBlob pubKeyBlob = {.data = NULL, .len = 0};
    HcfBlob priKeyBlob = {.data = NULL, .len = 0};
    res = keyPair->pubKey->base.getEncoded((HcfKey *)keyPair->pubKey, &pubKeyBlob);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = keyPair->priKey->base.getEncoded((HcfKey *)keyPair->priKey, &priKeyBlob);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfKeyPair *dupKeyPair = NULL;
    res = generator->convertKey(generator, NULL, &pubKeyBlob, &priKeyBlob, &dupKeyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfPubKey *pubkey = dupKeyPair->pubKey;
    EXPECT_NE(pubkey->base.getAlgorithm((HcfKey *)pubkey), nullptr);
    EXPECT_NE(pubkey->base.getFormat((HcfKey *)pubkey), nullptr);
    EXPECT_NE(pubkey->base.base.getClass(), nullptr);
    EXPECT_NE(pubkey->base.base.destroy, nullptr);

    HcfPriKey *prikey = dupKeyPair->priKey;
    EXPECT_NE(prikey->base.getAlgorithm((HcfKey *)prikey), nullptr);
    EXPECT_NE(prikey->base.getFormat((HcfKey *)prikey), nullptr);
    EXPECT_NE(prikey->base.base.getClass(), nullptr);
    EXPECT_NE(prikey->base.base.destroy, nullptr);
    EXPECT_NE(prikey->clearMem, nullptr);

    HcfFree(pubKeyBlob.data);
    HcfFree(priKeyBlob.data);
    HcfObjDestroy(generator);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(dupKeyPair);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest570, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA512", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfBlob pubKeyBlob = {.data = NULL, .len = 0};
    HcfBlob priKeyBlob = {.data = NULL, .len = 0};
    res = keyPair->pubKey->base.getEncoded((HcfKey *)keyPair->pubKey, &pubKeyBlob);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = keyPair->priKey->base.getEncoded((HcfKey *)keyPair->priKey, &priKeyBlob);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfKeyPair *dupKeyPair = NULL;
    res = generator->convertKey(generator, NULL, &pubKeyBlob, &priKeyBlob, &dupKeyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfPubKey *pubkey = dupKeyPair->pubKey;
    EXPECT_NE(pubkey->base.getAlgorithm((HcfKey *)pubkey), nullptr);
    EXPECT_NE(pubkey->base.getFormat((HcfKey *)pubkey), nullptr);
    EXPECT_NE(pubkey->base.base.getClass(), nullptr);
    EXPECT_NE(pubkey->base.base.destroy, nullptr);

    HcfPriKey *prikey = dupKeyPair->priKey;
    EXPECT_NE(prikey->base.getAlgorithm((HcfKey *)prikey), nullptr);
    EXPECT_NE(prikey->base.getFormat((HcfKey *)prikey), nullptr);
    EXPECT_NE(prikey->base.base.getClass(), nullptr);
    EXPECT_NE(prikey->base.base.destroy, nullptr);
    EXPECT_NE(prikey->clearMem, nullptr);

    HcfFree(pubKeyBlob.data);
    HcfFree(priKeyBlob.data);
    HcfObjDestroy(generator);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(dupKeyPair);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest580, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA768", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfBlob pubKeyBlob = {.data = NULL, .len = 0};
    HcfBlob priKeyBlob = {.data = NULL, .len = 0};
    res = keyPair->pubKey->base.getEncoded((HcfKey *)keyPair->pubKey, &pubKeyBlob);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = keyPair->priKey->base.getEncoded((HcfKey *)keyPair->priKey, &priKeyBlob);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfKeyPair *dupKeyPair = NULL;
    res = generator->convertKey(generator, NULL, &pubKeyBlob, &priKeyBlob, &dupKeyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfPubKey *pubkey = dupKeyPair->pubKey;
    EXPECT_NE(pubkey->base.getAlgorithm((HcfKey *)pubkey), nullptr);
    EXPECT_NE(pubkey->base.getFormat((HcfKey *)pubkey), nullptr);
    EXPECT_NE(pubkey->base.base.getClass(), nullptr);
    EXPECT_NE(pubkey->base.base.destroy, nullptr);

    HcfPriKey *prikey = dupKeyPair->priKey;
    EXPECT_NE(prikey->base.getAlgorithm((HcfKey *)prikey), nullptr);
    EXPECT_NE(prikey->base.getFormat((HcfKey *)prikey), nullptr);
    EXPECT_NE(prikey->base.base.getClass(), nullptr);
    EXPECT_NE(prikey->base.base.destroy, nullptr);
    EXPECT_NE(prikey->clearMem, nullptr);

    HcfFree(pubKeyBlob.data);
    HcfFree(priKeyBlob.data);
    HcfObjDestroy(generator);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(dupKeyPair);
}

// generateKeyPair incorrect case: user wrong ECC class, ignore in this version
HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest600, TestSize.Level0)
{
    HcfAsyKeyGenerator *eccGenerator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("ECC224", &eccGenerator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(eccGenerator, nullptr);

    HcfAsyKeyGenerator *rsaGenerator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024", &rsaGenerator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(rsaGenerator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = rsaGenerator->generateKeyPair(eccGenerator, NULL, &keyPair);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(eccGenerator);
    HcfObjDestroy(rsaGenerator);
}

// generateKeyPair incorrect case: generator class is null
HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest610, TestSize.Level0)
{
    HcfAsyKeyGenerator *rsaGenerator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024", &rsaGenerator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(rsaGenerator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = rsaGenerator->generateKeyPair(NULL, NULL, &keyPair);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(keyPair, nullptr);

    HcfObjDestroy(rsaGenerator);
}

// generateKeyPair incorrect case: keypair is null
HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest620, TestSize.Level0)
{
    HcfAsyKeyGenerator *rsaGenerator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024", &rsaGenerator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(rsaGenerator, nullptr);

    res = rsaGenerator->generateKeyPair(rsaGenerator, NULL, NULL);
    EXPECT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(rsaGenerator);
}

// convertKey correct case
HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest700, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA2048", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfBlob pubKeyBlob = {.data = NULL, .len = 0};
    HcfBlob priKeyBlob = {.data = NULL, .len = 0};
    res = keyPair->pubKey->base.getEncoded((HcfKey *)keyPair->pubKey, &pubKeyBlob);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = keyPair->priKey->base.getEncoded((HcfKey *)keyPair->priKey, &priKeyBlob);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfKeyPair *dupKeyPair = NULL;
    res = generator->convertKey(generator, NULL, &pubKeyBlob, &priKeyBlob, &dupKeyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfPubKey *pubkey = dupKeyPair->pubKey;
    EXPECT_NE(pubkey->base.getAlgorithm((HcfKey *)pubkey), nullptr);
    EXPECT_NE(pubkey->base.getFormat((HcfKey *)pubkey), nullptr);
    EXPECT_NE(pubkey->base.base.getClass(), nullptr);
    EXPECT_NE(pubkey->base.base.destroy, nullptr);

    HcfPriKey *prikey = dupKeyPair->priKey;
    EXPECT_NE(prikey->base.getAlgorithm((HcfKey *)prikey), nullptr);
    EXPECT_NE(prikey->base.getFormat((HcfKey *)prikey), nullptr);
    EXPECT_NE(prikey->base.base.getClass(), nullptr);
    EXPECT_NE(prikey->base.base.destroy, nullptr);
    EXPECT_NE(prikey->clearMem, nullptr);

    HcfFree(pubKeyBlob.data);
    HcfFree(priKeyBlob.data);
    HcfObjDestroy(generator);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(dupKeyPair);
}

// convertKey incorrect case: input ECC class
HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest710, TestSize.Level0)
{
    HcfAsyKeyGenerator *eccGenerator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("ECC224", &eccGenerator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(eccGenerator, nullptr);

    HcfAsyKeyGenerator *rsaGenerator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024", &rsaGenerator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(rsaGenerator, nullptr);

    HcfKeyPair *dupKeyPair = NULL;
    res = rsaGenerator->convertKey(eccGenerator, NULL, NULL, NULL, &dupKeyPair);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(dupKeyPair, nullptr);

    HcfObjDestroy(eccGenerator);
    HcfObjDestroy(rsaGenerator);
}


// convertKey incorrect case: input null generator
HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest720, TestSize.Level0)
{
    HcfAsyKeyGenerator *rsaGenerator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024", &rsaGenerator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(rsaGenerator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = rsaGenerator->generateKeyPair(rsaGenerator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfBlob pubKeyBlob = {.data = NULL, .len = 0};
    HcfBlob priKeyBlob = {.data = NULL, .len = 0};
    res = keyPair->pubKey->base.getEncoded((HcfKey *)keyPair->pubKey, &pubKeyBlob);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = keyPair->priKey->base.getEncoded((HcfKey *)keyPair->priKey, &priKeyBlob);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfKeyPair *dupKeyPair = NULL;
    res = rsaGenerator->convertKey(NULL, NULL, &pubKeyBlob, &priKeyBlob, &dupKeyPair);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(dupKeyPair, nullptr);

    HcfFree(pubKeyBlob.data);
    HcfFree(priKeyBlob.data);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(rsaGenerator);
}

// convertKey incorrect case: input null dupkeypair
HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest730, TestSize.Level0)
{
    HcfAsyKeyGenerator *rsaGenerator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024", &rsaGenerator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(rsaGenerator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = rsaGenerator->generateKeyPair(rsaGenerator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfBlob pubKeyBlob = {.data = NULL, .len = 0};
    HcfBlob priKeyBlob = {.data = NULL, .len = 0};
    res = keyPair->pubKey->base.getEncoded((HcfKey *)keyPair->pubKey, &pubKeyBlob);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = keyPair->priKey->base.getEncoded((HcfKey *)keyPair->priKey, &priKeyBlob);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfKeyPair *dupKeyPair = NULL;
    res = rsaGenerator->convertKey(rsaGenerator, NULL, &pubKeyBlob, &priKeyBlob, NULL);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(dupKeyPair, nullptr);

    HcfFree(pubKeyBlob.data);
    HcfFree(priKeyBlob.data);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(rsaGenerator);
}

// Incorrect case: use wrong bits or primes
HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest800, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1111", &generator);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(generator, nullptr);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest810, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024|Primessf", &generator);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(generator, nullptr);
    HcfObjDestroy(generator);
}

// 测试异常释放
HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest820, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024|Primessf", &generator);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(generator, nullptr);
    HcfObjDestroy(generator);
}

// prikey clear mem
HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest830, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024", &generator);
    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(keyPair, nullptr);

    keyPair->priKey->clearMem(NULL);

    HcfObjDestroy(generator);
    HcfObjDestroy(keyPair);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest840, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024", &generator);
    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(keyPair, nullptr);

    keyPair->priKey->clearMem((HcfPriKey *)keyPair->pubKey);

    HcfObjDestroy(generator);
    HcfObjDestroy(keyPair);
}

HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest850, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024", &generator);
    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(keyPair, nullptr);

    keyPair->priKey->clearMem(keyPair->priKey);

    HcfObjDestroy(generator);
    HcfObjDestroy(keyPair);
}

// correct case: use destroy function inclass(not HcfObjDestroy)
HWTEST_F(CryptoRsaAsyKeyGeneratorTest, CryptoRsaAsyKeyGeneratorTest900, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    HcfPubKey *pubkey = keyPair->pubKey;
    HcfPriKey *prikey = keyPair->priKey;

    EXPECT_EQ(pubkey->base.getFormat((HcfKey *)prikey), nullptr);
    EXPECT_EQ(prikey->base.getFormat((HcfKey *)pubkey), nullptr);

    EXPECT_EQ(pubkey->base.getFormat(NULL), nullptr);
    EXPECT_EQ(prikey->base.getFormat(NULL), nullptr);

    EXPECT_EQ(pubkey->base.getAlgorithm((HcfKey *)prikey), nullptr);
    EXPECT_EQ(prikey->base.getAlgorithm((HcfKey *)pubkey), nullptr);

    EXPECT_EQ(pubkey->base.getAlgorithm(NULL), nullptr);
    EXPECT_EQ(prikey->base.getAlgorithm(NULL), nullptr);

    prikey->base.base.destroy(NULL);
    pubkey->base.base.destroy(NULL);
    keyPair->base.destroy(NULL);

    prikey->base.base.destroy((HcfObjectBase *)pubkey);
    pubkey->base.base.destroy((HcfObjectBase *)prikey);
    keyPair->base.destroy((HcfObjectBase *)prikey);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}
}
