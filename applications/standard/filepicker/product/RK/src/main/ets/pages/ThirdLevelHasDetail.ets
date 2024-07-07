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

import { MediaType } from '../../../../../../common/src/main/ets/components/Data/Constants'
import { FileInfo } from '../../../../../../common/src/main/ets/components/Data/FileInfo'
import { logInfo } from '../../../../../../common/src/main/ets/components/Utils/LogUtils'
import { getListFile } from '../../../../../../common/src/main/ets/components/Utils/FileManagerServiceUtils'
import { getShowIconBySuffix, updateTopPathInfo } from '../../../../../../common/src/main/ets/components/Utils/Utils'
import { ChooseDialog } from '../../../../../../common/src/main/ets/components/View/ChooseDialog'

let TAG: string = 'ThirdLevelHasDetail'

@Component
export struct ThirdLevelHasDetail {
  private mCurrentMode: string
  private mWorker
  @Link mFileInfo: Array<FileInfo>
  @Link @Watch('refreshFiles') mCurrentPath: string
  @Link mMenuLevel: number
  @Link mType: string

  aboutToAppear(): void{
    logInfo(TAG, 'aboutToAppear')
    if (globalThis.debugMode) {
      this.mFileInfo = getListFile('local', this.mType, this.mCurrentPath)
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

  aboutToDisappear() {
    logInfo(TAG, 'aboutToDisappear')
    AppStorage.Set('choseFiles', [])
  }

  refreshFiles() {
    logInfo(TAG, 'refreshFiles')
    AppStorage.Set('choseFiles', [])
    if (!globalThis.debugMode) {
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
      Flex({
        direction: FlexDirection.Row,
        wrap: FlexWrap.Wrap,
        justifyContent: FlexAlign.Start,
        alignItems: ItemAlign.Start
      }) {
        ForEach(this.mFileInfo, (item: FileInfo, index: number) => {
          Column() {
            DetailRowView({
              listItem: item,
              mMenuLevel: $mMenuLevel,
              mCurrentPath: $mCurrentPath
            })

            Divider()
              .color('#000000')
              .opacity(0.05)
              .visibility(this.mFileInfo.length == index + 1 ? Visibility.Hidden : Visibility.Visible)
              .margin({ left: 64 * 1.3, right: 12 * 1.3 })
          }
        }, (item: FileInfo) => item.name)
      }
    }
    .padding({ bottom: 4 * 1.3 })
    .alignItems(HorizontalAlign.Start)
    .border({ radius: 24 })
    .opacity(0.9)
    .backgroundColor(Color.White)
  }
}

@Component
struct DetailRowView {
  private dialogController: CustomDialogController = new CustomDialogController({
    builder: ChooseDialog(),
    cancel: () => {
    },
    autoCancel: true
  })
  @State listItem: FileInfo = new FileInfo('')
  @Link mCurrentPath: string
  @Link mMenuLevel: number
  @StorageLink('choseFiles') @Watch('choseFilesChange') choseFiles: Array<FileInfo> = []

  private choseFilesChange() {
    this.choseFiles.forEach((item: FileInfo) => {
      logInfo(TAG, 'choseFilesChange item.name =  ' + item.name + ' gridItem.name = ' + this.listItem.name)
      logInfo(TAG, 'choseFilesChange item.path =  ' + item.path + ' gridItem.path = ' + this.listItem.path)
      if (item.name == this.listItem.name && item.path == this.listItem.path) {
        this.listItem.listBackground = '#19007DFF'
      } else {
        this.listItem.listBackground = ''
      }

    })
  }

  build() {
    Row() {
      if (this.listItem.type == MediaType.MEDIA_TYPE_ALBUM) {
        Row() {
          Image($r("app.media.ic_type_bigfile"))
            .width(40 * 1.3)
            .height(40 * 1.3)
            .margin({ left: 10 * 1.3 })
          Text(this.listItem.name)
            .fontSize(16 * 1.3)
            .fontColor('#182431')
            .fontWeight(FontWeight.Medium)
            .margin({ left: 16 * 1.3 })
            .maxLines(1)
            .textOverflow({ overflow: TextOverflow.Ellipsis })
          Blank()
          Image($r("app.media.ic_right"))
            .width(12 * 1.3)
            .height(24 * 1.3)
            .margin({ right: 12 * 1.3 })
        }
        .margin({ right: 12 * 1.3 })
        .height(64 * 1.3)
        .width('100%')
        .backgroundColor(this.listItem.listBackground)
      } else {
        Row() {
          Image(getShowIconBySuffix(this.listItem.name.substring(this.listItem.name.lastIndexOf('.'))))
            .width(40 * 1.3)
            .height(40 * 1.3)
            .margin({ left: 8 * 1.3 })
          Column() {
            Text(this.listItem.name)
              .height(22 * 1.3)
              .fontSize(16 * 1.3)
              .fontColor(this.listItem.fontColor)
              .fontWeight(FontWeight.Medium)
              .maxLines(1)
              .textOverflow({ overflow: TextOverflow.Ellipsis })
              .margin({ top: 10 * 1.3 })
              .height(22 * 1.3)
              .width(248 * 1.3)
            Row() {
              Text('' + this.listItem.modifiedTime)
                .fontSize(14 * 1.3)
                .fontColor(this.listItem.fontColor)
                .fontWeight(FontWeight.Regular)
                .maxLines(1)
                .textOverflow({ overflow: TextOverflow.Ellipsis })
              Text('-')
                .fontSize(14 * 1.3)
                .fontColor(this.listItem.fontColor)
                .margin({ left: 8 * 1.3 })
              Text('' + this.listItem.size)
                .fontSize(14 * 1.3)
                .fontColor(this.listItem.fontColor)
                .fontWeight(FontWeight.Regular)
                .margin({ left: 8 * 1.3 })
                .maxLines(1)
                .textOverflow({ overflow: TextOverflow.Ellipsis })
            }
            .width(248 * 1.3)
            .height(19 * 1.3)
            .margin({ top: 2 * 1.3 })
            .opacity(0.6)
          }
          .height(64 * 1.3)
          .alignItems(HorizontalAlign.Start)
          .margin({ left: 16 * 1.3 })
        }
        .width('100%')
        .height(64 * 1.3) // TODO

      }
    }
    .margin({ top: 4 * 1.3, left: 4 * 1.3, right: 4 * 1.3 })
    .border({ radius: 20 })
    .backgroundColor(this.listItem.listBackground)
    .onClick(() => {
      if (this.listItem.type == MediaType.MEDIA_TYPE_ALBUM) {
        logInfo(TAG, 'fold onClick')
        this.mCurrentPath = this.listItem.path
        updateTopPathInfo(AppStorage.Get('topPathInfo'), this.listItem.name, this.listItem.path)
      } else {
        var index = this.choseFiles.indexOf(this.listItem)
        this.choseFiles.splice(0, this.choseFiles.length)
        if (index == -1) {
          this.listItem.fontColor = Color.Black
          this.listItem.opacity = 0.1
          this.listItem.listBackground = 'rgba(46,136,255,0.1)'
          this.choseFiles.push(this.listItem)
        } else {
          this.listItem.fontColor = Color.Black
          this.listItem.opacity = 1
          this.listItem.listBackground = ''
        }
      }
    })
    .gesture(
    LongPressGesture({ repeat: false })
      .onAction((event: GestureEvent) => {
        if (this.listItem.type != MediaType.MEDIA_TYPE_ALBUM) {
          AppStorage.SetOrCreate<FileInfo>('selectedFileInfo', this.listItem)
          this.dialogController.open()
        }
      })
    )
  }
}