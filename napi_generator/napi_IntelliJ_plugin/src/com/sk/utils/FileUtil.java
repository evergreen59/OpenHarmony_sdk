/*
 * Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.sk.utils;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.intellij.notification.NotificationType;
import com.intellij.openapi.diagnostic.Logger;
import com.intellij.openapi.project.Project;
import net.minidev.json.JSONObject;
import net.minidev.json.parser.JSONParser;
import net.minidev.json.parser.ParseException;
import org.apache.http.util.TextUtils;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;
import java.util.Properties;
import java.util.regex.Pattern;

/**
 * 文本文件工具
 *
 * @author: xudong
 * @see: file utils
 * @version: v1.0.0
 * @since 2022-02-21
 */
public class FileUtil {
    private static final Logger LOG = Logger.getInstance(FileUtil.class);

    private static final int COMPILE_SDK_VERSION = 5;

    private static final String LF = getNewline(); // 换行符

    private static final String BUILD_OPTION = "{" + LF
                    + "    \"externalNativeOptions\": {" + LF
                    + "      \"path\": \"\"," + LF
                    + "      \"arguments\": \"-v\"," + LF
                    + "      \"abiFilters\": [" + LF
                    + "        \"armeabi-v7a\"," + LF
                    + "        \"arm64-v8a\"" + LF
                    + "      ]," + LF
                    + "      \"cppFlags\": \"\"," + LF
                    + "    }" + LF
                    + "  }";

    /**
     * 改写build-profile.json5文件
     *
     * @param buildJsonFilePath build-profile.json5 文件路径
     * @param cmakeFilePath CMakeList.txt 文件路径
     */
    public void writeBuildJsonFile(String buildJsonFilePath, String cmakeFilePath) {
        try {
            String buildStr = readWholeFile(buildJsonFilePath);
            JSONParser jsParser = new JSONParser(JSONParser.DEFAULT_PERMISSIVE_MODE);
            JSONObject buildObj = (JSONObject) jsParser.parse(buildStr);
            JSONObject buildOptionObj = (JSONObject) jsParser.parse(BUILD_OPTION);
            ((JSONObject) buildOptionObj.get("externalNativeOptions")).put("path", cmakeFilePath);
            buildObj.put("buildOption", buildOptionObj);
            ObjectMapper mapper = new ObjectMapper();
            buildStr = mapper.writerWithDefaultPrettyPrinter().writeValueAsString(buildObj);

            writeContentToFile(buildJsonFilePath, buildStr);
        } catch (ParseException parseException) {
            LOG.error("Failed to parse file [" + buildJsonFilePath + "], error: " + parseException);
        } catch (JsonProcessingException jsonProcessingEx) {
            LOG.error("Failed to write file [" + buildJsonFilePath + "], error: " + jsonProcessingEx);
        }
    }

    /**
     * 将数据写入到指定文件中
     *
     * @param path    文件路径
     * @param content 数据内容
     */
    public void writeContentToFile(String path, String content) {
        File file = new File(path);
        try (FileWriter fw = new FileWriter(file, false)) {
            fw.write(content + FileUtil.getNewline());
        } catch (IOException ioException) {
            LOG.error("Failed to write file [" + path + "], error: " + ioException);
        }
    }

    /**
     * 创建文件
     *
     * @param path 文件路径
     * @return 文件路径
     */
    public String makeFile(String path) {
        File file = new File(path);
        if (!file.exists()) {
            try {
                boolean isCreateFile = file.createNewFile();
                if (isCreateFile) {
                    LOG.info(String.format("makeFile %s success", path));
                }
            } catch (IOException ioException) {
                LOG.error(String.format("makeFile %s error:%s", path, ioException));
                return "";
            }
        }
        return file.getPath();
    }

    /**
     * 将错误信息输入到txt中
     *
     * @param path    路径
     * @param content 内容
     */
    public void writeErrorToTxt(String path, String content) {
        File file = new File(path);
        try (FileWriter fw = new FileWriter(file, true)) {
            fw.write(content + FileUtil.getNewline());
        } catch (IOException ioException) {
            LOG.error("writeErrorToTxt io error" + ioException);
        }
    }

    /**
     * 判断文件是否包含指定字符串
     *
     * @param path    文件路径
     * @param content 指定内容
     * @return 是否包含指定字符串
     * @throws IOException 异常信息
     */
    public boolean findStringInFile(String path, String content) throws IOException {
        File file = new File(path);
        String[] command = content.split(FileUtil.getNewline());

        try (InputStreamReader read = new InputStreamReader(new FileInputStream(file), StandardCharsets.UTF_8);
            BufferedReader bufferedReader = new BufferedReader(read)) {
            return isContainString(bufferedReader, command[1]);
        } catch (FileNotFoundException foundException) {
            LOG.error("file not found" + foundException);
            return false;
        }
    }

    private boolean isContainString(BufferedReader bufferedReader, String command) {
        String line = null;
        while (true) {
            try {
                if ((line = bufferedReader.readLine()) == null) {
                    return false;
                }
            } catch (IOException ioException) {
                LOG.error("findStringInFile IOException" + ioException);
            }

            if (line.contains(command)) {
                return true;
            }
        }
    }

    /**
     * 获取换行符
     *
     * @return 换行符
     */
    public static String getNewline() {
        return System.getProperty("line.separator");
    }

    /**
     * 正则匹配所选文件名是否符合规范
     *
     * @param fileName 文件名
     * @return boolean 是否匹配
     */
    public static boolean patternFileName(String fileName) {
        String pattern = "(@ohos.([.a-z_A-Z0-9]+).d.ts|([.a-z_A-Z0-9]+).h)";
        return Pattern.matches(pattern, fileName);
    }

    /**
     * 正则匹配所选文件名是否符合规范
     *
     * @param fileName 文件名
     * @return boolean 是否匹配
     */
    public static boolean patternFileNameH(String fileName) {
        String pattern = "([.a-z_A-Z0-9]+).h";
        return Pattern.matches(pattern, fileName);
    }

    /**
     * check project SDK
     *
     * @param project  projectid
     * @param baseFile project root file
     * @return boolean
     */
    public static boolean checkProjectSDK(Project project, String baseFile) {

        String gradlePath = "";
        File baseDir = new File(baseFile);
        if (baseDir.isDirectory()) {
            File[] childFile = baseDir.listFiles();
            assert childFile != null;
            for (File file : childFile) {
                if (file.getName().equals("build.gradle") || file.getName().equals("build-profile.json5")) {
                    gradlePath = file.getPath();
                }
            }
        }

        Properties properties = new Properties();
        if (TextUtils.isBlank(gradlePath)) {
            GenNotification.notifyMessage(project, "项目结构中没有grandle配置文件。",
                    "当前项目结构不支持",
                    NotificationType.WARNING);
            return true;
        }
        try {
            properties.load(new FileInputStream(gradlePath));
        } catch (IOException e) {
            GenNotification.notifyMessage(project, e.getMessage(), "提示", NotificationType.ERROR);
            LOG.error(String.format("Can not load file :%s . %s", gradlePath, e));
            return true;
        }
        String ohosSDK = properties.getProperty("compileSdkVersion");

        if (ohosSDK != null && Integer.parseInt(ohosSDK) < COMPILE_SDK_VERSION) {
            GenNotification.notifyMessage(project, "SKD版本过低，NAPI仅支持5.0及以上版本",
                    "提示",
                    NotificationType.WARNING);
            return true;
        }
        return false;
    }

    private String readWholeFile(String fileName) {
        File file = new File(fileName);
        byte[] rdBuf = new byte[(int) file.length()];
        try(FileInputStream in = new FileInputStream(file)) {
            in.read(rdBuf);
            return new String(rdBuf, "UTF-8");
        } catch (FileNotFoundException foundException) {
            LOG.error(String.format("File %s does not exist.", fileName));
        } catch (IOException ioException) {
            LOG.error(String.format("Failed to read file %s. Error: %s", fileName, ioException));
        }
        return "";
    }
}
