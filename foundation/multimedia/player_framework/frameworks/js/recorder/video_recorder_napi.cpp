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

#include "video_recorder_napi.h"
#include "recorder_callback_napi.h"
#include "media_log.h"
#include "media_errors.h"
#include "common_napi.h"
#include "directory_ex.h"
#include "string_ex.h"
#include "surface_utils.h"
#include "recorder_napi_utils.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "VideoRecorderNapi"};
}

namespace OHOS {
namespace Media {
thread_local napi_ref VideoRecorderNapi::constructor_ = nullptr;
const std::string CLASS_NAME = "VideoRecorder";

VideoRecorderNapi::VideoRecorderNapi()
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR "Instances create", FAKE_POINTER(this));
}

VideoRecorderNapi::~VideoRecorderNapi()
{
    CancelCallback();
    recorder_ = nullptr;
    callbackNapi_ = nullptr;
    MEDIA_LOGD("0x%{public}06" PRIXPTR "Instances destroy", FAKE_POINTER(this));
}

napi_value VideoRecorderNapi::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("prepare", Prepare),
        DECLARE_NAPI_FUNCTION("getInputSurface", GetInputSurface),
        DECLARE_NAPI_FUNCTION("start", Start),
        DECLARE_NAPI_FUNCTION("pause", Pause),
        DECLARE_NAPI_FUNCTION("resume", Resume),
        DECLARE_NAPI_FUNCTION("stop", Stop),
        DECLARE_NAPI_FUNCTION("reset", Reset),
        DECLARE_NAPI_FUNCTION("release", Release),
        DECLARE_NAPI_FUNCTION("on", On),

        DECLARE_NAPI_GETTER("state", GetState),
    };

    napi_property_descriptor staticProperty[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createVideoRecorder", CreateVideoRecorder),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    CHECK_AND_RETURN_RET_LOG(status == napi_ok, nullptr, "Failed to define VideoRecorder class");

    status = napi_create_reference(env, constructor, 1, &constructor_);
    CHECK_AND_RETURN_RET_LOG(status == napi_ok, nullptr, "Failed to create reference of constructor");

    status = napi_set_named_property(env, exports, CLASS_NAME.c_str(), constructor);
    CHECK_AND_RETURN_RET_LOG(status == napi_ok, nullptr, "Failed to set constructor");

    status = napi_define_properties(env, exports, sizeof(staticProperty) / sizeof(staticProperty[0]), staticProperty);
    CHECK_AND_RETURN_RET_LOG(status == napi_ok, nullptr, "Failed to define static function");

    MEDIA_LOGD("Init success");

    return exports;
}

napi_value VideoRecorderNapi::Constructor(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    napi_value jsThis = nullptr;
    size_t argCount = 0;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        MEDIA_LOGE ("Failed to retrieve details about the callback");
        return result;
    }

    VideoRecorderNapi *recorderNapi = new(std::nothrow) VideoRecorderNapi();
    CHECK_AND_RETURN_RET_LOG(recorderNapi != nullptr, result, "No memory!");

    recorderNapi->env_ = env;
    recorderNapi->recorder_ = RecorderFactory::CreateRecorder();
    CHECK_AND_RETURN_RET_LOG(recorderNapi->recorder_ != nullptr, result, "failed to CreateRecorder");

    if (recorderNapi->callbackNapi_ == nullptr && recorderNapi->recorder_ != nullptr) {
        recorderNapi->callbackNapi_ = std::make_shared<RecorderCallbackNapi>(env, true);
        (void)recorderNapi->recorder_->SetRecorderCallback(recorderNapi->callbackNapi_);
    }

    status = napi_wrap(env, jsThis, reinterpret_cast<void *>(recorderNapi),
        VideoRecorderNapi::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete recorderNapi;
        MEDIA_LOGE("Failed to warp native instance!");
        return result;
    }

    MEDIA_LOGD("Constructor success");
    return jsThis;
}

void VideoRecorderNapi::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)env;
    (void)finalize;
    if (nativeObject != nullptr) {
        VideoRecorderNapi *napi = reinterpret_cast<VideoRecorderNapi *>(nativeObject);
        if (napi->surface_ != nullptr) {
            auto id = napi->surface_->GetUniqueId();
            if (napi->IsSurfaceIdVaild(id)) {
                (void)SurfaceUtils::GetInstance()->Remove(id);
            }
        }
        delete napi;
    }
    MEDIA_LOGD("Destructor success");
}

napi_value VideoRecorderNapi::CreateVideoRecorder(napi_env env, napi_callback_info info)
{
    MEDIA_LOGD("CreateVideoRecorder In");

    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    // get args
    napi_value jsThis = nullptr;
    napi_value args[1] = { nullptr };
    size_t argCount = 1;
    napi_status status = napi_get_cb_info(env, info, &argCount, args, &jsThis, nullptr);
    CHECK_AND_RETURN_RET_LOG(status == napi_ok, result, "failed to napi_get_cb_info");

    std::unique_ptr<VideoRecorderAsyncContext> asyncCtx = std::make_unique<VideoRecorderAsyncContext>(env);

    asyncCtx->callbackRef = CommonNapi::CreateReference(env, args[0]);
    asyncCtx->deferred = CommonNapi::CreatePromise(env, asyncCtx->callbackRef, result);
    asyncCtx->JsResult = std::make_unique<MediaJsResultInstance>(constructor_);
    asyncCtx->ctorFlag = true;

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "createVideoRecorder", NAPI_AUTO_LENGTH, &resource);
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resource, [](napi_env env, void* data) {},
        MediaAsyncContext::CompleteCallback, static_cast<void *>(asyncCtx.get()), &asyncCtx->work));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCtx->work));
    asyncCtx.release();

    return result;
}

static void SignError(VideoRecorderAsyncContext *asyncCtx, int32_t code,
    const std::string &param1, const std::string &param2)
{
    std::string message = MSExtErrorAPI9ToString(static_cast<MediaServiceExtErrCodeAPI9>(code), param1, param2);
    asyncCtx->SignError(code, message);
}

napi_value VideoRecorderNapi::Prepare(napi_env env, napi_callback_info info)
{
    MEDIA_LOGD("Prepare In");

    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_value jsThis = nullptr;
    napi_value args[2] = { nullptr };

    auto asyncCtx = std::make_unique<VideoRecorderAsyncContext>(env);

    // get args
    size_t argCount = 2;
    napi_status status = napi_get_cb_info(env, info, &argCount, args, &jsThis, nullptr);
    if (status != napi_ok || jsThis == nullptr) {
        SignError(asyncCtx.get(), MSERR_EXT_API9_INVALID_PARAMETER, "prepare", "");
    }

    // get recordernapi
    (void)napi_unwrap(env, jsThis, reinterpret_cast<void **>(&asyncCtx->napi));

    // get param
    napi_valuetype valueType = napi_undefined;
    if (args[0] == nullptr || napi_typeof(env, args[0], &valueType) != napi_ok || valueType != napi_object) {
        SignError(asyncCtx.get(), MSERR_EXT_API9_INVALID_PARAMETER, "prepare", "");
    }

    std::string urlPath = CommonNapi::GetPropertyString(env, args[0], "url");

    VideoRecorderProperties videoProperties;

    asyncCtx->napi->GetConfig(env, args[0], asyncCtx, videoProperties);

    if (asyncCtx->napi->GetVideoRecorderProperties(env, args[0], videoProperties) != MSERR_OK) {
        SignError(asyncCtx.get(), MSERR_EXT_API9_INVALID_PARAMETER, "prepare", "");
    }

    // set param
    if (asyncCtx->napi->SetVideoRecorderProperties(asyncCtx, videoProperties) != MSERR_OK) {
        SignError(asyncCtx.get(), MSERR_EXT_API9_INVALID_PARAMETER, "prepare", "");
    }
    if (asyncCtx->napi->SetUrl(urlPath) != MSERR_OK) {
        SignError(asyncCtx.get(), MSERR_EXT_API9_INVALID_PARAMETER, "urlPath", "");
    }

    asyncCtx->callbackRef = CommonNapi::CreateReference(env, args[1]);
    asyncCtx->deferred = CommonNapi::CreatePromise(env, asyncCtx->callbackRef, result);

    asyncCtx->napi->currentStates_ = VideoRecorderState::STATE_PREPARED;
    // async work
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "Prepare", NAPI_AUTO_LENGTH, &resource);
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resource, [](napi_env env, void* data) {
        auto threadCtx = reinterpret_cast<VideoRecorderAsyncContext *>(data);
        CHECK_AND_RETURN_LOG(threadCtx != nullptr, "threadCtx is nullptr!");
        if (threadCtx->napi == nullptr || threadCtx->napi->recorder_ == nullptr) {
            SignError(threadCtx, MSERR_EXT_API9_OPERATE_NOT_PERMIT, "prepare", "");
            return;
        }

        if (threadCtx->napi->recorder_->Prepare() != MSERR_OK) {
            SignError(threadCtx, MSERR_EXT_API9_OPERATE_NOT_PERMIT, "prepare", "");
        }
    }, MediaAsyncContext::CompleteCallback, static_cast<void *>(asyncCtx.get()), &asyncCtx->work));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCtx->work));
    asyncCtx.release();
    return result;
}

napi_value VideoRecorderNapi::GetInputSurface(napi_env env, napi_callback_info info)
{
    MEDIA_LOGD("GetInputSurface In");

    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    auto asyncCtx = std::make_unique<VideoRecorderAsyncContext>(env);
    // get args
    napi_value jsThis = nullptr;
    napi_value args[1] = {nullptr};
    size_t argCount = 1;
    napi_status status = napi_get_cb_info(env, info, &argCount, args, &jsThis, nullptr);
    if (status != napi_ok || jsThis == nullptr) {
        SignError(asyncCtx.get(), MSERR_EXT_API9_INVALID_PARAMETER, "GetInputSurface", "");
    }

    // get recordernapi
    (void)napi_unwrap(env, jsThis, reinterpret_cast<void **>(&asyncCtx->napi));

    asyncCtx->callbackRef = CommonNapi::CreateReference(env, args[0]);
    asyncCtx->deferred = CommonNapi::CreatePromise(env, asyncCtx->callbackRef, result);

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "GetInputSurface", NAPI_AUTO_LENGTH, &resource);
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resource, [](napi_env env, void* data) {
        auto threadCtx = reinterpret_cast<VideoRecorderAsyncContext *>(data);
        CHECK_AND_RETURN_LOG(threadCtx != nullptr, "threadCtx is nullptr!");
        if (threadCtx->napi == nullptr || threadCtx->napi->recorder_ == nullptr) {
            SignError(threadCtx, MSERR_EXT_API9_OPERATE_NOT_PERMIT, "GetInputSurface", "");
            return;
        }

        threadCtx->napi->surface_ = threadCtx->napi->recorder_->GetSurface(threadCtx->napi->videoSourceID); // source id
        if (threadCtx->napi->surface_ != nullptr) {
            SurfaceError error = SurfaceUtils::GetInstance()->Add(threadCtx->napi->surface_->GetUniqueId(),
                threadCtx->napi->surface_);
            if (error != SURFACE_ERROR_OK) {
                SignError(threadCtx, MSERR_EXT_API9_OPERATE_NOT_PERMIT, "GetInputSurface", "");
            }
            auto surfaceId = std::to_string(threadCtx->napi->surface_->GetUniqueId());
            threadCtx->JsResult = std::make_unique<MediaJsResultString>(surfaceId);
        } else {
            SignError(threadCtx, MSERR_EXT_API9_OPERATE_NOT_PERMIT, "GetInputSurface", "");
        }
    }, MediaAsyncContext::CompleteCallback, static_cast<void *>(asyncCtx.get()), &asyncCtx->work));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCtx->work));
    asyncCtx.release();
    return result;
}

napi_value VideoRecorderNapi::Start(napi_env env, napi_callback_info info)
{
    MEDIA_LOGD("Start In");

    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    auto asyncCtx = std::make_unique<VideoRecorderAsyncContext>(env);

    napi_value jsThis = nullptr;
    napi_value args[1] = { nullptr };
    size_t argCount = 1;
    napi_status status = napi_get_cb_info(env, info, &argCount, args, &jsThis, nullptr);
    if (status != napi_ok || jsThis == nullptr) {
        SignError(asyncCtx.get(), MSERR_EXT_API9_INVALID_PARAMETER, "Start", "");
    }

    // get recordernapi
    (void)napi_unwrap(env, jsThis, reinterpret_cast<void **>(&asyncCtx->napi));

    asyncCtx->callbackRef = CommonNapi::CreateReference(env, args[0]);
    asyncCtx->deferred = CommonNapi::CreatePromise(env, asyncCtx->callbackRef, result);

    // async work
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "Start", NAPI_AUTO_LENGTH, &resource);
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resource, [](napi_env env, void* data) {
        auto threadCtx = reinterpret_cast<VideoRecorderAsyncContext *>(data);
        CHECK_AND_RETURN_LOG(threadCtx != nullptr, "threadCtx is nullptr!");
        if (threadCtx->napi == nullptr || threadCtx->napi->recorder_ == nullptr) {
            SignError(threadCtx, MSERR_EXT_API9_OPERATE_NOT_PERMIT, "Start", "");
            return;
        }
        if (threadCtx->napi->recorder_->Start() != MSERR_OK) {
            SignError(threadCtx, MSERR_EXT_API9_OPERATE_NOT_PERMIT, "Start", "");
        }
        threadCtx->napi->currentStates_ = VideoRecorderState::STATE_PLAYING;
    }, MediaAsyncContext::CompleteCallback, static_cast<void *>(asyncCtx.get()), &asyncCtx->work));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCtx->work));
    asyncCtx.release();
    return result;
}

napi_value VideoRecorderNapi::Pause(napi_env env, napi_callback_info info)
{
    MEDIA_LOGD("Pause In");

    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    auto asyncCtx = std::make_unique<VideoRecorderAsyncContext>(env);

    napi_value jsThis = nullptr;
    napi_value args[1] = { nullptr };
    size_t argCount = 1;
    napi_status status = napi_get_cb_info(env, info, &argCount, args, &jsThis, nullptr);
    if (status != napi_ok || jsThis == nullptr) {
        SignError(asyncCtx.get(), MSERR_EXT_API9_INVALID_PARAMETER, "Pause", "");
    }

    // get recordernapi
    (void)napi_unwrap(env, jsThis, reinterpret_cast<void **>(&asyncCtx->napi));

    asyncCtx->callbackRef = CommonNapi::CreateReference(env, args[0]);
    asyncCtx->deferred = CommonNapi::CreatePromise(env, asyncCtx->callbackRef, result);

    // async work
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "Pause", NAPI_AUTO_LENGTH, &resource);
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resource, [](napi_env env, void* data) {
        auto threadCtx = reinterpret_cast<VideoRecorderAsyncContext *>(data);
        CHECK_AND_RETURN_LOG(threadCtx != nullptr, "threadCtx is nullptr!");
        if (threadCtx->napi == nullptr || threadCtx->napi->recorder_ == nullptr) {
            SignError(threadCtx, MSERR_EXT_API9_OPERATE_NOT_PERMIT, "Pause", "");
            return;
        }
        if (threadCtx->napi->recorder_->Pause() != MSERR_OK) {
            SignError(threadCtx, MSERR_EXT_API9_OPERATE_NOT_PERMIT, "Pause", "");
        }
        threadCtx->napi->currentStates_ = VideoRecorderState::STATE_PAUSED;
    }, MediaAsyncContext::CompleteCallback, static_cast<void *>(asyncCtx.get()), &asyncCtx->work));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCtx->work));
    asyncCtx.release();
    return result;
}

napi_value VideoRecorderNapi::Resume(napi_env env, napi_callback_info info)
{
    MEDIA_LOGD("Resume In");

    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    auto asyncCtx = std::make_unique<VideoRecorderAsyncContext>(env);

    napi_value jsThis = nullptr;
    napi_value args[1] = { nullptr };
    size_t argCount = 1;
    napi_status status = napi_get_cb_info(env, info, &argCount, args, &jsThis, nullptr);
    if (status != napi_ok || jsThis == nullptr) {
        SignError(asyncCtx.get(), MSERR_EXT_API9_INVALID_PARAMETER, "Resume", "");
    }

    // get recordernapi
    (void)napi_unwrap(env, jsThis, reinterpret_cast<void **>(&asyncCtx->napi));

    asyncCtx->callbackRef = CommonNapi::CreateReference(env, args[0]);
    asyncCtx->deferred = CommonNapi::CreatePromise(env, asyncCtx->callbackRef, result);

    // async work
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "Resume", NAPI_AUTO_LENGTH, &resource);
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resource, [](napi_env env, void* data) {
        auto threadCtx = reinterpret_cast<VideoRecorderAsyncContext *>(data);
        CHECK_AND_RETURN_LOG(threadCtx != nullptr, "threadCtx is nullptr!");
        if (threadCtx->napi == nullptr || threadCtx->napi->recorder_ == nullptr) {
            SignError(threadCtx, MSERR_EXT_API9_OPERATE_NOT_PERMIT, "Resume", "");
            return;
        }
        if (threadCtx->napi->recorder_->Resume() != MSERR_OK) {
            SignError(threadCtx, MSERR_EXT_API9_OPERATE_NOT_PERMIT, "Resume", "");
        }
        threadCtx->napi->currentStates_ = VideoRecorderState::STATE_PLAYING;
    }, MediaAsyncContext::CompleteCallback, static_cast<void *>(asyncCtx.get()), &asyncCtx->work));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCtx->work));
    asyncCtx.release();
    return result;
}

napi_value VideoRecorderNapi::Stop(napi_env env, napi_callback_info info)
{
    MEDIA_LOGD("Stop In");

    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    auto asyncCtx = std::make_unique<VideoRecorderAsyncContext>(env);

    napi_value jsThis = nullptr;
    napi_value args[1] = { nullptr };
    size_t argCount = 1;
    napi_status status = napi_get_cb_info(env, info, &argCount, args, &jsThis, nullptr);
    if (status != napi_ok || jsThis == nullptr) {
        SignError(asyncCtx.get(), MSERR_EXT_API9_INVALID_PARAMETER, "Stop", "");
    }

    // get recordernapi
    (void)napi_unwrap(env, jsThis, reinterpret_cast<void **>(&asyncCtx->napi));

    asyncCtx->callbackRef = CommonNapi::CreateReference(env, args[0]);
    asyncCtx->deferred = CommonNapi::CreatePromise(env, asyncCtx->callbackRef, result);

    // async work
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "Stop", NAPI_AUTO_LENGTH, &resource);
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resource, [](napi_env env, void* data) {
        auto threadCtx = reinterpret_cast<VideoRecorderAsyncContext *>(data);
        CHECK_AND_RETURN_LOG(threadCtx != nullptr, "threadCtx is nullptr!");
        if (threadCtx->napi == nullptr || threadCtx->napi->recorder_ == nullptr) {
            SignError(threadCtx, MSERR_EXT_API9_OPERATE_NOT_PERMIT, "Stop", "");
            return;
        }
        if (threadCtx->napi->recorder_->Stop(false) != MSERR_OK) {
            SignError(threadCtx, MSERR_EXT_API9_OPERATE_NOT_PERMIT, "Stop", "");
        }
        threadCtx->napi->currentStates_ = VideoRecorderState::STATE_STOPPED;
    }, MediaAsyncContext::CompleteCallback, static_cast<void *>(asyncCtx.get()), &asyncCtx->work));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCtx->work));
    asyncCtx.release();
    return result;
}

napi_value VideoRecorderNapi::Reset(napi_env env, napi_callback_info info)
{
    MEDIA_LOGD("Reset In");

    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    auto asyncCtx = std::make_unique<VideoRecorderAsyncContext>(env);

    napi_value jsThis = nullptr;
    napi_value args[1] = { nullptr };
    size_t argCount = 1;
    napi_status status = napi_get_cb_info(env, info, &argCount, args, &jsThis, nullptr);
    if (status != napi_ok || jsThis == nullptr) {
        SignError(asyncCtx.get(), MSERR_EXT_API9_INVALID_PARAMETER, "Reset", "");
    }

    // get recordernapi
    (void)napi_unwrap(env, jsThis, reinterpret_cast<void **>(&asyncCtx->napi));

    asyncCtx->callbackRef = CommonNapi::CreateReference(env, args[0]);
    asyncCtx->deferred = CommonNapi::CreatePromise(env, asyncCtx->callbackRef, result);

    // async work
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "Reset", NAPI_AUTO_LENGTH, &resource);
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resource, [](napi_env env, void* data) {
        auto threadCtx = reinterpret_cast<VideoRecorderAsyncContext *>(data);
        CHECK_AND_RETURN_LOG(threadCtx != nullptr, "threadCtx is nullptr!");
        if (threadCtx->napi == nullptr || threadCtx->napi->recorder_ == nullptr) {
            SignError(threadCtx, MSERR_EXT_API9_OPERATE_NOT_PERMIT, "Reset", "");
            return;
        }
        if (threadCtx->napi->surface_ != nullptr) {
            auto id = threadCtx->napi->surface_->GetUniqueId();
            if (threadCtx->napi->IsSurfaceIdVaild(id)) {
                (void)SurfaceUtils::GetInstance()->Remove(id);
            }
            threadCtx->napi->surface_ = nullptr;
        }
        if (threadCtx->napi->recorder_->Reset() != MSERR_OK) {
            SignError(threadCtx, MSERR_EXT_API9_OPERATE_NOT_PERMIT, "Reset", "");
        }
        threadCtx->napi->currentStates_ = VideoRecorderState::STATE_IDLE;
    }, MediaAsyncContext::CompleteCallback, static_cast<void *>(asyncCtx.get()), &asyncCtx->work));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCtx->work));
    asyncCtx.release();
    return result;
}

napi_value VideoRecorderNapi::Release(napi_env env, napi_callback_info info)
{
    MEDIA_LOGD("Release In");

    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    auto asyncCtx = std::make_unique<VideoRecorderAsyncContext>(env);

    napi_value jsThis = nullptr;
    napi_value args[1] = { nullptr };
    size_t argCount = 1;
    napi_status status = napi_get_cb_info(env, info, &argCount, args, &jsThis, nullptr);
    if (status != napi_ok || jsThis == nullptr) {
        SignError(asyncCtx.get(), MSERR_EXT_API9_INVALID_PARAMETER, "Release", "");
    }

    // get recordernapi
    (void)napi_unwrap(env, jsThis, reinterpret_cast<void **>(&asyncCtx->napi));

    asyncCtx->callbackRef = CommonNapi::CreateReference(env, args[0]);
    asyncCtx->deferred = CommonNapi::CreatePromise(env, asyncCtx->callbackRef, result);

    // async work
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "Release", NAPI_AUTO_LENGTH, &resource);
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resource, [](napi_env env, void *data) {
        auto threadCtx = reinterpret_cast<VideoRecorderAsyncContext *>(data);
        CHECK_AND_RETURN_LOG(threadCtx != nullptr, "threadCtx is nullptr!");
        if (threadCtx->napi == nullptr || threadCtx->napi->recorder_ == nullptr) {
            SignError(threadCtx, MSERR_EXT_API9_OPERATE_NOT_PERMIT, "Release", "");
            return;
        }
        if (threadCtx->napi->surface_ != nullptr) {
            auto id = threadCtx->napi->surface_->GetUniqueId();
            if (threadCtx->napi->IsSurfaceIdVaild(id)) {
                (void)SurfaceUtils::GetInstance()->Remove(id);
            }
            threadCtx->napi->surface_ = nullptr;
        }
        if (threadCtx->napi->recorder_->Release() != MSERR_OK) {
            SignError(threadCtx, MSERR_EXT_API9_OPERATE_NOT_PERMIT, "Release", "");
        }
        threadCtx->napi->currentStates_ = VideoRecorderState::STATE_IDLE;
        threadCtx->napi->CancelCallback();
    }, MediaAsyncContext::CompleteCallback, static_cast<void *>(asyncCtx.get()), &asyncCtx->work));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCtx->work));
    asyncCtx.release();
    return result;
}

napi_value VideoRecorderNapi::On(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    static constexpr size_t minArgCount = 2;
    size_t argCount = minArgCount;
    napi_value args[minArgCount] = { nullptr, nullptr };
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, args, &jsThis, nullptr);
    if (status != napi_ok || jsThis == nullptr || args[0] == nullptr || args[1] == nullptr) {
        MEDIA_LOGE("Failed to retrieve details about the callback");
        return result;
    }

    VideoRecorderNapi *recorderNapi = nullptr;
    status = napi_unwrap(env, jsThis, reinterpret_cast<void **>(&recorderNapi));
    CHECK_AND_RETURN_RET_LOG(status == napi_ok && recorderNapi != nullptr, result, "Failed to retrieve instance");

    napi_valuetype valueType0 = napi_undefined;
    napi_valuetype valueType1 = napi_undefined;
    if (napi_typeof(env, args[0], &valueType0) != napi_ok || valueType0 != napi_string ||
        napi_typeof(env, args[1], &valueType1) != napi_ok || valueType1 != napi_function) {
        recorderNapi->ErrorCallback(MSERR_EXT_INVALID_VAL);
        return result;
    }

    std::string callbackName = CommonNapi::GetStringArgument(env, args[0]);
    MEDIA_LOGD("callbackName: %{public}s", callbackName.c_str());

    napi_ref ref = nullptr;
    status = napi_create_reference(env, args[1], 1, &ref);
    CHECK_AND_RETURN_RET_LOG(status == napi_ok && ref != nullptr, result, "failed to create reference!");

    std::shared_ptr<AutoRef> autoRef = std::make_shared<AutoRef>(env, ref);
    recorderNapi->SetCallbackReference(callbackName, autoRef);
    return result;
}


void VideoRecorderNapi::GetConfig(napi_env env, napi_value args,
    std::unique_ptr<VideoRecorderAsyncContext> &ctx, VideoRecorderProperties &properties)
{
    int32_t audioSource = AUDIO_SOURCE_INVALID;
    int32_t videoSource = VIDEO_SOURCE_BUTT;

    bool ret = CommonNapi::GetPropertyInt32(env, args, "audioSourceType", audioSource);
    if (ret) {
        // audio + video
        properties.audioSourceType = static_cast<AudioSourceType>(audioSource);
    } else {
        // pure video
        ctx->napi->isPureVideo = true;
        MEDIA_LOGI("No audioSource Type input!");
    }

    (void)CommonNapi::GetPropertyInt32(env, args, "videoSourceType", videoSource);
    properties.videoSourceType = static_cast<VideoSourceType>(videoSource);

    (void)CommonNapi::GetPropertyInt32(env, args, "rotation", properties.orientationHint);

    napi_value geoLocation = nullptr;
    napi_get_named_property(env, args, "location", &geoLocation);
    double tempLatitude = 0;
    double tempLongitude = 0;
    (void)CommonNapi::GetPropertyDouble(env, geoLocation, "latitude", tempLatitude);
    (void)CommonNapi::GetPropertyDouble(env, geoLocation, "longitude", tempLongitude);
    properties.location.latitude = static_cast<float>(tempLatitude);
    properties.location.longitude = static_cast<float>(tempLongitude);
}

int32_t VideoRecorderNapi::GetVideoRecorderProperties(napi_env env, napi_value args,
    VideoRecorderProperties &properties)
{
    napi_value item = nullptr;
    napi_get_named_property(env, args, "profile", &item);

    (void)CommonNapi::GetPropertyInt32(env, item, "audioBitrate", properties.profile.audioBitrate);
    (void)CommonNapi::GetPropertyInt32(env, item, "audioChannels", properties.profile.audioChannels);
    std::string audioCodec = CommonNapi::GetPropertyString(env, item, "audioCodec");
    (void)MapMimeToAudioCodecFormat(audioCodec, properties.profile.audioCodecFormat);
    (void)CommonNapi::GetPropertyInt32(env, item, "audioSampleRate", properties.profile.auidoSampleRate);
    (void)CommonNapi::GetPropertyInt32(env, item, "durationTime", properties.profile.duration);
    std::string outputFile = CommonNapi::GetPropertyString(env, item, "fileFormat");
    (void)MapExtensionNameToOutputFormat(outputFile, properties.profile.outputFormat);
    (void)CommonNapi::GetPropertyInt32(env, item, "videoBitrate", properties.profile.videoBitrate);
    std::string videoCodec = CommonNapi::GetPropertyString(env, item, "videoCodec");
    (void)MapMimeToVideoCodecFormat(videoCodec, properties.profile.videoCodecFormat);
    (void)CommonNapi::GetPropertyInt32(env, item, "videoFrameWidth", properties.profile.videoFrameWidth);
    (void)CommonNapi::GetPropertyInt32(env, item, "videoFrameHeight", properties.profile.videoFrameHeight);
    (void)CommonNapi::GetPropertyInt32(env, item, "videoFrameRate", properties.profile.videoFrameRate);

    return MSERR_OK;
}

int32_t VideoRecorderNapi::SetVideoRecorderProperties(std::unique_ptr<VideoRecorderAsyncContext> &ctx,
    const VideoRecorderProperties &properties)
{
    int32_t ret;
    CHECK_AND_RETURN_RET(recorder_ != nullptr, MSERR_INVALID_OPERATION);
    if (ctx->napi->isPureVideo != true) {
        // audio + video
        ret = recorder_->SetAudioSource(properties.audioSourceType, ctx->napi->audioSourceID);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Fail to set AudioSource");

        ret = recorder_->SetVideoSource(properties.videoSourceType, ctx->napi->videoSourceID);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Fail to set VideoSource");

        ret = recorder_->SetOutputFormat(properties.profile.outputFormat);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Fail to set OutputFormat");

        ret = recorder_->SetAudioEncoder(ctx->napi->audioSourceID, properties.profile.audioCodecFormat);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Fail to set audioCodec");

        ret = recorder_->SetAudioSampleRate(ctx->napi->audioSourceID, properties.profile.auidoSampleRate);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Fail to set auidoSampleRate");

        ret = recorder_->SetAudioChannels(ctx->napi->audioSourceID, properties.profile.audioChannels);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Fail to set audioChannels");

        ret = recorder_->SetAudioEncodingBitRate(ctx->napi->audioSourceID, properties.profile.audioBitrate);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Fail to set audioBitrate");
    } else {
        ret = recorder_->SetVideoSource(properties.videoSourceType, ctx->napi->videoSourceID);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Fail to set VideoSource");

        ret = recorder_->SetOutputFormat(properties.profile.outputFormat);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Fail to set OutputFormat");
    }
    ret = recorder_->SetVideoEncoder(ctx->napi->videoSourceID, properties.profile.videoCodecFormat);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Fail to set videoCodec");

    ret = recorder_->SetVideoSize(ctx->napi->videoSourceID, properties.profile.videoFrameWidth,
        properties.profile.videoFrameHeight);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Fail to set videoSize");

    ret = recorder_->SetVideoFrameRate(ctx->napi->videoSourceID, properties.profile.videoFrameRate);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Fail to set videoFrameRate");

    ret = recorder_->SetVideoEncodingBitRate(ctx->napi->videoSourceID, properties.profile.videoBitrate);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Fail to set videoBitrate");

    recorder_->SetLocation(properties.location.latitude, properties.location.longitude);
    recorder_->SetOrientationHint(properties.orientationHint);

    return MSERR_OK;
}

int32_t VideoRecorderNapi::SetUrl(const std::string &urlPath)
{
    CHECK_AND_RETURN_RET_LOG(recorder_ != nullptr, MSERR_INVALID_OPERATION, "No memory");
    const std::string fdHead = "fd://";

    if (urlPath.find(fdHead) != std::string::npos) {
        int32_t fd = -1;
        std::string inputFd = urlPath.substr(fdHead.size());
        CHECK_AND_RETURN_RET(StrToInt(inputFd, fd) == true, MSERR_INVALID_VAL);
        CHECK_AND_RETURN_RET(fd >= 0, MSERR_INVALID_OPERATION);
        CHECK_AND_RETURN_RET(recorder_->SetOutputFile(fd) == MSERR_OK, MSERR_INVALID_OPERATION);
    } else {
        MEDIA_LOGE("invalid input uri, not a fd!");
        return MSERR_INVALID_OPERATION;
    }

    return MSERR_OK;
}

bool VideoRecorderNapi::IsSurfaceIdVaild(uint64_t surfaceID)
{
    auto surface = SurfaceUtils::GetInstance()->GetSurface(surfaceID);
    if (surface == nullptr) {
        return false;
    }
    return true;
}

napi_value VideoRecorderNapi::GetState(napi_env env, napi_callback_info info)
{
    napi_value jsThis = nullptr;
    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    size_t argCount = 0;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    CHECK_AND_RETURN_RET_LOG(status == napi_ok, result, "Failed to retrieve details about the callback");

    VideoRecorderNapi *recorderNapi = nullptr;
    status = napi_unwrap(env, jsThis, reinterpret_cast<void **>(&recorderNapi));
    CHECK_AND_RETURN_RET_LOG(status == napi_ok, result, "Failed to retrieve instance");

    std::string curState = VideoRecorderState::STATE_ERROR;
    if (recorderNapi->callbackNapi_ != nullptr) {
        curState = recorderNapi->currentStates_;
        MEDIA_LOGD("GetState success, State: %{public}s", curState.c_str());
    }

    napi_value jsResult = nullptr;
    status = napi_create_string_utf8(env, curState.c_str(), NAPI_AUTO_LENGTH, &jsResult);
    CHECK_AND_RETURN_RET_LOG(status == napi_ok, result, "napi_create_string_utf8 error");
    return jsResult;
}

// Synchronous interface can use this to report error
void VideoRecorderNapi::ErrorCallback(MediaServiceExtErrCode errCode)
{
    if (callbackNapi_ != nullptr) {
        auto napiCb = std::static_pointer_cast<RecorderCallbackNapi>(callbackNapi_);
        napiCb->SendErrorCallback(errCode);
    }
}

void VideoRecorderNapi::SetCallbackReference(const std::string &callbackName, std::shared_ptr<AutoRef> ref)
{
    refMap_[callbackName] = ref;
    if (callbackNapi_ != nullptr) {
        auto napiCb = std::static_pointer_cast<RecorderCallbackNapi>(callbackNapi_);
        napiCb->SaveCallbackReference(callbackName, ref);
    }
}

void VideoRecorderNapi::CancelCallback()
{
    if (callbackNapi_ != nullptr) {
        auto napiCb = std::static_pointer_cast<RecorderCallbackNapi>(callbackNapi_);
        napiCb->ClearCallbackReference();
    }
}
} // namespace Media
} // namespace OHOS
