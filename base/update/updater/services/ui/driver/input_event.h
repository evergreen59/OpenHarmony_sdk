/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef UPDATER_UI_INPUT_EVENT_H
#define UPDATER_UI_INPUT_EVENT_H
#include <linux/input.h>
#include "dock/key_input_device.h"
#include "dock/pointer_input_device.h"
#include "input_manager.h"
#include "macros.h"

namespace Updater {
class TouchInputDevice : public OHOS::PointerInputDevice {
    DISALLOW_COPY_MOVE(TouchInputDevice);
public:
    TouchInputDevice() = default;
    virtual ~TouchInputDevice() = default;
    static TouchInputDevice &GetInstance();
    bool Read(OHOS::DeviceData &data) override;
};

class KeysInputDevice : public OHOS::KeyInputDevice {
    DISALLOW_COPY_MOVE(KeysInputDevice);
public:
    KeysInputDevice() = default;
    virtual ~KeysInputDevice() = default;
    static KeysInputDevice &GetInstance();
    bool Read(OHOS::DeviceData &data) override;
};

int HandleInputEvent(const struct input_event *iev);
void ReportEventPkgCallback(const InputEventPackage **pkgs, const uint32_t count, uint32_t devIndex);
int HdfInit();
} // namespace Updater
#endif // UPDATER_UI_INPUT_EVENT_H
