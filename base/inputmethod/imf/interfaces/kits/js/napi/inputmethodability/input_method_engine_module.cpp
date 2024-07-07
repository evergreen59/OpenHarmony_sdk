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

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "js_input_method_engine_setting.h"
#include "js_keyboard_delegate_setting.h"
#include "js_keyboard_controller_engine.h"
#include "js_text_input_client_engine.h"

EXTERN_C_START
/*
 * function for module exports
 */
static napi_value Init(napi_env env, napi_value exports)
{
    OHOS::MiscServices::JsInputMethodEngineSetting::Init(env, exports);
    OHOS::MiscServices::JsKeyboardControllerEngine::Init(env, exports);
    OHOS::MiscServices::JsTextInputClientEngine::Init(env, exports);
    OHOS::MiscServices::JsKeyboardDelegateSetting::Init(env, exports);
    return exports;
}
EXTERN_C_END
/*
 * module define
 */
static napi_module _module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "inputMethodEngine",
    .nm_priv = ((void *)0),
    .reserved = {0}
};
/*
 * module register
 */
extern "C" __attribute__((constructor)) void Register()
{
    napi_module_register(&_module);
}