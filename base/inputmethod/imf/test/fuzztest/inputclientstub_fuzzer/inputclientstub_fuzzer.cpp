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

#include "inputclientstub_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <input_method_agent_stub.h>

#include "global.h"
#include "input_client_stub.h"
#include "message_parcel.h"

using namespace OHOS::MiscServices;
namespace OHOS {
constexpr size_t THRESHOLD = 10;
constexpr int32_t OFFSET = 4;
const std::u16string INPUTCLIENTSTUB_INTERFACE_TOKEN = u"ohos.miscservices.inputmethod.InputClient";

uint32_t ConvertToUint32(const uint8_t *ptr)
{
    if (ptr == nullptr) {
        return 0;
    }
    uint32_t bigVar = (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | (ptr[3]);
    return bigVar;
}

void FuzzInputClientStub(const uint8_t *rawData, size_t size)
{
    uint32_t code = ConvertToUint32(rawData);
    rawData = rawData + OFFSET;
    size = size - OFFSET;

    MessageParcel data;
    data.WriteInterfaceToken(INPUTCLIENTSTUB_INTERFACE_TOKEN);
    data.WriteBuffer(rawData, size);
    data.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    sptr<InputClientStub> mClient = new InputClientStub();
    MessageHandler *handler = MessageHandler::Instance();
    mClient->SetHandler(handler);
    mClient->OnRemoteRequest(code, data, reply, option);
}

void TextOnInputReady()
{
    sptr<InputClientStub> mClient = new InputClientStub();
    sptr<InputMethodAgentStub> mInputMethodAgentStub = new InputMethodAgentStub();
    MessageParcel data;
    data.WriteRemoteObject(mInputMethodAgentStub->AsObject());
    auto remoteObject = data.ReadRemoteObject();
    sptr<IInputMethodAgent> iface = iface_cast<IInputMethodAgent>(remoteObject);
    mClient->onInputReady(iface);
}

void TestOnSwitchInput()
{
    sptr<InputClientStub> mClient = new InputClientStub();
    Property property = {};
    SubProperty subProperty = {};

    mClient->OnSwitchInput(property, subProperty);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < OHOS::THRESHOLD) {
        return 0;
    }
    /* Run your code on data */
    OHOS::FuzzInputClientStub(data, size);
    OHOS::TextOnInputReady();
    OHOS::TestOnSwitchInput();
    return 0;
}