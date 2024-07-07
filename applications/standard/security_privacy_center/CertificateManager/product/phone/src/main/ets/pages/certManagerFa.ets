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

import ConfigData from '../../../../../../common/util/ConfigData';
import HeadComponent from '../../../../../../common/component/headComponent';
import { SubEntryComponent } from '../../../../../../common/component/subEntryComponent';
import CMFaPresenter from '../presenter/CMFaPresenter';

@Entry
@Component
struct CertificateComponent {
  @State touchedItem: boolean = false;
  @State columnMargin: string = '12vp';
  @State mFaPresenter: CMFaPresenter = CMFaPresenter.getInstance();

  build() {
    Column() {

      GridContainer({
        columns: 12,
        sizeType: SizeType.Auto,
        gutter: vp2px(1) === 2 ? '12vp' : '0vp',
        margin: vp2px(1) === 2 ? '24vp' : '0vp'
      }) {
        Row({}) {
          Column() {
          }
          .width(ConfigData.WH_100_100)
          .height(ConfigData.WH_100_100)
          .useSizeType({
            xs: { span: 0, offset: 0 }, sm: { span: 0, offset: 0 },
            md: { span: 0, offset: 0 }, lg: { span: 2, offset: 0 }
          });

          Column() {
            HeadComponent({ headName: $r('app.string.certificateTab') })
            Column({ space: this.columnMargin }) {

              SubEntryComponent({ targetPage: "pages/trustedCa", title: $r("app.string.trustedEvidence") })

              SubEntryComponent({ targetPage: 'pages/cerEvidenceFa', title: $r("app.string.userEvidence") })

              CertInstallComponent({ mFaPresenter: $mFaPresenter })

              DeleteAll({ mFaPresenter: $mFaPresenter })
            }
            .margin({ top: $r("app.float.distance_8") })
            .width(ConfigData.WH_100_100)
          }
          .backgroundColor($r("sys.color.ohos_id_color_sub_background"))
          .padding({ left: $r('app.float.wh_24'), right: $r('app.float.wh_24') })
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

  onPageShow() {
    let uri = globalThis.abilityWant.uri;
    globalThis.abilityWant.uri = "";

    if (uri === "certInstall") {
      this.mFaPresenter.startInstall();
    } else if (uri === "requestAuthorize") {
      this.mFaPresenter.startRequestAuth(globalThis.abilityWant.parameters.appUid);
    }
  }
}

@CustomDialog
@Component
struct CustomDialogExampleDeleteAll {
  controller: CustomDialogController;
  @Link deleteAll: CMFaPresenter;

  build() {
      Column() {
        Text($r("app.string.deleteAllCredDialogTitle"))
          .height($r("app.float.wh_value_56"))
          .fontSize($r("sys.float.ohos_id_text_size_dialog_tittle"))
          .fontColor($r("sys.color.ohos_id_color_text_primary"))
          .fontWeight(FontWeight.Medium)
          .margin({
            left: $r('app.float.wh_value_24'),
            right: $r('app.float.wh_value_24')
          })
          .alignSelf(ItemAlign.Start)
        Text($r("app.string.deleteAllCredDialogMessage"))
          .fontSize($r('sys.float.ohos_id_text_size_body1'))
          .fontWeight(FontWeight.Regular)
          .fontColor($r("sys.color.ohos_id_color_text_primary"))
          .margin({
            top: $r("app.float.wh_value_16"),
            left: $r('app.float.wh_value_24'),
            right: $r('app.float.wh_value_24')
          })
          .alignSelf(ItemAlign.Start)

        Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceAround }) {
          Button($r("app.string.deleteAllCredCancel"))
            .onClick(() => {
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
              this.deleteAll.uninstallAllCert();
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
struct DeleteAll {
  @State isTouched: boolean = false;
  @Link mFaPresenter: CMFaPresenter;

  deleteWarnDialog: CustomDialogController = new CustomDialogController({
    builder: CustomDialogExampleDeleteAll({ deleteAll: $mFaPresenter }),
    autoCancel: true,
    alignment:DialogAlignment.Bottom,
    offset :{ dx:0, dy:ConfigData.DIALOG_DY_OFFSET }
  })

  build() {
    Row() {
      Text($r("app.string.deleteAllCred"))
        .fontSize($r('app.float.font_16'))
        .lineHeight($r('app.float.wh_value_22'))
        .fontWeight(FontWeight.Medium)
        .fontColor($r('app.color.cm_del_all_font_color'))
        .margin({ left: $r('app.float.distance_8') })
        .textAlign(TextAlign.Start);
    }
    .padding($r('app.float.distance_4'))
    .height($r('app.float.wh_value_56'))
    .width(ConfigData.WH_100_100)
    .borderRadius($r('app.float.radius_24'))
    .linearGradient(this.isTouched ? {
                                       angle: 90,
                                       direction: GradientDirection.Right,
                                       colors: [[$r("app.color.DCEAF9"), 0.0], [$r("app.color.FAFAFA"), 1.0]]
                                     } : {
                                           angle: 90,
                                           direction: GradientDirection.Right,
                                           colors: [[$r("sys.color.ohos_id_color_foreground_contrary"), 1],
                                           [$r("sys.color.ohos_id_color_foreground_contrary"), 1]]
                                         })
    .onTouch((event: TouchEvent) => {
      if (event.type === TouchType.Down) {
        this.isTouched = true;
      }
      if (event.type === TouchType.Up) {
        this.isTouched = false;
      }
    })
    .onClick(() => {
      this.deleteWarnDialog.open();
    })
  }
}

@Component
export struct CertInstallComponent {
  @State isTouched: boolean = false;
  @Link mFaPresenter: CMFaPresenter;

  build() {
    Column() {
      Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
        Row() {
          Text($r('app.string.installInStorageDevice'))
            .fontSize($r('app.float.font_16'))
            .lineHeight($r('app.float.wh_value_22'))
            .fontColor($r('app.color.font_color_182431'))
            .fontWeight(FontWeight.Medium)
            .margin({ left: $r('app.float.distance_8') })
            .textAlign(TextAlign.Start);
        }

        Image('/res/image/ic_settings_arrow.svg')
          .width($r('app.float.wh_value_12'))
          .height($r('app.float.wh_value_24'))
          .margin({ right: $r('app.float.distance_8') })
          .fillColor($r("sys.color.ohos_id_color_primary"))
          .opacity($r("app.float.opacity_0_2"))
      }
      .height(ConfigData.WH_100_100)
      .width(ConfigData.WH_100_100)
      .borderRadius($r('app.float.radius_20'))
      .linearGradient(this.isTouched ? {
                                         angle: 90,
                                         direction: GradientDirection.Right,
                                         colors: [[$r("app.color.DCEAF9"), 0.0], [$r("app.color.FAFAFA"), 1.0]]
                                       } : {
                                             angle: 90,
                                             direction: GradientDirection.Right,
                                             colors: [[$r("sys.color.ohos_id_color_foreground_contrary"), 1],
                                             [$r("sys.color.ohos_id_color_foreground_contrary"), 1]]
                                           })
      .onTouch((event: TouchEvent) => {
        if (event.type === TouchType.Down) {
          this.isTouched = true;
        }
        if (event.type === TouchType.Up) {
          this.isTouched = false;
        }
      })
    }
    .padding($r('app.float.distance_4'))
    .backgroundColor($r('app.color.white_bg_color'))
    .height($r('app.float.wh_value_56'))
    .borderRadius($r('app.float.radius_24'))
    .onClick(() => {
      this.mFaPresenter.requestPermissions()
    });
  }
}
