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
import { PadSize } from '../module/Constants'

@Component
export struct ThirdLevelHasDetail {
  private TAG: string = 'ThirdLevelHasDetail'
  private mWorker
  @Link mFileInfo: Array<FileInfo>
  @Link @Watch('refreshFiles') mCurrentPath: string
  @Link mMenuLevel: number
  @Link mType: string

  aboutToAppear(): void{
    logInfo(this.TAG, 'aboutToAppear')
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
    logInfo(this.TAG, 'aboutToDisappear')
    AppStorage.Set('choseFiles', [])
  }

  refreshFiles() {
    logInfo(this.TAG, 'refreshFiles')
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
      List() {
        ForEach(this.mFileInfo, (item: FileInfo) => {
          ListItem() {
            DetailRowView({
              listItem: item,
              mMenuLevel: $mMenuLevel,
              mCurrentPath: $mCurrentPath
            })
          }
        }, (item: FileInfo) => item.name)
      }.align(Alignment.Top)
    }
    .height(PadSize.TWO_HUNDRED_AND_SIXTY)
    .alignItems(HorizontalAlign.Start)
    .opacity(0.9)
    .backgroundColor(Color.White)
    .border({ radius: PadSize.TWENTY_FOUR })
    .margin({ top: PadSize.SIXTEEN })
  }
}

@Component
struct DetailRowView {
  private TAG: string = 'DetailRowView'
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
      logInfo(this.TAG, 'choseFilesChange item.name =  ' + item.name + ' gridItem.name = ' + this.listItem.name)
      logInfo(this.TAG, 'choseFilesChange item.path =  ' + item.path + ' gridItem.path = ' + this.listItem.path)
      if (item.name == this.listItem.name && item.path == this.listItem.path) {
        this.listItem.listBackground = '#19007DFF'
      } else {
        this.listItem.listBackground = ''
      }

    })
  }

  build() {
    Stack() {
      Divider()
        .vertical(false)
        .color("#0C000000")
        .margin({ left: PadSize.EIGHTY, top: PadSize.SIXTY_FOUR, right: PadSize.TWENTY_FOUR })
      Row() {
        if (this.listItem.type == MediaType.MEDIA_TYPE_ALBUM) {
          Image($r("app.media.ic_file_fold"))
            .width(PadSize.FORTY)
            .height(PadSize.FORTY)
            .margin({ left: PadSize.EIGHT })
          Text(this.listItem.name)
            .fontSize(PadSize.SIXTEEN)
            .fontColor('#182431')
            .fontWeight(FontWeight.Medium)
            .margin({ left: PadSize.SIXTEEN })
            .maxLines(PadSize.ONE)
            .textOverflow({ overflow: TextOverflow.Ellipsis })
          Blank()
          Image($r("app.media.ic_right"))
            .width(PadSize.TWELVE)
            .height(PadSize.TWENTY_FOUR)
        } else {
          Image(getShowIconBySuffix(this.listItem.name.substring(this.listItem.name.lastIndexOf('.'))))
            .width(PadSize.FORTY)
            .height(PadSize.FORTY)
            .margin({ left: PadSize.EIGHT })
          Column() {
            Text(this.listItem.name)
              .height(PadSize.TWENTY_TWO)
              .fontSize(PadSize.SIXTEEN)
              .fontWeight(FontWeight.Medium)
              .fontColor(this.listItem.fontColor)
              .maxLines(1)
              .textOverflow({ overflow: TextOverflow.Ellipsis })
              .margin({ top: PadSize.TEN })
            Row() {
              Text('' + this.listItem.modifiedTime)
                .fontSize(PadSize.FOURTEEN)
                .fontWeight(FontWeight.Regular)
                .fontColor(this.listItem.fontColor)
                .maxLines(PadSize.ONE)
                .textOverflow({ overflow: TextOverflow.Ellipsis })
              Text('-')
                .fontSize(PadSize.FOURTEEN)
                .fontColor(this.listItem.fontColor)
                .margin({ left: PadSize.EIGHT })
              Text('' + this.listItem.size)
                .fontSize(PadSize.FOURTEEN)
                .fontWeight(FontWeight.Regular)
                .fontColor(this.listItem.fontColor)
                .margin({ left: PadSize.EIGHT })
                .maxLines(PadSize.ONE)
                .textOverflow({ overflow: TextOverflow.Ellipsis })
            }
            .opacity(0.6)
            .height(PadSize.NINETEEN)
            .margin({ top: PadSize.TWO, bottom: PadSize.ELEVEN })
          }
          .alignItems(HorizontalAlign.Start)
          .margin({ left: PadSize.SIXTEEN })
        }
      }
      .width(PadSize.FIVE_HUNDRED_AND_FOUR) // TODO
      .height(PadSize.SIXTY_FOUR)
      .margin({ left: PadSize.SIXTEEN, right: PadSize.TWENTY_FOUR, bottom: PadSize.ONE })
      .border({ radius: PadSize.TWENTY })
      .backgroundColor(this.listItem.listBackground)
      .onClick(() => {
        if (this.listItem.type == MediaType.MEDIA_TYPE_ALBUM) {
          this.mCurrentPath = this.listItem.path
          updateTopPathInfo(AppStorage.Get('topPathInfo'), this.listItem.name, this.listItem.path)
        } else {
          var index = this.choseFiles.indexOf(this.listItem)
          this.choseFiles.splice(0, this.choseFiles.length)
          if (index == -1) {
            this.listItem.listBackground = '#19007DFF'
            this.choseFiles.push(this.listItem)
          } else {
            this.listItem.listBackground = ''
          }
        }
      })
      .gesture(LongPressGesture({ repeat: false })
        .onAction((event: GestureEvent) => {
          if (this.listItem.type != MediaType.MEDIA_TYPE_ALBUM) {
            AppStorage.SetOrCreate<FileInfo>('selectedFileInfo', this.listItem)
            this.dialogController.open()
          }
        })
      )
    }
  }
}