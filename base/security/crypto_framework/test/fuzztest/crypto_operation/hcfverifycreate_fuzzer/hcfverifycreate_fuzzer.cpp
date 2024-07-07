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

#include "hcfverifycreate_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <string>

#include "asy_key_generator.h"
#include "blob.h"
#include "result.h"
#include "signature.h"

namespace OHOS {
    static char g_mockMessage[] = "hello world";
    const int INPUT_MSG_LEN = 12;

    static void TestVerify(void)
    {
        HcfAsyKeyGenerator *generator = NULL;
        HcfResult res = HcfAsyKeyGeneratorCreate("ECC224", &generator);
        if (res != HCF_SUCCESS) {
            return;
        }

        HcfKeyPair *ecc224KeyPair = NULL;
        res = generator->generateKeyPair(generator, NULL, &ecc224KeyPair);
        HcfObjDestroy(generator);
        if (res != HCF_SUCCESS) {
            return;
        }

        HcfSign *sign = NULL;
        res = HcfSignCreate("ECC224|SHA384", &sign);
        if (res != HCF_SUCCESS) {
            HcfObjDestroy(ecc224KeyPair);
            return;
        }
        static HcfBlob mockInput = {
            .data = reinterpret_cast<uint8_t *>(g_mockMessage),
            .len = INPUT_MSG_LEN
        };
        (void)sign->init(sign, NULL, ecc224KeyPair->priKey);
        (void)sign->update(sign, &mockInput);

        HcfVerify *verify = NULL;
        res = HcfVerifyCreate("ECC224|SHA384", &verify);
        if (res != HCF_SUCCESS) {
            HcfObjDestroy(ecc224KeyPair);
            HcfObjDestroy(sign);
            return;
        }
        HcfBlob out = {
            .data = NULL,
            .len = 0
        };
        (void)sign->sign(sign, NULL, &out);
        (void)verify->init(verify, NULL, ecc224KeyPair->pubKey);
        (void)verify->update(verify, &mockInput);
        (void)verify->verify(verify, NULL, &out);
        HcfObjDestroy(ecc224KeyPair);
        HcfObjDestroy(sign);
        HcfBlobDataFree(&out);
        HcfObjDestroy(verify);
    }

    bool HcfVerifyCreateFuzzTest(const uint8_t* data, size_t size)
    {
        TestVerify();
        HcfVerify *verify = nullptr;
        std::string algoName(reinterpret_cast<const char *>(data), size);
        HcfResult res = HcfVerifyCreate(algoName.c_str(), &verify);
        if (res != HCF_SUCCESS) {
            return false;
        }
        HcfObjDestroy(verify);
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::HcfVerifyCreateFuzzTest(data, size);
    return 0;
}
