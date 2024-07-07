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

#ifndef DATASHARE_UV_QUEUE_H
#define DATASHARE_UV_QUEUE_H

#include <functional>
#include <condition_variable>
#include <mutex>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "uv.h"

namespace OHOS {
namespace DataShare {
class DataShareUvQueue {
    using NapiVoidFunc = std::function<void()>;
    using NapiBoolFunc = std::function<bool()>;

public:
    explicit DataShareUvQueue(napi_env env);
    virtual ~DataShareUvQueue() = default;

    void SyncCall(NapiVoidFunc func = NapiVoidFunc(), NapiBoolFunc retFunc = NapiBoolFunc());

    void CheckFuncAndExec(NapiBoolFunc retFunc);

private:
    struct UvEntry {
        napi_env env;
        NapiVoidFunc func;
        bool done;
        bool purge;
        std::condition_variable condition;
        std::mutex mutex;
        NapiBoolFunc retFunc;
    };

    static void Purge(uv_work_t* work);

    napi_env env_ = nullptr;
    uv_loop_s* loop_ = nullptr;
};
} // namespace DataShare
} // namespace OHOS
#endif // DATASHARE_UV_QUEUE_H