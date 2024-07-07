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

#ifndef OHOS_NAPI_META_DATA_H
#define OHOS_NAPI_META_DATA_H

#include "avmeta_data.h"
#include "napi/native_api.h"

namespace OHOS::AVSession {
class NapiMetaData {
public:
    static napi_status ConvertFilter(napi_env env, napi_value filter, AVMetaData::MetaMaskType& mask);

    static napi_status GetValue(napi_env env, napi_value in, AVMetaData& out);
    static napi_status SetValue(napi_env env, const AVMetaData& in, napi_value& out);

    using GetterType = std::function<napi_status(napi_env, napi_value in, AVMetaData& out)>;
    using SetterType = std::function<napi_status(napi_env env, const AVMetaData& in, napi_value& out)>;

private:
    static napi_status SetUndefinedMeta(napi_env env, napi_value& meta);

    static napi_status GetAssetId(napi_env env, napi_value in, AVMetaData& out);
    static napi_status SetAssetId(napi_env env, const AVMetaData& in, napi_value& out);

    static napi_status GetTitle(napi_env env, napi_value in, AVMetaData& out);
    static napi_status SetTitle(napi_env env, const AVMetaData& in, napi_value& out);

    static napi_status GetArtist(napi_env env, napi_value in, AVMetaData& out);
    static napi_status SetArtist(napi_env env, const AVMetaData& in, napi_value& out);

    static napi_status GetAuthor(napi_env env, napi_value in, AVMetaData& out);
    static napi_status SetAuthor(napi_env env, const AVMetaData& in, napi_value& out);

    static napi_status GetAlbum(napi_env env, napi_value in, AVMetaData& out);
    static napi_status SetAlbum(napi_env env, const AVMetaData& in, napi_value& out);

    static napi_status GetWriter(napi_env env, napi_value in, AVMetaData& out);
    static napi_status SetWriter(napi_env env, const AVMetaData& in, napi_value& out);

    static napi_status GetComposer(napi_env env, napi_value in, AVMetaData& out);
    static napi_status SetComposer(napi_env env, const AVMetaData& in, napi_value& out);

    static napi_status GetDuration(napi_env env, napi_value in, AVMetaData& out);
    static napi_status SetDuration(napi_env env, const AVMetaData& in, napi_value& out);

    static napi_status GetMediaImage(napi_env env, napi_value in, AVMetaData& out);
    static napi_status SetMediaImage(napi_env env, const AVMetaData& in, napi_value& out);
    static napi_status SetMediaImageUri(napi_env env, const AVMetaData& in, napi_value& out);

    static napi_status GetPublishDate(napi_env env, napi_value in, AVMetaData& out);
    static napi_status SetPublishDate(napi_env env, const AVMetaData& in, napi_value& out);

    static napi_status GetSubtitle(napi_env env, napi_value in, AVMetaData& out);
    static napi_status SetSubtitle(napi_env env, const AVMetaData& in, napi_value& out);

    static napi_status GetDescription(napi_env env, napi_value in, AVMetaData& out);
    static napi_status SetDescription(napi_env env, const AVMetaData& in, napi_value& out);

    static napi_status GetLyric(napi_env env, napi_value in, AVMetaData& out);
    static napi_status SetLyric(napi_env env, const AVMetaData& in, napi_value& out);

    static napi_status GetPreviousAssetId(napi_env env, napi_value in, AVMetaData& out);
    static napi_status SetPreviousAssetId(napi_env env, const AVMetaData& in, napi_value& out);

    static napi_status GetNextAssetId(napi_env env, napi_value in, AVMetaData& out);
    static napi_status SetNextAssetId(napi_env env, const AVMetaData& in, napi_value& out);

    static std::map<std::string, GetterType> getterMap_;
    static std::map<int32_t, SetterType> setterMap_;
    static std::pair<std::string, int32_t> filterMap_[AVMetaData::META_KEY_MAX];

    static constexpr int GETTER_INDEX = 0;
    static constexpr int SETTER_INDEX = 1;
    static constexpr int ENUM_INDEX = 2;
};
}
#endif // OHOS_NAPI_META_DATA_H