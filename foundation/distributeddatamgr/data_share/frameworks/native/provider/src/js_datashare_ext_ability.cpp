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

#include "js_datashare_ext_ability.h"

#include "ability_info.h"
#include "dataobs_mgr_client.h"
#include "datashare_stub_impl.h"
#include "datashare_log.h"
#include "js_datashare_ext_ability_context.h"
#include "js_runtime.h"
#include "js_runtime_utils.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_common_util.h"
#include "napi_common_want.h"
#include "napi_remote_object.h"

#include "napi_datashare_values_bucket.h"
#include "datashare_predicates_proxy.h"

namespace OHOS {
namespace DataShare {
using namespace AbilityRuntime;
namespace {
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
constexpr size_t ARGC_THREE = 3;
constexpr int INVALID_VALUE = -1;
const std::string ASYNC_CALLBACK_NAME = "AsyncCallback";
}

bool MakeNapiColumn(napi_env env, napi_value &napiColumns, const std::vector<std::string> &columns);

using namespace OHOS::AppExecFwk;
using DataObsMgrClient = OHOS::AAFwk::DataObsMgrClient;

JsDataShareExtAbility* JsDataShareExtAbility::Create(const std::unique_ptr<Runtime>& runtime)
{
    return new JsDataShareExtAbility(static_cast<JsRuntime&>(*runtime));
}

JsDataShareExtAbility::JsDataShareExtAbility(JsRuntime& jsRuntime) : jsRuntime_(jsRuntime) {}

JsDataShareExtAbility::~JsDataShareExtAbility()
{
    LOG_DEBUG("Js datashare extension destructor.");
    jsRuntime_.FreeNativeReference(std::move(jsObj_));
}

void JsDataShareExtAbility::Init(const std::shared_ptr<AbilityLocalRecord> &record,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    DataShareExtAbility::Init(record, application, handler, token);
    std::string srcPath = "";
    GetSrcPath(srcPath);
    if (srcPath.empty()) {
        LOG_ERROR("Failed to get srcPath");
        return;
    }

    std::string moduleName(Extension::abilityInfo_->moduleName);
    moduleName.append("::").append(abilityInfo_->name);
    LOG_INFO("module:%{public}s, srcPath:%{public}s.", moduleName.c_str(), srcPath.c_str());
    HandleScope handleScope(jsRuntime_);
    auto& engine = jsRuntime_.GetNativeEngine();

    jsObj_ = jsRuntime_.LoadModule(moduleName, srcPath, abilityInfo_->hapPath,
        abilityInfo_->compileMode == CompileMode::ES_MODULE);
    if (jsObj_ == nullptr) {
        LOG_ERROR("Failed to get jsObj_");
        return;
    }
    LOG_INFO("ConvertNativeValueTo.");
    NativeObject* obj = ConvertNativeValueTo<NativeObject>(jsObj_->Get());
    if (obj == nullptr) {
        LOG_ERROR("Failed to get JsDataShareExtAbility object");
        return;
    }

    auto context = GetContext();
    if (context == nullptr) {
        LOG_ERROR("Failed to get context");
        return;
    }
    LOG_INFO("CreateJsDataShareExtAbilityContext.");
    NativeValue* contextObj = CreateJsDataShareExtAbilityContext(engine, context);
    auto contextRef = jsRuntime_.LoadSystemModule("application.DataShareExtensionAbilityContext",
        &contextObj, ARGC_ONE);
    contextObj = contextRef->Get();
    LOG_INFO("Bind.");
    context->Bind(jsRuntime_, contextRef.release());
    LOG_INFO("SetProperty.");
    obj->SetProperty("context", contextObj);

    auto nativeObj = ConvertNativeValueTo<NativeObject>(contextObj);
    if (nativeObj == nullptr) {
        LOG_ERROR("Failed to get datashare extension ability native object");
        return;
    }

    LOG_INFO("Set datashare extension ability context pointer is nullptr: %{public}d", context.get() == nullptr);

    nativeObj->SetNativePointer(new std::weak_ptr<AbilityRuntime::Context>(context),
        [](NativeEngine*, void* data, void*) {
            LOG_INFO("Finalizer for weak_ptr datashare extension ability context is called");
            delete static_cast<std::weak_ptr<AbilityRuntime::Context>*>(data);
        }, nullptr);
}

void JsDataShareExtAbility::OnStart(const AAFwk::Want &want)
{
    Extension::OnStart(want);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return;
    }
    napi_value napiWant = OHOS::AppExecFwk::WrapWant(env, want);
    NativeValue* nativeWant = reinterpret_cast<NativeValue*>(napiWant);
    NativeValue* argv[] = {nativeWant};
    CallObjectMethod("onCreate", argv, ARGC_ONE);
    napi_close_handle_scope(env, scope);
}

sptr<IRemoteObject> JsDataShareExtAbility::OnConnect(const AAFwk::Want &want)
{
    Extension::OnConnect(want);
    sptr<DataShareStubImpl> remoteObject = new (std::nothrow) DataShareStubImpl(
        std::static_pointer_cast<JsDataShareExtAbility>(shared_from_this()),
        reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine()));
    if (remoteObject == nullptr) {
        LOG_ERROR("No memory allocated for DataShareStubImpl");
        return nullptr;
    }
    return remoteObject->AsObject();
}

void JsDataShareExtAbility::CheckAndSetAsyncResult(NativeEngine* engine)
{
    auto result = GetAsyncResult();
    auto type = result->TypeOf();
    if (type == NATIVE_NUMBER) {
        int32_t value = OHOS::AppExecFwk::UnwrapInt32FromJS(reinterpret_cast<napi_env>(engine),
            reinterpret_cast<napi_value>(result));
        SetResult(value);
    } else if (type == NATIVE_STRING) {
        std::string value = OHOS::AppExecFwk::UnwrapStringFromJS(reinterpret_cast<napi_env>(engine),
            reinterpret_cast<napi_value>(result));
        SetResult(value);
    } else if (type == NATIVE_OBJECT) {
        ResultSetBridge::Creator *proxy = nullptr;
        napi_unwrap(reinterpret_cast<napi_env>(engine), reinterpret_cast<napi_value>(result),
            reinterpret_cast<void **>(&proxy));
        if (proxy == nullptr) {
            std::vector<std::string> value;
            OHOS::AppExecFwk::UnwrapArrayStringFromJS(reinterpret_cast<napi_env>(engine),
                reinterpret_cast<napi_value>(result), value);
            SetResult(value);
        } else {
            std::shared_ptr<ResultSetBridge> value = proxy->Create();
            std::shared_ptr<DataShareResultSet> resultSet = std::make_shared<DataShareResultSet>(value);
            SetResult(resultSet);
        }
    } else {
        callbackResultNumber_ = -1;
        callbackResultString_ = "";
        callbackResultStringArr_ = {};
        callbackResultObject_ = nullptr;
    }
}

NativeValue* JsDataShareExtAbility::AsyncCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    LOG_INFO("engine == nullptr : %{public}d, info == nullptr : %{public}d.", engine == nullptr, info == nullptr);
    if (engine == nullptr || info == nullptr) {
        LOG_ERROR("invalid param.");
        return nullptr;
    }
    if (info->argc < ARGC_TWO || info->argv[0] == nullptr || info->argv[1] == nullptr) {
        LOG_ERROR("invalid args.");
        return engine->CreateUndefined();
    }

    int32_t value = -1;
    if ((info->argv[0])->TypeOf() == NATIVE_NUMBER) {
        value = OHOS::AppExecFwk::UnwrapInt32FromJS(reinterpret_cast<napi_env>(engine),
            reinterpret_cast<napi_value>(info->argv[0]));
        LOG_INFO("value_number : %{public}d.", value);
    }

    if (info->functionInfo == nullptr || info->functionInfo->data == nullptr) {
        LOG_ERROR("invalid object.");
        return engine->CreateUndefined();
    }

    JsDataShareExtAbility* instance = static_cast<JsDataShareExtAbility*>(info->functionInfo->data);
    if (instance != nullptr) {
        instance->SetBlockWaiting(true);
        instance->SetAsyncResult(info->argv[1]);
        instance->CheckAndSetAsyncResult(engine);
    }

    return engine->CreateUndefined();
}

NativeValue* JsDataShareExtAbility::CallObjectMethod(const char* name, NativeValue* const* argv, size_t argc,
    bool isAsync)
{
    LOG_INFO("JsDataShareExtAbility::CallObjectMethod(%{public}s), begin", name);

    if (!jsObj_) {
        LOG_WARN("Not found DataShareExtAbility.js");
        return nullptr;
    }

    HandleEscape handleEscape(jsRuntime_);
    auto& nativeEngine = jsRuntime_.GetNativeEngine();

    NativeValue* value = jsObj_->Get();
    NativeObject* obj = ConvertNativeValueTo<NativeObject>(value);
    if (obj == nullptr) {
        LOG_ERROR("Failed to get DataShareExtAbility object");
        return nullptr;
    }

    NativeValue* method = obj->GetProperty(name);
    if (method == nullptr) {
        LOG_ERROR("Failed to get '%{public}s' from DataShareExtAbility object", name);
        return nullptr;
    }

    size_t count = argc + 1;
    NativeValue **args = new (std::nothrow) NativeValue *[count];
    if (args == nullptr) {
        LOG_ERROR("JsDataShareExtAbility::CallObjectMethod new NativeValue error.");
        return nullptr;
    }
    for (size_t i = 0; i < argc; i++) {
        args[i] = argv[i];
    }

    if (isAsync) {
        callbackResultNumber_ = -1;
        callbackResultString_ = "";
        callbackResultStringArr_ = {};
        callbackResultObject_ = nullptr;
        args[argc] = nativeEngine.CreateFunction(ASYNC_CALLBACK_NAME.c_str(),
            ASYNC_CALLBACK_NAME.length(), JsDataShareExtAbility::AsyncCallback, this);
        LOG_INFO("AsyncType::ASYNC_COMMON.");
    } else {
        args[argc] = nullptr;
    }

    SetBlockWaiting(false);
    return handleEscape.Escape(nativeEngine.CallFunction(value, method, args, count));
}

void JsDataShareExtAbility::GetSrcPath(std::string &srcPath)
{
    if (!Extension::abilityInfo_->isStageBasedModel) {
        /* temporary compatibility api8 + config.json */
        srcPath.append(Extension::abilityInfo_->package);
        srcPath.append("/assets/js/");
        if (!Extension::abilityInfo_->srcPath.empty()) {
            srcPath.append(Extension::abilityInfo_->srcPath);
        }
        srcPath.append("/").append(Extension::abilityInfo_->name).append(".abc");
        return;
    }

    if (!Extension::abilityInfo_->srcEntrance.empty()) {
        srcPath.append(Extension::abilityInfo_->moduleName + "/");
        srcPath.append(Extension::abilityInfo_->srcEntrance);
        srcPath.erase(srcPath.rfind('.'));
        srcPath.append(".abc");
    }
}

std::vector<std::string> JsDataShareExtAbility::GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
{
    auto ret = DataShareExtAbility::GetFileTypes(uri, mimeTypeFilter);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }
    napi_value napiMimeTypeFilter = nullptr;
    status = napi_create_string_utf8(env, mimeTypeFilter.c_str(), NAPI_AUTO_LENGTH, &napiMimeTypeFilter);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }

    NativeValue* nativeUri = reinterpret_cast<NativeValue*>(napiUri);
    NativeValue* nativeMimeTypeFilter = reinterpret_cast<NativeValue*>(napiMimeTypeFilter);
    NativeValue* argv[] = {nativeUri, nativeMimeTypeFilter};
    CallObjectMethod("getFileTypes", argv, ARGC_TWO);
    napi_close_handle_scope(env, scope);
    return ret;
}

int JsDataShareExtAbility::OpenFile(const Uri &uri, const std::string &mode)
{
    auto ret = DataShareExtAbility::OpenFile(uri, mode);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }
    napi_value napiMode = nullptr;
    status = napi_create_string_utf8(env, mode.c_str(), NAPI_AUTO_LENGTH, &napiMode);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }

    NativeValue* nativeUri = reinterpret_cast<NativeValue*>(napiUri);
    NativeValue* nativeMode = reinterpret_cast<NativeValue*>(napiMode);
    NativeValue* argv[] = {nativeUri, nativeMode};
    CallObjectMethod("openFile", argv, ARGC_TWO);
    napi_close_handle_scope(env, scope);
    return ret;
}

int JsDataShareExtAbility::OpenRawFile(const Uri &uri, const std::string &mode)
{
    auto ret = DataShareExtAbility::OpenRawFile(uri, mode);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }
    napi_value napiMode = nullptr;
    status = napi_create_string_utf8(env, mode.c_str(), NAPI_AUTO_LENGTH, &napiMode);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }

    NativeValue* nativeUri = reinterpret_cast<NativeValue*>(napiUri);
    NativeValue* nativeMode = reinterpret_cast<NativeValue*>(napiMode);
    NativeValue* argv[] = {nativeUri, nativeMode};
    CallObjectMethod("openRawFile", argv, ARGC_TWO, false);
    napi_close_handle_scope(env, scope);
    return ret;
}

int JsDataShareExtAbility::Insert(const Uri &uri, const DataShareValuesBucket &value)
{
    int ret = INVALID_VALUE;
    ret = DataShareExtAbility::Insert(uri, value);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }
    napi_value napiValue = DataShareValueBucketNewInstance(env, const_cast<DataShareValuesBucket&>(value));
    if (napiValue == nullptr) {
        LOG_ERROR("failed to make new instance of rdbValueBucket.");
        napi_close_handle_scope(env, scope);
        return ret;
    }

    NativeValue* nativeUri = reinterpret_cast<NativeValue*>(napiUri);
    NativeValue* nativeValue = reinterpret_cast<NativeValue*>(napiValue);
    NativeValue* argv[] = {nativeUri, nativeValue};
    CallObjectMethod("insert", argv, ARGC_TWO);
    napi_close_handle_scope(env, scope);
    return ret;
}

int JsDataShareExtAbility::Update(const Uri &uri, const DataSharePredicates &predicates,
    const DataShareValuesBucket &value)
{
    int ret = INVALID_VALUE;
    ret = DataShareExtAbility::Update(uri, predicates, value);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }

    napi_value napiPredicates = MakePredicates(env, predicates);
    if (napiPredicates == nullptr) {
        napi_close_handle_scope(env, scope);
        return ret;
    }

    napi_value napiValue = DataShareValueBucketNewInstance(env, const_cast<DataShareValuesBucket&>(value));
    if (napiValue == nullptr) {
        LOG_ERROR("failed to make new instance of rdbValueBucket.");
        napi_close_handle_scope(env, scope);
        return ret;
    }

    NativeValue* nativeUri = reinterpret_cast<NativeValue*>(napiUri);
    NativeValue* nativePredicates = reinterpret_cast<NativeValue*>(napiPredicates);
    NativeValue* nativeValue = reinterpret_cast<NativeValue*>(napiValue);
    NativeValue* argv[] = {nativeUri, nativePredicates, nativeValue};
    CallObjectMethod("update", argv, ARGC_THREE);
    napi_close_handle_scope(env, scope);
    return ret;
}

int JsDataShareExtAbility::Delete(const Uri &uri, const DataSharePredicates &predicates)
{
    int ret = INVALID_VALUE;
    ret = DataShareExtAbility::Delete(uri, predicates);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }

    napi_value napiPredicates = MakePredicates(env, predicates);
    if (napiPredicates == nullptr) {
        napi_close_handle_scope(env, scope);
        return ret;
    }

    NativeValue* nativeUri = reinterpret_cast<NativeValue*>(napiUri);
    NativeValue* nativePredicates = reinterpret_cast<NativeValue*>(napiPredicates);
    NativeValue* argv[] = {nativeUri, nativePredicates};
    CallObjectMethod("delete", argv, ARGC_TWO);
    napi_close_handle_scope(env, scope);
    return ret;
}

std::shared_ptr<DataShareResultSet> JsDataShareExtAbility::Query(const Uri &uri,
    const DataSharePredicates &predicates, std::vector<std::string> &columns)
{
    std::shared_ptr<DataShareResultSet> ret;
    ret = DataShareExtAbility::Query(uri, predicates, columns);

    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }

    napi_value napiPredicates = MakePredicates(env, predicates);
    if (napiPredicates == nullptr) {
        napi_close_handle_scope(env, scope);
        return ret;
    }

    napi_value napiColumns = nullptr;
    if (!MakeNapiColumn(env, napiColumns, columns)) {
        LOG_ERROR("MakeNapiColumn failed");
        napi_close_handle_scope(env, scope);
        return ret;
    }

    NativeValue* nativeUri = reinterpret_cast<NativeValue*>(napiUri);
    NativeValue* nativePredicates = reinterpret_cast<NativeValue*>(napiPredicates);
    NativeValue* nativeColumns = reinterpret_cast<NativeValue*>(napiColumns);
    NativeValue* argv[] = {nativeUri, nativePredicates, nativeColumns};
    CallObjectMethod("query", argv, ARGC_THREE);
    napi_close_handle_scope(env, scope);
    return std::make_shared<DataShareResultSet>();
}

std::string JsDataShareExtAbility::GetType(const Uri &uri)
{
    auto ret = DataShareExtAbility::GetType(uri);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }
    NativeValue* nativeUri = reinterpret_cast<NativeValue*>(napiUri);
    NativeValue* argv[] = {nativeUri};
    CallObjectMethod("getType", argv, ARGC_ONE);
    napi_close_handle_scope(env, scope);
    return ret;
}

int JsDataShareExtAbility::BatchInsert(const Uri &uri, const std::vector<DataShareValuesBucket> &values)
{
    int ret = INVALID_VALUE;
    ret = DataShareExtAbility::BatchInsert(uri, values);

    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }

    napi_value napiValues = nullptr;
    status = napi_create_array(env, &napiValues);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_array status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }
    bool isArray = false;
    if (napi_is_array(env, napiValues, &isArray) != napi_ok || !isArray) {
        LOG_ERROR("JsDataShareExtAbility create array failed");
        napi_close_handle_scope(env, scope);
        return ret;
    }
    int32_t index = 0;
    for (const auto &value : values) {
        napi_value result = DataShareValueBucketNewInstance(env, const_cast<DataShareValuesBucket&>(value));
        if (result == nullptr) {
            LOG_ERROR("failed to make new instance of rdbValueBucket.");
            napi_close_handle_scope(env, scope);
            return ret;
        }
        napi_set_element(env, napiValues, index++, result);
    }

    NativeValue* nativeUri = reinterpret_cast<NativeValue*>(napiUri);
    NativeValue* nativeValues = reinterpret_cast<NativeValue*>(napiValues);
    NativeValue* argv[] = {nativeUri, nativeValues};
    CallObjectMethod("batchInsert", argv, ARGC_TWO);
    napi_close_handle_scope(env, scope);
    return ret;
}

bool JsDataShareExtAbility::RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    DataShareExtAbility::RegisterObserver(uri, dataObserver);
    auto obsMgrClient = DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        LOG_ERROR("obsMgrClient is nullptr");
        return false;
    }

    ErrCode ret = obsMgrClient->RegisterObserver(uri, dataObserver);
    if (ret != ERR_OK) {
        LOG_ERROR("obsMgrClient->RegisterObserver error return %{public}d", ret);
        return false;
    }
    return true;
}

bool JsDataShareExtAbility::UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    DataShareExtAbility::UnregisterObserver(uri, dataObserver);
    auto obsMgrClient = DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        LOG_ERROR("obsMgrClient is nullptr");
        return false;
    }

    ErrCode ret = obsMgrClient->UnregisterObserver(uri, dataObserver);
    if (ret != ERR_OK) {
        LOG_ERROR("obsMgrClient->UnregisterObserver error return %{public}d", ret);
        return false;
    }
    return true;
}

bool JsDataShareExtAbility::NotifyChange(const Uri &uri)
{
    DataShareExtAbility::NotifyChange(uri);
    auto obsMgrClient = DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        LOG_ERROR("obsMgrClient is nullptr");
        return false;
    }

    ErrCode ret = obsMgrClient->NotifyChange(uri);
    if (ret != ERR_OK) {
        LOG_ERROR("obsMgrClient->NotifyChange error return %{public}d", ret);
        return false;
    }
    return true;
}

Uri JsDataShareExtAbility::NormalizeUri(const Uri &uri)
{
    auto ret = DataShareExtAbility::NormalizeUri(uri);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }
    NativeValue* nativeUri = reinterpret_cast<NativeValue*>(napiUri);
    NativeValue* argv[] = {nativeUri};
    CallObjectMethod("normalizeUri", argv, ARGC_ONE);
    napi_close_handle_scope(env, scope);
    return ret;
}

Uri JsDataShareExtAbility::DenormalizeUri(const Uri &uri)
{
    auto ret = DataShareExtAbility::DenormalizeUri(uri);
    HandleScope handleScope(jsRuntime_);
    napi_env env = reinterpret_cast<napi_env>(&jsRuntime_.GetNativeEngine());
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }
    NativeValue* nativeUri = reinterpret_cast<NativeValue*>(napiUri);
    NativeValue* argv[] = {nativeUri};
    CallObjectMethod("denormalizeUri", argv, ARGC_ONE);
    napi_close_handle_scope(env, scope);
    return ret;
}

napi_value JsDataShareExtAbility::MakePredicates(napi_env env, const DataSharePredicates &predicates)
{
    std::shared_ptr<DataSharePredicates> predicatesPtr = std::make_shared<DataSharePredicates>(predicates);
    if (predicatesPtr == nullptr) {
        LOG_ERROR("No memory allocated for predicates");
        return nullptr;
    }
    napi_value napiPredicates = DataSharePredicatesProxy::NewInstance(env, predicatesPtr);
    if (napiPredicates == nullptr) {
        LOG_ERROR("failed to make new instance of DataSharePredicates.");
    }
    return napiPredicates;
}

bool MakeNapiColumn(napi_env env, napi_value &napiColumns, const std::vector<std::string> &columns)
{
    napi_status status = napi_create_array(env, &napiColumns);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_array status : %{public}d", status);
        return false;
    }

    bool isArray = false;
    if (napi_is_array(env, napiColumns, &isArray) != napi_ok || !isArray) {
        LOG_ERROR("JsDataShareExtAbility create array failed");
        return false;
    }

    int32_t index = 0;
    for (const auto &column : columns) {
        napi_value result = nullptr;
        napi_create_string_utf8(env, column.c_str(), column.length(), &result);
        napi_set_element(env, napiColumns, index++, result);
    }

    return true;
}
} // namespace DataShare
} // namespace OHOS