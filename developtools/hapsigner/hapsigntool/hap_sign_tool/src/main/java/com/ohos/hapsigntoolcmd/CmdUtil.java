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

package com.ohos.hapsigntoolcmd;

import com.ohos.hapsigntool.error.CustomException;
import com.ohos.hapsigntool.error.ERROR;
import com.ohos.hapsigntool.utils.StringUtils;
import com.ohos.hapsigntool.utils.ValidateUtils;

import java.util.Arrays;
import java.util.List;
import java.util.Locale;
import java.util.regex.Pattern;

/**
 * CmdUtil.
 *
 * @since 2021/12/28
 */
public final class CmdUtil {
    /**
     * Minimum length of input args.
     */
    private static final int ARGS_MIN_LEN = 2;

    /**
     * Match size String.
     */
    private static final Pattern INTEGER_PATTERN = Pattern.compile("\\d{1,10}");

    private CmdUtil() {
    }

    /**
     * Analyze and convert args to Params object.
     *
     * @param args Command line args
     * @return Params
     */
    public static Params convert2Params(String[] args) {
        ValidateUtils.throwIfNotMatches(args.length >= ARGS_MIN_LEN, ERROR.COMMAND_ERROR, "");

        Params params = new Params();
        params.setMethod(args[0]);
        String keyStandBy = null;
        List<String> trustList = ParamsTrustlist.getTrustList(args[0]);
        if (trustList.size() == 0) {
            CustomException.throwException(ERROR.COMMAND_ERROR, "Unsupported cmd");
        }
        for (int i = 1; i < args.length; i++) {
            String value = args[i];
            // prepare key
            if (value != null && (value.startsWith("-"))) {
                boolean isTrust = trustList.contains(value);
                ValidateUtils.throwIfNotMatches(isTrust,
                        ERROR.COMMAND_PARAM_ERROR, "Not support command param:" + value);
                keyStandBy = value.substring(1);
            } else {
                // prepare value
                boolean success = validAndPutParam(params, keyStandBy, value);
                if (success) {
                    keyStandBy = null;
                }
            }
        }
        return params;
    }

    private static boolean validAndPutParam(Params params, String key, String value) {
        boolean result;
        if (StringUtils.isEmpty(key)) {
            result = false;
        } else if (StringUtils.isEmpty(value)) {
            CustomException.throwException(ERROR.COMMAND_ERROR,
                    String.format("Command -%s could not be empty", key));
            result = false;
        } else if (params.getOptions().containsKey(key)) {
            CustomException.throwException(ERROR.COMMAND_ERROR,
                    String.format("Duplicate param '%s'. Stop processing", key));
            result = false;
        } else if (key.toLowerCase(Locale.ROOT).endsWith("pwd")) {
            params.getOptions().put(key, value.toCharArray());
            result = true;
        } else {
            params.getOptions().put(key, value);
            result = true;
        }
        return result;
    }

    /**
     * Alg type must between RSA and ECC.
     *
     * @param alg Incoming string
     */
    public static void judgeAlgType(String alg) {
        if (!"RSA".equalsIgnoreCase(alg) && !"ECC".equalsIgnoreCase(alg)) {
            CustomException.throwException(ERROR.COMMAND_ERROR,
                    "KeyAlg params is incorrect");
        }
    }

    /**
     * Check whether the algorithm size is within specified scope.
     *
     * @param size algorithm size
     * @param alg algorithm
     */
    public static void judgeSize(String size, String alg) {
        String[] array = {"2048", "3072", "4096", "NIST-P-256", "NIST-P-384"};
        List<String> arrayList = Arrays.asList(array);
        if (!arrayList.contains(size)) {
            CustomException.throwException(ERROR.COMMAND_ERROR, String.format("KeySize '%s' is incorrect", size));
        }

        if ("RSA".equalsIgnoreCase(alg)) {
            if (!"2048".equals(size) && !"3072".equals(size) && !"4096".equals(size)) {
                CustomException.throwException(ERROR.COMMAND_ERROR,
                        String.format("KeySize of '%s' is incorrect", alg));
            }
        } else {
            if (!"NIST-P-256".equalsIgnoreCase(size) && !"NIST-P-384".equalsIgnoreCase(size)) {
                CustomException.throwException(ERROR.COMMAND_ERROR,
                        String.format("KeySize of '%s' is incorrect", alg));
            }
        }
    }

    /**
     * Check whether the signature algorithm is within specified scope.
     *
     * @param signAlg signature algorithm
     */
    public static void judgeSignAlgType(String signAlg) {
        List<String> arrayList = Arrays.asList("SHA256withRSA", "SHA384withRSA", "SHA256withECDSA",
                "SHA384withECDSA");
        if (!arrayList.contains(signAlg)) {
            CustomException.throwException(ERROR.COMMAND_ERROR,
                    "SignAlg params is incorrect");
        }
    }

    /**
     * Check whether the signature algorithm is within specified scope.
     *
     * @param signAlg signature algorithm
     */
    public static void judgeEndSignAlgType(String signAlg) {
        List<String> arrayList = Arrays.asList("SHA256withECDSA", "SHA384withECDSA");
        if (!arrayList.contains(signAlg)) {
            CustomException.throwException(ERROR.NOT_SUPPORT_ERROR,
                    "SignAlg params is incorrect, signature algorithms include SHA256withECDSA,SHA384withECDSA");
        }
    }

    /**
     * Verify target type.
     *
     * @param inputType Types with ','
     * @param supportTypes Target types with ','
     */
    public static void verifyType(String inputType, String supportTypes) {
        String[] types = inputType.split(",");
        List<String> supportList = Arrays.asList(supportTypes.split(","));
        for (String type : types) {
            if (StringUtils.isEmpty(type)) {
                continue;
            }
            if (!supportList.contains(type.trim())) {
                CustomException.throwException(ERROR.COMMAND_ERROR,
                        "'" + type + "' in params '" + inputType + "' is not support");
            }
        }
    }

    /**
     * Convert passed in args to 'integer'.
     *
     * @param size String passed in
     * @return 'integer' String
     */
    public static String convertAlgSize(String size) {
        if (size.startsWith("NIST-P-")) {
            return size.replace("NIST-P-", "");
        } else if (INTEGER_PATTERN.matcher(size).matches()) {
            return size;
        } else {
            CustomException.throwException(ERROR.COMMAND_ERROR,
                    String.format("KeySize '%s' is incorrect", size));
            return size;
        }
    }

    /**
     * Command parameter constant
     *
     */
    public static final class Method {
        /**
         * Generate app cert method name.
         */
        public static final String GENERATE_APP_CERT = "generate-app-cert";
        /**
         * Generate ca method name.
         */
        public static final String GENERATE_CA = "generate-ca";
        /**
         * Generate cert method name.
         */
        public static final String GENERATE_CERT = "generate-cert";
        /**
         * Generate csr method name.
         */
        public static final String GENERATE_CSR = "generate-csr";
        /**
         * Generate key pair method name.
         */
        public static final String GENERATE_KEYPAIR = "generate-keypair";
        /**
         * Generate profile cert method name.
         */
        public static final String GENERATE_PROFILE_CERT = "generate-profile-cert";
        /**
         * Sign app method name.
         */
        public static final String SIGN_APP = "sign-app";
        /**
         * Sign profile method name.
         */
        public static final String SIGN_PROFILE = "sign-profile";
        /**
         * Verify app method name.
         */
        public static final String VERIFY_APP = "verify-app";
        /**
         * Verify profile method name.
         */
        public static final String VERIFY_PROFILE = "verify-profile";

        /**
         * Constructor of Method.
         */
        private Method() {
            // Empty constructor of Method.
        }
    }
}
