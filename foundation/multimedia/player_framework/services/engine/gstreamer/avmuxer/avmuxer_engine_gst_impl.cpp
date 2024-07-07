/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "avmuxer_engine_gst_impl.h"
#include <unistd.h>
#include "gst_utils.h"
#include "media_errors.h"
#include "media_log.h"
#include "uri_helper.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "AVMuxerEngineGstImpl"};
    constexpr int32_t MAX_LATITUDE = 90;
    constexpr int32_t MIN_LATITUDE = -90;
    constexpr int32_t MAX_LONGITUDE = 180;
    constexpr int32_t MIN_LONGITUDE = -180;
    constexpr uint32_t MULTIPLY10000 = 10000;
    constexpr uint32_t MAX_VIDEO_TRACK_NUM = 1;
    constexpr uint32_t MAX_AUDIO_TRACK_NUM = 16;
    constexpr uint32_t US_TO_NS = 1000;
}

namespace OHOS {
namespace Media {
static void StartFeed(const GstElement *src, guint length, gpointer userData)
{
    (void)length;
    CHECK_AND_RETURN_LOG(src != nullptr, "AppSrc does not exist");
    CHECK_AND_RETURN_LOG(userData != nullptr, "User data does not exist");
    std::map<int32_t, TrackInfo> trackInfo = *reinterpret_cast<std::map<int32_t, TrackInfo> *>(userData);
    for (auto& info : trackInfo) {
        if (info.second.src_ == src) {
            info.second.needData_ = true;
            break;
        }
    }
}

static void StopFeed(const GstElement *src, gpointer userData)
{
    CHECK_AND_RETURN_LOG(src != nullptr, "AppSrc does not exist");
    CHECK_AND_RETURN_LOG(userData != nullptr, "User data does not exist");
    std::map<int32_t, TrackInfo> trackInfo = *reinterpret_cast<std::map<int32_t, TrackInfo> *>(userData);
    for (auto& info : trackInfo) {
        if (info.second.src_ == src) {
            info.second.needData_ = false;
            break;
        }
    }
}

AVMuxerEngineGstImpl::AVMuxerEngineGstImpl()
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances create", FAKE_POINTER(this));
}

AVMuxerEngineGstImpl::~AVMuxerEngineGstImpl()
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances destroy", FAKE_POINTER(this));
    if (muxBin_ != nullptr) {
        gst_object_unref(muxBin_);
        muxBin_ = nullptr;
    }
    if (allocator_ != nullptr) {
        gst_object_unref(allocator_);
        allocator_ = nullptr;
    }
}

int32_t AVMuxerEngineGstImpl::Init()
{
    MEDIA_LOGD("Init");
    muxBin_ = GST_ELEMENT(gst_object_ref_sink(gst_element_factory_make("muxbin", "avmuxerbin")));
    CHECK_AND_RETURN_RET_LOG(muxBin_ != nullptr, MSERR_UNKNOWN, "Failed to create muxbin");

    int32_t ret = SetupMsgProcessor();
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, ret, "Failed to setup message processor");

    allocator_ = gst_shmem_wrap_allocator_new();
    CHECK_AND_RETURN_RET_LOG(allocator_ != nullptr, MSERR_NO_MEMORY, "Failed to create allocator");

    return MSERR_OK;
}

std::vector<std::string> AVMuxerEngineGstImpl::GetAVMuxerFormatList()
{
    MEDIA_LOGD("GetAVMuxerFormatList");
    CHECK_AND_RETURN_RET_LOG(muxBin_ != nullptr, std::vector<std::string>(), "Muxbin does not exist");

    return AVMuxerUtil::FindFormat();
}

int32_t AVMuxerEngineGstImpl::SetOutput(int32_t fd, const std::string &format)
{
    MEDIA_LOGD("SetOutput");
    CHECK_AND_RETURN_RET_LOG(muxBin_ != nullptr, MSERR_INVALID_OPERATION, "Muxbin does not exist");

    std::string mux;
    CHECK_AND_RETURN_RET_LOG(AVMuxerUtil::FindMux(format, mux), MSERR_INVALID_VAL, "Illegal format");
    g_object_set(muxBin_, "fd", fd, "mux", mux.c_str(), nullptr);
    format_ = format;

    return MSERR_OK;
}

int32_t AVMuxerEngineGstImpl::SetLocation(float latitude, float longitude)
{
    MEDIA_LOGD("SetLocation");
    CHECK_AND_RETURN_RET_LOG(muxBin_ != nullptr, MSERR_INVALID_OPERATION, "Muxbin does not exist");

    bool setLocationToMux = true;
    if (latitude < MIN_LATITUDE || latitude > MAX_LATITUDE || longitude < MIN_LONGITUDE
        || longitude > MAX_LONGITUDE) {
        setLocationToMux = false;
        MEDIA_LOGW("Invalid GeoLocation, latitude must be greater than %{public}d and less than %{public}d,"
            "longitude must be greater than %{public}d and less than %{public}d",
            MIN_LATITUDE, MAX_LATITUDE, MIN_LONGITUDE, MAX_LONGITUDE);
    }

    if (setLocationToMux) {
        int32_t latitudex10000 = latitude * MULTIPLY10000;
        int32_t longitudex10000 = longitude * MULTIPLY10000;
        g_object_set(muxBin_, "latitude", latitudex10000, "longitude", longitudex10000, nullptr);
    }

    return MSERR_OK;
}

int32_t AVMuxerEngineGstImpl::SetRotation(int32_t rotation)
{
    MEDIA_LOGD("SetRotation");
    CHECK_AND_RETURN_RET_LOG(muxBin_ != nullptr, MSERR_INVALID_OPERATION, "Muxbin does not exist");

    bool setRotationToMux = true;
    if (rotation != VIDEO_ROTATION_90 && rotation != VIDEO_ROTATION_180 && rotation != VIDEO_ROTATION_270) {
        setRotationToMux = false;
        MEDIA_LOGW("Invalid rotation: %{public}d, keep default 0", rotation);
    }

    if (setRotationToMux) {
        g_object_set(muxBin_, "rotation", rotation, nullptr);
    }

    return MSERR_OK;
}

int32_t AVMuxerEngineGstImpl::AddTrack(const MediaDescription &trackDesc, int32_t &trackId)
{
    MEDIA_LOGD("AddTrack");
    CHECK_AND_RETURN_RET_LOG(muxBin_ != nullptr, MSERR_INVALID_OPERATION, "Muxbin does not exist");

    std::string mimeType;
    bool val = trackDesc.GetStringValue(MediaDescriptionKey::MD_KEY_CODEC_MIME, mimeType);
    CHECK_AND_RETURN_RET_LOG(val, MSERR_INVALID_VAL, "Failed to get MD_KEY_CODEC_MIME");
    std::set<std::string> mimeTypes;
    CHECK_AND_RETURN_RET_LOG(AVMuxerUtil::FindMimeTypes(format_, mimeTypes), MSERR_INVALID_VAL, "Illegal format");
    CHECK_AND_RETURN_RET_LOG(mimeTypes.find(mimeType) != mimeTypes.end(),
        MSERR_INVALID_OPERATION, "The mime type can not be added in current container format");

    trackId = trackInfo_.size() + 1;
    trackInfo_[trackId] = TrackInfo();
    trackInfo_[trackId].mimeType_ = mimeType;

    GstCaps *srcCaps = nullptr;
    uint32_t *trackNum = nullptr;
    MediaType mediaType;
    CHECK_AND_RETURN_RET_LOG(AVMuxerUtil::FindMediaType(mimeType, mediaType), MSERR_INVALID_VAL, "Illegal mimeType");
    if (mediaType == MEDIA_TYPE_VID) {
        CHECK_AND_RETURN_RET_LOG(videoTrackNum_ < MAX_VIDEO_TRACK_NUM, MSERR_INVALID_OPERATION,
            "Only 1 video Track can be added");
        trackNum = &videoTrackNum_;
    } else if (mediaType == MEDIA_TYPE_AUD) {
        CHECK_AND_RETURN_RET_LOG(audioTrackNum_ < MAX_AUDIO_TRACK_NUM, MSERR_INVALID_OPERATION,
            "Only 16 audio Tracks can be added");
        trackNum = &audioTrackNum_;
    } else {
        MEDIA_LOGE("Failed to check track type");
        return MSERR_INVALID_VAL;
    }

    if (!AVMuxerUtil::SetCaps(trackDesc, mimeType, &srcCaps)) {
        MEDIA_LOGE("Failed to call SetCaps");
        trackInfo_.erase(trackId);
        return MSERR_INVALID_VAL;
    }

    trackInfo_[trackId].caps_ = srcCaps;

    gst_caps_ref(trackInfo_[trackId].caps_);
    MEDIA_LOGD("caps ref: %{public}d", GST_MINI_OBJECT(trackInfo_[trackId].caps_)->refcount);
    std::string name = "src_";
    name += static_cast<char>('0' + trackId);
    std::string parse;
    if (!AVMuxerUtil::FindParse(mimeType, parse)) {
        MEDIA_LOGE("Failed to call FindParse");
        trackInfo_.erase(trackId);
        return MSERR_INVALID_VAL;
    }
    g_signal_emit_by_name(muxBin_, "add-track", name.c_str(),
        (parse + std::to_string(trackId)).c_str(), static_cast<int32_t>(mediaType));
    (*trackNum)++;

    return MSERR_OK;
}

int32_t AVMuxerEngineGstImpl::Start()
{
    MEDIA_LOGD("Start");
    std::unique_lock<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(muxBin_ != nullptr, MSERR_INVALID_OPERATION, "Muxbin does not exist");
    gst_element_set_state(GST_ELEMENT_CAST(muxBin_), GST_STATE_PLAYING);

    for (auto& info : trackInfo_) {
        std::string name = "src_";
        name += static_cast<char>('0' + info.first);
        GstElement *src = gst_bin_get_by_name(GST_BIN_CAST(muxBin_), name.c_str());
        CHECK_AND_RETURN_RET_LOG(src != nullptr, MSERR_INVALID_OPERATION, "src does not exist");
        g_signal_connect(src, "need-data", G_CALLBACK(StartFeed), reinterpret_cast<gpointer *>(&trackInfo_));
        g_signal_connect(src, "enough-data", G_CALLBACK(StopFeed), reinterpret_cast<gpointer *>(&trackInfo_));
        info.second.src_ = src;
    }

    return MSERR_OK;
}

int32_t AVMuxerEngineGstImpl::WriteData(std::shared_ptr<AVSharedMemory> sampleData,
    const TrackSampleInfo &sampleInfo, GstElement *src)
{
    CHECK_AND_RETURN_RET_LOG(sampleData != nullptr, MSERR_INVALID_VAL, "sampleData is nullptr");
    CHECK_AND_RETURN_RET_LOG(src != nullptr, MSERR_INVALID_VAL, "src is nullptr");
    CHECK_AND_RETURN_RET_LOG(allocator_ != nullptr, MSERR_INVALID_VAL, "allocator is nullptr");

    GstMemory *mem = gst_shmem_wrap(GST_ALLOCATOR_CAST(allocator_), sampleData);
    CHECK_AND_RETURN_RET_LOG(mem != nullptr, MSERR_NO_MEMORY, "Failed to call gst_shmem_wrap");

    GstBuffer *buffer = gst_buffer_new();
    if (buffer == nullptr) {
        gst_memory_unref(mem);
        MEDIA_LOGE("Failed to call gst_buffer_new");
        return MSERR_NO_MEMORY;
    }

    gst_buffer_append_memory(buffer, mem);
    GST_BUFFER_DTS(buffer) = static_cast<uint64_t>(sampleInfo.timeUs * US_TO_NS);
    GST_BUFFER_PTS(buffer) = static_cast<uint64_t>(sampleInfo.timeUs * US_TO_NS);

    GstFlowReturn ret;
    g_signal_emit_by_name(src, "push-buffer", buffer, &ret);
    if (ret != GST_FLOW_OK) {
        gst_buffer_unref(buffer);
        MEDIA_LOGE("Failed to call g_signal_emit_by_name");
        return MSERR_INVALID_OPERATION;
    }
    gst_buffer_unref(buffer);

    return MSERR_OK;
}

int32_t AVMuxerEngineGstImpl::WriteTrackSample(std::shared_ptr<AVSharedMemory> sampleData,
    const TrackSampleInfo &sampleInfo)
{
    MEDIA_LOGD("WriteTrackSample, sampleInfo.trackIdx is %{public}d", sampleInfo.trackIdx);
    std::unique_lock<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(errHappened_ != true, MSERR_INVALID_OPERATION, "Error happend");
    CHECK_AND_RETURN_RET_LOG(muxBin_ != nullptr, MSERR_INVALID_OPERATION, "Muxbin does not exist");
    CHECK_AND_RETURN_RET_LOG(sampleData != nullptr, MSERR_INVALID_VAL, "sampleData is nullptr");
    CHECK_AND_RETURN_RET_LOG(trackInfo_[sampleInfo.trackIdx].needData_ == true, MSERR_INVALID_OPERATION,
        "Failed to push data, the queue is full");
    CHECK_AND_RETURN_RET_LOG(sampleInfo.timeUs >= 0, MSERR_INVALID_VAL, "Failed to check dts, dts muxt >= 0");

    int32_t ret;
    GstElement *src = trackInfo_[sampleInfo.trackIdx].src_;
    CHECK_AND_RETURN_RET_LOG(src != nullptr, MSERR_INVALID_VAL, "Failed to get AppSrc");

    if ((sampleInfo.flags & AVCODEC_BUFFER_FLAG_CODEC_DATA) &&
        trackInfo_[sampleInfo.trackIdx].hasCodecData_ != true) {
        CHECK_AND_RETURN_RET_LOG(trackInfo_[sampleInfo.trackIdx].caps_ != nullptr,
            MSERR_INVALID_OPERATION, "Failed to check caps");
        g_object_set(src, "caps", trackInfo_[sampleInfo.trackIdx].caps_, nullptr);
        ret = WriteData(sampleData, sampleInfo, src);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, ret, "Failed to call WriteData");
        trackInfo_[sampleInfo.trackIdx].hasCodecData_ = true;
    } else if (!(sampleInfo.flags & AVCODEC_BUFFER_FLAG_CODEC_DATA & AVCODEC_BUFFER_FLAG_EOS) &&
        trackInfo_[sampleInfo.trackIdx].hasCodecData_ == true) {
        ret = WriteData(sampleData, sampleInfo, src);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, ret, "Failed to call WriteData");
    } else {
        MEDIA_LOGW("Failed to Write sample, note: first frame must be code_data");
    }

    return MSERR_OK;
}

int32_t AVMuxerEngineGstImpl::Stop()
{
    MEDIA_LOGD("Stop");
    std::unique_lock<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(muxBin_ != nullptr, MSERR_INVALID_OPERATION, "Muxbin does not exist");

    GstFlowReturn ret;
    if (isPlay_) {
        for (auto& info : trackInfo_) {
            g_signal_emit_by_name(info.second.src_, "end-of-stream", &ret);
            CHECK_AND_RETURN_RET_LOG(ret == GST_FLOW_OK, ret, "Failed to push end of stream");
        }
        cond_.wait(lock, [this]() {
            return endFlag_ || errHappened_;
        });
    }
    gst_element_set_state(GST_ELEMENT_CAST(muxBin_), GST_STATE_NULL);
    Clear();
    return MSERR_OK;
}

int32_t AVMuxerEngineGstImpl::SetupMsgProcessor()
{
    CHECK_AND_RETURN_RET_LOG(muxBin_ != nullptr, MSERR_INVALID_OPERATION, "Muxbin does not exist");
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE_CAST(muxBin_));
    CHECK_AND_RETURN_RET_LOG(bus != nullptr, MSERR_INVALID_OPERATION, "Failed to create GstBus");

    auto msgNotifier = std::bind(&AVMuxerEngineGstImpl::OnNotifyMessage, this, std::placeholders::_1);
    msgProcessor_ = std::make_unique<GstMsgProcessor>(*bus, msgNotifier);
    gst_object_unref(bus);
    bus = nullptr;

    int32_t ret = msgProcessor_->Init();
    CHECK_AND_RETURN_RET(ret == MSERR_OK, ret);

    msgProcessor_->AddMsgFilter(ELEM_NAME(GST_ELEMENT_CAST(muxBin_)));

    return MSERR_OK;
}

void AVMuxerEngineGstImpl::OnNotifyMessage(const InnerMessage &msg)
{
    switch (msg.type) {
        case InnerMsgType::INNER_MSG_EOS: {
            std::unique_lock<std::mutex> lock(mutex_);
            MEDIA_LOGD("End of stream");
            endFlag_ = true;
            cond_.notify_all();
            break;
        }
        case InnerMsgType::INNER_MSG_ERROR: {
            std::unique_lock<std::mutex> lock(mutex_);
            MEDIA_LOGE("Error happened");
            msgProcessor_->FlushBegin();
            msgProcessor_->Reset();
            errHappened_ = true;
            cond_.notify_all();
            break;
        }
        case InnerMsgType::INNER_MSG_STATE_CHANGED: {
            std::unique_lock<std::mutex> lock(mutex_);
            MEDIA_LOGD("State change");
            GstState currState = static_cast<GstState>(msg.detail2);
            if (currState == GST_STATE_PLAYING) {
                isPlay_ = true;
            }
            cond_.notify_all();
            break;
        }
        default:
            break;
    }
}

void AVMuxerEngineGstImpl::Clear()
{
    for (auto iter = trackInfo_.begin(); iter != trackInfo_.end(); iter++) {
        gst_caps_unref(iter->second.caps_);
    }
    trackInfo_.clear();
    endFlag_ = false;
    errHappened_ = false;
    isPlay_ = false;

    mutex_.unlock();
    msgProcessor_->Reset();
    mutex_.lock();
}
}  // namespace Media
}  // namespace OHOS
