/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#include "js_text_input_client_engine.h"

#include "input_method_ability.h"
#include "js_utils.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "string_ex.h"

namespace OHOS {
namespace MiscServices {
thread_local napi_ref JsTextInputClientEngine::TICRef_ = nullptr;
const std::string JsTextInputClientEngine::TIC_CLASS_NAME = "TextInputClient";

napi_value JsTextInputClientEngine::Init(napi_env env, napi_value info)
{
    IMSA_HILOGI("JsTextInputClientEngine init");
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("sendKeyFunction", SendKeyFunction),
        DECLARE_NAPI_FUNCTION("deleteForward", DeleteForward),
        DECLARE_NAPI_FUNCTION("deleteBackward", DeleteBackward),
        DECLARE_NAPI_FUNCTION("insertText", InsertText),
        DECLARE_NAPI_FUNCTION("getForward", GetForward),
        DECLARE_NAPI_FUNCTION("getBackward", GetBackward),
        DECLARE_NAPI_FUNCTION("getEditorAttribute", GetEditorAttribute),
        DECLARE_NAPI_FUNCTION("moveCursor", MoveCursor),
    };
    napi_value cons = nullptr;
    NAPI_CALL(env, napi_define_class(env, TIC_CLASS_NAME.c_str(), TIC_CLASS_NAME.size(),
        JsConstructor, nullptr, sizeof(properties) / sizeof(napi_property_descriptor), properties, &cons));
    NAPI_CALL(env, napi_create_reference(env, cons, 1, &TICRef_));
    NAPI_CALL(env, napi_set_named_property(env, info, TIC_CLASS_NAME.c_str(), cons));

    return info;
}

napi_value JsTextInputClientEngine::MoveCursor(napi_env env, napi_callback_info info)
{
    auto ctxt = std::make_shared<MoveCursorContext>();
    auto input = [ctxt](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        napi_status status = napi_generic_failure;
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[0], &valueType);
        if (argc < 1) {
            JsUtils::ThrowException(
                env, IMFErrorCode::EXCEPTION_PARAMCHECK, " should 1 or 2 parameters!", TypeCode::TYPE_NONE);
            return status;
        }
        if (valueType != napi_number) {
            JsUtils::ThrowException(env, IMFErrorCode::EXCEPTION_PARAMCHECK, " direction", TypeCode::TYPE_NUMBER);
            return status;
        }
        status = GetMoveCursorParam(env, argv[0], ctxt);
        return status;
    };
    auto exec = [ctxt](AsyncCall::Context *ctx) {
        int32_t code = InputMethodAbility::GetInstance()->MoveCursor(ctxt->num);
        if (code == ErrorCode::NO_ERROR) {
            ctxt->status = napi_ok;
            ctxt->SetState(ctxt->status);
        } else {
            ctxt->SetErrorCode(code);
        }
    };
    ctxt->SetAction(std::move(input));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(ctxt));
    return asyncCall.Call(env, exec);
}

napi_value JsTextInputClientEngine::JsConstructor(napi_env env, napi_callback_info cbinfo)
{
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, cbinfo, nullptr, nullptr, &thisVar, nullptr));

    JsTextInputClientEngine *clientObject = new (std::nothrow) JsTextInputClientEngine();
    if (clientObject == nullptr) {
        IMSA_HILOGE("clientObject is nullptr");
        napi_value result = nullptr;
        napi_get_null(env, &result);
        return result;
    }
    auto finalize = [](napi_env env, void *data, void *hint) {
        IMSA_HILOGE("JsTextInputClientEngine finalize");
        auto *objInfo = reinterpret_cast<JsTextInputClientEngine *>(data);
        if (objInfo != nullptr) {
            delete objInfo;
        }
    };
    napi_status status = napi_wrap(env, thisVar, clientObject, finalize, nullptr, nullptr);
    if (status != napi_ok) {
        IMSA_HILOGE("JsTextInputClientEngine napi_wrap failed: %{public}d", status);
        delete clientObject;
        return nullptr;
    }
    return thisVar;
}

napi_value JsTextInputClientEngine::GetTextInputClientInstance(napi_env env)
{
    napi_value instance = nullptr;
    napi_value cons = nullptr;
    if (napi_get_reference_value(env, TICRef_, &cons) != napi_ok) {
        IMSA_HILOGE("JsTextInputClientEngine::napi_get_reference_value not ok");
        return nullptr;
    }
    if (napi_new_instance(env, cons, 0, nullptr, &instance) != napi_ok) {
        IMSA_HILOGE("JsTextInputClientEngine::napi_new_instance not ok");
        return nullptr;
    }
    return instance;
}

int32_t JsTextInputClientEngine::GetNumberProperty(napi_env env, napi_value jsNumber)
{
    int32_t number;
    if (napi_get_value_int32(env, jsNumber, &number) != napi_ok) {
        IMSA_HILOGE("GetNumberProperty error");
    }
    return number;
}

std::string JsTextInputClientEngine::GetStringProperty(napi_env env, napi_value jsString)
{
    char propValue[MAX_VALUE_LEN] = {0};
    size_t propLen;
    if (napi_get_value_string_utf8(env, jsString, propValue, MAX_VALUE_LEN, &propLen) != napi_ok) {
        IMSA_HILOGE("GetStringProperty error");
        return "";
    }
    return std::string(propValue);
}

napi_value JsTextInputClientEngine::GetResult(napi_env env, std::string &text)
{
    napi_value jsText = nullptr;
    napi_create_string_utf8(env, text.c_str(), NAPI_AUTO_LENGTH, &jsText);
    return jsText;
}

napi_value JsTextInputClientEngine::GetResultEditorAttribute(napi_env env,
    std::shared_ptr<GetEditorAttributeContext> getEditorAttribute)
{
    napi_value editorAttribute = nullptr;
    napi_create_object(env, &editorAttribute);

    napi_value jsValue = nullptr;
    napi_create_int32(env, getEditorAttribute->enterKeyType, &jsValue);
    napi_set_named_property(env, editorAttribute, "enterKeyType", jsValue);

    napi_value jsMethodId = nullptr;
    napi_create_int32(env, getEditorAttribute->inputPattern, &jsMethodId);
    napi_set_named_property(env, editorAttribute, "inputPattern", jsMethodId);
    
    return editorAttribute;
}

napi_status JsTextInputClientEngine::GetAction(napi_env env, napi_value argv,
    std::shared_ptr<SendKeyFunctionContext> ctxt)
{
    napi_valuetype valueType = napi_undefined;
    napi_status status = napi_generic_failure;
    status = napi_typeof(env, argv, &valueType);
    if (valueType == napi_number) {
        ctxt->action = GetNumberProperty(env, argv);
    }
    return status;
}

napi_status JsTextInputClientEngine::GetDeleteForwardLength(napi_env env, napi_value argv,
    std::shared_ptr<DeleteForwardContext> ctxt)
{
    napi_valuetype valueType = napi_undefined;
    napi_status status = napi_generic_failure;
    status = napi_typeof(env, argv, &valueType);
    if (valueType == napi_number) {
        ctxt->length = GetNumberProperty(env, argv);
    }
    return status;
}

napi_status JsTextInputClientEngine::GetMoveCursorParam(
    napi_env env, napi_value argv, std::shared_ptr<MoveCursorContext> ctxt)
{
    napi_valuetype valueType = napi_undefined;
    napi_status status = napi_generic_failure;
    status = napi_typeof(env, argv, &valueType);
    if (valueType == napi_number) {
        ctxt->num = GetNumberProperty(env, argv);
    }
    return status;
}

napi_status JsTextInputClientEngine::GetDeleteBackwardLength(napi_env env, napi_value argv,
    std::shared_ptr<DeleteBackwardContext> ctxt)
{
    napi_valuetype valueType = napi_undefined;
    napi_status status = napi_generic_failure;
    status = napi_typeof(env, argv, &valueType);
    if (valueType == napi_number) {
        ctxt->length = GetNumberProperty(env, argv);
    }
    return status;
}

napi_status JsTextInputClientEngine::GetInsertText(napi_env env, napi_value argv,
    std::shared_ptr<InsertTextContext> ctxt)
{
    napi_valuetype valueType = napi_undefined;
    napi_status status = napi_generic_failure;
    status = napi_typeof(env, argv, &valueType);
    if (valueType == napi_string) {
        ctxt->text = GetStringProperty(env, argv);
    }
    return status;
}

napi_status JsTextInputClientEngine::GetForwardLength(napi_env env, napi_value argv,
    std::shared_ptr<GetForwardContext> ctxt)
{
    napi_valuetype valueType = napi_undefined;
    napi_status status = napi_generic_failure;
    status = napi_typeof(env, argv, &valueType);
    if (valueType == napi_number) {
        ctxt->length = GetNumberProperty(env, argv);
    }
    return status;
}

napi_status JsTextInputClientEngine::GetBackwardLength(napi_env env, napi_value argv,
    std::shared_ptr<GetBackwardContext> ctxt)
{
    napi_valuetype valueType = napi_undefined;
    napi_status status = napi_generic_failure;
    status = napi_typeof(env, argv, &valueType);
    if (valueType == napi_number) {
        ctxt->length = GetNumberProperty(env, argv);
    }
    return status;
}

napi_value JsTextInputClientEngine::SendKeyFunction(napi_env env, napi_callback_info info)
{
    auto ctxt = std::make_shared<SendKeyFunctionContext>();
    auto input = [ctxt](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        napi_status status = napi_generic_failure;
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[0], &valueType);
        if (argc < 1) {
            JsUtils::ThrowException(
                env, IMFErrorCode::EXCEPTION_PARAMCHECK, " should 1 or 2 parameters!", TypeCode::TYPE_NONE);
            return status;
        }
        if (valueType != napi_number) {
            JsUtils::ThrowException(env, IMFErrorCode::EXCEPTION_PARAMCHECK, " 'action'", TypeCode::TYPE_NUMBER);
            return status;
        }
        status = GetAction(env, argv[0], ctxt);
        return status;
    };
    auto output = [ctxt](napi_env env, napi_value *result) -> napi_status {
        napi_status status = napi_get_boolean(env, ctxt->isSendKeyFunction, result);
        return status;
    };
    auto exec = [ctxt](AsyncCall::Context *ctx) {
        int32_t code = InputMethodAbility::GetInstance()->SendFunctionKey(ctxt->action);
        if (code == ErrorCode::NO_ERROR) {
            ctxt->status = napi_ok;
            ctxt->SetState(ctxt->status);
            ctxt->isSendKeyFunction = true;
        } else {
            ctxt->SetErrorCode(code);
        }
    };
    ctxt->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(ctxt), 1);
    return asyncCall.Call(env, exec);
}

napi_value JsTextInputClientEngine::DeleteForward(napi_env env, napi_callback_info info)
{
    auto ctxt = std::make_shared<DeleteForwardContext>();
    auto input = [ctxt](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        napi_status status = napi_generic_failure;
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[0], &valueType);
        if (argc < 1) {
            JsUtils::ThrowException(
                env, IMFErrorCode::EXCEPTION_PARAMCHECK, " should 1 or 2 parameters!", TypeCode::TYPE_NONE);
            return status;
        }
        if (valueType != napi_number) {
            JsUtils::ThrowException(env, IMFErrorCode::EXCEPTION_PARAMCHECK, " 'length'", TypeCode::TYPE_NUMBER);
            return status;
        }
        status = GetDeleteForwardLength(env, argv[0], ctxt);
        return status;
    };
    auto output = [ctxt](napi_env env, napi_value *result) -> napi_status {
        napi_status status = napi_get_boolean(env, ctxt->isDeleteForward, result);
        return status;
    };
    auto exec = [ctxt](AsyncCall::Context *ctx) {
        int32_t code = InputMethodAbility::GetInstance()->DeleteForward(ctxt->length);
        if (code == ErrorCode::NO_ERROR) {
            ctxt->status = napi_ok;
            ctxt->SetState(ctxt->status);
            ctxt->isDeleteForward = true;
        } else {
            ctxt->SetErrorCode(code);
        }
    };
    ctxt->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(ctxt), 1);
    return asyncCall.Call(env, exec);
}

napi_value JsTextInputClientEngine::DeleteBackward(napi_env env, napi_callback_info info)
{
    auto ctxt = std::make_shared<DeleteBackwardContext>();
    auto input = [ctxt](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        napi_status status = napi_generic_failure;
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[0], &valueType);
        if (argc < 1) {
            JsUtils::ThrowException(
                env, IMFErrorCode::EXCEPTION_PARAMCHECK, " should 1 or 2 parameters!", TypeCode::TYPE_NONE);
            return status;
        }
        if (valueType != napi_number) {
            JsUtils::ThrowException(env, IMFErrorCode::EXCEPTION_PARAMCHECK, " 'length", TypeCode::TYPE_NUMBER);
            return status;
        }
        status = GetDeleteBackwardLength(env, argv[0], ctxt);
        return status;
    };
    auto output = [ctxt](napi_env env, napi_value *result) -> napi_status {
        napi_status status = napi_get_boolean(env, ctxt->isDeleteBackward, result);
        return status;
    };
    auto exec = [ctxt](AsyncCall::Context *ctx) {
        int32_t code = InputMethodAbility::GetInstance()->DeleteBackward(ctxt->length);
        if (code == ErrorCode::NO_ERROR) {
            ctxt->status = napi_ok;
            ctxt->SetState(ctxt->status);
            ctxt->isDeleteBackward = true;
        } else {
            ctxt->SetErrorCode(code);
        }
    };
    ctxt->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(ctxt), 1);
    return asyncCall.Call(env, exec);
}

napi_value JsTextInputClientEngine::InsertText(napi_env env, napi_callback_info info)
{
    auto ctxt = std::make_shared<InsertTextContext>();
    auto input = [ctxt](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        napi_status status = napi_generic_failure;
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[0], &valueType);
        if (argc < 1) {
            JsUtils::ThrowException(
                env, IMFErrorCode::EXCEPTION_PARAMCHECK, " should 1 or 2 parameters!", TypeCode::TYPE_NONE);
            return status;
        }
        if (valueType != napi_string) {
            JsUtils::ThrowException(env, IMFErrorCode::EXCEPTION_PARAMCHECK, " 'text'", TypeCode::TYPE_STRING);
            return status;
        }
        status = GetInsertText(env, argv[0], ctxt);
        return status;
    };
    auto output = [ctxt](napi_env env, napi_value *result) -> napi_status {
        napi_status status = napi_get_boolean(env, ctxt->isInsertText, result);
        return status;
    };
    auto exec = [ctxt](AsyncCall::Context *ctx) {
        int32_t code = InputMethodAbility::GetInstance()->InsertText(ctxt->text);
        if (code == ErrorCode::NO_ERROR) {
            ctxt->status = napi_ok;
            ctxt->SetState(ctxt->status);
            ctxt->isInsertText = true;
        } else {
            ctxt->SetErrorCode(code);
        }
    };
    ctxt->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(ctxt), 1);
    return asyncCall.Call(env, exec);
}

napi_value JsTextInputClientEngine::GetForward(napi_env env, napi_callback_info info)
{
    auto ctxt = std::make_shared<GetForwardContext>();
    auto input = [ctxt](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        napi_status status = napi_generic_failure;
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[0], &valueType);
        if (argc < 1) {
            JsUtils::ThrowException(
                env, IMFErrorCode::EXCEPTION_PARAMCHECK, " should 1 or 2 parameters!", TypeCode::TYPE_NONE);
            return status;
        }
        if (valueType != napi_number) {
            JsUtils::ThrowException(env, IMFErrorCode::EXCEPTION_PARAMCHECK, " 'length'", TypeCode::TYPE_NUMBER);
            return status;
        }
        status = GetForwardLength(env, argv[0], ctxt);
        return status;
    };
    auto output = [ctxt](napi_env env, napi_value *result) -> napi_status {
        napi_value data = GetResult(env, ctxt->text);
        *result = data;
        return napi_ok;
    };
    auto exec = [ctxt](AsyncCall::Context *ctx) {
        std::u16string temp;
        int32_t code = InputMethodAbility::GetInstance()->GetTextBeforeCursor(ctxt->length, temp);
        if (code == ErrorCode::NO_ERROR) {
            ctxt->status = napi_ok;
            ctxt->SetState(ctxt->status);
            ctxt->text = Str16ToStr8(temp);
        } else {
            ctxt->SetErrorCode(code);
        }
    };
    ctxt->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(ctxt), 1);
    return asyncCall.Call(env, exec);
}

napi_value JsTextInputClientEngine::GetBackward(napi_env env, napi_callback_info info)
{
    auto ctxt = std::make_shared<GetBackwardContext>();
    auto input = [ctxt](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        napi_status status = napi_generic_failure;
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[0], &valueType);
        if (argc < 1) {
            JsUtils::ThrowException(
                env, IMFErrorCode::EXCEPTION_PARAMCHECK, " should 1 or 2 parameters!", TypeCode::TYPE_NONE);
            return status;
        }
        if (valueType != napi_number) {
            JsUtils::ThrowException(env, IMFErrorCode::EXCEPTION_PARAMCHECK, " 'length'", TypeCode::TYPE_NUMBER);
            return status;
        }
        status = GetBackwardLength(env, argv[0], ctxt);
        return status;
    };
    auto output = [ctxt](napi_env env, napi_value *result) -> napi_status {
        napi_value data = GetResult(env, ctxt->text);
        *result = data;
        return napi_ok;
    };
    auto exec = [ctxt](AsyncCall::Context *ctx) {
        std::u16string temp;
        int32_t code = InputMethodAbility::GetInstance()->GetTextAfterCursor(ctxt->length, temp);
        if (code == ErrorCode::NO_ERROR) {
            ctxt->status = napi_ok;
            ctxt->SetState(ctxt->status);
            ctxt->text = Str16ToStr8(temp);
        } else {
            ctxt->SetErrorCode(code);
        }
    };
    ctxt->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(ctxt), 1);
    return asyncCall.Call(env, exec);
}

napi_value JsTextInputClientEngine::GetEditorAttribute(napi_env env, napi_callback_info info)
{
    auto ctxt = std::make_shared<GetEditorAttributeContext>();
    auto input = [ctxt](napi_env env, size_t argc, napi_value *argv, napi_value self) -> napi_status {
        return napi_ok;
    };
    auto output = [ctxt](napi_env env, napi_value *result) -> napi_status {
        napi_value data = GetResultEditorAttribute(env, ctxt);
        *result = data;
        return napi_ok;
    };
    auto exec = [ctxt](AsyncCall::Context *ctx) {
        int32_t typeCode = InputMethodAbility::GetInstance()->GetEnterKeyType(ctxt->enterKeyType);
        int32_t patternCode = InputMethodAbility::GetInstance()->GetInputPattern(ctxt->inputPattern);
        if (typeCode == ErrorCode::NO_ERROR && patternCode == ErrorCode::NO_ERROR) {
            ctxt->status = napi_ok;
            ctxt->SetState(ctxt->status);
        } else {
            typeCode == ErrorCode::NO_ERROR ? ctxt->SetErrorCode(patternCode) : ctxt->SetErrorCode(typeCode);
        }
    };
    ctxt->SetAction(std::move(input), std::move(output));
    AsyncCall asyncCall(env, info, std::dynamic_pointer_cast<AsyncCall::Context>(ctxt), 0);
    return asyncCall.Call(env, exec);
}
} // namespace MiscServices
} // namespace OHOS
