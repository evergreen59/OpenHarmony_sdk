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

#ifndef MODULETEST_OHOS_ABILITY_RUNTIME_MOCK_ABILITY_SCHEDULER_STUB_H
#define MODULETEST_OHOS_ABILITY_RUNTIME_MOCK_ABILITY_SCHEDULER_STUB_H

#include "ability_scheduler_stub.h"

namespace OHOS {
namespace AAFwk {
class MockAbilitySchedulerStub : public AbilitySchedulerStub {
public:
    MOCK_METHOD2(ScheduleAbilityTransaction, void(const Want &, const LifeCycleStateInfo &));
    MOCK_METHOD3(SendResult, void(int, int, const Want &));
    MOCK_METHOD1(ScheduleConnectAbility, void(const Want &));
    MOCK_METHOD1(ScheduleDisconnectAbility, void(const Want &));
    MOCK_METHOD3(ScheduleCommandAbility, void(const Want &, bool, int));
    MOCK_METHOD0(ScheduleSaveAbilityState, void());
    MOCK_METHOD1(ScheduleRestoreAbilityState, void(const PacMap &));
    MOCK_METHOD2(GetFileTypes, std::vector<std::string>(const Uri &, const std::string &));
    MOCK_METHOD2(OpenFile, int(const Uri &, const std::string &));
    MOCK_METHOD2(Insert, int(const Uri &, const NativeRdb::ValuesBucket &));
    MOCK_METHOD3(Update, int(const Uri &, const NativeRdb::ValuesBucket &, const NativeRdb::DataAbilityPredicates &));
    MOCK_METHOD2(Delete, int(const Uri &, const NativeRdb::DataAbilityPredicates &));
    MOCK_METHOD3(Query, std::shared_ptr<NativeRdb::AbsSharedResultSet>(
                            const Uri &, std::vector<std::string> &, const NativeRdb::DataAbilityPredicates &));
    MOCK_METHOD4(Call, std::shared_ptr<PacMap>(const Uri &, const std::string &, const std::string &, const PacMap &));
    MOCK_METHOD1(GetType, std::string(const Uri &uri));
    MOCK_METHOD2(OpenRawFile, int(const Uri &uri, const std::string &mode));
    MOCK_METHOD2(Reload, bool(const Uri &uri, const PacMap &extras));
    MOCK_METHOD2(BatchInsert, int(const Uri &uri, const std::vector<NativeRdb::ValuesBucket> &values));
    MOCK_METHOD1(NormalizeUri, Uri(const Uri &));
    MOCK_METHOD1(DenormalizeUri, Uri(const Uri &));
    MOCK_METHOD2(ScheduleRegisterObserver, bool(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver));
    MOCK_METHOD2(
        ScheduleUnregisterObserver, bool(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver));
    MOCK_METHOD1(ScheduleNotifyChange, bool(const Uri &uri));
    MOCK_METHOD1(ExecuteBatch, std::vector<std::shared_ptr<AppExecFwk::DataAbilityResult>>(
                                   const std::vector<std::shared_ptr<AppExecFwk::DataAbilityOperation>> &operations));
    MOCK_METHOD1(NotifyContinuationResult, void(int32_t result));
    MOCK_METHOD2(ContinueAbility, void(const std::string& deviceId, uint32_t versionCode));
    MOCK_METHOD2(DumpAbilityInfo, void(const std::vector<std::string> &params, std::vector<std::string> &info));

    virtual void CallRequest()
    {
        return;
    }

    #ifdef ABILITY_COMMAND_FOR_TEST
    virtual int BlockAbility() override
    {
        return 0;
    }
    #endif
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // MODULETEST_OHOS_ABILITY_RUNTIME_MOCK_ABILITY_SCHEDULER_STUB_H
