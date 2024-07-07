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
import Ability from '@ohos.app.ability.UIAbility'
import Want from '@ohos.application.Want';
import AbilityConstant from "@ohos.app.ability.AbilityConstant";
import Log from '../../../../../../../common/src/main/ets/default/Log';
import AbilityManager from '../../../../../../../common/src/main/ets/default/abilitymanager/abilityManager';
import display from '@ohos.display';

const TAG = 'NotificationManagement-MainAbility';

export default class MainAbility extends Ability {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    Log.showInfo(TAG, 'MainAbility onCreate');
    AbilityManager.setAbilityContext(AbilityManager.ABILITY_NAME_NOTIFICATION_MANAGEMENT, this.context);
    globalThis[AbilityManager.ABILITY_NAME_NOTIFICATION_MANAGEMENT + '_want'] = want;
  }

  onDestroy(): void {
    Log.showInfo(TAG, 'MainAbility onDestroy');
  }

  onWindowStageCreate(windowStage): void {
    Log.showInfo(TAG, 'MainAbility onWindowStageCreate');
    windowStage.setUIContent(this.context, 'pages/notificationManagenment', null);
  }

  onWindowStageDestroy(): void {
    Log.showInfo(TAG, 'MainAbility onWindowStageDestroy');
  }

  onForeground(): void {
    Log.showInfo(TAG, 'MainAbility onForeground');
  }

  onBackground(): void {
    Log.showInfo(TAG, 'MainAbility onBackground');
  }
};
