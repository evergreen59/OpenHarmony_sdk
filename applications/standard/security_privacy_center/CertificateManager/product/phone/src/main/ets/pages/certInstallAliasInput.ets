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

import HeadComponent from '../../../../../../common/component/headComponent';
import LogUtil from '../../../../../../common/util/LogUtil';
import ConfigData from '../../../../../../common/util/ConfigData';
import router from '@system.router';
import CMInstallPresenter from '../presenter/CMInstallPresenter';
import CMFaPresenter from '../presenter/CMFaPresenter';

@Entry
@Component
struct CertInstallAliasInput {
  @State mAppCredAuthPresenter: CMInstallPresenter = CMInstallPresenter.getInstance();
  @State mFaPresenter: CMFaPresenter = CMFaPresenter.getInstance();
  @State certAlias: string = "";

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
      Column() {
        HeadComponent({ headName: $r('app.string.certificateAliasTab') });
        Text($r('app.string.certificateAliasInfo'))
          .fontColor($r("sys.color.ohos_id_color_text_primary"))
          .fontSize($r("sys.float.ohos_id_text_size_body1"))
          .fontWeight(FontWeight.Medium)
          .margin({
            top: $r('app.float.distance_24'),
            left: $r('app.float.distance_24')
          })
          .alignSelf(ItemAlign.Start)

        TextInput()
          .type(InputType.Normal)
          .backgroundColor($r("sys.color.ohos_id_color_sub_background"))
          .enterKeyType(EnterKeyType.Done)
          .fontSize($r("sys.float.ohos_id_text_size_body1"))
          .fontWeight(FontWeight.Medium)
          .fontColor($r("sys.color.ohos_id_color_text_primary"))
          .onChange((value: string) => {
            this.certAlias = value;
          })
          .maxLength(ConfigData.ALIAS_MAX_LENGTH)
          .margin({
            left: $r('app.float.distance_24'),
            right: $r('app.float.distance_24')
          })
          .height($r('app.float.distance_48'))

        Row() {
          Divider().color($r('app.color.CertManager_Divider_Color_182431'))
        }
        .margin({
          left: $r('app.float.distance_24'),
          right: $r('app.float.distance_24'),
        })

        Text($r('app.string.installAliasInputInfo'))
          .fontColor($r("sys.color.ohos_id_color_text_primary"))
          .fontWeight(FontWeight.Regular)
          .fontSize($r("sys.float.ohos_id_text_size_body2"))
          .margin({
            left: $r('app.float.distance_24'),
            top: $r('app.float.distance_16')
          })
          .alignSelf(ItemAlign.Start)
          .opacity($r('app.float.opacity_100_60'))
      }
      .width(ConfigData.WH_100_100)
      .height(ConfigData.WH_75_100);

      Column() {
        Row() {
          Button() {
            Text($r('app.string.installAliasInputCancel'))
              .fontSize($r('sys.float.ohos_id_text_size_body1'))
              .fontWeight(FontWeight.Medium)
              .fontColor($r('sys.color.ohos_id_color_text_hyperlink'))
          }
          .type(ButtonType.Capsule)
          .backgroundColor($r('app.color.install_cancel_bt_bg_color'))
          .width($r('app.float.component_button_width_phone'))
          .height($r('app.float.application_button_height'))
          .margin({
            left: $r('app.float.distance_24'),
          })
          .onClick(() => {
            router.back();
          })

          if ((this.certAlias.length > 0) && (this.certAlias[0] != ' ')) {
            Button() {
              Text($r('app.string.installAliasInputConfirm'))
                .fontSize($r('sys.float.ohos_id_text_size_body1'))
                .fontWeight(FontWeight.Medium)
                .fontColor($r('app.color.install_confirm_bt_font_color'))
            }
            .type(ButtonType.Capsule)
            .backgroundColor($r('app.color.install_confirm_bt_bg_color'))
            .width($r('app.float.component_button_width_phone'))
            .height($r('app.float.application_button_height'))
            .margin({
              left: $r('app.float.distance_16'),
              right: $r('app.float.distance_24'),
            })
            .onClick(() => {
              this.mAppCredAuthPresenter.installCert(router.getParams().uri,
                this.certAlias,
                router.getParams().suffix);
            })
          } else {
            Button() {
              Text($r('app.string.installAliasInputConfirm'))
                .fontSize($r('sys.float.ohos_id_text_size_body1'))
                .fontWeight(FontWeight.Medium)
                .fontColor($r('app.color.install_confirm_bt_font_color'))
            }
            .type(ButtonType.Capsule)
            .backgroundColor($r('app.color.install_confirm_bt_bg_disable_color'))
            .width($r('app.float.component_button_width_phone'))
            .height($r('app.float.application_button_height'))
            .margin({
              left: $r('app.float.distance_16'),
              right: $r('app.float.distance_24'),
            })
          }
        }
        .margin({ bottom: $r('app.float.wh_value_24') })
      }
      .justifyContent(FlexAlign.End)
      .height(ConfigData.WH_25_100);
    }
    .backgroundColor($r("sys.color.ohos_id_color_sub_background"))
    .width(ConfigData.WH_100_100)
    .height(ConfigData.WH_100_100);
  }
}
