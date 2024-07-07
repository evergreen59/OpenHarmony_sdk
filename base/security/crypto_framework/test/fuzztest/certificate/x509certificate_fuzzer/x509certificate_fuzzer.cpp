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

#include "x509certificate_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include "securec.h"

#include "blob.h"
#include "result.h"
#include "x509_certificate.h"

namespace OHOS {
    static char g_fuzzCert[] =
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIEMjCCAxqgAwIBAgICARAwDQYJKoZIhvcNAQELBQAwdjELMAkGA1UEBhMCQ04x\r\n"
    "CzAJBgNVBAgMAkJKMQswCQYDVQQHDAJCSjELMAkGA1UECgwCSEQxDDAKBgNVBAsM\r\n"
    "A2RldjELMAkGA1UEAwwCY2ExJTAjBgkqhkiG9w0BCQEWFmNhQGNyeXB0b2ZyYW1l\r\n"
    "d29yay5jb20wHhcNMjIwODE5MTI0OTA2WhcNMzIwODE2MTI0OTA2WjB2MQswCQYD\r\n"
    "VQQGEwJDTjELMAkGA1UECAwCQkoxCzAJBgNVBAcMAkJKMQswCQYDVQQKDAJIRDEM\r\n"
    "MAoGA1UECwwDZGV2MQswCQYDVQQDDAJjYTElMCMGCSqGSIb3DQEJARYWY2FAY3J5\r\n"
    "cHRvZnJhbWV3b3JrLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\r\n"
    "AJ8p0IWE7WwwbtATg+AbYQj33WNBBktU+/AVf+Tl1aAa4TOeW2/ZARc4sdwLVTxd\r\n"
    "XCipFseuiGN30hwXrXFUHrcMf0w2sCkznJVZ/rQcfEO5Kb1vBz6DEEcgISYEhhqO\r\n"
    "BfYBit5qfpq5R2+2R/Th/ybV+kBrUl+GssXbDAe6oZCy56lGphDvmHMUO7a13j+S\r\n"
    "FmThMbI2yeyua1LagSoaBJfY1J+i7jWPmmEFR0dQ2p0EGjHTgQGhRo5VuwDHipNS\r\n"
    "v0XP8OUA/PYbL/SBj1Fq4C3gtfvjeswUbzVaMoq/wCuy1qcXI80ZLe3whR24c0cX\r\n"
    "YFO0uGi9egPp24fw7yYGqgECAwEAAaOByTCBxjAdBgNVHQ4EFgQUjKM7QmMBs01R\r\n"
    "9uQttYN/GDkvt7UwHwYDVR0jBBgwFoAUjKM7QmMBs01R9uQttYN/GDkvt7UwEgYD\r\n"
    "VR0TAQH/BAgwBgEB/wIBAjALBgNVHQ8EBAMCAQYwHQYDVR0lBBYwFAYIKwYBBQUH\r\n"
    "AwEGCCsGAQUFBwMCMCEGA1UdEQQaMBiBFmNhQGNyeXB0b2ZyYW1ld29yay5jb20w\r\n"
    "IQYDVR0SBBowGIEWY2FAY3J5cHRvZnJhbWV3b3JrLmNvbTANBgkqhkiG9w0BAQsF\r\n"
    "AAOCAQEAh+4RE6cJ62/gLYssLkc7ESg7exKwZlmisHyBicuy/+XagOZ3cTbgQNXl\r\n"
    "QoZKbw/ks/B/cInbQGYbpAm47Sudo+I/G9xj0X7gQB9wtSrbStOs6SjnLiYU0xFc\r\n"
    "Fsc0j6k2SrlyiwRQcjS4POKiUS0Cm3F3DHGdj55PlBkXxudXCq2V3J3VwKf2bVjQ\r\n"
    "bzz2+M/Q1m+P7FhB+JmeO8eemkqMQ0tFMU3EM441NpejC5iFVAGgownC8S0B+fxH\r\n"
    "9dBJuHM6vpxEWw3ckZFDZQ1kd91YRgr7jY8fc0v/T0tzHWbOEVzklEIBWL1mompL\r\n"
    "BCwe0/Gw+BO60bfi2MoJw8t2IcB1Qw==\r\n"
    "-----END CERTIFICATE-----\r\n";

    static bool g_testFlag = true;

    static void TestGetEncoded(HcfX509Certificate *x509CertObj)
    {
        HcfEncodingBlob derBlob = { 0 };
        HcfResult res = x509CertObj->base.getEncoded(&(x509CertObj->base), &derBlob);
        if (res != HCF_SUCCESS) {
            return;
        }
        free(derBlob.data);
    }

    static void TestVerify(HcfX509Certificate *x509CertObj)
    {
        HcfPubKey *keyOut = nullptr;
        HcfResult res = x509CertObj->base.getPublicKey(&(x509CertObj->base), &keyOut);
        if (res != HCF_SUCCESS) {
            return;
        }
        (void)x509CertObj->base.verify(&(x509CertObj->base), keyOut);
        HcfObjDestroy(keyOut);
    }

    static void TestQuery(HcfX509Certificate *x509CertObj)
    {
        long serialNumber = x509CertObj->getSerialNumber(x509CertObj);
        if (serialNumber < 0) {
            return;
        }
        HcfBlob issuerName = { 0 };
        (void)x509CertObj->getIssuerName(x509CertObj, &issuerName);
        HcfBlobDataClearAndFree(&issuerName);

        HcfBlob subjectName = { 0 };
        (void)x509CertObj->getSubjectName(x509CertObj, &subjectName);
        HcfBlobDataClearAndFree(&subjectName);

        HcfBlob notBeforeTime = { 0 };
        (void)x509CertObj->getNotBeforeTime(x509CertObj, &notBeforeTime);
        HcfBlobDataClearAndFree(&notBeforeTime);

        HcfBlob notAfterTime = { 0 };
        (void)x509CertObj->getNotAfterTime(x509CertObj, &notAfterTime);
        HcfBlobDataClearAndFree(&notAfterTime);

        HcfBlob sigOut = { 0 };
        (void)x509CertObj->getSignature(x509CertObj, &sigOut);
        HcfBlobDataClearAndFree(&sigOut);

        HcfBlob sigAlgOid = { 0 };
        (void)x509CertObj->getSignatureAlgOid(x509CertObj, &sigAlgOid);
        HcfBlobDataClearAndFree(&sigAlgOid);

        HcfBlob sigAlgParamsOut = { 0 };
        (void)x509CertObj->getSignatureAlgParams(x509CertObj, &sigAlgParamsOut);
        HcfBlobDataClearAndFree(&sigAlgParamsOut);

        HcfArray keyUsageOut = { 0 };
        (void)x509CertObj->getExtKeyUsage(x509CertObj, &keyUsageOut);
        HcfArrayDataClearAndFree(&keyUsageOut);

        int32_t pathLen = x509CertObj->getBasicConstraints(x509CertObj);
        if (pathLen < 0) {
            return;
        }
        HcfArray subjectAltName = { 0 };
        (void)x509CertObj->getSubjectAltNames(x509CertObj, &subjectAltName);
        HcfArrayDataClearAndFree(&subjectAltName);
    }

    static void CreateOneCert(void)
    {
        HcfEncodingBlob inStream = { 0 };
        inStream.data = reinterpret_cast<uint8_t *>(g_fuzzCert);
        inStream.encodingFormat = HCF_FORMAT_PEM;
        inStream.len = strlen(g_fuzzCert) + 1;
        HcfX509Certificate *x509CertObj = nullptr;
        HcfResult res = HcfX509CertificateCreate(&inStream, &x509CertObj);
        if (res != HCF_SUCCESS) {
            return;
        }
        TestGetEncoded(x509CertObj);
        TestVerify(x509CertObj);
        TestQuery(x509CertObj);
    }

    bool X509CertificateFuzzTest(const uint8_t* data, size_t size)
    {
        if (g_testFlag) {
            CreateOneCert();
            g_testFlag = false;
        }
        if (data == nullptr) {
            return false;
        }
        HcfEncodingBlob inStream = { 0 };
        inStream.data = const_cast<uint8_t *>(data);
        inStream.encodingFormat = HCF_FORMAT_PEM;
        inStream.len = size;
        HcfX509Certificate *x509CertObj = nullptr;
        HcfResult res = HcfX509CertificateCreate(&inStream, &x509CertObj);
        if (res != HCF_SUCCESS) {
            return false;
        }
        HcfObjDestroy(x509CertObj);
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::X509CertificateFuzzTest(data, size);
    return 0;
}
