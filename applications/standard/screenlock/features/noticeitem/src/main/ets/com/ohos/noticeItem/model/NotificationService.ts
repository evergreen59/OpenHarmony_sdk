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

import Log from '../../../../../../../../../common/src/main/ets/default/Log';
import NotificationManager from './NotificationManager';
import ParseDataUtil from './ParseDataUtil';
import RuleController from './rule/RuleController';
import CommonUtil from '../common/CommonUtil';
import createOrGet from "../../../../../../../../../common/src/main/ets/default/SingleInstanceHelper";
import DistributionManager from './NotificationDistributionManager';

const TAG = 'NotificationServiceSc';

interface NotificationListener {
  userId: number;
  onNotificationConsume: Function;
  onNotificationCancel: (hashcode: number) => void;
}

/**
 * notification service api
 */
export class NotificationService {
  mListeners = new Set<NotificationListener>();
  mSubscriber;
  mSortingMap = {};

  constructor() {
    this.subscribeNotification(this.getSubscriber());
    this.loadNotificationTemplate()
  }

  public register(listener: NotificationListener) {
    let res = this.mListeners.add(listener);
    Log.showInfo(TAG, `register, listener size: ${res.size}`);
  }

  public unRegister(listener: NotificationListener) {
    let res = this.mListeners.delete(listener);
    Log.showInfo(TAG, `unRegister, res: ${res}`);
  }

  public removeAll() {
    Log.showInfo(TAG, 'removeAll start');
    NotificationManager.removeAll(TAG, (data) => {
      Log.showInfo(TAG, `removeAll => data: ${JSON.stringify(data)}`);
    });
  }

  public remove(code: string, isClickItem?: boolean) {
    NotificationManager.remove(TAG, code, (data) => {
      Log.showInfo(TAG, `removeNotificationItem ==> data: ${JSON.stringify(data)}`);
    }, isClickItem);
  }

  public loadAllNotifications() {
    NotificationManager.getAllActiveNotifications(TAG, (err, requestsArr) => {
      Log.showInfo(TAG, `getAllActiveNotifications error:${err}}, requestsArr: ${JSON.stringify(requestsArr)}`);
      if (Array.isArray(requestsArr)) {
        for (let i = 0, len = requestsArr.length; i < len; i++) {
          this.handleNotificationAdd(requestsArr[i]);
        }
      }
    })
  }

  getSubscriber() {
    if (!this.mSubscriber) {
      this.mSubscriber = {
        onConsume: this.handleNotificationAddAndSortMap.bind(this),
        onCancel: this.handleNotificationCancel.bind(this),
      }
    }
    return this.mSubscriber;
  }

  handleNotificationAddAndSortMap(data): void {
    Log.showInfo(TAG, 'handleNotificationAddAndSortMap');
    this.mSortingMap = { ...this.mSortingMap, ...data?.sortingMap };
    this.handleNotificationAdd(data?.request);
  }

  handleNotificationAdd(request): void {
    ParseDataUtil.parseData(request, this.mSortingMap).then((intermediateData) => {
      Log.showInfo(TAG, `parseData id=${intermediateData?.id}, timestamp=${intermediateData?.timestamp}, bundleName=${intermediateData?.bundleName}`);
      RuleController.getNotificationData(intermediateData, (finalItemData) => {
        this.mListeners.forEach((listener) => {
          Log.showInfo(TAG, `notifcationUserId: ${finalItemData?.userId}, listener.userId: ${listener?.userId}`);
          if (CommonUtil.checkVisibilityByUser(finalItemData.userId, listener.userId)) {
            listener.onNotificationConsume(finalItemData);
          }
        });
      });
    }).catch(errorInfo => Log.showError(TAG, errorInfo));
  }

  handleNotificationCancel(data) {
    Log.showDebug(TAG, `handleNotificationCancel hashCode: ${JSON.stringify(data?.request?.hashCode)}`);
    this.mSortingMap = { ...this.mSortingMap, ...data?.sortingMap };
    const hashCode = data?.request?.hashCode;
    if (!hashCode) {
      Log.showError(TAG, `handleNotificationCancel data: ${JSON.stringify(data)}`);
      return;
    }
    this.mListeners.forEach((listener) => listener.onNotificationCancel(hashCode));
  }

  subscribeNotification(subscriber) {
    let callback = (err, data) => {
      Log.showInfo(TAG, `subscribeCallback finished err: ${JSON.stringify(err)} data: ${JSON.stringify(data)}`)
    };
    NotificationManager.subscribeNotification(TAG, subscriber, callback);
  }

  loadNotificationTemplate() {
    Log.showInfo(TAG, 'loadNotificationTemplate start');
    NotificationManager.initNotificationTemplateMap(TAG);
  }

  getPluginTempLate(templateName) {
    Log.showInfo(TAG, `getPluginTempLate param:${templateName}`);
    let pluginTempLate = null;
    if (NotificationManager.NotificationTemplateMap !== null) {
      pluginTempLate = NotificationManager.NotificationTemplateMap.get(templateName);
    }
    Log.showInfo(TAG, `getPluginTempLate pluginTempLate:${JSON.stringify(pluginTempLate)}`);
    return pluginTempLate;
  }

  enableNotification(bundleOption, data: boolean): void {
    Log.showDebug(TAG, `enableNotification bundleOption:${JSON.stringify(bundleOption)} data:${JSON.stringify(data)} `);
    NotificationManager.enableNotification(TAG, bundleOption, data, () => {
      Log.showInfo(TAG, 'enableNotification succeed');
    });
  }

  getTrustedDeviceDeviceName(deviceId) {
    Log.showInfo(TAG, `getTrustedDeviceDeviceName deviceId:${JSON.stringify(deviceId)} `);
    return DistributionManager.getInstance().getTrustedDeviceDeviceName(deviceId);
  }
}

let notificationService = createOrGet(NotificationService, TAG);

export default notificationService as NotificationService;