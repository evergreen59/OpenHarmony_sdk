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

/**
 * @file: call manager service
 */

import TelephonyCall from './TelephonyApi';
import commonEvent from '@ohos.commonEvent';
import LogUtils from "../common/utils/LogUtils";
import CallManager from '../model/CallManager'

let subscriber;
const TAG = "CallManagerService";
const CALL_BUNDLE_NAME = 'com.ohos.callui';
const ABILITY_NAME = 'com.ohos.callui.MainAbility';
const CALL_STATUS_INCOMING = 4;
const CALL_STATUS_WAITING = 5;
const CALL_STATUS_DIALING = 2;
const CALL_STATUS_DISCONNECTED = 6;
const CALL_STATUS_DISCONNECTING = 7;
const events = ['callui.event.callEvent', 'callui.event.click'];

/**
 * class CallManagerService
 */
export default class CallManagerService {
  private mTelephonyCall: TelephonyCall ;
  private callData = null;
  private callList = [];
  private context: any;

  public constructor(context: any) {
    this.mTelephonyCall = new TelephonyCall;
    this.addRegisterListener();
    this.addSubscriber();
    this.context = context;
  }

  /**
   * add callui app subscriber
   */
  async addSubscriber() {
    LogUtils.i(TAG, "addSubscriber")
    subscriber = await new Promise((resolve) => {
      commonEvent.createSubscriber({
        events,
        publisherPermission: "ohos.permission.GET_TELEPHONY_STATE"
      }, (err, data) => {
        resolve(data);
      });
    });

    commonEvent.subscribe(subscriber, (err, res) => {
      if (err.code === 0) {
        if (res.event === events[0]) {
          const obj = JSON.parse(res.data);
          if (obj && obj.key === 'getInitCallData') {
            this.publishData(this.callData);
          }
        }

        if (res.event === events[1]) {
          const {callId,btnType} = res.parameters
          this.btnclickAgent(callId, btnType)
        }
      } else {
        LogUtils.i(TAG, "addSubscriber commonEvent.subscribe failed err :" + JSON.stringify(err))
      }
      subscriber.finishCommonEvent()
        .then(() => {
          LogUtils.i(TAG, "addSubscriber finishCommonEvent")
        })
    });
  }

  /**
   * click button agent
   *
   * @param { number } callId - call id
   * @param { string } btnType - button type
   */
  btnclickAgent(callId, btnType) {
    LogUtils.i(TAG, "btnclickAgent btnType :" + btnType)
    this.getMapObj(btnType, callId)
  }

  getMapObj(typeKey, callId) {
    if (typeKey === 'answer') {
      this.mTelephonyCall.acceptCall(callId)
      this.startAbility(this.callData)
    }
    if (typeKey === 'reject') {
      this.mTelephonyCall.rejectCall(callId)
    }
    if (typeKey === 'hangUp') {
      this.mTelephonyCall.hangUpCall(callId)
    }
  }

  /**
   * add register listener
   */
  addRegisterListener() {
    this.mTelephonyCall.registerCallStateCallback(this.getCallData.bind(this));
  }

  /**
   * get callData
   *
   * @param { Object } callData - Object
   */
  getCallData(callData) {
    this.callData = callData;
    this.updateCallList();
    const {callState} = this.callData;

    /**
     * single call or dialing pull up the application
     */
    if (callState === CALL_STATUS_INCOMING || callState === CALL_STATUS_WAITING || callState === CALL_STATUS_DIALING) {
      if (this.callList.length > 1) {
        this.publishData(callData)
      }
      this.startAbility(callData);
    } else if (callState !== CALL_STATUS_DISCONNECTING) {
      this.publishData(callData);
    }
  }

  /**
   * service disconnected
   *
   * @return void
   */
  public onDisconnected(): void {
    this.callData.callState = CALL_STATUS_DISCONNECTED;
    this.publishData(this.callData);
  }

  /**
   * start ability
   *
   * @param { Object } callData - Object
   */
  startAbility(callData) {
    this.context.startAbility({
      bundleName: CALL_BUNDLE_NAME,
      abilityName: ABILITY_NAME,
      parameters: callData
    }).then((data) => {
      LogUtils.i(TAG, "callUI service startAbility data :" + JSON.stringify(data))
    }).catch((err) => {
      LogUtils.i(TAG, "callUI service startAbility err :" + JSON.stringify(err))
    });
  }

  /**
   * update callList
   */
  updateCallList() {
    const {callState, callId} = this.callData;
    const targetObj = this.callList.find((v) => v.callId === callId);
    if (targetObj) {
      Object.assign(targetObj, {
        ...this.callData
      });
    } else {
      this.callList.push({
        ...this.callData
      });
    }
    if (callState === CALL_STATUS_DISCONNECTED) {
      const index = this.callList.findIndex((v) => v.callId === callId);
      this.callList.splice(index, 1);
    }
  }

  /**
   * commonEvent publish data
   *
   * @param { Object } callData - Object
   */
  publishData(callData) {
    CallManager.getInstance().update(callData);
  }

  /**
   * unsubscribe
   */
  unsubscribe() {
    commonEvent.unsubscribe(subscriber, (err) => {
      if (err.code !== 0) {
        LogUtils.i(TAG, "unsubscribe commonEvent.unsubscribe err:" + JSON.stringify(err))
      }
    });
  }

  /**
   * remove register listener
   */
  removeRegisterListener() {
    this.mTelephonyCall.unRegisterCallStateCallback();
  }
}