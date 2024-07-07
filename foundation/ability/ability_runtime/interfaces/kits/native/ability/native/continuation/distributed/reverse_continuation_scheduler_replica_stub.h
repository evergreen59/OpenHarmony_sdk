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

#ifndef OHOS_ABILITY_RUNTIME_REVERSE_CONTINUATION_SCHEDULER_REPLICA_STUB_H
#define OHOS_ABILITY_RUNTIME_REVERSE_CONTINUATION_SCHEDULER_REPLICA_STUB_H

#include "iremote_stub.h"
#include "event_handler.h"

#include "reverse_continuation_scheduler_replica_interface.h"

namespace OHOS {
namespace AppExecFwk {
class ReverseContinuationSchedulerReplicaStub : public IRemoteStub<IReverseContinuationSchedulerReplica> {
public:
    ReverseContinuationSchedulerReplicaStub();
    virtual ~ReverseContinuationSchedulerReplicaStub();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

    int32_t PassPrimaryInner(MessageParcel &data, MessageParcel &reply);

    int32_t ReverseContinuationInner(MessageParcel &data, MessageParcel &reply);

    int32_t NotifyReverseResultInner(MessageParcel &data, MessageParcel &reply);

private:
    using ReverseContinuationSchedulerReplicaFunc = int32_t (ReverseContinuationSchedulerReplicaStub::*)(
        MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, ReverseContinuationSchedulerReplicaFunc> continuationFuncMap_;

    DISALLOW_COPY_AND_MOVE(ReverseContinuationSchedulerReplicaStub);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif
