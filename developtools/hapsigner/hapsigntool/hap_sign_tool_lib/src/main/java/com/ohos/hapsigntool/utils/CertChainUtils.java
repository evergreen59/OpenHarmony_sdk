/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

package com.ohos.hapsigntool.utils;

import com.ohos.hapsigntool.error.CustomException;
import com.ohos.hapsigntool.error.ERROR;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import javax.security.auth.x500.X500Principal;
import java.io.IOException;
import java.math.BigInteger;
import java.security.InvalidAlgorithmParameterException;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.cert.CertPath;
import java.security.cert.CertPathBuilder;
import java.security.cert.CertPathBuilderException;
import java.security.cert.CertPathValidator;
import java.security.cert.CertPathValidatorException;
import java.security.cert.CertPathValidatorResult;
import java.security.cert.CertStore;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;
import java.security.cert.CollectionCertStoreParameters;
import java.security.cert.PKIXBuilderParameters;
import java.security.cert.PKIXCertPathBuilderResult;
import java.security.cert.PKIXCertPathValidatorResult;
import java.security.cert.PKIXParameters;
import java.security.cert.X509CertSelector;
import java.security.cert.X509Certificate;
import java.time.LocalDateTime;
import java.time.ZoneId;
import java.util.Base64;
import java.util.Date;
import java.util.List;

/**
 * Check cert list is cert chain.
 */
public class CertChainUtils {
    private static final Logger LOGGER = LogManager.getLogger(CertUtils.class);

    private CertChainUtils() {
    }

    private static CertPath getCertPath(List<X509Certificate> certificates, KeyStore trustStore, X500Principal issuer,
                                        BigInteger serial) throws KeyStoreException, InvalidAlgorithmParameterException,
            NoSuchAlgorithmException, CertPathBuilderException, CertificateException {
        if (certificates.size() != 1 && (issuer != null || serial != null)) {
            X509CertSelector targetCertSelector = new X509CertSelector();
            targetCertSelector.setIssuer(issuer);
            targetCertSelector.setSerialNumber(serial);
            PKIXBuilderParameters params = new PKIXBuilderParameters(trustStore, targetCertSelector);
            CertStore certStore = CertStore.getInstance("Collection",
                    new CollectionCertStoreParameters(certificates));
            params.addCertStore(certStore);
            params.setDate(Date.from(LocalDateTime.now().atZone(ZoneId.systemDefault()).toInstant()));
            params.setRevocationEnabled(false);
            CertPathBuilder certPathBuilder = CertPathBuilder.getInstance("PKIX");
            PKIXCertPathBuilderResult result = (PKIXCertPathBuilderResult) certPathBuilder.build(params);
            return result.getCertPath();
        } else {
            CertificateFactory factory = CertificateFactory.getInstance("X.509");
            return factory.generateCertPath(certificates);
        }
    }


    /**
     * Check cert list is cert chain.
     *
     * @param certificates cert list to be verified
     * @param issuer       issuer
     * @param serial       serial number
     * @param root         root cert
     */
    public static void verifyCertChain(List<X509Certificate> certificates, X500Principal issuer, BigInteger serial,
                                       X509Certificate root) {
        try {
            KeyStore trustStore = KeyStore.getInstance(KeyStore.getDefaultType());
            trustStore.load(null, null);
            trustStore.setCertificateEntry("root", root);
            CertPath certPath = getCertPath(certificates, trustStore, issuer, serial);
            PKIXParameters params = new PKIXParameters(trustStore);
            params.setRevocationEnabled(false);
            params.setDate(Date.from(LocalDateTime.now().atZone(ZoneId.systemDefault()).toInstant()));
            CertStore certStore = CertStore.getInstance("Collection", new CollectionCertStoreParameters());
            params.addCertStore(certStore);
            CertPathValidator validator = CertPathValidator.getInstance("PKIX");
            CertPathValidatorResult validatorResult = validator.validate(certPath, params);
            ValidateUtils.throwIfNotMatches(validatorResult instanceof PKIXCertPathValidatorResult,
                    ERROR.VERIFY_ERROR, "Validator result not target type");
            if (validatorResult instanceof PKIXCertPathValidatorResult) {
                PKIXCertPathValidatorResult pkixValidatorResult = (PKIXCertPathValidatorResult) validatorResult;
                ValidateUtils.throwIfNotMatches(params.getTrustAnchors().contains(pkixValidatorResult.getTrustAnchor()),
                        ERROR.VERIFY_ERROR, "Anchor is not trusted: " + Base64.getEncoder().encodeToString(
                                pkixValidatorResult.getTrustAnchor().getTrustedCert().getEncoded()));
            } else {
                CustomException.throwException(ERROR.VERIFY_ERROR, "Validator result not target type");
            }
        } catch (InvalidAlgorithmParameterException | NoSuchAlgorithmException | IOException | CertificateException
                | KeyStoreException | CertPathBuilderException | CertPathValidatorException exception) {
            LOGGER.debug(exception.getMessage(), exception);
            CustomException.throwException(ERROR.VERIFY_ERROR, "Failed to verify signature: " + exception.getMessage());
        }
    }
}
