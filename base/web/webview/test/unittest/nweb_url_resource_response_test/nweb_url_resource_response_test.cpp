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

#include <gtest/gtest.h>
#include <map>

#include "nweb_url_resource_response.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;

namespace OHOS::NWeb {
const std::string mime_type = "html";
const std::string encoding = "utf8";
const int status_code = 401;
const std::string reason_phrase = "Request requires authentication";
const std::map<std::string, std::string> response_headers = {{"key1", "value1"}, {"key2", "value2"}};
std::string input_stream = "input stream";

class NWebUrlResourceResponseTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void NWebUrlResourceResponseTest::SetUpTestCase(void)
{}

void NWebUrlResourceResponseTest::TearDownTestCase(void)
{}

void NWebUrlResourceResponseTest::SetUp(void)
{}

void NWebUrlResourceResponseTest::TearDown(void)
{}

/**
 * @tc.name: NWebUrlResourceResponse_Constructor_001
 * @tc.desc: Test constructor with 5 params.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebUrlResourceResponseTest, NWebUrlResourceResponse_Constructor_001, TestSize.Level1)
{
    std::shared_ptr<NWebUrlResourceResponse> webResonse = std::make_shared<NWebUrlResourceResponse>(mime_type,
        encoding, status_code, reason_phrase, response_headers, input_stream);
    std::string actual_stream = webResonse->ResponseData();
    EXPECT_STREQ(input_stream.c_str(), actual_stream.c_str());

    std::string actual_encoding = webResonse->ResponseEncoding();
    EXPECT_STREQ(encoding.c_str(), actual_encoding.c_str());

    std::string actual_mimtype = webResonse->ResponseMimeType();
    EXPECT_STREQ(mime_type.c_str(), actual_mimtype.c_str());

    std::map<std::string, std::string> actual_header = webResonse->ResponseHeaders();
    EXPECT_EQ(response_headers, actual_header);

    int actual_status_code = webResonse->ResponseStatusCode();
    EXPECT_EQ(status_code, actual_status_code);

    std::string actual_status = webResonse->ResponseStatus();
    EXPECT_STREQ(reason_phrase.c_str(), actual_status.c_str());
}

/**
 * @tc.name: NWebUrlResourceResponse_Constructor_002
 * @tc.desc: Test constructor with 3 params.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebUrlResourceResponseTest, NWebUrlResourceResponse_Constructor_002, TestSize.Level1)
{
    std::shared_ptr<NWebUrlResourceResponse> webResonse = std::make_shared<NWebUrlResourceResponse>(mime_type,
                        encoding, input_stream);
    std::string actual_stream = webResonse->ResponseData();
    EXPECT_STREQ(input_stream.c_str(), actual_stream.c_str());

    std::string actual_encoding = webResonse->ResponseEncoding();
    EXPECT_STREQ(encoding.c_str(), actual_encoding.c_str());

    std::string actual_mimtype = webResonse->ResponseMimeType();
    EXPECT_STREQ(mime_type.c_str(), actual_mimtype.c_str());
}

/**
 * @tc.name: NWebUrlResourceResponse_Constructor_003
 * @tc.desc: Test set and get functions.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebUrlResourceResponseTest, NWebUrlResourceResponse_Constructor_003, TestSize.Level1)
{
    std::shared_ptr<NWebUrlResourceResponse> webResonse = std::make_shared<NWebUrlResourceResponse>(mime_type,
        encoding, status_code, reason_phrase, response_headers, input_stream);
    webResonse->PutResponseData(input_stream);
    std::string actual_stream = webResonse->ResponseData();
    EXPECT_STREQ(input_stream.c_str(), actual_stream.c_str());

    webResonse->PutResponseEncoding(encoding);
    std::string actual_encoding = webResonse->ResponseEncoding();
    EXPECT_STREQ(encoding.c_str(), actual_encoding.c_str());

    webResonse->PutResponseMimeType(mime_type);
    std::string actual_mimtype = webResonse->ResponseMimeType();
    EXPECT_STREQ(mime_type.c_str(), actual_mimtype.c_str());

    webResonse->PutResponseHeaders(response_headers);
    std::map<std::string, std::string> actual_header = webResonse->ResponseHeaders();
    EXPECT_EQ(response_headers, actual_header);

    webResonse->PutResponseStateAndStatuscode(status_code, reason_phrase);
    int actual_status_code = webResonse->ResponseStatusCode();
    EXPECT_EQ(status_code, actual_status_code);
    std::string actual_reason_status = webResonse->ResponseStatus();
    EXPECT_STREQ(reason_phrase.c_str(), actual_reason_status.c_str());
}
} // namespace OHOS::NWeb
