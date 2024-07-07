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

#include "inputmethod_extension.h"

#include "ability_loader.h"
#include "connection_manager.h"
#include "global.h"
#include "inputmethod_extension_context.h"
#include "js_inputmethod_extension.h"
#include "runtime.h"

namespace OHOS {
namespace AbilityRuntime {
using namespace OHOS::AppExecFwk;
InputMethodExtension *InputMethodExtension::Create(const std::unique_ptr<Runtime> &runtime)
{
    IMSA_HILOGI("InputMethodExtension::Create runtime");
    if (!runtime) {
        return new InputMethodExtension();
    }
    switch (runtime->GetLanguage()) {
        case Runtime::Language::JS:
            return JsInputMethodExtension::Create(runtime);
        default:
            return new InputMethodExtension();
    }
}

void InputMethodExtension::Init(const std::shared_ptr<AbilityLocalRecord> &record,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    IMSA_HILOGI("InputMethodExtension init context");
    ExtensionBase<InputMethodExtensionContext>::Init(record, application, handler, token);
}

std::shared_ptr<InputMethodExtensionContext> InputMethodExtension::CreateAndInitContext(
    const std::shared_ptr<AbilityLocalRecord> &record, const std::shared_ptr<OHOSApplication> &application,
    std::shared_ptr<AbilityHandler> &handler, const sptr<IRemoteObject> &token)
{
    IMSA_HILOGI("InputMethodExtension CreateAndInitContext");
    std::shared_ptr<InputMethodExtensionContext> context =
        ExtensionBase<InputMethodExtensionContext>::CreateAndInitContext(record, application, handler, token);
    if (context == nullptr) {
        IMSA_HILOGE("InputMethodExtension::CreateAndInitContext context is nullptr");
        return context;
    }
    return context;
}
} // namespace AbilityRuntime
} // namespace OHOS