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
import CMShowAppCredPresenter from '../presenter/CMShowAppCredPresenter';
import CMShowPrivateCredPresenter from '../presenter/CMShowPrivateCredPresenter';
import CMFaPresenter from '../presenter/CMFaPresenter';

@Component
export struct DialogComponent {
  @Link uidItem: CMShowAppCredPresenter;
  private AppName: string;
  private AppImage: string;
  private Index: number;

  build() {
    Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
      Row() {
        Image(this.AppImage)
          .width($r("app.float.credentials_app_image_wh"))
          .height($r("app.float.credentials_app_image_hg"))

        Text(this.AppName)
          .fontSize($r("sys.float.ohos_id_text_size_body2"))
          .fontColor($r("sys.color.ohos_id_color_text_primary"))
          .fontWeight(FontWeight.Medium)
          .margin({ left: $r("app.float.credentials_app_name_margin") })
      }
      .alignItems(VerticalAlign.Center)

      Toggle({ type: ToggleType.Switch, isOn: true })
        .width($r("app.float.credentials_app_Toggle_wh"))
        .height($r("app.float.credentials_app_Toggle_hg"))
        .selectedColor($r("app.color.credentials_app_Toggle_selectColor"))
        .onChange((isOn: boolean) => {
          this.uidItem.appInfoList[this.Index].status = isOn
        })
        .offset({ x: 7, y:0 })

    }.height($r("app.float.wh_value_72"))
  }
}

@CustomDialog
@Component
struct CustomDialogExampleAuthor {
  controller: CustomDialogController;
  @Link isShowAuthDialog: boolean;
  @Link AuthorInfo: CMShowAppCredPresenter;
  @State updateStatusList: boolean[] = [];

  build() {
    Column() {
      Text($r("app.string.managerAuthApp"))
        .height($r("app.float.wh_value_56"))
        .fontSize($r("sys.float.ohos_id_text_size_dialog_tittle"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Medium)
        .margin({
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
        .alignSelf(ItemAlign.Start);
      if (this.AuthorInfo.appInfoList.length > ConfigData.APP_AUTH_MAX_LENGTH) {
        List() {
          ForEach(this.AuthorInfo.appInfoList, (item, index) => {
            ListItem() {
              DialogComponent({ AppImage: item.appImage, AppName: item.appName, Index: index, uidItem: $AuthorInfo })
            }
          })
        }
        .margin({
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
        .height(ConfigData.WH_50_100)
        .divider({
          strokeWidth: $r("app.float.Evidence_strokeWidth"),
          color: $r("app.color.Evidence_color")})
        .borderRadius($r("app.float.Evidence_borderRadius"))
        .backgroundColor($r("app.color.credentials_app_finish_backgroundColor"))
        .visibility(this.AuthorInfo.appInfoList.length > 0 ? Visibility.Visible : Visibility.None)
      } else {
        List() {
          ForEach(this.AuthorInfo.appInfoList, (item, index) => {
            ListItem() {
              DialogComponent({ AppImage: item.appImage, AppName: item.appName, Index: index, uidItem: $AuthorInfo })
            }
          })
        }
        .margin({
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
        .divider({
          strokeWidth: $r("app.float.Evidence_strokeWidth"),
          color: $r("app.color.Evidence_color")})
        .borderRadius($r("app.float.Evidence_borderRadius"))
        .backgroundColor($r("app.color.credentials_app_finish_backgroundColor"))
        .visibility(this.AuthorInfo.appInfoList.length > 0 ? Visibility.Visible : Visibility.None)
      }

      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceAround }) {
        Button($r("app.string.cancelAuthApp"))
          .onClick(() => {
            this.isShowAuthDialog = false;
            this.controller.close()
          })
          .backgroundColor($r("app.color.credentials_app_finish_backgroundColor"))
          .fontSize($r("sys.float.ohos_id_text_size_button1"))
          .fontColor($r("sys.color.ohos_id_color_text_hyperlink"))
          .width($r('app.float.wh_value_152'))
          .height($r('app.float.wh_value_40'))
        Divider()
          .strokeWidth(ConfigData.VERTICAL_DIVIDER_WIDTH)
          .color($r("sys.color.ohos_id_color_list_separator"))
          .vertical(true)
          .opacity($r("app.float.opacity_0_2"))
          .height($r('app.float.wh_value_40'))
        Button($r("app.string.finishAuthApp"))
          .onClick(() => {
            this.AuthorInfo.removeGrantedAppList(this.AuthorInfo.credInfo.keyUri).then(() => {
              this.isShowAuthDialog = false;
              this.controller.close()
            })
          })
          .backgroundColor($r("app.color.credentials_app_finish_backgroundColor"))
          .fontSize($r("sys.float.ohos_id_text_size_button1"))
          .fontColor($r("sys.color.ohos_id_color_text_hyperlink"))
          .width($r('app.float.wh_value_152'))
          .height($r('app.float.wh_value_40'))
      }
      .height($r('app.float.wh_value_64'))
      .padding({
        bottom: $r('app.float.wh_value_16')
      })
      .margin({
        top: $r('app.float.wh_value_6'),
        left: $r('app.float.wh_value_24'),
        right: $r('app.float.wh_value_24')
      })
    }
  }
}

@CustomDialog
@Component
struct CustomDialogExamplePrivate {
  controller: CustomDialogController;
  @State PriCerInfoPresenter: CMShowPrivateCredPresenter = CMShowPrivateCredPresenter.getInstance();

  build() {
    Column() {
      Text($r("app.string.evidenceDetails"))
        .height($r("app.float.wh_value_56"))
        .fontSize($r("sys.float.ohos_id_text_size_dialog_tittle"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Medium)
        .margin({
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
        .alignSelf(ItemAlign.Start)

      Text(this.PriCerInfoPresenter.credInfo.alias)
        .fontSize($r("sys.float.ohos_id_text_size_body1"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Medium)
        .margin({
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
        .alignSelf(ItemAlign.Start)

      Text($r("app.string.entryContains"))
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
        .margin({
          top: $r('app.float.wh_value_24'),
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
        .alignSelf(ItemAlign.Start)

      Text($r("app.string.keyNum", String(this.PriCerInfoPresenter.credInfo.keyNum)))
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
        .margin({
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
        .opacity($r('app.float.opacity_100_60'))
        .alignSelf(ItemAlign.Start)

      Text($r("app.string.userCerNum", String(this.PriCerInfoPresenter.credInfo.certNum)))
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
        .margin({
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
        .opacity($r('app.float.opacity_100_60'))
        .alignSelf(ItemAlign.Start)

      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceAround }) {
        Button($r("app.string.privateDetailsClose"))
          .onClick(() => {
            this.controller.close()
          })
          .backgroundColor($r("app.color.credentials_app_finish_backgroundColor"))
          .fontSize($r("sys.float.ohos_id_text_size_button1"))
          .fontColor($r("sys.color.ohos_id_color_text_hyperlink"))
          .width($r('app.float.wh_value_152'))
          .height($r('app.float.wh_value_40'))
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
struct CustomDialogDeleteWarning {
  controller: CustomDialogController;
  @Link deleteWarn: CMShowAppCredPresenter;
  @Link isShowWarnDialog: boolean;

  build() {
    Column() {
      Text($r("app.string.warning_title"))
        .height($r("app.float.wh_value_56"))
        .fontSize($r("sys.float.ohos_id_text_size_dialog_tittle"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Medium)
        .margin({
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
        .alignSelf(ItemAlign.Start)
      Text($r("app.string.warning_message"))
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
          .backgroundColor($r("app.color.credentials_app_finish_backgroundColor"))
          .fontSize($r("sys.float.ohos_id_text_size_button1"))
          .fontColor($r("sys.color.ohos_id_color_text_hyperlink"))
          .width($r('app.float.wh_value_152'))
          .height($r('app.float.wh_value_40'))
        Divider()
          .strokeWidth(ConfigData.VERTICAL_DIVIDER_WIDTH)
          .color($r("sys.color.ohos_id_color_list_separator"))
          .vertical(true)
          .height($r('app.float.wh_value_40'))
          .opacity($r("app.float.opacity_0_2"))
        Button($r("app.string.deleteAllCredDelete"))
          .onClick(() => {
            this.deleteWarn.deleteAppCred(this.deleteWarn.credInfo.keyUri);
            this.isShowWarnDialog = false;
            this.controller.close()
          })
          .backgroundColor($r("app.color.credentials_app_finish_backgroundColor"))
          .fontSize($r("sys.float.ohos_id_text_size_button1"))
          .fontColor($r("sys.color.ohos_id_color_warning"))
          .width($r('app.float.wh_value_152'))
          .height($r('app.float.wh_value_40'))
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
struct CustomDialogExampleApp {
  controller: CustomDialogController;
  @Link mShowAppCaPresenter: CMShowAppCredPresenter;
  @Link isShowAuthDialog: boolean;
  @Link isShowWarnDialog: boolean;

  aboutToAppear() {
    this.mShowAppCaPresenter.getAuthorizedAppList(this.mShowAppCaPresenter.credInfo.keyUri)
  }

  build() {
    Column() {
      Text($r("app.string.evidenceDetails"))
        .height($r("app.float.wh_value_56"))
        .fontSize($r("sys.float.ohos_id_text_size_dialog_tittle"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Medium)
        .margin({
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
        .alignSelf(ItemAlign.Start)

      Text(this.mShowAppCaPresenter.credInfo.alias)
        .fontSize($r("sys.float.ohos_id_text_size_body1"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Medium)
        .margin({
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
        .alignSelf(ItemAlign.Start)

      Text($r("app.string.entryContains"))
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
        .margin({
          top: $r('app.float.wh_value_24'),
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
        .alignSelf(ItemAlign.Start)

      Text($r("app.string.keyNum", String(this.mShowAppCaPresenter.credInfo.keyNum)))
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
        .margin({
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
        .opacity($r('app.float.opacity_100_60'))
        .alignSelf(ItemAlign.Start)

      Text($r("app.string.userCerNum", String(this.mShowAppCaPresenter.credInfo.certNum)))
        .fontSize($r("sys.float.ohos_id_text_size_body2"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
        .margin({
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
        .opacity($r('app.float.opacity_100_60'))
        .alignSelf(ItemAlign.Start)

      Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
        Column() {
          Text($r("app.string.managerAuthApp"))
            .fontSize($r("sys.float.ohos_id_text_size_body1"))
            .fontColor($r("sys.color.ohos_id_color_text_primary"))
            .fontWeight(FontWeight.Medium)
        }

        Row() {
          Image($r('app.media.ic_settings_arrow'))
            .width($r("app.float.managerAuthApp_image_wh"))
            .height($r("app.float.managerAuthApp_image_hg"))
            .fillColor($r("sys.color.ohos_id_color_primary"))
            .opacity($r("app.float.managerAuthApp_image_opacity"))
        }
      }
      .onClick(() => {
        this.isShowAuthDialog = true;
      })
      .margin({
        top: $r('app.float.wh_value_12'),
        left: $r('app.float.wh_value_24'),
        right: $r('app.float.wh_value_24')
      })
      .height('48vp')

      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceAround }) {
        Button($r("app.string.publicDetailsCancel"))
          .onClick(() => {
            this.isShowAuthDialog = false;
            this.isShowWarnDialog = false;
            this.controller.close();
          })
          .backgroundColor($r("app.color.credentials_app_finish_backgroundColor"))
          .fontSize($r("sys.float.ohos_id_text_size_button1"))
          .fontColor($r("sys.color.ohos_id_color_text_hyperlink"))
          .width($r('app.float.wh_value_152'))
          .height($r('app.float.wh_value_40'))
        Divider()
          .strokeWidth(ConfigData.VERTICAL_DIVIDER_WIDTH)
          .color($r("sys.color.ohos_id_color_list_separator"))
          .vertical(true)
          .height($r('app.float.wh_value_40'))
          .opacity($r("app.float.opacity_0_2"))
        Button($r("app.string.publicDetailsDelete"))
          .onClick(() => {
            this.isShowWarnDialog = true;
            this.isShowAuthDialog = false;
            this.controller.close()
          })
          .backgroundColor($r("app.color.credentials_app_finish_backgroundColor"))
          .fontSize($r("sys.float.ohos_id_text_size_button1"))
          .fontColor($r("sys.color.ohos_id_color_warning"))
          .width($r('app.float.wh_value_152'))
          .height($r('app.float.wh_value_40'))
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
export struct componentPublic {
  private Alias: string;
  private KeyUri: string;
  @Link mShowAppCaPresenter: CMShowAppCredPresenter;
  @State @Watch("onShowAuthMngChange") isShowAuthMng: boolean = false;
  @State @Watch("onShowDeleteWarnDialog") isShowWarning: boolean = false;
  detailsDialogController: CustomDialogController = new CustomDialogController({
    builder: CustomDialogExampleApp({
      isShowAuthDialog: $isShowAuthMng,
      isShowWarnDialog: $isShowWarning,
      mShowAppCaPresenter: $mShowAppCaPresenter
    }),
    autoCancel: true,
    alignment: DialogAlignment.Bottom,
    offset: { dx: 0, dy: ConfigData.DIALOG_DY_OFFSET }
  })
  appDialogController: CustomDialogController = new CustomDialogController({
    builder: CustomDialogExampleAuthor({ isShowAuthDialog: $isShowAuthMng, AuthorInfo: $mShowAppCaPresenter }),
    autoCancel: false,
    alignment: DialogAlignment.Bottom,
    offset: { dx: 0, dy: ConfigData.DIALOG_DY_OFFSET }
  })
  deleteWarnDialog: CustomDialogController = new CustomDialogController({
    builder: CustomDialogDeleteWarning({ isShowWarnDialog: $isShowWarning, deleteWarn: $mShowAppCaPresenter }),
    autoCancel: false,
    alignment: DialogAlignment.Bottom,
    offset: { dx: 0, dy: ConfigData.DIALOG_DY_OFFSET }
  })

  onShowAuthMngChange() {
    if (this.isShowAuthMng == true) {
      this.appDialogController.open();
    } else {
      this.detailsDialogController.close();
      this.detailsDialogController.open();
      this.appDialogController.close();
    }
  }

  onShowDeleteWarnDialog() {
    if (this.isShowWarning == true) {
      this.deleteWarnDialog.open();
    } else {
      this.detailsDialogController.close();
      this.deleteWarnDialog.close();
    }
  }

  build() {
    Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
      Column() {
        Text(this.Alias)
          .fontSize($r("sys.float.ohos_id_text_size_body1"))
          .fontColor($r("sys.color.ohos_id_color_text_primary"))
          .fontWeight(FontWeight.Medium)
      }
      .margin({
        left: $r("app.float.publicList_margin")
      })

      Row() {
        Image($r('app.media.ic_settings_arrow'))
          .width($r("app.float.publicList_image_wh"))
          .height($r("app.float.publicList_image_hg"))
      }
      .margin({
        left: $r("app.float.publicList_margin"),
        right: $r("app.float.publicList_image_margin")
      })
    }
    .height($r("app.float.wh_value_48"))
    .borderRadius($r("app.float.publicList_borderRadius"))
    .onClick(() => {
      this.mShowAppCaPresenter.getAppCred(this.KeyUri)
      this.detailsDialogController.open()
    })
  }
}

@Component
export struct componentPrivate {
  private Alias: string;
  private KeyUri: string;
  @State PrivateCerPresenter: CMShowPrivateCredPresenter = CMShowPrivateCredPresenter.getInstance();
  privateDetailDialog: CustomDialogController = new CustomDialogController({
    builder: CustomDialogExamplePrivate(),
    autoCancel: true,
    alignment: DialogAlignment.Bottom,
    offset: { dx: 0, dy: ConfigData.DIALOG_DY_OFFSET }
  })

  build() {
    Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
      Column() {
        Text(this.Alias)
          .fontSize($r("sys.float.ohos_id_text_size_body1"))
          .fontColor($r("sys.color.ohos_id_color_text_primary"))
          .fontWeight(FontWeight.Medium)
      }
      .margin({
        left: $r("app.float.publicList_margin")
      })

      Row() {
        Image($r('app.media.ic_settings_arrow'))
          .width($r("app.float.publicList_image_wh"))
          .height($r("app.float.publicList_image_hg"))
      }
      .margin({
        left: $r("app.float.publicList_margin"),
        right: $r("app.float.publicList_image_margin")
      })
    }
    .height($r("app.float.wh_value_48"))
    .borderRadius($r("app.float.publicList_borderRadius"))
    .onClick(() => {
      this.PrivateCerPresenter.getPrivateCred(this.KeyUri)
      this.privateDetailDialog.open()
    })
  }
}

@Entry
@Component
struct evidenceList {
  @State mShowAppCaPresenter: CMShowAppCredPresenter = CMShowAppCredPresenter.getInstance();
  @State mShowPrivateCaPresenter: CMShowPrivateCredPresenter = CMShowPrivateCredPresenter.getInstance();
  @State mFaPresenter: CMFaPresenter = CMFaPresenter.getInstance();
  @State isTouched: boolean = false;
  @State currentIndex: number = 0
  @State fontColor: Resource = $r("app.color.evidenceList_TabBuilder_fontColor_182431")
  private controller: TabsController = new TabsController();

  @Builder TabBuilder(index: number) {
    Column() {
      Text(index == 0 ? $r('app.string.tabName_public') : $r('app.string.tabName_private'))
        .fontColor(this.currentIndex === index ? $r("sys.color.ohos_id_color_bottom_tab_icon") : this.fontColor)
        .fontSize($r('app.float.font_16'))
        .fontWeight(this.currentIndex === index ? FontWeight.Medium : FontWeight.Regular)
        .alignSelf(ItemAlign.Center)
        .margin({
          top: $r('app.float.TrustedEvidence_TabBuilder_Text_padding_top_value') })
      if (this.currentIndex === index) {
        Divider()
          .width($r('app.float.TrustedEvidence_TabBuilder_Divider_width_value'))
          .margin({ top: $r('app.float.TrustedEvidence_TabBuilder_Divider_padding_top_value') })
          .strokeWidth(2)
          .color($r("sys.color.ohos_id_color_bottom_tab_icon"))
          .alignSelf(ItemAlign.Center)
      }
    }
    .width(ConfigData.WH_100_100)
  }

  aboutToAppear() {
    LogUtil.info('in aboutToAppear');
    this.mShowAppCaPresenter.updateAppCredListCallback(() => {
      console.info('get AppCredList first and then get PrivateCredList')
      this.mShowPrivateCaPresenter.updatePrivateCredList();
    })
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
      GridContainer({
        columns: 12,
        sizeType: SizeType.Auto,
        gutter: vp2px(1) === 2 ? '12vp' : '0vp',
        margin: vp2px(1) === 2 ? '24vp' : '0vp'
      }) {
        Row() {
          Column() {
            HeadComponent({ headName: $r('app.string.userEvidence') })
            Column() {
              Tabs({ barPosition: BarPosition.Start, index: 0, controller: this.controller }) {
                TabContent() {
                  List() {
                    ForEach(this.mShowAppCaPresenter.credList, (item) => {
                      ListItem() {
                        componentPublic({
                          Alias: item.alias,
                          KeyUri: item.keyUri,
                          mShowAppCaPresenter: $mShowAppCaPresenter
                        })
                      }
                    }, item => JSON.stringify(item))
                  }
                  .width(ConfigData.WH_92_100)
                  .margin({
                    left: ConfigData.WH_4_100
                  })
                  .divider({
                    strokeWidth: $r("app.float.Evidence_strokeWidth"),
                    color: $r("app.color.Evidence_color"),
                    startMargin: $r("app.float.Evidence_startMargin"),
                    endMargin: $r("app.float.Evidence_endMargin")
                  })
                  .borderRadius($r("app.float.Evidence_borderRadius"))
                  .backgroundColor($r("app.color.credentials_app_finish_backgroundColor"))
                  .visibility(this.mShowAppCaPresenter.credList.length > 0 ? Visibility.Visible : Visibility.None)
                }
                .tabBar(this.TabBuilder(0))
                .onTouch((event: TouchEvent) => {
                  if (event.type === TouchType.Down) {
                    this.isTouched = true;
                  }
                  if (event.type === TouchType.Up) {
                    this.isTouched = false;
                  }
                })

                TabContent() {
                  List() {
                    ForEach(this.mShowPrivateCaPresenter.credList, (item) => {
                      ListItem() {
                        componentPrivate({ Alias: item.alias, KeyUri: item.keyUri })
                      }
                    }, item => JSON.stringify(item))
                  }
                  .width(ConfigData.WH_92_100)
                  .margin({
                    left: ConfigData.WH_4_100
                  })
                  .divider({
                    strokeWidth: $r("app.float.Evidence_strokeWidth"),
                    color: $r("app.color.Evidence_color"),
                    startMargin: $r("app.float.Evidence_startMargin"),
                    endMargin: $r("app.float.Evidence_endMargin") })
                  .borderRadius($r("app.float.Evidence_borderRadius"))
                  .backgroundColor($r("app.color.credentials_app_finish_backgroundColor"))
                  .visibility(this.mShowPrivateCaPresenter.credList.length > 0 ? Visibility.Visible : Visibility.None)
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
              .barWidth('192vp')
              .barHeight($r("app.float.Evidence_barHeight"))
              .animationDuration(400)
              .width(ConfigData.WH_100_100)
              .backgroundColor($r("app.color.TrustedEvidence_tabs_backgroundColor_F1F3F5"))
              .onChange((index: number) => {
                this.currentIndex = index
              });
            }
            .height(ConfigData.WH_94_100)
            .width(ConfigData.WH_100_100)
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
          })
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
