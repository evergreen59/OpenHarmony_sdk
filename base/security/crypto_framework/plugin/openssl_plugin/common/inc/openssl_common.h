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

#ifndef HCF_OPENSSL_COMMON_H
#define HCF_OPENSSL_COMMON_H

#include <stdint.h>
#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>

#define HCF_OPENSSL_SUCCESS 1     /* openssl return 1: success */
#define HCF_BITS_PER_BYTE 8

#ifdef __cplusplus
extern "C" {
#endif

const char *GetAlgorithmName(const char *oid);
int32_t GetOpensslCurveId(int32_t keyLen, int32_t *returnCurveId);
const EVP_MD *GetOpensslDigestAlg(uint32_t alg);
void HcfPrintOpensslError(void);

int32_t GetOpensslPadding(int32_t padding, int32_t *opensslPadding);

int32_t GetRealPrimes(int32_t primesFlag);

#ifdef __cplusplus
}
#endif

#endif
