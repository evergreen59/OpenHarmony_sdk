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
import CMAppCredAuthPresenter from '../presenter/CMAppCredAuthPresenter';
import router from '@system.router';
import ConfigData from '../../../../../../common/util/ConfigData';

@CustomDialog
@Component
struct CustomDialogAuthForApp {
  controller: CustomDialogController
  @State mAppCredAuthPresenter: CMAppCredAuthPresenter = CMAppCredAuthPresenter.getInstance();
  selectUri: string = "";

  aboutToAppear(): void {
    LogUtil.info('certPwdInput about to appear');
    this.mAppCredAuthPresenter.UpdateAppNameFromUid(router.getParams().appUid);
    this.mAppCredAuthPresenter.updateAppCredList();
  }

  build() {
    Column() {
      Text($r('app.string.pickCredToAuth'))
        .fontSize($r('sys.float.ohos_id_text_size_dialog_tittle'))
        .height($r("app.float.wh_value_56"))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Medium)
        .margin({
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
        .alignSelf(ItemAlign.Start)

      Text($r('app.string.requestAuthMsg', this.mAppCredAuthPresenter.appName))
        .fontSize($r('sys.float.ohos_id_text_size_body1'))
        .fontColor($r("sys.color.ohos_id_color_text_primary"))
        .fontWeight(FontWeight.Regular)
        .margin({
          top: $r('app.float.wh_value_8'),
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
        .alignSelf(ItemAlign.Start)

      if (this.mAppCredAuthPresenter.credList.length > ConfigData.REQUEST_AUTH_MAX_LENGTH) {
        List() {
          ForEach(this.mAppCredAuthPresenter.credList, (item) => {
            ListItem() {
              Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {

                Column() {
                  Text(item.alias)
                    .fontSize($r('app.float.font_18'))
                    .fontColor($r('app.color.request_auth_font_color'))
                    .fontWeight(FontWeight.Medium)
                    .textAlign(TextAlign.Start);
                }
                .alignItems(HorizontalAlign.Start)

                Column() {
                  Radio({ value: item.alias, group: 'appCredGroup' })
                    .checked(false)
                    .onChange((value: boolean) => {
                      console.log(item.alias + ' status is ' + value)
                      if (value) {
                        this.selectUri = item.keyUri;
                        console.log('selectUri is ' + this.selectUri)
                      }
                    })
                }
                .alignItems(HorizontalAlign.End)
              }
              .width(ConfigData.WH_100_100)
            }
            .height($r('app.float.wh_value_64'))
          }, item => JSON.stringify(item))
        }
        .height(ConfigData.WH_50_100)
        .margin({
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
        .divider({
          strokeWidth: $r("app.float.request_auth_strokeWidth"),
          color: $r("app.color.request_auth_color"),
          endMargin: $r("app.float.request_auth_divider_endMargin")})
        .borderRadius($r("app.float.request_auth_borderRadius"))
        .backgroundColor($r("app.color.request_auth_background_color"))
        .visibility(this.mAppCredAuthPresenter.credList.length > 0 ? Visibility.Visible : Visibility.None)
      } else {
        List() {
          ForEach(this.mAppCredAuthPresenter.credList, (item) => {
            ListItem() {
              Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
                Column() {
                  Text(item.alias)
                    .fontSize($r('app.float.font_18'))
                    .fontColor($r('app.color.request_auth_font_color'))
                    .fontWeight(FontWeight.Medium)
                    .textAlign(TextAlign.Start);
                }
                .alignItems(HorizontalAlign.Start)

                Column() {
                  Radio({ value: item.alias, group: 'appCredGroup' })
                    .checked(false)
                    .onChange((value: boolean) => {
                      console.log(item.alias + ' status is ' + value)
                      if (value) {
                        this.selectUri = item.keyUri;
                        console.log('selectUri is ' + this.selectUri)
                      }
                    })
                }
                .alignItems(HorizontalAlign.End)
              }
              .width(ConfigData.WH_100_100)
            }
            .height($r('app.float.wh_value_64'))
          }, item => JSON.stringify(item))
          ListItem() {
            Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
              Column() {
                Text()
              }
              .alignItems(HorizontalAlign.Start)

              Column() {
                Radio({ value: 'null', group: 'appCredGroup' })
                  .checked(false)
                  .onChange((value: boolean) => {
                    if (value) {
                      this.selectUri = 'null';
                      console.log('selectUri is ' + this.selectUri)
                    }
                  })
              }
            }
            .visibility(Visibility.None)
          }
        }
        .margin({
          left: $r('app.float.wh_value_24'),
          right: $r('app.float.wh_value_24')
        })
        .divider({
          strokeWidth: $r("app.float.request_auth_strokeWidth"),
          color: $r("app.color.request_auth_color"),
          endMargin: $r("app.float.request_auth_divider_endMargin") })
        .borderRadius($r("app.float.request_auth_borderRadius"))
        .backgroundColor($r("app.color.request_auth_background_color"))
        .visibility(this.mAppCredAuthPresenter.credList.length > 0 ? Visibility.Visible : Visibility.None)
      }

      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceAround }) {
        Button($r('app.string.requestAuthCancel'))
          .onClick(() => {
            this.mAppCredAuthPresenter.cancelProcess();
            this.controller.close();
          })
          .backgroundColor($r('app.color.request_auth_background_color'))
          .fontColor($r('sys.color.ohos_id_color_text_hyperlink'))
          .fontSize($r('sys.float.ohos_id_text_size_button1'))
          .width($r('app.float.wh_value_152'))
          .height($r('app.float.wh_value_40'))

        Divider()
          .strokeWidth(ConfigData.VERTICAL_DIVIDER_WIDTH)
          .color($r("sys.color.ohos_id_color_list_separator"))
          .vertical(true)
          .height($r('app.float.wh_value_40'))
          .opacity($r("app.float.opacity_0_2"))

        Button($r('app.string.requestAuthFinish'))
          .onClick(() => {
            LogUtil.info('requestAuthorize uri: ' + this.selectUri);
            if (router.getParams() && router.getParams().appUid) {
              LogUtil.info('requestAuthorize appUid: ' + router.getParams().appUid);
              this.mAppCredAuthPresenter.requestAuthorize(this.selectUri, router.getParams().appUid);
              this.controller.close();
            } else {
              LogUtil.info('requestAuthorize fail');
            }
          })
          .backgroundColor($r('app.color.request_auth_background_color'))
          .fontColor($r('sys.color.ohos_id_color_text_hyperlink'))
          .fontSize($r('sys.float.ohos_id_text_size_button1'))
          .width($r('app.float.wh_value_152'))
          .height($r('app.float.wh_value_40'))
      }
      .height($r('app.float.wh_value_64'))
      .padding({
        bottom: $r('app.float.wh_value_16')
      })
      .margin({
        left: $r('app.float.wh_value_24'),
        right: $r('app.float.wh_value_24')
      })
    }
  }
}

@Entry
@Component
struct RadioAuth {
  dialogController: CustomDialogController = new CustomDialogController({
    builder: CustomDialogAuthForApp(),
    autoCancel: false,
    alignment: DialogAlignment.Bottom,
    offset: { dx: 0, dy: ConfigData.DIALOG_DY_OFFSET }
  })

  aboutToAppear() {
    LogUtil.info('RadioAuth aboutToAppear');

    if (router.getParams() && router.getParams().appUid) {
      this.dialogController.open();
    } else {
      router.replace({ uri: "pages/certManagerFa" });
    }
  }

  build() {
  }
}
