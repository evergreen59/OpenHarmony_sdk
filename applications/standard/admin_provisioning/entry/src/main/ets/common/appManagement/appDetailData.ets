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

import bundle from '@ohos.bundle.bundleManager';
import accountManager from '../accountManager';
import baseData from '../baseData';
import logger from '../logger'
import utils from '../utils'

let icon_default = $r('app.media.icon');
const TAG = 'AppDetailData';

const permissionsDetailList: Array<any> = [
  {
    "permissionName": 'ohos.permission.ENTERPRISE_SET_DATETIME',
    "permissionLabel": $r('app.string.permissionLabel'),
    "permissionDescription": $r('app.string.permissionDescription')
  },
];

export class AppDetailData {
  public ret = { val: true };

  async checkAppItem(elementNameVal) {
    logger.info(TAG, 'checkAppItem in bundleName:' + elementNameVal.bundleName + ' | abilityName:'
      + elementNameVal.abilityName);
    let want = {
      "bundleName": elementNameVal.bundleName,
      "abilityName": elementNameVal.abilityName
    };
    let userId = { localId : 0 };
    let retVal = await accountManager.getAccountUserId(userId);
    if (!retVal) {
      logger.warn(TAG, 'checkAppItem getAccountUserId fail!');
      this.ret.val = false;
      return this.ret;
    }
    let data;
    try {
      data = await bundle.queryExtensionAbilityInfo(want, bundle.ExtensionAbilityType.ENTERPRISE_ADMIN,
        bundle.ExtensionAbilityFlag.GET_EXTENSION_ABILITY_INFO_WITH_APPLICATION, userId.localId);
    } catch (e) {
      logger.error(TAG, 'checkAppItem queryExtensionAbilityInfo try fail! ' + JSON.stringify(e));
    }
    if (!utils.isValid(data) || data.length <= 0) {
      logger.warn(TAG, 'checkAppItem queryExtensionAbilityInfo fail! data is null');
      this.ret.val = false;
      return this.ret;
    }
    logger.info(TAG, 'checkAppItem success');
    return this.ret;
  }

  async getBundleInfoItem(bundleName: string, appInfo) {
    logger.info(TAG, 'getBundleInfoItem in bundleName:' + bundleName);
    let userId = { localId : 0 };
    let retVal = await accountManager.getAccountUserId(userId);
    if (!retVal) {
      logger.warn(TAG, 'getBundleInfoItem getAccountUserId fail!');
      return;
    }
    let data = await bundle.getBundleInfo(bundleName, bundle.BundleFlag.GET_BUNDLE_INFO_WITH_REQUESTED_PERMISSION |
      bundle.BundleFlag.GET_BUNDLE_INFO_WITH_APPLICATION, userId.localId);
    await this.getResourceItem(bundleName, data, appInfo);
    await this.getPermissionList(data, appInfo);
    logger.info(TAG, 'getBundleInfoItem out');
  }

  async getAppInfoLabel(resMgr, id) {
    let label = await resMgr.getString(id);
    logger.info(TAG, 'getAppInfoLabel start label:' + label);
    if (!utils.isValid(label) || label === baseData.EMPTY_STR) {
      return baseData.EMPTY_STR;
    }
    return label;
  }

  async getAppInfoIcon(resMgr, id) {
    let iconVal = await resMgr.getMediaBase64(id);
    logger.info(TAG, 'getAppInfoIcon start iconVal:' + iconVal);
    if (!utils.isValid(iconVal) || iconVal === baseData.EMPTY_STR) {
      return baseData.EMPTY_STR;
    }
    return iconVal;
  }

  async getResourceItem(bundleName, data, appInfo) {
    logger.info(TAG, 'getResourceItem getResourceManager in');
    let bundleContext = await globalThis.adminProvisioningContext.createBundleContext(bundleName);
    let resMgr = await bundleContext.resourceManager;
    let appInfoTemp = data.appInfo;
    let label = '';
    let iconVal = '';

    if (appInfoTemp.labelId > 0) {
      label = await this.getAppInfoLabel(resMgr, appInfoTemp.labelId);
      logger.info(TAG, 'getResourceItem success appInfo.label:' + label);
    } else {
      label = appInfoTemp.label;
      logger.info(TAG, 'getResourceItem defaults appInfo.label:' + label);
    }

    if (appInfoTemp.iconId > 0) {
      iconVal = await this.getAppInfoIcon(resMgr, appInfoTemp.iconId);
      if (iconVal === baseData.EMPTY_STR) {
        appInfo.appIcon = icon_default;
        appInfo.appTitle = label;
      }
      appInfo.appIcon = iconVal;
      appInfo.appTitle = label;
      AppStorage.SetOrCreate('applicationInfo', appInfo);
    }
    logger.info(TAG, 'getResourceItem getResourceManager out');
  }

  async terminateAbilityPage() {
    logger.info(TAG, 'terminateAbilityPage in:');
    await globalThis.adminProvisioningContext.terminateSelf();
  }

  getPermissionInfoVal(permissionName): number {
    for (let i = 0; i < permissionsDetailList.length; i++) {
      if (permissionsDetailList[i].permissionName === permissionName) {
        return i;
      }
    }
    return -1;
  }

  async getPermissionList(data, appInfo) {
    let permissions = data.reqPermissionDetails;
    if (permissions !== null && permissions !== []) {
      logger.info(TAG, 'getPermissionList permission length:' + permissions.length);
      for (var i = 0; i < permissions.length; i++) {
        logger.info(TAG, 'getPermissionList permission is in val:' + permissions[i].name);
        let j = this.getPermissionInfoVal(permissions[i].name);
        if (j >= 0) {
          appInfo.appPermissionList.push({
            permissionName: permissionsDetailList[j].permissionName,
            permissionLabel: permissionsDetailList[j].permissionLabel,
            permissionDescription: permissionsDetailList[j].permissionDescription,
          });
        }
      }
      logger.info(TAG, 'getPermissionList permission appInfo.appPermissionList:'
        + JSON.stringify(appInfo.appPermissionList));
    }
    logger.info(TAG, 'getPermissionList permission out');
  }
}

let appDetailData = new AppDetailData();

export default appDetailData as AppDetailData;