/**
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

import bundle from '@ohos.bundle'
import { CMModelErrorCode } from '../model/CertMangerModel'

export class BundleNameModel {
  async getAppInfoList(AppUid, callback) {
    try {
      let AppInfo = {
        AppImage: "",
        AppName: "",
      }
      let AppBundleName = await bundle.getNameForUid(AppUid)
      var bundleContext = await globalThis.certManagerAbilityContext.createBundleContext(AppBundleName)

      let labelId = Number(bundleContext.applicationInfo.labelId)
      let iconId = Number(bundleContext.applicationInfo.iconId)

      AppInfo.AppImage = await bundleContext.resourceManager.getMediaBase64(iconId)

      AppInfo.AppName = await bundleContext.resourceManager.getString(labelId)

      console.log("AppName1111111111", JSON.stringify(AppInfo));

      callback(CMModelErrorCode.CM_MODEL_ERROR_SUCCESS, AppInfo)
    } catch (err) {
      console.log("getAppNameListErr", JSON.stringify(err.code));
      callback(CMModelErrorCode.CM_MODEL_ERROR_EXCEPTION, null);
    }
  }
}

let bundleNameModel = new BundleNameModel();

export default bundleNameModel as BundleNameModel;
