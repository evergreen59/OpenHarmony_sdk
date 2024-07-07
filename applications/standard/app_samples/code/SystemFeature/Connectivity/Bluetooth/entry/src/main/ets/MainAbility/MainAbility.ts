/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
import logger from '../Model/Logger'
import abilityAccessCtrl, { Permissions } from '@ohos.abilityAccessCtrl'

const TAG: string = 'MainAbility'

export default class MainAbility extends UIAbility {
  async onCreate(want, launchParam) {
    logger.info(TAG, `[Demo] MainAbility onCreate`)

    const PERMISSIONS: Array<Permissions> = [
      'ohos.permission.USE_BLUETOOTH',
      'ohos.permission.DISCOVER_BLUETOOTH',
      'ohos.permission.LOCATION',
      'ohos.permission.MANAGE_BLUETOOTH',
      'ohos.permission.APPROXIMATELY_LOCATION'
    ]

    let atManager = abilityAccessCtrl.createAtManager()
    try {
      atManager.requestPermissionsFromUser(this.context, PERMISSIONS).then((data) => {
        logger.info(TAG, `data: ${JSON.stringify(data)}`)
      }).catch((err) => {
        logger.info(TAG, `err: ${JSON.stringify(err)}`)
      })
    } catch (err) {
      logger.info(TAG, `catch err->${JSON.stringify(err)}`);
    }
  }

  onDestroy() {
    logger.info(TAG, `[Demo] MainAbility onDestroy`)
  }

  onWindowStageCreate(windowStage) {
    logger.info(TAG, `[Demo] MainAbility onWindowStageCreate`)
    windowStage.setUIContent(this.context, "pages/Index", null)
  }

  onWindowStageDestroy() {
    logger.info(TAG, `[Demo] MainAbility onWindowStageDestroy`)
  }

  onForeground() {
    logger.info(TAG, `[Demo] MainAbility onForeground`)
  }

  onBackground() {
    logger.info(TAG, `[Demo] MainAbility onBackground`)
  }
}
