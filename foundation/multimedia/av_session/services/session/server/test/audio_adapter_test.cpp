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
#include "audio_adapter.h"
#include "avsession_log.h"
#include "audio_info.h"

using namespace testing::ext;
using namespace OHOS::AVSession;
using OHOS::AudioStandard::AudioRendererChangeInfo;
using OHOS::AudioStandard::RendererState;

class AudioAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static constexpr int32_t TEST_CLIENT_UID = 1;
    static constexpr int32_t TEST_SESSION_ID = 2;
};

void AudioAdapterTest::SetUpTestCase()
{}

void AudioAdapterTest::TearDownTestCase()
{}

void AudioAdapterTest::SetUp()
{}

void AudioAdapterTest::TearDown()
{}

/**
* @tc.name: OnRendererStateChange001
* @tc.desc: verifying the renderer state listener
* @tc.type: FUNC
* @tc.require: AR000H31KJ
*/
static HWTEST(AudioAdapterTest, OnRendererStateChange001, TestSize.Level1)
{
    SLOGI("OnRendererStateChange001 begin!");
    std::unique_ptr<AudioRendererChangeInfo> info = std::make_unique<AudioRendererChangeInfo>();
    info->clientUID = AudioAdapterTest::TEST_CLIENT_UID;
    info->sessionId = AudioAdapterTest::TEST_SESSION_ID;
    info->rendererState = RendererState::RENDERER_RELEASED;
    AudioRendererChangeInfos infosExpected;
    infosExpected.push_back(std::move(info));
    AudioRendererChangeInfos infosActual;

    AudioAdapter::GetInstance().Init();
    AudioAdapter::GetInstance().AddStreamRendererStateListener([&infosActual](const AudioRendererChangeInfos& infos) {
        SLOGI("AddStreamRendererStateListener start!");
        for (const auto& info : infos) {
            std::unique_ptr<AudioRendererChangeInfo> infoActual = std::make_unique<AudioRendererChangeInfo>();
            *infoActual = *info;
            infosActual.push_back(std::move(infoActual));
        }
        SLOGI("AddStreamRendererStateListener end!");
    });
    AudioAdapter::GetInstance().OnRendererStateChange(infosExpected);

    ASSERT_NE(infosActual.size(), 0);
    const auto& infoExpected = infosExpected[0];
    const auto& infoActual = infosActual[0];
    EXPECT_EQ(infoExpected->clientUID, infoActual->clientUID);
    EXPECT_EQ(infoExpected->sessionId, infoActual->sessionId);
    EXPECT_EQ(infoExpected->rendererState, infoActual->rendererState);
    SLOGI("OnRendererStateChange001 end!");
}