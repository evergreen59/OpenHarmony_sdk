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

import hardware_deviceManager from '@ohos.distributedHardware.deviceManager'
import Logger from '../model/Logger'

let SUBSCRIBE_ID: number = 100
const RANDOM: number = 65536
const TAG: string = 'RemoteDeviceModel'

export class RemoteDeviceModel {
  public deviceLists: Array<hardware_deviceManager.DeviceInfo> = []
  public discoverLists: Array<hardware_deviceManager.DeviceInfo> = []
  private callback: () => void = null
  private authCallback: () => void = null
  private deviceManager: hardware_deviceManager.DeviceManager = undefined

  registerDeviceListCallback(callback) {
    if (typeof (this.deviceManager) === 'undefined') {
      Logger.info(TAG, 'deviceManager.createDeviceManager begin')
      try {
        hardware_deviceManager.createDeviceManager('ohos.samples.etsdistributedmusicplayer', (error, value) => {
          if (error) {
            Logger.error(TAG, 'createDeviceManager failed.')
            return
          }
          this.deviceManager = value
          this.registerDeviceList(callback)
          Logger.info(TAG, `createDeviceManager callback returned, error= ${error} value= ${value}`)
        })
      } catch(error) {
        Logger.info(TAG, `createDeviceManager throw error, error=${error} message=${error.message}`)
      }
      Logger.info(TAG, 'deviceManager.createDeviceManager end')
    } else {
      this.registerDeviceList(callback)
    }
  }

  registerDeviceList(callback) {
    Logger.info(TAG, 'registerDeviceListCallback')
    this.callback = callback
    if (this.deviceManager === undefined) {
      Logger.error(TAG, 'deviceManager has not initialized')
      this.callback()
      return
    }

    Logger.info(TAG, 'getTrustedDeviceListSync begin')
    let list: hardware_deviceManager.DeviceInfo[] = []
    try {
      list = this.deviceManager.getTrustedDeviceListSync()
    } catch (error) {
      Logger.info(TAG, `getTrustedDeviceListSync throw error, error=${error} message=${error.message}`)
    }
    Logger.info(TAG, `getTrustedDeviceListSync end, deviceLists= ${JSON.stringify(list)}`)
    if (typeof (list) !== 'undefined' && typeof (list.length) !== 'undefined') {
      this.deviceLists = list
    }
    this.callback()
    Logger.info(TAG, 'callback finished')

    try {
      this.deviceManager.on('deviceStateChange', (data) => {
        Logger.info(TAG, `deviceStateChange data= ${JSON.stringify(data)}`)
        switch (data.action) {
          case hardware_deviceManager.DeviceStateChangeAction.READY:
            this.discoverLists = []
            this.deviceLists.push(data.device)
            Logger.info(TAG, `reday, updated device list= ${JSON.stringify(this.deviceLists)} `)
            let list: hardware_deviceManager.DeviceInfo[] = []
            try {
              list = this.deviceManager.getTrustedDeviceListSync()
            } catch (err) {
              Logger.info(TAG, `this err is ${JSON.stringify(err)}`)
            }
            Logger.info(TAG, `getTrustedDeviceListSync end, deviceList= ${JSON.stringify(list)}`)
            if (typeof (list) !== 'undefined' && typeof (list.length) !== 'undefined') {
              this.deviceLists = list;
            }
            this.callback()
            break
          case hardware_deviceManager.DeviceStateChangeAction.OFFLINE:
            if (this.deviceLists.length > 0) {
              let list = [];
              for (let i = 0; i < this.deviceLists.length; i++) {
                if (this.deviceLists[i].deviceId !== data.device.deviceId) {
                  list[i] = data.device;
                }
              }
              this.deviceLists = list;
            }
            Logger.info(TAG, `offline, updated device list= ${JSON.stringify(this.deviceLists)}`)
            this.callback()
            break
          default:
            break
        }
      })
      this.deviceManager.on('deviceFound', (data) => {
        Logger.info(TAG, `deviceFound data= ${JSON.stringify(data)}`)
        Logger.info(TAG, `deviceFound this.deviceLists= ${this.deviceLists}, this.deviceLists.length= ${this.deviceLists.length}`)
        for (let i = 0;i < this.discoverLists.length; i++) {
          if (this.discoverLists[i].deviceId === data.device.deviceId) {
            Logger.info(TAG, 'device founded, ignored')
            return
          }
        }
        this.discoverLists[this.discoverLists.length] = data.device
        this.callback()
      })
      this.deviceManager.on('discoverFail', (data) => {
        Logger.info(TAG, `discoverFail data= ${JSON.stringify(data)}`)
      })
      this.deviceManager.on('serviceDie', () => {
        Logger.error(TAG, 'serviceDie')
      })
    } catch(error) {
      Logger.info(TAG, `on throw error, error=${error} message=${error.message}`)
    }

    SUBSCRIBE_ID = Math.floor(RANDOM * Math.random())
    let info = {
      subscribeId: SUBSCRIBE_ID,
      mode: 0xAA,
      medium: 2,
      freq: 2,
      isSameAccount: false,
      isWakeRemote: true,
      capability: 0
    }
    Logger.info(TAG, `startDeviceDiscovery ${SUBSCRIBE_ID}`)
    try {
      this.deviceManager.startDeviceDiscovery(info)
    } catch(error) {
      Logger.info(TAG, `startDeviceDiscovery throw error, error=${error} message=${error.message}`)
    }
  }

  authDevice(device, callback) {
    Logger.info(TAG, `authDevice ${device}`)
    for (let i = 0; i < this.discoverLists.length; i++) {
      if (this.discoverLists[i].deviceId === device.deviceId) {
        Logger.info(TAG, 'device founded, ignored')
        let extraInfo = {
          "targetPkgName": 'ohos.samples.etsdistributedmusicplayer',
          "appName": 'Music',
          "appDescription": 'Music player application',
          "business": '0'
        }
        let authParam = {
          "authType": 1,
          "appIcon": '',
          "appThumbnail": '',
          "extraInfo": extraInfo
        }
        Logger.info(TAG, `authenticateDevice ${JSON.stringify(this.discoverLists[i])}`)
        try {
          this.deviceManager.authenticateDevice(this.discoverLists[i], authParam, (err, data) => {
            Logger.info(TAG, `authenticateDevice succeed, data= ${JSON.stringify(data)}`)
            this.authCallback = callback
          })
        } catch(error) {
          Logger.info(TAG, `authenticateDevice throw error, error=${error} message=${error.message}`)
        }
      }
    }
  }

  unregisterDeviceListCallback() {
    Logger.info(TAG, `stopDeviceDiscovery ${SUBSCRIBE_ID}`)
    if (this.deviceManager === undefined) {
      return
    }
    try {
      this.deviceManager.stopDeviceDiscovery(SUBSCRIBE_ID)
      this.deviceManager.off('deviceStateChange')
      this.deviceManager.off('deviceFound')
      this.deviceManager.off('discoverFail')
      this.deviceManager.off('serviceDie')
    } catch(error) {
      Logger.info(TAG, `stopDeviceDiscovery throw error, error=${error} message=${error.message}`)
    }
    this.deviceLists = []
  }
}
