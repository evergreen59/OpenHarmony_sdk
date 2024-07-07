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

#include "media_client.h"
#include "avmetadatahelper_client.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "ipc_skeleton.h"
#ifdef SUPPORT_RECORDER
#include "i_standard_recorder_service.h"
#endif
#ifdef SUPPORT_PLAYER
#include "i_standard_player_service.h"
#endif
#ifdef SUPPORT_METADATA
#include "i_standard_avmetadatahelper_service.h"
#endif
#ifdef SUPPORT_MUXER
#include "i_standard_avmuxer_service.h"
#endif
#include "media_log.h"
#include "media_errors.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "MediaClient"};
}

namespace OHOS {
namespace Media {
static MediaClient mediaClientInstance;
IMediaService &MediaServiceFactory::GetInstance()
{
    return mediaClientInstance;
}

MediaClient::MediaClient() noexcept
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances create", FAKE_POINTER(this));
}

MediaClient::~MediaClient()
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances destroy", FAKE_POINTER(this));
}

bool MediaClient::IsAlived()
{
    if (mediaProxy_ == nullptr) {
        mediaProxy_ = GetMediaProxy();
    }

    return (mediaProxy_ != nullptr) ? true : false;
}

#ifdef SUPPORT_RECORDER
std::shared_ptr<IRecorderService> MediaClient::CreateRecorderService()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!IsAlived()) {
        MEDIA_LOGE("media service does not exist.");
        return nullptr;
    }

    sptr<IRemoteObject> object = mediaProxy_->GetSubSystemAbility(
        IStandardMediaService::MediaSystemAbility::MEDIA_RECORDER, listenerStub_->AsObject());
    CHECK_AND_RETURN_RET_LOG(object != nullptr, nullptr, "recorder proxy object is nullptr.");

    sptr<IStandardRecorderService> recorderProxy = iface_cast<IStandardRecorderService>(object);
    CHECK_AND_RETURN_RET_LOG(recorderProxy != nullptr, nullptr, "recorder proxy is nullptr.");

    std::shared_ptr<RecorderClient> recorder = RecorderClient::Create(recorderProxy);
    CHECK_AND_RETURN_RET_LOG(recorder != nullptr, nullptr, "failed to create recorder client.");

    recorderClientList_.push_back(recorder);
    return recorder;
}

int32_t MediaClient::DestroyMediaProfileService(std::shared_ptr<IRecorderProfilesService> recorderProfiles)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(recorderProfiles != nullptr, MSERR_NO_MEMORY, "input recorderProfiles is nullptr.");
    recorderProfilesClientList_.remove(recorderProfiles);
    return MSERR_OK;
}

int32_t MediaClient::DestroyRecorderService(std::shared_ptr<IRecorderService> recorder)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(recorder != nullptr, MSERR_NO_MEMORY, "input recorder is nullptr.");
    recorderClientList_.remove(recorder);
    return MSERR_OK;
}

std::shared_ptr<IRecorderProfilesService> MediaClient::CreateRecorderProfilesService()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!IsAlived()) {
        MEDIA_LOGE("media service does not exist.");
        return nullptr;
    }

    sptr<IRemoteObject> object = mediaProxy_->GetSubSystemAbility(
        IStandardMediaService::MediaSystemAbility::RECORDER_PROFILES, listenerStub_->AsObject());
    CHECK_AND_RETURN_RET_LOG(object != nullptr, nullptr, "recorderProfiles proxy object is nullptr.");

    sptr<IStandardRecorderProfilesService> recorderProfilesProxy = iface_cast<IStandardRecorderProfilesService>(object);
    CHECK_AND_RETURN_RET_LOG(recorderProfilesProxy != nullptr, nullptr, "recorderProfiles proxy is nullptr.");

    std::shared_ptr<RecorderProfilesClient> recorderProfiles = RecorderProfilesClient::Create(recorderProfilesProxy);
    CHECK_AND_RETURN_RET_LOG(recorderProfiles != nullptr, nullptr, "failed to create recorderProfiles client.");

    recorderProfilesClientList_.push_back(recorderProfiles);
    return recorderProfiles;
}
#endif

#ifdef SUPPORT_PLAYER
std::shared_ptr<IPlayerService> MediaClient::CreatePlayerService()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!IsAlived()) {
        MEDIA_LOGE("media service does not exist.");
        return nullptr;
    }

    sptr<IRemoteObject> object = mediaProxy_->GetSubSystemAbility(
        IStandardMediaService::MediaSystemAbility::MEDIA_PLAYER, listenerStub_->AsObject());
    CHECK_AND_RETURN_RET_LOG(object != nullptr, nullptr, "player proxy object is nullptr.");

    sptr<IStandardPlayerService> playerProxy = iface_cast<IStandardPlayerService>(object);
    CHECK_AND_RETURN_RET_LOG(playerProxy != nullptr, nullptr, "player proxy is nullptr.");

    std::shared_ptr<PlayerClient> player = PlayerClient::Create(playerProxy);
    CHECK_AND_RETURN_RET_LOG(player != nullptr, nullptr, "failed to create player client.");

    playerClientList_.push_back(player);
    return player;
}

int32_t MediaClient::DestroyPlayerService(std::shared_ptr<IPlayerService> player)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(player != nullptr, MSERR_NO_MEMORY, "input player is nullptr.");
    playerClientList_.remove(player);
    return MSERR_OK;
}
#endif

#ifdef SUPPORT_CODEC
std::shared_ptr<IAVCodecListService> MediaClient::CreateAVCodecListService()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!IsAlived()) {
        MEDIA_LOGE("media service does not exist.");
        return nullptr;
    }

    sptr<IRemoteObject> object = mediaProxy_->GetSubSystemAbility(
        IStandardMediaService::MediaSystemAbility::MEDIA_CODECLIST, listenerStub_->AsObject());
    CHECK_AND_RETURN_RET_LOG(object != nullptr, nullptr, "avcodeclist proxy object is nullptr.");

    sptr<IStandardAVCodecListService> avCodecListProxy = iface_cast<IStandardAVCodecListService>(object);
    CHECK_AND_RETURN_RET_LOG(avCodecListProxy != nullptr, nullptr, "avcodeclist proxy is nullptr.");

    std::shared_ptr<AVCodecListClient> avCodecList = AVCodecListClient::Create(avCodecListProxy);
    CHECK_AND_RETURN_RET_LOG(avCodecList != nullptr, nullptr, "failed to create avcodeclist client.");

    avCodecListClientList_.push_back(avCodecList);
    return avCodecList;
}

int32_t MediaClient::DestroyAVCodecListService(std::shared_ptr<IAVCodecListService> avCodecList)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(avCodecList != nullptr, MSERR_NO_MEMORY, "input avCodecList is nullptr.");
    avCodecListClientList_.remove(avCodecList);
    return MSERR_OK;
}

std::shared_ptr<IAVCodecService> MediaClient::CreateAVCodecService()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!IsAlived()) {
        MEDIA_LOGE("media service does not exist.");
        return nullptr;
    }

    sptr<IRemoteObject> object = mediaProxy_->GetSubSystemAbility(
        IStandardMediaService::MediaSystemAbility::MEDIA_AVCODEC, listenerStub_->AsObject());
    CHECK_AND_RETURN_RET_LOG(object != nullptr, nullptr, "avcodec proxy object is nullptr.");

    sptr<IStandardAVCodecService> avCodecProxy = iface_cast<IStandardAVCodecService>(object);
    CHECK_AND_RETURN_RET_LOG(avCodecProxy != nullptr, nullptr, "avcodec proxy is nullptr.");

    std::shared_ptr<AVCodecClient> avCodec = AVCodecClient::Create(avCodecProxy);
    CHECK_AND_RETURN_RET_LOG(avCodec != nullptr, nullptr, "failed to create avcodec client.");

    avCodecClientList_.push_back(avCodec);
    return avCodec;
}

int32_t MediaClient::DestroyAVCodecService(std::shared_ptr<IAVCodecService> avCodec)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(avCodec != nullptr, MSERR_NO_MEMORY, "input avcodec is nullptr.");
    avCodecClientList_.remove(avCodec);
    return MSERR_OK;
}
#endif

#ifdef SUPPORT_METADATA
std::shared_ptr<IAVMetadataHelperService> MediaClient::CreateAVMetadataHelperService()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!IsAlived()) {
        MEDIA_LOGE("media service does not exist.");
        return nullptr;
    }

    sptr<IRemoteObject> object = mediaProxy_->GetSubSystemAbility(
        IStandardMediaService::MediaSystemAbility::MEDIA_AVMETADATAHELPER, listenerStub_->AsObject());
    CHECK_AND_RETURN_RET_LOG(object != nullptr, nullptr, "avmetadatahelper proxy object is nullptr.");

    sptr<IStandardAVMetadataHelperService> avMetadataHelperProxy = iface_cast<IStandardAVMetadataHelperService>(object);
    CHECK_AND_RETURN_RET_LOG(avMetadataHelperProxy != nullptr, nullptr, "avmetadatahelper proxy is nullptr.");

    std::shared_ptr<AVMetadataHelperClient> avMetadataHelper = AVMetadataHelperClient::Create(avMetadataHelperProxy);
    CHECK_AND_RETURN_RET_LOG(avMetadataHelper != nullptr, nullptr, "failed to create avmetadatahelper client.");

    avMetadataHelperClientList_.push_back(avMetadataHelper);
    return avMetadataHelper;
}

int32_t MediaClient::DestroyAVMetadataHelperService(std::shared_ptr<IAVMetadataHelperService> avMetadataHelper)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(avMetadataHelper != nullptr, MSERR_NO_MEMORY,
        "input avmetadatahelper is nullptr.");
    avMetadataHelperClientList_.remove(avMetadataHelper);
    return MSERR_OK;
}
#endif

#ifdef SUPPORT_MUXER
std::shared_ptr<IAVMuxerService> MediaClient::CreateAVMuxerService()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!IsAlived()) {
        MEDIA_LOGE("media service does not exist.");
        return nullptr;
    }

    sptr<IRemoteObject> object = mediaProxy_->GetSubSystemAbility(
        IStandardMediaService::MediaSystemAbility::MEDIA_AVMUXER, listenerStub_->AsObject());
    CHECK_AND_RETURN_RET_LOG(object != nullptr, nullptr, "avmuxer proxy object is nullptr.");

    sptr<IStandardAVMuxerService> avmuxerProxy = iface_cast<IStandardAVMuxerService>(object);
    CHECK_AND_RETURN_RET_LOG(avmuxerProxy != nullptr, nullptr, "muxer proxy is nullptr.");

    std::shared_ptr<AVMuxerClient> avmuxer = AVMuxerClient::Create(avmuxerProxy);
    CHECK_AND_RETURN_RET_LOG(avmuxer != nullptr, nullptr, "failed to create avmuxer client.");

    avmuxerClientList_.push_back(avmuxer);
    return avmuxer;
}

int32_t MediaClient::DestroyAVMuxerService(std::shared_ptr<IAVMuxerService> avmuxer)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(avmuxer != nullptr, MSERR_NO_MEMORY, "input avmuxer is nullptr.");
    avmuxerClientList_.remove(avmuxer);
    return MSERR_OK;
}
#endif

sptr<IStandardMediaService> MediaClient::GetMediaProxy()
{
    MEDIA_LOGD("enter");
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    CHECK_AND_RETURN_RET_LOG(samgr != nullptr, nullptr, "system ability manager is nullptr.");

    sptr<IRemoteObject> object = samgr->GetSystemAbility(OHOS::PLAYER_DISTRIBUTED_SERVICE_ID);
    CHECK_AND_RETURN_RET_LOG(object != nullptr, nullptr, "media object is nullptr.");

    mediaProxy_ = iface_cast<IStandardMediaService>(object);
    CHECK_AND_RETURN_RET_LOG(mediaProxy_ != nullptr, nullptr, "media proxy is nullptr.");

    pid_t pid = 0;
    deathRecipient_ = new(std::nothrow) MediaDeathRecipient(pid);
    CHECK_AND_RETURN_RET_LOG(deathRecipient_ != nullptr, nullptr, "failed to new MediaDeathRecipient.");

    deathRecipient_->SetNotifyCb(std::bind(&MediaClient::MediaServerDied, std::placeholders::_1));
    bool result = object->AddDeathRecipient(deathRecipient_);
    if (!result) {
        MEDIA_LOGE("failed to add deathRecipient");
        return nullptr;
    }

    listenerStub_ = new(std::nothrow) MediaListenerStub();
    CHECK_AND_RETURN_RET_LOG(listenerStub_ != nullptr, nullptr, "failed to new MediaListenerStub");
    return mediaProxy_;
}

void MediaClient::MediaServerDied(pid_t pid)
{
    MEDIA_LOGE("media server is died, pid:%{public}d!", pid);
    mediaClientInstance.DoMediaServerDied();
}

void MediaClient::DoMediaServerDied()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (mediaProxy_ != nullptr) {
        (void)mediaProxy_->AsObject()->RemoveDeathRecipient(deathRecipient_);
        mediaProxy_ = nullptr;
    }
    listenerStub_ = nullptr;
    deathRecipient_ = nullptr;

#ifdef SUPPORT_RECORDER
    for (auto &it : recorderClientList_) {
        auto recorder = std::static_pointer_cast<RecorderClient>(it);
        if (recorder != nullptr) {
            recorder->MediaServerDied();
        }
    }

    for (auto &it : recorderProfilesClientList_) {
        auto recorderProfilesClient = std::static_pointer_cast<RecorderProfilesClient>(it);
        if (recorderProfilesClient != nullptr) {
            recorderProfilesClient->MediaServerDied();
        }
    }
#endif

#ifdef SUPPORT_PLAYER
    for (auto &it : playerClientList_) {
        auto player = std::static_pointer_cast<PlayerClient>(it);
        if (player != nullptr) {
            player->MediaServerDied();
        }
    }
#endif

#ifdef SUPPORT_METADATA
    for (auto &it : avMetadataHelperClientList_) {
        auto avMetadataHelper = std::static_pointer_cast<AVMetadataHelperClient>(it);
        if (avMetadataHelper != nullptr) {
            avMetadataHelper->MediaServerDied();
        }
    }
#endif

#ifdef SUPPORT_CODEC
    for (auto &it : avCodecClientList_) {
        auto avCodecClient = std::static_pointer_cast<AVCodecClient>(it);
        if (avCodecClient != nullptr) {
            avCodecClient->MediaServerDied();
        }
    }

    for (auto &it : avCodecListClientList_) {
        auto avCodecListClient = std::static_pointer_cast<AVCodecListClient>(it);
        if (avCodecListClient != nullptr) {
            avCodecListClient->MediaServerDied();
        }
    }
#endif

#ifdef SUPPORT_MUXER
    for (auto &it : avmuxerClientList_) {
        auto avmuxer = std::static_pointer_cast<AVMuxerClient>(it);
        if (avmuxer != nullptr) {
            avmuxer->MediaServerDied();
        }
    }
#endif
}
} // namespace Media
} // namespace OHOS
