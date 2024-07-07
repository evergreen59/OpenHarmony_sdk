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

#include "aes_openssl_common.h"

#include "log.h"
#include "memory.h"
#include "result.h"

const unsigned char *GetIv(HcfParamsSpec *params)
{
    if (params == NULL) {
        return NULL;
    }
    HcfIvParamsSpec *spec = (HcfIvParamsSpec *)params;
    uint8_t *iv = spec->iv.data;
    return (const unsigned char *)iv;
}

int32_t GetCcmTagLen(HcfParamsSpec *params)
{
    if (params == NULL) {
        return 0;
    }
    HcfCcmParamsSpec *spec = (HcfCcmParamsSpec *)params;
    size_t tagLen = spec->tag.len;
    return (int)tagLen;
}

void *GetCcmTag(HcfParamsSpec *params)
{
    if (params == NULL) {
        return NULL;
    }
    HcfCcmParamsSpec *spec = (HcfCcmParamsSpec *)params;
    uint8_t *tag = spec->tag.data;
    return (void *)tag;
}

void FreeCipherData(CipherData **data)
{
    if (data == NULL || *data == NULL) {
        return;
    }
    if ((*data)->ctx != NULL) {
        EVP_CIPHER_CTX_free((*data)->ctx);
        (*data)->ctx = NULL;
    }
    if ((*data)->aad != NULL) {
        HcfFree((*data)->aad);
        (*data)->aad = NULL;
    }
    if ((*data)->iv != NULL) {
        HcfFree((*data)->iv);
        (*data)->iv = NULL;
    }
    if ((*data)->tag != NULL) {
        HcfFree((*data)->tag);
        (*data)->tag = NULL;
    }
    HcfFree(*data);
    *data = NULL;
}

void FreeRedundantOutput(HcfBlob *blob)
{
    if (blob == NULL) {
        return;
    }
    // when decrypt result is empty plaintext, out blob data maybe not null (malloc by hcf before decryption)
    if ((blob->len == 0) && (blob->data != NULL)) {
        HcfFree(blob->data);
        blob->data = NULL;
    }
}