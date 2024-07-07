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
import LogUtil from '../../../../../../common/util/LogUtil';
import ConfigData from '../../../../../../common/util/ConfigData';
import HeadComponent from '../../../../../../common/component/headComponent';
import CMShowSysCaPresenter from '../presenter/CMShowSysCaPresenter';
import CMShowUserCaPresenter from '../presenter/CMShowUserCaPresenter';
import CMFaPresenter from '../presenter/CMFaPresenter';

@Component
struct DialogSubjectComponent {
  private map: Map<string, string>;
  private subjectNameCN: string = ''
  private subjectNameO: string = ''
  private subjectNameOU: string = ''
  private serial: string = ''
  aboutToAppear() {
    if (this.map != null) {
      this.subjectNameCN = this.map.get('常用名称:')
      this.subjectNameO = this.map.get('组织:')
      this.subjectNameOU = this.map.get('组织单位:')
      this.serial = this.map.get('序列号:')
    }
  }
  build() {
    Column() {
      Text($r("app.string.DialogSubjectComponent_firText"))
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
        .margin({
          top: $r("app.float.DialogSubjectComponent_Text_margin_top_value")
        })
      Text(this.subjectNameCN)
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
      Text($r("app.string.DialogSubjectComponent_secText"))
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
        .margin({
          top: $r("app.float.CustomDialogExample_Button_margin_top_value")
        })
      Text(this.subjectNameO)
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
      Text($r("app.string.DialogSubjectComponent_thdText"))
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
        .margin({
          top: $r("app.float.CustomDialogExample_Button_margin_top_value")
        })
      Text(this.subjectNameOU)
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
      Text($r("app.string.DialogSubjectComponent_fouText"))
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
        .margin({
          top: $r("app.float.wh_value_12")
        })
      Text(this.serial)
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
    }.alignItems(HorizontalAlign.Start);
  }
}

@Component
struct DialogIssuerComponent {
  private map: Map<string, string>;
  private issuerNameCN: string = ''
  private issuerNameO: string = ''
  private issuerNameOU: string = ''
  aboutToAppear() {
    if (this.map != null) {
      this.issuerNameCN = this.map.get('常用名称:')
      this.issuerNameO = this.map.get('组织:')
      this.issuerNameOU = this.map.get('组织单位:')
    }
  }

  build() {
    Column() {
      Text($r("app.string.DialogSubjectComponent_firText"))
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
        .margin({
          top: $r("app.float.wh_value_10")
        })
      Text(this.issuerNameCN)
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
      Text($r("app.string.DialogSubjectComponent_secText"))
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
        .margin({
          top: $r("app.float.CustomDialogExample_Button_margin_top_value")
        })
      Text(this.issuerNameO)
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
      Text($r("app.string.DialogSubjectComponent_thdText"))
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
        .margin({
          top: $r("app.float.CustomDialogExample_Button_margin_top_value")
        })
      Text(this.issuerNameOU)
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
    }.alignItems(HorizontalAlign.Start);
  }
}

@Component
struct DialogDateComponent {
  private map: Map<string, string>;
  private notBefore: string = ''
  private notAfter: string = ''
  aboutToAppear() {
    if (this.map != null) {
      this.notBefore = this.map.get('颁发时间:')
      this.notAfter = this.map.get('有效期至:')
    }
  }

  build() {
    Column() {
      Text($r("app.string.DialogSubjectComponent_fifText"))
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
        .margin({
          top: $r("app.float.wh_value_10")
        })
      Text(this.notBefore)
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
      Text($r("app.string.DialogSubjectComponent_sixText"))
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
        .margin({
          top: $r("app.float.CustomDialogExample_Button_margin_top_value")
        })
      Text(this.notAfter)
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
    }.alignItems(HorizontalAlign.Start);
  }
}

@Component
struct DialogFingerPrintComponent {
  private fingerprintSha256: string;

  build() {
    Column() {
      Text($r("app.string.CustomDialogExample_FingerPrint_text"))
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
        .margin({
          top: $r("app.float.wh_value_10")
        })
      Text(this.fingerprintSha256)
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
        .margin({
          right: $r("app.float.wh_value_24")
        })
    }.alignItems(HorizontalAlign.Start);
  }
}

@CustomDialog
@Component
struct CustomDialogDeleteWarning {
  controller: CustomDialogController;
  @Link deleteWarn: CMShowUserCaPresenter;
  @Link mShowUserListPresenter: CMShowUserCaPresenter;
  @Link isShowWarnDialog: boolean;

  build() {
    Column() {
      Text($r("app.string.CustomDialogExampleUser_warning_Button_title_text"))
        .height($r("app.float.wh_value_56"))
        .fontSize($r("sys.float.ohos_id_text_size_dialog_tittle"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Medium)
        .margin({
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
        .alignSelf(ItemAlign.Start)
      Text($r("app.string.CustomDialogExampleUser_warning_Button_message_text"))
        .fontSize($r('sys.float.ohos_id_text_size_body1'))
        .fontWeight(FontWeight.Regular)
        .fontColor($r("sys.color.ohos_id_color_primary"))
        .margin({
          top: $r("app.float.wh_value_16"),
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
        .alignSelf(ItemAlign.Start)

      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceAround }) {
        Button($r("app.string.deleteAllCredCancel"))
          .onClick(() => {
            this.isShowWarnDialog = false;
            this.controller.close()
          })
          .backgroundColor($r("app.color.CustomDialogExample_Button_backgroundColor_FFFFFF"))
          .fontSize($r("sys.float.ohos_id_text_size_button1"))
          .width($r('app.float.wh_value_152'))
          .height($r('app.float.wh_value_40'))
          .fontColor($r("sys.color.ohos_id_color_text_hyperlink"))
        Divider()
          .strokeWidth(1)
          .color($r("sys.color.ohos_id_color_list_separator"))
          .vertical(true)
          .height($r('app.float.wh_value_40'))
          .opacity($r("app.float.opacity_0_2"))
        Button($r("app.string.deleteAllCredDelete"))
          .onClick(() => {
            this.deleteWarn.deleteUserCertificate(this.deleteWarn.certInfo.uri);
            this.mShowUserListPresenter.updateUserTrustedCertificateList();
            this.isShowWarnDialog = false;
            this.controller.close()
          })
          .backgroundColor($r("app.color.CustomDialogExample_Button_backgroundColor_FFFFFF"))
          .fontSize($r("sys.float.ohos_id_text_size_button1"))
          .width($r('app.float.wh_value_152'))
          .height($r('app.float.wh_value_40'))
          .fontColor($r("sys.color.ohos_id_color_warning"))
      }
      .height($r('app.float.wh_value_64'))
      .padding({
        bottom: $r('app.float.wh_value_16')
      })
      .margin({
        top: $r('app.float.wh_value_16'),
        left: $r('app.float.wh_value_24'),
        right: $r('app.float.wh_value_24')
      })
    }
  }
}

@CustomDialog
@Component
struct CustomDialogExampleUser {
  controller: CustomDialogController;
  @Link userInfoPresenter: CMShowUserCaPresenter;
  @Link userInfoPresenterForAbstractList: CMShowUserCaPresenter;
  @Link isShowWarnDialog: boolean;
  @State IsOn: boolean = false;

  aboutToAppear() {
    this.IsOn = this.userInfoPresenter.certInfo.status;
  }

  build() {
      Column() {
        Text($r("app.string.CustomDialogExample_firText"))
          .fontSize($r("sys.float.ohos_id_text_size_dialog_tittle"))
          .fontColor($r("sys.color.ohos_id_color_text_primary"))
          .fontWeight(FontWeight.Medium)
          .margin({
            left: $r("app.float.CustomDialogExample_firText_margin_left_value"),
            top: $r("app.float.CustomDialogExample_firText_margin_top_value")
          })
          .alignSelf(ItemAlign.Start)
        Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
          Column() {
            Text(this.userInfoPresenter.certInfo.certAlias)
              .fontSize($r("sys.float.ohos_id_text_size_body1"))
              .fontColor($r("sys.color.ohos_id_color_text_primary"))
              .fontWeight(FontWeight.Medium)
              .margin({
                left: $r("app.float.wh_value_26")
              })
            if (this.IsOn) {
              Text($r("app.string.CustomDialogExampleUser_Status_true"))
                .fontSize($r("sys.float.ohos_id_text_size_body2"))
                .fontColor($r("sys.color.ohos_id_color_text_secondary"))
                .margin({
                  left: $r("app.float.wh_value_26")
                })
            } else {
              Text($r("app.string.CustomDialogExampleUser_Status_false"))
                .fontSize($r("sys.float.ohos_id_text_size_body2"))
                .fontColor($r("sys.color.ohos_id_color_text_secondary"))
                .margin({
                  left: $r("app.float.wh_value_26")
                })
            }
          }.alignItems(HorizontalAlign.Start);
          Toggle({ type: ToggleType.Switch, isOn: this.IsOn })
            .margin({
              right: $r("app.float.CustomDialogExampleUser_Flex_Toggle_margin_right_value")
            })
            .width($r("app.float.CustomDialogExampleUser_Flex_Toggle_width_value"))
            .height($r("app.float.CustomDialogExampleUser_Flex_Toggle_height_value"))
            .selectedColor($r("app.color.CustomDialogExampleUser_Flex_Toggle_selectedColor_007DFF"))
            .onChange((isOn: boolean) => {
              if (isOn) {
                this.IsOn = true;
                this.userInfoPresenter.setUserCertificateStatus(this.userInfoPresenter.certInfo.uri, true);
              } else {
                this.IsOn = false;
                this.userInfoPresenter.setUserCertificateStatus(this.userInfoPresenter.certInfo.uri, false);
              }
            });
        }
        .height($r("app.float.CustomDialogExampleUser_Flex_height_value"))
        .margin({});
        List() {
          ListItem() {
            Text($r("app.string.CustomDialogExample_firListItem_text"))
              .fontSize($r("sys.float.ohos_id_text_size_body2"))
              .fontColor($r("sys.color.ohos_id_color_text_primary"))
              .fontWeight(FontWeight.Regular)
              .margin({
                top: $r("app.float.wh_value_12")
              })
          }.width('100%').align(Alignment.Start)

          ListItem() {
            DialogSubjectComponent({ map: this.userInfoPresenter.certInfo.subjectNameMap });
          }.width('100%').align(Alignment.Start)

          ListItem() {
            Text($r("app.string.CustomDialogExample_secListItem_text"))
              .fontSize($r("sys.float.ohos_id_text_size_body2"))
              .fontColor($r("sys.color.ohos_id_color_text_primary"))
              .fontWeight(FontWeight.Regular)
              .margin({
                top: $r("app.float.wh_value_12")
              })
          }.width('100%').align(Alignment.Start)

          ListItem() {
            DialogIssuerComponent({ map: this.userInfoPresenter.certInfo.issuerNameMap });
          }.width('100%').align(Alignment.Start)

          ListItem() {
            Text($r("app.string.CustomDialogExample_thdListItem_text"))
              .fontSize($r("sys.float.ohos_id_text_size_body2"))
              .fontColor($r("sys.color.ohos_id_color_text_primary"))
              .fontWeight(FontWeight.Regular)
              .margin({
                top: $r("app.float.wh_value_12")
              })
          }.width('100%').align(Alignment.Start)

          ListItem() {
            DialogDateComponent({ map: this.userInfoPresenter.certInfo.dateMap });
          }.width('100%').align(Alignment.Start)

          ListItem() {
            Text($r("app.string.CustomDialogExample_fouListItem_text"))
              .fontSize($r("sys.float.ohos_id_text_size_body2"))
              .fontColor($r("sys.color.ohos_id_color_text_primary"))
              .fontWeight(FontWeight.Regular)
              .margin({
                top: $r("app.float.wh_value_12")
              })
          }.width('100%').align(Alignment.Start)

          ListItem() {
            DialogFingerPrintComponent({ fingerprintSha256: this.userInfoPresenter.certInfo.fingerprintSha256 });
          }.width('100%').align(Alignment.Start)
        }
        .height($r("app.float.CustomDialogExample_list_height_value"))
        .margin({
          bottom: $r("app.float.CustomDialogExample_secListItem_Text_margin_left_value"),
          left: $r("app.float.wh_value_24"),
          right: $r("app.float.wh_value_24")
        })
        .borderRadius($r("app.float.CustomDialogExample_list_borderRadius_value"))
        .scrollBar(BarState.Auto);

        Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceAround }) {
          Button($r("app.string.CustomDialogExampleUser_Flex_firButton_text"))
            .onClick(() => {
              this.userInfoPresenterForAbstractList.updateUserTrustedCertificateList();
              this.isShowWarnDialog = false;
              this.controller.close();
            })
            .backgroundColor($r("app.color.CustomDialogExample_Button_backgroundColor_FFFFFF"))
            .fontSize($r("sys.float.ohos_id_text_size_button1"))
            .width($r('app.float.wh_value_152'))
            .height($r('app.float.wh_value_40'))
            .fontColor($r("sys.color.ohos_id_color_text_hyperlink"))
          Divider()
            .strokeWidth(ConfigData.VERTICAL_DIVIDER_WIDTH)
            .color($r("sys.color.ohos_id_color_list_separator"))
            .vertical(true)
            .height($r('app.float.wh_value_40'))
            .opacity($r("app.float.opacity_0_2"))
          Button($r("app.string.CustomDialogExampleUser_Flex_secButton_text"))
            .onClick(() => {
              this.isShowWarnDialog = true;
              this.controller.close();
            })
            .backgroundColor($r("app.color.CustomDialogExample_Button_backgroundColor_FFFFFF"))
            .fontSize($r("sys.float.ohos_id_text_size_button1"))
            .width($r('app.float.wh_value_152'))
            .height($r('app.float.wh_value_40'))
            .fontColor($r("sys.color.ohos_id_color_warning"))
        }
        .height($r('app.float.wh_value_64'))
        .padding({
          bottom: $r('app.float.wh_value_16')
        })
        .margin({
          top: $r('app.float.wh_value_16'),
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
      }
  }
}

@CustomDialog
@Component
struct CustomDialogExampleSystem {
  controller: CustomDialogController;
  @Link sysInfoPresenter: CMShowSysCaPresenter;
  private isTrueButton: string = '';

  build() {
      Column() {
        Text($r("app.string.CustomDialogExample_firText"))
          .fontSize($r("sys.float.ohos_id_text_size_dialog_tittle"))
          .fontColor($r("sys.color.ohos_id_color_text_primary"))
          .fontWeight(FontWeight.Medium)
          .margin({
            left: $r("app.float.CustomDialogExample_firText_margin_left_value"),
            top: $r("app.float.CustomDialogExample_firText_margin_top_value")
          })
          .alignSelf(ItemAlign.Start)
        Text(this.sysInfoPresenter.certInfo.certAlias)
          .fontSize($r("sys.float.ohos_id_text_size_body1"))
          .fontColor($r("sys.color.ohos_id_color_text_primary"))
          .fontWeight(FontWeight.Medium)
          .margin({
            top: $r("app.float.wh_value_16"),
            left: $r("app.float.CustomDialogExample_secText_margin_left_value")
          })
          .alignSelf(ItemAlign.Start)
        List() {
          ListItem() {
            Text($r("app.string.CustomDialogExample_firListItem_text"))
              .fontSize($r("sys.float.ohos_id_text_size_body2"))
              .fontColor($r("sys.color.ohos_id_color_text_primary"))
              .fontWeight(FontWeight.Regular)
          }.width('100%').align(Alignment.Start)

          ListItem() {
            DialogSubjectComponent({ map: this.sysInfoPresenter.certInfo.subjectNameMap });
          }.width('100%').align(Alignment.Start)

          ListItem() {
            Text($r("app.string.CustomDialogExample_secListItem_text"))
              .fontSize($r("sys.float.ohos_id_text_size_body2"))
              .fontColor($r("sys.color.ohos_id_color_text_primary"))
              .fontWeight(FontWeight.Regular)
              .margin({
                top: $r("app.float.wh_value_12")
              })
          }.width('100%').align(Alignment.Start)

          ListItem() {
            DialogIssuerComponent({ map: this.sysInfoPresenter.certInfo.issuerNameMap });
          }.width('100%').align(Alignment.Start)

          ListItem() {
            Text($r("app.string.CustomDialogExample_thdListItem_text"))
              .fontSize($r("sys.float.ohos_id_text_size_body2"))
              .fontColor($r("sys.color.ohos_id_color_text_primary"))
              .fontWeight(FontWeight.Regular)
              .margin({
                top: $r("app.float.wh_value_12")
              })
          }.width('100%').align(Alignment.Start)

          ListItem() {
            DialogDateComponent({ map: this.sysInfoPresenter.certInfo.dateMap });
          }.width('100%').align(Alignment.Start)

          ListItem() {
            Text($r("app.string.CustomDialogExample_fouListItem_text"))
              .fontSize($r("sys.float.ohos_id_text_size_body2"))
              .fontColor($r("sys.color.ohos_id_color_text_primary"))
              .fontWeight(FontWeight.Regular)
              .margin({
                top: $r("app.float.wh_value_12")
              })
          }.width('100%').align(Alignment.Start)

          ListItem() {
            DialogFingerPrintComponent({ fingerprintSha256: this.sysInfoPresenter.certInfo.fingerprintSha256 });
          }.width('100%').align(Alignment.Start)
        }
        .height($r("app.float.CustomDialogExample_list_height_value"))
        .margin({
          left: $r("app.float.wh_value_24"),
          top: $r("app.float.CustomDialogExample_list_margin_top_value"),
          bottom: $r("app.float.CustomDialogExample_secListItem_Text_margin_left_value"),
          right: $r("app.float.wh_value_24")
        })
        .borderRadius($r("app.float.CustomDialogExample_list_borderRadius_value"))
        .scrollBar(BarState.Auto);

        Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceAround }) {
          Button($r("app.string.CustomDialogExample_Button_text"))
            .onClick(() => {
              this.controller.close();
            })
            .backgroundColor($r("app.color.CustomDialogExample_Button_backgroundColor_FFFFFF"))
            .fontSize($r("sys.float.ohos_id_text_size_button1"))
            .width($r('app.float.wh_value_152'))
            .height($r('app.float.wh_value_40'))
            .fontColor($r("sys.color.ohos_id_color_text_hyperlink"))
          Divider()
            .strokeWidth(ConfigData.VERTICAL_DIVIDER_WIDTH)
            .color($r("sys.color.ohos_id_color_list_separator"))
            .vertical(true)
            .height($r('app.float.wh_value_40'))
            .opacity($r("app.float.opacity_0_2"))
          if (this.sysInfoPresenter.certInfo.status) {
            Button($r("app.string.CustomDialogExample_Button_off"))
              .onClick(() => {
                this.sysInfoPresenter.setSystemCertificateStatus(this.sysInfoPresenter.certInfo.uri, false);
                this.controller.close();
              })
              .backgroundColor($r("app.color.CustomDialogExample_Button_backgroundColor_FFFFFF"))
              .fontSize($r("sys.float.ohos_id_text_size_button1"))
              .width($r('app.float.wh_value_152'))
              .height($r('app.float.wh_value_40'))
              .fontColor($r("sys.color.ohos_id_color_text_hyperlink"))
          } else {
            Button($r("app.string.CustomDialogExample_Button_on"))
              .onClick(() => {
                this.sysInfoPresenter.setSystemCertificateStatus(this.sysInfoPresenter.certInfo.uri, true);
                this.controller.close();
              })
              .backgroundColor($r("app.color.CustomDialogExample_Button_backgroundColor_FFFFFF"))
              .fontSize($r("sys.float.ohos_id_text_size_button1"))
              .width($r('app.float.wh_value_152'))
              .height($r('app.float.wh_value_40'))
              .fontColor($r("sys.color.ohos_id_color_text_hyperlink"))
          }
        }
        .height($r('app.float.wh_value_64'))
        .padding({
          bottom: $r('app.float.wh_value_16')
        })
        .margin({
          top: $r('app.float.wh_value_16'),
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
      }
  }
}

@Component
struct ComponentSystem {
  private certAlias: string;
  private subjectName: string;
  private Status: boolean;
  private Index : number;
  private Uri: string;
  isSwitchTouched : boolean = false;
  @State isTouched: boolean = false;
  @Link setStatus: CMShowSysCaPresenter;
  dialogController: CustomDialogController = new CustomDialogController({
    builder: CustomDialogExampleSystem({ sysInfoPresenter: $setStatus }),
    autoCancel: true,
    alignment: DialogAlignment.Bottom,
    offset: { dx: 0, dy: ConfigData.DIALOG_DY_OFFSET }
  })

  build() {
    Flex({  justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
      Column() {
        Text(this.certAlias)
          .height($r("app.float.ComponentSystem_firText_height_value"))
          .fontSize($r("sys.float.ohos_id_text_size_body1"))
          .fontColor($r("sys.color.ohos_id_color_text_primary"))
          .fontWeight(FontWeight.Medium)
          .textAlign(TextAlign.Start)
          .margin({top: $r("app.float.wh_value_10") });
        Text(this.subjectName)
          .height($r("app.float.ComponentSystem_secText_height_value"))
          .fontSize($r("sys.float.ohos_id_text_size_body2"))
          .fontColor($r("sys.color.ohos_id_color_text_secondary"))
          .fontWeight(FontWeight.Regular)
          .textAlign(TextAlign.Start)
          .margin({ top: $r("app.float.wh_value_2"), bottom: $r("app.float.wh_value_11") });
      }.alignItems(HorizontalAlign.Start);
      Toggle({ type: ToggleType.Switch, isOn: this.setStatus.certList[this.Index].status })
        .width($r("app.float.ComponentSystem_Toggle_width_value"))
        .height($r("app.float.ComponentSystem_Toggle_height_value"))
        .selectedColor($r("app.color.ComponentSystem_Toggle_selectedColor_007DFF"))
        .onChange((isOn: boolean) => {
          if (isOn) {
            console.info('Component status:' + isOn);
            this.setStatus.setSystemCertificateStatus(this.Uri, true);
          } else {
            console.info('Component status:' + isOn);
            this.setStatus.setSystemCertificateStatus(this.Uri, false);
          }
        })
        .offset({ x: ConfigData.SWITCH_BUTTON_X_OFFSET, y: 0 })
    }
    .height($r("app.float.ComponentSystem_Flex_height_value"))
    .borderRadius($r("app.float.ComponentSystem_Flex_borderRadius_value"))
    .onClick(() => {
      this.setStatus.getSystemTrustedCertificate(this.Uri, () => {
        console.info('this.setStatus.certInfo.status is:' + JSON.stringify(this.setStatus.certInfo));
        this.dialogController.open();
      });
    })
  }
}

@Component
struct ComponentUser {
  private certAlias: string;
  private subjectName: string;
  private Index : number = -1;
  private Uri: string;
  @State StatusText : Resource = $r("app.string.CustomDialogExampleUser_Status_false");
  @State isTouched: boolean = false;
  @Link setStatus: CMShowUserCaPresenter;
  @State mUserCaPresenterDetail: CMShowUserCaPresenter = CMShowUserCaPresenter.getInstance();
  @State @Watch("onShowDeleteWarnDialog") isShowWarning: boolean = false;

  dialogController: CustomDialogController = new CustomDialogController({
    builder: CustomDialogExampleUser({ isShowWarnDialog: $isShowWarning,userInfoPresenter: $mUserCaPresenterDetail,
      userInfoPresenterForAbstractList: $setStatus }),
    autoCancel: true,
    cancel: () => {
      this.setStatus.updateUserTrustedCertificateList();
    },
    alignment: DialogAlignment.Bottom,
    offset: { dx: 0, dy: ConfigData.DIALOG_DY_OFFSET }
  })

  deleteWarnDialog: CustomDialogController = new CustomDialogController({
    builder: CustomDialogDeleteWarning({
      isShowWarnDialog: $isShowWarning,
      deleteWarn: $mUserCaPresenterDetail,
      mShowUserListPresenter: $setStatus }),
    autoCancel: false,
    alignment:DialogAlignment.Bottom,
    offset: { dx: 0, dy: ConfigData.DIALOG_DY_OFFSET }
  })

  onShowDeleteWarnDialog() {
    if (this.isShowWarning == true) {
      this.deleteWarnDialog.open();
    } else {
      this.dialogController.close();
      this.deleteWarnDialog.close();
    }
  }

  aboutToAppear() {
    LogUtil.info('in ComponentUser aboutToAppear');
    if (this.setStatus.certList[this.Index].status == true) {
      this.StatusText = $r("app.string.CustomDialogExampleUser_Status_true");
    } else {
      this.StatusText = $r("app.string.CustomDialogExampleUser_Status_false");
    }
  }

  build() {
    Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
      Column() {
        Text(this.certAlias)
          .height($r("app.float.componentUser_firText_height_value"))
          .fontSize($r("sys.float.ohos_id_text_size_body1"))
          .fontColor($r("sys.color.ohos_id_color_text_primary"))
          .fontWeight(FontWeight.Medium)
          .textAlign(TextAlign.Start)
          .margin({ top: $r("app.float.wh_value_10") });
        Text(this.subjectName)
          .height($r("app.float.componentUser_secText_height_value"))
          .fontSize($r("sys.float.ohos_id_text_size_body2"))
          .fontColor($r("sys.color.ohos_id_color_text_primary"))
          .fontWeight(FontWeight.Regular)
          .textAlign(TextAlign.Start)
          .margin({ top: $r("app.float.wh_value_2"), bottom: $r("app.float.wh_value_11") });
      }.alignItems(HorizontalAlign.Start);
      Row() {
        Text(this.StatusText)
          .fontSize($r("sys.float.ohos_id_text_size_body2"))
          .fontColor($r("sys.color.ohos_id_color_text_secondary"))
          .textAlign(TextAlign.End)
          .margin({
            right: $r("app.float.componentUser_thdText_margin_value")
          })
        Image($r('app.media.ic_settings_arrow'))
          .width($r("app.float.componentUser_image_width_value"))
          .height($r("app.float.componentUser_image_height_value"))
          .fillColor($r("sys.color.ohos_id_color_primary"))
          .opacity($r("app.float.componentUser_image_opacity_value"));
      }
    }
    .height($r("app.float.componentUser_flex_height_value"))
    .borderRadius($r("app.float.componentUser_flex_borderRadius_value"))
    .onClick(() => {
      this.mUserCaPresenterDetail.getUserTrustedCertificate(this.Uri, () => {
        console.info('userCerInfo is:' + JSON.stringify(this.setStatus.certInfo));
        this.dialogController.open();
      });
    });
  }
}

@Entry
@Component
struct TrustedEvidence {
  @State mShowSysCaPresenter: CMShowSysCaPresenter = CMShowSysCaPresenter.getInstance();
  @State mShowUserCaPresenter: CMShowUserCaPresenter = CMShowUserCaPresenter.getInstance();
  @State mFaPresenter: CMFaPresenter = CMFaPresenter.getInstance();
  @State isTouched: boolean = false;
  @State currentIndex: number = 0
  @State fontColor: Resource = $r("app.color.TrustedEvidence_TabBuilder_fontColor_182431")
  @State selectedFontColor: Resource = $r("app.color.font_color_007DFF")
  private controller: TabsController = new TabsController();

  @Builder TabBuilder(index: number) {
    Column() {
      Text(index == 0 ? $r('app.string.system') : $r('app.string.user'))
        .fontColor(this.currentIndex === index ? this.selectedFontColor : this.fontColor)
        .fontSize($r('app.float.TrustedEvidence_TabBuilder_Text_fontSize_value'))
        .fontWeight(this.currentIndex === index ? FontWeight.Medium : FontWeight.Regular)
        .alignSelf(ItemAlign.Center)
        .margin({
          top: $r('app.float.TrustedEvidence_TabBuilder_Text_padding_top_value')
        })
      if (this.currentIndex === index) {
        Divider()
          .width($r('app.float.TrustedEvidence_TabBuilder_Divider_width_value'))
          .margin({ top: $r('app.float.TrustedEvidence_TabBuilder_Divider_padding_top_value') })
          .strokeWidth(2)
          .color(this.selectedFontColor)
          .alignSelf(ItemAlign.Center)
      }
    }
    .width(ConfigData.WH_100_100)
  }

  aboutToAppear() {
    LogUtil.info('in aboutToAppear');
    this.mShowSysCaPresenter.updateSystemTrustedCertificateList();
    this.mShowUserCaPresenter.updateUserTrustedCertificateList();
  }

  onPageShow() {
    let uri = globalThis.abilityWant.uri;
    globalThis.abilityWant.uri = "";

    if (uri === "certInstall") {
      this.mFaPresenter.startInstall();
    } else if (uri === "requestAuthorize") {
      this.mFaPresenter.startRequestAuth(globalThis.abilityWant.parameters.appUid);
    }
  }

  build() {
    Column() {
      GridContainer({columns:12, sizeType: SizeType.Auto, gutter: vp2px(1) === 2 ? '12vp' : '0vp',
        margin: vp2px(1) === 2 ? '24vp' : '0vp'}) {
        Row() {
          Column() {
            HeadComponent({ headName: $r('app.string.trustedEvidence') })
            Column() {
              Tabs({ barPosition: BarPosition.Start, index: 0, controller: this.controller }) {
                TabContent() {
                  List() {
                    ForEach(this.mShowSysCaPresenter.certList, (item, index) => {
                      ListItem() {
                        ComponentSystem({ certAlias: item.certAlias, subjectName: item.SubjectNameCN, Status: item.status,
                          Uri: item.uri, setStatus: $mShowSysCaPresenter, Index: index })
                      }
                    }, item => JSON.stringify(item))
                  }
                  .margin({
                    left: $r("app.float.wh_value_12"),
                    right: $r("app.float.wh_value_12")
                  })
                  .padding({
                    left: $r("app.float.wh_value_12"),
                    top: $r("app.float.wh_value_4"),
                    bottom: $r("app.float.wh_value_4")
                  })
                  .divider({
                    strokeWidth: $r("app.float.sys_list_divider_strokeWidth_value"),
                    color: $r("app.color.sys_list_divider_color_000000"),
                    endMargin: '12vp'
                  })
                  .borderRadius($r("sys.float.ohos_id_corner_radius_default_l"))
                  .backgroundColor($r("app.color.sys_list_backgroundColor_FFFFFF"))
                  .visibility(this.mShowSysCaPresenter.certList.length > 0 ? Visibility.Visible : Visibility.None);
                }
                .tabBar(this.TabBuilder(0))
                .onTouch((event: TouchEvent) => {
                  if (event.type === TouchType.Down) {
                    this.isTouched = true;
                  }
                  if (event.type === TouchType.Up) {
                    this.isTouched = false;
                  }
                });

                TabContent() {
                  List() {
                    ForEach(this.mShowUserCaPresenter.certList, (item, index) => {
                      ListItem() {
                        ComponentUser({ certAlias: item.certAlias, subjectName: item.SubjectNameCN, Uri: item.uri,
                          setStatus: $mShowUserCaPresenter, Index: index })
                      }
                    }, item => JSON.stringify(item))
                  }
                  .margin({
                    left: $r("app.float.wh_value_12"),
                    right: $r("app.float.wh_value_12")
                  })
                  .padding({
                    left: $r("app.float.wh_value_12"),
                    right: $r("app.float.wh_value_8"),
                    top: $r("app.float.wh_value_4"),
                    bottom: $r("app.float.wh_value_4")
                  })
                  .divider({
                    strokeWidth: $r("app.float.user_list_divider_strokeWidth_value"),
                    color: $r("app.color.user_list_divider_color_000000"),
                    endMargin: '3vp'
                  })
                  .borderRadius($r("app.float.user_list_divider_borderRadius_value"))
                  .backgroundColor($r("app.color.user_list_backgroundColor_FFFFFF"))
                  .visibility(this.mShowUserCaPresenter.certList.length > 0 ? Visibility.Visible : Visibility.None)
                }
                .tabBar(this.TabBuilder(1))
                .onTouch((event: TouchEvent) => {
                  if (event.type === TouchType.Down) {
                    this.isTouched = true;
                  }
                  if (event.type === TouchType.Up) {
                    this.isTouched = false;
                  }
                });
              }
              .vertical(false)
              .scrollable(true)
              .barMode(BarMode.Fixed)
              .barWidth($r("app.float.tabs_barWidth_value"))
              .barHeight($r("app.float.tabs_barHeight_value"))
              .animationDuration(400)
              .width(ConfigData.WH_100_100)
              .backgroundColor($r("app.color.TrustedEvidence_tabs_backgroundColor_F1F3F5"))
              .onChange((index: number) => {
                this.currentIndex = index
              });
            }
            .height(ConfigData.WH_94_100)
            .width(ConfigData.WH_100_100);
          }
          .backgroundColor($r("sys.color.ohos_id_color_sub_background"))
          .width(ConfigData.WH_100_100)
          .height(ConfigData.WH_100_100)
          .useSizeType({
            xs: { span: 12, offset: 0 }, sm: { span: 12, offset: 0 },
            md: { span: 12, offset: 0 }, lg: { span: 8, offset: 2 }
          });

          Column() {
          }
          .width(ConfigData.WH_100_100)
          .height(ConfigData.WH_100_100)
          .useSizeType({
            xs: { span: 0, offset: 12 }, sm: { span: 0, offset: 12 },
            md: { span: 0, offset: 12 }, lg: { span: 2, offset: 10 }
          });
        }
        .width(ConfigData.WH_100_100)
        .height(ConfigData.WH_100_100);
      }
      .width(ConfigData.WH_100_100)
      .height(ConfigData.WH_100_100);
    }
    .backgroundColor($r("sys.color.ohos_id_color_sub_background"))
    .width(ConfigData.WH_100_100)
    .height(ConfigData.WH_100_100);
  }
}
