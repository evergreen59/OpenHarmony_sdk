/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#ifndef DISTRIBUTEDDATAMGR_PASTEBOARD_PASTEBOARD_NAPI_H
#define DISTRIBUTEDDATAMGR_PASTEBOARD_PASTEBOARD_NAPI_H

#include <unordered_map>
#include "pastedata_napi.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
namespace OHOS {
namespace MiscServicesNapi {
class PasteboardNapi {
public:
    static napi_value PasteBoardInit(napi_env env, napi_value exports);

private:
    using FUNC = napi_value (*)(napi_env, napi_value);
    static std::unordered_map<std::string, FUNC> createRecordMap_;
    static std::unordered_map<std::string, FUNC> createDataMap_;

    static napi_value CreateHtmlRecord(napi_env env, napi_value in);
    static napi_value CreatePlainTextRecord(napi_env env, napi_value in);
    static napi_value CreateUriRecord(napi_env env, napi_value in);
    static napi_value CreatePixelMapRecord(napi_env env, napi_value in);
    static napi_value CreateWantRecord(napi_env env, napi_value in);

    static PasteDataNapi *CreateDataCommon(napi_env env, napi_value in, std::string &str, napi_value &instance);
    static napi_value CreateHtmlData(napi_env env, napi_value in);
    static napi_value CreatePlainTextData(napi_env env, napi_value in);
    static napi_value CreateUriData(napi_env env, napi_value in);
    static napi_value CreatePixelMapData(napi_env env, napi_value in);
    static napi_value CreateWantData(napi_env env, napi_value in);

    static napi_value JScreateHtmlTextRecord(napi_env env, napi_callback_info info);
    static napi_value JScreateWantRecord(napi_env env, napi_callback_info info);
    static napi_value JScreateShareOption(napi_env env, napi_callback_info info);
    static napi_value JScreatePlainTextRecord(napi_env env, napi_callback_info info);
    static napi_value JScreatePixelMapRecord(napi_env env, napi_callback_info info);
    static napi_value JScreateUriRecord(napi_env env, napi_callback_info info);
    static napi_value JSCreateRecord(napi_env env, napi_callback_info info);

    static napi_value JScreateHtmlData(napi_env env, napi_callback_info info);
    static napi_value JScreateWantData(napi_env env, napi_callback_info info);
    static napi_value JScreatePlainTextData(napi_env env, napi_callback_info info);
    static napi_value JScreatePixelMapData(napi_env env, napi_callback_info info);
    static napi_value JScreateUriData(napi_env env, napi_callback_info info);
    static napi_value JSCreateKvData(
        napi_env env, const std::string &mimeType, const std::vector<uint8_t> &arrayBuffer);
    static napi_value JSCreateData(napi_env env, napi_callback_info info);
    static napi_value JSgetSystemPasteboard(napi_env env, napi_callback_info info);
};
} // namespace MiscServicesNapi
} // namespace OHOS

#endif // DISTRIBUTEDDATAMGR_PASTEBOARD_PASTEBOARD_NAPI_H
