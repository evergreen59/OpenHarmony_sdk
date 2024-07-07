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
#include "session_listener_stub.h"
#include "avsession_log.h"
#include "avsession_errors.h"
#include "avsession_trace.h"

namespace OHOS::AVSession {
bool SessionListenerStub::CheckInterfaceToken(MessageParcel& data)
{
    auto localDescriptor = ISessionListener::GetDescriptor();
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (remoteDescriptor != localDescriptor) {
        SLOGI("interface token is not equal");
        return false;
    }
    return true;
}

int32_t SessionListenerStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
                                             MessageOption& option)
{
    if (!CheckInterfaceToken(data)) {
        return AVSESSION_ERROR;
    }
    if (code < LISTENER_CMD_MAX) {
        return (this->*handlers[code])(data, reply);
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t SessionListenerStub::HandleOnSessionCreate(MessageParcel& data, MessageParcel& reply)
{
    AVSESSION_TRACE_SYNC_START("SessionListenerStub::OnSessionCreate");
    AVSessionDescriptor descriptor;
    CHECK_AND_RETURN_RET_LOG(descriptor.ReadFromParcel(data), ERR_NONE, "read descriptor failed");
    OnSessionCreate(descriptor);
    return ERR_NONE;
}

int32_t SessionListenerStub::HandleOnSessionRelease(MessageParcel& data, MessageParcel& reply)
{
    AVSessionDescriptor descriptor;
    CHECK_AND_RETURN_RET_LOG(descriptor.ReadFromParcel(data), ERR_NONE, "read descriptor failed");
    OnSessionRelease(descriptor);
    return ERR_NONE;
}

int32_t SessionListenerStub::HandleOnTopSessionChange(MessageParcel& data, MessageParcel& reply)
{
    AVSESSION_TRACE_SYNC_START("SessionListenerStub::OnTopSessionChange");
    AVSessionDescriptor descriptor;
    CHECK_AND_RETURN_RET_LOG(descriptor.ReadFromParcel(data), ERR_NONE, "read descriptor failed");
    OnTopSessionChange(descriptor);
    return ERR_NONE;
}
} // namespace OHOS::AVSession