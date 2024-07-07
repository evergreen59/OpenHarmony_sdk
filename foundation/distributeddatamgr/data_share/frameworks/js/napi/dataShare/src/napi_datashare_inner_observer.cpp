/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "napi_datashare_observer.h"

#include <memory>
#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
NAPIInnerObserver::NAPIInnerObserver(napi_env env, napi_value callback)
    : env_(env)
{
    napi_create_reference(env, callback, 1, &ref_);
    napi_get_uv_event_loop(env, &loop_);
}

void NAPIInnerObserver::OnComplete(uv_work_t *work, int status)
{
    LOG_DEBUG("uv_queue_work start");
    std::shared_ptr<ObserverWorker> innerWorker(reinterpret_cast<ObserverWorker *>(work->data));
    auto observer = innerWorker->observer_.lock();
    if (observer == nullptr || observer->ref_ == nullptr) {
        delete work;
        LOG_ERROR("innerWorker->observer_->ref_ is nullptr");
        return;
    }
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(observer->env_, &scope);
    if (scope == nullptr) {
        delete work;
        return;
    }
    napi_value callback = nullptr;
    napi_value args[2] = {0};
    napi_value global = nullptr;
    napi_value result;
    napi_get_reference_value(observer->env_, observer->ref_, &callback);
    napi_get_global(observer->env_, &global);
    napi_status callStatus = napi_call_function(observer->env_, global, callback, 2, args, &result);
    napi_close_handle_scope(observer->env_, scope);
    if (callStatus != napi_ok) {
        LOG_ERROR("napi_call_function failed status : %{public}d", callStatus);
    }
    delete work;
}

void NAPIInnerObserver::OnChange()
{
    LOG_DEBUG("NAPIInnerObserver Start");
    if (ref_ == nullptr) {
        LOG_ERROR("ref_ is nullptr");
        return;
    }
    ObserverWorker *observerWorker = new (std::nothrow)ObserverWorker(shared_from_this());
    if (observerWorker == nullptr) {
        LOG_ERROR("Failed to create observerWorker");
        return;
    }
    uv_work_t *work = new (std::nothrow)uv_work_t();
    if (work == nullptr) {
        delete observerWorker;
        LOG_ERROR("Failed to create uv work");
        return;
    }
    work->data = observerWorker;
    int ret = uv_queue_work(loop_, work, [](uv_work_t *work) {}, NAPIInnerObserver::OnComplete);
    if (ret != 0) {
        LOG_ERROR("uv_queue_work failed");
        delete observerWorker;
        delete work;
    }
}

void NAPIInnerObserver::DeleteReference()
{
    if (ref_ != nullptr) {
        napi_delete_reference(env_, ref_);
        ref_ = nullptr;
    }
}

napi_ref NAPIInnerObserver::GetCallback()
{
    return ref_;
}
}  // namespace DataShare
}  // namespace OHOS
