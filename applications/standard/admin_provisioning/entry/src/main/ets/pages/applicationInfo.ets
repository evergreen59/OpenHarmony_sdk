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

import edmEnterpriseDeviceManager from '@ohos.enterprise.adminManager';
import router from '@ohos.router';
import appDetailData from '../common/appManagement/appDetailData'
import baseData from '../common/baseData';
import entryComponent from './component/entryComponent'
import headComponent from './component/headComponent';
import logger from '../common/logger'
import permissionListComponent from './component/permissionListComponent';
import utils from '../common/utils'

let appInfo;
let elementNameVal;
let isAdminTypeVal;
let isActiveState;
let enterInfo;
const TAG = 'ApplicationInfo';

@Entry
@Component
struct ApplicationInfo {
  @StorageLink('applicationInfo') applicationInfo: any = {};
  @State isShowActive: boolean = false;
  @State isEnableButton: boolean = false;
  @StorageLink('activeType') isAdminType: number = 0;
  @State deviceAdminActive: string = '';
  @State deviceAdminDisActive: string = '';
  @State enterpriseName: string = '';
  @State enterpriseDescription: string = '';

  build() {
    Column() {
      Column() {
        GridContainer({
          columns: utils.isLargeDevice(),
          sizeType: SizeType.Auto,
          gutter: '12vp',
          margin: '12vp'
        }) {
          Column() {
            headComponent({ headName: $r('app.string.adminprovisioning') });

            Row() {
              entryComponent({
                appIcon: this.applicationInfo.appIcon,
                appTitle: this.applicationInfo.appTitle,
              })
            }

            Column() {
              Text(this.enterpriseName)
                .height($r('app.float.wh_value_21'))
                .fontColor($r('sys.color.ohos_id_color_text_primary'))
                .fontSize($r('app.float.font_16'))
                .fontWeight(FontWeight.Medium)
                .textAlign(TextAlign.Start)
                .maxLines(3)
                .textOverflow({ overflow: TextOverflow.Ellipsis })
                .margin({
                  left: $r('app.float.wh_value_36'),
                  top: $r('app.float.wh_value_12'),
                });

              Text(this.enterpriseDescription)
                .height($r('app.float.wh_value_19'))
                .fontColor($r('sys.color.ohos_id_color_text_secondary'))
                .fontSize($r('app.float.font_14'))
                .fontWeight(FontWeight.Regular)
                .textAlign(TextAlign.Start)
                .maxLines(3)
                .textOverflow({ overflow: TextOverflow.Ellipsis })
                .margin({
                  left: $r('app.float.wh_value_36'),
                  top: $r('app.float.wh_value_2'),
                });
            }
            .width('100%')
            .alignItems(HorizontalAlign.Start)
            .align(Alignment.Start)

            Column() {
              Text(this.isShowActive ? this.deviceAdminActive : this.deviceAdminDisActive)
                .fontColor($r('sys.color.ohos_id_color_text_primary'))
                .fontSize($r('app.float.font_16'))
                .fontWeight(FontWeight.Medium)
                .textAlign(TextAlign.Start)
                .maxLines(3)
                .textOverflow({ overflow: TextOverflow.Ellipsis })
                .margin({
                  left: $r('app.float.wh_value_36'),
                  top: $r('app.float.wh_value_24'),
                });

              List() {
                ForEach(appInfo.appPermissionList, (item) => {
                  ListItem() {
                    permissionListComponent({
                      permissionName: item.permissionLabel,
                      permissionDescription: item.permissionDescription,
                    });
                  }
                }, item => JSON.stringify(item));
              }
            }
            .alignItems(HorizontalAlign.Start)
            .align(Alignment.Start)
          }
          .useSizeType({
            xs: { span: 8, offset: 0 }, sm: { span: 8, offset: 0 },
            md: { span: 8, offset: 0 }, lg: { span: 8, offset: 2 }
          })
        }
      }
      .layoutWeight(1)

      Column() {
        GridContainer({
          columns: utils.isLargeDevice(),
          sizeType: SizeType.Auto,
          gutter: '12vp',
          margin: '12vp'
        }) {
          Row() {
            Row()
              .useSizeType({
                xs: { span: 1, offset: 0 }, sm: { span: 1, offset: 0 },
                md: { span: 1, offset: 0 }, lg: { span: 3, offset: 0 }
              })

            Button() {
              Text(this.isShowActive ? $r('app.string.deActivate') : $r('app.string.activation'))
                .opacity(this.isEnableButton ? 1 : 0.38)
                .fontSize($r('sys.float.ohos_id_text_size_button1'))
                .fontColor($r('sys.color.ohos_id_color_text_primary_activated'))
            }
            .enabled(this.isEnableButton)
            .backgroundColor($r('sys.color.ohos_id_color_button_normal'))
            .height($r('app.float.button_height'))
            .margin({
              right: $r('app.float.wh_value_6'),
              bottom: $r('app.float.wh_value_24')
            })
            .onClick(() => {
              logger.info(TAG, 'button active:' + this.isShowActive);
              this.activateAdmin(this.isAdminType);
            })
            .useSizeType({
              xs: { span: 3, offset: 1 }, sm: { span: 3, offset: 1 },
              md: { span: 3, offset: 1 }, lg: { span: 3, offset: 3 }
            })

            Button() {
              Text($r('app.string.cancel'))
                .fontSize($r('sys.float.ohos_id_text_size_button1'))
                .fontColor($r('sys.color.ohos_id_color_text_primary_activated'))
            }
            .backgroundColor($r('sys.color.ohos_id_color_button_normal'))
            .height($r('app.float.button_height'))
            .margin({
              left: $r('app.float.wh_value_6'),
              bottom: $r('app.float.wh_value_24')
            })
            .onClick(() => {
              logger.info(TAG, 'button cancel in isShowActive:' + this.isShowActive);
              appDetailData.terminateAbilityPage();
            })
            .useSizeType({
              xs: { span: 3, offset: 4 }, sm: { span: 3, offset: 4 },
              md: { span: 3, offset: 4 }, lg: { span: 3, offset: 6 }
            })

            Row()
              .useSizeType({
                xs: { span: 1, offset: 7 }, sm: { span: 1, offset: 7 },
                md: { span: 1, offset: 7 }, lg: { span: 3, offset: 9 }
              })
          }
          .justifyContent(FlexAlign.Center)
        }
      }.width('100%')
    }
    .height('100%')
    .width('100%')
    .backgroundColor($r('sys.color.ohos_id_color_sub_background'))
  }

  aboutToAppear(): void {
    appInfo = {
      appIcon: '',
      appTitle: '',
      appBundleName: '',
      appPermissionList: [],
    };

    elementNameVal = {
      abilityName: '',
      bundleName: '',
    };

    enterInfo = {
      name: '',
      description: '',
    };

    isAdminTypeVal = { admintype: 0 };
    logger.info(TAG, 'aboutToAppear in');
    this.getCheckAbilityList(appInfo, elementNameVal, isAdminTypeVal);
    logger.info(TAG, 'aboutToAppear out');
  }

  aboutToDisappear(): void {
    logger.info(TAG, 'aboutToDisappear');
  }

  onPageShow(): void {
    logger.info(TAG, 'onPageShow in');
    this.isAdminActive();
    logger.info(TAG, 'onPageShow isActiveState =' + isActiveState);
    if (!utils.isValid(isActiveState)) {
      return;
    }
    this.isShowActive = isActiveState;
    logger.info(TAG, 'onPageShow out');
  }

  onPageHide() {
    logger.info(TAG, 'onPageHide');
  }

  onBackPress() {
    logger.info(TAG, 'onBackPress');
    router.back();
  }

  async isAdminActive() {
    let wantTemp = {
      bundleName: elementNameVal.bundleName,
      abilityName: elementNameVal.abilityName,
    };
    let retAppState;
    let retSuperState;
    logger.info(TAG, 'isAdminActive elementNameVal.bundleName=' + elementNameVal.bundleName
      + ' | elementNameVal.abilityName=' + elementNameVal.abilityName);
    if (elementNameVal.bundleName === baseData.EMPTY_STR || elementNameVal.abilityName === baseData.EMPTY_STR) {
      logger.info(TAG, 'isAdminActive elementNameVal is null');
      return;
    }

    retAppState = await edmEnterpriseDeviceManager.isAdminEnabled(wantTemp);
    retSuperState = await edmEnterpriseDeviceManager.isSuperAdmin(elementNameVal.bundleName);
    if (!retAppState) {
      this.isShowActive = false;
      this.isEnableButton = true;
    } else {
      if (retSuperState) {
        this.isEnableButton = false;
      } else {
        this.isEnableButton = true;
      }
      this.isShowActive = true;
    }
    logger.info(TAG, 'isAdminActive retAppState:' + retAppState
      + ' | retSuperState:' + retSuperState);
    isActiveState = this.isShowActive;
  }

  async activateAdmin(adminType) {
    logger.info(TAG, 'activateAdmin isShowActive:' + this.isShowActive);
    let wantTemp = {
      bundleName: elementNameVal.bundleName,
      abilityName: elementNameVal.abilityName,
    };
    await this.isAdminActive();
    let ret = true;
    if (!this.isShowActive) {
      if (adminType === edmEnterpriseDeviceManager.AdminType.ADMIN_TYPE_NORMAL) {
        await edmEnterpriseDeviceManager.enableAdmin(wantTemp,
          { name: enterInfo.name, description: enterInfo.description },
          edmEnterpriseDeviceManager.AdminType.ADMIN_TYPE_NORMAL)
          .catch((error) => {
            ret = false;
            logger.info(TAG, 'errorCode : ' + error.code + 'errorMessage : ' + error.message);
          });
      } else {
        logger.warn(TAG, 'not support AdminType.ADMIN_TYPE_SUPER enable Admin')
      }
      logger.info(TAG, 'activateAdmin Activate admin end');
    } else {
      if (this.isEnableButton) {
        await edmEnterpriseDeviceManager.disableAdmin(wantTemp)
          .catch((error) => {
            ret = false;
            logger.info(TAG, 'errorCode : ' + error.code + 'errorMessage : ' + error.message);
          });
      } else {
        logger.info(TAG, 'activateAdmin not support super admin active');
      }
    }
    logger.info(TAG, 'activateAdmin adminType:' + adminType + ' | ret:' + ret);
    if (ret) {
      appDetailData.terminateAbilityPage();
    }
  }

  async getSelfResourceVal(resource) {
    let resMgr = await globalThis.adminProvisioningContext.resourceManager;
    logger.info(TAG, 'getSelfResourceVal in:');
    let value = await resMgr.getString(resource.id);
    logger.info(TAG, 'getSelfResourceVal finish value:' + value);
    if (!utils.isValid(value) || value === baseData.EMPTY_STR) {
      return baseData.EMPTY_STR;
    }
    return value;
  }

  async getAbilityWantVal(appInfo, elementNameVal, adminType) {
    logger.info(TAG, 'getAbilityWantVal in:');
    let data = await globalThis.adminProvisioningWant;

    if (!utils.checkObjPropertyValid(data, 'parameters.elementName.abilityName')
      || !utils.checkObjPropertyValid(data, 'parameters.enterprise.name')
      || !utils.isValid(data.parameters.elementName.bundleName)
      || !utils.isValid(data.parameters.activeType)
      || !utils.isValid(data.parameters.enterprise.description)) {
      logger.warn(TAG, 'data.parameters = ' + JSON.stringify(data.parameters));
      return;
    }

    elementNameVal.abilityName = data.parameters.elementName.abilityName;
    elementNameVal.bundleName = data.parameters.elementName.bundleName;
    appInfo.appBundleName = data.parameters.elementName.bundleName;
    adminType.admintype = data.parameters.activeType;
    AppStorage.SetOrCreate('activeType', adminType.admintype);

    enterInfo.name = data.parameters.enterprise.name.substring(0, baseData.MAX_LEN);
    enterInfo.description = data.parameters.enterprise.description.substring(0, baseData.MAX_LEN);
    logger.info(TAG, 'getAbilityWantVal out isAdminTypeVal.admintype:'
      + isAdminTypeVal.admintype + ' enter.name=' + enterInfo.name + ' enter.des=' + enterInfo.description);
  }

  async getCheckAbilityList(appInfo, elementNameVal, adminType) {
    let deviceActiveOne = '';
    let deviceActiveTwo = '';
    let deviceDeactivate = '';
    let enterName = '';
    let enterDescription = '';
    let getName = '';
    let getDescription = '';
    logger.info(TAG, 'getCheckAbilityList in:');
    await this.getAbilityWantVal(appInfo, elementNameVal, adminType);

    let ret = await appDetailData.checkAppItem(elementNameVal);
    if (!ret.val) {
      logger.info(TAG, 'aboutToAppear not exist bundleName:' + appInfo.appBundleName);
      appDetailData.terminateAbilityPage();
      return;
    }

    await this.isAdminActive();

    if (appInfo.appBundleName) {
      await appDetailData.getBundleInfoItem(appInfo.appBundleName, appInfo);
    }

    deviceActiveOne = await this.getSelfResourceVal($r('app.string.device_active_1'));
    deviceActiveTwo = await this.getSelfResourceVal($r('app.string.device_active_2'));
    deviceDeactivate = await this.getSelfResourceVal($r('app.string.device_deactivate'));
    enterName = await this.getSelfResourceVal($r('app.string.enterprise_name'));
    enterDescription = await this.getSelfResourceVal($r('app.string.enterprise_description'));
    this.deviceAdminActive = deviceActiveOne + `${appInfo.appTitle}` + deviceActiveTwo;
    this.deviceAdminDisActive = deviceDeactivate + `${appInfo.appTitle}` + deviceActiveTwo;

    let want = {
      bundleName: elementNameVal.bundleName,
      abilityName: elementNameVal.abilityName,
    };
    let getEnterInfo = {
      name: '',
      description: '',
    };
    if (this.isShowActive) {
      logger.info(TAG, 'getCheckAbilityList get getEnterInfo');
      getEnterInfo = await edmEnterpriseDeviceManager.getEnterpriseInfo(want);
    }

    logger.info(TAG, 'getCheckAbilityList start enterpriseName=' + getEnterInfo.name
      + ' enterpriseDescription=' + getEnterInfo.description);
    if (utils.isValid(getEnterInfo.name)) {
      getName = getEnterInfo.name.substring(0, baseData.MAX_LEN);
    }
    if (utils.isValid(getEnterInfo.description)) {
      getDescription = getEnterInfo.description.substring(0, baseData.MAX_LEN);
    }

    if (this.isShowActive) {
      this.enterpriseName = enterName + getName;
      this.enterpriseDescription = enterDescription + getDescription;
    } else {
      this.enterpriseName = enterName + enterInfo.name;
      this.enterpriseDescription = enterDescription + enterInfo.description;
    }
    logger.info(TAG, 'getCheckAbilityList enterpriseName=' + this.enterpriseName
      + ' enterpriseDescription=' + this.enterpriseDescription);
  }
}