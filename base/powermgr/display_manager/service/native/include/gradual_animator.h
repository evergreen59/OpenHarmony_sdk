/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef DISPLAYMGR_GRADUAL_ANIMATOR_H
#define DISPLAYMGR_GRADUAL_ANIMATOR_H

#include <memory>
#include <string>
#include <cstdint>
#include <iosfwd>
#include <event_handler.h>
#include <event_runner.h>
#include "inner_event.h"

namespace OHOS {
namespace DisplayPowerMgr {
class AnimateCallback {
public:
    virtual ~AnimateCallback() = default;
    virtual void OnStart() = 0;
    virtual void OnChanged(uint32_t currentValue) = 0;
    virtual void OnEnd() = 0;
    virtual void DiscountBrightness(double discount) = 0;
};

class GradualAnimator : public std::enable_shared_from_this<GradualAnimator> {
public:
    GradualAnimator(const std::string& name, std::shared_ptr<AnimateCallback>& callback);
    virtual ~GradualAnimator() = default;
    void StartAnimation(uint32_t from, uint32_t to, uint32_t duration);
    void StopAnimation();
    bool IsAnimating() const;
    uint32_t GetAnimationUpdateTime() const;
private:
    static const uint32_t DEFAULT_UPDATE_TIME = 30;
    static const uint32_t EVENT_STEP = 1;
    static const int32_t STRIDE_ABSOLUTE_MIN = 1;
    class AnimatorHandler : public AppExecFwk::EventHandler {
    public:
        AnimatorHandler(const std::shared_ptr<AppExecFwk::EventRunner>& runner,
            const std::shared_ptr<GradualAnimator>& owner);
        ~AnimatorHandler() = default;
        void ProcessEvent(const AppExecFwk::InnerEvent::Pointer& event) override;
    private:
        std::weak_ptr<GradualAnimator> owner_;
    };
    void NextStep();
    std::string name_;
    std::shared_ptr<AppExecFwk::EventRunner> eventRunner_;
    std::shared_ptr<AnimatorHandler> handler_;
    std::shared_ptr<AnimateCallback> callback_;
    bool animating_ = false;
    uint32_t fromBrightness_;
    uint32_t toBrightness_;
    uint32_t duration_;
    uint32_t updateTime_;
    uint32_t totalSteps_;
    int32_t stride_;
    uint32_t currentBrightness_;
    uint32_t currentStep_;
};
} // namespace DisplayPowerMgr
} // namespace OHOS
#endif // DISPLAYMGR_GRADUAL_ANIMATOR_H