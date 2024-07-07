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

#include "dcamera_pipeline_source.h"
#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "mock_dcamera_data_process_listener.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
class DCameraPipelineSourceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<IDataProcessPipeline> testSourcePipeline_;
};

namespace {
const int32_t TEST_WIDTH = 1920;
const int32_t TEST_HEIGTH = 1080;
const int32_t TEST_WIDTH2 = 640;
const int32_t TEST_HEIGTH2 = 480;
}

void DCameraPipelineSourceTest::SetUpTestCase(void)
{
}

void DCameraPipelineSourceTest::TearDownTestCase(void)
{
}

void DCameraPipelineSourceTest::SetUp(void)
{
    testSourcePipeline_ = std::make_shared<DCameraPipelineSource>();
}

void DCameraPipelineSourceTest::TearDown(void)
{
    testSourcePipeline_ = nullptr;
}

/**
 * @tc.name: dcamera_pipeline_source_test_001
 * @tc.desc: Verify pipeline source CreateDataProcessPipeline normal.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DCameraPipelineSourceTest, dcamera_pipeline_source_test_001, TestSize.Level1)
{
    EXPECT_EQ(false, testSourcePipeline_ == nullptr);

    std::shared_ptr<DataProcessListener> listener = std::make_shared<MockDCameraDataProcessListener>();
    VideoConfigParams srcParams(VideoCodecType::CODEC_H264,
                                Videoformat::NV12,
                                DCAMERA_PRODUCER_FPS_DEFAULT,
                                TEST_WIDTH,
                                TEST_HEIGTH);
    VideoConfigParams destParams(VideoCodecType::CODEC_H264,
                                 Videoformat::NV21,
                                 DCAMERA_PRODUCER_FPS_DEFAULT,
                                 TEST_WIDTH2,
                                 TEST_HEIGTH2);
    int32_t rc = testSourcePipeline_->CreateDataProcessPipeline(PipelineType::VIDEO, srcParams, destParams, listener);
    EXPECT_EQ(rc, DCAMERA_OK);
}

/**
 * @tc.name: dcamera_pipeline_source_test_002
 * @tc.desc: Verify pipeline source ProcessData normal.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DCameraPipelineSourceTest, dcamera_pipeline_source_test_002, TestSize.Level1)
{
    EXPECT_EQ(false, testSourcePipeline_ == nullptr);

    std::shared_ptr<DataProcessListener> listener = std::make_shared<MockDCameraDataProcessListener>();
    VideoConfigParams srcParams(VideoCodecType::CODEC_H264,
                                Videoformat::NV21,
                                DCAMERA_PRODUCER_FPS_DEFAULT,
                                TEST_WIDTH,
                                TEST_HEIGTH);
    VideoConfigParams destParams(VideoCodecType::CODEC_H264,
                                 Videoformat::NV21,
                                 DCAMERA_PRODUCER_FPS_DEFAULT,
                                 TEST_WIDTH,
                                 TEST_HEIGTH);
    int32_t rc = testSourcePipeline_->CreateDataProcessPipeline(PipelineType::VIDEO, srcParams, destParams, listener);
    EXPECT_EQ(rc, DCAMERA_OK);
}

/**
 * @tc.name: dcamera_pipeline_source_test_003
 * @tc.desc: Verify pipeline source CreateDataProcessPipeline abnormal.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DCameraPipelineSourceTest, dcamera_pipeline_source_test_003, TestSize.Level1)
{
    EXPECT_EQ(false, testSourcePipeline_ == nullptr);

    std::shared_ptr<DataProcessListener> listener = std::make_shared<MockDCameraDataProcessListener>();
    VideoConfigParams srcParams(VideoCodecType::NO_CODEC,
                                Videoformat::NV21,
                                DCAMERA_PRODUCER_FPS_DEFAULT,
                                TEST_WIDTH,
                                TEST_HEIGTH);
    VideoConfigParams destParams(VideoCodecType::CODEC_H264,
                                 Videoformat::NV21,
                                 DCAMERA_PRODUCER_FPS_DEFAULT,
                                 TEST_WIDTH,
                                 TEST_HEIGTH);
    int32_t rc = testSourcePipeline_->CreateDataProcessPipeline(
        PipelineType::PHOTO_JPEG, srcParams, destParams, listener);
    EXPECT_EQ(rc, DCAMERA_NOT_FOUND);
}

/**
 * @tc.name: dcamera_pipeline_source_test_004
 * @tc.desc: Verify pipeline source ProcessData abnormal.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DCameraPipelineSourceTest, dcamera_pipeline_source_test_004, TestSize.Level1)
{
    EXPECT_EQ(false, testSourcePipeline_ == nullptr);

    std::shared_ptr<DataProcessListener> listener = std::make_shared<MockDCameraDataProcessListener>();
    VideoConfigParams srcParams(VideoCodecType::NO_CODEC,
                                Videoformat::NV21,
                                DCAMERA_PRODUCER_FPS_DEFAULT,
                                TEST_WIDTH,
                                TEST_HEIGTH);
    VideoConfigParams destParams(VideoCodecType::NO_CODEC,
                                 Videoformat::NV21,
                                 DCAMERA_PRODUCER_FPS_DEFAULT,
                                 TEST_WIDTH,
                                 TEST_HEIGTH);
    int32_t rc = testSourcePipeline_->CreateDataProcessPipeline(PipelineType::VIDEO, srcParams, destParams, listener);
    EXPECT_EQ(rc, DCAMERA_OK);

    std::vector<std::shared_ptr<DataBuffer>> buffers;
    rc = testSourcePipeline_->ProcessData(buffers);
    EXPECT_EQ(rc, DCAMERA_BAD_VALUE);
}

/**
 * @tc.name: dcamera_pipeline_source_test_005
 * @tc.desc: Verify pipeline source ProcessData abnormal.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DCameraPipelineSourceTest, dcamera_pipeline_source_test_005, TestSize.Level1)
{
    EXPECT_EQ(false, testSourcePipeline_ == nullptr);

    size_t capacity = 100;
    std::vector<std::shared_ptr<DataBuffer>> buffers;
    std::shared_ptr<DataBuffer> db = std::make_shared<DataBuffer>(capacity);
    buffers.push_back(db);
    int32_t rc = testSourcePipeline_->ProcessData(buffers);
    EXPECT_EQ(rc, DCAMERA_INIT_ERR);
}
} // namespace DistributedHardware
} // namespace OHOS
