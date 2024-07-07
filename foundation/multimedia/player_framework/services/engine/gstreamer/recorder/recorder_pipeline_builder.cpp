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

#include "recorder_pipeline_builder.h"
#include "media_errors.h"
#include "media_log.h"
#include "recorder_private_param.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "RecorderPipelineBuilder"};
}

namespace OHOS {
namespace Media {
#define ADD_LINK_DESC(srcElem, dstElem, srcPad, sinkPad, isSrcPadStatic, isSinkPadStatic)                       \
    do {                                                                                                        \
        RecorderPipelineDesc::LinkDesc linkDesc = { dstElem, srcPad, sinkPad, isSrcPadStatic, isSinkPadStatic}; \
        pipelineDesc_->allLinkDescs[srcElem] = linkDesc;                                                        \
    } while (false)

RecorderPipelineBuilder::RecorderPipelineBuilder(int32_t appUid, int32_t appPid, uint32_t appTokenId)
    : appUid_(appUid), appPid_(appPid), appTokenId_(appTokenId)
{
    MEDIA_LOGD("enter, ctor");
}

RecorderPipelineBuilder::~RecorderPipelineBuilder()
{
    MEDIA_LOGD("enter, dtor");
    Reset();
}

void RecorderPipelineBuilder::EnsureSourceOrder(bool isVideo)
{
    auto srcIter = std::next(pipelineDesc_->allElems.end(), -1);
    if (isVideo) {
        auto insertPos = std::next(pipelineDesc_->allElems.begin(), videoSrcCount_);
        (void)pipelineDesc_->allElems.insert(insertPos, *srcIter);
        videoSrcCount_ += 1;
    } else {
        auto insertPos = std::next(pipelineDesc_->allElems.begin(), videoSrcCount_ + otherSrcCount_);
        (void)pipelineDesc_->allElems.insert(insertPos, *srcIter);
        otherSrcCount_ += 1;
    }
    (void)pipelineDesc_->allElems.erase(srcIter);
}

std::shared_ptr<RecorderElement> RecorderPipelineBuilder::CreateElement(
    const std::string &name,
    const RecorderSourceDesc &desc,
    bool isSource)
{
    if (pipelineDesc_ == nullptr) {
        pipelineDesc_ = std::make_shared<RecorderPipelineDesc>();
    }

    RecorderElement::CreateParam createParam = { desc, name };
    std::shared_ptr<RecorderElement> element = RecorderElementFactory::GetInstance().CreateElement(name, createParam);
    if (element == nullptr) {
        std::string sourceKind = desc.IsVideo() ? "video" : (desc.IsAudio() ? "audio" : "unknown");
        MEDIA_LOGE("Unable to create element for %{public}s source type: %{public}d, element name: %{public}s",
                   sourceKind.c_str(), desc.type_, name.c_str());
        return nullptr;
    }

    pipelineDesc_->allElems.push_back(element);
    if (isSource) {
        (void)pipelineDesc_->srcElems.emplace(desc.handle_, element);
        EnsureSourceOrder(desc.IsVideo());
    }

    return element;
}

int32_t RecorderPipelineBuilder::CreateMuxSink()
{
    if (muxSink_ != nullptr) {
        return MSERR_OK;
    }

    RecorderSourceDesc desc {}; // default initialization, meaninglessly
    muxSink_ = CreateElement("MuxSinkBin", desc, false);
    if (muxSink_ == nullptr) {
        MEDIA_LOGE("Unable to create element for MuxSinkBin !");
        return MSERR_INVALID_OPERATION;
    }
    pipelineDesc_->muxerSinkBin = muxSink_;

    return MSERR_OK;
}

int32_t RecorderPipelineBuilder::SetVideoSource(const RecorderSourceDesc &desc)
{
    int32_t ret = CreateMuxSink();
    CHECK_AND_RETURN_RET(ret == MSERR_OK, ret);

    // ES Source and YUV Source is supported.
    if (desc.type_ == VideoSourceType::VIDEO_SOURCE_SURFACE_ES ||
        desc.type_ == VideoSourceType::VIDEO_SOURCE_SURFACE_YUV ||
        desc.type_ == VideoSourceType::VIDEO_SOURCE_SURFACE_RGBA) {
        videoSrcElem_ = CreateElement("VideoSource", desc, true);
    } else {
        MEDIA_LOGE("Video source type %{public}d currently unsupported", desc.type_);
    }

    CHECK_AND_RETURN_RET(videoSrcElem_ != nullptr, MSERR_INVALID_VAL);

    videoConverElem_ = CreateElement("VideoConverter", desc, false);
    CHECK_AND_RETURN_RET(videoConverElem_ != nullptr, MSERR_INVALID_VAL);

    videoParseElem_ = CreateElement("VideoParse", desc, false);
    CHECK_AND_RETURN_RET(videoParseElem_ != nullptr, MSERR_INVALID_VAL);

    // check yuv/rgbs stream
    if (desc.type_ == VideoSourceType::VIDEO_SOURCE_SURFACE_YUV ||
        desc.type_ == VideoSourceType::VIDEO_SOURCE_SURFACE_RGBA) {
        videoEncElem_ = CreateElement("VideoEncoder", desc, false);
        CHECK_AND_RETURN_RET(videoEncElem_ != nullptr, MSERR_INVALID_VAL);

        // for the second video source, the sinkpad name should be video_aux_%u
        ADD_LINK_DESC(videoSrcElem_, videoConverElem_, "src", "sink", true, true);
        ADD_LINK_DESC(videoConverElem_, videoEncElem_, "src", "sink", true, true);
        ADD_LINK_DESC(videoEncElem_, muxSink_, "src", "video", true, false);
    } else {
        // es stream
        ADD_LINK_DESC(videoSrcElem_, videoParseElem_, "src", "sink", true, true);
        ADD_LINK_DESC(videoParseElem_, muxSink_, "src", "video", true, false);
    }

    currentVideoSourceType_ = desc.type_;

    return MSERR_OK;
}

int32_t RecorderPipelineBuilder::SetAudioSource(const RecorderSourceDesc &desc)
{
    int32_t ret = CreateMuxSink();
    CHECK_AND_RETURN_RET(ret == MSERR_OK, ret);

    std::shared_ptr<RecorderElement> audioSrcElem;
    // currently only the mic is supported.
    if (desc.type_ == AudioSourceType::AUDIO_MIC) {
        audioSrcElem = CreateElement("AudioSource", desc, true);
    } else {
        MEDIA_LOGE("Audio source type %{public}d currently unsupported", desc.type_);
    }

    CHECK_AND_RETURN_RET(audioSrcElem != nullptr, MSERR_INVALID_VAL);
    (void)audioSrcElem->Configure(AppInfo {appUid_, appPid_, appTokenId_});

    std::shared_ptr<RecorderElement> audioConvert = CreateElement("AudioConverter", desc, false);
    CHECK_AND_RETURN_RET(audioConvert != nullptr, MSERR_INVALID_VAL);

    std::shared_ptr<RecorderElement> audioEncElem = CreateElement("AudioEncoder", desc, false);
    CHECK_AND_RETURN_RET(audioEncElem != nullptr, MSERR_INVALID_VAL);

    ADD_LINK_DESC(audioSrcElem, audioConvert, "src", "sink", true, true);
    ADD_LINK_DESC(audioConvert, audioEncElem, "src", "sink", true, true);
    ADD_LINK_DESC(audioEncElem, muxSink_, "src", "audio_%u", true, false);

    return MSERR_OK;
}

int32_t RecorderPipelineBuilder::SetSource(const RecorderSourceDesc &desc)
{
    if (desc.IsVideo()) {
        return SetVideoSource(desc);
    } else if (desc.IsAudio()) {
        return SetAudioSource(desc);
    }

    // should never go to here.
    MEDIA_LOGE("Invalid source description !");
    return MSERR_INVALID_VAL;
}

int32_t RecorderPipelineBuilder::SetOutputFormat(OutputFormatType formatType)
{
    if (muxSink_ == nullptr) {
        MEDIA_LOGE("No source set, set the output format invalid !");
        return MSERR_INVALID_OPERATION;
    }

    int32_t ret = muxSink_->Configure(OutputFormat(formatType));
    CHECK_AND_RETURN_RET(ret == MSERR_OK, ret);

    outputFormatConfiged_ = true;
    return MSERR_OK;
}

int32_t RecorderPipelineBuilder::CheckConfigure(int32_t sourceId, const RecorderParam &param)
{
    (void)sourceId;
    if (param.type == RecorderPublicParamType::VID_ENC_FMT) {
        const VidEnc &tempParam = static_cast<const VidEnc &>(param);

        if ((currentVideoSourceType_ == VideoSourceType::VIDEO_SOURCE_BUTT) ||
            ((currentVideoSourceType_ == VideoSourceType::VIDEO_SOURCE_SURFACE_ES))) {
            needVideoParse_ = false;
            return MSERR_OK;
        }

        if (tempParam.encFmt == VideoCodecFormat::H264) {
            needVideoParse_ = true;
        } else {
            needVideoParse_ = false;
        }
    }

    return MSERR_OK;
}

int32_t RecorderPipelineBuilder::Configure(int32_t sourceId, const RecorderParam &param)
{
    if (!outputFormatConfiged_) {
        MEDIA_LOGE("Output format not set, configure the pipeline is invalid !");
        return MSERR_INVALID_OPERATION;
    }

    int32_t ret = CheckConfigure(sourceId, param);
    CHECK_AND_RETURN_RET(ret == MSERR_OK, ret);

    // distribute parameters to elements
    for (auto &elem : pipelineDesc_->allElems) {
        if (elem->GetSourceId() == sourceId) {
            ret = elem->Configure(param);
            CHECK_AND_RETURN_RET(ret == MSERR_OK, ret);
        }
    }

    return MSERR_OK;
}

int32_t RecorderPipelineBuilder::CheckPipeline()
{
    if (needVideoParse_) {
        RecorderSourceDesc desc {};
        videoParseElem_ = CreateElement("VideoParse", desc, false);
        CHECK_AND_RETURN_RET(videoParseElem_ != nullptr, MSERR_UNKNOWN);

        for (auto iter = pipelineDesc_->allLinkDescs.begin(); iter != pipelineDesc_->allLinkDescs.end();) {
            if (iter->first->GetName() == "VideoEncoder") {
                pipelineDesc_->allLinkDescs.erase(iter++);
                break;
            } else {
                iter++;
            }
        }

        CHECK_AND_RETURN_RET(videoEncElem_ != nullptr, MSERR_INVALID_OPERATION);
        ADD_LINK_DESC(videoEncElem_, videoParseElem_, "src", "sink", true, true);
        ADD_LINK_DESC(videoParseElem_, muxSink_, "src", "video", true, false);
    }

    return MSERR_OK;
}

int32_t RecorderPipelineBuilder::Build(std::shared_ptr<RecorderPipeline> &pipeline)
{
    if (!outputFormatConfiged_) {
        MEDIA_LOGE("Output format not configured, build pipeline failed !");
        return MSERR_INVALID_OPERATION;
    }

    /*
     * Execute a series of policies to filter pipeline graphs or check pipeline parameter configurations.
     *
     * | Remove stream policy | -> | Check parameter completeness policy | -> | Add elements policy |
     *
     * 1. Execute those policies firsly that maybe remove the whole audio stream or video stream
     * 2. Now, all streams are needed. Then, execute those policies that check whether all parameters are
     *    configured completely. If not completely, refuse to build pipeline.
     * 3. Execute those policies that maybe need to add some elements into pipeline.
     *
     * Specifically:
     * 1. Process the mismatch between capture rate and frame rate. If capture rate less than the frame rate, the
     *    all audio stream need be removed.
     * 2. Check whether the parameter fully configured. For example, the audio encoder required, but the audio encoder
     *    format is not configured or erroneously configured. The element itself can judge whether the required
     *    parameters are complete. If the parameter configured not completely, refuse to build pipeline.
     * 3. Process the audio caps mismatch between audio source and audio encoder. If caps mismatch is true, add the
     *    audio converter element into audio stream.
     */

    int32_t ret = CheckPipeline();
    CHECK_AND_RETURN_RET(ret == MSERR_OK, ret);

    for (auto &elem : pipelineDesc_->allElems) {
        ret = elem->CheckConfigReady();  // Check whether the parameter fully configured
        CHECK_AND_RETURN_RET(ret == MSERR_OK, ret);
    }

    ret = ExecuteLink();
    CHECK_AND_RETURN_RET(ret == MSERR_OK, ret);

    pipeline = pipeline_;
    return MSERR_OK;
}

int32_t RecorderPipelineBuilder::ExecuteLink()
{
    auto pipeline = std::make_shared<RecorderPipeline>(pipelineDesc_);
    int32_t ret = pipeline->Init();
    CHECK_AND_RETURN_RET(ret == MSERR_OK, ret);

    auto linkHelper = std::make_unique<RecorderPipelineLinkHelper>(pipeline, pipelineDesc_);
    ret = linkHelper->ExecuteLink();
    CHECK_AND_RETURN_RET(ret == MSERR_OK, ret);

    pipeline_ = pipeline;
    linkHelper_ = std::move(linkHelper);
    return MSERR_OK;
}

void RecorderPipelineBuilder::Reset()
{
    linkHelper_ = nullptr;
    muxSink_ = nullptr;
    videoSrcElem_ = nullptr;
    videoEncElem_ = nullptr;
    videoParseElem_ = nullptr;
    videoConverElem_ = nullptr;
    if (pipeline_ != nullptr) {
        (void)pipeline_->Reset();
    }
    pipeline_ = nullptr;
    pipelineDesc_ = nullptr;

    videoSrcCount_ = 0;
    otherSrcCount_ = 0;

    outputFormatConfiged_ = false;
    currentVideoSourceType_ = VideoSourceType::VIDEO_SOURCE_BUTT;
    needVideoParse_ = false;
}
} // namespace Media
} // namespace OHOS
