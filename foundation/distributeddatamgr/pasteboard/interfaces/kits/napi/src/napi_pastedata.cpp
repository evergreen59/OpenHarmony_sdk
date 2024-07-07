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
#include <cstddef>
#include "pastedata_napi.h"
#include "pastedata_record_napi.h"
#include "pasteboard_common.h"
#include "pasteboard_hilog.h"
#include "napi_common.h"
#include "pasteboard_js_err.h"
using namespace OHOS::MiscServices;
using namespace OHOS::Media;

namespace OHOS {
namespace MiscServicesNapi {
namespace {
constexpr int ARGC_TYPE_SET0 = 0;
constexpr int ARGC_TYPE_SET1 = 1;
constexpr int ARGC_TYPE_SET2 = 2;
const int32_t STR_MAX_SIZE = 256;
constexpr int32_t MIMETYPE_MAX_SIZE = 1024;
constexpr int32_t MAX_TEXT_LEN = 20 * 1024 * 1024;
constexpr size_t STR_TAIL_LENGTH = 1;
constexpr size_t MAX_RECORD_NUM = 512;
}  // namespace
static thread_local napi_ref g_pasteData = nullptr;

PasteDataNapi::PasteDataNapi() : env_(nullptr), wrapper_(nullptr)
{
    value_ = std::make_shared<PasteData>();
}

PasteDataNapi::~PasteDataNapi()
{
    napi_delete_reference(env_, wrapper_);
}

napi_value PasteDataNapi::AddHtmlRecord(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "AddHtmlRecord is called!");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc > ARGC_TYPE_SET0, "Wrong number of arguments");

    std::string str;
    bool ret = GetValue(env, argv[0], str);
    if (!ret) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Failed to GetValue!");
        return nullptr;
    }

    PasteDataNapi *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if ((status != napi_ok) || (obj == nullptr)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get AddHtmlRecord object failed");
        return nullptr;
    }
    obj->value_->AddHtmlRecord(str);
    return nullptr;
}

napi_value PasteDataNapi::AddPixelMapRecord(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "AddPixelMapRecord is called begin!");
    size_t argc = ARGC_TYPE_SET1;
    napi_value argv[ARGC_TYPE_SET1] = { 0 };
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc > ARGC_TYPE_SET0, "Wrong number of arguments");

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valueType));
    NAPI_ASSERT(env, valueType == napi_object, "Wrong argument type. Object expected.");

    std::shared_ptr<PixelMap> pixelMap = PixelMapNapi::GetPixelMap(env, argv[0]);
    if (pixelMap == nullptr) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Failed to GetPixelMap!");
        return nullptr;
    }

    PasteDataNapi *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if ((status != napi_ok) || (obj == nullptr)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "unwrap failed");
        return nullptr;
    }
    obj->value_->AddPixelMapRecord(pixelMap);
    return nullptr;
}

napi_value PasteDataNapi::AddTextRecord(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "AddTextRecord is called!");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc > ARGC_TYPE_SET0, "Wrong number of arguments");

    std::string str;
    bool ret = GetValue(env, argv[0], str);
    if (!ret) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Failed to GetValue!");
        return nullptr;
    }

    PasteDataNapi *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if ((status != napi_ok) || (obj == nullptr)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get AddTextRecord object failed");
        return nullptr;
    }
    obj->value_->AddTextRecord(str);
    return nullptr;
}

napi_value PasteDataNapi::AddUriRecord(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "AddUriRecord is called!");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc > ARGC_TYPE_SET0, "Wrong number of arguments");

    std::string str;
    bool ret = GetValue(env, argv[0], str);
    if (!ret) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Failed to GetValue!");
        return nullptr;
    }

    PasteDataNapi *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if ((status != napi_ok) || (obj == nullptr)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get AddUriRecord object failed");
        return nullptr;
    }
    obj->value_->AddUriRecord(OHOS::Uri(str));
    return nullptr;
}

napi_value PasteDataNapi::GetPrimaryHtml(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "GetPrimaryHtml is called!");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= ARGC_TYPE_SET0, "Wrong number of arguments");

    PasteDataNapi *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if ((status != napi_ok) || (obj == nullptr)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get GetPrimaryHtml object failed");
        return nullptr;
    }

    std::shared_ptr<std::string> p = obj->value_->GetPrimaryHtml();
    if (p == nullptr) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get GetPrimaryHtml failed");
        return nullptr;
    }

    napi_value result = nullptr;
    napi_create_string_utf8(env, p->c_str(), NAPI_AUTO_LENGTH, &result);
    return result;
}

napi_value PasteDataNapi::GetPrimaryPixelMap(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "GetPrimaryPixelMap is called!");
    size_t argc = ARGC_TYPE_SET1;
    napi_value argv[ARGC_TYPE_SET1] = { 0 };
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= ARGC_TYPE_SET0, "Wrong number of arguments");

    PasteDataNapi *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if ((status != napi_ok) || (obj == nullptr)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "unwrap failed");
        return nullptr;
    }

    std::shared_ptr<PixelMap> pixelMap = obj->value_->GetPrimaryPixelMap();
    if (!pixelMap) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "pixelMap is nullptr");
        return nullptr;
    }

    napi_value jsPixelMap = PixelMapNapi::CreatePixelMap(env, pixelMap);
    return jsPixelMap;
}

napi_value PasteDataNapi::GetPrimaryText(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "GetPrimaryText is called!");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= ARGC_TYPE_SET0, "Wrong number of arguments");

    PasteDataNapi *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if ((status != napi_ok) || (obj == nullptr)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get GetPrimaryText object failed");
        return nullptr;
    }

    std::shared_ptr<std::string> p = obj->value_->GetPrimaryText();
    if (p == nullptr) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get GetPrimaryText failed");
        return nullptr;
    }

    napi_value result = nullptr;
    napi_create_string_utf8(env, p->c_str(), NAPI_AUTO_LENGTH, &result);
    return result;
}

napi_value PasteDataNapi::GetPrimaryUri(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "GetPrimaryUri is called!");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= ARGC_TYPE_SET0, "Wrong number of arguments");

    PasteDataNapi *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if ((status != napi_ok) || (obj == nullptr)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get GetPrimaryUri object failed");
        return nullptr;
    }

    std::shared_ptr<OHOS::Uri> p = obj->value_->GetPrimaryUri();
    if (p == nullptr) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get GetPrimaryUri failed");
        return nullptr;
    }

    std::string text = p->ToString();
    napi_value result = nullptr;
    napi_create_string_utf8(env, text.c_str(), NAPI_AUTO_LENGTH, &result);
    return result;
}

napi_value PasteDataNapi::HasMimeType(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "HasMimeType is called!");
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if ((!CheckExpression(env, argc > ARGC_TYPE_SET0, JSErrorCode::INVALID_PARAMETERS,
            "Parameter error. Wrong number of arguments."))
        || (!CheckArgsType(env, argv[0], napi_string, "Parameter error. The type of mimeType must be string."))) {
        return nullptr;
    }

    std::string mimeType;
    if (!GetValue(env, argv[0], mimeType)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Failed to GetValue!");
        return nullptr;
    }

    PasteDataNapi *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if ((status != napi_ok) || (obj == nullptr)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get AddHtmlRecord object failed");
        return nullptr;
    }

    bool ret = obj->value_->HasMimeType(mimeType);
    napi_value result = nullptr;
    napi_get_boolean(env, ret, &result);
    return result;
}

napi_value PasteDataNapi::HasType(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "HasType is called!");
    return HasMimeType(env, info);
}

PasteDataNapi* PasteDataNapi::RemoveAndGetRecordCommon(napi_env env, napi_callback_info info, uint32_t &index)
{
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));

    if ((!CheckExpression(env, argc > ARGC_TYPE_SET0, JSErrorCode::INVALID_PARAMETERS,
            "Parameter error. Wrong number of arguments."))
        || (!CheckArgsType(env, argv[0], napi_number, "Parameter error. The type of mimeType must be number."))) {
        return nullptr;
    }
    PasteDataNapi *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if ((status != napi_ok) || (obj == nullptr)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get RemoveRecord object failed");
        return nullptr;
    }
    NAPI_CALL(env, napi_get_value_uint32(env, argv[0], &index));
    return obj;
}

napi_value PasteDataNapi::RemoveRecordAt(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "RemoveRecordAt is called!");
    uint32_t index = 0;
    PasteDataNapi *obj = RemoveAndGetRecordCommon(env, info, index);
    if (obj == nullptr) {
        return nullptr;
    }
    bool ret = obj->value_->RemoveRecordAt(index);
    napi_value result = nullptr;
    napi_get_boolean(env, ret, &result);
    return result;
}

napi_value PasteDataNapi::RemoveRecord(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "RemoveRecord is called!");
    uint32_t index = 0;
    PasteDataNapi *obj = RemoveAndGetRecordCommon(env, info, index);
    if (obj == nullptr
        || !CheckExpression(env, index < obj->value_->GetRecordCount(),
            JSErrorCode::OUT_OF_RANGE, "index out of range.")) {
        return nullptr;
    }
    obj->value_->RemoveRecordAt(index);
    return nullptr;
}

napi_value PasteDataNapi::GetPrimaryMimeType(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "GetPrimaryMimeType is called!");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= ARGC_TYPE_SET0, "Wrong number of arguments");

    PasteDataNapi *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if ((status != napi_ok) || (obj == nullptr)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get GetPrimaryMimeType object failed");
        return nullptr;
    }
    std::shared_ptr<std::string> mimeType = obj->value_->GetPrimaryMimeType();
    if (mimeType == nullptr) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get GetPrimaryMimeType failed");
        return nullptr;
    }
    napi_value result = nullptr;
    napi_create_string_utf8(env, mimeType->c_str(), NAPI_AUTO_LENGTH, &result);

    return result;
}

napi_value PasteDataNapi::GetRecordCount(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "GetRecordCount is called!");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= ARGC_TYPE_SET0, "Wrong number of arguments");

    PasteDataNapi *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if ((status != napi_ok) || (obj == nullptr)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get GetRecordCount object failed");
        return nullptr;
    }

    size_t count = obj->value_->GetRecordCount();
    napi_value result = nullptr;
    napi_create_int64(env, count, &result);

    return result;
}

napi_value PasteDataNapi::GetTag(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "GetTag is called!");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= ARGC_TYPE_SET0, "Wrong number of arguments");

    PasteDataNapi *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if ((status != napi_ok) || (obj == nullptr)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get GetTag object failed");
        return nullptr;
    }
    std::string tag = obj->value_->GetTag();
    napi_value result = nullptr;
    napi_create_string_utf8(env, tag.c_str(), NAPI_AUTO_LENGTH, &result);

    return result;
}

napi_value PasteDataNapi::GetMimeTypes(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "GetMimeTypes is called!");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= ARGC_TYPE_SET0, "Wrong number of arguments");

    PasteDataNapi *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if ((status != napi_ok) || (obj == nullptr)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get GetMimeTypes object failed");
        return nullptr;
    }
    std::vector<std::string> mimeTypes = obj->value_->GetMimeTypes();
    if (mimeTypes.size() == 0) {
        return nullptr;
    }

    napi_value nMimeTypes = nullptr;
    if (napi_create_array(env, &nMimeTypes) != napi_ok) {
        return nullptr;
    }
    size_t index = 0;
    napi_value value = nullptr;
    for (auto type : mimeTypes) {
        napi_create_string_utf8(env, type.c_str(), NAPI_AUTO_LENGTH, &value);
        napi_set_element(env, nMimeTypes, index, value);
        index++;
    }
    return nMimeTypes;
}

void PasteDataNapi::AddRecord(napi_env env, napi_value *argv, size_t argc, PasteDataNapi *obj)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "AddRecordV9!");
    if (!CheckExpression(env, obj->value_->GetRecordCount() < MAX_RECORD_NUM, JSErrorCode::RECORD_EXCEEDS_LIMIT,
        "The count of records in PasteData exceeds MAX_RECORD_NUM.")) {
        return;
    }
    std::string mimeType;
    if (!CheckArgs(env, argv, argc, mimeType)) {
        return;
    }
    bool isArrayBuffer = false;
    NAPI_CALL_RETURN_VOID(env, napi_is_arraybuffer(env, argv[1], &isArrayBuffer));
    if (isArrayBuffer) {
        void *data = nullptr;
        size_t dataLen = 0;
        NAPI_CALL_RETURN_VOID(env, napi_get_arraybuffer_info(env, argv[1], &data, &dataLen));
        obj->value_->AddKvRecord(mimeType,
            std::vector<uint8_t>(reinterpret_cast<uint8_t *>(data), reinterpret_cast<uint8_t *>(data) + dataLen));
        return;
    }
    if (mimeType == MIMETYPE_PIXELMAP) {
        std::shared_ptr<PixelMap> pixelMap = PixelMapNapi::GetPixelMap(env, argv[1]);
        obj->value_->AddPixelMapRecord(pixelMap);
        return;
    } else if (mimeType == MIMETYPE_TEXT_WANT) {
        OHOS::AAFwk::Want want;
        AppExecFwk::UnwrapWant(env, argv[1], want);
        obj->value_->AddWantRecord(std::make_shared<OHOS::AAFwk::Want>(want));
        return;
    }

    std::string str;
    bool ret = GetValue(env, argv[1], str);
    if (!ret) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Failed to GetValue!");
        return;
    }
    if (mimeType == MIMETYPE_TEXT_HTML) {
        obj->value_->AddHtmlRecord(str);
    } else if (mimeType == MIMETYPE_TEXT_PLAIN) {
        obj->value_->AddTextRecord(str);
    } else {
        obj->value_->AddUriRecord(OHOS::Uri(str));
    }
}

bool PasteDataNapi::SetStringProp(
    napi_env env, const std::string &propName, napi_value &propValueNapi, PasteDataRecord::Builder &builder)
{
    std::string propValue;
    bool ret = GetValue(env, propValueNapi, propValue);
    if (!ret) {
        return false;
    }
    if ((propName == "mimeType") && (propValue.size() <= MIMETYPE_MAX_SIZE)) {
        builder.SetMimeType(propValue);
    } else if ((propName == "htmlText") && (propValue.size() <= MAX_TEXT_LEN)) {
        builder.SetHtmlText(std::make_shared<std::string>(propValue));
    } else if ((propName == "plainText") && (propValue.size() <= MAX_TEXT_LEN)) {
        builder.SetPlainText(std::make_shared<std::string>(propValue));
    } else if (propName == "uri") {
        builder.SetUri(std::make_shared<OHOS::Uri>(Uri(propValue)));
    } else {
        return false;
    }
    return true;
}

std::shared_ptr<MiscServices::PasteDataRecord> PasteDataNapi::ParseRecord(napi_env env, napi_value &recordNapi)
{
    napi_value propNames = nullptr;
    NAPI_CALL(env, napi_get_property_names(env, recordNapi, &propNames));
    uint32_t propNameNums = 0;
    NAPI_CALL(env, napi_get_array_length(env, propNames, &propNameNums));
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "propNameNums = %{public}d", propNameNums);
    PasteDataRecord::Builder builder("");
    for (uint32_t i = 0; i < propNameNums; i++) {
        napi_value propNameNapi = nullptr;
        NAPI_CALL(env, napi_get_element(env, propNames, i, &propNameNapi));
        size_t len = 0;
        char str[STR_MAX_SIZE] = { 0 };
        NAPI_CALL(env, napi_get_value_string_utf8(env, propNameNapi, str, STR_MAX_SIZE - STR_TAIL_LENGTH, &len));
        napi_value propValueNapi = nullptr;
        NAPI_CALL(env, napi_get_named_property(env, recordNapi, str, &propValueNapi));
        std::string propName = str;
        PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "propName = %{public}s,", propName.c_str());

        if (propName == "mimeType" || propName == "htmlText" || propName == "plainText" || propName == "uri") {
            if (!SetStringProp(env, propName, propValueNapi, builder)) {
                return nullptr;
            }
        } else if (propName == "want") {
            AAFwk::Want want;
            if (OHOS::AppExecFwk::UnwrapWant(env, propValueNapi, want) != true) {
                return nullptr;
            }
            builder.SetWant(std::make_shared<AAFwk::Want>(want));
        } else if (propName == "pixelMap") {
            std::shared_ptr<PixelMap> pixelMap = PixelMapNapi::GetPixelMap(env, propValueNapi);
            if (pixelMap == nullptr) {
                return nullptr;
            }
            builder.SetPixelMap(pixelMap);
        } else if (propName == "data") {
            std::shared_ptr<MineCustomData> customData = PasteDataRecordNapi::GetNativeKvData(env, propValueNapi);
            if (customData == nullptr) {
                return nullptr;
            }
            builder.SetCustomData(customData);
        }
    }
    return builder.Build();
}

void PasteDataNapi::AddRecord(napi_env env, napi_value argv, PasteDataNapi *obj)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "AddPasteDataRecord!");

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL_RETURN_VOID(env, napi_typeof(env, argv, &valueType));
    NAPI_ASSERT_RETURN_VOID(env, valueType == napi_object, "Wrong argument type. Object expected.");

    std::shared_ptr<PasteDataRecord> pasteDataRecord = ParseRecord(env, argv);
    if (pasteDataRecord == nullptr) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "ParseRecord failed!");
        return;
    }
    obj->value_->AddRecord(*pasteDataRecord);
}

napi_value PasteDataNapi::AddRecord(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "AddRecord is called!");
    size_t argc = ARGC_TYPE_SET2;
    napi_value argv[ARGC_TYPE_SET2] = { 0 };
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc > 0, "Wrong number of arguments");
    PasteDataNapi *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if ((status != napi_ok) || (obj == nullptr)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get AddRecord object failed");
        return nullptr;
    }

    if (argc == ARGC_TYPE_SET1) {
        AddRecord(env, argv[0], obj);
        return nullptr;
    }
    AddRecord(env, argv, argc, obj);
    return nullptr;
}

napi_value PasteDataNapi::ReplaceRecordAt(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "ReplaceRecordAt is called!");
    size_t argc = ARGC_TYPE_SET2;
    napi_value argv[ARGC_TYPE_SET2] = { 0 };
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_boolean(env, false, &result);

    NAPI_CALL_BASE(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL), result);
    NAPI_ASSERT(env, argc > ARGC_TYPE_SET1, "Wrong number of arguments");
    napi_valuetype valueType = napi_undefined;
    NAPI_CALL_BASE(env, napi_typeof(env, argv[0], &valueType), result);
    NAPI_ASSERT(env, valueType == napi_number, "Wrong argument type. number expected.");
    NAPI_CALL_BASE(env, napi_typeof(env, argv[1], &valueType), result);
    NAPI_ASSERT(env, valueType == napi_object, "Wrong argument type. Object expected.");

    PasteDataNapi *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if ((status != napi_ok) || (obj == nullptr)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get ReplaceRecordAt object failed");
        return result;
    }

    std::shared_ptr<PasteDataRecord> pasteDataRecord = ParseRecord(env, argv[1]);
    if (pasteDataRecord == nullptr) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "ParseRecord failed!");
        return result;
    }

    int64_t number = 0;
    napi_get_value_int64(env, argv[0], &number);
    bool ret = obj->value_->ReplaceRecordAt(number, pasteDataRecord);
    napi_get_boolean(env, ret, &result);

    return result;
}

napi_value PasteDataNapi::ReplaceRecord(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "ReplaceRecord is called!");
    size_t argc = ARGC_TYPE_SET2;
    napi_value argv[ARGC_TYPE_SET2] = { 0 };
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if (!CheckExpression(env, argc > ARGC_TYPE_SET1, JSErrorCode::INVALID_PARAMETERS, "Wrong number of arguments.")
        || !CheckArgsType(env, argv[0], napi_number, "The type of mimeType must be number.")) {
        return nullptr;
    }

    PasteDataNapi *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if ((status != napi_ok) || (obj == nullptr)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get ReplaceRecord object failed");
        return nullptr;
    }
    uint32_t index = 0;
    NAPI_CALL(env, napi_get_value_uint32(env, argv[0], &index));
    if (!CheckExpression(env, index < obj->value_->GetRecordCount(),
        JSErrorCode::OUT_OF_RANGE, "index out of range.")) {
        return nullptr;
    }

    if (!CheckArgsType(env, argv[1], napi_object, "The type of record must be PasteDataRecord.")) {
        return nullptr;
    }
    std::shared_ptr<PasteDataRecord> pasteDataRecord = ParseRecord(env, argv[1]);
    if (!CheckExpression(env, pasteDataRecord != nullptr, JSErrorCode::INVALID_PARAMETERS,
        "The type of record must be PasteDataRecord.")) {
        return nullptr;
    }
    obj->value_->ReplaceRecordAt(index, pasteDataRecord);
    return nullptr;
}

napi_value PasteDataNapi::AddWantRecord(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "AddWantRecord is called!");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc > ARGC_TYPE_SET0, "Wrong number of arguments");

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valueType));
    NAPI_ASSERT(env, valueType == napi_object, "Wrong argument type. Object expected.");
    OHOS::AAFwk::Want want;
    if (!OHOS::AppExecFwk::UnwrapWant(env, argv[0], want)) {
        return nullptr;
    }

    PasteDataNapi *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if ((status != napi_ok) || (obj == nullptr)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get AddWantRecord object failed");
        return nullptr;
    }

    obj->value_->AddWantRecord(std::make_shared<OHOS::AAFwk::Want>(want));
    return nullptr;
}

napi_value PasteDataNapi::GetPrimaryWant(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "GetPrimaryWant is called!");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= ARGC_TYPE_SET0, "Wrong number of arguments");

    PasteDataNapi *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if ((status != napi_ok) || (obj == nullptr)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get GetPrimaryWant object failed");
        return nullptr;
    }

    std::shared_ptr<OHOS::AAFwk::Want> want = obj->value_->GetPrimaryWant();
    if (!want) {
        return nullptr;
    }
    return OHOS::AppExecFwk::WrapWant(env, *want);
}

bool PasteDataNapi::SetNapiProperty(napi_env env, const PasteDataProperty &property, napi_value &nProperty)
{
    napi_value value = nullptr;
    napi_value arr = nullptr;
    int count = 0;

    // additions : {[key: string]: object}
    value = OHOS::AppExecFwk::WrapWantParams(env, property.additions);
    napi_set_named_property(env, nProperty, "additions", value);

    // mimeTypes: Array<string>
    napi_create_array(env, &arr);
    for (auto vec : property.mimeTypes) {
        napi_create_string_utf8(env, vec.c_str(), NAPI_AUTO_LENGTH, &value);
        napi_set_element(env, arr, count, value);
        count++;
    }
    napi_set_named_property(env, nProperty, "mimeTypes", arr);

    // tag: string
    napi_create_string_utf8(env, property.tag.c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, nProperty, "tag", value);

    // timestamp: number
    napi_create_int64(env, property.timestamp, &value);
    napi_set_named_property(env, nProperty, "timestamp", value);

    // localOnly: boolean
    napi_get_boolean(env, property.localOnly, &value);
    napi_set_named_property(env, nProperty, "localOnly", value);

    napi_create_int32(env, static_cast<int32_t>(property.shareOption), &value);
    napi_set_named_property(env, nProperty, "shareOption", value);
    return true;
}

napi_value PasteDataNapi::GetProperty(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "GetProperty is called!");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= ARGC_TYPE_SET0, "Wrong number of arguments");

    PasteDataNapi *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if ((status != napi_ok) || (obj == nullptr)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Get GetProperty object failed");
        return nullptr;
    }
    PasteDataProperty property = obj->value_->GetProperty();
    napi_value nProperty = nullptr;
    napi_create_object(env, &nProperty);
    if (!SetNapiProperty(env, property, nProperty)) {
        return nullptr;
    }
    return nProperty;
}

napi_value PasteDataNapi::GetRecordAt(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "GetRecordAt is called!");
    uint32_t index = 0;
    PasteDataNapi *obj = RemoveAndGetRecordCommon(env, info, index);
    if (obj == nullptr) {
        return nullptr;
    }

    std::shared_ptr<PasteDataRecord> record = obj->value_->GetRecordAt(index);
    napi_value instance = nullptr;
    PasteDataRecordNapi::NewInstanceByRecord(env, instance, record);
    return instance;
}

napi_value PasteDataNapi::GetRecord(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "GetRecord is called!");
    uint32_t index = 0;
    PasteDataNapi *obj = RemoveAndGetRecordCommon(env, info, index);
    if (obj == nullptr
        || !CheckExpression(env, index < obj->value_->GetRecordCount(),
            JSErrorCode::OUT_OF_RANGE, "index out of range.")) {
        return nullptr;
    }

    std::shared_ptr<PasteDataRecord> record = obj->value_->GetRecordAt(index);
    napi_value instance = nullptr;
    PasteDataRecordNapi::NewInstanceByRecord(env, instance, record);
    return instance;
}

void PasteDataNapi::SetProperty(napi_env env, napi_value in, PasteDataNapi *obj)
{
    napi_value propertyNames = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_property_names(env, in, &propertyNames));
    uint32_t propertyNamesNum = 0;
    NAPI_CALL_RETURN_VOID(env, napi_get_array_length(env, propertyNames, &propertyNamesNum));
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_JS_NAPI, "propertyNamesNum = %{public}d", propertyNamesNum);

    for (uint32_t i = 0; i < propertyNamesNum; i++) {
        napi_value propertyNameNapi = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_get_element(env, propertyNames, i, &propertyNameNapi));
        size_t len = 0;
        char str[STR_MAX_SIZE] = { 0 };
        NAPI_CALL_RETURN_VOID(env, napi_get_value_string_utf8(env, propertyNameNapi, str, STR_MAX_SIZE, &len));
        std::string propertyName = str;
        PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "propertyName = %{public}s,", propertyName.c_str());
        napi_value propertyNameValueNapi = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, in, str, &propertyNameValueNapi));
        if (propertyName == "shareOption") {
            int32_t shareOptionValue = 0;
            NAPI_CALL_RETURN_VOID(env, napi_get_value_int32(env, propertyNameValueNapi, &shareOptionValue));
            PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "shareOptionValue = %{public}d,", shareOptionValue);
            obj->value_->SetShareOption(static_cast<ShareOption>(shareOptionValue));
        }
        if (propertyName == "tag") {
            char tagValue[STR_MAX_SIZE] = { 0 };
            size_t tagValueLen = 0;
            NAPI_CALL_RETURN_VOID(env,
                napi_get_value_string_utf8(env, propertyNameValueNapi, tagValue, STR_MAX_SIZE, &tagValueLen));
            PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "tagValue = %{public}s,", tagValue);
            std::string tagValueStr = tagValue;
            obj->value_->SetTag(tagValueStr);
        }
        if (propertyName == "additions") {
            AAFwk::WantParams additions;
            bool ret = OHOS::AppExecFwk::UnwrapWantParams(env, propertyNameValueNapi, additions);
            PASTEBOARD_HILOGI(PASTEBOARD_MODULE_CLIENT, "additions ret = %{public}d,", ret);
            obj->value_->SetAdditions(additions);
        }
    }
}

bool PasteDataNapi::IsProperty(napi_env env, napi_value in)
{
    napi_valuetype valueType = napi_undefined;
    NAPI_CALL_BASE(env, napi_typeof(env, in, &valueType), false);
    if (valueType != napi_object) {
        return false;
    }
    napi_value propertyNames = nullptr;
    NAPI_CALL_BASE(env, napi_get_property_names(env, in, &propertyNames), false);
    uint32_t propertyNamesNum = 0;
    NAPI_CALL_BASE(env, napi_get_array_length(env, propertyNames, &propertyNamesNum), false);
    bool hasProperty = false;
    const char *key[] = { "additions", "mimeTypes", "tag", "timestamp", "localOnly", "shareOption" };
    const napi_valuetype type[] = { napi_object, napi_object, napi_string, napi_number, napi_boolean, napi_number };
    napi_value propertyValue = nullptr;
    for (uint32_t i = 0; i < propertyNamesNum; i++) {
        NAPI_CALL_BASE(env, napi_has_named_property(env, in, key[i], &hasProperty), false);
        if (!hasProperty) {
            return false;
        }
        NAPI_CALL_BASE(env, napi_get_named_property(env, in, key[i], &propertyValue), false);
        if (i == ARGC_TYPE_SET1) {
            bool isArray = false;
            NAPI_CALL_BASE(env, napi_is_array(env, propertyValue, &isArray), false);
            if (!isArray) {
                return false;
            }
            continue;
        }
        NAPI_CALL_BASE(env, napi_typeof(env, propertyValue, &valueType), false);
        if (valueType != type[i]) {
            return false;
        }
    }
    return true;
}

napi_value PasteDataNapi::SetProperty(napi_env env, napi_callback_info info)
{
    PASTEBOARD_HILOGI(PASTEBOARD_MODULE_JS_NAPI, "SetProperty is called!");
    size_t argc = ARGC_TYPE_SET1;
    napi_value argv[ARGC_TYPE_SET1] = { 0 };
    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if (!CheckExpression(env, argc > 0, JSErrorCode::INVALID_PARAMETERS, "Parameter error. Wrong number of arguments.")
        || !CheckExpression(env, IsProperty(env, argv[0]), JSErrorCode::INVALID_PARAMETERS,
            "Parameter error. The type of property must be PasteDataProperty.")) {
        return nullptr;
    }

    PasteDataNapi *obj = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&obj));
    if ((status != napi_ok) || (obj == nullptr)) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "napi_unwrap failed");
        return nullptr;
    }
    SetProperty(env, argv[0], obj);
    return nullptr;
}

napi_value PasteDataNapi::PasteDataInit(napi_env env, napi_value exports)
{
    napi_status status = napi_ok;
    napi_property_descriptor descriptors[] = {
        DECLARE_NAPI_FUNCTION("addHtmlRecord", AddHtmlRecord),
        DECLARE_NAPI_FUNCTION("addWantRecord", AddWantRecord),
        DECLARE_NAPI_FUNCTION("addRecord", AddRecord),
        DECLARE_NAPI_FUNCTION("addTextRecord", AddTextRecord),
        DECLARE_NAPI_FUNCTION("addUriRecord", AddUriRecord),
        DECLARE_NAPI_FUNCTION("addPixelMapRecord", AddPixelMapRecord),
        DECLARE_NAPI_FUNCTION("getMimeTypes", GetMimeTypes),
        DECLARE_NAPI_FUNCTION("getPrimaryHtml", GetPrimaryHtml),
        DECLARE_NAPI_FUNCTION("getPrimaryWant", GetPrimaryWant),
        DECLARE_NAPI_FUNCTION("getPrimaryMimeType", GetPrimaryMimeType),
        DECLARE_NAPI_FUNCTION("getPrimaryText", GetPrimaryText),
        DECLARE_NAPI_FUNCTION("getPrimaryUri", GetPrimaryUri),
        DECLARE_NAPI_FUNCTION("getPrimaryPixelMap", GetPrimaryPixelMap),
        DECLARE_NAPI_FUNCTION("getProperty", GetProperty),
        DECLARE_NAPI_FUNCTION("getRecordAt", GetRecordAt),
        DECLARE_NAPI_FUNCTION("getRecord", GetRecord),
        DECLARE_NAPI_FUNCTION("getRecordCount", GetRecordCount),
        DECLARE_NAPI_FUNCTION("getTag", GetTag),
        DECLARE_NAPI_FUNCTION("hasMimeType", HasMimeType),
        DECLARE_NAPI_FUNCTION("hasType", HasType),
        DECLARE_NAPI_FUNCTION("removeRecordAt", RemoveRecordAt),
        DECLARE_NAPI_FUNCTION("removeRecord", RemoveRecord),
        DECLARE_NAPI_FUNCTION("replaceRecordAt", ReplaceRecordAt),
        DECLARE_NAPI_FUNCTION("replaceRecord", ReplaceRecord),
        DECLARE_NAPI_FUNCTION("setProperty", SetProperty)
    };

    napi_value constructor;
    napi_define_class(env, "PasteData", NAPI_AUTO_LENGTH, New, nullptr,
        sizeof(descriptors) / sizeof(napi_property_descriptor), descriptors, &constructor);
    if (status != napi_ok) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "Failed to define class at Init");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &g_pasteData);
    if (status != napi_ok) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "PasteDataNapi Init create reference failed");
        return nullptr;
    }
    napi_set_named_property(env, exports, "PasteData", constructor);
    return exports;
}

void PasteDataNapi::Destructor(napi_env env, void *nativeObject, void *finalize_hint)
{
    PasteDataNapi *obj = static_cast<PasteDataNapi *>(nativeObject);
    delete obj;
}

napi_value PasteDataNapi::New(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_TYPE_SET1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);

    // get native object
    PasteDataNapi *obj = new PasteDataNapi();
    obj->env_ = env;
    NAPI_CALL(env, napi_wrap(env, thisVar, obj, PasteDataNapi::Destructor,
        nullptr, // finalize_hint
        &obj->wrapper_));
    return thisVar;
}

napi_status PasteDataNapi::NewInstance(napi_env env, napi_value &instance)
{
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_JS_NAPI, "enter");
    napi_status status;
    napi_value constructor;
    status = napi_get_reference_value(env, g_pasteData, &constructor);
    if (status != napi_ok) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "get reference failed");
        return status;
    }

    status = napi_new_instance(env, constructor, 0, nullptr, &instance);
    if (status != napi_ok) {
        PASTEBOARD_HILOGE(PASTEBOARD_MODULE_JS_NAPI, "new instance failed");
        return status;
    }
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_JS_NAPI, "new instance ok");
    return napi_ok;
}

bool PasteDataNapi::IsPasteData(napi_env env, napi_value in)
{
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_JS_NAPI, "enter");
    napi_valuetype type = napi_undefined;
    NAPI_CALL_BASE(env, napi_typeof(env, in, &type), false);
    if (type != napi_object) {
        return false;
    }
    napi_value constructor;
    bool isPasteData = false;
    NAPI_CALL_BASE(env, napi_get_reference_value(env, g_pasteData, &constructor), false);
    NAPI_CALL_BASE(env, napi_instanceof(env, in, constructor, &isPasteData), false);
    PASTEBOARD_HILOGD(PASTEBOARD_MODULE_JS_NAPI, "isPasteData is [%{public}d]", isPasteData);
    return isPasteData;
}
} // namespace MiscServicesNapi
} // namespace OHOS
