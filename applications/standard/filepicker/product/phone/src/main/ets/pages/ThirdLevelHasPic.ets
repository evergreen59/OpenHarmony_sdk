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
import { logDebug, logInfo } from '../../../../../../common/src/main/ets/components/Utils/LogUtils'
import { ChooseDialog } from '../../../../../../common/src/main/ets/components/View/ChooseDialog'

let TAG: string = 'ThirdLevelHasPic'

@Component
export struct ThirdLevelHasPic {
  private mColumnTemplate: string
  private mCurrentMode: string
  private mCurrentIndex: number
  private mWorker
  @Link mFileList: Array<FileInfo>
  @Link mMenuLevel: number
  @Link mType: string
  @Link mCurrentPath: string
  @StorageLink('choseFiles') tmpFiles: Array<FileInfo> = []

  aboutToAppear() {
    logInfo(TAG, 'aboutToAppear send message to worker')
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
      Flex({
        direction: FlexDirection.Row,
        wrap: FlexWrap.Wrap,
        justifyContent: FlexAlign.Start,
        alignItems: ItemAlign.Start
      }) {
        ForEach(this.mFileList, (item: FileInfo) => {
          GridItemView({ gridItem: item })
        }, item => item.name)
      }
      .margin({ top: 8 })
    }
    .alignItems(HorizontalAlign.Start)
    .border({ radius: 24 })
    .opacity(0.9)
    .backgroundColor(Color.White)
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
        this.gridItem.opacity = 0.1
        this.gridItem.fontColor = '#FFFFFF'
        this.gridItem.listBackground = 'rgba(46,136,255,0.1)'
      } else {
        this.gridItem.opacity = 1
        this.gridItem.fontColor = '#000000'
        this.gridItem.listBackground = ''
      }
    })
  }

  build() {
    Flex({
      direction: FlexDirection.Column,
      alignItems: ItemAlign.Center,
      justifyContent: FlexAlign.Center
    }) {
      Image(`${this.gridItem.path}/thumbnail/160/160`)
        .borderRadius(12)
        .objectFit(ImageFit.Contain)
        .width(80)
        .height(80)
        .margin({
          left: 8,
          right: 8,
          top: 4
        })
      Row() {
        Text(this.gridItem.name)
          .width(82)
          .fontSize(12)
          .fontColor(this.gridItem.listBackground == 'rgba(46,136,255,0.1)' ? Color.White : Color.Black)
          .fontWeight(FontWeight.Regular)
          .opacity(0.9)
          .textAlign(TextAlign.Center)
          .maxLines(2)
          .textOverflow({ overflow: TextOverflow.Ellipsis })
          .margin({ top: 2 })
          .padding({ right: 4, left: 4 })
      }
      .borderRadius(12)
      .backgroundColor(this.gridItem.listBackground == 'rgba(46,136,255,0.1)' ? 0x2E8BFF : '')
      .alignItems(VerticalAlign.Top)
      .width(82)
      .height(36)
      .margin({ top: 4 })
    }
    .padding({ bottom: 8 })
    .width(96)
    .height(126)
    .margin({ bottom: 12, left: 12 })
    .border({ radius: 20 })
    .backgroundColor(this.gridItem.listBackground)
    .gesture(
    LongPressGesture({ repeat: false })
      .onAction((event: GestureEvent) => {
        AppStorage.SetOrCreate('selectedFileInfo', this.gridItem)
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
        this.gridItem.opacity = 0.1
        this.gridItem.fontColor = '#FFFFFF'
        this.gridItem.listBackground = 'rgba(46,136,255,0.1)'
        choseFiles.push(this.gridItem)
      } else {
        this.gridItem.opacity = 1
        this.gridItem.fontColor = '#000000'
        this.gridItem.listBackground = ''
      }
    })

  }
}