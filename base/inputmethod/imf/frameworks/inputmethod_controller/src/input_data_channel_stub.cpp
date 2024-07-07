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

#include "input_data_channel_stub.h"

#include "global.h"
#include "input_method_controller.h"
#include "ipc_object_stub.h"
#include "ipc_types.h"
#include "message.h"

namespace OHOS {
namespace MiscServices {
    constexpr int32_t WAIT_TIME_STUB = 110;
    InputDataChannelStub::InputDataChannelStub() : msgHandler(nullptr)
    {
    }

    InputDataChannelStub::~InputDataChannelStub()
    {
    }

    int32_t InputDataChannelStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option)
    {
        IMSA_HILOGI("InputDataChannelStub::OnRemoteRequest code = %{public}d", code);
        auto descriptorToken = data.ReadInterfaceToken();
        if (descriptorToken != GetDescriptor()) {
            return ErrorCode::ERROR_STATUS_UNKNOWN_TRANSACTION;
        }
        switch (code) {
            case INSERT_TEXT: {
                auto text = data.ReadString16();
                reply.WriteInt32(InsertText(text));
                break;
            }
            case DELETE_FORWARD: {
                auto length = data.ReadInt32();
                reply.WriteInt32(DeleteForward(length));
                break;
            }
            case DELETE_BACKWARD: {
                auto length = data.ReadInt32();
                DeleteBackward(length);
                break;
            }
            case GET_TEXT_BEFORE_CURSOR: {
                auto number = data.ReadInt32();
                std::u16string text;
                reply.WriteInt32(HandleGetOperation(number, text, GET_TEXT_BEFORE_CURSOR));
                reply.WriteString16(text);
                break;
            }
            case GET_TEXT_AFTER_CURSOR: {
                auto number = data.ReadInt32();
                std::u16string text;
                reply.WriteInt32(HandleGetOperation(number, text, GET_TEXT_AFTER_CURSOR));
                reply.WriteString16(text);
                break;
            }
            case SEND_KEYBOARD_STATUS: {
                auto status = data.ReadInt32();
                SendKeyboardStatus(status);
                break;
            }
            case SEND_FUNCTION_KEY: {
                auto funcKey = data.ReadInt32();
                reply.WriteInt32(SendFunctionKey(funcKey));
                break;
            }
            case MOVE_CURSOR: {
                auto keyCode = data.ReadInt32();
                reply.WriteInt32(MoveCursor(keyCode));
                break;
            }
            case GET_ENTER_KEY_TYPE: {
                int32_t keyType = 0;
                reply.WriteInt32(GetEnterKeyType(keyType));
                reply.WriteInt32(keyType);
                break;
            }
            case GET_INPUT_PATTERN: {
                int32_t inputPattern = 0;
                reply.WriteInt32(GetInputPattern(inputPattern));
                reply.WriteInt32(inputPattern);
                break;
            }
            case HANDLE_SET_SELECTION: {
                auto start = data.ReadInt32();
                auto end = data.ReadInt32();
                HandleSetSelection(start, end);
                break;
            }
            case HANDLE_EXTEND_ACTION: {
                auto action = data.ReadInt32();
                HandleExtendAction(action);
                break;
            }
            case HANDLE_SELECT: {
                auto keyCode = data.ReadInt32();
                auto cursorMoveSkip = data.ReadInt32();
                HandleSelect(keyCode, cursorMoveSkip);
                break;
            }
            default:
                return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
        return NO_ERROR;
    }

    int32_t InputDataChannelStub::InsertText(const std::u16string& text)
    {
        IMSA_HILOGI("InputDataChannelStub::InsertText");
        if (msgHandler) {
            MessageParcel *parcel = new MessageParcel;
            parcel->WriteString16(text);
            Message *msg = new Message(MessageID::MSG_ID_INSERT_CHAR, parcel);
            msgHandler->SendMessage(msg);
            IMSA_HILOGI("InputDataChannelStub::InsertText return true");
            return ErrorCode::NO_ERROR;
        }
        return ErrorCode::ERROR_CLIENT_NULL_POINTER;
    }

    int32_t InputDataChannelStub::DeleteForward(int32_t length)
    {
        IMSA_HILOGI("InputDataChannelStub::DeleteForward");
        if (!msgHandler) {
            return ErrorCode::ERROR_CLIENT_NULL_POINTER;
        }
        MessageParcel *parcel = new MessageParcel;
        parcel->WriteInt32(length);
        Message *msg = new Message(MessageID::MSG_ID_DELETE_FORWARD, parcel);
        msgHandler->SendMessage(msg);

        return ErrorCode::NO_ERROR;
    }

    int32_t InputDataChannelStub::DeleteBackward(int32_t length)
    {
        IMSA_HILOGI("InputDataChannelStub::DeleteBackward");
        if (msgHandler) {
            MessageParcel *parcel = new MessageParcel;
            parcel->WriteInt32(length);
            Message *msg = new Message(MessageID::MSG_ID_DELETE_BACKWARD, parcel);
            msgHandler->SendMessage(msg);
            return ErrorCode::NO_ERROR;
        }
        return ErrorCode::ERROR_CLIENT_NULL_POINTER;
    }


    int32_t InputDataChannelStub::GetTextBeforeCursor(int32_t number, std::u16string &text)
    {
        IMSA_HILOGI("InputDataChannelStub::GetTextBeforeCursor");
        return InputMethodController::GetInstance()->GetTextBeforeCursor(number, text);
    }

    int32_t InputDataChannelStub::GetTextAfterCursor(int32_t number, std::u16string &text)
    {
        IMSA_HILOGI("InputDataChannelStub::GetTextAfterCursor");
        return InputMethodController::GetInstance()->GetTextAfterCursor(number, text);
    }

    int32_t InputDataChannelStub::GetEnterKeyType(int32_t &keyType)
    {
        IMSA_HILOGI("InputDataChannelStub::GetEnterKeyType");
        return InputMethodController::GetInstance()->GetEnterKeyType(keyType);
    }

    int32_t InputDataChannelStub::GetInputPattern(int32_t &inputPattern)
    {
        IMSA_HILOGI("InputDataChannelStub::GetInputPattern");
        return InputMethodController::GetInstance()->GetInputPattern(inputPattern);
    }

    int32_t InputDataChannelStub::HandleGetOperation(int32_t number, std::u16string &text, int32_t msgType)
    {
        IMSA_HILOGI("InputDataChannelStub::start, msgId: %{public}d, number: %{public}d", msgType, number);
        if (msgHandler == nullptr) {
            return ErrorCode::ERROR_CLIENT_NULL_POINTER;
        }
        int32_t msgId;
        if (msgType == GET_TEXT_BEFORE_CURSOR) {
            msgId = MessageID::MSG_ID_GET_TEXT_BEFORE_CURSOR;
        } else {
            msgId = MessageID::MSG_ID_GET_TEXT_AFTER_CURSOR;
        }
        MessageParcel *parcel = new MessageParcel;
        Message *msg = new Message(msgId, parcel);
        msgHandler->SendMessage(msg);

        std::unique_lock<std::mutex> lock(getOperationListenerLock_);
        getOperationListenerCv_.wait_for(lock, std::chrono::milliseconds(WAIT_TIME_STUB));
        if (msgType == GET_TEXT_BEFORE_CURSOR) {
            return InputMethodController::GetInstance()->GetTextBeforeCursor(number, text);
        } else {
            return InputMethodController::GetInstance()->GetTextAfterCursor(number, text);
        }
    }

    void InputDataChannelStub::NotifyGetOperationCompletion()
    {
        getOperationListenerCv_.notify_one();
    }

    void InputDataChannelStub::SendKeyboardStatus(int32_t status)
    {
        IMSA_HILOGI("InputDataChannelStub::SendKeyboardStatus");
        if (msgHandler) {
            MessageParcel *parcel = new MessageParcel;
            parcel->WriteInt32(status);
            Message *msg = new Message(MessageID::MSG_ID_SEND_KEYBOARD_STATUS, parcel);
            msgHandler->SendMessage(msg);
        }
    }

    int32_t InputDataChannelStub::SendFunctionKey(int32_t funcKey)
    {
        IMSA_HILOGI("InputDataChannelStub::SendFunctionKey");
        if (msgHandler) {
            MessageParcel *parcel = new MessageParcel;
            parcel->WriteInt32(funcKey);
            Message *msg = new Message(MessageID::MSG_ID_SEND_FUNCTION_KEY, parcel);
            msgHandler->SendMessage(msg);
            return ErrorCode::NO_ERROR;
        }
        return ErrorCode::ERROR_CLIENT_NULL_POINTER;
    }

    int32_t InputDataChannelStub::MoveCursor(int32_t keyCode)
    {
        IMSA_HILOGI("InputDataChannelStub::MoveCursor");
        if (msgHandler) {
            MessageParcel *parcel = new MessageParcel;
            parcel->WriteInt32(keyCode);
            Message *msg = new Message(MessageID::MSG_ID_MOVE_CURSOR, parcel);
            msgHandler->SendMessage(msg);
            return ErrorCode::NO_ERROR;
        }
        return ErrorCode::ERROR_CLIENT_NULL_POINTER;
    }

    void InputDataChannelStub::HandleSetSelection(int32_t start, int32_t end)
    {
        IMSA_HILOGI("InputDataChannelStub::HandleSetSelection");
        if (msgHandler) {
            MessageParcel *parcel = new MessageParcel;
            parcel->WriteInt32(start);
            parcel->WriteInt32(end);
            Message *msg = new Message(MessageID::MSG_ID_HANDLE_SET_SELECTION, parcel);
            msgHandler->SendMessage(msg);
        }
    }

    void InputDataChannelStub::HandleExtendAction(int32_t action)
    {
        IMSA_HILOGI("InputDataChannelStub::HandleExtendAction");
        if (msgHandler) {
            MessageParcel *parcel = new MessageParcel;
            parcel->WriteInt32(action);
            Message *msg = new Message(MessageID::MSG_ID_HANDLE_EXTEND_ACTION, parcel);
            msgHandler->SendMessage(msg);
        }
    }

    void InputDataChannelStub::HandleSelect(int32_t keyCode, int32_t cursorMoveSkip)
    {
        IMSA_HILOGI("InputDataChannelStub::HandleSelect");
        if (msgHandler) {
            MessageParcel *parcel = new MessageParcel;
            parcel->WriteInt32(keyCode);
            parcel->WriteInt32(cursorMoveSkip);
            Message *msg = new Message(MessageID::MSG_ID_HANDLE_SELECT, parcel);
            msgHandler->SendMessage(msg);
        }
    }

    void InputDataChannelStub::SetHandler(MessageHandler *handler)
    {
        msgHandler = handler;
    }
} // namespace MiscServices
} // namespace OHOS
