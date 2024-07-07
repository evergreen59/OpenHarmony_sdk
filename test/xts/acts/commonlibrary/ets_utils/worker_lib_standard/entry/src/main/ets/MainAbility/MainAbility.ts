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
import Ability from '@ohos.app.ability.UIAbility';

var printLog = 'Worker:MainAbility:'
export default class MainAbility extends Ability {
    onCreate(want, launchParam) {
        console.info(printLog + 'onCreate')
    }

    onDestroy() {
        console.info(printLog + 'onDestroy')
    }

    onWindowStageCreate(windowStage) {
        // Main window is created, set main page for this ability
        console.info(printLog + 'onWindowStageCreate')

        windowStage.loadContent("MainAbility/pages/index", (err, data) => {
            if (err.code) {
                console.error('Failed to load the content. Cause:' + JSON.stringify(err));
                return;
            }
            console.info('Succeeded in loading the content. Data: ' + JSON.stringify(data))
        });
    }

    onWindowStageDestroy() {
        console.info(printLog + 'onWindowStageDestroy')
    }

    onForeground() {
        console.info(printLog + 'onForeground')
    }

    onBackground() {
        console.info(printLog + 'onBackground')
    }
};
