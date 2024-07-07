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

#ifndef NWEB_INPUT_EVENT_CONSUMER_TEST_H
#define NWEB_INPUT_EVENT_CONSUMER_TEST_H

#include <memory>
#include "axis_event.h"
#include "key_event.h"
#include "nweb.h"
#include "pointer_event.h"
#include "window.h"

namespace OHOS::NWeb {
class NWebInputEventConsumerTest : public Rosen::IInputEventConsumer {
public:
    explicit NWebInputEventConsumerTest(std::shared_ptr<NWeb> nweb);
    ~NWebInputEventConsumerTest() override = default;
    bool OnInputEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) const override;
    bool OnInputEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) const override;
    bool OnInputEvent(const std::shared_ptr<MMI::AxisEvent>& axisEvent) const override;

private:
    void DispatchPointerEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const;
    void DispatchKeyEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const;

private:
    std::weak_ptr<NWeb> nwebweak_;
};
} // namespace OHOS::NWeb

#endif // NWEB_INPUT_EVENT_CONSUMER_TEST_H