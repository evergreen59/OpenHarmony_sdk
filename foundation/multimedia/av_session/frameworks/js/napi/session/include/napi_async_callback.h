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

#ifndef OHOS_NAPI_UV_QUEUE_H
#define OHOS_NAPI_UV_QUEUE_H

#include <functional>
#include "avsession_log.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "uv.h"

namespace OHOS::AVSession {
class NapiAsyncCallback final {
public:
    using NapiArgsGetter = std::function<void(napi_env env, int& argc, napi_value* argv)>;

    explicit NapiAsyncCallback(napi_env env);
    ~NapiAsyncCallback();

    napi_env GetEnv() const;

    void Call(napi_ref method, NapiArgsGetter getter = NapiArgsGetter());

private:
    static void AfterWorkCallback(uv_work_t* work, int aStatus);

    struct DataContext {
        napi_env env;
        napi_ref method;
        NapiArgsGetter getter;
    };
    napi_env env_ = nullptr;
    uv_loop_s* loop_ = nullptr;

    static constexpr size_t ARGC_MAX = 6;
};
} // namespace OHOS::AVSession
#endif