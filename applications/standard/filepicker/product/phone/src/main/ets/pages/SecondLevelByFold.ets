/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

import { MenuLevel } from '../../../../../../common/src/main/ets/components/Data/Constants'
import { FileInfo } from '../../../../../../common/src/main/ets/components/Data/FileInfo'
import { getListFile } from '../../../../../../common/src/main/ets/components/Utils/FileManagerServiceUtils'
import { logDebug, logInfo } from '../../../../../../common/src/main/ets/components/Utils/LogUtils'
import { changeTypeToString, updateTopPathInfo } from '../../../../../../common/src/main/ets/components/Utils/Utils'

let TAG: string = 'SecondLevelByFold'

@Component
export struct SecondLevelByFold {
  private mWorker
  @Link mFileList: FileInfo[]
  @Link @Watch('fileTypeChange') mType: string
  @Link mMenuLevel: number
  @Link mCurrentPath: string

  aboutToAppear(): void{
    logInfo(TAG, 'aboutToAppear')
    if (globalThis.debugMode) {
      this.mFileList = getListFile('local', this.mType, this.mCurrentPath)
      logDebug(TAG, 'mFileList length = ' + this.mFileList.length)
    } else {
      this.mWorker.postMessage({
        request_data: 'listFile',
        device_name: 'local',
        menu_level: this.mMenuLevel,
        MediaType: this.mType,
        path: this.mCurrentPath,
        context: globalThis.context,
      })
    }

    updateTopPathInfo([], '文件类型', '')
    updateTopPathInfo(AppStorage.Get('topPathInfo'), changeTypeToString(this.mType), this.mCurrentPath)
  }

  fileTypeChange() {
    logInfo(TAG, 'fileTypeChange ' + this.mType)
    if (globalThis.debugMode) {
      this.mFileList = getListFile('local', this.mType, this.mCurrentPath)
    } else {
      this.mWorker.postMessage({
        request_data: 'listFile',
        device_name: 'local',
        menu_level: this.mMenuLevel,
        MediaType: this.mType,
        path: this.mCurrentPath,
        context: globalThis.context,
      })
    }
  }

  build() {
    Column() {
      ForEach(this.mFileList, (item: FileInfo, index: number) => {
        Column() {
          SecondRowView({
            item: item,
            mMenuLevel: $mMenuLevel,
            mCurrentPath: $mCurrentPath,
            isType: $mType
          })

          Divider()
            .color('#000000')
            .opacity(0.05)
            .visibility(this.mFileList.length == index + 1 ? Visibility.Hidden : Visibility.Visible)
            .margin({ left: 40 })
        }
        .width('100%')
        .height(56)
      }, item => item.name.toString())
    }
    .padding({ top: 4, bottom: 4, left: 12, right: 12 })
    .width('100%')
    .alignItems(HorizontalAlign.Start)
    .border({ radius: 24 })
    .opacity(0.9)
    .backgroundColor(Color.White)
  }
}

@Component
struct SecondRowView {
  private item: FileInfo = new FileInfo('')
  @Link mMenuLevel: number
  @Link mCurrentPath: string
  @Link isType: string

  build() {
    Row() {
      Image($r("app.media.ic_type_smallfile"))
        .width(24)
        .height(24)
      Text(this.item.name)
        .fontSize(16)
        .fontColor('#182431')
        .fontWeight(FontWeight.Medium)
        .margin({ left: 17 })
      Blank()
      Image($r("app.media.ic_right"))
        .width(12)
        .height(24)
    }
    .height(56)
    .width('100%') // TODO
    .backgroundColor(this.item.listBackground)
    .onClick(() => {
      logInfo(TAG, 'onClick this.mCurrentPath = ' + this.mCurrentPath)
      this.mCurrentPath = this.item.path
      updateTopPathInfo(AppStorage.Get('topPathInfo'), this.item.name, this.mCurrentPath)

      if (this.item.path.includes('datashare:///media/root') == false) {
        if (this.isType != 'file') {
          logInfo(TAG, 'MENU_THIRD_LEVEL_HAS_PIC - onClick')
          this.mMenuLevel = MenuLevel.MENU_THIRD_LEVEL_HAS_PIC
        } else {
          this.mMenuLevel = MenuLevel.MENU_THIRD_LEVEL_HAS_DETAIL
          logInfo(TAG, 'MENU_THIRD_LEVEL_HAS_DETAIL - onClick')
        }
      }
    })
  }
}