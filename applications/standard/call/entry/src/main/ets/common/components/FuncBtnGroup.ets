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

import Method from '../utils/Method';
import FuncBtn from './FuncBtn';
import CallStateConst from '../constant/CallStateConst';
import CallServiceProxy from '../../model/CallServiceProxy';
import Clone from '../utils/Clone';
import BtnGroupConfig from '../configs/BtnGroupConfig';
import LogUtils from '../utils/LogUtils';
import screenLock from '@ohos.screenLock';
import GlobalThisHelper from '../utils/GlobalThisHelper';
import Constants from '../utils/Constants';

const TAG = 'FuncBtnGroup';
const textMap =
  {
    'record': $r('app.string.recording'),
    'keep': $r('app.string.keep'),
    'add': $r('app.string.addCall'),
    'video': $r('app.string.videoCall'),
    'mute': $r('app.string.mute'),
    'contact': $r('app.string.contactPerson'),
    'exchange': $r('app.string.exchange'),
    'merge': $r('app.string.mergeCall'),
  };

@Component
export default struct FuncBtnGroup {
  @Link @Watch("updateBtnList") callData: any;
  @Link @Watch("updateBtnList") callList: Array<any>;
  @State count: number = 0;
  @State btnList: Array<any> = [];
  @State m: number = 0;
  @State n: number = 0;
  @State oldCallState: any = CallStateConst.CALL_STATUS_IDLE;
  private mCallServiceProxy: CallServiceProxy;
  private mBtnGroupConfig = BtnGroupConfig;
  private timer: any = null;
  private mClone: Clone;
  private btnListCall;
  private btnListDialing;
  private btnListMulti;

  aboutToAppear() {
    this.mClone = Clone.getInstance()
    this.mCallServiceProxy = CallServiceProxy.getInstance();
    this.mBtnGroupConfig.btnGroupList.forEach((v) => {
      v.iconText = textMap[v.type];
    });
    this.mBtnGroupConfig.threePartyList.forEach((v) => {
      v.iconText = textMap[v.type];
    });
    this.btnListCall = this.mClone.clone(this.mBtnGroupConfig.btnGroupList);
    this.getBtnListCall();
    this.btnList = this.btnListCall;
    this.getBtnListMulti();
    this.updateBtnList();
    LogUtils.i(TAG, "aboutToAppear :");
  }

  /**
   * Switching BtnList Based on CallStatus
   */
  updateBtnList() {
    if (this.callData.callState === CallStateConst.CALL_STATUS_DIALING || this.callData.callState === CallStateConst.CALL_STATUS_ALERTING) {
      this.btnList = this.btnListDialing;
    } else if (this.callList.length > 1){
      this.btnList = this.btnListMulti;
    } else {
      this.btnList = this.btnListCall;
    }
    this.onCallStateChange(this.callData.callState);
  }

  /**
   * Get BtnList Based on CallStatus
   */
  getBtnListCall() {
    this.btnListDialing = this.mClone.clone(this.mBtnGroupConfig.btnGroupList);
    this.btnListDialing[1].isDisable = true
    this.btnListDialing[2].isDisable = true
  }

  /**
   * Get BtnList for Multi Calls
   */
  getBtnListMulti() {
    this.btnListMulti = this.mClone.clone(this.mBtnGroupConfig.btnGroupList);
    this.btnListMulti[1] = this.mClone.clone(this.mBtnGroupConfig.threePartyList[0]);
    this.btnListMulti[2] = this.mClone.clone(this.mBtnGroupConfig.threePartyList[1]);
  }

  /**
   * update state of group buttons
   *
   * @param {Object} callData - call data
   */
  onCallStateChange(newVal) {
    LogUtils.i(TAG, "Calling status changed newState : " + JSON.stringify(newVal));
    const BtnName = ['video', 'record', 'add', 'contact']
    if (newVal === CallStateConst.CALL_STATUS_ACTIVE || newVal === CallStateConst.CALL_STATUS_HOLDING) {
      this.btnList.forEach((item) => {
        if (!Method.includes(BtnName, (item.type))) {
          item.isDisable = false;
          item.isActive = false;
        }
      });
      if (newVal === CallStateConst.CALL_STATUS_HOLDING) {
        this.btnList[1].isActive = true;
        this.btnList[4].isDisable = true;
      }
    } else {
      this.btnList.forEach((item) => {
        if (item.type === 'contact') {
          item.isDisable = false;
        }
      });
    }
  }

  /**
   * Display the buttons of the button group
   *
   * @param {Object} obj - object
   */
  btnClick(obj) {
    LogUtils.i(TAG, "btnClick get icon type : " + JSON.stringify(obj));
    const BtnName = ['record', 'video', 'mute']
    const type = obj.type;
    const { callId } = this.callData;
    if (Method.includes(BtnName, type)) {
      this.btnList.forEach((item) => {
        if (item.type === type) {
          item.isActive = !item.isActive;
        }
      });
      if (type === 'record') {
        if (this.btnList[0].isActive) {
        } else {
          this.count = 0;
          clearInterval(this.timer);
          this.btnList[0].iconText = $r('app.string.recording');
        }
      }
    }

    switch (type) {
      case 'record':
        break;
      case 'keep':
        this.keepHandle('keep');
        break;
      case 'exchange':
        LogUtils.i(TAG, "exchange button clicked, callid: " + callId);
        this.callList.forEach((item) => {
          if (item.callState === CallStateConst.CALL_STATUS_HOLDING) {
            this.mCallServiceProxy.unHoldCall(item.callId);
            return;
          }
        });
        break;
      case 'add':
        this.startContact('page_flag_dialer')
        break;
      case 'video':
        break;
      case 'mute':
        this.muteHandle('mute');
        break;
      case 'contact':
        this.startContact("page_flag_choose_contacts")
        break;
      case 'merge':
        this.mCallServiceProxy.combineConference(callId);
        break;
      default:
        break;
    }
  }

  startContact(pageFlag) {
    if (screenLock.isLocked()) {
      screenLock.unlock((err, isUnlock) => {
        if (isUnlock) {
          this.startContactAbility(pageFlag)
        } else {
          LogUtils.i(TAG, "startContact screen isLocked")
        }
      });
    } else {
        this.startContactAbility(pageFlag)
    }
  }

  startContactAbility(pageFlag) {
    GlobalThisHelper.get<any>(Constants.GLOBALTHIS_CONTEXT)?.startAbility({
      bundleName: 'com.ohos.contacts',
      abilityName: 'com.ohos.contacts.MainAbility',
      parameters: {
        pageFlag: pageFlag
      }
    });
  }

  /**
   * Call hold interface
   *
   * @param {string} type - Click the hold button
   */
  keepHandle(type) {
    const awaitIsActive = this.btnList.find((v) => v.type === type).isActive;
    LogUtils.i(TAG, "keep handle awaitIsActive : " + !awaitIsActive);
    !awaitIsActive ? this.mCallServiceProxy.holdCall(this.callData.callId) : this.mCallServiceProxy.unHoldCall(this.callData.callId);
  }

  /**
   * Call hold mute
   *
   * @param {string} type - Click the hold button
   */
  muteHandle(type) {
    const awaitIsActive = this.btnList.find((v) => v.type === type).isActive;
    LogUtils.i(TAG, "mute Handle awaitIsActive : " + awaitIsActive);
    awaitIsActive ? this.mCallServiceProxy.setMuted() : this.mCallServiceProxy.cancelMuted();
  }

  /**
   * Clear timer
   */
  onDestroy() {
    LogUtils.i(TAG, "onDestroy");
    this.timer && clearInterval(this.timer);
  }

  build() {
    GridRow({ columns: { sm: 4, md: 8, lg: 12 }, gutter: 0 }) {
      GridCol({ span: { sm: 4, md: 6, lg: 6 }, offset: { md: 1, lg: 3 } }) {
        Grid() {
          ForEach(this.btnList, (item) => {
            GridItem() {
              FuncBtn({
                btnType: item.type,
                isDisable: item.isDisable,
                isActive: item.isActive,
                iconText: item.iconText,
                iconDisableUrl: item.iconDisableUrl,
                iconDefaultUrl: item.iconDefaultUrl,
                iconActiveUrl: item.iconActiveUrl,
                btnClick: () => {
                  this.btnClick(item)
                }
              })
            }
            .height(51.5)
          })
        }
        .columnsGap(24)
        .rowsGap(29.5)
        .height(132.5)
        .columnsTemplate('1fr 1fr 1fr')
        .rowsTemplate('1fr 1fr')
      }
    }
    .margin(24)
  }
}