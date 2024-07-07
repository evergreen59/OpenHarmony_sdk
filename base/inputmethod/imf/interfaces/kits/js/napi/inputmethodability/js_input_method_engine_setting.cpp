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

#include "js_input_method_engine_setting.h"

#include <thread>

#include "input_method_ability.h"
#include "input_method_property.h"
#include "input_method_utils.h"
#include "js_keyboard_controller_engine.h"
#include "js_text_input_client_engine.h"
#include "js_utils.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace MiscServices {
constexpr size_t ARGC_ZERO = 0;
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
constexpr size_t ARGC_MAX = 6;
const std::string JsInputMethodEngineSetting::IMES_CLASS_NAME = "InputMethodEngine";
thread_local napi_ref JsInputMethodEngineSetting::IMESRef_ = nullptr;

std::mutex JsInputMethodEngineSetting::engineMutex_;
std::shared_ptr<JsInputMethodEngineSetting> JsInputMethodEngineSetting::inputMethodEngine_ { nullptr };

napi_value JsInputMethodEngineSetting::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor descriptor[] = {
        DECLARE_NAPI_PROPERTY("ENTER_KEY_TYPE_UNSPECIFIED",
            GetJsConstProperty(env, static_cast<uint32_t>(EnterKeyType::UNSPECIFIED))),
        DECLARE_NAPI_PROPERTY("ENTER_KEY_TYPE_GO",
            GetJsConstProperty(env, static_cast<uint32_t>(EnterKeyType::GO))),
        DECLARE_NAPI_PROPERTY("ENTER_KEY_TYPE_SEARCH",
            GetJsConstProperty(env, static_cast<uint32_t>(EnterKeyType::SEARCH))),
        DECLARE_NAPI_PROPERTY("ENTER_KEY_TYPE_SEND",
            GetJsConstProperty(env, static_cast<uint32_t>(EnterKeyType::SEND))),
        DECLARE_NAPI_PROPERTY("ENTER_KEY_TYPE_NEXT",
            GetJsConstProperty(env, static_cast<uint32_t>(EnterKeyType::NEXT))),
        DECLARE_NAPI_PROPERTY("ENTER_KEY_TYPE_DONE",
            GetJsConstProperty(env, static_cast<uint32_t>(EnterKeyType::DONE))),
        DECLARE_NAPI_PROPERTY("ENTER_KEY_TYPE_PREVIOUS",
            GetJsConstProperty(env, static_cast<uint32_t>(EnterKeyType::PREVIOUS))),

        DECLARE_NAPI_PROPERTY("PATTERN_NULL",
            GetIntJsConstProperty(env, static_cast<int32_t>(TextInputType::NONE))),
        DECLARE_NAPI_PROPERTY("PATTERN_TEXT",
            GetJsConstProperty(env, static_cast<uint32_t>(TextInputType::TEXT))),
        DECLARE_NAPI_PROPERTY("PATTERN_NUMBER",
            GetJsConstProperty(env, static_cast<uint32_t>(TextInputType::NUMBER))),
        DECLARE_NAPI_PROPERTY("PATTERN_PHONE",
            GetJsConstProperty(env, static_cast<uint32_t>(TextInputType::PHONE))),
        DECLARE_NAPI_PROPERTY("PATTERN_DATETIME",
            GetJsConstProperty(env, static_cast<uint32_t>(TextInputType::DATETIME))),
        DECLARE_NAPI_PROPERTY("PATTERN_EMAIL",
            GetJsConstProperty(env, static_cast<uint32_t>(TextInputType::EMAIL_ADDRESS))),
        DECLARE_NAPI_PROPERTY("PATTERN_URI",
            GetJsConstProperty(env, static_cast<uint32_t>(TextInputType::URL))),
        DECLARE_NAPI_PROPERTY("PATTERN_PASSWORD",
            GetJsConstProperty(env, static_cast<uint32_t>(TextInputType::VISIBLE_PASSWORD))),

        DECLARE_NAPI_FUNCTION("MoveCursor", MoveCursor),
        DECLARE_NAPI_FUNCTION("getInputMethodEngine", GetInputMethodEngine),
        DECLARE_NAPI_FUNCTION("getInputMethodAbility", GetInputMethodAbility),
    };
    NAPI_CALL(
        env, napi_define_properties(env, exports, sizeof(descriptor) / sizeof(napi_property_descriptor), descriptor));

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("on", Subscribe),
        DECLARE_NAPI_FUNCTION("off", UnSubscribe),
    };
    napi_value cons = nullptr;
    NAPI_CALL(env, napi_define_class(env, IMES_CLASS_NAME.c_str(), IMES_CLASS_NAME.size(),
        JsConstructor, nullptr, sizeof(properties) / sizeof(napi_property_descriptor), properties, &cons));
    NAPI_CALL(env, napi_create_reference(env, cons, 1, &IMESRef_));
    NAPI_CALL(env, napi_set_named_property(env, exports, IMES_CLASS_NAME.c_str(), cons));
    return exports;
};

napi_value JsInputMethodEngineSetting::GetJsConstProperty(napi_env env, uint32_t num)
{
    napi_value jsNumber = nullptr;
    napi_create_uint32(env, num, &jsNumber);
    return jsNumber;
}

napi_value JsInputMethodEngineSetting::GetIntJsConstProperty(napi_env env, int32_t num)
{
    napi_value jsNumber = nullptr;
    napi_create_int32(env, num, &jsNumber);
    return jsNumber;
}

std::shared_ptr<JsInputMethodEngineSetting> JsInputMethodEngineSetting::GetInputMethodEngineSetting()
{
    if (inputMethodEngine_ == nullptr) {
        std::lock_guard<std::mutex> lock(engineMutex_);
        if (inputMethodEngine_ == nullptr) {
            auto engine = std::make_shared<JsInputMethodEngineSetting>();
            if (engine == nullptr) {
                IMSA_HILOGE("input method engine nullptr");
                return nullptr;
            }
            inputMethodEngine_ = engine;
        }
    }
    return inputMethodEngine_;
}

bool JsInputMethodEngineSetting::InitInputMethodSetting()
{
    if (InputMethodAbility::GetInstance()->SetCoreAndAgent() != ErrorCode::NO_ERROR) {
        return false;
    }
    auto engine = GetInputMethodEngineSetting();
    if (engine == nullptr) {
        return false;
    }
    InputMethodAbility::GetInstance()->setImeListener(engine);
    return true;
}

napi_value JsInputMethodEngineSetting::JsConstructor(napi_env env, napi_callback_info info)
{
    IMSA_HILOGI("run in JsConstructor");
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
    auto setting = GetInputMethodEngineSetting();
    if (setting == nullptr || !InitInputMethodSetting()) {
        IMSA_HILOGE("get setting nullptr");
        napi_value result = nullptr;
        napi_get_null(env, &result);
        return result;
    }
    napi_status status = napi_wrap(
        env, thisVar, setting.get(), [](napi_env env, void *nativeObject, void *hint) {}, nullptr, nullptr);
    if (status != napi_ok) {
        IMSA_HILOGE("JsInputMethodEngineSetting napi_wrap failed: %{public}d", status);
        return nullptr;
    }
    if (setting->loop_ == nullptr) {
        napi_get_uv_event_loop(env, &setting->loop_);
    }
    return thisVar;
};

napi_value JsInputMethodEngineSetting::GetInputMethodAbility(napi_env env, napi_callback_info info)
{
    return GetIMEInstance(env, info);
}

napi_value JsInputMethodEngineSetting::GetInputMethodEngine(napi_env env, napi_callback_info info)
{
    return GetIMEInstance(env, info);
}

napi_value JsInputMethodEngineSetting::GetIMEInstance(napi_env env, napi_callback_info info)
{
    napi_value instance = nullptr;
    napi_value cons = nullptr;
    if (napi_get_reference_value(env, IMESRef_, &cons) != napi_ok) {
        IMSA_HILOGE("failed to get reference value");
        return nullptr;
    }
    if (napi_new_instance(env, cons, 0, nullptr, &instance) != napi_ok) {
        IMSA_HILOGE("failed to new instance");
        return nullptr;
    }
    return instance;
}

napi_value JsInputMethodEngineSetting::MoveCursor(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_MAX;
    napi_value argv[ARGC_MAX] = { nullptr };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    NAPI_ASSERT(env, argc == 1, "Wrong number of arguments, requires 1");

    napi_valuetype valuetype;
    NAPI_CALL(env, napi_typeof(env, argv[ARGC_ZERO], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_number, "type is not a number");

    int32_t number;
    if (napi_get_value_int32(env, argv[ARGC_ZERO], &number) != napi_ok) {
        IMSA_HILOGE("GetNumberProperty error");
    }

    InputMethodAbility::GetInstance()->MoveCursor(number);

    napi_value result = nullptr;
    napi_get_null(env, &result);
    return result;
}

std::string JsInputMethodEngineSetting::GetStringProperty(napi_env env, napi_value jsString)
{
    char propValue[MAX_VALUE_LEN] = {0};
    size_t propLen;
    if (napi_get_value_string_utf8(env, jsString, propValue, MAX_VALUE_LEN, &propLen) != napi_ok) {
        IMSA_HILOGE("GetStringProperty error");
        return "";
    }
    return std::string(propValue);
}

void JsInputMethodEngineSetting::RegisterListener(
    napi_value callback, std::string type, std::shared_ptr<JSCallbackObject> callbackObj)
{
    IMSA_HILOGI("RegisterListener %{public}s", type.c_str());
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        IMSA_HILOGE("methodName: %{public}s not registertd!", type.c_str());
    }
    auto callbacks = jsCbMap_[type];
    bool ret = std::any_of(callbacks.begin(), callbacks.end(), [&callback](std::shared_ptr<JSCallbackObject> cb) {
        return Equals(cb->env_, callback, cb->callback_, cb->threadId_);
    });
    if (ret) {
        IMSA_HILOGE("JsInputMethodEngineListener::RegisterListener callback already registered!");
        return;
    }

    IMSA_HILOGI("Add %{public}s callbackObj into jsCbMap_", type.c_str());
    jsCbMap_[type].push_back(std::move(callbackObj));
}

void JsInputMethodEngineSetting::UnRegisterListener(napi_value callback, std::string type)
{
    IMSA_HILOGI("UnRegisterListener %{public}s", type.c_str());
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        IMSA_HILOGE("methodName: %{public}s already unRegisterted!", type.c_str());
        return;
    }

    if (callback == nullptr) {
        jsCbMap_.erase(type);
        IMSA_HILOGE("callback is nullptr");
        return;
    }

    for (auto item = jsCbMap_[type].begin(); item != jsCbMap_[type].end(); item++) {
        if (Equals((*item)->env_, callback, (*item)->callback_, (*item)->threadId_)) {
            jsCbMap_[type].erase(item);
            break;
        }
    }

    if (jsCbMap_[type].empty()) {
        jsCbMap_.erase(type);
    }
}

JsInputMethodEngineSetting *JsInputMethodEngineSetting::GetNative(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_MAX;
    void *native = nullptr;
    napi_value self = nullptr;
    napi_value argv[ARGC_MAX] = { nullptr };
    napi_status status = napi_invalid_arg;
    status = napi_get_cb_info(env, info, &argc, argv, &self, nullptr);
    if (self == nullptr && argc >= ARGC_MAX) {
        IMSA_HILOGE("napi_get_cb_info failed");
        return nullptr;
    }

    status = napi_unwrap(env, self, &native);
    NAPI_ASSERT(env, (status == napi_ok && native != nullptr), "napi_unwrap failed!");
    return reinterpret_cast<JsInputMethodEngineSetting*>(native);
}

napi_value JsInputMethodEngineSetting::Subscribe(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = { nullptr };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    if (argc != ARGC_TWO) {
        JsUtils::ThrowException(
            env, IMFErrorCode::EXCEPTION_PARAMCHECK, "Wrong number of arguments, requires 2", TypeCode::TYPE_NONE);
        return nullptr;
    }

    napi_valuetype valuetype;
    NAPI_CALL(env, napi_typeof(env, argv[ARGC_ZERO], &valuetype));
    if (valuetype != napi_string) {
        JsUtils::ThrowException(env, IMFErrorCode::EXCEPTION_PARAMCHECK, "'type'", TypeCode::TYPE_STRING);
        return nullptr;
    }
    std::string type = GetStringProperty(env, argv[ARGC_ZERO]);
    IMSA_HILOGE("event type is: %{public}s", type.c_str());

    valuetype = napi_undefined;
    napi_typeof(env, argv[ARGC_ONE], &valuetype);
    if (valuetype != napi_function) {
        JsUtils::ThrowException(env, IMFErrorCode::EXCEPTION_PARAMCHECK, "'callback'", TypeCode::TYPE_FUNCTION);
        return nullptr;
    }

    auto engine = GetNative(env, info);
    if (engine == nullptr) {
        return nullptr;
    }
    std::shared_ptr<JSCallbackObject> callback =
        std::make_shared<JSCallbackObject>(env, argv[ARGC_ONE], std::this_thread::get_id());
    engine->RegisterListener(argv[ARGC_ONE], type, callback);

    napi_value result = nullptr;
    napi_get_null(env, &result);
    return result;
}

napi_value JsInputMethodEngineSetting::UnSubscribe(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    if (argc != ARGC_ONE && argc != ARGC_TWO) {
        JsUtils::ThrowException(env, IMFErrorCode::EXCEPTION_PARAMCHECK, "Wrong number of arguments, requires 1 or 2",
            TypeCode::TYPE_NONE);
        return nullptr;
    }

    napi_valuetype valuetype;
    NAPI_CALL(env, napi_typeof(env, argv[ARGC_ZERO], &valuetype));
    if (valuetype != napi_string) {
        JsUtils::ThrowException(env, IMFErrorCode::EXCEPTION_PARAMCHECK, "'type'", TypeCode::TYPE_STRING);
        return nullptr;
    }
    std::string type = GetStringProperty(env, argv[ARGC_ZERO]);
    IMSA_HILOGE("event type is: %{public}s", type.c_str());

    auto engine = GetNative(env, info);
    if (engine == nullptr) {
        return nullptr;
    }

    if (argc == ARGC_TWO) {
        valuetype = napi_undefined;
        napi_typeof(env, argv[ARGC_ONE], &valuetype);
        if (valuetype != napi_function) {
            JsUtils::ThrowException(env, IMFErrorCode::EXCEPTION_PARAMCHECK, "'callback'", TypeCode::TYPE_FUNCTION);
            return nullptr;
        }
    }
    engine->UnRegisterListener(argv[ARGC_ONE], type);
    napi_value result = nullptr;
    napi_get_null(env, &result);
    return result;
}

bool JsInputMethodEngineSetting::Equals(napi_env env, napi_value value, napi_ref copy, std::thread::id threadId)
{
    if (copy == nullptr) {
        return (value == nullptr);
    }

    if (threadId != std::this_thread::get_id()) {
        IMSA_HILOGD("napi_value can not be compared");
        return false;
    }

    napi_value copyValue = nullptr;
    napi_get_reference_value(env, copy, &copyValue);

    bool isEquals = false;
    napi_strict_equals(env, value, copyValue, &isEquals);
    IMSA_HILOGD("value compare result: %{public}d", isEquals);
    return isEquals;
}

napi_value JsInputMethodEngineSetting::GetResultOnSetSubtype(napi_env env, const SubProperty &property)
{
    napi_value subType = nullptr;
    napi_create_object(env, &subType);

    napi_value label = nullptr;
    napi_create_string_utf8(env, property.label.c_str(), property.name.size(), &label);
    napi_set_named_property(env, subType, "label", label);

    napi_value name = nullptr;
    napi_create_string_utf8(env, property.name.c_str(), property.name.size(), &name);
    napi_set_named_property(env, subType, "name", name);

    napi_value id = nullptr;
    napi_create_string_utf8(env, property.id.c_str(), property.id.size(), &id);
    napi_set_named_property(env, subType, "id", id);

    napi_value mode = nullptr;
    napi_create_string_utf8(env, property.mode.c_str(), property.mode.size(), &mode);
    napi_set_named_property(env, subType, "mode", mode);

    napi_value locale = nullptr;
    napi_create_string_utf8(env, property.locale.c_str(), property.locale.size(), &locale);
    napi_set_named_property(env, subType, "locale", locale);

    napi_value language = nullptr;
    napi_create_string_utf8(env, property.language.c_str(), property.language.size(), &language);
    napi_set_named_property(env, subType, "language", language);

    napi_value icon = nullptr;
    napi_create_string_utf8(env, property.icon.c_str(), property.icon.size(), &icon);
    napi_set_named_property(env, subType, "icon", icon);

    napi_value iconId = nullptr;
    napi_create_int32(env, property.iconId, &iconId);
    napi_set_named_property(env, subType, "iconId", iconId);

    napi_value extra = nullptr;
    napi_create_object(env, &extra);
    napi_set_named_property(env, subType, "extra", extra);

    return subType;
}

void JsInputMethodEngineSetting::OnInputStart()
{
    IMSA_HILOGD("run in");
    std::string type = "inputStart";
    uv_work_t *work = GetUVwork(type);
    if (work == nullptr) {
        IMSA_HILOGD("failed to get uv entry");
        return;
    }
    uv_queue_work(
        loop_, work, [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            std::shared_ptr<UvEntry> entry(static_cast<UvEntry *>(work->data), [work](UvEntry *data) {
                delete data;
                delete work;
            });
            if (entry == nullptr) {
                IMSA_HILOGE("OnInputStart:: entryptr is null");
                return;
            }

            auto getInputStartProperty = [](napi_value *args, uint8_t argc,
                                             std::shared_ptr<JSCallbackObject> item) -> bool {
                if (argc < 2) {
                    return false;
                }
                napi_value textInput = JsTextInputClientEngine::GetTextInputClientInstance(item->env_);
                napi_value keyBoardController = JsKeyboardControllerEngine::GetKeyboardControllerInstance(item->env_);
                if (keyBoardController == nullptr || textInput == nullptr) {
                    IMSA_HILOGE("get KBCins or TICins failed:");
                    return false;
                }
                args[ARGC_ZERO] = keyBoardController;
                args[ARGC_ONE] = textInput;
                return true;
            };
            JsUtils::TraverseCallback(entry->vecCopy, ARGC_TWO, getInputStartProperty);
        });
}

void JsInputMethodEngineSetting::OnKeyboardStatus(bool isShow)
{
    std::string type = isShow ? "keyboardShow" : "keyboardHide";
    IMSA_HILOGD("run in, %{public}s", type.c_str());
    uv_work_t *work = GetUVwork(type);
    if (work == nullptr) {
        IMSA_HILOGD("failed to get uv entry");
        return;
    }
    uv_queue_work(
        loop_, work, [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            std::shared_ptr<UvEntry> entry(static_cast<UvEntry *>(work->data), [work](UvEntry *data) {
                delete data;
                delete work;
            });

            auto getKeyboardStatusProperty = [](napi_value *args, uint8_t argc,
                                                 std::shared_ptr<JSCallbackObject> item) -> bool {
                if (argc == 0) {
                    return false;
                }
                args[ARGC_ZERO] = nullptr;
                return true;
            };
            JsUtils::TraverseCallback(entry->vecCopy, ARGC_ZERO, getKeyboardStatusProperty);
        });
}

void JsInputMethodEngineSetting::OnInputStop(const std::string &imeId)
{
    IMSA_HILOGD("run in");
    std::string type = "inputStop";
    uv_work_t *work = GetUVwork(type, [&imeId](UvEntry &entry) { entry.imeid = imeId; });
    if (work == nullptr) {
        IMSA_HILOGD("failed to get uv entry");
        return;
    }
    uv_queue_work(
        loop_, work, [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            std::shared_ptr<UvEntry> entry(static_cast<UvEntry *>(work->data), [work](UvEntry *data) {
                delete data;
                delete work;
            });
            if (entry == nullptr) {
                IMSA_HILOGE("OnInputStop:: entryptr is null");
                return;
            }

            auto getInputStopProperty = [entry](napi_value *args, uint8_t argc,
                                            std::shared_ptr<JSCallbackObject> item) -> bool {
                if (argc == 0) {
                    return false;
                }
                napi_create_string_utf8(item->env_, entry->imeid.c_str(), NAPI_AUTO_LENGTH, &args[ARGC_ZERO]);
                return true;
            };
            JsUtils::TraverseCallback(entry->vecCopy, ARGC_ONE, getInputStopProperty);
        });
}

void JsInputMethodEngineSetting::OnSetCallingWindow(uint32_t windowId)
{
    IMSA_HILOGD("run in");
    std::string type = "setCallingWindow";
    uv_work_t *work = GetUVwork(type, [windowId](UvEntry &entry) { entry.windowid = windowId; });
    if (work == nullptr) {
        IMSA_HILOGD("failed to get uv entry");
        return;
    }
    uv_queue_work(
        loop_, work, [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            std::shared_ptr<UvEntry> entry(static_cast<UvEntry *>(work->data), [work](UvEntry *data) {
                delete data;
                delete work;
            });
            if (entry == nullptr) {
                IMSA_HILOGE("setCallingWindow:: entryptr is null");
                return;
            }

            auto getCallingWindowProperty = [entry](napi_value *args, uint8_t argc,
                                                std::shared_ptr<JSCallbackObject> item) -> bool {
                if (argc == 0) {
                    return false;
                }
                napi_create_int32(item->env_, entry->windowid, &args[ARGC_ZERO]);
                return true;
            };
            JsUtils::TraverseCallback(entry->vecCopy, ARGC_ONE, getCallingWindowProperty);
        });
}

void JsInputMethodEngineSetting::OnSetSubtype(const SubProperty &property)
{
    IMSA_HILOGD("run in");
    std::string type = "setSubtype";
    uv_work_t *work = GetUVwork(type, [&property](UvEntry &entry) { entry.subProperty = property; });
    if (work == nullptr) {
        IMSA_HILOGD("failed to get uv entry");
        return;
    }
    uv_queue_work(
        loop_, work, [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            std::shared_ptr<UvEntry> entry(static_cast<UvEntry *>(work->data), [work](UvEntry *data) {
                delete data;
                delete work;
            });
            if (entry == nullptr) {
                IMSA_HILOGE("OnSetSubtype:: entryptr is null");
                return;
            }

            auto getSubtypeProperty = [entry](napi_value *args, uint8_t argc,
                                          std::shared_ptr<JSCallbackObject> item) -> bool {
                if (argc == 0) {
                    return false;
                }
                napi_value jsObject = GetResultOnSetSubtype(item->env_, entry->subProperty);
                if (jsObject == nullptr) {
                    IMSA_HILOGE("get GetResultOnSetSubtype failed: jsObject is nullptr");
                    return false;
                }
                args[ARGC_ZERO] = { jsObject };
                return true;
            };
            JsUtils::TraverseCallback(entry->vecCopy, ARGC_ONE, getSubtypeProperty);
        });
}

uv_work_t *JsInputMethodEngineSetting::GetUVwork(const std::string &type, EntrySetter entrySetter)
{
    IMSA_HILOGD("run in, type: %{public}s", type.c_str());
    UvEntry *entry = nullptr;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);

        if (jsCbMap_[type].empty()) {
            IMSA_HILOGE("%{public}s cb-vector is empty", type.c_str());
            return nullptr;
        }
        entry = new (std::nothrow) UvEntry(jsCbMap_[type], type);
        if (entry == nullptr) {
            IMSA_HILOGE("entry ptr is nullptr!");
            return nullptr;
        }
        if (entrySetter != nullptr) {
            entrySetter(*entry);
        }
    }
    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        IMSA_HILOGE("entry ptr is nullptr!");
        return nullptr;
    }
    work->data = entry;
    return work;
}
} // namespace MiscServices
} // namespace OHOS
