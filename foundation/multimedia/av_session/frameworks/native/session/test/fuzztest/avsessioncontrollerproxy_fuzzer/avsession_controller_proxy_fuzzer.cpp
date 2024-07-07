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

#include <iostream>
#include <cstddef>
#include <cstdint>

#include "avsession_callback_proxy.h"
#include "avsession_controller_proxy.h"
#include "avsession_errors.h"
#include "avsession_log.h"
#include "avsession_controller_proxy_fuzzer.h"

using namespace std;
using namespace OHOS;
using namespace OHOS::AVSession;

constexpr int32_t MAX_CODE_TEST  = 12;
constexpr int32_t MAX_CODE_LEN = 512;
constexpr int32_t MIN_SIZE_NUM = 4;

bool AvsessionControllerProxyFuzzer::FuzzSendRequest(uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size > MAX_CODE_LEN) || (size < MIN_SIZE_NUM)) {
        return false;
    }

    uint32_t cmdCode = *(reinterpret_cast<const uint32_t*>(data));
    if (cmdCode >= MAX_CODE_TEST) {
        return false;
    }

    sptr<IRemoteObject> remoteObject = nullptr;
    std::shared_ptr<AvsessionControllerProxyFuzzerTest> avSessionProxy =
        std::make_shared<AvsessionControllerProxyFuzzerTest>(remoteObject);

    MessageParcel request;
    CHECK_AND_RETURN_RET_LOG(request.WriteInterfaceToken(avSessionProxy->GetDescriptor()), ERR_MARSHALLING,
        "write interface token failed");

    MessageParcel reply;
    MessageOption option;
    auto remote = avSessionProxy->GetRemote();
    CHECK_AND_RETURN_RET_LOG(remote != nullptr, ERR_SERVICE_NOT_EXIST, "get remote service failed");
    size -= sizeof(uint32_t);
    request.WriteBuffer(data + sizeof(uint32_t), size);
    request.RewindRead(0);
    int32_t result = AVSESSION_ERROR;
    CHECK_AND_RETURN_RET_LOG((result = remote->SendRequest(cmdCode, request, reply, option)) == 0,
        ERR_IPC_SEND_REQUEST, "send request failed");
    return result == AVSESSION_SUCCESS;
}

bool OHOS::AVSession::AvsessionControllerProxySendRequest(uint8_t* data, size_t size)
{
    auto avsessionProxy = std::make_unique<AvsessionControllerProxyFuzzer>();
    CHECK_AND_RETURN_RET_LOG(avsessionProxy != nullptr, false, "avsessionProxy is null");
    return avsessionProxy->FuzzSendRequest(data, size);
}

void OHOS::AVSession::AvsessionControllerProxyTest(uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size > MAX_CODE_LEN) || (size < MIN_SIZE_NUM)) {
        return;
    }

    AVPlaybackState state;
    int32_t _state = *(reinterpret_cast<const int32_t*>(data));
    state.SetState(_state);

    AVMetaData metaData;
    std::string _data(reinterpret_cast<const char*>(data), size);
    metaData.SetAssetId(_data);

    std::vector<int32_t> cmds;
    int32_t _cmds = *(reinterpret_cast<const int32_t*>(data));
    cmds.push_back(_cmds);

    bool isActive = *(reinterpret_cast<const bool*>(data));

    AVControlCommand controlCommand;
    int32_t cmd = *(reinterpret_cast<const int32_t*>(data));
    controlCommand.SetCommand(cmd);

    sptr<IRemoteObject> impl = nullptr;
    AVSessionControllerProxy avSessionControllerProxy(impl);
    avSessionControllerProxy.GetAVPlaybackState(state);
    avSessionControllerProxy.GetAVMetaData(metaData);
    avSessionControllerProxy.GetValidCommands(cmds);
    avSessionControllerProxy.IsSessionActive(isActive);
    avSessionControllerProxy.SendControlCommand(controlCommand);
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AVSession::AvsessionControllerProxySendRequest(data, size);
    OHOS::AVSession::AvsessionControllerProxyTest(data, size);
    return 0;
}