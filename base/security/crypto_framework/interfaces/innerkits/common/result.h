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

#ifndef HCF_RESULT_H
#define HCF_RESULT_H

typedef enum HcfResult {
    /** Indicates success. */
    HCF_SUCCESS = 0,
    /** Indicates that input params is invalid . */
    HCF_INVALID_PARAMS = -10001,
    /** Indicates that function or algorithm is not supported. */
    HCF_NOT_SUPPORT = -10002,

    /** Indicates that memory malloc fails. */
    HCF_ERR_MALLOC = 20001,
    /** Indicates that memory copy fails. */
    HCF_ERR_COPY = -20002,

    /** Indicates that third part has something wrong. */
    HCF_ERR_CRYPTO_OPERATION = -30001,
    /* Indicates that cert signature check fails. */
    HCF_ERR_CERT_SIGNATURE_FAILURE = -30002,
    /* Indicates that cert is not yet valid. */
    HCF_ERR_CERT_NOT_YET_VALID = -30003,
    /* Indicates that cert has expired. */
    HCF_ERR_CERT_HAS_EXPIRED = -30004,
    /* Indicates that we can not get the untrusted cert's issuer. */
    HCF_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY = -30005,
    /* Key usage does not include certificate sign. */
    HCF_ERR_KEYUSAGE_NO_CERTSIGN = -30006,
    /* Key usage does not include digital sign. */
    HCF_ERR_KEYUSAGE_NO_DIGITAL_SIGNATURE = -30007,
} HcfResult;

#endif
