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

import ServiceExtensionAbility from '@ohos.app.ability.ServiceExtensionAbility';
import windowManager from '@ohos.window';
import display from '@ohos.display';
import Log from '../../../../../../common/src/main/ets/default/Log';
import Constants from '../../../../../../features/screenshot/src/main/ets/com/ohos/common/constants';
import ScreenShotModel from '../../../../../../features/screenshot/src/main/ets/com/ohos/model/screenShotModel';

const TAG = "ScreenShot-ScreenShotServiceAbility";
const INDEX_PAGE = "pages/index";
const ZOOM_RATIO = 0.4;
const SHOT_WINDOW_TYPE = 2111;
const WINDOW_Y = 300;

class ServiceExtAbility extends ServiceExtensionAbility {
    onCreate(want) {
        Log.showInfo(TAG, 'api8New onCreate, want:' + want.abilityName);
        globalThis.shotScreenContext = this.context;
        windowManager.create(this.context, Constants.WIN_NAME, SHOT_WINDOW_TYPE).then((win) => {
            Log.showInfo(TAG, "create window finish!")
            win.moveTo(0, WINDOW_Y).then(() => {
                Log.showInfo(TAG, " window moveTo finish")
                display.getDefaultDisplay().then(dis => {
                    Log.showInfo(TAG, " dis.width = " + dis.width + " dis.height = " + dis.height)
                    win.resetSize(dis.width * ZOOM_RATIO, dis.height * ZOOM_RATIO).then(() => {
                        Log.showInfo(TAG, " window reset size finish")
                        win.loadContent(INDEX_PAGE).then(() => {
                            ScreenShotModel.shotScreen();
                            Log.showInfo(TAG, "then begin window loadContent in then! ");
                        })
                    })
                })
            })
        }, (error) => {
            Log.showInfo(TAG, " window createFailed, error.code = " + error.code)
        })
        Log.showInfo(TAG, " after window create")
    }

    onDestroy() {
        Log.showInfo(TAG, `onDestroy`);
    }
}

export default ServiceExtAbility