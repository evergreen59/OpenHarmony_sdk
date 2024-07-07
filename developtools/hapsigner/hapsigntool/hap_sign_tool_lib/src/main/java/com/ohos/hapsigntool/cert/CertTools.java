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

package com.ohos.hapsigntool.cert;

import com.ohos.hapsigntool.api.LocalizationAdapter;
import com.ohos.hapsigntool.api.ServiceApi;
import com.ohos.hapsigntool.api.model.Options;
import com.ohos.hapsigntool.error.CustomException;
import com.ohos.hapsigntool.error.ERROR;
import com.ohos.hapsigntool.utils.ValidateUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.bouncycastle.asn1.x500.X500Name;
import org.bouncycastle.asn1.x509.KeyPurposeId;
import org.bouncycastle.asn1.x509.KeyUsage;
import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.bouncycastle.operator.ContentSigner;
import org.bouncycastle.operator.OperatorCreationException;
import org.bouncycastle.operator.jcajce.JcaContentSignerBuilder;
import org.bouncycastle.pkcs.PKCS10CertificationRequest;
import org.bouncycastle.pkcs.jcajce.JcaPKCS10CertificationRequestBuilder;

import java.io.IOException;
import java.security.KeyPair;
import java.security.PrivateKey;
import java.security.cert.X509Certificate;
import java.security.interfaces.ECPrivateKey;
import java.security.interfaces.RSAPrivateKey;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * CertTools.
 *
 * @since 2021/12/28
 */
public final class CertTools {
    /**
     * Ten years, in days.
     */
    private static final int TEN_YEAR_DAY = 3650;

    /**
     * Three years, in dats.
     */
    private static final int THREE_YEAR_DAY = 1095;

    /**
     * Empty csr array.
     */
    private static final byte[] NO_CSR = {};

    /**
     * ECC.
     */
    private static final String ECC = "ECDSA";

    /**
     * Compile String.
     */
    private static final Pattern SIGN_ALGORITHM_PATTERN = Pattern.compile("^SHA([0-9]{3})with([A-Z]{1,5})$");

    /**
     * Logger.
     */
    private static final Logger LOGGER = LogManager.getLogger(ServiceApi.class);

    private CertTools() {
    }

    /**
     * Generate root ca certificate.
     *
     * @param keyPair keyPair
     * @param csr     csr
     * @param adapter adapter
     * @return X509Certificate
     */
    public static X509Certificate generateRootCaCert(KeyPair keyPair, byte[] csr, LocalizationAdapter adapter) {
        try {
            return new CertBuilder(keyPair, adapter.getIssuer(), csr,
                    adapter.getOptions().getInt(Options.VALIDITY, TEN_YEAR_DAY))
                    .withAuthorityKeyIdentifier(CertLevel.ROOT_CA)
                    .withBasicConstraints(CertLevel.ROOT_CA, true, true,
                            adapter.getBasicConstraintsPathLen())
                    .withKeyUsages(new KeyUsage(KeyUsage.keyCertSign | KeyUsage.cRLSign), true)
                    .withExtKeyUsages(null, false)
                    .build(adapter.getSignAlg());
        } catch (IOException exception) {
            LOGGER.debug(exception.getMessage(), exception);
            CustomException.throwException(ERROR.IO_CERT_ERROR, exception.getMessage());
        }
        return null;
    }

    /**
     * Generate sub ca certificate.
     *
     * @param keyPair keyPair
     * @param csr     csr
     * @param adapter parameter
     * @return X509Certificate
     */
    public static X509Certificate generateSubCert(KeyPair keyPair, byte[] csr, LocalizationAdapter adapter) {
        try {
            return new CertBuilder(keyPair, adapter.getIssuer(), csr,
                    adapter.getOptions().getInt(Options.VALIDITY, TEN_YEAR_DAY))
                    .withAuthorityKeyIdentifier(CertLevel.SUB_CA)
                    .withBasicConstraints(CertLevel.SUB_CA, true, true,
                            adapter.getBasicConstraintsPathLen())
                    .withKeyUsages(new KeyUsage(KeyUsage.keyCertSign | KeyUsage.cRLSign), true)
                    .build(adapter.getSignAlg());
        } catch (IOException exception) {
            LOGGER.debug(exception.getMessage(), exception);
            CustomException.throwException(ERROR.IO_CERT_ERROR, exception.getMessage());
        }
        return null;
    }

    /**
     * Generate certificate.
     *
     * @param keyPair keyPair
     * @param csr     csr
     * @param adapter parameter
     * @return X509Certificate
     */
    public static X509Certificate generateCert(KeyPair keyPair, byte[] csr, LocalizationAdapter adapter) {
        try {
            return new CertBuilder(keyPair, adapter.getIssuer(), csr,
                    adapter.getOptions().getInt(Options.VALIDITY, THREE_YEAR_DAY))
                    // Need CertLevel
                    .withAuthorityKeyIdentifier(CertLevel.ROOT_CA)
                    .withBasicConstraints(CertLevel.ROOT_CA,
                            adapter.isBasicConstraintsCritical(),
                            adapter.isBasicConstraintsCa(),
                            adapter.getBasicConstraintsPathLen())
                    .withKeyUsages(adapter.getKeyUsage(), adapter.isKeyUsageCritical())
                    .withExtKeyUsages(adapter.getExtKeyUsage(), adapter.isExtKeyUsageCritical())
                    .build(adapter.getSignAlg());
        } catch (IOException exception) {
            LOGGER.debug(exception.getMessage(), exception);
            CustomException.throwException(ERROR.IO_CERT_ERROR, exception.getMessage());
        }
        return null;
    }

    /**
     * Generate app certificate.
     *
     * @param keyPair keyPair
     * @param csr     csr
     * @param adapter adapter
     * @return X509Certificate
     */
    public static X509Certificate generateEndCert(KeyPair keyPair, byte[] csr, LocalizationAdapter adapter,
                                                  byte[] signingCapabiltyBytes) {
        try {
            return new CertBuilder(keyPair, adapter.getIssuer(), csr,
                    adapter.getOptions().getInt(Options.VALIDITY, THREE_YEAR_DAY))
                    .withBasicConstraints(CertLevel.END_ENTITY, false, false,
                            null)
                    .withKeyUsages(new KeyUsage(KeyUsage.digitalSignature), true)
                    .withExtKeyUsages(new KeyPurposeId[]{KeyPurposeId.id_kp_codeSigning}, false)
                    .withSigningCapabilty(signingCapabiltyBytes)
                    .build(adapter.getSignAlg());
        } catch (IOException exception) {
            LOGGER.debug(exception.getMessage(), exception);
            CustomException.throwException(ERROR.IO_CERT_ERROR, exception.getMessage());
        }
        return null;
    }

    /**
     * generateCsr.
     *
     * @param keyPair       Applier keypair
     * @param signAlgorithm sign algorithm
     * @param subject       Applier subject
     * @return csr bytes
     */
    public static byte[] generateCsr(KeyPair keyPair, String signAlgorithm, X500Name subject) {
        JcaPKCS10CertificationRequestBuilder csrBuilder = new JcaPKCS10CertificationRequestBuilder(subject,
                keyPair.getPublic());
        PKCS10CertificationRequest csr = csrBuilder.build(createFixedContentSigner(keyPair.getPrivate(),
                signAlgorithm));
        try {
            return csr.getEncoded();
        } catch (IOException exception) {
            LOGGER.debug(exception.getMessage(), exception);
            CustomException.throwException(ERROR.IO_CSR_ERROR, "Not support " + subject);
            return NO_CSR;
        }
    }

    /**
     * Auto fix algorithm according key type and create content signer.
     *
     * @param privateKey    Sign key
     * @param signAlgorithm Sign algorithm
     * @return ContentSigner
     */
    public static ContentSigner createFixedContentSigner(PrivateKey privateKey, String signAlgorithm) {
        Matcher matcher = SIGN_ALGORITHM_PATTERN.matcher(signAlgorithm);
        ValidateUtils.throwIfNotMatches(matcher.matches(), ERROR.NOT_SUPPORT_ERROR, "Not Support " + signAlgorithm);
        // Auto fix signAlgorithm error
        if (privateKey instanceof ECPrivateKey && signAlgorithm.contains("RSA")) {
            signAlgorithm = signAlgorithm.replace("RSA", ECC);
        } else {
            if (privateKey instanceof RSAPrivateKey && signAlgorithm.contains(ECC)) {
                signAlgorithm = signAlgorithm.replace(ECC, "RSA");
            }
        }

        JcaContentSignerBuilder jcaContentSignerBuilder = new JcaContentSignerBuilder(signAlgorithm);
        jcaContentSignerBuilder.setProvider(BouncyCastleProvider.PROVIDER_NAME);
        try {
            return jcaContentSignerBuilder.build(privateKey);
        } catch (OperatorCreationException exception) {
            LOGGER.debug(exception.getMessage(), exception);
            CustomException.throwException(ERROR.OPERATOR_CREATION_ERROR, exception.getMessage());
        }
        return null;
    }

}
