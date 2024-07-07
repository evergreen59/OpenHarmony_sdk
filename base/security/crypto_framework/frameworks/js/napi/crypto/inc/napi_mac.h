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

#ifndef NAPI_MAC_H
#define NAPI_MAC_H

#include <string>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

#include "mac.h"

namespace OHOS {
namespace CryptoFramework {
class NapiMac {
public:
    explicit NapiMac(HcfMac *macObj);
    ~NapiMac();
    static thread_local napi_ref classRef_;

    static void DefineMacJSClass(napi_env env, napi_value exports);
    static napi_value CreateMac(napi_env env, napi_callback_info info);
    static napi_value MacConstructor(napi_env env, napi_callback_info info);

    napi_value MacInit(napi_env env, napi_callback_info info);
    napi_value MacUpdate(napi_env env, napi_callback_info info);
    napi_value MacDoFinal(napi_env env, napi_callback_info info);
    napi_value GetMacLength(napi_env env, napi_callback_info info);

    HcfMac *GetMac()
    {
        return macObj_;
    }

private:
    HcfMac *macObj_ = nullptr;
};
} // namespace CryptoFramework
} // namespace OHOS

#endif // NAPI_X509_CERTIFICATE_H
