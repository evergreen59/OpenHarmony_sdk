/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import Ability from '@ohos.app.ability.UIAbility'

export default class MainAbility extends Ability {
    onCreate(want, launchParam) {
        console.log("[DataShareServer]: MainAbility onCreate")
        globalThis.abilityWant = want;
    }

    onDestroy() {
        console.log("[DataShareServer]: MainAbility onDestroy")
    }

    onWindowStageCreate(windowStage) {
        // Main window is created, set main page for this ability
        console.log("[DataShareServer]: MainAbility onWindowStageCreate")

        windowStage.loadContent("pages/index", (err, data) => {
            if (err.code) {
                console.error("[DataShareServer]: Failed to load the content. Cause: " + JSON.stringify(err));
                return;
            }
            console.log("[DataShareServer]: Succeeded in loading the content. Data: " + JSON.stringify(data))
        });
    }

    onWindowStageDestroy() {
        // Main window is destroyed, release UI related resources
        console.log("[DataShareServer]: MainAbility onWindowStageDestroy")
    }

    onForeground() {
        // Ability has brought to foreground
        console.log("[DataShareServer]: MainAbility onForeground")
    }

    onBackground() {
        // Ability has back to background
        console.log("[DataShareServer]: MainAbility onBackground")
    }
};
