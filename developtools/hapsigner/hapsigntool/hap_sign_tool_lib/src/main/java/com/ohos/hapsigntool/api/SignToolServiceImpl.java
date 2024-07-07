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

package com.ohos.hapsigntool.api;


import com.ohos.hapsigntool.api.model.Options;
import com.ohos.hapsigntool.cert.CertTools;
import com.ohos.hapsigntool.error.CustomException;
import com.ohos.hapsigntool.error.ERROR;
import com.ohos.hapsigntool.hap.provider.LocalJKSSignProvider;
import com.ohos.hapsigntool.hap.provider.RemoteSignProvider;
import com.ohos.hapsigntool.hap.provider.SignProvider;
import com.ohos.hapsigntool.hap.verify.VerifyHap;
import com.ohos.hapsigntool.profile.ProfileSignTool;
import com.ohos.hapsigntool.profile.VerifyHelper;
import com.ohos.hapsigntool.profile.model.VerificationResult;
import com.ohos.hapsigntool.utils.CertUtils;
import com.ohos.hapsigntool.utils.FileUtils;
import com.ohos.hapsigntool.utils.ProfileUtils;
import com.ohos.hapsigntool.utils.StringUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.bouncycastle.jce.provider.BouncyCastleProvider;

import java.io.File;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.security.KeyPair;
import java.security.Security;
import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Arrays;

/**
 * Main entry of lib.
 *
 * @since 2021/12/28
 */
public class SignToolServiceImpl implements ServiceApi {
    static {
        Security.addProvider(new BouncyCastleProvider());
    }

    /**
     * App signing Capabilty Bytes.
     */
    private static final byte[] APP_SIGNING_CAPABILITY = {0x30, 0x06, 0x02, 0x01, 0x01, 0x0A, 0x01, 0x00};

    /**
     * Profile signing Capabilty Bytes.
     */
    private static final byte[] PROFILE_SIGNING_CAPABILITY = {0x30, 0x06, 0x02, 0x01, 0x01, 0x0A, 0x01, 0x01};

    /**
     * Logger.
     */
    private static final Logger logger = LogManager.getLogger(ServiceApi.class);

    /**
     * Generate keyStore.
     *
     * @param options options
     * @return Generate or not
     */
    @Override
    public boolean generateKeyStore(Options options) {
        LocalizationAdapter adapter = new LocalizationAdapter(options);
        adapter.errorOnExist(options.getString(Options.KEY_ALIAS));
        KeyPair keyPair = adapter.getAliasKey(true);
        adapter.releasePwd();
        return keyPair != null;
    }

    /**
     * Generate csr.
     *
     * @param options options
     * @return Generate or not
     */
    @Override
    public boolean generateCsr(Options options) {
        LocalizationAdapter adapter = new LocalizationAdapter(options);
        KeyPair keyPair = adapter.getAliasKey(false);
        adapter.releasePwd();
        byte[] csr = CertTools.generateCsr(keyPair, adapter.getSignAlg(), adapter.getSubject());
        if (csr == null) {
            return false;
        }
        String csrContent = CertUtils.toCsrTemplate(csr);
        outputString(csrContent, adapter.getOutFile());
        return true;
    }

    /**
     * Generate cert.
     *
     * @param options options
     * @return Generate or not
     */
    @Override
    public boolean generateCert(Options options) {
        LocalizationAdapter adapter = new LocalizationAdapter(options);
        adapter.errorIfNotExist(adapter.getOptions().getString(Options.KEY_ALIAS));
        String issuerAlias = adapter.getOptions().getString(Options.ISSUER_KEY_ALIAS);
        adapter.errorIfNotExist(issuerAlias);

        KeyPair subjectKeyPair = adapter.getAliasKey(false);
        if (options.containsKey(Options.ISSUER_KEY_STORE_FILE)){
            adapter.setKeyStoreHelper(null);
            adapter.setIssuerKeyStoreFile(true);
        }
        KeyPair rootKeyPair = adapter.getIssuerAliasKey();
        adapter.releasePwd();
        byte[] csr = CertTools.generateCsr(subjectKeyPair, adapter.getSignAlg(), adapter.getSubject());

        X509Certificate cert = CertTools.generateCert(rootKeyPair, csr, adapter);
        return outputCert(cert, adapter.getOutFile());
    }

    /**
     * Generate CA.
     *
     * @param options options
     * @return Generate or not
     */
    @Override
    public boolean generateCA(Options options) {
        LocalizationAdapter adapter = new LocalizationAdapter(options);
        boolean genRootCA = StringUtils.isEmpty(options.getString(Options.ISSUER_KEY_ALIAS));
        KeyPair subKey = adapter.getAliasKey(true);
        KeyPair rootKey;
        String ksFile = options.getString(Options.KEY_STORE_FILE);
        String iksFile = options.getString(Options.ISSUER_KEY_STORE_FILE);
        if (genRootCA) {
            if (!StringUtils.isEmpty(iksFile) && !ksFile.equals(iksFile)){
                CustomException.throwException(ERROR.WRITE_FILE_ERROR,
                        String.format("Parameter '%s' and parameter '%s' are inconsistent",ksFile,iksFile));
            }
            if (options.containsKey(Options.ISSUER_KEY_STORE_RIGHTS) ){
                boolean isEqual = Arrays.equals(options.getChars(Options.KEY_STORE_RIGHTS),
                        options.getChars(Options.ISSUER_KEY_STORE_RIGHTS));
                if (!isEqual){
                    CustomException.throwException(ERROR.WRITE_FILE_ERROR,
                            String.format("Parameter '%s' and parameter '%s' are inconsistent",
                                    Options.KEY_STORE_RIGHTS,Options.ISSUER_KEY_STORE_RIGHTS));
                }
            }
            rootKey = subKey;
        } else {
            if (options.containsKey(Options.ISSUER_KEY_STORE_FILE)){
                FileUtils.validFileType(options.getString(Options.ISSUER_KEY_STORE_FILE), "p12", "jks");
                adapter.setKeyStoreHelper(null);
                adapter.setIssuerKeyStoreFile(true);
            }
            rootKey = adapter.getIssuerAliasKey();
        }
        adapter.releasePwd();

        byte[] csr = CertTools.generateCsr(subKey, adapter.getSignAlg(), adapter.getSubject());
        X509Certificate cert;
        if (genRootCA) {
            cert = CertTools.generateRootCaCert(rootKey, csr, adapter);
        } else {
            cert = CertTools.generateSubCert(rootKey, csr, adapter);
        }
        return outputCert(cert, adapter.getOutFile());
    }

    /**
     * Generate app cert.
     *
     * @param options options
     * @return Generate or not
     */
    @Override
    public boolean generateAppCert(Options options) {
        LocalizationAdapter adapter = new LocalizationAdapter(options);
        KeyPair keyPair = adapter.getAliasKey(false);
        if (options.containsKey(Options.ISSUER_KEY_STORE_FILE)){
            adapter.setKeyStoreHelper(null);
            adapter.setIssuerKeyStoreFile(true);
        }
        KeyPair issueKeyPair = adapter.getIssuerAliasKey();
        adapter.releasePwd();

        byte[] csr = CertTools.generateCsr(keyPair, adapter.getSignAlg(), adapter.getSubject());
        X509Certificate cert = CertTools.generateEndCert(issueKeyPair, csr, adapter, APP_SIGNING_CAPABILITY);
        if (adapter.isOutFormChain()) {
            List<X509Certificate> certificates = new ArrayList<>();
            certificates.add(cert);
            certificates.add(adapter.getSubCaCertFile());
            certificates.add(adapter.getCaCertFile());
            return outputCertChain(certificates, adapter.getOutFile());
        } else {
            return outputCert(cert, adapter.getOutFile());
        }
    }

    /**
     * Generate profile cert.
     *
     * @param options options
     * @return Generate or not
     */
    @Override
    public boolean generateProfileCert(Options options) {
        LocalizationAdapter adapter = new LocalizationAdapter(options);
        KeyPair keyPair = adapter.getAliasKey(false);
        if (options.containsKey(Options.ISSUER_KEY_STORE_FILE)){
            adapter.setKeyStoreHelper(null);
            adapter.setIssuerKeyStoreFile(true);
        }
        KeyPair issueKeyPair = adapter.getIssuerAliasKey();
        adapter.releasePwd();

        byte[] csr = CertTools.generateCsr(keyPair, adapter.getSignAlg(), adapter.getSubject());
        X509Certificate cert = CertTools.generateEndCert(issueKeyPair, csr, adapter, PROFILE_SIGNING_CAPABILITY);
        if (adapter.isOutFormChain()) {
            List<X509Certificate> certificates = new ArrayList<>();
            certificates.add(cert);
            certificates.add(adapter.getSubCaCertFile());
            certificates.add(adapter.getCaCertFile());
            return outputCertChain(certificates, adapter.getOutFile());
        } else {
            return outputCert(cert, adapter.getOutFile());
        }
    }

    /**
     * Sign for profile.
     *
     * @param options options
     * @return Sign or not
     */
    @Override
    public boolean signProfile(Options options) {
        boolean result;
        try {
            LocalizationAdapter adapter = new LocalizationAdapter(options);
            byte[] provisionContent = ProfileUtils.getProvisionContent(new File(adapter.getInFile()));
            byte[] p7b = ProfileSignTool.generateP7b(adapter, provisionContent);
            FileUtils.write(p7b, new File(adapter.getOutFile()));
            result = true;
        } catch (IOException exception) {
            logger.debug(exception.getMessage(), exception);
            logger.error(exception.getMessage());
            result = false;
        }
        return result;
    }

    /**
     * Verify profile.
     *
     * @param options options
     * @return Verify or not
     */
    @Override
    public boolean verifyProfile(Options options) {
        boolean result;
        try {
            LocalizationAdapter adapter = new LocalizationAdapter(options);
            VerifyHelper verifyHelper = new VerifyHelper();
            byte[] p7b = FileUtils.readFile(new File(adapter.getInFile()));
            VerificationResult verificationResult = verifyHelper.verify(p7b);
            result = verificationResult.isVerifiedPassed();
            if (!result) {
                logger.error(verificationResult.getMessage());
            }
            outputString(FileUtils.GSON_PRETTY_PRINT.toJson(verificationResult), adapter.getOutFile());
        } catch (IOException exception) {
            logger.debug(exception.getMessage(), exception);
            logger.error(exception.getMessage());
            result = false;
        }
        return result;
    }

    /**
     * Sign for hap.
     *
     * @param options options
     * @return Sign or not
     */
    @Override
    public boolean signHap(Options options) {
        String mode = options.getString(Options.MODE);
        // sign online or locally
        SignProvider signProvider;
        if ("localSign".equalsIgnoreCase(mode)) {
            signProvider = new LocalJKSSignProvider();
        } else if ("remoteSign".equalsIgnoreCase(mode)){
            signProvider = new RemoteSignProvider();
        } else {
            logger.info("Resign mode. But not implement yet");
            return false;
        }

        // The type of file is bin or hap
        String inForm = options.getString(Options.IN_FORM, "zip");
        if ("zip".equalsIgnoreCase(inForm)) {
            return signProvider.sign(options);
        } else {
            return signProvider.signBin(options);
        }
    }

    @Override
    public boolean verifyHap(Options options) {
        VerifyHap hapVerify = new VerifyHap();
        return hapVerify.verify(options);
    }

    /**
     * Output string, save into file or print.
     *
     * @param content String Content
     * @param file    file
     */
    public void outputString(String content, String file) {
        if (StringUtils.isEmpty(file)) {
            logger.info(content);
        } else {
            try {
                FileUtils.write(content.getBytes(StandardCharsets.UTF_8), new File(file));
            } catch (IOException exception) {
                logger.debug(exception.getMessage(), exception);
                CustomException.throwException(ERROR.WRITE_FILE_ERROR, exception.getMessage());
            }
        }
    }

    /**
     * Output certificate.
     *
     * @param certificate certificate
     * @param file        file
     * @return Whether to output certificate
     */
    public boolean outputCert(X509Certificate certificate, String file) {
        try {
            if (StringUtils.isEmpty(file)) {
                logger.info(CertUtils.generateCertificateInCer(certificate));
            } else {
                FileUtils.write(CertUtils.generateCertificateInCer(certificate).getBytes(StandardCharsets.UTF_8),
                        new File(file));
            }
            return true;
        } catch (CertificateException | IOException exception) {
            logger.debug(exception.getMessage(), exception);
            CustomException.throwException(ERROR.WRITE_FILE_ERROR, exception.getMessage());
            return false;
        }
    }

    /**
     * Output certificate in certificates list.
     *
     * @param certificates certificates
     * @param file         file
     * @return Whether to output certificate in certificates list
     */
    public boolean outputCertChain(List<X509Certificate> certificates, String file) {
        try {
            StringBuilder stringBuilder = new StringBuilder();
            for (X509Certificate cert : certificates) {
                stringBuilder.append(CertUtils.generateCertificateInCer(cert));
            }
            if (StringUtils.isEmpty(file)) {
                logger.info(stringBuilder.toString());
            } else {
                FileUtils.write(stringBuilder.toString().getBytes(StandardCharsets.UTF_8), new File(file));
            }
            return true;
        } catch (CertificateException | IOException exception) {
            logger.debug(exception.getMessage(), exception);
            CustomException.throwException(ERROR.WRITE_FILE_ERROR, exception.getMessage());
            return false;
        }
    }
}
