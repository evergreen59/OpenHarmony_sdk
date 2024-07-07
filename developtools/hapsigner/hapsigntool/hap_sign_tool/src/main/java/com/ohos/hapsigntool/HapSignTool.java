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

package com.ohos.hapsigntool;


import com.ohos.hapsigntool.api.ServiceApi;
import com.ohos.hapsigntool.api.SignToolServiceImpl;
import com.ohos.hapsigntool.api.model.Options;
import com.ohos.hapsigntool.error.CustomException;
import com.ohos.hapsigntool.error.ERROR;
import com.ohos.hapsigntool.utils.FileUtils;
import com.ohos.hapsigntool.utils.StringUtils;
import com.ohos.hapsigntoolcmd.CmdUtil;
import com.ohos.hapsigntoolcmd.CmdUtil.Method;
import com.ohos.hapsigntoolcmd.HelpDocument;
import com.ohos.hapsigntoolcmd.Params;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

/**
 * HapSignTool.
 *
 * @since 2021/12/28
 */
public final class HapSignTool {
    /**
     * Add log info.
     */
    private static final Logger LOGGER = LogManager.getLogger(HapSignTool.class);

    /**
     * Tool version.
     */
    private static final String VERSION = "1.0.0";

    /**
     * Local sign.
     */
    private static final String LOCAL_SIGN = "localSign";

    /**
     * Remote sign.
     */
    private static final String REMOTE_SIGN = "remoteSign";

    /**
     * Signed.
     */
    private static final String SIGNED = "1";

    /**
     * No signed.
     */
    private static final String NOT_SIGNED = "0";

    private HapSignTool() {
    }

    /**
     * Main entry.
     *
     * @param args arguments
     */
    public static void main(String[] args) {
        try {
            boolean result = processCmd(args);
            if (!result) {
                System.exit(1);
            }
        } catch (CustomException exception) {
            LOGGER.debug(exception.getMessage(), exception);
            LOGGER.error(exception.getMessage());
        }
    }

    /**
     * Process command.
     *
     * @param args arguments
     * @return command processing result
     * @throws CustomException custom exception for command execution failure
     */
    public static boolean processCmd(String[] args) throws CustomException {
        if (args.length == 0 || StringUtils.isEmpty(args[0])) {
            help();
        } else if ("-h".equals(args[0]) || "-help".equals(args[0])) {
            help();
        } else if ("-v".equals(args[0]) || "-version".equals(args[0])) {
            version();
        } else {
            ServiceApi api = new SignToolServiceImpl();
            Params params = CmdUtil.convert2Params(args);
            LOGGER.debug(params.toString());
            LOGGER.info("Start {}", params.getMethod());
            boolean result;
            result = dispatchParams(params, api);
            if (result) {
                LOGGER.info(String.format("%s %s", params.getMethod(), "success"));
            } else {
                LOGGER.info(String.format("%s %s", params.getMethod(), "failed"));
            }
            return result;
        }
        return true;
    }

    private static boolean callGenerators(Params params, ServiceApi api) {
        boolean result = false;
        switch (params.getMethod()) {
            case Method.GENERATE_APP_CERT:
                result = runAppCert(params.getOptions(), api);
                break;
            case Method.GENERATE_CA:
                result = runCa(params.getOptions(), api);
                break;
            case Method.GENERATE_CERT:
                result = runCert(params.getOptions(), api);
                break;
            case Method.GENERATE_CSR:
                result = runCsr(params.getOptions(), api);
                break;
            case Method.GENERATE_KEYPAIR:
                result = runKeypair(params.getOptions(), api);
                break;
            case Method.GENERATE_PROFILE_CERT:
                result = runProfileCert(params.getOptions(), api);
                break;
            default:
                CustomException.throwException(ERROR.COMMAND_ERROR, "Unsupported cmd");
                break;
        }
        return result;
    }

    private static boolean dispatchParams(Params params, ServiceApi api) {
        boolean result;
        switch (params.getMethod()) {
            case Method.SIGN_APP:
                result = runSignApp(params.getOptions(), api);
                break;
            case Method.SIGN_PROFILE:
                result = runSignProfile(params.getOptions(), api);
                break;
            case Method.VERIFY_APP:
                result = runVerifyApp(params.getOptions(), api);
                break;
            case Method.VERIFY_PROFILE:
                result = runVerifyProfile(params.getOptions(), api);
                break;
            default:
                result = callGenerators(params, api);
                break;
        }

        return result;
    }

    private static void checkEndCertArguments(Options params) {
        params.required(Options.KEY_ALIAS, Options.ISSUER, Options.ISSUER_KEY_ALIAS, Options.SUBJECT,
                Options.SIGN_ALG, Options.KEY_STORE_FILE);
        String signAlg = params.getString(Options.SIGN_ALG);
        CmdUtil.judgeEndSignAlgType(signAlg);
        String outForm = params.getString(Options.OUT_FORM);
        if (!StringUtils.isEmpty(outForm)) {
            CmdUtil.verifyType(outForm, Options.OUT_FORM_SCOPE);
        }
        if (StringUtils.isEmpty(outForm) || "certChain".equals(outForm)) {
            params.required(Options.SUB_CA_CERT_FILE, Options.CA_CERT_FILE);
            FileUtils.validFileType(params.getString(Options.SUB_CA_CERT_FILE), "cer");
            FileUtils.validFileType(params.getString(Options.CA_CERT_FILE), "cer");
        }
        String keyStoreFile = params.getString(Options.KEY_STORE_FILE);
        FileUtils.validFileType(keyStoreFile, "p12", "jks");

        if (params.containsKey(Options.ISSUER_KEY_STORE_FILE)){
            String issuerKeyStoreFile = params.getString(Options.ISSUER_KEY_STORE_FILE);
            FileUtils.validFileType(issuerKeyStoreFile, "p12", "jks");
        }

        String outFile = params.getString(Options.OUT_FILE);
        if (!StringUtils.isEmpty(outFile)) {
            FileUtils.validFileType(outFile, "cer", "pem");
        }
    }

    private static boolean runAppCert(Options params, ServiceApi api) {
        checkEndCertArguments(params);
        return api.generateAppCert(params);
    }

    private static boolean runCa(Options params, ServiceApi api) {
        params.required(Options.KEY_ALIAS, Options.KEY_ALG, Options.KEY_SIZE, Options.SUBJECT,
                Options.SIGN_ALG, Options.KEY_STORE_FILE);
        String keyAlg = params.getString(Options.KEY_ALG);
        CmdUtil.judgeAlgType(keyAlg);
        String size = params.getString(Options.KEY_SIZE);
        CmdUtil.judgeSize(size, keyAlg);
        String signAlg = params.getString(Options.SIGN_ALG);
        CmdUtil.judgeSignAlgType(signAlg);
        FileUtils.validFileType(params.getString(Options.KEY_STORE_FILE), "p12", "jks");
        params.put(Options.KEY_SIZE, CmdUtil.convertAlgSize(size));
        return api.generateCA(params);
    }

    private static boolean runCert(Options params, ServiceApi api) {
        params.required(Options.KEY_ALIAS, Options.ISSUER, Options.ISSUER_KEY_ALIAS, Options.SUBJECT,
                Options.KEY_USAGE, Options.SIGN_ALG, Options.KEY_STORE_FILE);
        String keyUsage = params.getString(Options.KEY_USAGE);
        CmdUtil.verifyType(keyUsage, Options.KEY_USAGE_SCOPE);
        String extKeyUsage = params.getString(Options.EXT_KEY_USAGE);
        CmdUtil.verifyType(extKeyUsage, Options.EXT_KEY_USAGE_SCOPE);
        String signAlg = params.getString(Options.SIGN_ALG);
        CmdUtil.judgeSignAlgType(signAlg);
        FileUtils.validFileType(params.getString(Options.KEY_STORE_FILE), "p12", "jks");
        if (params.containsKey(Options.ISSUER_KEY_STORE_FILE)){
            String issuerKeyStoreFile = params.getString(Options.ISSUER_KEY_STORE_FILE);
            FileUtils.validFileType(issuerKeyStoreFile, "p12", "jks");
        }
        return api.generateCert(params);
    }

    private static boolean runCsr(Options params, ServiceApi api) {
        params.required(Options.KEY_ALIAS, Options.SUBJECT, Options.SIGN_ALG, Options.KEY_STORE_FILE);
        String signAlg = params.getString(Options.SIGN_ALG);
        CmdUtil.judgeSignAlgType(signAlg);
        FileUtils.validFileType(params.getString(Options.KEY_STORE_FILE), "p12", "jks");
        if (!StringUtils.isEmpty(params.getString(Options.OUT_FILE))) {
            FileUtils.validFileType(params.getString(Options.OUT_FILE), "csr");
        }

        return api.generateCsr(params);
    }

    private static boolean runKeypair(Options params, ServiceApi api) {
        params.required(Options.KEY_ALIAS, Options.KEY_ALG, Options.KEY_SIZE, Options.KEY_STORE_FILE);
        String keyAlg = params.getString(Options.KEY_ALG);
        CmdUtil.judgeAlgType(keyAlg);
        String size = params.getString(Options.KEY_SIZE);
        CmdUtil.judgeSize(size, keyAlg);
        params.put(Options.KEY_SIZE, CmdUtil.convertAlgSize(size));
        FileUtils.validFileType(params.getString(Options.KEY_STORE_FILE), "p12", "jks");

        return api.generateKeyStore(params);
    }

    private static boolean runProfileCert(Options params, ServiceApi api) {
        checkEndCertArguments(params);
        return api.generateProfileCert(params);
    }

    private static boolean runSignApp(Options params, ServiceApi api) {
        params.required(Options.MODE, Options.IN_FILE, Options.OUT_FILE, Options.PROFILE_FILE, Options.SIGN_ALG);
        String mode = params.getString(Options.MODE);
        if (!LOCAL_SIGN.equalsIgnoreCase(mode)
                && !REMOTE_SIGN.equalsIgnoreCase(mode)
                && !"remoteResign".equalsIgnoreCase(mode)) {
            CustomException.throwException(ERROR.COMMAND_ERROR, "mode params is incorrect");
        }

        if (LOCAL_SIGN.equalsIgnoreCase(mode)) {
            params.required(Options.KEY_STORE_FILE, Options.KEY_ALIAS, Options.APP_CERT_FILE);
            FileUtils.validFileType(params.getString(Options.KEY_STORE_FILE), "p12", "jks");
        }
        checkProfile(params);
        String inForm = params.getString(Options.IN_FORM);
        if (!StringUtils.isEmpty(inForm) && !"zip".equalsIgnoreCase(inForm) && !"bin".equalsIgnoreCase(inForm)) {
            CustomException.throwException(ERROR.NOT_SUPPORT_ERROR, "inForm params is incorrect");
        }
        String signAlg = params.getString(Options.SIGN_ALG);
        CmdUtil.judgeEndSignAlgType(signAlg);

        return api.signHap(params);
    }

    private static void checkProfile(Options params) {
        String profileFile = params.getString(Options.PROFILE_FILE);
        String profileSigned = params.getString(Options.PROFILE_SIGNED,SIGNED);
        if (!SIGNED.equals(profileSigned) && !NOT_SIGNED.equals(profileSigned)) {
            CustomException.throwException(ERROR.NOT_SUPPORT_ERROR, "profileSigned params is incorrect");
        }
        if (SIGNED.equals(profileSigned)) {
            FileUtils.validFileType(profileFile, "p7b");
        } else {
            FileUtils.validFileType(profileFile, "json");
        }
    }

    private static boolean runSignProfile(Options params, ServiceApi api) {
        params.required(Options.MODE, Options.SIGN_ALG, Options.OUT_FILE, Options.IN_FILE);
        String mode = params.getString(Options.MODE);
        if (!LOCAL_SIGN.equalsIgnoreCase(mode) && !REMOTE_SIGN.equalsIgnoreCase(mode)) {
            CustomException.throwException(ERROR.COMMAND_ERROR, "mode params is incorrect");
        }
        if (LOCAL_SIGN.equalsIgnoreCase(mode)) {
            params.required(Options.KEY_STORE_FILE, Options.KEY_ALIAS, Options.PROFILE_CERT_FILE);
            FileUtils.validFileType(params.getString(Options.KEY_STORE_FILE), "p12", "jks");
        }

        String signAlg = params.getString(Options.SIGN_ALG);
        CmdUtil.judgeEndSignAlgType(signAlg);
        String outFile = params.getString(Options.OUT_FILE);
        FileUtils.validFileType(outFile, "p7b");

        return api.signProfile(params);
    }

    private static boolean runVerifyApp(Options params, ServiceApi api) {
        params.required(Options.IN_FILE, Options.OUT_CERT_CHAIN,
                Options.OUT_PROFILE);
        FileUtils.validFileType(params.getString(Options.IN_FILE), "hap", "bin");
        FileUtils.validFileType(params.getString(Options.OUT_CERT_CHAIN), "cer");
        FileUtils.validFileType(params.getString(Options.OUT_PROFILE), "p7b");
        return api.verifyHap(params);
    }

    private static boolean runVerifyProfile(Options params, ServiceApi api) {
        params.required(Options.IN_FILE);
        FileUtils.validFileType(params.getString(Options.IN_FILE), "p7b");
        String outFile = params.getString(Options.OUT_FILE);
        if (!StringUtils.isEmpty(outFile)) {
            FileUtils.validFileType(outFile, "json");
        }

        return api.verifyProfile(params);
    }

    /**
     * Software version.
     */
    public static void version() {
        LOGGER.info(VERSION);
    }

    /**
     * Print help to console.
     */
    public static void help() {
        HelpDocument.printHelp(LOGGER);
    }
}
