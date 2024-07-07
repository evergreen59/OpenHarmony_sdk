/*
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

@CustomDialog
export struct WarningDialog {
  @Link code: number
  controller: CustomDialogController
  cancel: () => void
  confirm: () => void

  build() {
    Column() {
      Text('Warning Title').fontSize(20).margin(10)
      if (this.code == 1) {
        Text($r("app.string.save_file_has_same_file")).fontSize(16).margin(10)
      } else if (this.code == 2) {
        Text($r("app.string.save_file_no_media_permission")).fontSize(16).margin(10)
      } else {
        Text($r("app.string.save_file_unknown_reason")).fontSize(16).margin(10)
      }
      Flex({ justifyContent: FlexAlign.SpaceAround }) {
        Button('cancel')
          .onClick(() => {
            this.controller.close()
            this.cancel()
          })
          .backgroundColor("0xffffff")
          .fontColor(Color.Black)
        Button('confirm')
          .onClick(() => {
            this.controller.close()
            this.confirm()
          })
          .backgroundColor("0xffffff")
          .fontColor(Color.Red)
      }.margin({ bottom: 10 })
    }
  }
}