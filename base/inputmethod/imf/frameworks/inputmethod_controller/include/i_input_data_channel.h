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

#ifndef FRAMEWORKS_INPUTMETHOD_CONTROLLER_INCLUDE_I_INPUT_DATA_CHANNEL_H
#define FRAMEWORKS_INPUTMETHOD_CONTROLLER_INCLUDE_I_INPUT_DATA_CHANNEL_H
#include <errors.h>
#include "iremote_broker.h"
#include "global.h"
#include "input_method_utils.h"

/**
 * brief Definition of interface IInputDataChannel
 * It defines the remote calls from input method service to input client
 */
namespace OHOS {
namespace MiscServices {
    class IInputDataChannel : public IRemoteBroker {
    public:
        enum {
            INSERT_TEXT = 0,
            DELETE_FORWARD,
            DELETE_BACKWARD,
            GET_TEXT_BEFORE_CURSOR,
            GET_TEXT_AFTER_CURSOR,
            GET_ENTER_KEY_TYPE,
            GET_INPUT_PATTERN,
            SEND_KEYBOARD_STATUS,
            SEND_FUNCTION_KEY,
            MOVE_CURSOR,
            HANDLE_SET_SELECTION,
            HANDLE_EXTEND_ACTION,
            HANDLE_SELECT,
        };

        DECLARE_INTERFACE_DESCRIPTOR(u"ohos.miscservices.inputmethod.IInputDataChannel");

        virtual int32_t InsertText(const std::u16string& text) = 0;
        virtual int32_t DeleteForward(int32_t length) = 0;
        virtual int32_t DeleteBackward(int32_t length) = 0;
        virtual int32_t GetTextBeforeCursor(int32_t number, std::u16string &text) = 0;
        virtual int32_t GetTextAfterCursor(int32_t number, std::u16string &text) = 0;
        virtual void SendKeyboardStatus(int32_t status) = 0;
        virtual int32_t SendFunctionKey(int32_t funcKey) = 0;
        virtual int32_t MoveCursor(int32_t keyCode) = 0;
        virtual int32_t GetEnterKeyType(int32_t &keyType) = 0;
        virtual int32_t GetInputPattern(int32_t &inputPattern) = 0;
        virtual void HandleSetSelection(int32_t start, int32_t end) = 0;
        virtual void HandleExtendAction(int32_t action) = 0;
        virtual void HandleSelect(int32_t keyCode, int32_t cursorMoveSkip) = 0;
        virtual void NotifyGetOperationCompletion() = 0;
    };
} // namespace MiscServices
} // namespace OHOS
#endif // FRAMEWORKS_INPUTMETHOD_CONTROLLER_INCLUDE_I_INPUT_DATA_CHANNEL_H
