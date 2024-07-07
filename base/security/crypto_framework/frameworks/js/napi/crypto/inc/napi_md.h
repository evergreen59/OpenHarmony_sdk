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

#ifndef NAPI_MD_H
#define NAPI_MD_H

#include <string>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

#include "md.h"

namespace OHOS {
namespace CryptoFramework {
class NapiMd {
public:
    explicit NapiMd(HcfMd *mdObj);
    ~NapiMd();
    static thread_local napi_ref classRef_;

    static void DefineMdJSClass(napi_env env, napi_value exports);
    static napi_value CreateMd(napi_env env, napi_callback_info info);
    static napi_value MdConstructor(napi_env env, napi_callback_info info);

    napi_value MdUpdate(napi_env env, napi_callback_info info);
    napi_value MdDoFinal(napi_env env, napi_callback_info info);
    napi_value GetMdLength(napi_env env, napi_callback_info info);

    HcfMd *GetMd()
    {
        return mdObj_;
    }

private:
    HcfMd *mdObj_ = nullptr;
};
} // namespace CryptoFramework
} // namespace OHOS

#endif // NAPI_X509_CERTIFICATE_H
