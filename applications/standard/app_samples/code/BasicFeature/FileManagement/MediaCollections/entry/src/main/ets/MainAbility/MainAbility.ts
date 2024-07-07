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

import UIAbility from '@ohos.app.ability.UIAbility'
import display from '@ohos.display'

export default class MainAbility extends UIAbility {
  async onCreate(want, launchParam) {
    console.log("[Demo] MainAbility onCreate")
  }

  onDestroy() {
    console.log("[Demo] MainAbility onDestroy")
  }

  async onWindowStageCreate(windowStage) {
    console.log("[Demo] MainAbility onWindowStageCreate")
    let abilityDisplay = await display.getDefaultDisplaySync()
    let abilityDisplayWidth = abilityDisplay.width
    const displayWidth: number = 2500
    if (abilityDisplayWidth > displayWidth) {
      windowStage.loadContent("pages/index", (err, data) => {
        if (err.code) {
          console.error('Failed to load the content. Cause:' + JSON.stringify(err));
          return;
        }
        console.info('Succeeded in loading the content. Data: ' + JSON.stringify(data))
      });
    } else {
      windowStage.loadContent("pages/phoneMain", (err, data) => {
        if (err.code) {
          console.error('Failed to load the content. Cause:' + JSON.stringify(err));
          return;
        }
        console.info('Succeeded in loading the content. Data: ' + JSON.stringify(data))
      });
    }

  }

  onWindowStageDestroy() {
    console.log("[Demo] MainAbility onWindowStageDestroy")
  }

  onForeground() {
    console.log("[Demo] MainAbility onForeground")
  }

  onBackground() {
    console.log("[Demo] MainAbility onBackground")
  }
}
