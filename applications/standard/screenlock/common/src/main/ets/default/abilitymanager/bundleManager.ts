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

import BundleMgr from "@ohos.bundle";
import Context from "application/ServiceExtensionContext";
import Log from "../Log";
import SwitchUserManager from "../SwitchUserManager";

const TAG = "BRManager";

export default class BundleManager {
    static async getResourceManager(tag: string, context: Context, bundleName: string) {
        Log.showInfo(TAG, `getResourceManager from: ${tag}`);
        let bundleContext = await context.createBundleContext(bundleName)
        return await bundleContext.resourceManager;
    }

    static async getBundleInfo(tag: string, bundleName: string, getInfo:
    any, requestId?: number) {
        let userInfo = {
            userId: requestId ?? (await SwitchUserManager.getInstance().getCurrentUserInfo()).userId,
        };
        Log.showDebug(TAG, `getBundleInfo from: ${tag}, userId: ${userInfo.userId}`);
        return await BundleMgr.getBundleInfo(bundleName, getInfo, userInfo);
    }
}