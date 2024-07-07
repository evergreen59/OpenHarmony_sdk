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

#include "hdi_init.h"
#include <hdf_base.h>
#include "media_log.h"
#include "media_errors.h"
#include "display_type.h"
#include "scope_guard.h"
#include "hdi_codec_util.h"
#include "hdf_remote_service.h"
#include "codec_internal.h"
#include "servmgr_hdi.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "HdiInit"};
    using namespace OHOS::Media;
    const std::unordered_map<int32_t, int32_t> AVC_PROFILE_MAP = {
        {OMX_VIDEO_AVCProfileBaseline, AVC_PROFILE_BASELINE},
        {OMX_VIDEO_AVCProfileMain, AVC_PROFILE_MAIN},
        {OMX_VIDEO_AVCProfileHigh, AVC_PROFILE_HIGH},
        {OMX_VIDEO_AVCProfileExtended, AVC_PROFILE_EXTENDED},
    };

    const std::unordered_map<int32_t, int32_t> HEVC_PROFILE_MAP = {
        {CODEC_HEVC_PROFILE_MAIN, HEVC_PROFILE_MAIN},
        {CODEC_HEVC_PROFILE_MAIN10, HEVC_PROFILE_MAIN_10},
        {CODEC_HEVC_PROFILE_MAIN_STILL, HEVC_PROFILE_MAIN_STILL},
        {CODEC_HEVC_PROFILE_MAIN10_HDR10, HEVC_PROFILE_MAIN_10_HDR10},
    };

    const std::unordered_map<int32_t, int32_t> AVC_LEVEL_MAP = {
        {OMX_VIDEO_AVCLevel1, AVC_LEVEL_1},
        {OMX_VIDEO_AVCLevel1b, AVC_LEVEL_1b},
        {OMX_VIDEO_AVCLevel11, AVC_LEVEL_11},
        {OMX_VIDEO_AVCLevel12, AVC_LEVEL_12},
        {OMX_VIDEO_AVCLevel13, AVC_LEVEL_13},
        {OMX_VIDEO_AVCLevel2, AVC_LEVEL_2},
        {OMX_VIDEO_AVCLevel21, AVC_LEVEL_21},
        {OMX_VIDEO_AVCLevel22, AVC_LEVEL_22},
        {OMX_VIDEO_AVCLevel3, AVC_LEVEL_3},
        {OMX_VIDEO_AVCLevel31, AVC_LEVEL_31},
        {OMX_VIDEO_AVCLevel32, AVC_LEVEL_32},
        {OMX_VIDEO_AVCLevel4, AVC_LEVEL_4},
        {OMX_VIDEO_AVCLevel41, AVC_LEVEL_41},
        {OMX_VIDEO_AVCLevel42, AVC_LEVEL_42},
        {OMX_VIDEO_AVCLevel5, AVC_LEVEL_5},
        {OMX_VIDEO_AVCLevel51, AVC_LEVEL_51},
    };

    const std::unordered_map<int32_t, int32_t> HEVC_LEVEL_MAP = {
        {CODEC_HEVC_MAIN_TIER_LEVEL1, HEVC_LEVEL_1},
        {CODEC_HEVC_MAIN_TIER_LEVEL2, HEVC_LEVEL_2},
        {CODEC_HEVC_MAIN_TIER_LEVEL21, HEVC_LEVEL_21},
        {CODEC_HEVC_MAIN_TIER_LEVEL3, HEVC_LEVEL_3},
        {CODEC_HEVC_MAIN_TIER_LEVEL31, HEVC_LEVEL_31},
        {CODEC_HEVC_MAIN_TIER_LEVEL4, HEVC_LEVEL_4},
        {CODEC_HEVC_MAIN_TIER_LEVEL41, HEVC_LEVEL_41},
        {CODEC_HEVC_MAIN_TIER_LEVEL5, HEVC_LEVEL_5},
        {CODEC_HEVC_MAIN_TIER_LEVEL51, HEVC_LEVEL_51},
        {CODEC_HEVC_MAIN_TIER_LEVEL52, HEVC_LEVEL_52},
        {CODEC_HEVC_MAIN_TIER_LEVEL6, HEVC_LEVEL_6},
        {CODEC_HEVC_MAIN_TIER_LEVEL61, HEVC_LEVEL_61},
        {CODEC_HEVC_MAIN_TIER_LEVEL62, HEVC_LEVEL_62},
    };
    constexpr int32_t MAX_COMPONENT_NUM = 1024;
}

namespace OHOS {
namespace Media {
const std::unordered_map<int32_t, HdiInit::GetProfileLevelsFunc> HdiInit::PROFILE_LEVEL_FUNC_MAP = {
    {MEDIA_ROLETYPE_VIDEO_AVC, HdiInit::GetH264ProfileLevels},
    {MEDIA_ROLETYPE_VIDEO_HEVC, HdiInit::GetH265ProfileLevels},
};

HdiInit &HdiInit::GetInstance()
{
    static HdiInit omx;
    return omx;
}

HdiInit::HdiInit()
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances create", FAKE_POINTER(this));
    CodecComponentManagerInit();
}

HdiInit::~HdiInit()
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances destroy", FAKE_POINTER(this));
    CodecComponentManagerRelease();
}

static void HdiCodecOnRemoteDied(HdfDeathRecipient *deathRecipient, HdfRemoteService *remote)
{
    (void)deathRecipient;
    (void)remote;
    HdiInit::GetInstance().CodecComponentManagerReset();
}

void HdiInit::CodecComponentManagerInit()
{
    MEDIA_LOGD("CodecComponentManagerInit In");

    mgr_ = GetCodecComponentManager();
    CHECK_AND_RETURN_LOG(mgr_ != nullptr, "GetCodecComponentManager failed");

    HDIServiceManager *serviceMgr = HDIServiceManagerGet();
    CHECK_AND_RETURN_LOG(serviceMgr != nullptr, "HDIServiceManagerGet failed");

    HdfRemoteService *remoteOmx = serviceMgr->GetService(serviceMgr, CODEC_HDI_OMX_SERVICE_NAME);
    HDIServiceManagerRelease(serviceMgr);
    CHECK_AND_RETURN_LOG(remoteOmx != nullptr, "HDIServiceManagerGet failed");

    static HdfDeathRecipient recipient = {
        .OnRemoteDied = HdiCodecOnRemoteDied,
    };

    HdfRemoteServiceAddDeathRecipient(remoteOmx, &recipient);

    MEDIA_LOGD("CodecComponentManagerInit End");
}

void HdiInit::CodecComponentManagerReset()
{
    MEDIA_LOGD("CodecComponentManagerReset In");

    std::lock_guard<std::mutex> lock(mutex_);
    CodecComponentManagerRelease();
    mgr_ = nullptr;
    handleMap_.clear();

    MEDIA_LOGD("CodecComponentManagerReset End");
}

int32_t HdiInit::GetCodecType(CodecType hdiType)
{
    switch (hdiType) {
        case VIDEO_DECODER:
            return AVCODEC_TYPE_VIDEO_DECODER;
        case VIDEO_ENCODER:
            return AVCODEC_TYPE_VIDEO_ENCODER;
        case AUDIO_DECODER:
            return AVCODEC_TYPE_AUDIO_DECODER;
        case AUDIO_ENCODER:
            return AVCODEC_TYPE_AUDIO_ENCODER;
        default:
            MEDIA_LOGW("Unknow codecType");
            break;
    }
    return AVCODEC_TYPE_NONE;
}

std::string HdiInit::GetCodecMime(AvCodecRole &role)
{
    switch (role) {
        case MEDIA_ROLETYPE_VIDEO_AVC:
            return "video/avc";
        case MEDIA_ROLETYPE_VIDEO_HEVC:
            return "video/hevc";
        default:
            MEDIA_LOGW("Unknow codecRole %{public}d", (int32_t)role);
            break;
    }
    return "invalid";
}

std::vector<int32_t> HdiInit::GetBitrateMode(CodecVideoPortCap &port)
{
    int32_t index = 0;
    std::vector<int32_t> bitrate;
    while (index < BIT_RATE_MODE_NUM && port.bitRatemode[index] != BIT_RATE_MODE_INVALID) {
        switch (port.bitRatemode[index]) {
            case BIT_RATE_MODE_VBR:
                bitrate.push_back(VBR);
                break;
            case BIT_RATE_MODE_CBR:
                bitrate.push_back(CBR);
                break;
            case BIT_RATE_MODE_CQ:
                bitrate.push_back(CQ);
                break;
            default:
                MEDIA_LOGW("Unknow bitrate mode %{public}d", port.bitRatemode[index]);
        }
        index++;
    }
    return bitrate;
}

std::map<ImgSize, Range> HdiInit::GetMeasuredFrameRate(CodecVideoPortCap &port)
{
    int32_t index = 0;
    std::map<ImgSize, Range> rateMap;
    const int32_t measureStep = 4;
    // measuredFrameRate is 0 width 1 height 2 minRate 3 maxRate. So the step is 4.
    while (index < MEASURED_FRAME_RATE_NUM && port.measuredFrameRate[index] > 0) {
        ImgSize imageSize(port.measuredFrameRate[index], port.measuredFrameRate[index + 1]);
        Range range(port.measuredFrameRate[index + 2], port.measuredFrameRate[index + 3]);
        rateMap[imageSize] = range;
        index += measureStep;
    }
    return rateMap;
}

std::vector<int32_t> HdiInit::GetCodecFormats(CodecVideoPortCap &port)
{
    int32_t index = 0;
    std::vector<int32_t> formats;
    while (index < PIX_FORMAT_NUM && port.supportPixFmts[index] > 0) {
        switch (port.supportPixFmts[index]) {
            case PIXEL_FMT_YCBCR_420_SP:
                formats.push_back(NV12);
                break;
            case PIXEL_FMT_YCRCB_420_SP:
                formats.push_back(NV21);
                break;
            case PIXEL_FMT_YCBCR_420_P:
                formats.push_back(YUVI420);
                break;
            case PIXEL_FMT_RGBA_8888:
                formats.push_back(RGBA);
                break;
            default:
                MEDIA_LOGW("Unknow Format %{public}d", port.supportPixFmts[index]);
        }
        index++;
    }

    return formats;
}

std::map<int32_t, std::vector<int32_t>> HdiInit::GetH264ProfileLevels(CodecCompCapability &hdiCap)
{
    std::map<int32_t, std::vector<int32_t>> profileLevelsMap;
    int32_t index = 0;
    while (index < PROFILE_NUM && hdiCap.supportProfiles[index] > 0) {
        if (AVC_PROFILE_MAP.find(hdiCap.supportProfiles[index]) == AVC_PROFILE_MAP.end()) {
            MEDIA_LOGW("Unknow AVC profile %{public}d", hdiCap.supportProfiles[index]);
            break;
        }
        int32_t profile = AVC_PROFILE_MAP.at(hdiCap.supportProfiles[index]);
        if (profileLevelsMap.find(profile) == profileLevelsMap.end()) {
            profileLevelsMap[profile] = std::vector<int32_t>();
        }
        index++;
        if (AVC_LEVEL_MAP.find(hdiCap.supportProfiles[index]) == AVC_LEVEL_MAP.end()) {
            MEDIA_LOGW("Unknow AVC level %{public}d", hdiCap.supportProfiles[index]);
            break;
        }
        profileLevelsMap[profile].push_back(AVC_LEVEL_MAP.at(hdiCap.supportProfiles[index]));
        index++;
    }

    return profileLevelsMap;
}

std::map<int32_t, std::vector<int32_t>> HdiInit::GetH265ProfileLevels(CodecCompCapability &hdiCap)
{
    std::map<int32_t, std::vector<int32_t>> profileLevelsMap;
    int32_t index = 0;
    std::vector<int32_t> formats;
    while (index < PROFILE_NUM && hdiCap.supportProfiles[index] > 0) {
        if (HEVC_PROFILE_MAP.find(hdiCap.supportProfiles[index]) == HEVC_PROFILE_MAP.end()) {
            MEDIA_LOGW("Unknow HEVC profile %{public}d", hdiCap.supportProfiles[index]);
            break;
        }
        int32_t profile = HEVC_PROFILE_MAP.at(hdiCap.supportProfiles[index]);
        if (profileLevelsMap.find(profile) == profileLevelsMap.end()) {
            profileLevelsMap[profile] = std::vector<int32_t>();
        }
        index++;
        if (HEVC_LEVEL_MAP.find(hdiCap.supportProfiles[index]) == HEVC_LEVEL_MAP.end()) {
            MEDIA_LOGW("Unknow HEVC level %{public}d", hdiCap.supportProfiles[index]);
            break;
        }
        profileLevelsMap[profile].push_back(HEVC_LEVEL_MAP.at(hdiCap.supportProfiles[index]));
        index++;
    }

    return profileLevelsMap;
}

std::map<int32_t, std::vector<int32_t>> HdiInit::GetCodecProfileLevels(CodecCompCapability &hdiCap)
{
    if (PROFILE_LEVEL_FUNC_MAP.find(hdiCap.role) == PROFILE_LEVEL_FUNC_MAP.end()) {
        MEDIA_LOGW("Unknow role %{public}d", hdiCap.role);
        return std::map<int32_t, std::vector<int32_t>>();
    }
    return PROFILE_LEVEL_FUNC_MAP.at(hdiCap.role)(hdiCap);
}

void HdiInit::AddHdiCap(CodecCompCapability &hdiCap)
{
    MEDIA_LOGI("Add codec name %{public}s", hdiCap.compName);
    CapabilityData codecCap;
    codecCap.codecName = hdiCap.compName;
    codecCap.codecType = GetCodecType(hdiCap.type);
    codecCap.mimeType = GetCodecMime(hdiCap.role);
    codecCap.isVendor = !hdiCap.isSoftwareCodec;
    codecCap.alignment = {hdiCap.port.video.whAlignment.widthAlignment, hdiCap.port.video.whAlignment.heightAlignment};
    codecCap.bitrateMode = GetBitrateMode(hdiCap.port.video);
    codecCap.width = {hdiCap.port.video.minSize.width, hdiCap.port.video.maxSize.width};
    codecCap.height = {hdiCap.port.video.minSize.height, hdiCap.port.video.maxSize.height};
    codecCap.bitrate = {hdiCap.bitRate.min, hdiCap.bitRate.max};
    codecCap.frameRate = {hdiCap.port.video.frameRate.min, hdiCap.port.video.frameRate.max};
    codecCap.format = GetCodecFormats(hdiCap.port.video);
    codecCap.blockPerFrame = {hdiCap.port.video.blockCount.min, hdiCap.port.video.blockCount.max};
    codecCap.blockPerSecond = {hdiCap.port.video.blocksPerSecond.min, hdiCap.port.video.blocksPerSecond.max};
    codecCap.blockSize = {hdiCap.port.video.blockSize.width, hdiCap.port.video.blockSize.height};
    codecCap.measuredFrameRate = GetMeasuredFrameRate(hdiCap.port.video);
    codecCap.profileLevelsMap = GetCodecProfileLevels(hdiCap);
    capabilitys_.push_back(codecCap);
}

void HdiInit::InitCaps()
{
    if (!capabilitys_.empty()) {
        return;
    }
    auto len = mgr_->GetComponentNum();
    if (len >= MAX_COMPONENT_NUM || len <= 0) {
        MEDIA_LOGW("Component num is %{public}d", len);
        return;
    }
    CodecCompCapability *hdiCaps = new CodecCompCapability[len];
    CHECK_AND_RETURN_LOG(hdiCaps != nullptr, "New CodecCompCapability fail");
    ON_SCOPE_EXIT(0) { delete[] hdiCaps; };
    auto ret = mgr_->GetComponentCapabilityList(hdiCaps, len);
    CHECK_AND_RETURN_LOG(ret == HDF_SUCCESS, "GetComponentCapabilityList fail");
    for (auto i = 0; i < len; ++i) {
        AddHdiCap(hdiCaps[i]);
    }
}

std::vector<CapabilityData> HdiInit::GetCapabilitys()
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(mgr_ != nullptr, capabilitys_, "mgr is nullptr");
    InitCaps();
    return capabilitys_;
}

int32_t HdiInit::GetHandle(CodecComponentType **component, uint32_t &id, std::string name,
    void *appData, CodecCallbackType *callbacks)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (mgr_ == nullptr) {
        CodecComponentManagerInit();
        CHECK_AND_RETURN_RET_LOG(mgr_ != nullptr, HDF_FAILURE, "mgr is nullptr");
    }
    
    CHECK_AND_RETURN_RET_LOG(component != nullptr, HDF_FAILURE, "component is nullptr");
    int32_t ret = mgr_->CreateComponent(component, &id, const_cast<char *>(name.c_str()),
        reinterpret_cast<int64_t>(appData), callbacks);
    if (ret == HDF_SUCCESS) {
        handleMap_[*component] = id;
    }

    return ret;
}

int32_t HdiInit::FreeHandle(CodecComponentType *component, uint32_t id)
{
    CHECK_AND_RETURN_RET_LOG(mgr_ != nullptr, HDF_FAILURE, "mgr_ is nullptr");

    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = handleMap_.find(component);
    CHECK_AND_RETURN_RET_LOG(iter != handleMap_.end(), HDF_SUCCESS, "The handle has been released!");
    CHECK_AND_RETURN_RET_LOG(iter->second == id, HDF_FAILURE, "Handle and id do not match!");
    
    int32_t ret =  mgr_->DestroyComponent(id);
    if (ret == HDF_SUCCESS) {
        handleMap_.erase(iter);
    }

    return ret;
}
}  // namespace Media
}  // namespace OHOS
