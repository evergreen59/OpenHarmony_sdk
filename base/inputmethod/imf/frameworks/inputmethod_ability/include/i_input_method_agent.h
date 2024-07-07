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

#ifndef FRAMEWORKS_INPUTMETHOD_ABILITY_INCLUDE_I_INPUT_METHOD_AGENT_H
#define FRAMEWORKS_INPUTMETHOD_ABILITY_INCLUDE_I_INPUT_METHOD_AGENT_H

#include "iremote_broker.h"
#include "global.h"

/**
 * brief Definition of interface IInputMethodAgent
 * It defines the remote calls from input client to input method service
 */
namespace OHOS {
namespace MiscServices {
    class IInputMethodAgent : public IRemoteBroker {
    public:
        enum {
            DISPATCH_KEY_EVENT = FIRST_CALL_TRANSACTION,
            ON_CURSOR_UPDATE,
            ON_SELECTION_CHANGE,
            SET_CALLING_WINDOW_ID,
        };

        DECLARE_INTERFACE_DESCRIPTOR(u"ohos.miscservices.inputmethod.IInputMethodAgent");

        virtual bool DispatchKeyEvent(MessageParcel& data) = 0;
        virtual void OnCursorUpdate(int32_t positionX, int32_t positionY, int height) = 0;
        virtual void OnSelectionChange(std::u16string text, int32_t oldBegin, int32_t oldEnd,
                                       int32_t newBegin, int32_t newEnd) = 0;
        virtual void SetCallingWindow(uint32_t windowId) = 0;
    };
} // namespace MiscServices
} // namespace OHOS
#endif // FRAMEWORKS_INPUTMETHOD_ABILITY_INCLUDE_I_INPUT_METHOD_AGENT_H
