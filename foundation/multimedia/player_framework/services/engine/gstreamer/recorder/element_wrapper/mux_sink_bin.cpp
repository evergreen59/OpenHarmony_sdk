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

#include "mux_sink_bin.h"
#include <unistd.h>
#include <fcntl.h>
#include <gst/gst.h>
#include "datetime_ex.h"
#include "media_errors.h"
#include "directory_ex.h"
#include "media_log.h"
#include "recorder_private_param.h"
#include "scope_guard.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "MuxSinkBin"};
    constexpr int32_t MAX_LATITUDE = 90;
    constexpr int32_t MIN_LATITUDE = -90;
    constexpr int32_t MAX_LONGITUDE = 180;
    constexpr int32_t MIN_LONGITUDE = -180;
    constexpr uint32_t MULTIPLY10000 = 10000;
}

namespace OHOS {
namespace Media {
MuxSinkBin::~MuxSinkBin()
{
    MEDIA_LOGD("enter, dtor");
    (void)Reset();
}

int32_t MuxSinkBin::Init()
{
    ON_SCOPE_EXIT(0) {
        if (gstElem_ != nullptr) {
            gst_object_unref(gstElem_);
            gstElem_ = nullptr;
        }
    };

    gstElem_ = gst_element_factory_make("splitmuxsink", name_.c_str());
    if (gstElem_ == nullptr) {
        MEDIA_LOGE("Create splitmuxsink gst element failed !");
        return MSERR_INVALID_OPERATION;
    }

    gstSink_ = gst_element_factory_make("fdsink", "fdsink");
    if (gstSink_ == nullptr) {
        MEDIA_LOGE("Create fdsink gst element failed !");
        return MSERR_INVALID_OPERATION;
    }

    CANCEL_SCOPE_EXIT_GUARD(0);
    g_object_set(gstElem_, "sink", gstSink_, nullptr);
    return MSERR_OK;
}

int32_t MuxSinkBin::Configure(const RecorderParam &recParam)
{
    int32_t ret = MSERR_OK;
    switch (recParam.type) {
        case RecorderPrivateParamType::OUTPUT_FORMAT:
            ret = ConfigureOutputFormat(recParam);
            break;
        case RecorderPublicParamType::OUT_FD:
            ret = ConfigureOutputTarget(recParam);
            break;
        case RecorderPublicParamType::MAX_DURATION:
            ret = ConfigureMaxDuration(recParam);
            break;
        case RecorderPublicParamType::MAX_SIZE:
            ret = ConfigureMaxFileSize(recParam);
            break;
        case RecorderPublicParamType::GEO_LOCATION:
            ret = ConfigureGeoLocation(recParam);
            break;
        case RecorderPublicParamType::VID_ORIENTATION_HINT:
            ret = ConfigureRotationAngle(recParam);
            break;
        default:
            break;
    }
    return ret;
}

int32_t MuxSinkBin::ConfigureOutputFormat(const RecorderParam &recParam)
{
    if (recParam.type != RecorderPrivateParamType::OUTPUT_FORMAT) {
        return MSERR_OK;
    }

    const OutputFormat &param = static_cast<const OutputFormat &>(recParam);
    if ((param.format_ == OutputFormatType::FORMAT_MPEG_4) || (param.format_ == OutputFormatType::FORMAT_M4A)) {
        int ret = CreateMuxerElement("mp4mux");
        CHECK_AND_RETURN_RET(ret == MSERR_OK, ret);
    } else {
        MEDIA_LOGE("output format type unsupported currently, format: %{public}d", param.format_);
        return MSERR_INVALID_VAL;
    }

    MEDIA_LOGI("configure output format: %{public}d", param.format_);
    format_ = param.format_;
    MarkParameter(param.type);

    return MSERR_OK;
}

int32_t MuxSinkBin::ConfigureOutputTarget(const RecorderParam &recParam)
{
    isReg_ = false;

    if (recParam.type == RecorderPublicParamType::OUT_FD) {
        const OutFd &param = static_cast<const OutFd &>(recParam);
        int flags = fcntl(param.fd, F_GETFL);
        if (flags == -1) {
            MEDIA_LOGE("Fail to get File Status Flags");
            return MSERR_INVALID_VAL;
        }
        if ((static_cast<unsigned int>(flags) & (O_RDWR | O_WRONLY)) == 0) {
            MEDIA_LOGE("File descriptor is not in read-write mode or write-only mode");
            return MSERR_INVALID_VAL;
        }
        MEDIA_LOGI("Configure output fd ok");
        if (outFd_ > 0) {
            (void)::close(outFd_);
        }
        outFd_ = dup(param.fd);
        g_object_set(gstSink_, "fd", outFd_, nullptr);
    }

    MarkParameter(recParam.type);
    return MSERR_OK;
}

int32_t MuxSinkBin::ConfigureMaxDuration(const RecorderParam &recParam)
{
    const MaxDuration &param = static_cast<const MaxDuration &>(recParam);
    if (param.duration <= 0) {
        MEDIA_LOGE("Invalid max record duration: %{public}d", param.duration);
        return MSERR_INVALID_VAL;
    }
    MEDIA_LOGI("Set max duration success: %{public}d", param.duration);

    MarkParameter(recParam.type);
    maxDuration_ = param.duration;
    return MSERR_OK;
}

int32_t MuxSinkBin::ConfigureMaxFileSize(const RecorderParam &recParam)
{
    const MaxFileSize &param = static_cast<const MaxFileSize &>(recParam);
    if (param.size <= 0) {
        MEDIA_LOGE("Invalid max record file size: (%{public}" PRId64 ")", param.size);
        return MSERR_INVALID_VAL;
    }
    MEDIA_LOGI("Set max filesize success: (%{public}" PRId64 ")", param.size);

    MarkParameter(recParam.type);
    maxSize_ = param.size;
    return MSERR_OK;
}

int32_t MuxSinkBin::ConfigureGeoLocation(const RecorderParam &recParam)
{
    const GeoLocation &param = static_cast<const GeoLocation &>(recParam);
    bool setLocationToMux = true;
    if (param.latitude < MIN_LATITUDE || param.latitude > MAX_LATITUDE ||
        param.longitude < MIN_LONGITUDE || param.longitude > MAX_LONGITUDE) {
        setLocationToMux = false;
        MEDIA_LOGE("Invalid GeoLocation!");
    }

    MarkParameter(recParam.type);
    int32_t latitudex10000 = param.latitude * MULTIPLY10000;
    int32_t longitudex10000 = param.longitude * MULTIPLY10000;
    if (setLocationToMux) {
        g_object_set(gstMuxer_, "set-latitude", latitudex10000, nullptr);
        g_object_set(gstMuxer_, "set-longitude", longitudex10000, nullptr);
    }
    return MSERR_OK;
}

int32_t MuxSinkBin::ConfigureRotationAngle(const RecorderParam &recParam)
{
    const RotationAngle &param = static_cast<const RotationAngle &>(recParam);
    bool setRotationToMux = true;
    if (param.rotation != VIDEO_ROTATION_90 && param.rotation != VIDEO_ROTATION_180 &&
        param.rotation != VIDEO_ROTATION_270) {
        setRotationToMux = false;
        MEDIA_LOGE("Invalid rotation: %{public}d, keep default 0", param.rotation);
    }

    MarkParameter(recParam.type);
    if (setRotationToMux) {
        g_object_set(gstMuxer_, "orientation-hint", param.rotation, nullptr);
        MEDIA_LOGI("set rotation: %{public}d", param.rotation);
    }
    return MSERR_OK;
}

int32_t MuxSinkBin::CheckConfigReady()
{
    std::set<int32_t> expectedParam = { RecorderPrivateParamType::OUTPUT_FORMAT, RecorderPublicParamType::OUT_FD };
    bool configed = CheckAllParamsConfigured(expectedParam);
    CHECK_AND_RETURN_RET(configed == true, MSERR_INVALID_OPERATION);

    return MSERR_OK;
}

int32_t MuxSinkBin::Reset()
{
    if (outFd_ > 0) {
        (void)::close(outFd_);
        outFd_ = -1;
    }

    return MSERR_OK;
}

int32_t MuxSinkBin::SetParameter(const RecorderParam &recParam)
{
    (void)recParam;
    return MSERR_OK;
}

int32_t MuxSinkBin::CreateMuxerElement(const std::string &name)
{
    gstMuxer_ = gst_element_factory_make(name.c_str(), name.c_str());
    if (gstMuxer_ == nullptr) {
        MEDIA_LOGE("Create %{public}s gst element failed !", name.c_str());
        return MSERR_INVALID_OPERATION;
    }
    g_object_set(gstElem_, "muxer", gstMuxer_, nullptr);
    return MSERR_OK;
}

void MuxSinkBin::Dump()
{
    MEDIA_LOGI("file format = %{public}d, max duration = %{public}d, "
               "max size = %{public}" PRId64 ", fd = %{public}d",
               format_, maxDuration_,  maxSize_, outFd_);
}

REGISTER_RECORDER_ELEMENT(MuxSinkBin);
} // namespace Media
} // namespace OHOS
