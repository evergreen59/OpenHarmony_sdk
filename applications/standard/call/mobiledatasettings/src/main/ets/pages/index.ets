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
 * @file: Mobile Network Home page
 */
import publiccontent from '../common/components/publiccontent';
import item from '../common/components/listItem/listItem';
import call from '@ohos.telephony.call';
import { registerSimStateChange, getMaxSimCount } from '../model/registerSimStateApi';
import { getSimStateCardOne, getSimStateCardTwo } from '../common/model/getSimStateApi';
import { getSimCardOnePhoneNumber, getSimCardTwoPhoneNumber } from '../common/model/getSimTelephoneNumberApi';
import {
  isCellularDataRoamingEnabledCardOne,
  isCellularDataRoamingEnabledCardTwo,
  enableCellularDataRoaming,
  disableCellularDataRoaming,
  getSupportNetwork
} from '../common/model/getCellularDataRoamingEnabledApi';
import { setPreferredNetwork } from '../common/model/setPreferredNetworkApi';
import { getPreferredNetwork } from '../common/model/getPreferredNetworkModeApi';
import {
  isCellularDataEnabled,
  enableCellularData,
  disableCellularData,
  getOperatorName
} from '../model/mobileDataStatus';
import HeadComponent from '../common/components/headComponent';
import { SubHeader } from '../common/components/subHeader';
import LogUtils from '../common/utils/LogUtils';

const TAG = "Index";

@Entry
@Component
struct Index {
  scroller: Scroller = new Scroller();
  @State byValueWLMS: any = publiccontent.strings.fourAuto;
  @State publicheader: any = publiccontent.strings.mobileData;
  @State isLoading: boolean = true;
  @State simState: string = '';
  @State moisBtn: boolean = true;
  @State isBtn: boolean = true;
  @State gqBtn: boolean = true;
  @State moislastone: boolean = true;
  @State isSupport: boolean = true;
  @State mobileDataDisabled: boolean = false;
  @State mobileDataDisabled2: boolean = false;
  // Mobile data switch
  @State isDataEnable: boolean = true;
  // Data roaming switch
  @State dataRoamSwitchCardOne: boolean = false;
  @State dataRoamSwitchCardTwo: boolean = false;
  // HD call switch
  @State volteSwitch: boolean = false;
  // Card I status
  @State simStateStatusCardOne: boolean = true;
  // Card 2 status
  @State simStateStatusCardTwo: boolean = false;
  @State ControlswitchWLMS: boolean = false;
  @State slotId: number = 0;
  @State dialogTitle: any = publiccontent.strings.preferredNetworkMode;
  setCache_cun_name = 'DISTRIBUTEDDATA_CUN_NAME';
  setCache_wl_name = 'SETCACHE_WL_NAME';
  @State enable5g: boolean = false;
  @StorageLink("imsOne")enableISM: boolean = false;
  @State enableISM2: boolean = false;
  @State TelephoneNumber: string = '';
  @State TelephoneNumber2?: string = '';
  @State ceshi: boolean = false;
  @State cardOne: number = 0;
  @State cardTwo: number = 1;

  /**
   * Get network mode
   *
   * @param {Object} slotId - call slotId
   */
  async getPreferredNetwork(slotId) {
    const res = await getPreferredNetwork(this.slotId);
    if (res == 0) {
      this.byValueWLMS = $r('app.string.mobile_data_5g');
    } else if (res == 5) {
      this.byValueWLMS = $r("app.string.mobile_data_4g");
    } else if (res == 6) {
      this.byValueWLMS = $r("app.string.mobile_data_3g");
    } else if (res == 1) {
      this.byValueWLMS = $r("app.string.mobile_data_2g");
    }
  }

  /**
   * This interface is used to obtain sim cardOne status
   */
  getSimStateDataCardOne() {
    getSimStateCardOne().then((res: any) => {
      LogUtils.i(TAG, "getSimStateData Card1 :success " + JSON.stringify(res))
      const simState = {
        SIM_STATE_UNKNOWN: 0,
        SIM_STATE_NOT_PRESENT: 1,
        SIM_STATE_LOCKED: 2,
        SIM_STATE_NOT_READY: 3,
        SIM_STATE_READY: 4,
        SIM_STATE_LOADED: 5
      };
      this.simState = res;
      if (res == simState.SIM_STATE_LOADED || res == simState.SIM_STATE_READY) {
        this.simStateStatusCardOne = true;
      } else {
        this.simStateStatusCardOne = false;
      }
    }).catch((err) => {
      this.simStateStatusCardOne = false;
      LogUtils.i(TAG, 'getSimStateData Card1 :console.error() ' + JSON.stringify(err.message));
    });
  }

  /**
   * This interface is used to obtain sim cardTwo status
   */
  getSimStateDataCardTwo() {
    getSimStateCardTwo().then((res: any) => {
      LogUtils.i(TAG, "getSimState Card2 :success then" + JSON.stringify(res));
      const simState = {
        SIM_STATE_UNKNOWN: 0,
        SIM_STATE_NOT_PRESENT: 1,
        SIM_STATE_LOCKED: 2,
        SIM_STATE_NOT_READY: 3,
        SIM_STATE_READY: 4,
        SIM_STATE_LOADED: 5
      };
      if (res == simState.SIM_STATE_LOADED || res == simState.SIM_STATE_READY) {
        this.simStateStatusCardTwo = true;
      } else {
        this.simStateStatusCardTwo = false;
      }
    }).catch((err) => {
      this.simStateStatusCardTwo = false;
      LogUtils.i(TAG, 'getSimState Card2 :console.error() ' + JSON.stringify(err.message));
    });
  }

  getSupportNetwork(slotId) {
    getSupportNetwork(slotId).then((res) => {
      if (res) {
        this.isSupport = true;
      } else {
        this.isSupport = false;
      }
    }).catch((err) => {
    });
  }

  /**
   * Get mobile phone number
   */
  getSimCardOnePhoneNumber() {
    getSimCardOnePhoneNumber().then((res) => {
      LogUtils.i(TAG, "getSimTelephoneNumber card1 other success" + JSON.stringify(res));
      this.TelephoneNumber = res;
    }).catch((err) => {
      LogUtils.i(TAG, "getSimTelephoneNumber card1 other catch" + JSON.stringify(err));
    });
  }

  /**
   * Get mobile phone number
   */
  getSimCardTwoPhoneNumber() {
    getSimCardTwoPhoneNumber().then((res) => {
      LogUtils.i(TAG, "getSimTelephoneNumber card2 other success" + JSON.stringify(res));
      this.TelephoneNumber2 = res;
    }).catch((err) => {
      LogUtils.i(TAG, "getSimTelephoneNumber card2 other catch" + JSON.stringify(err));
    });
  }

  /**
   * Get mobile data on or off
   */
  getCellularDataState() {
    isCellularDataEnabled().then((data) => {
      LogUtils.i(TAG, "isCellularDataEnabled success: isON:" + JSON.stringify(data));
      this.isDataEnable = data;
    }).catch((error) => {
      LogUtils.i(TAG, "isCellularDataEnabled error catch" + JSON.stringify(error));
    });
  }

  addRegisterSimStateChange(slotId) {
    registerSimStateChange(slotId, async () => {
      this.getCellularDataRoamingEnabled();
      this.getSimStateDataCardOne();
      this.getSimStateDataCardTwo();
      this.getPreferredNetwork(slotId);
      this.getSupportNetwork(slotId);
      this.getSimCardOnePhoneNumber();
      this.getSimCardTwoPhoneNumber();
      this.getCellularDataState();
      this.isImsSwitchEnabled(0);
      if (getMaxSimCount() === 2) {
        this.isImsSwitchEnabled(1);
      }
    })
  }

  isImsSwitchEnabled(slotId) {
    call.isImsSwitchEnabled(slotId).then((res: any) => {
      if (slotId) {
        this.enableISM2 = res;
        LogUtils.i(TAG, "isImsSwitchEnable enable 1:" + JSON.stringify(this.enableISM2));
      } else {
        this.enableISM = res;
        LogUtils.i(TAG, "isImsSwitchEnable enable 0:" + JSON.stringify(this.enableISM));
      }
    });
  }

  aboutToAppear() {
    LogUtils.i(TAG, "aboutToAppear")
    if (getMaxSimCount() === 2) {
      this.addRegisterSimStateChange(1);
      this.isImsSwitchEnabled(1);
      this.getSimStateDataCardTwo();
    }
    this.addRegisterSimStateChange(0);
    //Get data roaming status
    this.getCellularDataRoamingEnabled();
    //Get HD call status
    this.isImsSwitchEnabled(0);
    this.getSimStateDataCardOne();
    //Get network mode
    this.getPreferredNetwork(this.slotId);
    getOperatorName().then((res) => {
    })
    this.getSupportNetwork(this.slotId);
    //card number acquire
    this.getSimCardOnePhoneNumber();
    this.getSimCardTwoPhoneNumber();
    this.getCellularDataState();
  }

  onPageShow() {
    this.isImsSwitchEnabled(0);
  }

  /**
   * Set network mode
   */
  radioChange(slotId, v) {
    const res = setPreferredNetwork(slotId, v);
    LogUtils.i(TAG, "radioChange res:" + JSON.stringify(res));
  }

  getCellularDataRoamingEnabled() {
    isCellularDataRoamingEnabledCardOne().then((res) => {
      LogUtils.i(TAG,"getCellularDataRoamingEnabled card one success then:" + JSON.stringify(res));
      this.dataRoamSwitchCardOne = res;
    }).catch((err) => {
      this.dataRoamSwitchCardOne = false;
      LogUtils.i(TAG, "enableCellularDataRoaming card one catch:" + JSON.stringify(err));
    });
    if (getMaxSimCount() === 2) {
      isCellularDataRoamingEnabledCardTwo().then((res) => {
        LogUtils.i(TAG,"getCellularDataRoamingEnabled card two success then:" + JSON.stringify(res));
        this.dataRoamSwitchCardTwo = res;
      }).catch((err) => {
        this.dataRoamSwitchCardTwo = false;
        LogUtils.i(TAG, "enableCellularDataRoaming card two catch:" + JSON.stringify(err));
      });
    }
  }

  build() {
    GridRow({ columns: { sm: 4, md: 8, lg: 12 }, gutter: { x: 12 } }) {
      GridCol({ span: { sm: 4, md: 8, lg: 8 }, offset: { sm: 0, md: 1, lg: 2 } }) {
        Column() {
          Column() {

            HeadComponent();

            Stack({ alignContent: Alignment.TopStart }) {
              Scroll(this.scroller) {
                Column() {
                  SubHeader({ titleContent: $r('app.string.mobile_data_general'), subTitleContent: '' });

                  Flex({
                    direction: FlexDirection.Row,
                    justifyContent: FlexAlign.SpaceBetween,
                    alignItems: ItemAlign.Center
                  }) {
                    Column() {
                      Row() {
                        Text($r('app.string.mobile_data'))
                          .fontSize(16)
                          .fontWeight(FontWeight.Medium)
                          .fontFamily('HarmonyHeiTi')
                          .lineHeight(22)
                          .fontColor($r('app.color.font_color_182431'))
                          .opacity(this.isDataEnable ? 0.9 : 0.6)
                      }

                      Row() {
                        Text($r('app.string.mobile_data_charges'))
                          .fontSize(14)
                          .fontWeight(FontWeight.Regular)
                          .fontFamily('HarmonyHeiTi')
                          .fontColor('#555')
                          .opacity(0.6)
                          .lineHeight(19)
                          .maxLines(2)
                      }
                      .margin({ top: 2, right: 26 })
                    }
                    .alignItems(HorizontalAlign.Start)

                    Toggle({ type: ToggleType.Switch, isOn: this.isDataEnable })
                      .width(36)
                      .height(20)
                      .enabled(this.simStateStatusCardOne || this.simStateStatusCardTwo)
                      .onChange((isOn: boolean) => {
                        this.isDataEnable = !this.isDataEnable;
                        LogUtils.i(TAG, "mobile data switch changes enable:" + JSON.stringify(this.isDataEnable));
                        if (this.isDataEnable == false) {
                          disableCellularData().then((data) => {
                            LogUtils.i(TAG, "disableCellularData: success then" + JSON.stringify(data));
                          }).catch((error) => {
                            LogUtils.i(TAG, "disableCellularData: error catch" + JSON.stringify(error));
                          });
                        } else {
                          enableCellularData().then((data) => {
                            LogUtils.i(TAG, "enableCellularData: success then" + JSON.stringify(data));
                          }).catch((error) => {
                            LogUtils.i(TAG, "enableCellularData: error catch" + JSON.stringify(error));
                          });
                        }
                      })
                  }
                  .padding({
                    left: 12,
                    right: 12,
                    top: 4,
                    bottom: 4
                  })
                  .height(90)
                  .width("100%")
                  .borderRadius(24)
                  .backgroundColor($r("sys.color.ohos_id_color_foreground_contrary"))
                  .opacity((this.simStateStatusCardOne || this.simStateStatusCardTwo) ? 1 : 0.4)

                  SubHeader({
                    titleContent: $r('app.string.mobile_data_card1'),
                    subTitleContent: this.TelephoneNumber
                  });

                  Column() {
                    item({
                      isBtn: this.moisBtn,
                      cardType: this.cardOne,
                      controlSwitch: $dataRoamSwitchCardOne,
                      title: $r('app.string.mobile_data_dataRoaming'),
                      describe: $r('app.string.mobile_data_enableDataWhileRoaming'),
                      isDisabled: this.simStateStatusCardOne,
                      isCard: true,
                      isSupport: false,
                      isCon: 0,
                    })

                    Divider()
                      .strokeWidth(0.5)
                      .color("#E3E3E3")
                      .lineCap(LineCapStyle.Round)
                      .margin({ right: '2%' })

                    item({
                      isBtn: this.gqBtn,
                      cardType: this.cardOne,
                      controlSwitch: $enableISM,
                      title: $r('app.string.mobile_data_volte'),
                      describe: $r('app.string.mobile_data_confirmation_function'),
                      isDisabled: this.simStateStatusCardOne,
                      isSupport: false,
                      isCard: true,
                      isCon: 22,
                    })
                  }
                  .padding({
                    left: 12,
                    right: 12,
                    top: 4,
                    bottom: 4
                  })
                  .width("100%")
                  .borderRadius(24)
                  .backgroundColor($r("sys.color.ohos_id_color_foreground_contrary"))
                  .opacity(this.simStateStatusCardOne ? 1 : 0.4)

                  SubHeader({
                    titleContent: $r('app.string.mobile_data_card2'),
                    subTitleContent: this.TelephoneNumber2
                  })
                  .visibility(getMaxSimCount() === 2 ? Visibility.Visible : Visibility.Hidden)

                  Column() {
                    item({
                      isBtn: this.moisBtn,
                      cardType: this.cardTwo,
                      controlSwitch: $mobileDataDisabled2,
                      title: $r('app.string.mobile_data_dataRoaming'),
                      describe: $r('app.string.mobile_data_enableDataWhileRoaming'),
                      isDisabled: this.simStateStatusCardTwo,
                      isSupport: false,
                      isCard: true,
                      isCon: 0,
                    })
                    Divider()
                      .strokeWidth(0.5)
                      .color("#E3E3E3")
                      .lineCap(LineCapStyle.Round)
                      .margin({ right: '2%' })
                    item({
                      isBtn: this.gqBtn,
                      cardType: this.cardTwo,
                      controlSwitch: $enableISM2,
                      title: $r('app.string.mobile_data_volte'),
                      describe: $r('app.string.mobile_data_confirmation_function'),
                      isDisabled: this.simStateStatusCardTwo,
                      isSupport: false,
                      isCard: true,
                      isCon: 22,
                    })
                  }
                  .padding({
                    left: 12,
                    right: 12,
                    top: 4,
                    bottom: 4
                  })
                  .padding({
                    left: 12,
                    right: 12,
                    top: 4,
                    bottom: 4
                  })
                  .width("100%")
                  .borderRadius(24)
                  .backgroundColor($r("sys.color.ohos_id_color_foreground_contrary"))
                  .opacity(this.simStateStatusCardTwo ? 1 : 0.4)
                  .visibility(getMaxSimCount() === 2 ? Visibility.Visible : Visibility.Hidden)
                }
                .alignItems(HorizontalAlign.Start)
              }
            }
          }
          .useSizeType({
            sm: { span: 4, offset: 0 },
            md: { span: 6, offset: 1 },
            lg: { span: 8, offset: 2 }
          })
        }
        .visibility(this.isLoading ? Visibility.Visible : Visibility.Hidden)
      }
    }
    .padding({ left: 12, right: 12 })
    .width('100%')
    .height('100%')
    .backgroundColor($r("sys.color.ohos_id_color_sub_background"))
  }
}