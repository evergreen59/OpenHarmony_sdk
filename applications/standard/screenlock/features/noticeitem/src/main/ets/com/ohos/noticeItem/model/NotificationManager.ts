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

//import { NotificationSubscriber } from './notification/notificationSubscriber';
import Notification from '@ohos.notification';
import PluginComponentManager from '@ohos.pluginComponent';
import Systemparameter from '@ohos.systemparameter';
import Log from '../../../../../../../../../common/src/main/ets/default/Log';

const TAG = 'NotificationManager';
// Temporary path
const EXTERNAL_JSON_PATH = '/system/etc/NotificationTemplate/external.json'
const DEBUG_SETTING_KEY = 'debug.systemui.notificationtemplate';
const DEBUG_BUNDLE_NAME = 'com.ohos.example.notificationtemplate'
const DEBUG_ABILITY_NAME = 'com.ohos.example.notificationtemplate.MainAbility'
const DEBUG_TEMPLATE_NAME = 'NotificationTemplate'

export default class NotificationManager {
  static TYPE_BASIC: number = Notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT;
  static TYPE_LONG: number = Notification.ContentType.NOTIFICATION_CONTENT_LONG_TEXT;
  static TYPE_MULTI: number = Notification.ContentType.NOTIFICATION_CONTENT_MULTILINE;
  static TYPE_PICTURE: number = Notification.ContentType.NOTIFICATION_CONTENT_PICTURE;
  static NotificationTemplateMap = new Map();

  static subscribeNotification(tag, subscriber, asyncCallback) {
    Log.showInfo(TAG, `subscribeNotification from: ${tag}`);
    Notification.subscribe(subscriber, asyncCallback);
  }

  static unsubscribeNotification(tag, subscriber) {
    Log.showInfo(TAG, `subscribeNotification from: ${tag}`);
    Notification.unsubscribe(subscriber);
  }

  static removeAll(tag, callback) {
    Log.showInfo(TAG, `removeAll from: ${tag}`);
    Notification.removeAll(callback);
  }

  static remove(tag, hashCode, callback, isClickItem?: boolean) {
    Log.showInfo(TAG, `remove from: ${tag}`);
    var clickValue = isClickItem == true ? Notification.RemoveReason.CLICK_REASON_REMOVE : Notification.RemoveReason.CANCEL_REASON_REMOVE;
    Notification.remove(hashCode, clickValue, callback);
  }

  static getAllActiveNotifications(tag, callback) {
    Log.showInfo(TAG, `getAllActiveNotifications from: ${tag}`);
    Notification.getAllActiveNotifications(callback);
  }

  static enableNotification(tag, bundleOption, data, callback) {
    Log.showInfo(TAG, `enableNotification from: ${tag}`);
    Notification.enableNotification(bundleOption, data, callback);
  }

  static initNotificationTemplateMap(tag) {
    Log.showInfo(TAG, `initNotificationTemplateMap from: ${tag}`);
    if (NotificationManager.isDebugMode(tag)) {
      NotificationManager.requestDebugTemplate(tag);
    } else {
      NotificationManager.requestTemplate(tag, '', EXTERNAL_JSON_PATH);
    }
  }

  static request(tag, param, asyncCallback) {
    Log.showInfo(TAG, `request from: ${tag}`);
    PluginComponentManager.request(param, (err, data) => {
      asyncCallback(err, data);
    });
  }

  static push(tag, param, asyncCallback) {
    Log.showInfo(TAG, `push from: ${tag}`);
    PluginComponentManager.push(param, () => {
      asyncCallback();
    });
  }

  static requestListener(tag, asyncCallback) {
    Log.showInfo(TAG, `requestListener from: ${tag}`);
    PluginComponentManager.on('request', (source, name, data) => {
      asyncCallback(source, name, data);
    })
  }

  static pushListener(tag, asyncCallback) {
    Log.showInfo(TAG, `pushListener from: ${tag}`);
    PluginComponentManager.on('push', (source, template, data, extraData) => {
      asyncCallback(source, template, data, extraData);
    });
  }

  static requestTemplate(tag, templateName, templatePath) {
    Log.showInfo(TAG, `requestTemplate from: ${tag}`);

    let reqWant = {
      bundleName: '',
      abilityName: ''
    };
    let reqData = {}

    let requestParam = {
      want: reqWant,
      name: templateName,
      data: reqData,
      jsonPath: templatePath
    };

    Log.showDebug(TAG, `requestTemplate requestParam: ${JSON.stringify(requestParam)}`)
    NotificationManager.request(tag, requestParam, (err, data) => {
      Log.showDebug(TAG, `request finished err: ${JSON.stringify(err)} data: ${JSON.stringify(data)}`)
      if (data !== null && data !== undefined) {
        let templates = JSON.parse(data.componentTemplate.source);
        Log.showDebug(TAG, `request data.componentTemplate.source:${JSON.stringify(data.componentTemplate.source)}`+
          `templates: ${JSON.stringify(templates)}`)
        for (let key in templates) {
          NotificationManager.NotificationTemplateMap.set(key, {
            "source": templates[key], "ability": ""
          });
        }
      }
    });
  }

  static requestDebugTemplate(tag) {
    Log.showInfo(TAG, `requestDebugTemplate from: ${tag}`);
    let reqWant = {
      bundleName: DEBUG_BUNDLE_NAME,
      abilityName: DEBUG_ABILITY_NAME
    };
    let reqData = {}
    let requestParam = {
      want: reqWant,
      name: DEBUG_TEMPLATE_NAME,
      data: reqData
    };
    Log.showDebug(TAG, `requestDebugTemplate requestParam: ${JSON.stringify(requestParam)}`);
    NotificationManager.request(tag, requestParam, (err, data) => {
      Log.showDebug(TAG, `requestDebugTemplate finished err: ${JSON.stringify(err)} data: ${JSON.stringify(data)}`)
      if (!!data) {
        Log.showDebug(TAG, `requestDebugTemplate finished data.componentTemplate.source:${JSON.stringify(data.componentTemplate.source)}`)
        NotificationManager.NotificationTemplateMap.set(DEBUG_TEMPLATE_NAME, data.componentTemplate);
      }
    });
  }

  static isDebugMode(tag): boolean{
    Log.showDebug(TAG, `isDebugMode from: ${tag}`);
    let debug = Systemparameter.getSync(DEBUG_SETTING_KEY, "")
    Log.showInfo(TAG, `Systemparameter DEBUG_SETTING: ${debug}`);
    return!!debug;
  }

}