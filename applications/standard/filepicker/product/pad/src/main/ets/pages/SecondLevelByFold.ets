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

import { PadSize } from '../module/Constants'
import { MediaName, MenuLevel } from '../../../../../../common/src/main/ets/components/Data/Constants'
import { FileInfo } from '../../../../../../common/src/main/ets/components/Data/FileInfo'
import { getListFile } from '../../../../../../common/src/main/ets/components/Utils/FileManagerServiceUtils'
import { logInfo } from '../../../../../../common/src/main/ets/components/Utils/LogUtils'
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
      logInfo(TAG, 'mFileList length = ' + this.mFileList.length)
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
    Grid() {
      ForEach(this.mFileList, fileItem => {
        GridItem() {
          Column() {
            Image($r("app.media.ic_public_big_folder"))
              .width(PadSize.EIGHTY)
              .height(PadSize.EIGHTY)
              .margin({
                left: PadSize.TWELVE,
                right: PadSize.TWELVE,
                top: PadSize.FOUR,
                bottom: PadSize.TWO
              })
            Row() {
              Text(fileItem.name)
                .width(PadSize.EIGHTY_EIGHT)
                .fontSize(PadSize.FOURTEEN)
                .fontWeight(FontWeight.Regular)
                .fontColor('#000000')
                .textAlign(TextAlign.Center)
                .maxLines(PadSize.TWO)
                .margin(PadSize.FOUR)
            }
            .alignItems(VerticalAlign.Top)
            .width(PadSize.NINETY_SIX)
            .height(PadSize.FORTY_SIX)
            .margin({ left: PadSize.FOUR, right: PadSize.FOUR, bottom: PadSize.FOUR })
          }
          .width(PadSize.ONE_HUNDRED_AND_FOUR)
          .height(PadSize.ONE_HUNDRED_AND_THIRTY_SIX)
          .margin({ left: PadSize.SIXTEEN, right: PadSize.EIGHT, bottom: PadSize.TWENTY })
          .alignItems(HorizontalAlign.Center)
        }
        .onClick(() => {
          this.mCurrentPath = fileItem.path
          logInfo(TAG, 'fileItem.mediaType = ' + fileItem.mediaType)
          if (fileItem.mediaType == MediaName.FILE) {
            this.mMenuLevel = MenuLevel.MENU_THIRD_LEVEL_HAS_DETAIL
          } else {
            this.mMenuLevel = MenuLevel.MENU_THIRD_LEVEL_HAS_PIC
          }
          updateTopPathInfo(AppStorage.Get('topPathInfo'), fileItem.name, fileItem.mCurrentPath)
        })
      }, fileItem => fileItem.name)
    }
    .width(PadSize.GRID_WIDTH)
    .columnsTemplate('1fr 1fr 1fr 1fr')
    .columnsGap(1)
    .rowsGap(1)
    .margin({ top: PadSize.SIXTEEN })
  }
}