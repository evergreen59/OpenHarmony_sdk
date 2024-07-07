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

#include "avmetadatahelper_server.h"
#include "media_log.h"
#include "media_errors.h"
#include "engine_factory_repo.h"
#include "uri_helper.h"
#include "media_dfx.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "AVMetadataHelperServer"};
}

namespace OHOS {
namespace Media {
std::shared_ptr<IAVMetadataHelperService> AVMetadataHelperServer::Create()
{
    std::shared_ptr<AVMetadataHelperServer> server = std::make_shared<AVMetadataHelperServer>();
    CHECK_AND_RETURN_RET_LOG(server != nullptr, nullptr, "Failed to new AVMetadataHelperServer");
    return server;
}

AVMetadataHelperServer::AVMetadataHelperServer()
    : taskQue_("AVMetadata")
{
    (void)taskQue_.Start();
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances create", FAKE_POINTER(this));
}

AVMetadataHelperServer::~AVMetadataHelperServer()
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances destroy", FAKE_POINTER(this));
    std::lock_guard<std::mutex> lock(mutex_);
    auto task = std::make_shared<TaskHandler<void>>([&, this] {
        avMetadataHelperEngine_ = nullptr;
    });
    (void)taskQue_.EnqueueTask(task);
    (void)task->GetResult();
    uriHelper_ = nullptr;
    taskQue_.Stop();
}

int32_t AVMetadataHelperServer::SetSource(const std::string &uri, int32_t usage)
{
    std::lock_guard<std::mutex> lock(mutex_);
    MediaTrace trace("AVMetadataHelperServer::SetSource_uri");
    MEDIA_LOGD("Current uri is : %{public}s %{public}u", uri.c_str(), usage);

    uriHelper_ = std::make_unique<UriHelper>(uri);
    if (!uriHelper_->AccessCheck(UriHelper::URI_READ)) {
        MEDIA_LOGE("Failed to read the file");
        return MSERR_INVALID_VAL;
    }
    auto task = std::make_shared<TaskHandler<int32_t>>([&, this] {
        auto engineFactory = EngineFactoryRepo::Instance().GetEngineFactory(
            IEngineFactory::Scene::SCENE_AVMETADATA, uriHelper_->FormattedUri());
        CHECK_AND_RETURN_RET_LOG(engineFactory != nullptr, (int32_t)MSERR_CREATE_AVMETADATAHELPER_ENGINE_FAILED,
            "Failed to get engine factory");
        avMetadataHelperEngine_ = engineFactory->CreateAVMetadataHelperEngine();
        CHECK_AND_RETURN_RET_LOG(avMetadataHelperEngine_ != nullptr,
            (int32_t)MSERR_CREATE_AVMETADATAHELPER_ENGINE_FAILED, "Failed to create avmetadatahelper engine");
        int32_t ret = avMetadataHelperEngine_->SetSource(uriHelper_->FormattedUri(), usage);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, ret, "SetSource failed!");
        return ret;
    });
    int32_t ret = taskQue_.EnqueueTask(task);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, ret, "EnqueueTask failed");

    auto result = task->GetResult();
    return result.Value();
}

int32_t AVMetadataHelperServer::SetSource(int32_t fd, int64_t offset, int64_t size, int32_t usage)
{
    std::lock_guard<std::mutex> lock(mutex_);
    MediaTrace trace("AVMetadataHelperServer::SetSource_fd");
    MEDIA_LOGD("Current is fd source, offset: %{public}" PRIi64 ", size: %{public}" PRIi64 " usage: %{public}u",
               offset, size, usage);

    uriHelper_ = std::make_unique<UriHelper>(fd, offset, size);
    CHECK_AND_RETURN_RET_LOG(uriHelper_->AccessCheck(UriHelper::URI_READ), MSERR_INVALID_VAL, "Failed to read the fd");

    auto task = std::make_shared<TaskHandler<int32_t>>([&, this] {
        auto engineFactory = EngineFactoryRepo::Instance().GetEngineFactory(
            IEngineFactory::Scene::SCENE_AVMETADATA, uriHelper_->FormattedUri());
        CHECK_AND_RETURN_RET_LOG(engineFactory != nullptr, (int32_t)MSERR_CREATE_AVMETADATAHELPER_ENGINE_FAILED,
            "Failed to get engine factory");
        avMetadataHelperEngine_ = engineFactory->CreateAVMetadataHelperEngine();
        CHECK_AND_RETURN_RET_LOG(avMetadataHelperEngine_ != nullptr,
            (int32_t)MSERR_CREATE_AVMETADATAHELPER_ENGINE_FAILED, "Failed to create avmetadatahelper engine");

        int32_t ret = avMetadataHelperEngine_->SetSource(uriHelper_->FormattedUri(), usage);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, ret, "SetSource failed!");
        return ret;
    });
    int32_t ret = taskQue_.EnqueueTask(task);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, ret, "EnqueueTask failed");

    auto result = task->GetResult();
    return result.Value();
}

std::string AVMetadataHelperServer::ResolveMetadata(int32_t key)
{
    std::lock_guard<std::mutex> lock(mutex_);
    MediaTrace trace("AVMetadataHelperServer::ResolveMetadata_key");
    MEDIA_LOGD("Key is %{public}d", key);
    CHECK_AND_RETURN_RET_LOG(avMetadataHelperEngine_ != nullptr, "", "avMetadataHelperEngine_ is nullptr");
    auto task = std::make_shared<TaskHandler<std::string>>([&, this] {
        return avMetadataHelperEngine_->ResolveMetadata(key);
    });
    int32_t ret = taskQue_.EnqueueTask(task);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, "", "EnqueueTask failed");

    auto result = task->GetResult();
    return result.Value();
}

std::unordered_map<int32_t, std::string> AVMetadataHelperServer::ResolveMetadata()
{
    std::lock_guard<std::mutex> lock(mutex_);
    MediaTrace trace("AVMetadataHelperServer::ResolveMetadata");
    CHECK_AND_RETURN_RET_LOG(avMetadataHelperEngine_ != nullptr, {}, "avMetadataHelperEngine_ is nullptr");
    auto task = std::make_shared<TaskHandler<std::unordered_map<int32_t, std::string>>>([&, this] {
        return avMetadataHelperEngine_->ResolveMetadata();
    });
    int32_t ret = taskQue_.EnqueueTask(task);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, {}, "EnqueueTask failed");

    auto result = task->GetResult();
    return result.Value();
}

std::shared_ptr<AVSharedMemory> AVMetadataHelperServer::FetchArtPicture()
{
    std::lock_guard<std::mutex> lock(mutex_);
    MediaTrace trace("AVMetadataHelperServer::FetchArtPicture");
    CHECK_AND_RETURN_RET_LOG(avMetadataHelperEngine_ != nullptr, {}, "avMetadataHelperEngine_ is nullptr");
    auto task = std::make_shared<TaskHandler<std::shared_ptr<AVSharedMemory>>>([&, this] {
        return avMetadataHelperEngine_->FetchArtPicture();
    });
    int32_t ret = taskQue_.EnqueueTask(task);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, nullptr, "EnqueueTask failed");

    auto result = task->GetResult();
    return result.Value();
}

std::shared_ptr<AVSharedMemory> AVMetadataHelperServer::FetchFrameAtTime(int64_t timeUs, int32_t option,
    const OutputConfiguration &param)
{
    std::lock_guard<std::mutex> lock(mutex_);
    MediaTrace trace("AVMetadataHelperServer::FetchFrameAtTime");
    CHECK_AND_RETURN_RET_LOG(avMetadataHelperEngine_ != nullptr, nullptr, "avMetadataHelperEngine_ is nullptr");
    auto task = std::make_shared<TaskHandler<std::shared_ptr<AVSharedMemory>>>([&, this] {
        return avMetadataHelperEngine_->FetchFrameAtTime(timeUs, option, param);
    });
    int32_t ret = taskQue_.EnqueueTask(task);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, nullptr, "EnqueueTask failed");

    auto result = task->GetResult();
    return result.Value();
}

void AVMetadataHelperServer::Release()
{
    std::lock_guard<std::mutex> lock(mutex_);
    MediaTrace trace("AVMetadataHelperServer::Release");
    auto task = std::make_shared<TaskHandler<void>>([&, this] {
        avMetadataHelperEngine_ = nullptr;
    });
    (void)taskQue_.EnqueueTask(task);
    (void)task->GetResult();
    uriHelper_ = nullptr;
}
} // namespace Media
} // namespace OHOS
