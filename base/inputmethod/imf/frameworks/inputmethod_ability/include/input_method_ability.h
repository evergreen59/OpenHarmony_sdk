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

#ifndef FRAMEWORKS_INPUTMETHOD_ABILITY_INCLUDE_INPUT_METHOD_ABILITY_H
#define FRAMEWORKS_INPUTMETHOD_ABILITY_INCLUDE_INPUT_METHOD_ABILITY_H

#include <atomic>
#include <thread>

#include "i_input_control_channel.h"
#include "i_input_data_channel.h"
#include "i_input_method_agent.h"
#include "i_input_method_core.h"
#include "input_attribute.h"
#include "input_channel.h"
#include "input_control_channel_proxy.h"
#include "input_data_channel_proxy.h"
#include "input_method_core_stub.h"
#include "input_method_engine_listener.h"
#include "input_method_system_ability_proxy.h"
#include "iremote_object.h"
#include "keyboard_listener.h"
#include "message.h"
#include "message_handler.h"
#include "utils.h"
#include "visibility.h"

namespace OHOS {
namespace MiscServices {
struct InputStartNotifier {
    bool isNotify{ false };
    bool isShowKeyboard{};
    SubProperty subProperty{};
};
    class MessageHandler;
    class InputMethodAbility : public RefBase {
    public:
        InputMethodAbility();
        ~InputMethodAbility();
        IMF_API static sptr<InputMethodAbility> GetInstance();
        IMF_API int32_t SetCoreAndAgent();
        IMF_API int32_t InsertText(const std::string text);
        IMF_API void setImeListener(std::shared_ptr<InputMethodEngineListener> imeListener);
        IMF_API void setKdListener(std::shared_ptr<KeyboardListener> kdListener);
        IMF_API int32_t DeleteForward(int32_t length);
        IMF_API int32_t DeleteBackward(int32_t length);
        IMF_API int32_t HideKeyboardSelf();
        IMF_API int32_t GetTextBeforeCursor(int32_t number, std::u16string &text);
        IMF_API int32_t GetTextAfterCursor(int32_t number, std::u16string &text);
        IMF_API int32_t SendFunctionKey(int32_t funcKey);
        IMF_API int32_t MoveCursor(int32_t keyCode);
        IMF_API bool DispatchKeyEvent(int32_t keyCode, int32_t keyStatus);
        IMF_API void SetCallingWindow(uint32_t windowId);
        IMF_API int32_t GetEnterKeyType(int32_t &keyType);
        IMF_API int32_t GetInputPattern(int32_t &inputPattern);
        IMF_API void OnImeReady();

    private:
        std::thread workThreadHandler;
        MessageHandler *msgHandler;
        InputAttribute editorAttribute;
        InputChannel *writeInputChannel;
        bool stop_;
        int32_t KEYBOARD_HIDE = 1;
        int32_t KEYBOARD_SHOW = 2;

        std::mutex controlChannelLock_;
        std::shared_ptr<InputControlChannelProxy> controlChannel_ = nullptr;

        std::mutex dataChannelLock_;
        std::shared_ptr<InputDataChannelProxy> dataChannel_ = nullptr;
        std::shared_ptr<InputMethodEngineListener> imeListener_;
        std::shared_ptr<KeyboardListener> kdListener_;
        static std::mutex instanceLock_;

        static sptr<InputMethodAbility> instance_;
        sptr<InputMethodSystemAbilityProxy> mImms;
        struct ServiceDeathRecipient : public IRemoteObject::DeathRecipient {
            std::shared_ptr<InputMethodEngineListener> listener{ nullptr };
            void OnRemoteDied(const wptr<IRemoteObject> &object) override;
            std::string currentIme_;
        };
        sptr<ServiceDeathRecipient> deathRecipientPtr_{ nullptr };
        sptr<InputMethodSystemAbilityProxy> GetImsaProxy();

        void SetInputDataChannel(sptr<IRemoteObject> &object);
        std::shared_ptr<InputDataChannelProxy> GetInputDataChannel();
        void SetInputControlChannel(sptr<IRemoteObject> &object);
        std::shared_ptr<InputControlChannelProxy> GetInputControlChannel();

        void Initialize();
        void WorkThread();
        void QuitWorkThread();

        void OnShowKeyboard(Message *msg);
        void OnHideKeyboard(Message *msg);
        void OnInitInputControlChannel(Message *msg);
        void OnSetSubtype(Message *msg);

        void OnCursorUpdate(Message *msg);
        void OnSelectionChange(Message *msg);
        void ShowInputWindow(bool isShowKeyboard, const SubProperty &subProperty);
        void DissmissInputWindow();
        bool isImeReady_{ false };
        InputStartNotifier notifier_;
        std::atomic_bool isBound_{ false };
    };
} // namespace MiscServices
} // namespace OHOS
#endif // FRAMEWORKS_INPUTMETHOD_ABILITY_INCLUDE_INPUT_METHOD_ABILITY_H
