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

package com.ohos.hapsigntool.hap.provider;

import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParseException;
import com.google.gson.JsonParser;
import com.ohos.hapsigntool.api.model.Options;
import com.ohos.hapsigntool.error.CustomException;
import com.ohos.hapsigntool.hap.config.SignerConfig;
import com.ohos.hapsigntool.hap.entity.SigningBlock;
import com.ohos.hapsigntool.hap.exception.InvalidParamsException;
import com.ohos.hapsigntool.hap.exception.MissingParamsException;
import com.ohos.hapsigntool.hap.exception.ProfileException;
import com.ohos.hapsigntool.hap.exception.SignatureException;
import com.ohos.hapsigntool.hap.exception.VerifyCertificateChainException;
import com.ohos.hapsigntool.hap.exception.HapFormatException;
import com.ohos.hapsigntool.hap.sign.SignBin;
import com.ohos.hapsigntool.hap.sign.SignHap;
import com.ohos.hapsigntool.hap.sign.SignatureAlgorithm;
import com.ohos.hapsigntool.hap.verify.VerifyUtils;
import com.ohos.hapsigntool.utils.CertificateUtils;
import com.ohos.hapsigntool.utils.DigestUtils;
import com.ohos.hapsigntool.utils.EscapeCharacter;
import com.ohos.hapsigntool.utils.FileUtils;
import com.ohos.hapsigntool.utils.HapUtils;
import com.ohos.hapsigntool.utils.ParamConstants;
import com.ohos.hapsigntool.utils.ParamProcessUtil;
import com.ohos.hapsigntool.utils.StringUtils;
import com.ohos.hapsigntool.zip.ByteBufferZipDataInput;
import com.ohos.hapsigntool.zip.RandomAccessFileZipDataInput;
import com.ohos.hapsigntool.zip.RandomAccessFileZipDataOutput;
import com.ohos.hapsigntool.zip.ZipDataInput;
import com.ohos.hapsigntool.zip.ZipDataOutput;
import com.ohos.hapsigntool.zip.ZipFileInfo;
import com.ohos.hapsigntool.zip.ZipUtils;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.bouncycastle.cms.CMSException;
import org.bouncycastle.cms.CMSSignedData;
import org.bouncycastle.jce.provider.BouncyCastleProvider;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;
import java.security.InvalidKeyException;
import java.security.Security;
import java.security.cert.CertificateException;
import java.security.cert.X509CRL;
import java.security.cert.X509Certificate;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TimeZone;
import java.util.Optional;
import java.util.jar.JarFile;
import java.util.jar.JarOutputStream;

/**
 * Sign provider super class
 *
 * @since 2021-12-14
 */
public abstract class SignProvider {
    private static final Logger LOGGER = LogManager.getLogger(SignProvider.class);
    private static final List<String> VALID_SIGN_ALG_NAME = new ArrayList<String>();
    private static final List<String> PARAMETERS_NEED_ESCAPE = new ArrayList<String>();
    private static final long TIMESTAMP = 1230768000000L;
    private static final int COMPRESSION_MODE = 9;

    static {
        VALID_SIGN_ALG_NAME.add(ParamConstants.HAP_SIG_ALGORITHM_SHA256_ECDSA);
        VALID_SIGN_ALG_NAME.add(ParamConstants.HAP_SIG_ALGORITHM_SHA384_ECDSA);
        VALID_SIGN_ALG_NAME.add(ParamConstants.HAP_SIG_ALGORITHM_SHA512_ECDSA);
        VALID_SIGN_ALG_NAME.add(ParamConstants.HAP_SIG_ALGORITHM_SHA256_RSA_PSS);
        VALID_SIGN_ALG_NAME.add(ParamConstants.HAP_SIG_ALGORITHM_SHA384_RSA_PSS);
        VALID_SIGN_ALG_NAME.add(ParamConstants.HAP_SIG_ALGORITHM_SHA512_RSA_PSS);
        VALID_SIGN_ALG_NAME.add(ParamConstants.HAP_SIG_ALGORITHM_SHA256_RSA_MGF1);
        VALID_SIGN_ALG_NAME.add(ParamConstants.HAP_SIG_ALGORITHM_SHA384_RSA_MGF1);
        VALID_SIGN_ALG_NAME.add(ParamConstants.HAP_SIG_ALGORITHM_SHA512_RSA_MGF1);
    }

    static {
        PARAMETERS_NEED_ESCAPE.add(ParamConstants.PARAM_REMOTE_CODE);
        PARAMETERS_NEED_ESCAPE.add(ParamConstants.PARAM_LOCAL_JKS_KEYSTORE_CODE);
        PARAMETERS_NEED_ESCAPE.add(ParamConstants.PARAM_LOCAL_JKS_KEYALIAS_CODE);
    }

    /**
     * list of hap signature optional blocks
     */
    protected List<SigningBlock> optionalBlocks = new ArrayList<SigningBlock>();

    /**
     * parameters only used in signing
     */
    protected Map<String, String> signParams = new HashMap<String, String>();

    /**
     * Read data of optional blocks from file user inputted.
     *
     * @throws InvalidParamsException Exception occurs when the input is invalid.
     */
    protected void loadOptionalBlocks() throws InvalidParamsException {
        String property = signParams.get(ParamConstants.PARAM_BASIC_PROPERTY);
        loadOptionalBlock(property, HapUtils.HAP_PROPERTY_BLOCK_ID);

        String profile = signParams.get(ParamConstants.PARAM_BASIC_PROFILE);
        loadOptionalBlock(profile, HapUtils.HAP_PROFILE_BLOCK_ID);

        String proofOfRotation = signParams.get(ParamConstants.PARAM_BASIC_PROOF);
        loadOptionalBlock(proofOfRotation, HapUtils.HAP_PROOF_OF_ROTATION_BLOCK_ID);
    }

    private void loadOptionalBlock(String file, int type) throws InvalidParamsException {
        if (!checkStringIsNotNullAndEmity(file)) {
            return;
        }
        if (!checkFile(file)) {
            LOGGER.error("check file failed");
            throw new InvalidParamsException("Invalid file: " + file + ", filetype: " + type);
        }
        try {
            byte[] optionalBlockBytes = HapUtils.readFileToByte(file);
            if (optionalBlockBytes == null || optionalBlockBytes.length <= 0) {
                LOGGER.warn("Optional block is null!");
                return;
            }
            optionalBlocks.add(new SigningBlock(type, optionalBlockBytes));
        } catch (IOException e) {
            LOGGER.error("read file error", e);
            throw new InvalidParamsException("Invalid file: " + file + " is not readable. filetype: " + type);
        }
    }

    /**
     * check if the input path is a file
     *
     * @param filePath input file path
     * @return true, if path is a file and can be read
     */
    private boolean checkFile(String filePath) {
        if (!(checkStringIsNotNullAndEmity(filePath))) {
            LOGGER.error("fileName is null");
            return false;
        }
        File file = new File(filePath);
        if (!file.canRead() || !file.isFile()) {
            LOGGER.error(filePath + " not exist or can not read!");
            return false;
        }
        return true;
    }

    private boolean checkStringIsNotNullAndEmity(String str) {
        return !(str == null || "".equals(str));
    }

    /**
     * Get certificate chain used to sign.
     *
     * @return list of x509 certificates.
     */
    private List<X509Certificate> getPublicCerts() {
        String publicCertsFile = signParams.get(ParamConstants.PARAM_LOCAL_PUBLIC_CERT);
        if (StringUtils.isEmpty(publicCertsFile)) {
            return Collections.emptyList();
        }
        return getCertificateChainFromFile(publicCertsFile);
    }

    /**
     * get certificate revocation list used to sign
     *
     * @return certificate revocation list
     */
    public Optional<X509CRL> getCrl() {
        return Optional.empty();
    }

    /**
     * Create SignerConfig by certificate chain and certificate revocation list.
     *
     * @param certificates certificate chain
     * @param crl certificate revocation list
     * @param options options
     * @return Object of SignerConfig
     * @throws InvalidKeyException on error when the key is invalid.
     */
    public SignerConfig createSignerConfigs(List<X509Certificate> certificates, Optional<X509CRL> crl, Options options)
            throws InvalidKeyException {
        SignerConfig signerConfig = new SignerConfig();
        signerConfig.fillParameters(this.signParams);
        signerConfig.setCertificates(certificates);
        signerConfig.setOptions(options);

        List<SignatureAlgorithm> signatureAlgorithms = new ArrayList<SignatureAlgorithm>();
        signatureAlgorithms.add(
            ParamProcessUtil.getSignatureAlgorithm(this.signParams.get(ParamConstants.PARAM_BASIC_SIGANTURE_ALG)));
        signerConfig.setSignatureAlgorithms(signatureAlgorithms);

        if (!crl.equals(Optional.empty())) {
            signerConfig.setX509CRLs(Collections.singletonList(crl.get()));
        }
        return signerConfig;
    }

    /**
     * sign bin file
     *
     * @param options parameters used to sign bin file
     * @return true, if sign successfully.
     */
    public boolean signBin(Options options) {
        Security.addProvider(new BouncyCastleProvider());
        List<X509Certificate> publicCert = null;
        SignerConfig signerConfig;
        try {
            publicCert = getX509Certificates(options);

            // Get x509 CRL
            Optional<X509CRL> crl = getCrl();

            // Create signer configs, which contains public cert and crl info.
            signerConfig = createSignerConfigs(publicCert, crl, options);
        } catch (InvalidKeyException | InvalidParamsException | MissingParamsException | ProfileException e) {
            LOGGER.error("create signer configs failed.", e);
            printErrorLogWithoutStack(e);
            return false;
        }

        /* 6. make signed file into output file. */
        if (!SignBin.sign(signerConfig, signParams)) {
            LOGGER.error("hap-sign-tool: error: Sign bin internal failed.");
            return false;
        }
        LOGGER.info("Sign success");
        return true;
    }

    /**
     * sign hap file
     *
     * @param options parameters used to sign hap file
     * @return true, if sign successfully
     */
    public boolean sign(Options options) {
        Security.addProvider(new BouncyCastleProvider());
        List<X509Certificate> publicCerts = null;
        File output = null;
        File tmpOutput = null;
        boolean isRet = false;
        boolean isPathOverlap = false;
        try {
            publicCerts = getX509Certificates(options);
            checkCompatibleVersion();
            File input = new File(signParams.get(ParamConstants.PARAM_BASIC_INPUT_FILE));
            output = new File(signParams.get(ParamConstants.PARAM_BASIC_OUTPUT_FILE));
            if (input.getCanonicalPath().equals(output.getCanonicalPath())) {
                tmpOutput = File.createTempFile("signedHap", ".hap");
                isPathOverlap = true;
            } else {
                tmpOutput = output;
            }
            // copy file and Alignment
            int alignment = Integer.parseInt(signParams.get(ParamConstants.PARAM_BASIC_ALIGNMENT));
            copyFileAndAlignment(input, tmpOutput, alignment);
            // generate sign block and output signedHap
            try (RandomAccessFile outputHap = new RandomAccessFile(tmpOutput, "rw")) {
                ZipDataInput outputHapIn = new RandomAccessFileZipDataInput(outputHap);
                ZipFileInfo zipInfo = ZipUtils.findZipInfo(outputHapIn);
                long centralDirectoryOffset = zipInfo.getCentralDirectoryOffset();
                ZipDataInput beforeCentralDir = outputHapIn.slice(0, centralDirectoryOffset);
                ByteBuffer centralDirBuffer =
                    outputHapIn.createByteBuffer(centralDirectoryOffset, zipInfo.getCentralDirectorySize());
                ZipDataInput centralDirectory = new ByteBufferZipDataInput(centralDirBuffer);

                ByteBuffer eocdBuffer = zipInfo.getEocd();
                ZipDataInput eocd = new ByteBufferZipDataInput(eocdBuffer);

                Optional<X509CRL> crl = getCrl();
                SignerConfig signerConfig = createSignerConfigs(publicCerts, crl, options);
                signerConfig.setCompatibleVersion(Integer.parseInt(
                        signParams.get(ParamConstants.PARAM_BASIC_COMPATIBLE_VERSION)));
                ZipDataInput[] contents = {beforeCentralDir, centralDirectory, eocd};
                byte[] signingBlock = SignHap.sign(contents, signerConfig, optionalBlocks);
                long newCentralDirectoryOffset = centralDirectoryOffset + signingBlock.length;
                ZipUtils.setCentralDirectoryOffset(eocdBuffer, newCentralDirectoryOffset);
                LOGGER.info("Generate signing block success, begin write it to output file");

                outputSignedFile(outputHap, centralDirectoryOffset, signingBlock, centralDirectory, eocdBuffer);
                isRet = true;
            }
        } catch (IOException | InvalidKeyException | HapFormatException | MissingParamsException
            | InvalidParamsException | ProfileException | NumberFormatException | CustomException e) {
            printErrorLogWithoutStack(e);
        } catch (SignatureException e) {
            printErrorLog(e);
        }
        return doAfterSign(isRet, isPathOverlap, tmpOutput, output);
    }

    /**
     * Load certificate chain from input parameters
     *
     * @param options parameters used to sign hap file
     * @return list of type x509certificate
     * @throws MissingParamsException Exception occurs when the required parameters are not entered.
     * @throws InvalidParamsException Exception occurs when the required parameters are invalid.
     * @throws ProfileException Exception occurs when profile is invalid.
     */
    private List<X509Certificate> getX509Certificates(Options options) throws MissingParamsException,
            InvalidParamsException, ProfileException {
        List<X509Certificate> publicCerts;
        // 1. check the parameters
        checkParams(options);
        // 2. get x509 verify certificate
        publicCerts = getPublicCerts();
        // 3. load optionalBlocks
        loadOptionalBlocks();
        checkProfileValid(publicCerts);
        return publicCerts;
    }

    private void outputSignedFile(RandomAccessFile outputHap, long centralDirectoryOffset,
        byte[] signingBlock, ZipDataInput centralDirectory, ByteBuffer eocdBuffer) throws IOException {
        ZipDataOutput outputHapOut = new RandomAccessFileZipDataOutput(outputHap, centralDirectoryOffset);
        outputHapOut.write(signingBlock, 0, signingBlock.length);
        centralDirectory.copyTo(0, centralDirectory.size(), outputHapOut);
        outputHapOut.write(eocdBuffer);
    }

    private boolean doAfterSign(boolean isSuccess, boolean pathOverlap, File tmpOutput, File output) {
        boolean isRet = isSuccess;
        if (isRet && pathOverlap) {
            try {
                Files.move(tmpOutput.toPath(), output.toPath(), StandardCopyOption.REPLACE_EXISTING);
            } catch (IOException e) {
                printErrorLog(e);
                isRet = false;
            }
        }

        if (isRet) {
            LOGGER.info("Sign Hap success!");
        } else {
            FileUtils.deleteFile(tmpOutput);
        }
        return isRet;
    }

    private void printErrorLog(Exception exception) {
        if (exception != null) {
            LOGGER.error("hap-sign-tool: error: {}", exception.getMessage(), exception);
        }
    }

    private void printErrorLogWithoutStack(Exception exception) {
        if (exception != null) {
            LOGGER.error("hap-sign-tool: error: {}", exception.getMessage());
        }
    }

    /**
     * Copy file and alignment
     *
     * @param input file input
     * @param tmpOutput file tmpOutput
     * @param alignment alignment
     * @throws IOException  io error
     */
    private void copyFileAndAlignment(File input, File tmpOutput, int alignment) throws IOException {
        try (JarFile inputJar = new JarFile(input, false);
            FileOutputStream outputFile = new FileOutputStream(tmpOutput);
            JarOutputStream outputJar = new JarOutputStream(outputFile)) {
            long timestamp = TIMESTAMP;
            timestamp -= TimeZone.getDefault().getOffset(timestamp);
            outputJar.setLevel(COMPRESSION_MODE);
            List<String> entryNames = SignHap.getEntryNamesFromHap(inputJar);
            SignHap.copyFiles(entryNames, inputJar, outputJar, timestamp, alignment);
        }
    }

    /**
     * check signature algorithm
     *
     * @throws InvalidParamsException Exception occurs when the inputted sign algorithm is invalid.
     */
    private void checkSignatureAlg() throws InvalidParamsException {
        String signAlg = signParams.get(ParamConstants.PARAM_BASIC_SIGANTURE_ALG).trim();
        for (String validAlg : VALID_SIGN_ALG_NAME) {
            if (validAlg.equalsIgnoreCase(signAlg)) {
                return;
            }
        }
        LOGGER.error("Unsupported signature algorithm :" + signAlg);
        throw new InvalidParamsException("Invalid parameter: Sign Alg");
    }

    /**
     * check alignment
     */
    protected void checkSignAlignment() {
        if (!signParams.containsKey(ParamConstants.PARAM_BASIC_ALIGNMENT)) {
            signParams.put(ParamConstants.PARAM_BASIC_ALIGNMENT, ParamConstants.ALIGNMENT);
        }
    }

    /**
     * Get CN value of developer certificate from profile.
     *
     * @param buildInfoObject json obect of buildInfo in profile.
     * @return Object of development-certificate.
     */
    private X509Certificate getDevelopmentCertificate(JsonObject buildInfoObject) {
        final String developmentCertElememt = "development-certificate";
        String developmentCertificate = buildInfoObject.get(developmentCertElememt).getAsString();
        return DigestUtils.decodeBase64ToX509Certifate(developmentCertificate);
    }

    /**
     * Get CN value of release certificate from profile.
     *
     * @param buildInfoObject json obect of buildInfo in profile.
     * @return Object of distribution-certificate.
     */
    private X509Certificate getReleaseCertificate(JsonObject buildInfoObject) {
        final String distributeCertElememt = "distribution-certificate";
        String distributeCertificate = buildInfoObject.get(distributeCertElememt).getAsString();
        return DigestUtils.decodeBase64ToX509Certifate(distributeCertificate);
    }

    private String getCertificateCN(X509Certificate cert) {
        if (cert == null) {
            return "";
        }
        String valueOfDN = cert.getSubjectDN().toString();
        valueOfDN = valueOfDN.replace("\"", "");
        String[] arrayDN = valueOfDN.split(",");
        for (String element : arrayDN) {
            if (element.trim().startsWith("CN=")) {
                return element.split("=")[1];
            }
        }
        return "";
    }

    private byte[] findProfileFromOptionalBlocks() {
        byte[] profile = new byte[0];
        for (SigningBlock optionalBlock : optionalBlocks) {
            if (optionalBlock.getType() == HapUtils.HAP_PROFILE_BLOCK_ID) {
                profile = optionalBlock.getValue();
            }
        }
        return profile;
    }

    /**
     * Check profile is valid. A valid profile must include type and
     * certificate which has a non-empty value of DN.
     *
     * @param inputCerts certificates inputted by user.
     * @throws ProfileException Exception occurs when profile is invalid.
     */
    private void checkProfileValid(List<X509Certificate> inputCerts) throws ProfileException {
        try {
            byte[] profile = findProfileFromOptionalBlocks();
            boolean isProfileWithoutSign = ParamConstants.ProfileSignFlag.UNSIGNED_PROFILE.getSignFlag().equals(
                    signParams.get(ParamConstants.PARAM_BASIC_PROFILE_SIGNED));
            String content;
            if (!isProfileWithoutSign) {
                CMSSignedData cmsSignedData = new CMSSignedData(profile);
                boolean isVerify = VerifyUtils.verifyCmsSignedData(cmsSignedData);
                if (!isVerify) {
                    throw new ProfileException("Verify profile pkcs7 failed! Profile is invalid.");
                }
                Object contentObj = cmsSignedData.getSignedContent().getContent();
                if (!(contentObj instanceof byte[])) {
                    throw new ProfileException("Check profile failed, signed profile content is not byte array!");
                }
                content = new String((byte[]) contentObj, StandardCharsets.UTF_8);
            } else {
                content = new String(profile, StandardCharsets.UTF_8);
            }
            JsonElement parser = JsonParser.parseString(content);
            JsonObject profileJson = parser.getAsJsonObject();
            checkProfileInfo(profileJson, inputCerts);
        } catch (CMSException e) {
            throw new ProfileException("Verify profile pkcs7 failed! Profile is invalid.", e);
        } catch (JsonParseException e) {
            throw new ProfileException("Invalid parameter: profile content is not a JSON.", e);
        }
    }

    private void checkProfileInfo(JsonObject profileJson, List<X509Certificate> inputCerts) throws ProfileException {
        String profileTypeKey = "type";
        String profileType = profileJson.get(profileTypeKey).getAsString();
        if (profileType == null || profileType.length() == 0) {
            throw new ProfileException("Get profile type error!");
        }
        String buildInfoMember = "bundle-info";
        JsonObject buildInfoObject = profileJson.getAsJsonObject(buildInfoMember);
        X509Certificate certInProfile;
        if (profileType.equalsIgnoreCase("release")) {
            certInProfile = getReleaseCertificate(buildInfoObject);
        } else if (profileType.equalsIgnoreCase("debug")) {
            certInProfile = getDevelopmentCertificate(buildInfoObject);
        } else {
            throw new ProfileException("Unsupported profile type!");
        }
        if (!inputCerts.isEmpty() && !checkInputCertMatchWithProfile(inputCerts.get(0), certInProfile)) {
                throw new ProfileException("input certificates do not match with profile!");
        }
        String cn = getCertificateCN(certInProfile);
        LOGGER.info("certificate in profile: {}", cn);
        if (cn.isEmpty()) {
            throw new ProfileException("Common name of certificate is empty!");
        }
    }

    /**
     * check whether certificate inputted by user is matched with the certificate in profile.
     *
     * @param inputCert certificates inputted by user.
     * @param certInProfile the certificate in profile.
     * @return true, if it is match.
     */
    protected boolean checkInputCertMatchWithProfile(X509Certificate inputCert, X509Certificate certInProfile) {
        return true;
    }

    /**
     * Check input parameters is valid. And put valid parameters into signParams.
     *
     * @param options parameters inputted by user.
     * @throws MissingParamsException Exception occurs when the required parameters are not entered.
     * @throws InvalidParamsException Exception occurs when the required parameters are invalid.
     */
    public void checkParams(Options options) throws MissingParamsException, InvalidParamsException {
        String[] paramFileds = {
            ParamConstants.PARAM_BASIC_ALIGNMENT,
            ParamConstants.PARAM_BASIC_SIGANTURE_ALG,
            ParamConstants.PARAM_BASIC_INPUT_FILE,
            ParamConstants.PARAM_BASIC_OUTPUT_FILE,
            ParamConstants.PARAM_BASIC_PRIVATE_KEY,
            ParamConstants.PARAM_BASIC_PROFILE,
            ParamConstants.PARAM_BASIC_PROOF,
            ParamConstants.PARAM_BASIC_PROPERTY,
            ParamConstants.PARAM_REMOTE_SERVER,
            ParamConstants.PARAM_BASIC_PROFILE_SIGNED,
            ParamConstants.PARAM_LOCAL_PUBLIC_CERT,
            ParamConstants.PARAM_BASIC_COMPATIBLE_VERSION
        };
        Set<String> paramSet = ParamProcessUtil.initParamField(paramFileds);

        for (String paramKey : options.keySet()) {
            if (paramSet.contains(paramKey)) {
                signParams.put(paramKey, getParamValue(paramKey, options.getString(paramKey)));
            }
        }
        if (!signParams.containsKey(ParamConstants.PARAM_BASIC_PROFILE_SIGNED)) {
            signParams.put(ParamConstants.PARAM_BASIC_PROFILE_SIGNED, "1");
        }
        checkSignatureAlg();
        checkSignAlignment();
    }

    /**
     * Check compatible version, if param do not have compatible version default 9.
     *
     * @throws InvalidParamsException invalid param
     * @throws MissingParamsException missing param
     */
    protected void checkCompatibleVersion() throws InvalidParamsException, MissingParamsException {
        if (!signParams.containsKey(ParamConstants.PARAM_BASIC_COMPATIBLE_VERSION)) {
            signParams.put(ParamConstants.PARAM_BASIC_COMPATIBLE_VERSION, "9");
            return;
        }
        String compatibleApiVersionVal = signParams.get(ParamConstants.PARAM_BASIC_COMPATIBLE_VERSION);
        try {
            int compatibleApiVersion = Integer.parseInt(compatibleApiVersionVal);
        } catch (NumberFormatException e) {
            throw new InvalidParamsException("Invalid parameter: " + ParamConstants.PARAM_BASIC_COMPATIBLE_VERSION);
        }
    }

    /**
     * Get parameters from inputted strings. This function unescape some escaped parameters and return it.
     *
     * @param paramName the name of parameter
     * @param paramValue the value of parameter
     * @return parameter value in the correct form.
     */
    protected String getParamValue(String paramName, String paramValue) {
        for (String name : PARAMETERS_NEED_ESCAPE) {
            if (name.equals(paramName)) {
                return EscapeCharacter.unescape(paramValue);
            }
        }
        return paramValue;
    }

    private List<X509Certificate> getCertificateChainFromFile(String certChianFile) {
        try {
            return CertificateUtils.getCertListFromFile(certChianFile);
        } catch (CertificateException e) {
            LOGGER.error("File content is not certificates! " + e.getMessage());
        } catch (IOException e) {
            LOGGER.error("Certificate file exception: " + e.getMessage());
        } catch (VerifyCertificateChainException e) {
            LOGGER.error(e.getMessage());
        }
        return Collections.emptyList();
    }
}
