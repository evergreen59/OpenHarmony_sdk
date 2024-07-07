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

#include "form_render_service_extension.h"

#include "ability_info.h"
#include "context_impl.h"
#include "form_render_impl.h"
#include "hitrace_meter.h"
#include "hilog_wrapper.h"
#include "service_extension_context.h"

namespace OHOS {
namespace AbilityRuntime {
using namespace OHOS::AppExecFwk::FormRender;

FormRenderServiceExtension* FormRenderServiceExtension::Create(const std::unique_ptr<Runtime>& runtime)
{
    return new (std::nothrow) FormRenderServiceExtension(static_cast<Runtime&>(*runtime));
}

FormRenderServiceExtension::FormRenderServiceExtension(Runtime& runtime) : runtime_(runtime) {}
FormRenderServiceExtension::~FormRenderServiceExtension() = default;

void FormRenderServiceExtension::Init(const std::shared_ptr<AbilityLocalRecord> &record,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    ServiceExtension::Init(record, application, handler, token);
}

void FormRenderServiceExtension::OnStart(const AAFwk::Want &want)
{
    Extension::OnStart(want);
    auto context = GetContext();
    if (context) {
        OHOS::DelayedSingleton<FormRenderImpl>::GetInstance()->SetConfiguration(context->GetConfiguration());
    }

    HILOG_INFO("FormRenderServiceExtension OnStart begin..");
}

void FormRenderServiceExtension::OnStop()
{
    ServiceExtension::OnStop();
    HILOG_INFO("FormRenderServiceExtension OnStop begin.");
}

sptr<IRemoteObject> FormRenderServiceExtension::OnConnect(const AAFwk::Want &want)
{
    HILOG_INFO("%{public}s begin.", __func__);
    return OHOS::DelayedSingleton<FormRenderImpl>::GetInstance()->AsObject();
}

sptr<IRemoteObject> FormRenderServiceExtension::OnConnect(const AAFwk::Want &want,
    AppExecFwk::AbilityTransactionCallbackInfo<sptr<IRemoteObject>> *callbackInfo, bool &isAsyncCallback)
{
    HILOG_INFO("%{public}s begin multiParams.", __func__);
    return OHOS::DelayedSingleton<FormRenderImpl>::GetInstance()->AsObject();
}

void FormRenderServiceExtension::OnDisconnect(const AAFwk::Want &want)
{
    HITRACE_METER_NAME(HITRACE_TAG_ABILITY_MANAGER, __PRETTY_FUNCTION__);
    Extension::OnDisconnect(want);
    HILOG_DEBUG("%{public}s begin.", __func__);
}

void FormRenderServiceExtension::OnDisconnect(const AAFwk::Want &want,
    AppExecFwk::AbilityTransactionCallbackInfo<> *callbackInfo, bool &isAsyncCallback)
{
    HITRACE_METER_NAME(HITRACE_TAG_ABILITY_MANAGER, __PRETTY_FUNCTION__);
    Extension::OnDisconnect(want);
    HILOG_DEBUG("%{public}s begin.", __func__);
}

void FormRenderServiceExtension::OnCommand(const AAFwk::Want &want, bool restart, int startId)
{
    Extension::OnCommand(want, restart, startId);
    HILOG_INFO("%{public}s begin restart=%{public}s,startId=%{public}d.",
        __func__,
        restart ? "true" : "false",
        startId);
}

void FormRenderServiceExtension::OnConfigurationUpdated(const AppExecFwk::Configuration& configuration)
{
    Extension::OnConfigurationUpdated(configuration);
    HILOG_INFO("%{public}s called.", __func__);
    auto config = std::make_shared<AppExecFwk::Configuration>(configuration);
    OHOS::DelayedSingleton<FormRenderImpl>::GetInstance()->OnConfigurationUpdated(config);
}
}
}
