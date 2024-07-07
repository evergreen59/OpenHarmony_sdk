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
 * @file: Call management
 */
import CallDataManager from './CallDataManager';
import CallUtils from '../common/utils/CallUtils';
import Utils from '../common/utils/utils';
import CallServiceProxy from './CallServiceProxy';
import LogUtils from '../common/utils/LogUtils'
import call from '@ohos.telephony.call';
import CallStateConst from '../common/constant/CallStateConst';
import GlobalThisHelper from '../common/utils/GlobalThisHelper'
import Constants from '../common/utils/Constants'

const TAG = "CallManager";
const TIMING = 1000;

/**
 * class CallManager
 */
export default class CallManager {
  private callData: any = {};
  private timer = null ;
  callTimeList = [];
  private ctx = [];
  private sendNotificationHandle: any;
  private mCallDataManager: CallDataManager;
  private mCallServiceProxy: CallServiceProxy ;
  private mUtils: Utils;
  private diffSeconds
  private mTimeMeter

  public static getInstance(): CallManager {
    if (GlobalThisHelper.get<any>(Constants.GLOBALTHIS_CALLMANAGER) === undefined) {
      GlobalThisHelper.set<any>(Constants.GLOBALTHIS_CALLMANAGER, new CallManager());
    }
    return GlobalThisHelper.get<any>(Constants.GLOBALTHIS_CALLMANAGER);
  }

  private constructor() {
    this.mCallServiceProxy = CallServiceProxy.getInstance();
    this.mUtils = Utils.getInstance();
    this.timer = null;
    this.mCallDataManager = CallDataManager.getInstance();
  }
  init(ctx) {
    this.callData = ctx.callData;
    this.timer = null;
    this.ctx = ctx;
    this.callTimeList = ctx.callTimeList;
    this.mCallDataManager?.init(ctx.callData, ctx.callList, ctx.callTimeList);
    this.openTimer();
    this.sendNotificationHandle = (arg) => arg;
    this.initCallData();
  }

  /**
   * init CallData
   */
  private initCallData() {
    if (!GlobalThisHelper.get<any>(Constants.GLOBALTHIS_ABILITY_WANT)
      && GlobalThisHelper.get<any>(Constants.GLOBALTHIS_ABILITY_WANT)?.parameters
      && ('callState' in GlobalThisHelper.get<any>(Constants.GLOBALTHIS_ABILITY_WANT)?.parameters)) {
      let callData = this.getCallDataFromWant(GlobalThisHelper.get<any>(Constants.GLOBALTHIS_ABILITY_WANT).parameters);
      this.update(callData);
      LogUtils.i(TAG, "initCallData featureAbility.getWant :")
    } else {
      this.mCallServiceProxy.publish({
        key: 'getInitCallData',
        params: []
      });
    }
  }

  /**
   * get callData from want parameters
   */
  private getCallDataFromWant(parameters) {
    return Object.assign({}, {
      accountId: parameters.accountId,
      accountNumber: parameters.accountNumber,
      callId: parameters.callId,
      callState: parameters.callState,
      callType: parameters.callType,
      conferenceState: parameters.conferenceState,
      isEcc: parameters.isEcc,
      startTime: parameters.startTime,
      videoState: parameters.videoState});
  }

  /**
   * update callData callBack
   *
   * @param { Object } callData -Object
   */
  async update(callData) {
    LogUtils.i(TAG, "update calldata:")
    if (this.callData != undefined && this.callData.callId === callData.callId) {
      const { callState } = this.callData;
      if (callState === 6) {
        GlobalThisHelper.get<any>(Constants.GLOBALTHIS_CONTEXT)?.terminateSelf().then((data) => {
          LogUtils.i(TAG, "calluiAbility terminateSelf because service disconnected");
        });
        return;
      }
    }
    this.callData = callData;
    this.mCallDataManager.update(callData);
    call.formatPhoneNumber(callData.accountNumber, (err, data) => {
      if (data === undefined) {
        AppStorage.SetOrCreate("AccountNumber", callData.accountNumber)
      } else {
        AppStorage.SetOrCreate("AccountNumber", data)
      }
    });
    CallUtils.isEmergencyPhoneNumber(callData.accountNumber)
    LogUtils.i(TAG, "update :")
  }

  /**
   * update call time list
   */
  updateCallTimeList() {
    if (!this.mCallDataManager.hasActiveCall()) {
      LogUtils.i(TAG, "no active calls to update");
      return;
    }

    this.callTimeList = AppStorage.Get("CallTimeList")
    this.callTimeList.forEach((item, i) => {
      if (this.mCallDataManager.isActiveCall(item.callId)) {
        item.endTimestamp = new Date().valueOf();
        const diffSeconds = item.endTimestamp - item.startTimestamp;
        this.diffSeconds = diffSeconds
        item.callTime = this.mUtils.formatTime(diffSeconds);
        this.callTimeList.splice(i, 1, {
          ...item,
        });
        AppStorage.SetOrCreate("CallTimeList", this.callTimeList);
      }
    });
    this.mTimeMeter = setTimeout(() => {
      this.updateCallTimeList();
    }, 1000 - this.diffSeconds % 1000);

  }

  /**
   * open timer
   *
   * @param { Function } callBack - add updateCallTimeList callBack
   */
  openTimer() {
    this.timer = setInterval(() => {
      this.updateCallTimeList();
      if (this.callData.callState === CallStateConst.CALL_STATUS_ACTIVE) {
        clearInterval(this.timer);
      }
    }, TIMING);
  }

  /**
   * clear timer
   */
  clearTimer() {
    clearInterval(this.timer);
  }
}
