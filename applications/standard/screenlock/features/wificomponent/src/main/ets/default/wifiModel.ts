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

import wifi from '@ohos.wifi';
import Constants, {WifiState, WifiConnectionState} from './common/constants'
import Log from "../../../../../../common/src/main/ets/default/Log";

const TAG = 'WifiComponent-WifiModel';

var mWifiInfo;
var mWifiStatus;
var mWifiOpenStatus;
var mWifiName;

export class WifiModel {
  mIsStart: boolean = false;
  mListener= null;

  initWifiModel() {
    if (this.mIsStart) {
      return;
    }
    Log.showDebug(TAG, `initWifiModel`)
    this.mIsStart = true;

    mWifiInfo = AppStorage.SetAndLink("wifiInfo", Constants.DEFAULT_WIFI_INFO);
    mWifiStatus = AppStorage.SetAndLink("wifiStatus", Constants.DEFAULT_WIFI_STATUS);
    mWifiName = AppStorage.SetAndLink("wifiName", Constants.DEFAULT_WIFI_NAME)
    mWifiOpenStatus = AppStorage.SetAndLink("wifiOpenStatus", Constants.DEFAULT_WIFI_OPEN_STATUS);

    this.getWifiInfo();

    wifi.on('wifiStateChange', this.onWifiStateChange.bind(this));
    wifi.on('wifiConnectionChange', this.onWifiConnectionChange.bind(this));
    wifi.on('wifiRssiChange', this.onWifiRssiChange.bind(this));
  }

  uninitWifiModel() {
    if (!this.mIsStart) {
      return;
    }
    Log.showInfo(TAG, `uninitWifiModel`)
    this.mIsStart = false;

    this.mListener.off('wifiRssiChange', (data) => {
      Log.showDebug(TAG, `uninitWifiModel->wifiRssiChange, data: ${JSON.stringify(data)}`)
    });
    this.mListener.off('wifiConnectionChange', (data) => {
      Log.showDebug(TAG, `uninitWifiModel->wifiConnectionChange, data: ${JSON.stringify(data)}`)
    });
    this.mListener.off('wifiStateChange', (data) => {
      Log.showDebug(TAG, `uninitWifiModel->wifiStateChange, data: ${JSON.stringify(data)}`)
    });
    this.mListener = null;
    mWifiOpenStatus.set(Constants.DEFAULT_WIFI_OPEN_STATUS);
    this.setDisconnectedStatus();
  }

  onWifiStateChange(data) {
    Log.showInfo(TAG, `onWifiStateChange, data: ${JSON.stringify(data)}`)

    let isWifiInactive = data == WifiState.STATE_OFF;
    mWifiOpenStatus.set(!isWifiInactive);
    if (!isWifiInactive) {
      this.getWifiConnectInfo();
    } else {
      this.setDisconnectedStatus();
    }
  }

  onWifiConnectionChange(data) {
    Log.showInfo(TAG, `onWifiConnectionChange, data: ${JSON.stringify(data)}`)

    if (data == WifiConnectionState.CONNECTED) {
      this.getLinkedInfo();
    } else {
      this.setDisconnectedStatus();
    }
  }

  onWifiRssiChange(data) {
    Log.showInfo(TAG, 'onWifiRssiChange')
    this.getLinkedInfo();
  }

  getWifiInfo() {
    let isWifiActive = wifi.isWifiActive();
    Log.showInfo(TAG, `getWifiInfo, isWifiActive: ${isWifiActive}`)
    mWifiOpenStatus.set(isWifiActive);
    if (isWifiActive) {
      this.getWifiConnectInfo();
    } else {
      this.setDisconnectedStatus();
    }
  }

  getWifiConnectInfo() {
    let isConnected = wifi.isConnected();
    Log.showInfo(TAG, `getWifiConnectInfo, isConnected: ${isConnected}`)
    if (isConnected) {
      mWifiStatus.set(true);
      this.getLinkedInfo();
    } else {
      this.setDisconnectedStatus();
    }
  }

  getLinkedInfo() {
    Log.showDebug(TAG, `getLinkedInfo`)
    wifi.getLinkedInfo((err, data) => {
      if (wifi.isConnected()) {
        mWifiStatus.set(true);
        mWifiName.set(data.ssid);
        let signalLevel = wifi.getSignalLevel(data.rssi, data.band);
        Log.showInfo(TAG, `getLinkedInfo, signalLevel: ${signalLevel}`)
        mWifiInfo.set(signalLevel);
      } else {
        this.setDisconnectedStatus();
      }
    });
  }

  setDisconnectedStatus() {
    Log.showInfo(TAG, `setDisconnectedStatus`)
    mWifiStatus.set(Constants.DEFAULT_WIFI_STATUS);
    mWifiInfo.set(Constants.DEFAULT_WIFI_INFO);
    mWifiName.set(Constants.DEFAULT_WIFI_NAME);
  }

  enableWifi() {
    let result = wifi.enableWifi();
    Log.showInfo(TAG, `enableWifi, result: ${result}`);
  }

  disableWifi() {
    let result = wifi.disableWifi();
    Log.showInfo(TAG, `disableWifi, result: ${result}`);
  }
}

let mWifiModel = new WifiModel();

export default mWifiModel as WifiModel;