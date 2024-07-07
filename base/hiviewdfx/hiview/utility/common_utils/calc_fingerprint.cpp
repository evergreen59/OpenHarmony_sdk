/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "calc_fingerprint.h"

#include <securec.h>

#include "common_defines.h"
#include "file_util.h"

using namespace std;
namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("CalcFingerprint");
int CalcFingerprint::ConvertToString(const unsigned char hash[SHA256_DIGEST_LENGTH], char *outstr, size_t len)
{
    uint32_t i;
    char *outHash = outstr;

    if (hash == nullptr || outHash == nullptr) {
        return EINVAL;
    }
    constexpr int charsEachHex = 2;
    if (len < (SHA256_DIGEST_LENGTH * charsEachHex + 1)) { // 1: add '\0'
        return ENOMEM;
    }
    for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        int err = snprintf_s(outHash, charsEachHex + 1, charsEachHex, "%02x", hash[i]);
        if (err < 0) {
            return err;
        }
        outHash += charsEachHex;
    }
    *outHash = '\0';
    return 0;
}

/*
 * API name : calc_file_sha1
 * Description : calculate a file sha1 hash for given file
 * Input parameters
 * filePath : path of the file to be calculated
 * hash      : buffer to store output sha1 string
 * Return
 * 0 : successful
 * x : fail
 */
int CalcFingerprint::CalcFileSha(const string& filePath, char *hash, size_t len)
{
    if (filePath.empty() || hash == nullptr || !FileUtil::IsLegalPath(filePath)) {
        return EINVAL;
    }
    FILE *fp = nullptr;
    fp = fopen(filePath.c_str(), "rb");
    if (fp == nullptr) {
        return errno; // if file not exist, errno will be ENOENT
    }

    size_t n;
    char buffer[HASH_BUFFER_SIZE] = {0};
    unsigned char value[SHA256_DIGEST_LENGTH] = {0};
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    while ((n = fread(buffer, 1, sizeof(buffer), fp))) {
        SHA256_Update(&ctx, (unsigned char *)buffer, n);
    }
    if (fclose(fp)) {
        HIVIEW_LOGE("fclose is failed");
    }
    fp = nullptr;
    SHA256_Final(value, &ctx);
    return ConvertToString(value, hash, len);
}

/*
 * API name : calc_buffer_sha1
 * Description : calculate a buffer sha1 hash for given buffer
 * Input parameters
 * buffer : buffer to store the content which needed to be calculated
 * hash_str   : buffer to store output sha1 string
 * Return
 * 0 : successful
 * x : fail
 */
int CalcFingerprint::CalcBufferSha(const string& buffer, size_t bufSize, char *hash, size_t len)
{
    if (buffer.empty() || hash == nullptr) {
        return EINVAL;
    }
    unsigned char value[SHA256_DIGEST_LENGTH] = {0};
    SHA256((unsigned char *)buffer.c_str(), bufSize, value);
    return ConvertToString(value, hash, len);
}
}
}
