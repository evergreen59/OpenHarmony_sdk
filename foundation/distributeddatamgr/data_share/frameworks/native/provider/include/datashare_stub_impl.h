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

#ifndef DATASHARE_STUB_IMPL_H
#define DATASHARE_STUB_IMPL_H

#include <memory>
#include "datashare_stub.h"
#include "datashare_uv_queue.h"
#include "js_datashare_ext_ability.h"
#include "napi_remote_object.h"

namespace OHOS {
namespace DataShare {
using DataShare::JsDataShareExtAbility;
class DataShareStubImpl : public DataShareStub {
public:
    explicit DataShareStubImpl(const std::shared_ptr<JsDataShareExtAbility>& extension, napi_env env)
        : extension_(extension)
    {
        uvQueue_ = std::make_shared<DataShare::DataShareUvQueue>(env);
    }

    virtual ~DataShareStubImpl() {}

    std::vector<std::string> GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter) override;

    int OpenFile(const Uri &uri, const std::string &mode) override;

    int OpenRawFile(const Uri &uri, const std::string &mode) override;

    int Insert(const Uri &uri, const DataShareValuesBucket &value) override;

    int Update(const Uri &uri, const DataSharePredicates &predicates,
        const DataShareValuesBucket &value) override;

    int Delete(const Uri &uri, const DataSharePredicates &predicates) override;

    std::shared_ptr<DataShareResultSet> Query(const Uri &uri,
        const DataSharePredicates &predicates, std::vector<std::string> &columns) override;

    std::string GetType(const Uri &uri) override;

    int BatchInsert(const Uri &uri, const std::vector<DataShareValuesBucket> &values) override;

    bool RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    bool UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    bool NotifyChange(const Uri &uri) override;

    Uri NormalizeUri(const Uri &uri) override;

    Uri DenormalizeUri(const Uri &uri) override;

private:
    std::shared_ptr<JsDataShareExtAbility> GetOwner();
    bool CheckCallingPermission(const std::string &permission);
    void GetCallingInfo(CallingInfo& callingInfo);
    std::shared_ptr<JsDataShareExtAbility> extension_;
    std::shared_ptr<DataShare::DataShareUvQueue> uvQueue_;
    std::mutex mutex_;
};
} // namespace DataShare
} // namespace OHOS
#endif // DATASHARE_STUB_IMPL_H

