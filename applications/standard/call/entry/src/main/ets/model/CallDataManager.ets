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
 * @file: Call data management
 */
import CallStateManager from './CallStateManager';
import CallStateConst from '../common/constant/CallStateConst';
import ContactManager from './ContactManager';
import NotificationManager from '../model/NotificationManager';
import LogUtils from '../common/utils/LogUtils';
import GlobalThisHelper from '../common/utils/GlobalThisHelper';
import Constants from '../common/utils/Constants';

const TAG = "CallDataManager";
/**
 * class CallDataManager
 */
export default class CallDataManager {
  callData: any = {};
  callList: any = [];
  callTimeList: any = [];
  private callStateChange: any;
  private mCallStateManager: CallStateManager ;
  private NotificationManager;
  private contactManager;
  private static sCallDataManager: CallDataManager;

  public static getInstance(): CallDataManager {
    if (!CallDataManager.sCallDataManager) {
      CallDataManager.sCallDataManager = new CallDataManager();
    }
    return CallDataManager.sCallDataManager;
  }

  /**
   * Init data.
   */
  public init(callData, callList, callTimeList) {
    this.NotificationManager = new NotificationManager();
    this.contactManager = new ContactManager();
    this.mCallStateManager = CallStateManager.getInstance()
    this.callData = callData;
    this.callList = callList;
    this.callTimeList = callTimeList;
    this.callStateChange = (arg) => arg;
    this.mCallStateManager = new CallStateManager(this.callData);
  }

  /**
   * update callList and callData callTimeList
   *`
   * @param { object } callData
   */
  public update(callData) {
    const { callState, callId } = callData;
    const targetObj = this.callList.find((v) => v.callId === callId);
    LogUtils.i(TAG, "update :")
    if (targetObj) {
      const { oldCallState } = targetObj;
      Object.assign(targetObj, {
        ...callData
      });
      if (oldCallState != callState) {
        AppStorage.Get<NotificationManager>('notificationManager').sendCapsuleNotification(callData, GlobalThisHelper.get<boolean>(Constants.GLOBALTHIS_APPINACTIVE_STATE));
      }
    } else {
      this.addCallList({
        ...callData
      });

      // use setTimeout to avoid block ui show
      setTimeout(() => {
        if (this.contactManager != undefined) {
          this.contactManager.getContactInfo(callData)
        }
      }, 0);
    }

    if (callData.callState === CallStateConst.CALL_STATUS_ACTIVE) {
      this.updateCallTimeList(callData);
    }
    const singleCallState = callState === CallStateConst.CALL_STATUS_ACTIVE ||
    callState === CallStateConst.CALL_STATUS_WAITING || this.callList.length === 1;
    const multiCallState = (callState === CallStateConst.CALL_STATUS_DIALING ||
    callState === CallStateConst.CALL_STATUS_ALERTING) && this.callList.length > 1;
    if (singleCallState || multiCallState) {
      this.mCallStateManager.update(callData);
      this.callStateChange(callState);
    }

    if (callState === CallStateConst.CALL_STATUS_DISCONNECTED) {
      if (this.callList.length === 1) {
        this.NotificationManager.cancelNotification();
        AppStorage.Get<NotificationManager>('notificationManager').sendCapsuleNotification(callData, true);
        AppStorage.Delete("CallTimeList");
        GlobalThisHelper.get<any>(Constants.GLOBALTHIS_CONTEXT)?.terminateSelf().then((data) => {
          LogUtils.i(TAG, "calluiAbility terminateSelf");
        });
      } else {
        this.removeCallById(callId);
        const activeCallData = this.callList.find((v) => v.callState === CallStateConst.CALL_STATUS_ACTIVE);
        if (activeCallData) {
          this.mCallStateManager.update(activeCallData);
          this.callStateChange(activeCallData);
        } else if (this.callList[0]) {
          this.mCallStateManager.update(this.callList[0]);
          this.callStateChange(this.callList[0].callState);
        }
      }
    }
  }

  /**
   * Judge whether the call exists.
   */
  public hasAliveCall() {
    const callData = this.callList.find((call) => call.callState !== CallStateConst.CALL_STATUS_DISCONNECTED
      && call.callState !== CallStateConst.CALL_STATUS_DISCONNECTING);
    LogUtils.i(TAG, "hasAliveCall:" + JSON.stringify(callData !== undefined));
    return callData !== undefined;
  }

  /**
   * Judge whether the call exists.
   */
  public hasActiveCall(): boolean {
    return this.callList.find((call) => call.callState === CallStateConst.CALL_STATUS_ACTIVE
    || call.callState === CallStateConst.CALL_STATUS_HOLDING);
  }

  /**
   * Judge whether the call is active or holding.
   */
  public isActiveCall(callId): boolean {
    return this.callList.find((call) => call.callId === callId && (call.callState === CallStateConst.CALL_STATUS_ACTIVE
    || call.callState === CallStateConst.CALL_STATUS_HOLDING));
  }

  /**
   * addCallList
   *
   * @param { object } callData
   */
  private addCallList(callData) {
    this.callList.push(callData);
  }

  /**
   * remove call by call id
   *
   * @param { object } callId - call id
   */
  private removeCallById(callId) {
    const index = this.callList.findIndex((v) => v.callId === callId);
    this.callList.splice(index, 1);
    if (this.callTimeList.find((v) => v.callId === callId)) {
      this.callTimeList.splice(index, 1);
    }
  }

  /**
   * update callTimeList
   *
   * @param { object } callData
   */
  updateCallTimeList(callData) {
    const CallTimeObj = this.callTimeList.find((v) => v.callId === callData.callId);
    LogUtils.i(TAG, "updateCallTimeList : " + JSON.stringify(CallTimeObj))
    if (!CallTimeObj && callData.callState === CallStateConst.CALL_STATUS_ACTIVE) {
      const obj = {
        callId: callData.callId,
        callTime: '00:00',
        startTimestamp: (callData.startTime * 1000).valueOf() || new Date().valueOf(),
        endTimestamp: 0,
      };
      this.callTimeList.push(obj);
      AppStorage.SetOrCreate("CallTimeList", this.callTimeList)
      LogUtils.i(TAG, "updateCallTimeList : " + JSON.stringify(this.callTimeList))
    }
  }
}