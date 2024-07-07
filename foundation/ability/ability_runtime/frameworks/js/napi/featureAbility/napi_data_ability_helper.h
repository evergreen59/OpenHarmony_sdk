/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ABILITY_RUNTIME_NAPI_DATA_ABILITY_HELPER_H
#define OHOS_ABILITY_RUNTIME_NAPI_DATA_ABILITY_HELPER_H
#include "data_ability_observer_stub.h"
#include "feature_ability_common.h"

namespace OHOS {
namespace AppExecFwk {
class NAPIDataAbilityObserver : public AAFwk::DataAbilityObserverStub {
public:
    void OnChange() override;
    void SetEnv(const napi_env &env);
    void SetCallbackRef(const napi_ref &ref);
    void ReleaseJSCallback();

    void CallJsMethod();

private:
    void SafeReleaseJSCallback();

    napi_env env_ = nullptr;
    napi_ref ref_ = nullptr;
    bool isCallingback_ = false;
    bool needRelease_ = false;
    std::mutex mutex_;
};

class NAPIDataAbilityHelperWrapper {
public:
    explicit NAPIDataAbilityHelperWrapper(std::weak_ptr<DataAbilityHelper>&& dataAbilityHelper)
        : dataAbilityHelper_(dataAbilityHelper) {}
    inline std::shared_ptr<DataAbilityHelper> GetDataAbilityHelper() const
    {
        return dataAbilityHelper_.lock();
    }

private:
    std::weak_ptr<DataAbilityHelper> dataAbilityHelper_;
};

/**
 * @brief DataAbilityHelper NAPI module registration.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param exports An empty object via the exports parameter as a convenience.
 *
 * @return The return value from Init is treated as the exports object for the module.
 */
napi_value DataAbilityHelperInit(napi_env env, napi_value exports);
napi_value DataAbilityHelperConstructor(napi_env env, napi_callback_info info);

/**
 * @brief DataAbilityHelper NAPI method : insert.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param info The callback info passed into the callback function.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value NAPI_Insert(napi_env env, napi_callback_info info);

/**
 * @brief Insert processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param insertCB Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value InsertWrap(napi_env env, napi_callback_info info, DAHelperInsertCB *insertCB);

/**
 * @brief Insert Async.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param args Indicates the arguments passed into the callback.
 * @param argcPromise Asynchronous data processing.
 * @param insertCB Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value InsertAsync(napi_env env, napi_value *args, const size_t argCallback, DAHelperInsertCB *insertCB);

/**
 * @brief Insert Promise.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param insertCB Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value InsertPromise(napi_env env, DAHelperInsertCB *insertCB);

/**
 * @brief Insert asynchronous processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void InsertExecuteCB(napi_env env, void *data);

/**
 * @brief The callback at the end of the asynchronous callback.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void InsertAsyncCompleteCB(napi_env env, napi_status status, void *data);

/**
 * @brief The callback at the end of the Promise callback.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void InsertPromiseCompleteCB(napi_env env, napi_status status, void *data);

/**
 * @brief DataAbilityHelper NAPI method : notifyChange.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param info The callback info passed into the callback function.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value NAPI_NotifyChange(napi_env env, napi_callback_info info);

/**
 * @brief NotifyChange processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param notifyChangeCB Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value NotifyChangeWrap(napi_env env, napi_callback_info info, DAHelperNotifyChangeCB *notifyChangeCB);

/**
 * @brief NotifyChange Async.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param args Indicates the arguments passed into the callback.
 * @param argcPromise Asynchronous data processing.
 * @param notifyChangeCB Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value NotifyChangeAsync(
    napi_env env, napi_value *args, size_t argcAsync, const size_t argcPromise, DAHelperNotifyChangeCB *notifyChangeCB);

/**
 * @brief NotifyChange Promise.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param notifyChangeCB Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value NotifyChangePromise(napi_env env, DAHelperNotifyChangeCB *notifyChangeCB);

/**
 * @brief NotifyChange asynchronous processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void NotifyChangeExecuteCB(napi_env env, void *data);

/**
 * @brief The callback at the end of the asynchronous callback.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void NotifyChangeAsyncCompleteCB(napi_env env, napi_status status, void *data);

/**
 * @brief The callback at the end of the Promise callback.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void NotifyChangePromiseCompleteCB(napi_env env, napi_status status, void *data);

/**
 * @brief DataAbilityHelper NAPI method : on.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param info The callback info passed into the callback function.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value NAPI_Register(napi_env env, napi_callback_info info);

/**
 * @brief On processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param onCB Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value RegisterWrap(napi_env env, napi_callback_info info, DAHelperOnOffCB *onCB);

/**
 * @brief On Async.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param args Indicates the arguments passed into the callback.
 * @param argcPromise Asynchronous data processing.
 * @param onCB Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value RegisterAsync(
    napi_env env, napi_value *args, size_t argcAsync, const size_t argcPromise, DAHelperOnOffCB *onCB);

/**
 * @brief On asynchronous processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void RegisterExecuteCB(napi_env env, void *data);
void RegisterCompleteCB(napi_env env, napi_status status, void *data);

/**
 * @brief DataAbilityHelper NAPI method : off.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param info The callback info passed into the callback function.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value NAPI_UnRegister(napi_env env, napi_callback_info info);

/**
 * @brief Off processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param offCB Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value UnRegisterWrap(napi_env env, napi_callback_info info, DAHelperOnOffCB *offCB);

/**
 * @brief Off Async.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param offCB Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value UnRegisterSync(napi_env env, DAHelperOnOffCB *offCB);

/**
 * @brief Off asynchronous processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void UnRegisterExecuteCB(napi_env env, void *data);
void UnRegisterCompleteCB(napi_env env, napi_status status, void *data);
void FindRegisterObs(napi_env env, DAHelperOnOffCB *data);
/**
 * @brief Parse the ValuesBucket parameters.
 *
 * @param param Indicates the want parameters saved the parse result.
 * @param env The environment that the Node-API call is invoked under.
 * @param args Indicates the arguments passed into the callback.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value UnwrapValuesBucket(std::string &value, napi_env env, napi_value args);

napi_value NAPI_GetType(napi_env env, napi_callback_info info);
napi_value NAPI_GetType(napi_env env, napi_callback_info info);
napi_value GetTypeWrap(napi_env env, napi_callback_info info, DAHelperGetTypeCB *gettypeCB);
napi_value GetTypeAsync(napi_env env, napi_value *args, const size_t argCallback, DAHelperGetTypeCB *gettypeCB);
napi_value GetTypePromise(napi_env env, DAHelperGetTypeCB *gettypeCB);
void GetTypeExecuteCB(napi_env env, void *data);
void GetTypeAsyncCompleteCB(napi_env env, napi_status status, void *data);
void GetTypePromiseCompleteCB(napi_env env, napi_status status, void *data);

napi_value NAPI_GetFileTypes(napi_env env, napi_callback_info info);
napi_value NAPI_GetFileTypes(napi_env env, napi_callback_info info);
napi_value GetFileTypesWrap(napi_env env, napi_callback_info info, DAHelperGetFileTypesCB *getfiletypesCB);
napi_value GetFileTypesAsync(
    napi_env env, napi_value *args, const size_t argCallback, DAHelperGetFileTypesCB *getfiletypesCB);
napi_value GetFileTypesPromise(napi_env env, DAHelperGetFileTypesCB *getfiletypesCB);
void GetFileTypesExecuteCB(napi_env env, void *data);
void GetFileTypesAsyncCompleteCB(napi_env env, napi_status status, void *data);
void GetFileTypesPromiseCompleteCB(napi_env env, napi_status status, void *data);
napi_value WrapGetFileTypesCB(napi_env env, const DAHelperGetFileTypesCB &getfiletypesCB);

napi_value NAPI_NormalizeUri(napi_env env, napi_callback_info info);
napi_value NAPI_NormalizeUri(napi_env env, napi_callback_info info);
napi_value NormalizeUriWrap(napi_env env, napi_callback_info info, DAHelperNormalizeUriCB *normalizeuriCB);
napi_value NormalizeUriAsync(
    napi_env env, napi_value *args, const size_t argCallback, DAHelperNormalizeUriCB *normalizeuriCB);
napi_value NormalizeUriPromise(napi_env env, DAHelperNormalizeUriCB *normalizeuriCB);
void NormalizeUriExecuteCB(napi_env env, void *data);
void NormalizeUriAsyncCompleteCB(napi_env env, napi_status status, void *data);
void NormalizeUriPromiseCompleteCB(napi_env env, napi_status status, void *data);

napi_value NAPI_DenormalizeUri(napi_env env, napi_callback_info info);
napi_value NAPI_DenormalizeUri(napi_env env, napi_callback_info info);
napi_value DenormalizeUriWrap(napi_env env, napi_callback_info info, DAHelperDenormalizeUriCB *denormalizeuriCB);
napi_value DenormalizeUriAsync(
    napi_env env, napi_value *args, const size_t argCallback, DAHelperDenormalizeUriCB *denormalizeuriCB);
napi_value DenormalizeUriPromise(napi_env env, DAHelperDenormalizeUriCB *denormalizeuriCB);
void DenormalizeUriExecuteCB(napi_env env, void *data);
void DenormalizeUriAsyncCompleteCB(napi_env env, napi_status status, void *data);
void DenormalizeUriPromiseCompleteCB(napi_env env, napi_status status, void *data);

napi_value NAPI_Delete(napi_env env, napi_callback_info info);

napi_value DeleteWrap(napi_env env, napi_callback_info info, DAHelperDeleteCB *deleteCB);
napi_value DeleteAsync(napi_env env, napi_value *args, const size_t argCallback, DAHelperDeleteCB *deleteCB);

napi_value DeletePromise(napi_env env, DAHelperDeleteCB *deleteCB);

void DeleteExecuteCB(napi_env env, void *data);

void DeleteAsyncCompleteCB(napi_env env, napi_status status, void *data);

void DeletePromiseCompleteCB(napi_env env, napi_status status, void *data);

napi_value NAPI_Update(napi_env env, napi_callback_info info);

napi_value UpdateWrap(napi_env env, napi_callback_info info, DAHelperUpdateCB *updateCB);
napi_value UpdateAsync(napi_env env, napi_value *args, const size_t argCallback, DAHelperUpdateCB *updateCB);

napi_value UpdatePromise(napi_env env, DAHelperUpdateCB *updateCB);

void UpdateExecuteCB(napi_env env, void *data);

void UpdateAsyncCompleteCB(napi_env env, napi_status status, void *data);

void UpdatePromiseCompleteCB(napi_env env, napi_status status, void *data);

napi_value NAPI_Call(napi_env env, napi_callback_info info);

napi_value NAPI_OpenFile(napi_env env, napi_callback_info info);

napi_value OpenFileWrap(napi_env env, napi_callback_info info, DAHelperOpenFileCB *openFileCB);
napi_value OpenFileAsync(napi_env env, napi_value *args, const size_t argCallback, DAHelperOpenFileCB *openFileCB);

napi_value OpenFilePromise(napi_env env, DAHelperOpenFileCB *openFileCB);

void OpenFileExecuteCB(napi_env env, void *data);

void OpenFileAsyncCompleteCB(napi_env env, napi_status status, void *data);

void OpenFilePromiseCompleteCB(napi_env env, napi_status status, void *data);

napi_value NAPI_BatchInsert(napi_env env, napi_callback_info info);

napi_value BatchInsertWrap(napi_env env, napi_callback_info info, DAHelperBatchInsertCB *batchInsertCB);
napi_value BatchInsertAsync(
    napi_env env, napi_value *args, const size_t argCallback, DAHelperBatchInsertCB *batchInsertCB);

napi_value BatchInsertPromise(napi_env env, DAHelperBatchInsertCB *batchInsertCB);

void BatchInsertExecuteCB(napi_env env, void *data);

void BatchInsertAsyncCompleteCB(napi_env env, napi_status status, void *data);

void BatchInsertPromiseCompleteCB(napi_env env, napi_status status, void *data);

std::vector<NativeRdb::ValuesBucket> NapiValueObject(napi_env env, napi_value param);

bool UnwrapArrayObjectFromJS(napi_env env, napi_value param, std::vector<NativeRdb::ValuesBucket> &value);

napi_value NAPI_Query(napi_env env, napi_callback_info info);

napi_value QueryWrap(napi_env env, napi_callback_info info, DAHelperQueryCB *queryCB);

napi_value QuerySync(napi_env env, napi_value *args, const size_t argCallback, DAHelperQueryCB *queryCB);

napi_value QueryPromise(napi_env env, DAHelperQueryCB *queryCB);

napi_value WrapResultSet(napi_env env, const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet);

void AnalysisValuesBucket(NativeRdb::ValuesBucket &valuesBucket, const napi_env &env, const napi_value &arg);
void SetValuesBucketObject(
    NativeRdb::ValuesBucket &valuesBucket, const napi_env &env, std::string keyStr, napi_value value);

void UnwrapDataAbilityPredicates(NativeRdb::DataAbilityPredicates &predicates, napi_env env, napi_value value);

/**
 * @brief DataAbilityHelper NAPI method : executeBatch.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param info The callback info passed into the callback function.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value NAPI_ExecuteBatch(napi_env env, napi_callback_info info);

/**
 * @brief ExecuteBatch processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param executeBatchCB Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value ExecuteBatchWrap(napi_env env, napi_callback_info info, DAHelperExecuteBatchCB *executeBatchCB);

bool UnwrapArrayOperationFromJS(
    napi_env env, napi_value param, std::vector<std::shared_ptr<DataAbilityOperation>> &result);
/**
 * @brief ExecuteBatch Async.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param args Indicates the arguments passed into the callback.
 * @param argcPromise Asynchronous data processing.
 * @param executeBatchCB Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value ExecuteBatchAsync(
    napi_env env, napi_value *args, size_t argcAsync, const size_t argcPromise, DAHelperExecuteBatchCB *executeBatchCB);

/**
 * @brief ExecuteBatch Promise.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param executeBatchCB Process data asynchronously.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value ExecuteBatchPromise(napi_env env, DAHelperExecuteBatchCB *executeBatchCB);

/**
 * @brief ExecuteBatch asynchronous processing function.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void ExecuteBatchExecuteCB(napi_env env, void *data);

/**
 * @brief The callback at the end of the asynchronous callback.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void ExecuteBatchAsyncCompleteCB(napi_env env, napi_status status, void *data);

/**
 * @brief The callback at the end of the Promise callback.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void ExecuteBatchPromiseCompleteCB(napi_env env, napi_status status, void *data);

void GetDataAbilityResultForResult(
    napi_env env, const std::vector<std::shared_ptr<DataAbilityResult>> &dataAbilityResult, napi_value result);

void GetDataAbilityHelper(napi_env env, napi_value thisVar, std::shared_ptr<DataAbilityHelper>& dataAbilityHelper);
void DeleteDAHelperOnOffCB(DAHelperOnOffCB *onCB);
bool NeedErase(std::vector<DAHelperOnOffCB*>::iterator& iter,
    const std::shared_ptr<DataAbilityHelper>&& dataAbilityHelper);
void EraseMemberProperties(DAHelperOnOffCB* onCB);
}  // namespace AppExecFwk
}  // namespace OHOS
#endif /* OHOS_ABILITY_RUNTIME_NAPI_DATA_ABILITY_HELPER_H */
