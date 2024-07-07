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

#include "napi_datashare_values_bucket.h"

#include "datashare_log.h"
#include "datashare_js_utils.h"
#include "datashare_value_object.h"

#include "securec.h"

namespace OHOS {
namespace DataShare {
napi_value DataShareValueBucketNewInstance(napi_env env, DataShareValuesBucket &valuesBucket)
{
    napi_value ret;
    NAPI_CALL(env, napi_create_object(env, &ret));
    std::map<std::string, DataShareValueObject> valuesMap = valuesBucket.valuesMap;
    std::map<std::string, DataShareValueObject>::iterator it;
    for (it = valuesMap.begin(); it != valuesMap.end(); ++it) {
        std::string key = it->first;
        auto valueObject = it->second;
        napi_value value = nullptr;
        switch (valueObject.GetType()) {
            case DataShareValueObjectType::TYPE_NULL: {
                    value = nullptr;
                } break;
            case DataShareValueObjectType::TYPE_INT: {
                    int64_t intVal = valueObject;
                    value = DataShareJSUtils::Convert2JSValue(env, intVal);
                } break;
            case DataShareValueObjectType::TYPE_DOUBLE: {
                    double doubleVal = valueObject;
                    value = DataShareJSUtils::Convert2JSValue(env, doubleVal);
                } break;
            case DataShareValueObjectType::TYPE_BLOB: {
                    std::vector<uint8_t> blobVal = valueObject;
                    value = DataShareJSUtils::Convert2JSValue(env, blobVal);
                } break;
            case DataShareValueObjectType::TYPE_BOOL: {
                    bool boolVal  = valueObject;
                    value = DataShareJSUtils::Convert2JSValue(env, boolVal);
                } break;
            default: {
                    std::string strVal = valueObject;
                    value = DataShareJSUtils::Convert2JSValue(env, strVal);
                } break;
        }
        NAPI_CALL(env, napi_set_named_property(env, ret, key.c_str(), value));
    }

    return ret;
}

bool SetValuesBucketObject(
    DataShareValuesBucket &valuesBucket, const napi_env &env, std::string keyStr, napi_value value)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    if (valueType == napi_string) {
        LOG_DEBUG("ValueObject is string");
        std::string valueString = DataShareJSUtils::UnwrapStringFromJS(env, value);
        valuesBucket.Put(keyStr, valueString);
    } else if (valueType == napi_number) {
        LOG_DEBUG("ValueObject is number");
        double valueNumber = 0;
        napi_get_value_double(env, value, &valueNumber);
        valuesBucket.Put(keyStr, valueNumber);
    } else if (valueType == napi_boolean) {
        LOG_DEBUG("ValueObject is boolean");
        bool valueBool = false;
        napi_get_value_bool(env, value, &valueBool);
        valuesBucket.Put(keyStr, valueBool);
    } else if (valueType == napi_null) {
        LOG_DEBUG("ValueObject is null");
        valuesBucket.Put(keyStr);
    } else if (valueType == napi_object) {
        LOG_DEBUG("ValueObject is Uint8Array");
        valuesBucket.Put(keyStr, DataShareJSUtils::Convert2U8Vector(env, value));
    } else {
        LOG_ERROR("valuesBucket error");
        return false;
    }
    return true;
}

bool AnalysisValuesBucket(DataShareValuesBucket &valuesBucket, const napi_env &env, const napi_value &arg)
{
    napi_value keys = 0;
    napi_get_property_names(env, arg, &keys);
    uint32_t arrLen = 0;
    napi_status status = napi_get_array_length(env, keys, &arrLen);
    if (status != napi_ok) {
        LOG_ERROR("ValuesBucket err");
        return false;
    }
    LOG_DEBUG("ValuesBucket num : %{public}u", arrLen);
    for (size_t i = 0; i < arrLen; ++i) {
        napi_value key = 0;
        status = napi_get_element(env, keys, i, &key);
        std::string keyStr = DataShareJSUtils::UnwrapStringFromJS(env, key);
        napi_value value = 0;
        napi_get_property(env, arg, key, &value);

        if (!SetValuesBucketObject(valuesBucket, env, keyStr, value)) {
            return false;
        }
    }
    return true;
}

bool GetValueBucketObject(DataShareValuesBucket &valuesBucket, const napi_env &env, const napi_value &arg)
{
    return AnalysisValuesBucket(valuesBucket, env, arg);
}
} // namespace DataShare
} // namespace OHOS