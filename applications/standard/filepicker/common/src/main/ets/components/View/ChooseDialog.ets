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

import { FileInfo } from '../Data/FileInfo'
import { logInfo } from '../Utils/LogUtils'

let TAG: string = 'DialogUtils'

@CustomDialog
export struct ChooseDialog {
  controller: CustomDialogController

  build() {
    Column() {
      Row() {
        Text('选择该文件')
      }

      Row() {
        Button() {
          Text('取消')
            .height('100%')
            .fontSize(24)
            .fontColor(Color.Blue)
        }
        .width(px2vp(200))
        .height(px2vp(50))
        .backgroundColor(Color.White)
        .onClick(() => {
          this.controller.close()
        })

        Divider()
          .vertical(true)
          .color(Color.Gray)
          .height(40)
          .margin({ top: 0, right: 10, bottom: 0, left: 10 })

        Button() {
          Text('确定')
            .height('100%')
            .fontSize(24)
            .fontColor(Color.Blue)
        }
        .width(px2vp(200))
        .height(px2vp(50))
        .backgroundColor(Color.White)
        .onClick(() => {
          let choseItem: FileInfo = AppStorage.Get<FileInfo>('selectedFileInfo')
          if (choseItem != null) {
            let files: Array<FileInfo> = AppStorage.Get('choseFiles')
            let flag: boolean = true
            files.forEach((item: FileInfo) => {
              logInfo(TAG, 'item.name = ' + item.name + '; item.path = ' + item.path)
              logInfo(TAG, 'choseItem.name = ' + choseItem.name + '; choseItem.path = ' + choseItem.path)
              if (item.name == choseItem.name && item.path == choseItem.path) {
                flag = false
              }
            })
            if (flag) {
              files.push(choseItem)
              AppStorage.Set('choseFiles', files)
            }
          }
          this.controller.close()
        })
      }
      .padding(20)
    }
    .padding({
      top: 25,
      right: 0,
      bottom: 15,
      left: 0
    })
  }
}