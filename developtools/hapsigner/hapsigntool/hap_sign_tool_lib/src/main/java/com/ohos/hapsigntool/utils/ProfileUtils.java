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

import com.google.gson.JsonParser;
import com.ohos.hapsigntool.profile.model.Provision;

import java.io.File;
import java.io.IOException;
import java.nio.charset.StandardCharsets;

/**
 * Utils to process provision profile
 */
public class ProfileUtils {
    private ProfileUtils() {
    }

    /**
     * Get provision content.
     *
     * @param input input provision profile
     * @return file data
     */
    public static byte[] getProvisionContent(File input) throws IOException {
        byte[] bytes = FileUtils.readFile(input);
        String json = JsonParser.parseString(new String(bytes, StandardCharsets.UTF_8)).toString();
        Provision provision = FileUtils.GSON.fromJson(new String(bytes, StandardCharsets.UTF_8), Provision.class);
        Provision.enforceValid(provision);
        return json.getBytes(StandardCharsets.UTF_8);
    }
}