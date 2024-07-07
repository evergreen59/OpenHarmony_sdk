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

#include "js_ability_manager.h"

#include <cstdint>

#include "ability_business_error.h"
#include "ability_manager_client.h"
#include "app_mgr_interface.h"
#include "errors.h"
#include "hilog_wrapper.h"
#include "js_error_utils.h"
#include "js_runtime.h"
#include "js_runtime_utils.h"
#include "napi/native_api.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "js_ability_manager_utils.h"
#include "event_runner.h"
#include "napi_common_configuration.h"

namespace OHOS {
namespace AbilityRuntime {
using AbilityManagerClient = AAFwk::AbilityManagerClient;
namespace {
OHOS::sptr<OHOS::AppExecFwk::IAppMgr> GetAppManagerInstance()
{
    OHOS::sptr<OHOS::ISystemAbilityManager> systemAbilityManager =
        OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::sptr<OHOS::IRemoteObject> appObject = systemAbilityManager->GetSystemAbility(OHOS::APP_MGR_SERVICE_ID);
    return OHOS::iface_cast<OHOS::AppExecFwk::IAppMgr>(appObject);
}

class JsAbilityManager final {
public:
    JsAbilityManager() = default;
    ~JsAbilityManager() = default;

    static void Finalizer(NativeEngine* engine, void* data, void* hint)
    {
        HILOG_INFO("JsAbilityManager::Finalizer is called");
        std::unique_ptr<JsAbilityManager>(static_cast<JsAbilityManager*>(data));
    }

    static NativeValue* GetAbilityRunningInfos(NativeEngine* engine, NativeCallbackInfo* info)
    {
        JsAbilityManager* me = CheckParamsAndGetThis<JsAbilityManager>(engine, info);
        return (me != nullptr) ? me->OnGetAbilityRunningInfos(*engine, *info) : nullptr;
    }

    static NativeValue* GetExtensionRunningInfos(NativeEngine* engine, NativeCallbackInfo* info)
    {
        JsAbilityManager* me = CheckParamsAndGetThis<JsAbilityManager>(engine, info);
        return (me != nullptr) ? me->OnGetExtensionRunningInfos(*engine, *info) : nullptr;
    }

    static NativeValue* UpdateConfiguration(NativeEngine* engine, NativeCallbackInfo* info)
    {
        JsAbilityManager* me = CheckParamsAndGetThis<JsAbilityManager>(engine, info);
        return (me != nullptr) ? me->OnUpdateConfiguration(*engine, *info) : nullptr;
    }

    static NativeValue* GetTopAbility(NativeEngine* engine, NativeCallbackInfo* info)
    {
        JsAbilityManager* me = CheckParamsAndGetThis<JsAbilityManager>(engine, info);
        return (me != nullptr) ? me->OnGetTopAbility(*engine, *info) : nullptr;
    }

private:
    NativeValue* OnGetAbilityRunningInfos(NativeEngine &engine, const NativeCallbackInfo &info)
    {
        HILOG_INFO("%{public}s is called", __FUNCTION__);
        AsyncTask::CompleteCallback complete =
            [](NativeEngine &engine, AsyncTask &task, int32_t status) {
                std::vector<AAFwk::AbilityRunningInfo> infos;
                auto errcode = AbilityManagerClient::GetInstance()->GetAbilityRunningInfos(infos);
                if (errcode == 0) {
#ifdef ENABLE_ERRCODE
                    task.ResolveWithNoError(engine, CreateJsAbilityRunningInfoArray(engine, infos));
                } else {
                    task.Reject(engine, CreateJsError(engine, GetJsErrorCodeByNativeError(errcode)));
#else
                    task.Resolve(engine, CreateJsAbilityRunningInfoArray(engine, infos));
                } else {
                    task.Reject(engine, CreateJsError(engine, errcode, "Get mission infos failed."));
#endif
                }
            };

        NativeValue* lastParam = (info.argc == 0) ? nullptr : info.argv[0];
        NativeValue* result = nullptr;
        AsyncTask::Schedule("JsAbilityManager::OnGetAbilityRunningInfos",
            engine, CreateAsyncTaskWithLastParam(engine,
            lastParam, nullptr, std::move(complete), &result));
        return result;
    }

    NativeValue* OnGetExtensionRunningInfos(NativeEngine &engine, NativeCallbackInfo &info)
    {
        HILOG_INFO("%{public}s is called", __FUNCTION__);
        if (info.argc == 0) {
            HILOG_ERROR("Not enough params");
#ifdef ENABLE_ERRCODE
            ThrowTooFewParametersError(engine);
#endif
            return engine.CreateUndefined();
        }
        int upperLimit = -1;
        if (!ConvertFromJsValue(engine, info.argv[0], upperLimit)) {
            HILOG_ERROR("Parse missionId failed");
#ifdef ENABLE_ERRCODE
            ThrowError(engine, AbilityErrorCode::ERROR_CODE_INVALID_PARAM);
#endif
            return engine.CreateUndefined();
        }

        AsyncTask::CompleteCallback complete =
            [upperLimit](NativeEngine &engine, AsyncTask &task, int32_t status) {
                std::vector<AAFwk::ExtensionRunningInfo> infos;
                auto errcode = AbilityManagerClient::GetInstance()->GetExtensionRunningInfos(upperLimit, infos);
                if (errcode == 0) {
#ifdef ENABLE_ERRCODE
                    task.ResolveWithNoError(engine, CreateJsExtensionRunningInfoArray(engine, infos));
                } else {
                    task.Reject(engine, CreateJsError(engine, GetJsErrorCodeByNativeError(errcode)));
#else
                    task.Resolve(engine, CreateJsExtensionRunningInfoArray(engine, infos));
                } else {
                    task.Reject(engine, CreateJsError(engine, errcode, "Get mission infos failed."));
#endif
                }
            };

        NativeValue* lastParam = (info.argc == 1) ? nullptr : info.argv[1];
        NativeValue* result = nullptr;
        AsyncTask::Schedule("JsAbilityManager::OnGetExtensionRunningInfos",
            engine, CreateAsyncTaskWithLastParam(engine,
            lastParam, nullptr, std::move(complete), &result));
        return result;
    }

    NativeValue* OnUpdateConfiguration(NativeEngine &engine, NativeCallbackInfo &info)
    {
        HILOG_INFO("%{public}s is called", __FUNCTION__);
        AsyncTask::CompleteCallback complete;

        do {
            if (info.argc == 0) {
                HILOG_ERROR("Not enough params");
#ifdef ENABLE_ERRCODE
                ThrowTooFewParametersError(engine);
#else
                complete = [](NativeEngine& engine, AsyncTask& task, int32_t status) {
                    task.Reject(engine, CreateJsError(engine, ERR_INVALID_VALUE, "no enough params."));
                };
#endif
                break;
            }

            AppExecFwk::Configuration changeConfig;
            if (!UnwrapConfiguration(reinterpret_cast<napi_env>(&engine),
                reinterpret_cast<napi_value>(info.argv[0]), changeConfig)) {
#ifdef ENABLE_ERRCODE
                ThrowError(engine, AbilityErrorCode::ERROR_CODE_INVALID_PARAM);
#else
                complete = [](NativeEngine& engine, AsyncTask& task, int32_t status) {
                    task.Reject(engine, CreateJsError(engine, ERR_INVALID_VALUE, "config is invalid."));
                };
#endif
                break;
            }

            complete = [changeConfig](NativeEngine& engine, AsyncTask& task, int32_t status) {
                auto errcode = GetAppManagerInstance()->UpdateConfiguration(changeConfig);
                if (errcode == 0) {
#ifdef ENABLE_ERRCODE
                    task.ResolveWithNoError(engine, engine.CreateUndefined());
                } else {
                    task.Reject(engine, CreateJsError(engine, GetJsErrorCodeByNativeError(errcode)));
#else
                    task.Resolve(engine, engine.CreateUndefined());
                } else {
                    task.Reject(engine, CreateJsError(engine, errcode, "update config failed."));
#endif
                }
            };
        } while (0);

        NativeValue* lastParam = (info.argc == 1) ? nullptr : info.argv[1];
        NativeValue* result = nullptr;
        AsyncTask::Schedule("JsAbilityManager::OnGetExtensionRunningInfos",
            engine, CreateAsyncTaskWithLastParam(engine,
            lastParam, nullptr, std::move(complete), &result));
        return result;
    }

    NativeValue* OnGetTopAbility(NativeEngine &engine, const NativeCallbackInfo &info)
    {
        HILOG_INFO("%{public}s is called", __FUNCTION__);
        AsyncTask::CompleteCallback complete =
            [](NativeEngine &engine, AsyncTask &task, int32_t status) {
                AppExecFwk::ElementName elementName = AbilityManagerClient::GetInstance()->GetTopAbility();
#ifdef ENABLE_ERRCOE
                task.ResolveWithNoError(engine, CreateJsElementName(engine, elementName));
#else
                task.Resolve(engine, CreateJsElementName(engine, elementName));
#endif
            };

        NativeValue* lastParam = (info.argc == 0) ? nullptr : info.argv[0];
        NativeValue* result = nullptr;
        AsyncTask::Schedule("JsAbilityManager::OnGetTopAbility",
            engine, CreateAsyncTaskWithLastParam(engine,
            lastParam, nullptr, std::move(complete), &result));
        return result;
    }
};
} // namespace

NativeValue* JsAbilityManagerInit(NativeEngine* engine, NativeValue* exportObj)
{
    HILOG_INFO("JsAbilityManagerInit is called");

    if (engine == nullptr || exportObj == nullptr) {
        HILOG_INFO("engine or exportObj null");
        return nullptr;
    }

    NativeObject* object = ConvertNativeValueTo<NativeObject>(exportObj);
    if (object == nullptr) {
        HILOG_INFO("object null");
        return nullptr;
    }

    std::unique_ptr<JsAbilityManager> jsAbilityManager = std::make_unique<JsAbilityManager>();
    object->SetNativePointer(jsAbilityManager.release(), JsAbilityManager::Finalizer, nullptr);

    object->SetProperty("AbilityState", AbilityStateInit(engine));

    HILOG_INFO("JsAbilityManagerInit BindNativeFunction called");
    const char *moduleName = "JsAbilityManager";
    BindNativeFunction(*engine, *object, "getAbilityRunningInfos", moduleName,
        JsAbilityManager::GetAbilityRunningInfos);
    BindNativeFunction(*engine, *object, "getExtensionRunningInfos", moduleName,
        JsAbilityManager::GetExtensionRunningInfos);
    BindNativeFunction(*engine, *object, "updateConfiguration", moduleName, JsAbilityManager::UpdateConfiguration);
    BindNativeFunction(*engine, *object, "getTopAbility", moduleName, JsAbilityManager::GetTopAbility);
    HILOG_INFO("JsAbilityManagerInit end");
    return engine->CreateUndefined();
}
}  // namespace AbilityRuntime
}  // namespace OHOS
