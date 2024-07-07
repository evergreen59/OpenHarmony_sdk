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

#include "asykeygenerator_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <string>

#include "asy_key_generator.h"
#include "blob.h"
#include "result.h"

namespace OHOS {
    static bool g_testFlag = true;
    static const int ECC224_PUB_KEY_LEN = 80;
    static const int ECC224_PRI_KEY_LEN = 44;
    static uint8_t g_mockEcc224PubKey[ECC224_PUB_KEY_LEN] = { 48, 78, 48, 16, 6, 7, 42, 134, 72, 206,
        61, 2, 1, 6, 5, 43, 129, 4, 0, 33, 3, 58, 0, 4, 252, 171, 11, 115, 79, 252, 109, 120, 46, 97, 131, 145, 207,
        141, 146, 235, 133, 37, 218, 180, 8, 149, 47, 244, 137, 238, 207, 95, 153, 65, 250, 32, 77, 184, 249, 181,
        172, 192, 2, 99, 194, 170, 25, 44, 255, 87, 246, 42, 133, 83, 66, 197, 97, 95, 12, 84 };

    static uint8_t g_mockEcc224PriKey[ECC224_PRI_KEY_LEN] = { 48, 42, 2, 1, 1, 4, 28, 250, 86, 6,
        147, 222, 43, 252, 139, 90, 139, 5, 33, 184, 230, 26, 68, 94, 57, 145, 229, 146, 49, 221, 119, 206, 32, 198,
        19, 160, 7, 6, 5, 43, 129, 4, 0, 33 };

    static void TestEccKey(void)
    {
        HcfAsyKeyGenerator *generator = nullptr;
        int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);
        if (res != HCF_SUCCESS) {
            return;
        }
        (void)generator->getAlgoName(generator);
        HcfKeyPair *keyPair = nullptr;
        res = generator->generateKeyPair(generator, nullptr, &keyPair);
        if (res != HCF_SUCCESS) {
            HcfObjDestroy(generator);
            return;
        }
        HcfKeyPair *convertKeyPair = nullptr;
        static HcfBlob mockEcc224PubKeyBlob = {
            .data = g_mockEcc224PubKey,
            .len = ECC224_PUB_KEY_LEN
        };

        static HcfBlob mockEcc224PriKeyBlob = {
            .data = g_mockEcc224PriKey,
            .len = ECC224_PRI_KEY_LEN
        };
        (void)generator->convertKey(generator, nullptr, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &convertKeyPair);
        HcfObjDestroy(keyPair);
        HcfObjDestroy(generator);
        HcfObjDestroy(convertKeyPair);
    }

    static void TestRsaKey(void)
    {
        HcfAsyKeyGenerator *generator = nullptr;
        HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024", &generator);
        if (res != HCF_SUCCESS) {
            return;
        }
        HcfKeyPair *keyPair = nullptr;
        res = generator->generateKeyPair(generator, nullptr, &keyPair);
        if (res != HCF_SUCCESS) {
            HcfObjDestroy(generator);
            return;
        }
        HcfBlob pubKeyBlob = {.data = nullptr, .len = 0};
        HcfBlob priKeyBlob = {.data = nullptr, .len = 0};
        (void)keyPair->pubKey->base.getEncoded(&(keyPair->pubKey->base), &pubKeyBlob);
        (void)keyPair->priKey->base.getEncoded(&(keyPair->priKey->base), &priKeyBlob);

        HcfKeyPair *dupKeyPair = nullptr;
        (void)generator->convertKey(generator, nullptr, &pubKeyBlob, &priKeyBlob, &dupKeyPair);
        HcfPubKey *pubKey = dupKeyPair->pubKey;
        (void)pubKey->base.getAlgorithm(&(pubKey->base));
        (void)pubKey->base.getFormat(&(pubKey->base));
        (void)pubKey->base.base.getClass();

        HcfBlobDataFree(&pubKeyBlob);
        HcfBlobDataFree(&priKeyBlob);
        HcfObjDestroy(generator);
        HcfObjDestroy(keyPair);
        HcfObjDestroy(dupKeyPair);
    }

    bool AsyKeyGeneratorFuzzTest(const uint8_t* data, size_t size)
    {
        if (g_testFlag) {
            TestEccKey();
            TestRsaKey();
            g_testFlag = false;
        }
        HcfAsyKeyGenerator *generator = nullptr;
        std::string algoName(reinterpret_cast<const char *>(data), size);
        HcfResult res = HcfAsyKeyGeneratorCreate(algoName.c_str(), &generator);
        if (res != HCF_SUCCESS) {
            return false;
        }
        HcfObjDestroy(generator);
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AsyKeyGeneratorFuzzTest(data, size);
    return 0;
}
