/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ABILITY_RUNTIME_ABILITY_WINDOW_H
#define OHOS_ABILITY_RUNTIME_ABILITY_WINDOW_H

#include <map>

#include "nocopyable.h"
#include "window.h"
#include "window_option.h"
#include "window_scene.h"
#include "foundation/ability/ability_runtime/interfaces/kits/native/ability/ability_runtime/ability_context.h"

#ifdef SUPPORT_GRAPHICS
#include "pixel_map.h"
#endif

namespace OHOS {
namespace AppExecFwk {
class IAbilityEvent;
class Ability;
class AbilityHandler;
class AbilityWindow : public NoCopyable {
public:
    AbilityWindow();
    virtual ~AbilityWindow();

    /**
     * @brief Init the AbilityWindow object.
     *
     * @param handler The EventHandler of the Ability the AbilityWindow belong.
     */
    void Init(std::shared_ptr<AbilityHandler> &handler, std::shared_ptr<Ability> ability);

    bool InitWindow(std::shared_ptr<AbilityRuntime::AbilityContext> &abilityContext,
        sptr<Rosen::IWindowLifeCycle> &listener, int32_t displayId, sptr<Rosen::WindowOption> option, bool isPrivacy);

    /**
     * @brief Called when this ability is background.
     *
     * @param sceneFlag flag for screen, lock or unlock, power on or off.
     */
    void OnPostAbilityBackground(uint32_t sceneFlag);

    /**
     * @brief Called when this ability is foreground.
     *
     * @param sceneFlag flag for screen, lock or unlock, power on or off.
     */
    void OnPostAbilityForeground(uint32_t sceneFlag);

    /**
     * @brief Called when this ability is stopped.
     *
     */
    void OnPostAbilityStop();

    /**
     * @brief Get the window belong to the ability.
     *
     * @return Returns a Window object pointer.
     */
    const sptr<Rosen::Window> GetWindow();

#ifdef SUPPORT_GRAPHICS
    /**
     * @brief Set mission label of this ability.
     *
     * @param label the label of this ability.
     * @return Returns ERR_OK if success.
     */
    virtual ErrCode SetMissionLabel(const std::string &label);

    /**
     * @brief Set mission icon of this ability.
     *
     * @param icon the icon of this ability.
     * @return Returns ERR_OK if success.
     */
    virtual ErrCode SetMissionIcon(const std::shared_ptr<OHOS::Media::PixelMap> &icon);
#endif

private:
    std::shared_ptr<AbilityHandler> handler_ = nullptr;
    std::weak_ptr<IAbilityEvent> ability_;
    std::shared_ptr<Rosen::WindowScene> windowScene_;
    bool isWindowAttached = false;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_ABILITY_WINDOW_H
