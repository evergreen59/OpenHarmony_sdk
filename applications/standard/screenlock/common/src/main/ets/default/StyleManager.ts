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

import Log from './Log';

const TAG = 'Common-StyleManager';

export class StyleManager {
    mAbilityPageName: string = '';

    setAbilityPageName(name: string): void{
        Log.showDebug(TAG, `setAbilityPageName, name: ${name}`);
        this.mAbilityPageName = name;
    }

    getStyle(key: string, generateDefaultFunction: Function) {
        let newKey = this.mAbilityPageName + '-' + key;
        if (!AppStorage.Has(newKey)) {
            let defaultStyle = generateDefaultFunction();
            AppStorage.SetOrCreate(newKey, defaultStyle);
            Log.showDebug(TAG, `Create storageKey of ${newKey}`);
        }
        return AppStorage.Get(newKey);
    }
}

let styleManager = new StyleManager();

export default styleManager as StyleManager;