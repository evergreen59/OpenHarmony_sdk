/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_KITS_JS_MEDIALIBRARY_INCLUDE_NAPI_ERROR_H_
#define INTERFACES_KITS_JS_MEDIALIBRARY_INCLUDE_NAPI_ERROR_H_

#include <string>
#include "datashare_result_set.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "medialibrary_napi_utils.h"

namespace OHOS {
namespace Media {
struct NapiError {
    int32_t error = 0;
    std::string apiName;
    void SetApiName(const std::string &Name);
    void SaveError(const std::shared_ptr<DataShare::DataShareResultSet> &resultSet);
    void SaveError(int32_t ret);
    void HandleError(napi_env env, napi_value &errorObj);
    void ThrowError(napi_env env, int32_t err);
};
} // namespace Media
} // namespace OHOS
#endif  // INTERFACES_KITS_JS_MEDIALIBRARY_INCLUDE_NAPI_ERROR_H_