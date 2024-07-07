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

package com.ohos.hapsigntool.hap.entity;

import com.ohos.hapsigntool.utils.ByteArrayUtils;

import java.io.IOException;

/**
 * define class of hap signature block head
 *
 * @since 2021-12-13
 */
public class HwSignHead {
    /**
     * length of sign head
     */
    public static final int SIGN_HEAD_LEN = 32;

    private static final char[] MAGIC = "hw signed app   ".toCharArray(); // 16Bytes-Magic
    private static final char[] VERSION = "1000".toCharArray(); // 4-Bytes, version is 1.0.0.0
    private static final int NUM_OF_BLOCK = 2; // number of sub-block
    private static final int RESERVE_LENGTH = 4;
    private char[] reserve = new char[RESERVE_LENGTH];

    /**
     * get serialization of HwSignHead
     *
     * @param subBlockSize the total size of all sub-blocks
     * @return Byte array after serialization of HwSignHead
     */
    public byte[] getSignHead(int subBlockSize) {
        int size = subBlockSize; // total size of sub-block
        byte[] signHead = new byte[SIGN_HEAD_LEN];
        int start = 0;
        try {
            start = ByteArrayUtils.insertCharToByteArray(signHead, start, MAGIC);
            if (start < 0) {
                throw new IOException();
            }
            start = ByteArrayUtils.insertCharToByteArray(signHead, start, VERSION);
            if (start < 0) {
                throw new IOException();
            }
            start = ByteArrayUtils.insertIntToByteArray(signHead, start, size);
            if (start < 0) {
                throw new IOException();
            }
            start = ByteArrayUtils.insertIntToByteArray(signHead, start, NUM_OF_BLOCK);
            if (start < 0) {
                throw new IOException();
            }
            start = ByteArrayUtils.insertCharToByteArray(signHead, start, reserve);
            if (start < 0) {
                throw new IOException();
            }
        } catch (IOException e) {
            return null;
        }
        return signHead;
    }
}
