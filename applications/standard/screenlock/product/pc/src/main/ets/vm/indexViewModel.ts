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

import Log from '../../../../../../common/src/main/ets/default/Log';
import ScreenlockStyle, {LockStyleMode
} from '../../../../../../features/screenlock/src/main/ets/com/ohos/model/screenlockStyle'
import ScreenLockService from '../../../../../../features/screenlock/src/main/ets/com/ohos/model/screenLockService'

const TAG = 'ScreenLock-IndexViewModel'

export default class IndexViewModel {
    mode: LockStyleMode = LockStyleMode.SlideScreenLock

    ViewModelInit(): void{
        this.serviceInit();
        this.mode = this.getMode();
        Log.showInfo(TAG, `ViewModelInit this.mode=${this.mode}`);
    }

    serviceInit() {
        ScreenLockService.init();
    }

    getMode(): number {
        return ScreenlockStyle.readMode()
    }

    onPageShow() {
        Log.showInfo(TAG, `onPageShow`)
        ScreenLockService.notifyDrawDone();
    }

    ViewModelDestroy() {
        Log.showDebug(TAG, 'ViewModelDestroy');
        ScreenLockService.destroy()
    }
}