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
import { MenuLevel } from '../../../../../../common/src/main/ets/components/Data/Constants'
import { FileInfo } from '../../../../../../common/src/main/ets/components/Data/FileInfo'
import { logInfo } from '../../../../../../common/src/main/ets/components/Utils/LogUtils'

@Component
export struct FirstLevelByType {
  private TAG: string = 'FirstLevelByType'
  private mWorker
  private mFileList: FileInfo[]
  @Link mType: string
  @Link mMenuLevel: number
  @Link mCurrentPath: string

  aboutToAppear() {
    logInfo(this.TAG, 'aboutToAppear mType = ' + this.mType)
    logInfo(this.TAG, 'aboutToAppear mCurrentPath = ' + this.mCurrentPath)
  }

  build() {
    Grid() {
      ForEach(this.mFileList, (item: FileInfo) => {
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
            Text(item.name)
              .width(PadSize.EIGHTY_EIGHT)
              .fontSize(PadSize.FOURTEEN)
              .fontWeight(FontWeight.Regular)
              .fontColor('#000000')
              .textAlign(TextAlign.Center)
              .padding(PadSize.FOUR)
              .margin({ left: PadSize.FOUR, right: PadSize.FOUR, bottom: PadSize.FOUR })
          }
          .width(PadSize.ONE_HUNDRED_AND_FOUR)
          .height(PadSize.ONE_HUNDRED_AND_THIRTY_SIX)
          .margin({ left: PadSize.SIXTEEN, right: PadSize.EIGHT, bottom: PadSize.TWENTY })
          .alignItems(HorizontalAlign.Center)
          .onClick(() => {
            this.mType = item.mediaType
            this.mCurrentPath = item.path
            this.mMenuLevel = MenuLevel.MENU_SECOND_LEVEL_BY_FOLD
          })
        }
      }, item => item.name)
    }
    .width(PadSize.GRID_WIDTH)
    .columnsTemplate('1fr 1fr 1fr 1fr')
    .columnsGap(1)
    .rowsGap(1)
    .margin({ top: PadSize.SIXTEEN })
  }
}