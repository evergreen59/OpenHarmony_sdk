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

#include "output/photo_output_napi.h"
#include <uv.h>

namespace OHOS {
namespace CameraStandard {
using namespace std;
using OHOS::HiviewDFX::HiLog;
using OHOS::HiviewDFX::HiLogLabel;

namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "PhotoOutputNapi"};
}

thread_local napi_ref PhotoOutputNapi::sConstructor_ = nullptr;
thread_local sptr<PhotoOutput> PhotoOutputNapi::sPhotoOutput_ = nullptr;
thread_local uint32_t PhotoOutputNapi::photoOutputTaskId = CAMERA_PHOTO_OUTPUT_TASKID;

PhotoOutputCallback::PhotoOutputCallback(napi_env env) : env_(env) {}

void PhotoOutputCallback::UpdateJSCallbackAsync(std::string propName, const CallbackInfo &info) const
{
    uv_loop_s* loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (!loop) {
        MEDIA_ERR_LOG("PhotoOutputCallback:UpdateJSCallbackAsync() failed to get event loop");
        return;
    }
    uv_work_t* work = new(std::nothrow) uv_work_t;
    if (!work) {
        MEDIA_ERR_LOG("PhotoOutputCallback:UpdateJSCallbackAsync() failed to allocate work");
        return;
    }
    std::unique_ptr<PhotoOutputCallbackInfo> callbackInfo =
        std::make_unique<PhotoOutputCallbackInfo>(propName, info, this);
    work->data = callbackInfo.get();
    int ret = uv_queue_work(loop, work, [] (uv_work_t* work) {}, [] (uv_work_t* work, int status) {
        PhotoOutputCallbackInfo* callbackInfo = reinterpret_cast<PhotoOutputCallbackInfo *>(work->data);
        if (callbackInfo) {
            callbackInfo->listener_->UpdateJSCallback(callbackInfo->eventName_, callbackInfo->info_);
            delete callbackInfo;
        }
        delete work;
    });
    if (ret) {
        MEDIA_ERR_LOG("PhotoOutputCallback:UpdateJSCallbackAsync() failed to execute work");
        delete work;
    } else {
        callbackInfo.release();
    }
}

void PhotoOutputCallback::OnCaptureStarted(const int32_t captureID) const
{
    CAMERA_SYNC_TRACE;
    MEDIA_INFO_LOG("PhotoOutputCallback:OnCaptureStarted() is called!, captureID: %{public}d", captureID);
    CallbackInfo info;
    info.captureID = captureID;
    UpdateJSCallbackAsync("OnCaptureStarted", info);
}

void PhotoOutputCallback::OnCaptureEnded(const int32_t captureID, const int32_t frameCount) const
{
    CAMERA_SYNC_TRACE;
    MEDIA_INFO_LOG("PhotoOutputCallback:OnCaptureEnded() is called!, captureID: %{public}d, frameCount: %{public}d",
                   captureID, frameCount);
    CallbackInfo info;
    info.captureID = captureID;
    info.frameCount = frameCount;
    UpdateJSCallbackAsync("OnCaptureEnded", info);
}

void PhotoOutputCallback::OnFrameShutter(const int32_t captureId, const uint64_t timestamp) const
{
    CAMERA_SYNC_TRACE;
    MEDIA_INFO_LOG("PhotoOutputCallback:OnFrameShutter() called, captureID: %{public}d, timestamp: %{public}" PRIu64,
        captureId, timestamp);
    CallbackInfo info;
    info.captureID = captureId;
    info.timestamp = timestamp;
    UpdateJSCallbackAsync("OnFrameShutter", info);
}

void PhotoOutputCallback::OnCaptureError(const int32_t captureId, const int32_t errorCode) const
{
    MEDIA_INFO_LOG("PhotoOutputCallback:OnCaptureError() is called!, captureID: %{public}d, errorCode: %{public}d",
        captureId, errorCode);
    CallbackInfo info;
    info.captureID = captureId;
    info.errorCode = errorCode;
    UpdateJSCallbackAsync("OnCaptureError", info);
}

void PhotoOutputCallback::SetCallbackRef(const std::string &eventType, const napi_ref &callbackRef)
{
    if (eventType.compare("captureStart") == 0) {
        captureStartCallbackRef_ = callbackRef;
    } else if (eventType.compare("captureEnd") == 0) {
        captureEndCallbackRef_ = callbackRef;
    } else if (eventType.compare("frameShutter") == 0) {
        frameShutterCallbackRef_ = callbackRef;
    } else if (eventType.compare("error") == 0) {
        errorCallbackRef_ = callbackRef;
    } else {
        MEDIA_ERR_LOG("Incorrect photo callback event type received from JS");
    }
}

void PhotoOutputCallback::UpdateJSCallback(std::string propName, const CallbackInfo &info) const
{
    napi_value result[ARGS_ONE];
    napi_value callback = nullptr;
    napi_value retVal;
    napi_value propValue;
    int32_t jsErrorCodeUnknown = -1;

    if (propName.compare("OnCaptureStarted") == 0) {
        CAMERA_NAPI_CHECK_NULL_PTR_RETURN_VOID(captureStartCallbackRef_,
            "OnCaptureStart callback is not registered by JS");
        napi_create_int32(env_, info.captureID, &result[PARAM0]);
        napi_get_reference_value(env_, captureStartCallbackRef_, &callback);
    } else if (propName.compare("OnCaptureEnded") == 0) {
        CAMERA_NAPI_CHECK_NULL_PTR_RETURN_VOID(captureEndCallbackRef_,
            "OnCaptureEnd callback is not registered by JS");
        napi_create_object(env_, &result[PARAM0]);
        napi_create_int32(env_, info.captureID, &propValue);
        napi_set_named_property(env_, result[PARAM0], "captureId", propValue);
        napi_create_int32(env_, info.frameCount, &propValue);
        napi_set_named_property(env_, result[PARAM0], "frameCount", propValue);
        napi_get_reference_value(env_, captureEndCallbackRef_, &callback);
    } else if (propName.compare("OnFrameShutter") == 0) {
        CAMERA_NAPI_CHECK_NULL_PTR_RETURN_VOID(frameShutterCallbackRef_,
            "OnFrameShutter callback is not registered by JS");
        napi_create_object(env_, &result[PARAM0]);
        napi_create_int32(env_, info.captureID, &propValue);
        napi_set_named_property(env_, result[PARAM0], "captureId", propValue);
        napi_create_int64(env_, info.timestamp, &propValue);
        napi_set_named_property(env_, result[PARAM0], "timestamp", propValue);
        napi_get_reference_value(env_, frameShutterCallbackRef_, &callback);
    } else {
        CAMERA_NAPI_CHECK_NULL_PTR_RETURN_VOID(errorCallbackRef_,
            "OnError callback is not registered by JS");
        napi_create_object(env_, &result[PARAM0]);
        napi_create_int32(env_, jsErrorCodeUnknown, &propValue);
        napi_set_named_property(env_, result[PARAM0], "code", propValue);
        napi_get_reference_value(env_, errorCallbackRef_, &callback);
        if (errorCallbackRef_ != nullptr) {
            napi_delete_reference(env_, errorCallbackRef_);
        }
    }

    napi_call_function(env_, nullptr, callback, ARGS_ONE, result, &retVal);
}

PhotoOutputNapi::PhotoOutputNapi() : env_(nullptr), wrapper_(nullptr)
{
}

PhotoOutputNapi::~PhotoOutputNapi()
{
    if (wrapper_ != nullptr) {
        napi_delete_reference(env_, wrapper_);
    }
    if (photoOutput_) {
        photoOutput_ = nullptr;
    }
    if (photoCallback_) {
        photoCallback_ = nullptr;
    }
}

void PhotoOutputNapi::PhotoOutputNapiDestructor(napi_env env, void* nativeObject, void* finalize_hint)
{
    MEDIA_DEBUG_LOG("PhotoOutputNapiDestructor enter");
    PhotoOutputNapi* photoOutput = reinterpret_cast<PhotoOutputNapi*>(nativeObject);
    if (photoOutput != nullptr) {
        delete photoOutput;
    }
}

napi_value PhotoOutputNapi::Init(napi_env env, napi_value exports)
{
    napi_status status;
    napi_value ctorObj;
    int32_t refCount = 1;

    napi_property_descriptor photo_output_props[] = {
        DECLARE_NAPI_FUNCTION("getDefaultCaptureSetting", GetDefaultCaptureSetting),
        DECLARE_NAPI_FUNCTION("capture", Capture),
        DECLARE_NAPI_FUNCTION("release", Release),
        DECLARE_NAPI_FUNCTION("isMirrorSupported", IsMirrorSupported),
        DECLARE_NAPI_FUNCTION("setMirror", SetMirror),
        DECLARE_NAPI_FUNCTION("on", On)
    };

    status = napi_define_class(env, CAMERA_PHOTO_OUTPUT_NAPI_CLASS_NAME, NAPI_AUTO_LENGTH,
                               PhotoOutputNapiConstructor, nullptr,
                               sizeof(photo_output_props) / sizeof(photo_output_props[PARAM0]),
                               photo_output_props, &ctorObj);
    if (status == napi_ok) {
        status = napi_create_reference(env, ctorObj, refCount, &sConstructor_);
        if (status == napi_ok) {
            status = napi_set_named_property(env, exports, CAMERA_PHOTO_OUTPUT_NAPI_CLASS_NAME, ctorObj);
            if (status == napi_ok) {
                return exports;
            }
        }
    }

    return nullptr;
}

// Constructor callback
napi_value PhotoOutputNapi::PhotoOutputNapiConstructor(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value result = nullptr;
    napi_value thisVar = nullptr;

    napi_get_undefined(env, &result);
    CAMERA_NAPI_GET_JS_OBJ_WITH_ZERO_ARGS(env, info, status, thisVar);

    if (status == napi_ok && thisVar != nullptr) {
        std::unique_ptr<PhotoOutputNapi> obj = std::make_unique<PhotoOutputNapi>();
        obj->env_ = env;
        obj->photoOutput_ = sPhotoOutput_;
        std::shared_ptr<PhotoOutputCallback> callback =
            std::make_shared<PhotoOutputCallback>(PhotoOutputCallback(env));
        ((sptr<PhotoOutput> &)(obj->photoOutput_))->SetCallback(callback);
        obj->photoCallback_ = callback;

        status = napi_wrap(env, thisVar, reinterpret_cast<void*>(obj.get()),
                           PhotoOutputNapi::PhotoOutputNapiDestructor, nullptr, nullptr);
        if (status == napi_ok) {
            obj.release();
            return thisVar;
        } else {
            MEDIA_ERR_LOG("Failure wrapping js to native napi");
        }
    }

    return result;
}

sptr<PhotoOutput> PhotoOutputNapi::GetPhotoOutput()
{
    return photoOutput_;
}

bool PhotoOutputNapi::IsPhotoOutput(napi_env env, napi_value obj)
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

napi_value PhotoOutputNapi::CreatePhotoOutput(napi_env env, Profile &profile, std::string surfaceId)
{
    CAMERA_SYNC_TRACE;
    napi_status status;
    napi_value result = nullptr;
    napi_value constructor;
    MEDIA_INFO_LOG("CreatePhotoOutput start");
    MEDIA_INFO_LOG("CreatePhotoOutput profile CameraFormat= %{public}d", profile.GetCameraFormat());
    napi_get_undefined(env, &result);
    status = napi_get_reference_value(env, sConstructor_, &constructor);
    if (status == napi_ok) {
        MEDIA_INFO_LOG("CreatePhotoOutput surfaceId: %{public}s", surfaceId.c_str());
        sptr<Surface> sface = Media::ImageReceiver::getSurfaceById(surfaceId);
        if (sface == nullptr) {
            MEDIA_ERR_LOG("failed to get surface from ImageReceiver");
            return result;
        }
        MEDIA_INFO_LOG("surface width: %{public}d, height: %{public}d", sface->GetDefaultWidth(),
                       sface->GetDefaultHeight());
        sface->SetUserData(CameraManager::surfaceFormat, std::to_string(profile.GetCameraFormat()));
        int retCode = CameraManager::GetInstance()->CreatePhotoOutput(profile, sface, &sPhotoOutput_);
        if (!CameraNapiUtils::CheckError(env, retCode)) {
            return nullptr;
        }
        if (sPhotoOutput_ == nullptr) {
            MEDIA_ERR_LOG("failed to create CreatePhotoOutput");
            return result;
        }
        status = napi_new_instance(env, constructor, 0, nullptr, &result);
        sPhotoOutput_ = nullptr;
        if (status == napi_ok && result != nullptr) {
            MEDIA_ERR_LOG("Success to create photo output instance");
            return result;
        } else {
            MEDIA_ERR_LOG("Failed to create photo output instance");
        }
    }
    MEDIA_INFO_LOG("CreatePhotoOutput get undefined");
    return result;
}

static void CommonCompleteCallback(napi_env env, napi_status status, void* data)
{
    auto context = static_cast<PhotoOutputAsyncContext*>(data);
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
            napi_get_boolean(env, context->isSupported, &jsContext->data);
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

int32_t QueryAndGetProperty(napi_env env, napi_value arg, const string &propertyName, napi_value &property)
{
    bool present = false;
    int32_t retval = 0;
    if ((napi_has_named_property(env, arg, propertyName.c_str(), &present) != napi_ok)
        || (!present) || (napi_get_named_property(env, arg, propertyName.c_str(), &property) != napi_ok)) {
            HiLog::Error(LABEL, "Failed to obtain property: %{public}s", propertyName.c_str());
            retval = -1;
    }

    return retval;
}

int32_t GetLocationProperties(napi_env env, napi_value locationObj, const PhotoOutputAsyncContext &context)
{
    PhotoOutputAsyncContext* asyncContext = const_cast<PhotoOutputAsyncContext *>(&context);
    napi_value latproperty = nullptr;
    napi_value lonproperty = nullptr;
    napi_value altproperty = nullptr;
    double latitude = -1.0;
    double longitude = -1.0;
    double altitude = -1.0;

    if ((QueryAndGetProperty(env, locationObj, "latitude", latproperty) == 0) &&
        (QueryAndGetProperty(env, locationObj, "longitude", lonproperty) == 0) &&
        (QueryAndGetProperty(env, locationObj, "altitude", altproperty) == 0)) {
        if ((napi_get_value_double(env, latproperty, &latitude) != napi_ok) ||
            (napi_get_value_double(env, lonproperty, &longitude) != napi_ok) ||
            (napi_get_value_double(env, altproperty, &altitude) != napi_ok)) {
            return -1;
        } else {
            asyncContext->location = std::make_unique<Location>();
            asyncContext->location->latitude = latitude;
            asyncContext->location->longitude = longitude;
            asyncContext->location->altitude = altitude;
        }
    } else {
        return -1;
    }

    return 0;
}

static void GetFetchOptionsParam(napi_env env, napi_value arg, const PhotoOutputAsyncContext &context, bool &err)
{
    PhotoOutputAsyncContext* asyncContext = const_cast<PhotoOutputAsyncContext *>(&context);
    int32_t intValue;
    std::string strValue;
    napi_value property = nullptr;
    PhotoCaptureSetting::QualityLevel quality;
    PhotoCaptureSetting::RotationConfig rotation;

    if (QueryAndGetProperty(env, arg, "quality", property) == 0) {
        if (napi_get_value_int32(env, property, &intValue) != napi_ok
            || CameraNapiUtils::MapQualityLevelFromJs(intValue, quality) == -1) {
            err = true;
            return;
        } else {
            asyncContext->quality = quality;
        }
    }

    if (QueryAndGetProperty(env, arg, "rotation", property) == 0) {
        if (napi_get_value_int32(env, property, &intValue) != napi_ok
            || CameraNapiUtils::MapImageRotationFromJs(intValue, rotation) == -1) {
            err = true;
            return;
        } else {
            asyncContext->rotation = rotation;
        }
    }

    if (QueryAndGetProperty(env, arg, "location", property) == 0) {
        if (GetLocationProperties(env, property, context) == -1) {
            err = true;
            return;
        }
    }

    if (QueryAndGetProperty(env, arg, "mirror", property) == 0) {
        bool isMirror = false;
        if (napi_get_value_bool(env, property, &isMirror) != napi_ok) {
            err = true;
            return;
        } else {
            asyncContext->isMirror = isMirror;
        }
    }
}

static napi_value ConvertJSArgsToNative(napi_env env, size_t argc, const napi_value argv[],
    PhotoOutputAsyncContext &asyncContext)
{
    const int32_t refCount = 1;
    napi_value result = nullptr;
    auto context = &asyncContext;
    bool err = false;

    NAPI_ASSERT(env, argv != nullptr, "Argument list is empty");

    for (size_t i = PARAM0; i < argc; i++) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[i], &valueType);
        if (i == PARAM0 && valueType == napi_object) {
            GetFetchOptionsParam(env, argv[PARAM0], asyncContext, err);
            if (err) {
                HiLog::Error(LABEL, "fetch options retrieval failed");
                NAPI_ASSERT(env, false, "type mismatch");
            }
            asyncContext.hasPhotoSettings = true;
        } else if ((i == PARAM0) && (valueType == napi_function)) {
            napi_create_reference(env, argv[i], refCount, &context->callbackRef);
            break;
        } else if ((i == PARAM1) && (valueType == napi_function)) {
            napi_create_reference(env, argv[i], refCount, &context->callbackRef);
            break;
        } else if ((i == PARAM0) && (valueType == napi_boolean)) {
            napi_get_value_bool(env, argv[i], &context->isSupported);
            break;
        } else if ((i == PARAM0) && (valueType == napi_undefined)) {
            break;
        } else {
            NAPI_ASSERT(env, false, "type mismatch");
        }
    }

    // Return true napi_value if params are successfully obtained
    napi_get_boolean(env, true, &result);
    return result;
}

napi_value PhotoOutputNapi::Capture(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value result = nullptr;
    size_t argc = ARGS_TWO;
    napi_value argv[ARGS_TWO] = {0};
    napi_value thisVar = nullptr;
    napi_value resource = nullptr;

    CAMERA_NAPI_GET_JS_ARGS(env, info, argc, argv, thisVar);

    napi_get_undefined(env, &result);
    unique_ptr<PhotoOutputAsyncContext> asyncContext = make_unique<PhotoOutputAsyncContext>();
    if (!CameraNapiUtils::CheckInvalidArgument(env, argc, ARGS_TWO, argv, PHOTO_OUT_CAPTURE)) {
        asyncContext->isInvalidArgument = true;
        asyncContext->status = false;
        asyncContext->errorCode = INVALID_ARGUMENT;
    }
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&asyncContext->objectInfo));
    if (status == napi_ok && asyncContext->objectInfo != nullptr) {
        if (!asyncContext->isInvalidArgument) {
            result = ConvertJSArgsToNative(env, argc, argv, *asyncContext);
        }
        CAMERA_NAPI_CHECK_NULL_PTR_RETURN_UNDEFINED(env, result, result, "Failed to obtain arguments");
        CAMERA_NAPI_CREATE_PROMISE(env, asyncContext->callbackRef, asyncContext->deferred, result);
        CAMERA_NAPI_CREATE_RESOURCE_NAME(env, resource, "Capture");
        status = napi_create_async_work(
            env, nullptr, resource, [](napi_env env, void* data) {
                PhotoOutputAsyncContext* context = static_cast<PhotoOutputAsyncContext*>(data);
                // Start async trace
                context->funcName = "PhotoOutputNapi::Capture";
                context->taskId = CameraNapiUtils::IncreamentAndGet(photoOutputTaskId);
                if (context->isInvalidArgument) {
                    return;
                }
                CAMERA_START_ASYNC_TRACE(context->funcName, context->taskId);
                if (context->objectInfo == nullptr) {
                    context->status = false;
                    return;
                }

                context->bRetBool = false;
                context->status = true;
                sptr<PhotoOutput> photoOutput = ((sptr<PhotoOutput> &)(context->objectInfo->photoOutput_));
                if ((context->hasPhotoSettings)) {
                    std::shared_ptr<PhotoCaptureSetting> capSettings = make_shared<PhotoCaptureSetting>();

                    if (context->quality != -1) {
                        capSettings->SetQuality(
                            static_cast<PhotoCaptureSetting::QualityLevel>(context->quality));
                    }

                    if (context->rotation != -1) {
                        capSettings->SetRotation(
                            static_cast<PhotoCaptureSetting::RotationConfig>(context->rotation));
                    }

                    capSettings->SetMirror(context->isMirror);

                    if (context->location != nullptr) {
                        capSettings->SetLocation(context->location);
                    }

                    context->errorCode = photoOutput->Capture(capSettings);
                } else {
                    context->errorCode = photoOutput->Capture();
                }
                context->status = context->errorCode == 0;
            }, CommonCompleteCallback, static_cast<void*>(asyncContext.get()), &asyncContext->work);
        if (status != napi_ok) {
            MEDIA_ERR_LOG("Failed to create napi_create_async_work for PhotoOutputNapi::Capture");
            napi_get_undefined(env, &result);
        } else {
            napi_queue_async_work(env, asyncContext->work);
            asyncContext.release();
        }
    }

    return result;
}

napi_value PhotoOutputNapi::Release(napi_env env, napi_callback_info info)
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
    std::unique_ptr<PhotoOutputAsyncContext> asyncContext = std::make_unique<PhotoOutputAsyncContext>();
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&asyncContext->objectInfo));
    if (status == napi_ok && asyncContext->objectInfo != nullptr) {
        if (argc == ARGS_ONE) {
            CAMERA_NAPI_GET_JS_ASYNC_CB_REF(env, argv[PARAM0], refCount, asyncContext->callbackRef);
        }

        CAMERA_NAPI_CREATE_PROMISE(env, asyncContext->callbackRef, asyncContext->deferred, result);
        CAMERA_NAPI_CREATE_RESOURCE_NAME(env, resource, "Release");

        status = napi_create_async_work(
            env, nullptr, resource, [](napi_env env, void* data) {
                auto context = static_cast<PhotoOutputAsyncContext*>(data);
                context->status = false;
                // Start async trace
                context->funcName = "PhotoOutputNapi::Release";
                context->taskId = CameraNapiUtils::IncreamentAndGet(photoOutputTaskId);
                CAMERA_START_ASYNC_TRACE(context->funcName, context->taskId);
                if (context->objectInfo != nullptr && context->objectInfo->photoOutput_ != nullptr) {
                    context->bRetBool = false;
                    context->status = true;
                    ((sptr<PhotoOutput> &)(context->objectInfo->photoOutput_))->Release();
                }
            },
            CommonCompleteCallback, static_cast<void*>(asyncContext.get()), &asyncContext->work);
        if (status != napi_ok) {
            MEDIA_ERR_LOG("Failed to create napi_create_async_work for PhotoOutputNapi::Release");
            napi_get_undefined(env, &result);
        } else {
            napi_queue_async_work(env, asyncContext->work);
            asyncContext.release();
        }
    }

    return result;
}

napi_value PhotoOutputNapi::GetDefaultCaptureSetting(napi_env env, napi_callback_info info)
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
    std::unique_ptr<PhotoOutputAsyncContext> asyncContext = std::make_unique<PhotoOutputAsyncContext>();
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&asyncContext->objectInfo));
    if (status == napi_ok && asyncContext->objectInfo != nullptr) {
        if (argc == ARGS_ONE) {
            CAMERA_NAPI_GET_JS_ASYNC_CB_REF(env, argv[PARAM0], refCount, asyncContext->callbackRef);
        }

        CAMERA_NAPI_CREATE_PROMISE(env, asyncContext->callbackRef, asyncContext->deferred, result);
        CAMERA_NAPI_CREATE_RESOURCE_NAME(env, resource, "GetDefaultCaptureSetting");

        status = napi_create_async_work(
            env, nullptr, resource, [](napi_env env, void* data) {
                auto context = static_cast<PhotoOutputAsyncContext*>(data);
                context->status = false;
                // Start async trace
                context->funcName = "PhotoOutputNapi::GetDefaultCaptureSetting";
                context->taskId = CameraNapiUtils::IncreamentAndGet(photoOutputTaskId);
                CAMERA_START_ASYNC_TRACE(context->funcName, context->taskId);
                if (context->objectInfo != nullptr) {
                    context->bRetBool = false;
                    context->status = true;
                }
            },
            CommonCompleteCallback, static_cast<void*>(asyncContext.get()), &asyncContext->work);
        if (status != napi_ok) {
            MEDIA_ERR_LOG("Failed to create napi_create_async_work for PhotoOutputNapi::GetDefaultCaptureSetting");
            napi_get_undefined(env, &result);
        } else {
            napi_queue_async_work(env, asyncContext->work);
            asyncContext.release();
        }
    }

    return result;
}

napi_value PhotoOutputNapi::IsMirrorSupported(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value result = nullptr;
    size_t argc = ARGS_ZERO;
    napi_value argv[ARGS_ZERO];
    napi_value thisVar = nullptr;

    CAMERA_NAPI_GET_JS_ARGS(env, info, argc, argv, thisVar);

    napi_get_undefined(env, &result);
    PhotoOutputNapi* photoOutputNapi = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&photoOutputNapi));
    if (status == napi_ok && photoOutputNapi != nullptr) {
        bool isSupported = photoOutputNapi->photoOutput_->IsMirrorSupported();
        napi_get_boolean(env, isSupported, &result);
    }

    return result;
}

napi_value PhotoOutputNapi::SetMirror(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value result = nullptr;
    const int32_t refCount = 1;
    napi_value resource = nullptr;
    size_t argc = ARGS_TWO;
    napi_value argv[ARGS_TWO] = {0};
    napi_value thisVar = nullptr;

    CAMERA_NAPI_GET_JS_ARGS(env, info, argc, argv, thisVar);
    NAPI_ASSERT(env, argc <= ARGS_TWO, "requires 2 parameters maximum");

    napi_get_undefined(env, &result);
    std::unique_ptr<PhotoOutputAsyncContext> asyncContext = std::make_unique<PhotoOutputAsyncContext>();
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&asyncContext->objectInfo));
    if (status == napi_ok && asyncContext->objectInfo != nullptr) {
        if (argc == ARGS_TWO) {
            CAMERA_NAPI_GET_JS_ASYNC_CB_REF(env, argv[PARAM1], refCount, asyncContext->callbackRef);
        }

        result = ConvertJSArgsToNative(env, argc, argv, *asyncContext);
        asyncContext->isMirror = asyncContext->isSupported;
        CAMERA_NAPI_CREATE_PROMISE(env, asyncContext->callbackRef, asyncContext->deferred, result);
        CAMERA_NAPI_CREATE_RESOURCE_NAME(env, resource, "SetMirror");
        status = napi_create_async_work(
            env, nullptr, resource, [](napi_env env, void* data) {
                auto context = static_cast<PhotoOutputAsyncContext*>(data);
                context->status = false;
                // Start async trace
                context->funcName = "PhotoOutputNapi::SetMirror";
                context->taskId = CameraNapiUtils::IncreamentAndGet(photoOutputTaskId);
                CAMERA_START_ASYNC_TRACE(context->funcName, context->taskId);
                if (context->objectInfo != nullptr) {
                    context->bRetBool = false;
                    context->status = true;
                }
            },
            CommonCompleteCallback, static_cast<void*>(asyncContext.get()), &asyncContext->work);
        if (status != napi_ok) {
            MEDIA_ERR_LOG("Failed to create napi_create_async_work for SetMirror");
            napi_get_undefined(env, &result);
        } else {
            napi_queue_async_work(env, asyncContext->work);
            asyncContext.release();
        }
    }

    return result;
}

napi_value PhotoOutputNapi::On(napi_env env, napi_callback_info info)
{
    CAMERA_SYNC_TRACE;
    napi_value undefinedResult = nullptr;
    size_t argCount = ARGS_TWO;
    napi_value argv[ARGS_TWO] = {nullptr};
    napi_value thisVar = nullptr;
    size_t res = 0;
    char buffer[SIZE];
    const int32_t refCount = 1;
    PhotoOutputNapi* obj = nullptr;
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
        std::string eventType = std::string(buffer);

        napi_ref callbackRef;
        napi_create_reference(env, argv[PARAM1], refCount, &callbackRef);

        if (!eventType.empty()) {
            obj->photoCallback_->SetCallbackRef(eventType, callbackRef);
        } else {
            MEDIA_ERR_LOG("Failed to Register Callback: event type is empty!");
        }
    }
    return undefinedResult;
}
} // namespace CameraStandard
} // namespace OHOS
