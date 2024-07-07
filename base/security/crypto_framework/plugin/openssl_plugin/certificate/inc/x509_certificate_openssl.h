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

#ifndef X509_CERTIFICATE_OEPNSSL_H
#define X509_CERTIFICATE_OEPNSSL_H

#include "x509_certificate_spi.h"

#ifdef __cplusplus
extern "C" {
#endif

HcfResult OpensslX509CertSpiCreate(const HcfEncodingBlob *inStream, HcfX509CertificateSpi **spi);

#ifdef __cplusplus
}
#endif

#endif // X509_CERTIFICATE_OEPNSSL_H