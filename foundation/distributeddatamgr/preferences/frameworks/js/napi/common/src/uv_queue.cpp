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
#include "uv_queue.h"

#include <memory>

#include "js_logger.h"

namespace OHOS::PreferencesJsKit {
constexpr size_t MAX_CALLBACK_ARG_NUM = 6;
UvQueue::UvQueue(napi_env env)
    : env_(env)
{
    if (env != nullptr) {
        napi_get_uv_event_loop(env, &loop_);
    }
}

UvQueue::~UvQueue()
{
    LOG_DEBUG("no memory leak for queue-callback");
    env_ = nullptr;
}

void UvQueue::AsyncCall(NapiCallbackGetter getter, NapiArgsGenerator genArgs)
{
    if (loop_ == nullptr || !getter) {
        LOG_ERROR("loop_ or callback is nullptr");
        return;
    }

    uv_work_t* work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        LOG_ERROR("no memory for uv_work_t");
        return;
    }
    work->data = new UvEntry{ env_, getter, std::move(genArgs) };
    uv_queue_work(
        loop_, work, [](uv_work_t* work) {},
        [](uv_work_t* work, int uvstatus) {
            std::shared_ptr<UvEntry> entry(static_cast<UvEntry *>(work->data), [work](UvEntry *data) {
                delete data;
                delete work;
            });
            napi_handle_scope scope = nullptr;
            napi_open_handle_scope(entry->env, &scope);
            if (scope == nullptr) {
                return;
            }
            napi_value method = entry->callback(entry->env);
            if (method == nullptr) {
                LOG_ERROR("the callback is invalid, maybe is cleared!");
                napi_close_handle_scope(entry->env, scope);
                return ;
            }
            int argc = 0;
            napi_value argv[MAX_CALLBACK_ARG_NUM] = { nullptr };
            if (entry->args) {
                argc = MAX_CALLBACK_ARG_NUM;
                entry->args(entry->env, argc, argv);
            }
            LOG_DEBUG("queue uv_after_work_cb");
            napi_value global = nullptr;
            napi_get_global(entry->env, &global);
            napi_value result;
            napi_status status = napi_call_function(entry->env, global, method, argc, argv, &result);
            if (status != napi_ok) {
                LOG_ERROR("notify data change failed status:%{public}d.", status);
            }
            napi_close_handle_scope(entry->env, scope);
        });
}

napi_env UvQueue::GetEnv()
{
    return env_;
}
} // namespace OHOS::DistributedKVStore
