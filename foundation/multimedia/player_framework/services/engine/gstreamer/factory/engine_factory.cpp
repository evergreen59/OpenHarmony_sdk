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

#include "i_engine_factory.h"
#include "avcodec_engine_gst_impl.h"
#include "avcodeclist_engine_gst_impl.h"
#include "avmetadatahelper_engine_gst_impl.h"
#include "gst_loader.h"
#include "media_errors.h"
#include "media_log.h"
#include "nocopyable.h"
#include "player_engine_gst_impl.h"
#include "recorder_engine_gst_impl.h"
#include "avmuxer_engine_gst_impl.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "GstEngineFactory"};
}

namespace OHOS {
namespace Media {
class GstEngineFactory : public IEngineFactory, public NoCopyable {
public:
    GstEngineFactory() = default;
    ~GstEngineFactory() = default;

    int32_t Score(Scene scene, const std::string &uri) override;
#ifdef SUPPORT_PLAYER
    std::unique_ptr<IPlayerEngine> CreatePlayerEngine(int32_t uid = 0, int32_t pid = 0) override;
#endif
#ifdef SUPPORT_RECORDER
    std::unique_ptr<IRecorderEngine> CreateRecorderEngine(int32_t appUid, int32_t appPid, uint32_t appTokenId) override;
#endif
#ifdef SUPPORT_METADATA
    std::unique_ptr<IAVMetadataHelperEngine> CreateAVMetadataHelperEngine() override;
#endif
#ifdef SUPPORT_CODEC
    std::unique_ptr<IAVCodecEngine> CreateAVCodecEngine() override;
    std::unique_ptr<IAVCodecListEngine> CreateAVCodecListEngine() override;
#endif
#ifdef SUPPORT_MUXER
    std::unique_ptr<IAVMuxerEngine> CreateAVMuxerEngine() override;
#endif
};

int32_t GstEngineFactory::Score(Scene scene, const std::string &uri)
{
    (void)uri;
    (void)scene;
    return MIN_SCORE + 1;
}

#ifdef SUPPORT_PLAYER
std::unique_ptr<IPlayerEngine> GstEngineFactory::CreatePlayerEngine(int32_t uid, int32_t pid)
{
    GstLoader::Instance().UpdateLogLevel();
    return std::make_unique<PlayerEngineGstImpl>(uid, pid);
}
#endif

#ifdef SUPPORT_METADATA
std::unique_ptr<IAVMetadataHelperEngine> GstEngineFactory::CreateAVMetadataHelperEngine()
{
    GstLoader::Instance().UpdateLogLevel();
    return std::make_unique<AVMetadataHelperEngineGstImpl>();
}
#endif

#ifdef SUPPORT_RECORDER
std::unique_ptr<IRecorderEngine> GstEngineFactory::CreateRecorderEngine(
    int32_t appUid, int32_t appPid, uint32_t appTokenId)
{
    GstLoader::Instance().UpdateLogLevel();
    auto engine = std::make_unique<RecorderEngineGstImpl>(appUid, appPid, appTokenId);
    int32_t ret = engine->Init();
    if (ret != MSERR_OK) {
        MEDIA_LOGE("recorder engine init failed, ret = %{public}d", ret);
        return nullptr;
    }
    return engine;
}
#endif

#ifdef SUPPORT_CODEC
std::unique_ptr<IAVCodecEngine> GstEngineFactory::CreateAVCodecEngine()
{
    GstLoader::Instance().UpdateLogLevel();
    return std::make_unique<AVCodecEngineGstImpl>();
}

std::unique_ptr<IAVCodecListEngine> GstEngineFactory::CreateAVCodecListEngine()
{
    GstLoader::Instance().UpdateLogLevel();
    return std::make_unique<AVCodecListEngineGstImpl>();
}
#endif

#ifdef SUPPORT_MUXER
std::unique_ptr<IAVMuxerEngine> GstEngineFactory::CreateAVMuxerEngine()
{
    GstLoader::Instance().UpdateLogLevel();
    return std::make_unique<AVMuxerEngineGstImpl>();
}
#endif
} // namespace Media
} // namespace OHOS

#ifdef __cplusplus
extern "C" {
#endif
__attribute__((visibility("default"))) OHOS::Media::IEngineFactory *CreateEngineFactory()
{
    int32_t ret = OHOS::Media::GstLoader::Instance().SetUp();
    if (ret != OHOS::Media::MSERR_OK) {
        MEDIA_LOGE("Gst Engine setup failed, ret = %{public}d", ret);
        return nullptr;
    }
    return new (std::nothrow) OHOS::Media::GstEngineFactory();
}
#ifdef __cplusplus
}
#endif