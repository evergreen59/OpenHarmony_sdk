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

#include "input_method_controller.h"

#include "global.h"
#include "input_client_stub.h"
#include "input_data_channel_stub.h"
#include "input_method_agent_proxy.h"
#include "input_method_property.h"
#include "input_method_status.h"
#include "input_method_system_ability_proxy.h"
#include "inputmethod_sysevent.h"
#include "inputmethod_trace.h"
#include "iservice_registry.h"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "utils.h"

namespace OHOS {
namespace MiscServices {
using namespace MessageID;
    sptr<InputMethodController> InputMethodController::instance_;
    std::mutex InputMethodController::instanceLock_;
    constexpr int32_t WAIT_TIME = 100;
    constexpr int32_t KEYBOARD_SHOW = 2;
    InputMethodController::InputMethodController() : stop_(false)
    {
        IMSA_HILOGI("InputMethodController structure");
        Initialize();
    }

    InputMethodController::~InputMethodController()
    {
        QuitWorkThread();
        delete msgHandler;
        msgHandler = nullptr;
    }

    sptr<InputMethodController> InputMethodController::GetInstance()
    {
        if (!instance_) {
            std::lock_guard<std::mutex> autoLock(instanceLock_);
            if (!instance_) {
                IMSA_HILOGI("InputMethodController::GetInstance instance_ is nullptr");
                instance_ = new InputMethodController();
            }
        }
        return instance_;
    }

    void InputMethodController::setImeListener(std::shared_ptr<InputMethodSettingListener> imeListener)
    {
        IMSA_HILOGI("InputMethodController::setImeListener");
        if (imeListener_ == nullptr) {
            imeListener_ = imeListener;
        }
    }

    bool InputMethodController::Initialize()
    {
        msgHandler = new MessageHandler();

        InputClientStub *client = new (std::nothrow) InputClientStub();
        if (client == nullptr) {
            IMSA_HILOGE("InputMethodController::Initialize client is nullptr");
            return false;
        }
        client->SetHandler(msgHandler);
        mClient = client;

        InputDataChannelStub *channel = new (std::nothrow) InputDataChannelStub();
        if (channel == nullptr) {
            IMSA_HILOGE("InputMethodController::Initialize channel is nullptr");
            return false;
        }
        channel->SetHandler(msgHandler);
        mInputDataChannel = channel;

        workThreadHandler = std::thread([this] { WorkThread(); });
        mAttribute.inputPattern = InputAttribute::PATTERN_TEXT;
        {
            std::lock_guard<std::mutex> lock(textListenerLock_);
            textListener = nullptr;
        }
        IMSA_HILOGI("InputMethodController::Initialize textListener is nullptr");
        PrepareInput(0, mClient, mInputDataChannel, mAttribute);
        return true;
    }

    sptr<IInputMethodSystemAbility> InputMethodController::GetSystemAbilityProxy()
    {
        std::lock_guard<std::mutex> lock(abilityLock_);
        if (abilityManager_ != nullptr) {
            return abilityManager_;
        }
        IMSA_HILOGI("get input method service proxy");
        sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (systemAbilityManager == nullptr) {
            IMSA_HILOGI("system ability manager is nullptr");
            return nullptr;
        }
        auto systemAbility = systemAbilityManager->GetSystemAbility(INPUT_METHOD_SYSTEM_ABILITY_ID, "");
        if (systemAbility == nullptr) {
            IMSA_HILOGI("system ability is nullptr");
            return nullptr;
        }
        if (deathRecipient_ == nullptr) {
            deathRecipient_ = new (std::nothrow)ImsaDeathRecipient();
            if (deathRecipient_ == nullptr) {
                IMSA_HILOGE("new death recipient failed");
                return nullptr;
            }
        }
        if ((systemAbility->IsProxyObject()) && (!systemAbility->AddDeathRecipient(deathRecipient_))) {
            IMSA_HILOGE("failed to add death recipient.");
            return nullptr;
        }
        abilityManager_ = iface_cast<IInputMethodSystemAbility>(systemAbility);
        return abilityManager_;
    }

    void InputMethodController::WorkThread()
    {
        while (!stop_) {
            Message *msg = msgHandler->GetMessage();
            switch (msg->msgId_) {
                case MSG_ID_INSERT_CHAR: {
                    MessageParcel *data = msg->msgContent_;
                    std::u16string text = data->ReadString16();
                    IMSA_HILOGI("InputMethodController::WorkThread InsertText");
                    std::lock_guard<std::mutex> lock(textListenerLock_);
                    if (textListener) {
                        textListener->InsertText(text);
                        std::unique_lock<std::mutex> numLock(textFieldReplyCountLock_);
                        textFieldReplyCount_++;
                    }
                    break;
                }

                case MSG_ID_DELETE_FORWARD: {
                    MessageParcel *data = msg->msgContent_;
                    int32_t length = data->ReadInt32();
                    IMSA_HILOGI("InputMethodController::WorkThread DeleteForward");
                    std::lock_guard<std::mutex> lock(textListenerLock_);
                    if (textListener) {
                        textListener->DeleteForward(length);
                        std::unique_lock<std::mutex> numLock(textFieldReplyCountLock_);
                        textFieldReplyCount_++;
                    }
                    break;
                }
                case MSG_ID_DELETE_BACKWARD: {
                    MessageParcel *data = msg->msgContent_;
                    int32_t length = data->ReadInt32();
                    IMSA_HILOGI("InputMethodController::WorkThread DeleteBackward");
                    std::lock_guard<std::mutex> lock(textListenerLock_);
                    if (textListener) {
                        textListener->DeleteBackward(length);
                        std::unique_lock<std::mutex> numLock(textFieldReplyCountLock_);
                        textFieldReplyCount_++;
                    }
                    break;
                }
                case MSG_ID_ON_INPUT_READY: {
                    MessageParcel *data = msg->msgContent_;
                    sptr<IRemoteObject> object = data->ReadRemoteObject();
                    if (object) {
                        SetInputMethodAgent(object);
                    }
                    break;
                }
                case MSG_ID_SEND_KEYBOARD_STATUS: {
                    MessageParcel *data = msg->msgContent_;
                    int32_t ret = data->ReadInt32();
                    KeyboardInfo *info = new KeyboardInfo();
                    info->SetKeyboardStatus(ret);
                    IMSA_HILOGI("InputMethodController::WorkThread SendKeyboardInfo");
                    std::lock_guard<std::mutex> lock(textListenerLock_);
                    if (textListener) {
                        textListener->SendKeyboardInfo(*info);
                        DoIncrease(ret);
                    }
                    delete info;
                    break;
                }
                case MSG_ID_SEND_FUNCTION_KEY: {
                    MessageParcel *data = msg->msgContent_;
                    int32_t ret = data->ReadInt32();
                    KeyboardInfo *info = new KeyboardInfo();
                    info->SetFunctionKey(ret);
                    IMSA_HILOGI("InputMethodController::WorkThread SendKeyboardInfo");
                    std::lock_guard<std::mutex> lock(textListenerLock_);
                    if (textListener) {
                        textListener->SendKeyboardInfo(*info);
                    }
                    delete info;
                    break;
                }
                case MSG_ID_MOVE_CURSOR: {
                    MessageParcel *data = msg->msgContent_;
                    int32_t ret = data->ReadInt32();
                    IMSA_HILOGI("InputMethodController::WorkThread MoveCursor");
                    std::lock_guard<std::mutex> lock(textListenerLock_);
                    if (textListener) {
                        Direction direction = static_cast<Direction>(ret);
                        textListener->MoveCursor(direction);
                        std::unique_lock<std::mutex> numLock(textFieldReplyCountLock_);
                        textFieldReplyCount_++;
                    }
                    break;
                }
                case MSG_ID_ON_SWITCH_INPUT: {
                    auto data = msg->msgContent_;
                    Property property;
                    SubProperty subProperty;
                    if (!ITypesUtil::Unmarshal(*data, property, subProperty)) {
                        IMSA_HILOGE("read property from message parcel failed");
                        break;
                    }
                    OnSwitchInput(property, subProperty);
                    break;
                }
                case MSG_ID_HANDLE_SET_SELECTION: {
                    MessageParcel *data = msg->msgContent_;
                    int32_t start = data->ReadInt32();
                    int32_t end = data->ReadInt32();
                    IMSA_HILOGI("InputMethodController::WorkThread HandleSetSelection");
                    if (textListener) {
                        textListener->HandleSetSelection(start, end);
                        std::unique_lock<std::mutex> numLock(textFieldReplyCountLock_);
                        textFieldReplyCount_++;
                    }
                    break;
                }
                case MSG_ID_HANDLE_EXTEND_ACTION: {
                    MessageParcel *data = msg->msgContent_;
                    int32_t action = data->ReadInt32();
                    IMSA_HILOGI("InputMethodController::WorkThread HandleExtendAction");
                    if (textListener) {
                        textListener->HandleExtendAction(action);
                        std::unique_lock<std::mutex> numLock(textFieldReplyCountLock_);
                        textFieldReplyCount_++;
                    }
                    break;
                }
                case MSG_ID_HANDLE_SELECT: {
                    MessageParcel *data = msg->msgContent_;
                    int32_t keyCode = data->ReadInt32();
                    int32_t cursorMoveSkip = data->ReadInt32();
                    IMSA_HILOGI("InputMethodController::WorkThread HandleSelect");
                    if (textListener) {
                        textListener->HandleSelect(keyCode, cursorMoveSkip);
                        std::unique_lock<std::mutex> numLock(textFieldReplyCountLock_);
                        textFieldReplyCount_++;
                    }
                    break;
                }
                case MSG_ID_GET_TEXT_BEFORE_CURSOR:
                case MSG_ID_GET_TEXT_AFTER_CURSOR: {
                    IMSA_HILOGI("InputMethodController::WorkThread HandleGetOperation, msgId: %{public}d", msg->msgId_);
                    HandleGetOperation();
                    break;
                }
                default: {
                    IMSA_HILOGD("the message is %{public}d.", msg->msgId_);
                    break;
                }
            }
            delete msg;
            msg = nullptr;
        }
    }

    void InputMethodController::DoIncrease(int32_t status)
    {
        if (status == KEYBOARD_SHOW) {
            std::unique_lock<std::mutex> numLock(textFieldReplyCountLock_);
            textFieldReplyCount_++;
        }
    }

    void InputMethodController::QuitWorkThread()
    {
        stop_ = true;
        Message *msg = new Message(MessageID::MSG_ID_QUIT_WORKER_THREAD, nullptr);
        msgHandler->SendMessage(msg);
        if (workThreadHandler.joinable()) {
            workThreadHandler.join();
        }
    }

    void InputMethodController::OnSwitchInput(const Property &property, const SubProperty &subProperty)
    {
        IMSA_HILOGE("InputMethodController::OnSwitchInput");
        if (imeListener_ == nullptr) {
            IMSA_HILOGE("imeListener_ is nullptr");
            return;
        }
        imeListener_->OnImeChange(property, subProperty);
    }

    void InputMethodController::Attach(sptr<OnTextChangedListener> &listener)
    {
        Attach(listener, true);
    }

    void InputMethodController::Attach(sptr<OnTextChangedListener> &listener, bool isShowKeyboard)
    {
        InputAttribute attribute;
        attribute.inputPattern = InputAttribute::PATTERN_TEXT;
        Attach(listener, isShowKeyboard, attribute);  
    }

    void InputMethodController::Attach(
        sptr<OnTextChangedListener> &listener, bool isShowKeyboard, InputAttribute &attribute)
    {
        {
            std::unique_lock<std::mutex> numLock(textFieldReplyCountLock_);
            textFieldReplyCount_ = 0;
        }
        {
            std::lock_guard<std::mutex> lock(textListenerLock_);
            textListener = listener;
        }
        IMSA_HILOGI("InputMethodController::Attach");
        InputmethodTrace tracer("InputMethodController Attach trace.");
        IMSA_HILOGI("InputMethodController::Attach isShowKeyboard %{public}s", isShowKeyboard ? "true" : "false");
        mAttribute = attribute;
        PrepareInput(0, mClient, mInputDataChannel, mAttribute);
        StartInput(mClient, isShowKeyboard);
    }

    void InputMethodController::ShowTextInput()
    {
        IMSA_HILOGI("InputMethodController::ShowTextInput");
        StartInput(mClient, true);
    }

    void InputMethodController::HideTextInput()
    {
        IMSA_HILOGD("InputMethodController::HideTextInput");
        StopInput(mClient);
    }

    int32_t InputMethodController::HideCurrentInput()
    {
        IMSA_HILOGD("InputMethodController::HideCurrentInput");
        auto proxy = GetSystemAbilityProxy();
        if (proxy == nullptr) {
            IMSA_HILOGE("proxy is nullptr");
            return ErrorCode::ERROR_EX_NULL_POINTER;
        }
        return proxy->HideCurrentInputDeprecated();
    }

    int32_t InputMethodController::ShowCurrentInput()
    {
        IMSA_HILOGI("InputMethodController::ShowCurrentInput");
        auto proxy = GetSystemAbilityProxy();
        if (proxy == nullptr) {
            IMSA_HILOGE("proxy is nullptr");
            return ErrorCode::ERROR_EX_NULL_POINTER;
        }
        return proxy->ShowCurrentInputDeprecated();
    }

    void InputMethodController::Close()
    {
        ReleaseInput(mClient);
        InputmethodTrace tracer("InputMethodController Close trace.");
        {
            std::lock_guard<std::mutex> lock(textListenerLock_);
            textListener = nullptr;
        }
        IMSA_HILOGI("InputMethodController::Close");
    }

    void InputMethodController::PrepareInput(int32_t displayId, sptr<IInputClient> &client,
        sptr<IInputDataChannel> &channel, InputAttribute &attribute)
    {
        IMSA_HILOGI("InputMethodController::PrepareInput");
        auto proxy = GetSystemAbilityProxy();
        if (proxy == nullptr) {
            IMSA_HILOGE("proxy is nullptr");
            return;
        }
        proxy->PrepareInput(displayId, client, channel, attribute);
    }

    int32_t InputMethodController::DisplayOptionalInputMethod()
    {
        IMSA_HILOGI("InputMethodController::DisplayOptionalInputMethod");
        auto proxy = GetSystemAbilityProxy();
        if (proxy == nullptr) {
            IMSA_HILOGE("proxy is nullptr");
            return ErrorCode::ERROR_EX_NULL_POINTER;
        }
        return proxy->DisplayOptionalInputMethodDeprecated();
    }

    int32_t InputMethodController::ListInputMethodCommon(InputMethodStatus status, std::vector<Property> &props)
    {
        IMSA_HILOGI("InputMethodController::ListInputMethodCommon");
        auto proxy = GetSystemAbilityProxy();
        if (proxy == nullptr) {
            IMSA_HILOGE("proxy is nullptr");
            return ErrorCode::ERROR_EX_NULL_POINTER;
        }
        return proxy->ListInputMethod(status, props);
    }

    int32_t InputMethodController::ListInputMethod(std::vector<Property> &props)
    {
        IMSA_HILOGI("InputMethodController::listInputMethod");
        return ListInputMethodCommon(ALL, props);
    }

    int32_t InputMethodController::ListInputMethod(bool enable, std::vector<Property> &props)
    {
        IMSA_HILOGI("InputMethodController::listInputMethod enable = %{public}s", enable ? "ENABLE" : "DISABLE");
        return ListInputMethodCommon(enable ? ENABLE : DISABLE, props);
    }

    std::shared_ptr<Property> InputMethodController::GetCurrentInputMethod()
    {
        IMSA_HILOGD("InputMethodController::GetCurrentInputMethod");
        auto proxy = GetSystemAbilityProxy();
        if (proxy == nullptr) {
            IMSA_HILOGE("proxy is nullptr");
            return nullptr;
        }
        auto property = proxy->GetCurrentInputMethod();
        if (property == nullptr) {
            IMSA_HILOGE("InputMethodController::GetCurrentInputMethod property is nullptr");
            return nullptr;
        }
        return property;
    }

    std::shared_ptr<SubProperty> InputMethodController::GetCurrentInputMethodSubtype()
    {
        IMSA_HILOGD("InputMethodController::GetCurrentInputMethod");
        auto proxy = GetSystemAbilityProxy();
        if (proxy == nullptr) {
            IMSA_HILOGE("proxy is nullptr");
            return nullptr;
        }
        auto property = proxy->GetCurrentInputMethodSubtype();
        if (property == nullptr) {
            IMSA_HILOGE("InputMethodController::GetCurrentInputMethodSubtype property is nullptr");
            return nullptr;
        }
        return property;
    }

    void InputMethodController::StartInput(sptr<IInputClient> &client, bool isShowKeyboard)
    {
        IMSA_HILOGI("InputMethodController::StartInput");
        isStopInput = false;
        auto proxy = GetSystemAbilityProxy();
        if (proxy == nullptr) {
            IMSA_HILOGE("proxy is nullptr");
            return;
        }
        proxy->StartInput(client, isShowKeyboard);
    }

    void InputMethodController::ReleaseInput(sptr<IInputClient> &client)
    {
        IMSA_HILOGD("InputMethodController::ReleaseInput");
        isStopInput = true;
        auto proxy = GetSystemAbilityProxy();
        if (proxy == nullptr) {
            IMSA_HILOGE("proxy is nullptr");
            return;
        }
        proxy->ReleaseInput(client);
    }

    void InputMethodController::StopInput(sptr<IInputClient> &client)
    {
        IMSA_HILOGD("InputMethodController::StopInput");
        isStopInput = true;
        auto proxy = GetSystemAbilityProxy();
        if (proxy == nullptr) {
            IMSA_HILOGE("proxy is nullptr");
            return;
        }
        proxy->StopInput(client);
    }

    void InputMethodController::OnRemoteSaDied(const wptr<IRemoteObject> &remote)
    {
        IMSA_HILOGE("input method service death");
        std::lock_guard<std::mutex> lock(abilityLock_);
        abilityManager_ = nullptr;
    }

    ImsaDeathRecipient::ImsaDeathRecipient()
    {
    }

    void ImsaDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &object)
    {
        InputMethodController::GetInstance()->OnRemoteSaDied(object);
    }

    void InputMethodController::OnCursorUpdate(CursorInfo cursorInfo)
    {
        if (isStopInput) {
            IMSA_HILOGD("InputMethodController::OnCursorUpdate isStopInput");
            return;
        }
        std::shared_ptr<IInputMethodAgent> agent = GetInputMethodAgent();
        if (agent == nullptr) {
            IMSA_HILOGI("InputMethodController::OnCursorUpdate agent is nullptr");
            return;
        }
        if (cursorInfo_.left == cursorInfo.left && cursorInfo_.top == cursorInfo.top
            && cursorInfo_.height == cursorInfo.height) {
            return;
        }
        cursorInfo_ = cursorInfo;
        agent->OnCursorUpdate(cursorInfo.left, cursorInfo.top, cursorInfo.height);
    }

    void InputMethodController::OnSelectionChange(std::u16string text, int start, int end)
    {
        IMSA_HILOGI("size: %{public}zu, start: %{public}d, end: %{public}d, replyCount: %{public}d", text.size(),
            start, end, textFieldReplyCount_);
        if (isStopInput) {
            IMSA_HILOGD("InputMethodController::OnSelectionChange isStopInput");
            return;
        }
        {
            std::unique_lock<std::mutex> numLock(textFieldReplyCountLock_);
            if (textFieldReplyCount_ > 0
                && (text.size() != mTextString.size() || start != mSelectNewBegin || end != mSelectNewEnd)) {
                textFieldReplyCount_--;
            }
            if (textFieldReplyCount_ == 0) {
                textFieldReplyCountCv_.notify_one();
            }
        }
        if (mTextString == text && mSelectNewBegin == start && mSelectNewEnd == end) {
            return;
        }
        mTextString = text;
        mSelectOldBegin = mSelectNewBegin;
        mSelectOldEnd = mSelectNewEnd;
        mSelectNewBegin = start;
        mSelectNewEnd = end;
        std::shared_ptr<IInputMethodAgent> agent = GetInputMethodAgent();
        if (agent == nullptr) {
            IMSA_HILOGI("InputMethodController::OnSelectionChange agent is nullptr");
            return;
        }
        agent->OnSelectionChange(mTextString, mSelectOldBegin, mSelectOldEnd, mSelectNewBegin, mSelectNewEnd);
    }

    void InputMethodController::OnConfigurationChange(Configuration info)
    {
        IMSA_HILOGI("InputMethodController::OnConfigurationChange");
        enterKeyType_ = static_cast<uint32_t>(info.GetEnterKeyType());
        inputPattern_ = static_cast<uint32_t>(info.GetTextInputType());
    }

    void InputMethodController::HandleGetOperation()
    {
        IMSA_HILOGI("InputMethodController::start");
        if (isStopInput) {
            IMSA_HILOGE("InputMethodController::text filed is not Focused");
            mSelectNewEnd = -1;
            mInputDataChannel->NotifyGetOperationCompletion();
            return;
        }
        std::unique_lock<std::mutex> numLock(textFieldReplyCountLock_);
        auto ret = textFieldReplyCountCv_.wait_for(
            numLock, std::chrono::milliseconds(WAIT_TIME), [this] { return textFieldReplyCount_ == 0; });
        if (!ret) {
            IMSA_HILOGE("InputMethodController::timeout");
            // timeout,reset the waitOnSelectionChangeNum_ to eliminate the impact on subsequent processing
            textFieldReplyCount_ = 0;
        }
        IMSA_HILOGI("InputMethodController::notify");
        mInputDataChannel->NotifyGetOperationCompletion();
    }

    bool InputMethodController::IsCorrectParam(int32_t number)
    {
        if (mTextString.size() > INT_MAX || number < 0 || mSelectNewEnd < 0 || mSelectNewBegin < 0) {
            IMSA_HILOGE("InputMethodController::param error, number: %{public}d, begin: %{public}d, end: %{public}d",
                number, mSelectNewBegin, mSelectNewEnd);
            return false;
        }
        if (mSelectNewBegin > mSelectNewEnd) {
            int32_t temp = mSelectNewEnd;
            mSelectNewEnd = mSelectNewBegin;
            mSelectNewBegin = temp;
        }
        if (static_cast<size_t>(mSelectNewEnd) > mTextString.size()) {
            IMSA_HILOGE("InputMethodController::param error, end: %{public}d, size: %{public}zu", mSelectNewEnd,
                mTextString.size());
            return false;
        }
        return true;
    }

    int32_t InputMethodController::GetTextBeforeCursor(int32_t number, std::u16string &text)
    {
        IMSA_HILOGI("InputMethodController::GetTextBeforeCursor");
        text = u"";
        if (!IsCorrectParam(number)) {
            return ErrorCode::ERROR_CONTROLLER_INVOKING_FAILED;
        }
        int32_t startPos = (number <= mSelectNewBegin ? (mSelectNewBegin - number) : 0);
        int32_t length = (number <= mSelectNewBegin ? number : mSelectNewBegin);
        text = mTextString.substr(startPos, length);
        return ErrorCode::NO_ERROR;
    }

    int32_t InputMethodController::GetTextAfterCursor(int32_t number, std::u16string &text)
    {
        IMSA_HILOGI("InputMethodController::GetTextAfterCursor");
        text = u"";
        if (!IsCorrectParam(number)) {
            return ErrorCode::ERROR_CONTROLLER_INVOKING_FAILED;
        }
        text = mTextString.substr(mSelectNewEnd, number);
        return ErrorCode::NO_ERROR;
    }

    bool InputMethodController::dispatchKeyEvent(std::shared_ptr<MMI::KeyEvent> keyEvent)
    {
        IMSA_HILOGI("InputMethodController::start");
        if (isStopInput) {
            IMSA_HILOGD("InputMethodController::dispatchKeyEvent isStopInput");
            return false;
        }
        std::shared_ptr<IInputMethodAgent> agent = GetInputMethodAgent();
        if (agent == nullptr) {
            IMSA_HILOGI("InputMethodController::dispatchKeyEvent agent is nullptr");
            return false;
        }
        MessageParcel data;
        if (!(data.WriteInterfaceToken(agent->GetDescriptor())
            && data.WriteInt32(keyEvent->GetKeyCode())
            && data.WriteInt32(keyEvent->GetKeyAction()))) {
            return false;
        }

        return agent->DispatchKeyEvent(data);
    }

    int32_t InputMethodController::GetEnterKeyType(int32_t &keyType)
    {
        IMSA_HILOGI("InputMethodController::GetEnterKeyType");
        keyType = enterKeyType_;
        return ErrorCode::NO_ERROR;
    }

    int32_t InputMethodController::GetInputPattern(int32_t &inputpattern)
    {
        IMSA_HILOGI("InputMethodController::GetInputPattern");
        inputpattern = inputPattern_;
        return ErrorCode::NO_ERROR;
    }

    void InputMethodController::SetCallingWindow(uint32_t windowId)
    {
        if (isStopInput) {
            IMSA_HILOGD("InputMethodController::SetCallingWindow isStopInput");
            return;
        }
        IMSA_HILOGI("InputMethodController::SetCallingWindow windowId = %{public}d", windowId);
        std::shared_ptr<IInputMethodAgent> agent = GetInputMethodAgent();
        if (agent == nullptr) {
            IMSA_HILOGI("InputMethodController::SetCallingWindow agent is nullptr");
            return;
        }
        agent->SetCallingWindow(windowId);
    }

    void InputMethodController::SetInputMethodAgent(sptr<IRemoteObject> &object)
    {
        IMSA_HILOGI("run in SetInputMethodAgent");
        std::lock_guard<std::mutex> lock(agentLock_);
        std::shared_ptr<IInputMethodAgent> agent = std::make_shared<InputMethodAgentProxy>(object);
        if (agent == nullptr) {
            IMSA_HILOGI("InputMethodController::SetInputMethodAgent agent is nullptr");
            return;
        }
        mAgent = agent;
    }

    std::shared_ptr<IInputMethodAgent> InputMethodController::GetInputMethodAgent()
    {
        std::lock_guard<std::mutex> lock(agentLock_);
        return mAgent;
    }

    int32_t InputMethodController::ShowSoftKeyboard()
    {
        IMSA_HILOGI("InputMethodController ShowSoftKeyboard");
        auto proxy = GetSystemAbilityProxy();
        if (proxy == nullptr) {
            IMSA_HILOGE("proxy is nullptr");
            return ErrorCode::ERROR_EX_NULL_POINTER;
        }
        return proxy->ShowCurrentInput();
    }

    int32_t InputMethodController::HideSoftKeyboard()
    {
        IMSA_HILOGI("InputMethodController HideSoftKeyboard");
        auto proxy = GetSystemAbilityProxy();
        if (proxy == nullptr) {
            IMSA_HILOGE("proxy is nullptr");
            return ErrorCode::ERROR_EX_NULL_POINTER;
        }
        return proxy->HideCurrentInput();
    }

    int32_t InputMethodController::StopInputSession()
    {
        IMSA_HILOGI("InputMethodController HideSoftKeyboard");
        isStopInput = true;
        auto proxy = GetSystemAbilityProxy();
        if (proxy == nullptr) {
            IMSA_HILOGE("proxy is nullptr");
            return ErrorCode::ERROR_EX_NULL_POINTER;
        }
        return proxy->StopInputSession();
    }

    int32_t InputMethodController::ShowOptionalInputMethod()
    {
        IMSA_HILOGI("InputMethodController::ShowOptionalInputMethod");
        auto proxy = GetSystemAbilityProxy();
        if (proxy == nullptr) {
            IMSA_HILOGE("proxy is nullptr");
            return ErrorCode::ERROR_EX_NULL_POINTER;
        }
        return proxy->DisplayOptionalInputMethod();
    }

    int32_t InputMethodController::ListInputMethodSubtype(const Property &property, std::vector<SubProperty> &subProps)
    {
        IMSA_HILOGI("InputMethodController::ListInputMethodSubtype");
        auto proxy = GetSystemAbilityProxy();
        if (proxy == nullptr) {
            IMSA_HILOGE("proxy is nullptr");
            return ErrorCode::ERROR_EX_NULL_POINTER;
        }
        return proxy->ListInputMethodSubtype(property.name, subProps);
    }

    int32_t InputMethodController::ListCurrentInputMethodSubtype(std::vector<SubProperty> &subProps)
    {
        IMSA_HILOGI("InputMethodController::ListCurrentInputMethodSubtype");
        auto proxy = GetSystemAbilityProxy();
        if (proxy == nullptr) {
            IMSA_HILOGE("proxy is nullptr");
            return ErrorCode::ERROR_EX_NULL_POINTER;
        }
        return proxy->ListCurrentInputMethodSubtype(subProps);
    }

    int32_t InputMethodController::SwitchInputMethod(const std::string &name, const std::string &subName)
    {
        IMSA_HILOGI("InputMethodController::SwitchInputMethod");
        auto proxy = GetSystemAbilityProxy();
        if (proxy == nullptr) {
            IMSA_HILOGE("proxy is nullptr");
            return ErrorCode::ERROR_EX_NULL_POINTER;
        }
        return proxy->SwitchInputMethod(name, subName);
    }
} // namespace MiscServices
} // namespace OHOS
