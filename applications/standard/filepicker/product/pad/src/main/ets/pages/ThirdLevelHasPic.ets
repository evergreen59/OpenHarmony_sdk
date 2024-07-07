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

import { FileInfo } from '../../../../../../common/src/main/ets/components/Data/FileInfo'
import { getListFile } from '../../../../../../common/src/main/ets/components/Utils/FileManagerServiceUtils'
import { logInfo } from '../../../../../../common/src/main/ets/components/Utils/LogUtils'
import { ChooseDialog } from '../../../../../../common/src/main/ets/components/View/ChooseDialog'
import { PadSize } from '../module/Constants'

let TAG: string = 'ThirdLevelHasPic'

@Component
export struct ThirdLevelHasPic {
  private mWorker
  private mColumnTemplate: string
  private mCurrentIndex: number
  @Link mFileList: Array<FileInfo>
  @Link mMenuLevel: number
  @Link mType: string
  @Link mCurrentPath: string

  aboutToAppear() {
    logInfo(TAG, 'aboutToAppear send message to worker')
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
        offset: 0,
        count: 100,
        context: globalThis.context,
      })
    }
  }

  aboutToDisappear() {
    logInfo(TAG, 'aboutToDisappear')
    AppStorage.Set('choseFiles', [])
  }

  build() {
    Column() {
      Grid() {
        ForEach(this.mFileList, (item: FileInfo) => {
          GridItem() {
            GridItemView({ gridItem: item })
          }
        }, item => item.name)
      }
      .columnsTemplate('1fr 1fr 1fr 1fr')

    }
    .width(PadSize.GRID_WIDTH)
    .alignItems(HorizontalAlign.Start)
    .border({ radius: PadSize.TWENTY_FOUR })
    .opacity(0.9)
    .backgroundColor(Color.White)
    .margin({ top: PadSize.SIXTEEN })
  }
}

@Component
struct GridItemView {
  private dialogController: CustomDialogController = new CustomDialogController({
    builder: ChooseDialog(),
    cancel: () => {
    },
    autoCancel: true
  })
  @State gridItem: FileInfo = new FileInfo('')
  @StorageLink('choseFiles') @Watch('choseFilesChange') choseFiles: Array<FileInfo> = []

  private choseFilesChange() {
    this.choseFiles.forEach((item: FileInfo) => {
      logInfo(TAG, 'choseFilesChange item.name =  ' + item.name + ' gridItem.name = ' + this.gridItem.name)
      logInfo(TAG, 'choseFilesChange item.path =  ' + item.path + ' gridItem.path = ' + this.gridItem.path)
      if (item.name == this.gridItem.name && item.path == this.gridItem.path) {
        this.gridItem.fontColor = Color.White
        this.gridItem.listBackground = '#19007DFF'
        this.gridItem.textBackground = "#007DFF"
        return;
      } else {
        this.gridItem.fontColor = Color.Black
        this.gridItem.listBackground = ''
        this.gridItem.textBackground = ""
      }
    })
  }

  build() {
    Flex({
      direction: FlexDirection.Column,
      alignItems: ItemAlign.Center,
      justifyContent: FlexAlign.Center
    }) {
      Image(`${this.gridItem.path}/thumbnail/${PadSize.EIGHTY}/${PadSize.EIGHTY}`)
        .objectFit(ImageFit.Contain)
        .width(PadSize.EIGHTY)
        .height(PadSize.EIGHTY)
        .margin({
          left: PadSize.TWELVE,
          right: PadSize.TWELVE,
          top: PadSize.FOUR,
          bottom: PadSize.TWO
        })
        .border({ radius: PadSize.TWELVE, width: 0, style: BorderStyle.Solid })

      Row() {
        Text(this.gridItem.name)
          .width(PadSize.EIGHTY_EIGHT)
          .fontSize(PadSize.FOURTEEN)
          .fontColor(this.gridItem.fontColor)
          .fontWeight(FontWeight.Regular)
          .maxLines(2)
          .opacity(0.9)
          .textAlign(TextAlign.Center)
          .textOverflow({ overflow: TextOverflow.Ellipsis })
          .margin(PadSize.FOUR)
      }
      .alignItems(VerticalAlign.Top)
      .width(PadSize.NINETY_SIX)
      .height(PadSize.FORTY_SIX)
      .margin({ left: PadSize.FOUR, right: PadSize.FOUR, bottom: PadSize.FOUR })
      .border({ radius: PadSize.TWELVE, width: 0, style: BorderStyle.Solid })
      .backgroundColor(this.gridItem.textBackground)
    }
    .width(PadSize.ONE_HUNDRED_AND_FOUR)
    .height(PadSize.ONE_HUNDRED_AND_THIRTY_SIX)
    .margin({ left: PadSize.SIXTEEN, right: PadSize.EIGHT, bottom: PadSize.TWENTY })
    .border({ radius: PadSize.TWELVE })
    .opacity(this.gridItem.opacity)
    .backgroundColor(this.gridItem.listBackground)
    .gesture(
    LongPressGesture({ repeat: false })
      .onAction((event: GestureEvent) => {
        AppStorage.SetOrCreate<FileInfo>('selectedFileInfo', this.gridItem)
        this.dialogController.open()
      })
    )
    .onClick(() => {
      let choseFiles: Array<FileInfo> = AppStorage.Get('choseFiles')
      let index: number = -1
      choseFiles.forEach((item: FileInfo) => {
        if (item.name == this.gridItem.name && item.path == this.gridItem.path) {
          index = choseFiles.indexOf(item)
        }
      })
      choseFiles.splice(0, choseFiles.length)
      if (index == -1) {
        this.gridItem.fontColor = Color.White
        this.gridItem.listBackground = '#19007DFF'
        this.gridItem.textBackground = "#007DFF"
        choseFiles.push(this.gridItem)
      } else {
        this.gridItem.fontColor = Color.Black
        this.gridItem.listBackground = ''
        this.gridItem.textBackground = ""
      }
    })
  }
}