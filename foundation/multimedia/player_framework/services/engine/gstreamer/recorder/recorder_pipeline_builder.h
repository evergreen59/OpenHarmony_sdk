/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef RECORDER_PIPELINE_BUILDER_H
#define RECORDER_PIPELINE_BUILDER_H

#include <memory>
#include "nocopyable.h"
#include "recorder_inner_defines.h"
#include "recorder_pipeline.h"
#include "recorder_element.h"
#include "recorder_pipeline_link_helper.h"

namespace OHOS {
namespace Media {
class RecorderPipelineBuilder : public NoCopyable {
public:
    RecorderPipelineBuilder(int32_t appUid, int32_t appPid, uint32_t appTokenId);
    ~RecorderPipelineBuilder();

    int32_t SetSource(const RecorderSourceDesc &desc);
    int32_t SetOutputFormat(OutputFormatType formatType);
    int32_t Configure(int32_t sourceId, const RecorderParam &param);
    int32_t Build(std::shared_ptr<RecorderPipeline> &pipeline);
    void Reset();

private:
    int32_t SetVideoSource(const RecorderSourceDesc &desc);
    int32_t SetAudioSource(const RecorderSourceDesc &desc);
    int32_t CreateMuxSink();
    std::shared_ptr<RecorderElement> CreateElement(
        const std::string &name, const RecorderSourceDesc &desc, bool isSource);
    void EnsureSourceOrder(bool isVideo);
    int32_t CheckConfigure(int32_t sourceId, const RecorderParam &param);
    int32_t CheckPipeline();

    int32_t ExecuteLink();

    std::shared_ptr<RecorderPipelineDesc> pipelineDesc_;
    std::shared_ptr<RecorderPipeline> pipeline_;
    std::shared_ptr<RecorderElement> muxSink_;
    std::shared_ptr<RecorderElement> videoSrcElem_;
    std::shared_ptr<RecorderElement> videoEncElem_;
    std::shared_ptr<RecorderElement> videoParseElem_;
    std::shared_ptr<RecorderElement> videoConverElem_;

    bool outputFormatConfiged_ = false;
    bool needVideoParse_ = false;
    std::unique_ptr<RecorderPipelineLinkHelper> linkHelper_;
    size_t videoSrcCount_ = 0;
    size_t otherSrcCount_ = 0;
    int32_t currentVideoSourceType_ = VideoSourceType::VIDEO_SOURCE_BUTT;
    int32_t appUid_;
    int32_t appPid_;
    uint32_t appTokenId_;
};
} // namespace Media
} // namespace OHOS
#endif
