/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "output/preview_output_napi.h"
#include <unistd.h>
#include <uv.h>

namespace OHOS {
namespace CameraStandard {
using namespace std;
using OHOS::HiviewDFX::HiLog;
using OHOS::HiviewDFX::HiLogLabel;

namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "PreviewOutputNapi"};
}

thread_local napi_ref PreviewOutputNapi::sConstructor_ = nullptr;
thread_local sptr<PreviewOutput> PreviewOutputNapi::sPreviewOutput_ = nullptr;
thread_local uint32_t PreviewOutputNapi::previewOutputTaskId = CAMERA_PREVIEW_OUTPUT_TASKID;

PreviewOutputCallback::PreviewOutputCallback(napi_env env) : env_(env) {}

void PreviewOutputCallback::UpdateJSCallbackAsync(std::string propName, const int32_t value) const
{
    uv_loop_s* loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (!loop) {
        MEDIA_ERR_LOG("PreviewOutputCallback:UpdateJSCallbackAsync() failed to get event loop");
        return;
    }
    uv_work_t* work = new(std::nothrow) uv_work_t;
    if (!work) {
        MEDIA_ERR_LOG("PreviewOutputCallback:UpdateJSCallbackAsync() failed to allocate work");
        return;
    }
    std::unique_ptr<PreviewOutputCallbackInfo> callbackInfo =
        std::make_unique<PreviewOutputCallbackInfo>(propName, value, this);
    work->data = callbackInfo.get();
    int ret = uv_queue_work(loop, work, [] (uv_work_t* work) {}, [] (uv_work_t* work, int status) {
        PreviewOutputCallbackInfo* callbackInfo = reinterpret_cast<PreviewOutputCallbackInfo *>(work->data);
        if (callbackInfo) {
            callbackInfo->listener_->UpdateJSCallback(callbackInfo->eventName_, callbackInfo->value_);
            delete callbackInfo;
        }
        delete work;
    });
    if (ret) {
        MEDIA_ERR_LOG("PreviewOutputCallback:UpdateJSCallbackAsync() failed to execute work");
        delete work;
    } else {
        callbackInfo.release();
    }
}

void PreviewOutputCallback::OnFrameStarted() const
{
    CAMERA_SYNC_TRACE;
    MEDIA_INFO_LOG("PreviewOutputCallback:OnFrameStarted() is called!");
    UpdateJSCallbackAsync("OnFrameStarted", -1);
}

void PreviewOutputCallback::OnFrameEnded(const int32_t frameCount) const
{
    CAMERA_SYNC_TRACE;
    MEDIA_INFO_LOG("PreviewOutputCallback:OnFrameEnded() is called!, frameCount: %{public}d", frameCount);
    UpdateJSCallbackAsync("OnFrameEnded", frameCount);
}

void PreviewOutputCallback::OnError(const int32_t errorCode) const
{
    MEDIA_INFO_LOG("PreviewOutputCallback:OnError() is called!, errorCode: %{public}d", errorCode);
    UpdateJSCallbackAsync("OnError", errorCode);
}

void PreviewOutputCallback::SetCallbackRef(const std::string &eventType, const napi_ref &callbackRef)
{
    if (eventType.compare("frameStart") == 0) {
        frameStartCallbackRef_ = callbackRef;
    } else if (eventType.compare("frameEnd") == 0) {
        frameEndCallbackRef_ = callbackRef;
    } else if (eventType.compare("error") == 0) {
        errorCallbackRef_ = callbackRef;
    } else {
        MEDIA_ERR_LOG("Incorrect preview callback event type received from JS");
    }
}

void PreviewOutputCallback::UpdateJSCallback(std::string propName, const int32_t value) const
{
    napi_value result[ARGS_ONE];
    napi_value callback = nullptr;
    napi_value retVal;
    napi_value propValue;
    int32_t jsErrorCodeUnknown = -1;

    if (propName.compare("OnFrameStarted") == 0) {
        CAMERA_NAPI_CHECK_NULL_PTR_RETURN_VOID(frameStartCallbackRef_,
            "OnFrameStart callback is not registered by JS");
        napi_get_undefined(env_, &result[PARAM0]);
        napi_get_reference_value(env_, frameStartCallbackRef_, &callback);
    } else if (propName.compare("OnFrameEnded") == 0) {
        CAMERA_NAPI_CHECK_NULL_PTR_RETURN_VOID(frameEndCallbackRef_,
            "OnFrameEnd callback is not registered by JS");
        napi_get_undefined(env_, &result[PARAM0]);
        napi_get_reference_value(env_, frameEndCallbackRef_, &callback);
    } else {
        CAMERA_NAPI_CHECK_NULL_PTR_RETURN_VOID(errorCallbackRef_,
            "OnError callback is not registered by JS");
        napi_create_object(env_, &result[PARAM0]);
        napi_create_int32(env_, jsErrorCodeUnknown, &propValue);
        napi_set_named_property(env_, result[PARAM0], "code", propValue);
        napi_get_reference_value(env_, errorCallbackRef_, &callback); // should errorcode be valued as -1
        if (errorCallbackRef_ != nullptr) {
            napi_delete_reference(env_, errorCallbackRef_);
        }
    }

    napi_call_function(env_, nullptr, callback, ARGS_ONE, result, &retVal);
}

PreviewOutputNapi::PreviewOutputNapi() : env_(nullptr), wrapper_(nullptr)
{
}

PreviewOutputNapi::~PreviewOutputNapi()
{
    if (wrapper_ != nullptr) {
        napi_delete_reference(env_, wrapper_);
    }
    if (previewOutput_) {
        previewOutput_ = nullptr;
    }
    if (previewCallback_) {
        previewCallback_ = nullptr;
    }
}

void PreviewOutputNapi::PreviewOutputNapiDestructor(napi_env env, void* nativeObject, void* finalize_hint)
{
    MEDIA_DEBUG_LOG("PreviewOutputNapiDestructor enter");
    PreviewOutputNapi* cameraObj = reinterpret_cast<PreviewOutputNapi*>(nativeObject);
    if (cameraObj != nullptr) {
        delete cameraObj;
    }
}

napi_value PreviewOutputNapi::Init(napi_env env, napi_value exports)
{
    napi_status status;
    napi_value ctorObj;
    int32_t refCount = 1;

    napi_property_descriptor preview_output_props[] = {
        DECLARE_NAPI_FUNCTION("addDeferredSurface", AddDeferredSurface),
        DECLARE_NAPI_FUNCTION("start", Start),
        DECLARE_NAPI_FUNCTION("stop", Stop),
        DECLARE_NAPI_FUNCTION("release", Release),
        DECLARE_NAPI_FUNCTION("on", On)
    };

    status = napi_define_class(env, CAMERA_PREVIEW_OUTPUT_NAPI_CLASS_NAME, NAPI_AUTO_LENGTH,
                               PreviewOutputNapiConstructor, nullptr,
                               sizeof(preview_output_props) / sizeof(preview_output_props[PARAM0]),
                               preview_output_props, &ctorObj);
    if (status == napi_ok) {
        status = napi_create_reference(env, ctorObj, refCount, &sConstructor_);
        if (status == napi_ok) {
            status = napi_set_named_property(env, exports, CAMERA_PREVIEW_OUTPUT_NAPI_CLASS_NAME, ctorObj);
            if (status == napi_ok) {
                return exports;
            }
        }
    }
    return nullptr;
}
// Constructor callback
napi_value PreviewOutputNapi::PreviewOutputNapiConstructor(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value result = nullptr;
    napi_value thisVar = nullptr;

    napi_get_undefined(env, &result);
    CAMERA_NAPI_GET_JS_OBJ_WITH_ZERO_ARGS(env, info, status, thisVar);

    if (status == napi_ok && thisVar != nullptr) {
        std::unique_ptr<PreviewOutputNapi> obj = std::make_unique<PreviewOutputNapi>();
        if (obj != nullptr) {
            obj->env_ = env;
            obj->previewOutput_ = sPreviewOutput_;

            std::shared_ptr<PreviewOutputCallback> callback =
                std::make_shared<PreviewOutputCallback>(PreviewOutputCallback(env));
            ((sptr<PreviewOutput> &)(obj->previewOutput_))->SetCallback(callback);
            obj->previewCallback_ = callback;

            status = napi_wrap(env, thisVar, reinterpret_cast<void*>(obj.get()),
                               PreviewOutputNapi::PreviewOutputNapiDestructor, nullptr, nullptr);
            if (status == napi_ok) {
                obj.release();
                return thisVar;
            } else {
                MEDIA_ERR_LOG("Failure wrapping js to native napi");
            }
        }
    }

    return result;
}

static void CommonCompleteCallback(napi_env env, napi_status status, void* data)
{
    auto context = static_cast<PreviewOutputAsyncContext*>(data);
    if (context == nullptr) {
        MEDIA_ERR_LOG("Async context is null");
        return;
    }

    std::unique_ptr<JSAsyncContextOutput> jsContext = std::make_unique<JSAsyncContextOutput>();

    if (!context->status) {
        CameraNapiUtils::CreateNapiErrorObject(env, context->errorCode, context->errorMsg.c_str(), jsContext);
    } else {
        jsContext->status = true;
        napi_get_undefined(env, &jsContext->error);
        if (context->bRetBool) {
            napi_get_boolean(env, context->status, &jsContext->data);
        } else {
            napi_get_undefined(env, &jsContext->data);
        }
    }

    if (!context->funcName.empty() && context->taskId > 0) {
        // Finish async trace
        CAMERA_FINISH_ASYNC_TRACE(context->funcName, context->taskId);
        jsContext->funcName = context->funcName;
    }

    if (context->work != nullptr) {
        CameraNapiUtils::InvokeJSAsyncMethod(env, context->deferred, context->callbackRef,
                                             context->work, *jsContext);
    }
    delete context;
}

napi_value PreviewOutputNapi::CreatePreviewOutput(napi_env env, Profile &profile, std::string surfaceId)
{
    CAMERA_SYNC_TRACE;
    napi_status status;
    napi_value result = nullptr;
    napi_value constructor;

    status = napi_get_reference_value(env, sConstructor_, &constructor);
    if (status == napi_ok) {
        uint64_t iSurfaceId;
        std::istringstream iss(surfaceId);
        iss >> iSurfaceId;
        sptr<Surface> surface = SurfaceUtils::GetInstance()->GetSurface(iSurfaceId);
        if (!surface) {
            surface = Media::ImageReceiver::getSurfaceById(surfaceId);
        }
        if (surface == nullptr) {
            MEDIA_ERR_LOG("failed to get surface");
            return result;
        }

        surface->SetUserData(CameraManager::surfaceFormat, std::to_string(profile.GetCameraFormat()));
        int retCode = CameraManager::GetInstance()->CreatePreviewOutput(profile, surface, &sPreviewOutput_);
        if (!CameraNapiUtils::CheckError(env, retCode)) {
            return nullptr;
        }
        if (sPreviewOutput_ == nullptr) {
            MEDIA_ERR_LOG("failed to create previewOutput");
            return result;
        }
        status = napi_new_instance(env, constructor, 0, nullptr, &result);
        sPreviewOutput_ = nullptr;

        if (status == napi_ok && result != nullptr) {
            return result;
        } else {
            MEDIA_ERR_LOG("Failed to create preview output instance");
        }
    }

    napi_get_undefined(env, &result);
    return result;
}

sptr<PreviewOutput> PreviewOutputNapi::GetPreviewOutput()
{
    return previewOutput_;
}

bool PreviewOutputNapi::IsPreviewOutput(napi_env env, napi_value obj)
{
    bool result = false;
    napi_status status;
    napi_value constructor = nullptr;

    status = napi_get_reference_value(env, sConstructor_, &constructor);
    if (status == napi_ok) {
        status = napi_instanceof(env, obj, constructor, &result);
        if (status != napi_ok) {
            result = false;
        }
    }

    return result;
}

napi_value PreviewOutputNapi::Release(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value result = nullptr;
    const int32_t refCount = 1;
    napi_value resource = nullptr;
    size_t argc = ARGS_ONE;
    napi_value argv[ARGS_ONE] = {0};
    napi_value thisVar = nullptr;

    CAMERA_NAPI_GET_JS_ARGS(env, info, argc, argv, thisVar);
    NAPI_ASSERT(env, argc <= ARGS_ONE, "requires 1 parameter maximum");

    napi_get_undefined(env, &result);
    std::unique_ptr<PreviewOutputAsyncContext> asyncContext = std::make_unique<PreviewOutputAsyncContext>();
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&asyncContext->objectInfo));
    if (status == napi_ok && asyncContext->objectInfo != nullptr) {
        if (argc == ARGS_ONE) {
            CAMERA_NAPI_GET_JS_ASYNC_CB_REF(env, argv[PARAM0], refCount, asyncContext->callbackRef);
        }

        CAMERA_NAPI_CREATE_PROMISE(env, asyncContext->callbackRef, asyncContext->deferred, result);
        CAMERA_NAPI_CREATE_RESOURCE_NAME(env, resource, "Release");

        status = napi_create_async_work(
            env, nullptr, resource, [](napi_env env, void* data) {
                auto context = static_cast<PreviewOutputAsyncContext*>(data);
                context->status = false;
                // Start async trace
                context->funcName = "PreviewOutputNapi::Release";
                context->taskId = CameraNapiUtils::IncreamentAndGet(previewOutputTaskId);
                CAMERA_START_ASYNC_TRACE(context->funcName, context->taskId);
                if (context->objectInfo != nullptr && context->objectInfo->previewOutput_ != nullptr) {
                    context->bRetBool = false;
                    context->status = true;
                    ((sptr<PreviewOutput> &)(context->objectInfo->previewOutput_))->Release();
                }
            },
            CommonCompleteCallback, static_cast<void*>(asyncContext.get()), &asyncContext->work);
        if (status != napi_ok) {
            MEDIA_ERR_LOG("Failed to create napi_create_async_work for PreviewOutputNapi::Release");
            napi_get_undefined(env, &result);
        } else {
            napi_queue_async_work(env, asyncContext->work);
            asyncContext.release();
        }
    }

    return result;
}

napi_value PreviewOutputNapi::AddDeferredSurface(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value result = nullptr;
    const int32_t refCount = 1;
    napi_value resource = nullptr;
    size_t argc = ARGS_TWO;
    napi_value argv[ARGS_TWO] = {0};
    napi_value thisVar = nullptr;

    CAMERA_NAPI_GET_JS_ARGS(env, info, argc, argv, thisVar);
    NAPI_ASSERT(env, argc <= ARGS_TWO, "requires 1 parameter maximum");

    napi_get_undefined(env, &result);
    std::unique_ptr<PreviewOutputAsyncContext> asyncContext = std::make_unique<PreviewOutputAsyncContext>();
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&asyncContext->objectInfo));
    if (status == napi_ok && asyncContext->objectInfo != nullptr) {
        if (argc == ARGS_TWO) {
            CAMERA_NAPI_GET_JS_ASYNC_CB_REF(env, argv[PARAM1], refCount, asyncContext->callbackRef);
        }

        CAMERA_NAPI_CREATE_PROMISE(env, asyncContext->callbackRef, asyncContext->deferred, result);
        CAMERA_NAPI_CREATE_RESOURCE_NAME(env, resource, "AddDeferredSurface");

        status = napi_create_async_work(
            env, nullptr, resource, [](napi_env env, void* data) {
                auto context = static_cast<PreviewOutputAsyncContext*>(data);
                context->status = false;
                // Start async trace
                context->funcName = "PreviewOutputNapi::AddDeferredSurface";
                context->taskId = CameraNapiUtils::IncreamentAndGet(previewOutputTaskId);
                CAMERA_START_ASYNC_TRACE(context->funcName, context->taskId);
                if (context->objectInfo != nullptr && context->objectInfo->previewOutput_ != nullptr) {
                    context->bRetBool = false;
                    context->status = true;
                    ((sptr<PreviewOutput> &)(context->objectInfo->previewOutput_))->Stop();
                }
            },
            CommonCompleteCallback, static_cast<void*>(asyncContext.get()), &asyncContext->work);
        if (status != napi_ok) {
            MEDIA_ERR_LOG("Failed to create napi_create_async_work for PreviewOutputNapi::Release");
            napi_get_undefined(env, &result);
        } else {
            napi_queue_async_work(env, asyncContext->work);
            asyncContext.release();
        }
    }

    return result;
}

napi_value PreviewOutputNapi::Start(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value result = nullptr;
    const int32_t refCount = 1;
    napi_value resource = nullptr;
    size_t argc = ARGS_ONE;
    napi_value argv[ARGS_ONE] = {0};
    napi_value thisVar = nullptr;

    CAMERA_NAPI_GET_JS_ARGS(env, info, argc, argv, thisVar);
    NAPI_ASSERT(env, argc <= ARGS_ONE, "requires 1 parameter maximum");

    napi_get_undefined(env, &result);
    std::unique_ptr<PreviewOutputAsyncContext> asyncContext = std::make_unique<PreviewOutputAsyncContext>();
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&asyncContext->objectInfo));
    if (status == napi_ok && asyncContext->objectInfo != nullptr) {
        if (argc == ARGS_ONE) {
            CAMERA_NAPI_GET_JS_ASYNC_CB_REF(env, argv[PARAM0], refCount, asyncContext->callbackRef);
        }

        CAMERA_NAPI_CREATE_PROMISE(env, asyncContext->callbackRef, asyncContext->deferred, result);
        CAMERA_NAPI_CREATE_RESOURCE_NAME(env, resource, "Start");

        status = napi_create_async_work(
            env, nullptr, resource, [](napi_env env, void* data) {
                auto context = static_cast<PreviewOutputAsyncContext*>(data);
                context->status = false;
                // Start async trace
                context->funcName = "PreviewOutputNapi::Start";
                context->taskId = CameraNapiUtils::IncreamentAndGet(previewOutputTaskId);
                CAMERA_START_ASYNC_TRACE(context->funcName, context->taskId);
                if (context->objectInfo != nullptr && context->objectInfo->previewOutput_ != nullptr) {
                    context->bRetBool = false;
                    context->errorCode = context->objectInfo->previewOutput_->Start();
                    context->status = context->errorCode == 0;
                }
            },
            CommonCompleteCallback, static_cast<void*>(asyncContext.get()), &asyncContext->work);
        if (status != napi_ok) {
            MEDIA_ERR_LOG("Failed to create napi_create_async_work for PreviewOutputNapi::Release");
            napi_get_undefined(env, &result);
        } else {
            napi_queue_async_work(env, asyncContext->work);
            asyncContext.release();
        }
    }

    return result;
}

napi_value PreviewOutputNapi::Stop(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value result = nullptr;
    const int32_t refCount = 1;
    napi_value resource = nullptr;
    size_t argc = ARGS_ONE;
    napi_value argv[ARGS_ONE] = {0};
    napi_value thisVar = nullptr;

    CAMERA_NAPI_GET_JS_ARGS(env, info, argc, argv, thisVar);
    NAPI_ASSERT(env, argc <= ARGS_ONE, "requires 1 parameter maximum");

    napi_get_undefined(env, &result);
    std::unique_ptr<PreviewOutputAsyncContext> asyncContext = std::make_unique<PreviewOutputAsyncContext>();
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&asyncContext->objectInfo));
    if (status == napi_ok && asyncContext->objectInfo != nullptr) {
        if (argc == ARGS_ONE) {
            CAMERA_NAPI_GET_JS_ASYNC_CB_REF(env, argv[PARAM0], refCount, asyncContext->callbackRef);
        }

        CAMERA_NAPI_CREATE_PROMISE(env, asyncContext->callbackRef, asyncContext->deferred, result);
        CAMERA_NAPI_CREATE_RESOURCE_NAME(env, resource, "Stop");

        status = napi_create_async_work(
            env, nullptr, resource, [](napi_env env, void* data) {
                auto context = static_cast<PreviewOutputAsyncContext*>(data);
                context->status = false;
                // Start async trace
                context->funcName = "PreviewOutputNapi::Stop";
                context->taskId = CameraNapiUtils::IncreamentAndGet(previewOutputTaskId);
                CAMERA_START_ASYNC_TRACE(context->funcName, context->taskId);
                if (context->objectInfo != nullptr && context->objectInfo->previewOutput_ != nullptr) {
                    context->bRetBool = false;
                    context->errorCode = context->objectInfo->previewOutput_->Stop();
                    context->status = context->errorCode == 0;
                }
            },
            CommonCompleteCallback, static_cast<void*>(asyncContext.get()), &asyncContext->work);
        if (status != napi_ok) {
            MEDIA_ERR_LOG("Failed to create napi_create_async_work for PreviewOutputNapi::Release");
            napi_get_undefined(env, &result);
        } else {
            napi_queue_async_work(env, asyncContext->work);
            asyncContext.release();
        }
    }
    return result;
}

napi_value PreviewOutputNapi::On(napi_env env, napi_callback_info info)
{
    napi_value undefinedResult = nullptr;
    size_t argCount = ARGS_TWO;
    napi_value argv[ARGS_TWO] = {nullptr};
    napi_value thisVar = nullptr;
    size_t res = 0;
    char buffer[SIZE];
    std::string eventType;
    const int32_t refCount = 1;
    PreviewOutputNapi* obj = nullptr;
    napi_status status;

    napi_get_undefined(env, &undefinedResult);

    CAMERA_NAPI_GET_JS_ARGS(env, info, argCount, argv, thisVar);
    NAPI_ASSERT(env, argCount == ARGS_TWO, "requires 2 parameters");

    if (thisVar == nullptr || argv[PARAM0] == nullptr || argv[PARAM1] == nullptr) {
        MEDIA_ERR_LOG("Failed to retrieve details about the callback");
        return undefinedResult;
    }

    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&obj));
    if (status == napi_ok && obj != nullptr) {
        napi_valuetype valueType = napi_undefined;
        if (napi_typeof(env, argv[PARAM0], &valueType) != napi_ok || valueType != napi_string
            || napi_typeof(env, argv[PARAM1], &valueType) != napi_ok || valueType != napi_function) {
            return undefinedResult;
        }

        napi_get_value_string_utf8(env, argv[PARAM0], buffer, SIZE, &res);
        eventType = std::string(buffer);

        napi_ref callbackRef;
        napi_create_reference(env, argv[PARAM1], refCount, &callbackRef);

        if (!eventType.empty()) {
            obj->previewCallback_->SetCallbackRef(eventType, callbackRef);
        } else {
            MEDIA_ERR_LOG("Failed to Register Callback: event type is empty!");
        }
    }

    return undefinedResult;
}
} // namespace CameraStandard
} // namespace OHOS
