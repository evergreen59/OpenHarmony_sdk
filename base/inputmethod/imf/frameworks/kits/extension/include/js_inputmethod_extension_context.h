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

#ifndef ABILITY_RUNTIME_JS_INPUTMETHOD_EXTENSION_CONTEXT_H
#define ABILITY_RUNTIME_JS_INPUTMETHOD_EXTENSION_CONTEXT_H

#include <memory>

#include "ability_connect_callback.h"
#include "event_handler.h"
#include "inputmethod_extension_context.h"
#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"

namespace OHOS {
namespace AbilityRuntime {
NativeValue *CreateJsInputMethodExtensionContext(
    NativeEngine &engine, std::shared_ptr<InputMethodExtensionContext> context);

class JSInputMethodExtensionConnection : public AbilityConnectCallback {
public:
    explicit JSInputMethodExtensionConnection(NativeEngine &engine);
    ~JSInputMethodExtensionConnection();
    void OnAbilityConnectDone(
        const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode) override;
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override;
    void HandleOnAbilityConnectDone(
        const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode);
    void HandleOnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode);
    void SetJsConnectionObject(NativeValue *jsConnectionObject);
    void CallJsFailed(int32_t errorCode);

private:
    NativeEngine &engine_;
    std::unique_ptr<NativeReference> jsConnectionObject_ = nullptr;
};

struct ConnecttionKey {
    AAFwk::Want want;
    int64_t id;
};

struct key_compare {
    bool operator()(const ConnecttionKey &key1, const ConnecttionKey &key2) const
    {
        return key1.id < key2.id;
    }
};

static std::map<ConnecttionKey, sptr<JSInputMethodExtensionConnection>, key_compare> connects_;
static int64_t serialNumber_ = 0;
static std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
} // namespace AbilityRuntime
} // namespace OHOS
#endif // ABILITY_RUNTIME_JS_INPUTMETHOD_EXTENSION_CONTEXT_H
