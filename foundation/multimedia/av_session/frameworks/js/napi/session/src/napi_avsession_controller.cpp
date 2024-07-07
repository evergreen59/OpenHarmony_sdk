/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "key_event.h"
#include "napi_async_work.h"
#include "napi_avcontroller_callback.h"
#include "napi_avsession_controller.h"
#include "napi_control_command.h"
#include "napi_meta_data.h"
#include "napi_playback_state.h"
#include "napi_utils.h"
#include "want_agent.h"
#include "avsession_errors.h"
#include "avsession_trace.h"
#include "napi_avsession_manager.h"

namespace OHOS::AVSession {
static __thread napi_ref AVControllerConstructorRef = nullptr;
std::map<std::string, std::pair<NapiAVSessionController::OnEventHandlerType,
    NapiAVSessionController::OffEventHandlerType>> NapiAVSessionController::EventHandlers_ = {
    { "sessionDestroy", { OnSessionDestroy, OffSessionDestroy } },
    { "metadataChange", { OnMetaDataChange, OffMetaDataChange } },
    { "playbackStateChange", { OnPlaybackStateChange, OffPlaybackStateChange } },
    { "activeStateChange", { OnActiveStateChange, OffActiveStateChange } },
    { "validCommandChange", { OnValidCommandChange, OffValidCommandChange } },
    { "outputDeviceChange", { OnOutputDeviceChange, OffOutputDeviceChange } },
};

NapiAVSessionController::NapiAVSessionController()
{
    SLOGI("construct");
}

NapiAVSessionController::~NapiAVSessionController()
{
    SLOGI("destroy");
}

napi_value NapiAVSessionController::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor descriptors[] = {
        DECLARE_NAPI_FUNCTION("on", OnEvent),
        DECLARE_NAPI_FUNCTION("off", OffEvent),
        DECLARE_NAPI_FUNCTION("getAVPlaybackState", GetAVPlaybackState),
        DECLARE_NAPI_FUNCTION("getAVMetadata", GetAVMetaData),
        DECLARE_NAPI_FUNCTION("getOutputDevice", GetOutputDevice),
        DECLARE_NAPI_FUNCTION("sendAVKeyEvent", SendAVKeyEvent),
        DECLARE_NAPI_FUNCTION("getLaunchAbility", GetLaunchAbility),
        DECLARE_NAPI_FUNCTION("getRealPlaybackPositionSync", GetRealPlaybackPositionSync),
        DECLARE_NAPI_FUNCTION("isActive", IsSessionActive),
        DECLARE_NAPI_FUNCTION("destroy", Destroy),
        DECLARE_NAPI_FUNCTION("getValidCommands", GetValidCommands),
        DECLARE_NAPI_FUNCTION("sendControlCommand", SendControlCommand),
    };

    auto property_count = sizeof(descriptors) / sizeof(napi_property_descriptor);
    napi_value constructor{};
    auto status = napi_define_class(env, "AVSessionController", NAPI_AUTO_LENGTH, ConstructorCallback, nullptr,
        property_count, descriptors, &constructor);
    if (status != napi_ok) {
        SLOGE("define class failed");
        return NapiUtils::GetUndefinedValue(env);
    }
    napi_create_reference(env, constructor, 1, &AVControllerConstructorRef);
    return exports;
}

napi_value NapiAVSessionController::ConstructorCallback(napi_env env, napi_callback_info info)
{
    napi_value self;
    NAPI_CALL_BASE(env, napi_get_cb_info(env, info, nullptr, nullptr, &self, nullptr), nullptr);

    auto finalize = [](napi_env env, void* data, void* hint) {
        auto* napiController = reinterpret_cast<NapiAVSessionController*>(data);
        napi_delete_reference(env, napiController->wrapperRef_);
        delete napiController;
    };

    auto* napiController = new(std::nothrow) NapiAVSessionController();
    if (napiController == nullptr) {
        SLOGE("no memory");
        return nullptr;
    }
    if (napi_wrap(env, self, static_cast<void*>(napiController), finalize, nullptr,
                  &(napiController->wrapperRef_)) != napi_ok) {
        SLOGE("wrap failed");
        return nullptr;
    }
    return self;
}

napi_status NapiAVSessionController::NewInstance(napi_env env, std::shared_ptr<AVSessionController>& nativeController,
    napi_value& out)
{
    napi_value constructor{};
    NAPI_CALL_BASE(env, napi_get_reference_value(env, AVControllerConstructorRef, &constructor), napi_generic_failure);
    napi_value instance{};
    NAPI_CALL_BASE(env, napi_new_instance(env, constructor, 0, nullptr, &instance), napi_generic_failure);
    NapiAVSessionController* napiController{};
    NAPI_CALL_BASE(env, napi_unwrap(env, instance, reinterpret_cast<void**>(&napiController)), napi_generic_failure);
    napiController->controller_ = std::move(nativeController);
    napiController->sessionId_ = napiController->controller_->GetSessionId();

    napi_value property {};
    auto status = NapiUtils::SetValue(env, napiController->sessionId_, property);
    CHECK_RETURN(status == napi_ok, "create object failed", napi_generic_failure);
    NAPI_CALL_BASE(env, napi_set_named_property(env, instance, "sessionId", property), napi_generic_failure);

    out = instance;
    return napi_ok;
}

napi_value NapiAVSessionController::GetAVPlaybackState(napi_env env, napi_callback_info info)
{
    struct ConcreteContext : public ContextBase {
        AVPlaybackState state;
    };
    auto context = std::make_shared<ConcreteContext>();
    context->GetCbInfo(env, info);

    auto executor = [context]() {
        auto* napiController = reinterpret_cast<NapiAVSessionController*>(context->native);
        if (napiController->controller_ == nullptr) {
            SLOGE("GetAVPlaybackState failed : controller is nullptr");
            context->status = napi_generic_failure;
            context->errMessage = "GetAVPlaybackState failed : controller is nullptr";
            context->errCode = NapiAVSessionManager::errcode_[ERR_CONTROLLER_NOT_EXIST];
            return;
        }
        int32_t ret = napiController->controller_->GetAVPlaybackState(context->state);
        if (ret != AVSESSION_SUCCESS) {
            if (ret == ERR_SESSION_NOT_EXIST) {
                context->errMessage = "GetAVPlaybackState failed : native session not exist";
            } else if (ret == ERR_CONTROLLER_NOT_EXIST) {
                context->errMessage = "GetAVPlaybackState failed : native controller not exist";
            } else {
                context->errMessage = "GetAVPlaybackState failed : native server exception";
            }
            SLOGE("controller GetAVPlaybackState failed:%{public}d", ret);
            context->status = napi_generic_failure;
            context->errCode = NapiAVSessionManager::errcode_[ret];
        }
    };

    auto complete = [env, context](napi_value& output) {
        context->status = NapiPlaybackState::SetValue(env, context->state, output);
        CHECK_STATUS_RETURN_VOID(context, "convert native object to javascript object failed",
            NapiAVSessionManager::errcode_[AVSESSION_ERROR]);
    };
    return NapiAsyncWork::Enqueue(env, context, "GetAVPlaybackState", executor, complete);
}

napi_value NapiAVSessionController::GetAVMetaData(napi_env env, napi_callback_info info)
{
    struct ConcreteContext : public ContextBase {
        AVMetaData data;
    };
    auto context = std::make_shared<ConcreteContext>();
    context->GetCbInfo(env, info);

    auto executor = [context]() {
        auto* napiController = reinterpret_cast<NapiAVSessionController*>(context->native);
        if (napiController->controller_ == nullptr) {
            SLOGE("GetAVMetaData failed : controller is nullptr");
            context->status = napi_generic_failure;
            context->errMessage = "GetAVMetaData failed : controller is nullptr";
            context->errCode = NapiAVSessionManager::errcode_[ERR_CONTROLLER_NOT_EXIST];
            return;
        }
        int32_t ret = napiController->controller_->GetAVMetaData(context->data);
        if (ret != AVSESSION_SUCCESS) {
            if (ret == ERR_SESSION_NOT_EXIST) {
                context->errMessage = "GetAVMetaData failed : native session not exist";
            } else if (ret == ERR_CONTROLLER_NOT_EXIST) {
                context->errMessage = "GetAVMetaData failed : native controller not exist";
            } else {
                context->errMessage = "GetAVMetaData failed : native server exception";
            }
            SLOGE("controller GetAVMetaData failed:%{public}d", ret);
            context->status = napi_generic_failure;
            context->errCode = NapiAVSessionManager::errcode_[ret];
        }
    };

    auto complete = [env, context](napi_value& output) {
        context->status = NapiMetaData::SetValue(env, context->data, output);
        CHECK_STATUS_RETURN_VOID(context, "convert native object to javascript object failed",
            NapiAVSessionManager::errcode_[AVSESSION_ERROR]);
    };

    return NapiAsyncWork::Enqueue(env, context, "GetAVMetaData", executor, complete);
}

napi_value NapiAVSessionController::SendAVKeyEvent(napi_env env, napi_callback_info info)
{
    AVSESSION_TRACE_SYNC_START("NapiAVSessionController::SendAVKeyEvent");
    struct ConcreteContext : public ContextBase {
        std::shared_ptr<MMI::KeyEvent> keyEvent_;
    };
    auto context = std::make_shared<ConcreteContext>();
    auto input = [env, context](size_t argc, napi_value* argv) {
        CHECK_ARGS_RETURN_VOID(context, argc == ARGC_ONE, "invalid arguments",
            NapiAVSessionManager::errcode_[ERR_INVALID_PARAM]);
        context->status = NapiUtils::GetValue(env, argv[ARGV_FIRST], context->keyEvent_);
        CHECK_ARGS_RETURN_VOID(context, (context->status == napi_ok) && (context->keyEvent_ != nullptr),
            "invalid keyEvent", NapiAVSessionManager::errcode_[ERR_INVALID_PARAM]);
    };
    context->GetCbInfo(env, info, input);
    context->taskId = NAPI_SEND_AV_KEY_EVENT_TASK_ID;

    auto executor = [context]() {
        auto* napiController = reinterpret_cast<NapiAVSessionController*>(context->native);
        if (napiController->controller_ == nullptr) {
            SLOGE("SendAVKeyEvent failed : controller is nullptr");
            context->status = napi_generic_failure;
            context->errMessage = "SendAVKeyEvent failed : controller is nullptr";
            context->errCode = NapiAVSessionManager::errcode_[ERR_CONTROLLER_NOT_EXIST];
            return;
        }
        int32_t ret = napiController->controller_->SendAVKeyEvent(*context->keyEvent_);
        if (ret != AVSESSION_SUCCESS) {
            if (ret == ERR_SESSION_NOT_EXIST) {
                context->errMessage = "SendAVKeyEvent failed : native session not exist";
            } else if (ret == ERR_CONTROLLER_NOT_EXIST) {
                context->errMessage = "SendAVKeyEvent failed : native controller not exist";
            } else if (ret == ERR_SESSION_DEACTIVE) {
                context->errMessage = "SendAVKeyEvent failed : native session is not active";
            } else if (ret == ERR_COMMAND_NOT_SUPPORT) {
                context->errMessage = "SendAVKeyEvent failed : native invalid KeyEvent";
            } else {
                context->errMessage = "SendAVKeyEvent failed : native server exception";
            }
            SLOGE("controller SendAVKeyEvent failed:%{public}d", ret);
            context->status = napi_generic_failure;
            context->errCode = NapiAVSessionManager::errcode_[ret];
        }
    };

    return NapiAsyncWork::Enqueue(env, context, "SendAVKeyEvent", executor);
}

napi_value NapiAVSessionController::GetLaunchAbility(napi_env env, napi_callback_info info)
{
    struct ConcreteContext : public ContextBase {
        AbilityRuntime::WantAgent::WantAgent ability;
    };
    auto context = std::make_shared<ConcreteContext>();
    context->GetCbInfo(env, info);

    auto executor = [context]() {
        auto* napiController = reinterpret_cast<NapiAVSessionController*>(context->native);
        if (napiController->controller_ == nullptr) {
            SLOGE("GetLaunchAbility failed : controller is nullptr");
            context->status = napi_generic_failure;
            context->errMessage = "GetLaunchAbility failed : controller is nullptr";
            context->errCode = NapiAVSessionManager::errcode_[ERR_CONTROLLER_NOT_EXIST];
            return;
        }
        int32_t ret = napiController->controller_->GetLaunchAbility(context->ability);
        if (ret != AVSESSION_SUCCESS) {
            if (ret == ERR_SESSION_NOT_EXIST) {
                context->errMessage = "GetLaunchAbility failed : native session not exist";
            } else if (ret == ERR_CONTROLLER_NOT_EXIST) {
                context->errMessage = "GetLaunchAbility failed : native controller not exist";
            } else {
                context->errMessage = "GetLaunchAbility failed : native server exception";
            }
            SLOGE("controller GetLaunchAbility failed:%{public}d", ret);
            context->status = napi_generic_failure;
            context->errCode = NapiAVSessionManager::errcode_[ret];
        }
    };

    auto complete = [env, context](napi_value& output) {
        context->status = NapiUtils::SetValue(env, context->ability, output);
        CHECK_STATUS_RETURN_VOID(context, "convert native object to javascript object failed",
            NapiAVSessionManager::errcode_[AVSESSION_ERROR]);
    };

    return NapiAsyncWork::Enqueue(env, context, "GetLaunchAbility", executor, complete);
}

napi_value NapiAVSessionController::GetValidCommands(napi_env env, napi_callback_info info)
{
    struct ConcreteContext : public ContextBase {
        std::vector<std::string> stringCmds;
    };
    auto context = std::make_shared<ConcreteContext>();
    context->GetCbInfo(env, info);

    auto executor = [context]() {
        auto* napiController = reinterpret_cast<NapiAVSessionController*>(context->native);
        if (napiController->controller_ == nullptr) {
            SLOGE("GetValidCommands failed : controller is nullptr");
            context->status = napi_generic_failure;
            context->errMessage = "GetValidCommands failed : controller is nullptr";
            context->errCode = NapiAVSessionManager::errcode_[ERR_CONTROLLER_NOT_EXIST];
            return;
        }
        std::vector<int32_t> cmds;
        int32_t ret = napiController->controller_->GetValidCommands(cmds);
        if (ret != AVSESSION_SUCCESS) {
            if (ret == ERR_SESSION_NOT_EXIST) {
                context->errMessage = "GetValidCommands failed : native session not exist";
            } else if (ret == ERR_CONTROLLER_NOT_EXIST) {
                context->errMessage = "GetValidCommands failed : native controller not exist";
            } else {
                context->errMessage = "GetValidCommands failed : native server exception";
            }
            SLOGE("controller GetValidCommands failed:%{public}d", ret);
            context->status = napi_generic_failure;
            context->errCode = NapiAVSessionManager::errcode_[ret];
        }
        context->stringCmds = NapiControlCommand::ConvertCommands(cmds);
    };

    auto complete = [env, context](napi_value& output) {
        context->status = NapiUtils::SetValue(env, context->stringCmds, output);
        CHECK_STATUS_RETURN_VOID(context, "convert native object to javascript object failed",
            NapiAVSessionManager::errcode_[AVSESSION_ERROR]);
    };

    return NapiAsyncWork::Enqueue(env, context, "GetValidCommands", executor, complete);
}

napi_value NapiAVSessionController::IsSessionActive(napi_env env, napi_callback_info info)
{
    struct ConcreteContext : public ContextBase {
        bool isActive {};
    };
    auto context = std::make_shared<ConcreteContext>();
    context->GetCbInfo(env, info);

    auto executor = [context]() {
        auto* napiController = reinterpret_cast<NapiAVSessionController*>(context->native);
        if (napiController->controller_ == nullptr) {
            SLOGE("IsSessionActive failed : controller is nullptr");
            context->status = napi_generic_failure;
            context->errMessage = "IsSessionActive failed : controller is nullptr";
            context->errCode = NapiAVSessionManager::errcode_[ERR_CONTROLLER_NOT_EXIST];
            return;
        }
        int32_t ret = napiController->controller_->IsSessionActive(context->isActive);
        if (ret != AVSESSION_SUCCESS) {
            if (ret == ERR_SESSION_NOT_EXIST) {
                context->errMessage = "IsSessionActive failed : native session not exist";
            } else if (ret == ERR_CONTROLLER_NOT_EXIST) {
                context->errMessage = "IsSessionActive failed : native controller not exist";
            } else {
                context->errMessage = "IsSessionActive failed : native server exception";
            }
            SLOGE("controller IsSessionActive failed:%{public}d", ret);
            context->status = napi_generic_failure;
            context->errCode = NapiAVSessionManager::errcode_[ret];
        }
    };

    auto complete = [env, context](napi_value& output) {
        context->status = NapiUtils::SetValue(env, context->isActive, output);
        CHECK_STATUS_RETURN_VOID(context, "convert native object to javascript object failed",
            NapiAVSessionManager::errcode_[AVSESSION_ERROR]);
    };

    return NapiAsyncWork::Enqueue(env, context, "IsSessionActive", executor, complete);
}

napi_value NapiAVSessionController::SendControlCommand(napi_env env, napi_callback_info info)
{
    AVSESSION_TRACE_SYNC_START("NapiAVSessionController::SendControlCommand");
    struct ConcrentContext : public ContextBase {
        AVControlCommand command;
    };
    auto context = std::make_shared<ConcrentContext>();
    auto input = [env, context](size_t argc, napi_value* argv) {
        CHECK_ARGS_RETURN_VOID(context, argc == ARGC_ONE, "invalid arguments",
            NapiAVSessionManager::errcode_[ERR_INVALID_PARAM]);
        context->status = NapiControlCommand::GetValue(env, argv[ARGV_FIRST], context->command);
        CHECK_ARGS_RETURN_VOID(context, (context->status == napi_ok), "invalid command",
            NapiAVSessionManager::errcode_[ERR_INVALID_PARAM]);
    };
    context->GetCbInfo(env, info, input);
    context->taskId = NAPI_SEND_CONTROL_COMMAND_TASK_ID;

    auto executor = [context]() {
        auto* napiController = reinterpret_cast<NapiAVSessionController*>(context->native);
        if (napiController->controller_ == nullptr) {
            SLOGE("SendControlCommand failed : controller is nullptr");
            context->status = napi_generic_failure;
            context->errMessage = "SendControlCommand failed : controller is nullptr";
            context->errCode = NapiAVSessionManager::errcode_[ERR_CONTROLLER_NOT_EXIST];
            return;
        }
        int32_t ret = napiController->controller_->SendControlCommand(context->command);
        if (ret != AVSESSION_SUCCESS) {
            if (ret == ERR_SESSION_NOT_EXIST) {
                context->errMessage = "SendControlCommand failed : native session not exist";
            } else if (ret == ERR_CONTROLLER_NOT_EXIST) {
                context->errMessage = "SendControlCommand failed : native controller not exist";
            } else if (ret == ERR_SESSION_DEACTIVE) {
                context->errMessage = "SendControlCommand failed : native session is not active";
            } else if (ret == ERR_COMMAND_NOT_SUPPORT) {
                context->errMessage = "SendControlCommand failed : native command not support";
            } else if (ret == ERR_COMMAND_SEND_EXCEED_MAX) {
                context->errMessage = "SendControlCommand failed : native command send nums overload";
            } else {
                context->errMessage = "SendControlCommand failed : native server exception";
            }
            SLOGE("controller SendControlCommand failed:%{public}d", ret);
            context->status = napi_generic_failure;
            context->errCode = NapiAVSessionManager::errcode_[ret];
        }
    };

    return NapiAsyncWork::Enqueue(env, context, "SendControlCommand", executor);
}

napi_value NapiAVSessionController::Destroy(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<ContextBase>();
    if (context == nullptr) {
        SLOGE("OnEvent failed : no memory");
        NapiUtils::ThrowError(env, "OnEvent failed : no memory", NapiAVSessionManager::errcode_[ERR_NO_MEMORY]);
        return NapiUtils::GetUndefinedValue(env);
    }

    context->GetCbInfo(env, info);

    auto executor = [context]() {
        auto* napiController = reinterpret_cast<NapiAVSessionController*>(context->native);
        if (napiController->controller_ == nullptr) {
            SLOGE("Destroy controller failed : controller is nullptr");
            context->status = napi_generic_failure;
            context->errMessage = "Destroy controller failed : controller is nullptr";
            context->errCode = NapiAVSessionManager::errcode_[ERR_CONTROLLER_NOT_EXIST];
            return;
        }
        int32_t ret = napiController->controller_->Destroy();
        if (ret != AVSESSION_SUCCESS) {
            if (ret == ERR_CONTROLLER_NOT_EXIST) {
                context->errMessage = "Destroy controller failed : native controller not exist";
            } else {
                context->errMessage = "Destroy controller failed : native server exception";
            }
            SLOGE("controller Destroy failed:%{public}d", ret);
            context->status = napi_generic_failure;
            context->errCode = NapiAVSessionManager::errcode_[ret];
            return;
        }
        napiController->controller_ = nullptr;
        napiController->callback_ = nullptr;
    };

    return NapiAsyncWork::Enqueue(env, context, "IsSessionActive", executor);
}

napi_value NapiAVSessionController::GetRealPlaybackPositionSync(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<ContextBase>();
    if (context == nullptr) {
        SLOGE("OnEvent failed : no memory");
        NapiUtils::ThrowError(env, "OnEvent failed : no memory", NapiAVSessionManager::errcode_[ERR_NO_MEMORY]);
        return NapiUtils::GetUndefinedValue(env);
    }

    context->GetCbInfo(env, info, NapiCbInfoParser(), true);

    auto* napiController = reinterpret_cast<NapiAVSessionController*>(context->native);
    if (napiController->controller_ == nullptr) {
        SLOGI("GetRealPlaybackPositionSync failed : controller is nullptr");
        NapiUtils::ThrowError(env, "GetRealPlaybackPositionSync failed : controller is nullptr",
            NapiAVSessionManager::errcode_[ERR_CONTROLLER_NOT_EXIST]);
        return NapiUtils::GetUndefinedValue(env);
    }

    auto position = napiController->controller_->GetRealPlaybackPosition();
    napi_value output {};
    auto status = NapiUtils::SetValue(env, position, output);
    if (status != napi_ok) {
        SLOGE("convert native object to javascript object failed");
        NapiUtils::ThrowError(env, "convert native object to javascript object failed",
            NapiAVSessionManager::errcode_[AVSESSION_ERROR]);
        return NapiUtils::GetUndefinedValue(env);
    }
    return output;
}

napi_value NapiAVSessionController::GetOutputDevice(napi_env env, napi_callback_info info)
{
    struct ConcreteContext : public ContextBase {
        OutputDeviceInfo outputDeviceInfo_;
    };
    auto context = std::make_shared<ConcreteContext>();
    context->GetCbInfo(env, info);

    auto executor = [context]() {
        auto* napiController = reinterpret_cast<NapiAVSessionController*>(context->native);
        if (napiController->controller_ == nullptr) {
            SLOGE("GetOutputDevice failed : controller is nullptr");
            context->status = napi_generic_failure;
            context->errMessage = "GetOutputDevice failed : controller is nullptr";
            context->errCode = NapiAVSessionManager::errcode_[ERR_CONTROLLER_NOT_EXIST];
            return;
        }
        AVSessionDescriptor descriptor;
        AVSessionManager::GetInstance().GetSessionDescriptorsBySessionId(napiController->controller_->GetSessionId(),
                                                                         descriptor);
        context->outputDeviceInfo_ = descriptor.outputDeviceInfo_;
    };

    auto complete = [env, context](napi_value& output) {
        context->status = NapiUtils::SetValue(env, context->outputDeviceInfo_, output);
        CHECK_STATUS_RETURN_VOID(context, "convert native object to javascript object failed",
            NapiAVSessionManager::errcode_[AVSESSION_ERROR]);
    };
    return NapiAsyncWork::Enqueue(env, context, "GetOutputDevice", executor, complete);
}

napi_status NapiAVSessionController::SetPlaybackStateFilter(napi_env env, NapiAVSessionController *napiController,
                                                            napi_value filter)
{
    AVPlaybackState::PlaybackStateMaskType playbackMask;
    auto status = NapiPlaybackState::ConvertFilter(env, filter, playbackMask);
    CHECK_RETURN(status == napi_ok, "convert filter failed", status);
    auto ret = napiController->controller_->SetPlaybackFilter(playbackMask);
    if (ret != AVSESSION_SUCCESS) {
        SLOGE("controller SetPlaybackFilter failed:%{public}d", ret);
        status = napi_generic_failure;
    }
    return status;
}

napi_status NapiAVSessionController::SetMetaFilter(napi_env env, NapiAVSessionController* napiController,
                                                   napi_value filter)
{
    AVMetaData::MetaMaskType metaMask;
    auto status = NapiMetaData::ConvertFilter(env, filter, metaMask);
    CHECK_RETURN(status == napi_ok, "convert filter failed", status);
    auto ret = napiController->controller_->SetMetaFilter(metaMask);
    if (ret != AVSESSION_SUCCESS) {
        SLOGE("controller SetMetaFilter failed:%{public}d", ret);
        status = napi_generic_failure;
    }
    return status;
}

napi_status NapiAVSessionController::RegisterCallback(napi_env env, const std::shared_ptr<ContextBase>& context,
    const std::string& event, napi_value filter, napi_value callback)
{
    auto it = EventHandlers_.find(event);
    if (it == EventHandlers_.end()) {
        SLOGE("event name invalid");
        NapiUtils::ThrowError(env, "event name invalid", NapiAVSessionManager::errcode_[ERR_INVALID_PARAM]);
        return napi_generic_failure;
    }
    auto* napiController = reinterpret_cast<NapiAVSessionController*>(context->native);
    if (napiController->controller_ == nullptr) {
        SLOGE("OnEvent failed : controller is nullptr");
        NapiUtils::ThrowError(env, "OnEvent failed : controller is nullptr",
            NapiAVSessionManager::errcode_[ERR_CONTROLLER_NOT_EXIST]);
        return napi_generic_failure;
    }
    if (napiController->callback_ == nullptr) {
        napiController->callback_= std::make_shared<NapiAVControllerCallback>();
        if (napiController->callback_ == nullptr) {
            SLOGE("OnEvent failed : no memory");
            NapiUtils::ThrowError(env, "OnEvent failed : no memory",
                NapiAVSessionManager::errcode_[ERR_NO_MEMORY]);
            return napi_generic_failure;
        }
        auto ret = napiController->controller_->RegisterCallback(napiController->callback_);
        if (ret != AVSESSION_SUCCESS) {
            SLOGE("controller RegisterCallback failed:%{public}d", ret);
            if (ret == ERR_CONTROLLER_NOT_EXIST) {
                NapiUtils::ThrowError(env, "OnEvent failed : native controller not exist",
                    NapiAVSessionManager::errcode_[ERR_CONTROLLER_NOT_EXIST]);
                return napi_generic_failure;
            } else if (ret == ERR_NO_MEMORY) {
                NapiUtils::ThrowError(env, "OnEvent failed : native no memory",
                    NapiAVSessionManager::errcode_[ERR_NO_MEMORY]);
                return napi_generic_failure;
            } else {
                NapiUtils::ThrowError(env, "OnEvent failed : native server exception",
                    NapiAVSessionManager::errcode_[ret]);
                return napi_generic_failure;
            }
        }
    }
    if (it->second.first(env, napiController, filter, callback) != napi_ok) {
        SLOGE("add event callback failed");
        NapiUtils::ThrowError(env, "add event callback failed", NapiAVSessionManager::errcode_[AVSESSION_ERROR]);
        return napi_generic_failure;
    }
    return napi_ok;
}

static bool IsThreeParamForOnEvent(const std::string& event)
{
    return event == "metadataChange" || event == "playbackStateChange";
}

napi_value NapiAVSessionController::OnEvent(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<ContextBase>();
    if (context == nullptr) {
        SLOGE("OnEvent failed : no memory");
        NapiUtils::ThrowError(env, "OnEvent failed : no memory", NapiAVSessionManager::errcode_[ERR_NO_MEMORY]);
        return NapiUtils::GetUndefinedValue(env);
    }

    std::string eventName;
    napi_value filter {};
    napi_value callback {};
    auto input = [&eventName, &callback, &filter, env, &context](size_t argc, napi_value* argv) {
        CHECK_ARGS_RETURN_VOID(context, argc >= ARGC_ONE, "invalid argument number",
            NapiAVSessionManager::errcode_[ERR_INVALID_PARAM]);
        context->status = NapiUtils::GetValue(env, argv[ARGV_FIRST], eventName);
        CHECK_STATUS_RETURN_VOID(context, "get event name failed",
            NapiAVSessionManager::errcode_[ERR_INVALID_PARAM]);
        napi_valuetype type = napi_undefined;
        if (!IsThreeParamForOnEvent(eventName)) {
            CHECK_ARGS_RETURN_VOID(context, argc == ARGC_TWO, "invalid argument number",
                NapiAVSessionManager::errcode_[ERR_INVALID_PARAM]);
            context->status = napi_typeof(env, argv[ARGV_SECOND], &type);
            CHECK_ARGS_RETURN_VOID(context, (context->status == napi_ok) && (type == napi_function),
                                   "callback type invalid", NapiAVSessionManager::errcode_[ERR_INVALID_PARAM]);
            callback = argv[ARGV_SECOND];
        } else {
            CHECK_ARGS_RETURN_VOID(context, argc == ARGC_THERE, "invalid argument number",
                NapiAVSessionManager::errcode_[ERR_INVALID_PARAM]);
            context->status = napi_typeof(env, argv[ARGV_SECOND], &type);
            CHECK_ARGS_RETURN_VOID(
                context, (context->status == napi_ok) && (type == napi_object || type == napi_string),
                "filter type invalid", NapiAVSessionManager::errcode_[ERR_INVALID_PARAM]);
            filter = argv[ARGV_SECOND];
            context->status = napi_typeof(env, argv[ARGV_THIRD], &type);
            CHECK_ARGS_RETURN_VOID(context, (context->status == napi_ok) && (type == napi_function),
                                   "callback type invalid", NapiAVSessionManager::errcode_[ERR_INVALID_PARAM]);
            callback = argv[ARGV_THIRD];
        }
    };
    context->GetCbInfo(env, info, input, true);
    if (context->status != napi_ok) {
        NapiUtils::ThrowError(env, context->errMessage.c_str(), context->errCode);
        return NapiUtils::GetUndefinedValue(env);
    }

    RegisterCallback(env, context, eventName, filter, callback);

    return NapiUtils::GetUndefinedValue(env);
}

napi_value NapiAVSessionController::OffEvent(napi_env env, napi_callback_info info)
{
    auto context = std::make_shared<ContextBase>();
    if (context == nullptr) {
        SLOGE("OnEvent failed : no memory");
        NapiUtils::ThrowError(env, "OnEvent failed : no memory", NapiAVSessionManager::errcode_[ERR_NO_MEMORY]);
        return NapiUtils::GetUndefinedValue(env);
    }
    
    std::string eventName;
    napi_value callback = nullptr;
    auto input = [&eventName, env, &context, &callback](size_t argc, napi_value* argv) {
        CHECK_ARGS_RETURN_VOID(context, argc == ARGC_ONE || argc == ARGC_TWO, "invalid argument number",
            NapiAVSessionManager::errcode_[ERR_INVALID_PARAM]);
        context->status = NapiUtils::GetValue(env, argv[ARGV_FIRST], eventName);
        CHECK_STATUS_RETURN_VOID(context, "get event name failed",
            NapiAVSessionManager::errcode_[ERR_INVALID_PARAM]);
        if (argc == ARGC_TWO) {
            callback = argv[ARGV_SECOND];
        }
    };

    context->GetCbInfo(env, info, input, true);
    if (context->status != napi_ok) {
        NapiUtils::ThrowError(env, context->errMessage.c_str(), context->errCode);
        return NapiUtils::GetUndefinedValue(env);
    }

    auto it = EventHandlers_.find(eventName);
    if (it == EventHandlers_.end()) {
        SLOGE("event name invalid:%{public}s", eventName.c_str());
        NapiUtils::ThrowError(env, "event name invalid", NapiAVSessionManager::errcode_[ERR_INVALID_PARAM]);
        return NapiUtils::GetUndefinedValue(env);
    }

    auto* napiController = reinterpret_cast<NapiAVSessionController*>(context->native);
    if (napiController->callback_ == nullptr) {
        SLOGI("function %{public}s not register yet", eventName.c_str());
        return NapiUtils::GetUndefinedValue(env);
    }

    if (it->second.second(env, napiController, callback) != napi_ok) {
        NapiUtils::ThrowError(env, "remove event callback failed", NapiAVSessionManager::errcode_[AVSESSION_ERROR]);
    }
    return NapiUtils::GetUndefinedValue(env);
}

napi_status NapiAVSessionController::OnSessionDestroy(napi_env env, NapiAVSessionController* napiController,
                                                      napi_value param, napi_value callback)
{
    return napiController->callback_->AddCallback(env, NapiAVControllerCallback::EVENT_SESSION_DESTROY, callback);
}

napi_status NapiAVSessionController::OnPlaybackStateChange(napi_env env, NapiAVSessionController* napiController,
                                                           napi_value param, napi_value callback)
{
    if (SetPlaybackStateFilter(env, napiController, param) != napi_ok) {
        return napi_generic_failure;
    }
    return napiController->callback_->AddCallback(env, NapiAVControllerCallback::EVENT_PLAYBACK_STATE_CHANGE,
                                                  callback);
}

napi_status NapiAVSessionController::OnMetaDataChange(napi_env env, NapiAVSessionController* napiController,
                                                      napi_value param, napi_value callback)
{
    if (SetMetaFilter(env, napiController, param) != napi_ok) {
        return napi_generic_failure;
    }
    return napiController->callback_->AddCallback(env, NapiAVControllerCallback::EVENT_META_DATA_CHANGE, callback);
}

napi_status NapiAVSessionController::OnActiveStateChange(napi_env env, NapiAVSessionController* napiController,
                                                         napi_value param, napi_value callback)
{
    return napiController->callback_->AddCallback(env, NapiAVControllerCallback::EVENT_ACTIVE_STATE_CHANGE, callback);
}

napi_status NapiAVSessionController::OnValidCommandChange(napi_env env, NapiAVSessionController* napiController,
                                                          napi_value param, napi_value callback)
{
    return napiController->callback_->AddCallback(env, NapiAVControllerCallback::EVENT_VALID_COMMAND_CHANGE,
                                                  callback);
}

napi_status NapiAVSessionController::OnOutputDeviceChange(napi_env env, NapiAVSessionController* napiController,
                                                          napi_value param, napi_value callback)
{
    return napiController->callback_->AddCallback(env, NapiAVControllerCallback::EVENT_OUTPUT_DEVICE_CHANGE,
                                                  callback);
}

napi_status NapiAVSessionController::OffSessionDestroy(napi_env env, NapiAVSessionController* napiController,
                                                       napi_value callback)
{
    return napiController->callback_->RemoveCallback(env, NapiAVControllerCallback::EVENT_SESSION_DESTROY, callback);
}

napi_status NapiAVSessionController::OffPlaybackStateChange(napi_env env, NapiAVSessionController* napiController,
                                                            napi_value callback)
{
    return napiController->callback_->RemoveCallback(env, NapiAVControllerCallback::EVENT_PLAYBACK_STATE_CHANGE,
                                                     callback);
}

napi_status NapiAVSessionController::OffMetaDataChange(napi_env env, NapiAVSessionController* napiController,
                                                       napi_value callback)
{
    return napiController->callback_->RemoveCallback(env, NapiAVControllerCallback::EVENT_META_DATA_CHANGE, callback);
}

napi_status NapiAVSessionController::OffActiveStateChange(napi_env env, NapiAVSessionController* napiController,
                                                          napi_value callback)
{
    return napiController->callback_->RemoveCallback(env, NapiAVControllerCallback::EVENT_ACTIVE_STATE_CHANGE,
                                                     callback);
}

napi_status NapiAVSessionController::OffValidCommandChange(napi_env env, NapiAVSessionController* napiController,
                                                           napi_value callback)
{
    return napiController->callback_->RemoveCallback(env, NapiAVControllerCallback::EVENT_VALID_COMMAND_CHANGE,
                                                     callback);
}

napi_status NapiAVSessionController::OffOutputDeviceChange(napi_env env, NapiAVSessionController* napiController,
                                                           napi_value callback)
{
    return napiController->callback_->RemoveCallback(env, NapiAVControllerCallback::EVENT_OUTPUT_DEVICE_CHANGE,
                                                     callback);
}
}