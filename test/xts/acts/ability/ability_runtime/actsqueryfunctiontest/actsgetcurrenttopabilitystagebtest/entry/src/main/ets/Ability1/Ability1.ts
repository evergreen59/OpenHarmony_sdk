/*
* Copyright (c) 2022 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
import Ability from '@ohos.app.ability.UIAbility'
import commonEvent from '@ohos.commonEvent'
function publishCallBackOne() {
    console.debug("====>Publish CallBack GetCurrentTopAbility");
}

export default class MainAbility extends Ability {
    onCreate(want, launchParam) {
        // Ability is creating, initialize resources for this ability
        console.log("Ability1 onCreate")
        globalThis.abilityWant = want;
    }

    onDestroy() {
        // Ability is destroying, release resources for this abilitya
        console.log("Ability1 onDestroy")
    }

    onWindowStageCreate(windowStage) {
        // Main window is created, set main page for this ability
        console.log("Ability1 onWindowStageCreate")

        globalThis.ability3Context = this.context
        windowStage.setUIContent(this.context, "pages/index1", null)
        windowStage.on('windowStageEvent', (data) => {
          console.log(`ActsGetCurrentTopAbilityStagebTest onWindwoStage is : ${JSON.stringify(data)}`)
          if (data == 2 ) {
            console.log(`ActsGetCurrentTopAbilityStagebTest getWindowStageActive is sucess`)
            commonEvent.publish("GetCurrentTopAbility", publishCallBackOne);
          }
        })
    }

    onWindowStageDestroy() {
        // Main window is destroyed, release UI related resources
        console.log("Ability1 onWindowStageDestroy")
    }

    onForeground() {
        // Ability has brought to foreground
        console.log("Ability1 onForeground")
    }


    onBackground() {
        // Ability has back to background
        console.log("Ability1 onBackground")
    }
}

