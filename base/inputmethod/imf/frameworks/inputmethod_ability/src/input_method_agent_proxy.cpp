/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "input_method_agent_proxy.h"
#include "global.h"
#include "message_option.h"

namespace OHOS {
namespace MiscServices {
    using namespace ErrorCode;
    InputMethodAgentProxy::InputMethodAgentProxy(const sptr<IRemoteObject> &object)
        : IRemoteProxy<IInputMethodAgent>(object)
    {
    }

    bool InputMethodAgentProxy::DispatchKeyEvent(MessageParcel& data)
    {
        MessageParcel reply;
        MessageOption option;

        auto ret = Remote()->SendRequest(DISPATCH_KEY_EVENT, data, reply, option);
        if (ret != NO_ERROR) {
            IMSA_HILOGE("InputMethodAgentProxy::DispatchKeyEvent SendRequest failed");
        }
        ret = reply.ReadBool();
        return ret;
    }

    void InputMethodAgentProxy::OnCursorUpdate(int32_t positionX, int32_t positionY, int32_t height)
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        if (!data.WriteInterfaceToken(GetDescriptor())) {
            IMSA_HILOGE("InputMethodAgentProxy::OnCursorUpdate descriptor is not match");
            return;
        }

        data.WriteInt32(positionX);
        data.WriteInt32(positionY);
        data.WriteInt32(height);

        Remote()->SendRequest(ON_CURSOR_UPDATE, data, reply, option);
    }

    void InputMethodAgentProxy::OnSelectionChange(std::u16string text, int32_t oldBegin, int32_t oldEnd,
                                                  int32_t newBegin, int32_t newEnd)
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        if (!data.WriteInterfaceToken(GetDescriptor())) {
            IMSA_HILOGE("InputMethodAgentProxy::OnSelectionChange descriptor is not match");
            return;
        }

        data.WriteString16(text);
        data.WriteInt32(oldBegin);
        data.WriteInt32(oldEnd);
        data.WriteInt32(newBegin);
        data.WriteInt32(newEnd);

        Remote()->SendRequest(ON_SELECTION_CHANGE, data, reply, option);
    }

    void InputMethodAgentProxy::SetCallingWindow(uint32_t windowId)
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        if (!data.WriteInterfaceToken(GetDescriptor())) {
            IMSA_HILOGE("InputMethodAgentProxy::SetCallingWindow descriptor is not match");
            return;
        }

        data.WriteUint32(windowId);
        Remote()->SendRequest(SET_CALLING_WINDOW_ID, data, reply, option);
    }
} // namespace MiscServices
} // namespace OHOS
