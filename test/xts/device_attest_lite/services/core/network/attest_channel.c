/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "securec.h"
#include "attest_tls.h"
#include "attest_utils.h"
#include "attest_utils_log.h"
#include "attest_channel.h"

#define MAX_REQUEST_SIZE 2048
const int32_t TLS_READ_TIMEOUT_MS = 500; // time unit is ms

typedef int32_t(*VerifyFunc)(void*, mbedtls_x509_crt*, int32_t, uint32_t*);

static int32_t LazyVerifyCert(void* data, mbedtls_x509_crt* crt, int32_t depth, uint32_t* flags)
{
    (void)depth;
    if (crt == NULL || flags == NULL) {
        return ERR_NET_INVALID_ARG;
    }
    if (((*flags & MBEDTLS_X509_BADCERT_EXPIRED) != 0) || ((*flags & MBEDTLS_X509_BADCERT_FUTURE) != 0)) {
        (*flags) &= ~(MBEDTLS_X509_BADCERT_EXPIRED | MBEDTLS_X509_BADCERT_FUTURE);
    }
    if (*flags != 0) {
        int32_t certsNum = GetCommonCertSize();
        int32_t i = 0;
        while (i < certsNum) {
            mbedtls_x509_crt caCert;
            mbedtls_x509_crt_init(&caCert);
            int32_t ret = LoadCommonOtherCert(&caCert, (unsigned int)i);
            if (ret != 0) {
                mbedtls_x509_crt_free(&caCert);
                return ERR_NET_PARSE_CERT_FAIL;
            }
            i++;
            ret = mbedtls_x509_crt_verify(crt, &caCert, NULL, NULL, flags, NULL, NULL);
            if ((ret != 0) && (ret != MBEDTLS_ERR_X509_CERT_VERIFY_FAILED)) {
                mbedtls_x509_crt_free(&caCert);
                continue;
            }

            (*flags) &= ~(MBEDTLS_X509_BADCERT_EXPIRED | MBEDTLS_X509_BADCERT_FUTURE);
            mbedtls_x509_crt_free(&caCert);
            if (*flags == 0) {
                break;
            }
        }
    }
    ((void)data);
    return ATTEST_OK;
}

static int32_t TLSSetupConfig(TLSSession* session)
{
    if (session == NULL) {
        ATTEST_LOG_ERROR("[TLSSetupConfig] Invalid parameter.");
        return ERR_NET_INVALID_ARG;
    }
    TLSConfig* tlsConfig = &(session->tlsConfig);
    int32_t ret = mbedtls_ssl_config_defaults(&(tlsConfig->sslConf), MBEDTLS_SSL_IS_CLIENT,
        MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret != 0) {
        ATTEST_LOG_ERROR("[TLSSetupConfig] Set default configuration failed, return -0x%x.", -ret);
        return ERR_NET_SET_SSL_CONFIG_FAIL;
    }

    mbedtls_ssl_conf_authmode(&(tlsConfig->sslConf), MBEDTLS_SSL_VERIFY_REQUIRED);
    ret = mbedtls_ssl_conf_max_frag_len(&(tlsConfig->sslConf), MBEDTLS_SSL_MAX_FRAG_LEN_4096);
    if (ret != 0) {
        ATTEST_LOG_ERROR("[TLSSetupConfig] Set up max fragment failed, return -0x%x.", -ret);
        return ERR_NET_SET_SSL_CONFIG_FAIL;
    }
    mbedtls_ssl_conf_ca_chain(&(tlsConfig->sslConf), &(tlsConfig->caCert), NULL);
    mbedtls_ssl_conf_verify(&(tlsConfig->sslConf), (VerifyFunc)LazyVerifyCert, NULL);

    mbedtls_ssl_conf_rng(&(tlsConfig->sslConf), mbedtls_ctr_drbg_random, &(tlsConfig->ctrDrbgCtx));
    mbedtls_ssl_conf_read_timeout(&(tlsConfig->sslConf), TLS_READ_TIMEOUT_MS);

    ret = mbedtls_ssl_setup(&(tlsConfig->sslCtx), &(tlsConfig->sslConf));
    if (ret != 0) {
        ATTEST_LOG_ERROR("[TLSSetupConfig] Set up net configuration failed, return -0x%x.", -ret);
        return ERR_NET_SETUP_FAIL;
    }
    ret = mbedtls_ssl_set_hostname(&(tlsConfig->sslCtx), session->serverInfo.hostName);
    if (ret != 0) {
        return ERR_NET_SET_HOSTNAME_FAIL;
    }

    mbedtls_ssl_set_bio(&(tlsConfig->sslCtx), &(tlsConfig->netCtx), mbedtls_net_send,
        mbedtls_net_recv, mbedtls_net_recv_timeout);
    
    return ATTEST_OK;
}

int32_t TLSConnect(TLSSession* session)
{
    ATTEST_LOG_DEBUG("[TLSConnect] Begin.");
    if (session == NULL) {
        return ERR_NET_INVALID_ARG;
    }

    TLSConfig* tlsConfig = &(session->tlsConfig);
    mbedtls_net_init(&(tlsConfig->netCtx));
    mbedtls_ssl_init(&(tlsConfig->sslCtx));
    mbedtls_ssl_config_init(&(tlsConfig->sslConf));
    mbedtls_ctr_drbg_init(&(tlsConfig->ctrDrbgCtx));
    mbedtls_x509_crt_init(&(tlsConfig->caCert));
    mbedtls_entropy_init(&(tlsConfig->entropyCtx));

    int32_t ret = mbedtls_ctr_drbg_seed(&(tlsConfig->ctrDrbgCtx), mbedtls_entropy_func,
        &(tlsConfig->entropyCtx), (const uint8_t *)session->entropySeed, strlen(session->entropySeed));
    if (ret != 0) {
        ATTEST_LOG_ERROR("[TLSConnect] Generate DRGB seed failed, ret = -0x%x.", -ret);
        return ERR_NET_DRBG_SEED_FAIL;
    }

    // Load the trusted CA
    if ((ret = LoadCommonCert(&(tlsConfig->caCert))) != 0) {
        ATTEST_LOG_ERROR("[TLSConnect] TLS load cert failed, ret = -0x%x.", -ret);
        return ret;
    }

    ret = mbedtls_net_connect(&(tlsConfig->netCtx),
        session->serverInfo.hostName, session->serverInfo.port, MBEDTLS_NET_PROTO_TCP);
    if (ret != 0) {
        ATTEST_LOG_ERROR("[TLSConnect] Connect to server failed, ret = -0x%x.", -ret);
        return ERR_NET_CONNECT_FAIL;
    }

    ret = mbedtls_net_set_nonblock(&(tlsConfig->netCtx));
    if (ret != 0) {
        ATTEST_LOG_ERROR("[TLSConnect] Set non block failed, ret = -0x%x.", -ret);
        return ERR_NET_SET_NON_BLOCK_FAIL;
    }

    ret = TLSSetupConfig(session);
    if (ret != 0) {
        ATTEST_LOG_ERROR("[TLSConnect] Set TLS session failed, ret = -0x%x.", -ret);
        return ERR_NET_SETUP_FAIL;
    }

    while ((ret = mbedtls_ssl_handshake(&(tlsConfig->sslCtx))) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE &&
            ret != MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS) {
            ATTEST_LOG_ERROR("[TLSConnect] Handshake with server failed, ret = -0x%x.", -ret);
            return ERR_NET_HANDSHAKE_FAIL;
        }
    }
    ATTEST_LOG_DEBUG("[TLSConnect] End.");
    return ATTEST_OK;
}

int32_t TLSWrite(const TLSSession* session, const uint8_t* buf, size_t len)
{
    if (session == NULL || buf == NULL || len > MAX_REQUEST_SIZE) {
        return ERR_NET_INVALID_ARG;
    }
    size_t writeLen = 0;
    while (writeLen < len) {
        int32_t ret = mbedtls_ssl_write((mbedtls_ssl_context *)&(session->tlsConfig.sslCtx),
                                        (uint8_t*)(buf + writeLen), len - writeLen);
        if (ret < 0 && ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE &&
            ret != MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS) {
            ATTEST_LOG_ERROR("[TLSWrite] Write to server failed, ret = -0x%x.", -ret);
            return ERR_NET_WRITE_FAIL;
        }
        if (ret >= 0) {
            writeLen += ret;
        }
    }
    return ATTEST_OK;
}

int32_t TLSRead(const TLSSession* session, uint8_t* buf, size_t len)
{
    if (session == NULL || buf == NULL || len == 0) {
        return ERR_NET_INVALID_ARG;
    }
    size_t readLen = 0;
    while (readLen < len) {
        int32_t ret = mbedtls_ssl_read((mbedtls_ssl_context *)&(session->tlsConfig.sslCtx), (uint8_t*)(buf + readLen),
                                       len - readLen);
        if (ret < 0 && ret != MBEDTLS_ERR_SSL_TIMEOUT && ret != MBEDTLS_ERR_SSL_WANT_READ &&
            ret != MBEDTLS_ERR_SSL_WANT_WRITE && ret != MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS) {
            return ERR_NET_READ_FAIL;
        }
        if (ret >= 0) {
            readLen += ret;
        }
    }
    return ATTEST_OK;
}

int32_t TLSClose(TLSSession* session)
{
    if (session == NULL) {
        return ATTEST_ERR;
    }
    int32_t ret;
    TLSConfig* tlsConfig = &(session->tlsConfig);
    do {
        ret = mbedtls_ssl_close_notify(&(tlsConfig->sslCtx));
    } while (ret == MBEDTLS_ERR_SSL_WANT_WRITE);
    mbedtls_net_free(&(tlsConfig->netCtx));
    mbedtls_x509_crt_free(&(tlsConfig->caCert));
    mbedtls_ssl_free(&(tlsConfig->sslCtx));
    mbedtls_ssl_config_free(&(tlsConfig->sslConf));
    mbedtls_ctr_drbg_free(&(tlsConfig->ctrDrbgCtx));
    mbedtls_entropy_free(&(tlsConfig->entropyCtx));
    return ret;
}
