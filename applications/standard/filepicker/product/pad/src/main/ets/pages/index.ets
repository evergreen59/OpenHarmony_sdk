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

import worker from '@ohos.worker'
import inputConsumer from '@ohos.multimodalInput.inputConsumer'
import { FirstLevelByType } from './FirstLevelByType'
import { SecondLevelByFold } from './SecondLevelByFold'
import { ThirdLevelHasDetail } from '../pages/ThirdLevelHasDetail'
import { ThirdLevelHasPic } from '../pages/ThirdLevelHasPic'
import { PadSize } from '../module/Constants'
import {
  MenuLevel,
  TerminateReason,
  MediaType,
  MediaName
} from '../../../../../../common/src/main/ets/components/Data/Constants'
import { FileInfo } from '../../../../../../common/src/main/ets/components/Data/FileInfo'
import { TopPathInfo } from '../../../../../../common/src/main/ets/components/Data/TopPathInfo'
import { terminateSelfWithResult } from '../../../../../../common/src/main/ets/components/Utils/AbilityUtils'
import { getFirstMenu } from '../../../../../../common/src/main/ets/components/Utils/FileManagerServiceUtils'
import { logInfo, logError } from '../../../../../../common/src/main/ets/components/Utils/LogUtils'
import {
  changeStringToType,
  changeTypeToString,
  updateTopPathInfo
} from '../../../../../../common/src/main/ets/components/Utils/Utils'
import { WarningDialog } from '../../../../../../common/src/main/ets/components/View/WarningDialog'


@Entry
@Component
struct Index {
  private TAG: string = 'Main'
  private mWorker
  private customDialogController: CustomDialogController = new CustomDialogController({
    builder: WarningDialog({ code: $code })
  })
  @State mFirstMenuData: FileInfo[] = []
  @State mSecondMenuData: FileInfo[] = []
  @State mThirdMenuPicData: FileInfo[] = []
  @State mThirdMenuDetailData: FileInfo[] = []
  @State mType: string = ''
  @State mMenuLevel: number = MenuLevel.MENU_FIRST_LEVEL_BY_TYPE
  @State mCurrentPath: string = ''
  @State code: number = 2;
  @StorageLink('topPathInfo') topPathInfo: Array<TopPathInfo> = updateTopPathInfo([], '文件类型', '')

  aboutToAppear() {
    logInfo(this.TAG, 'aboutToAppear')
    if (globalThis.debugMode) {
      this.mFirstMenuData = getFirstMenu('')
      logInfo(this.TAG, 'mFirstMenuData length = ' + this.mFirstMenuData.length)
    } else {
      this.initWorker()
      logInfo(this.TAG, 'send message to worker')
      this.mWorker.postMessage({
        request_data: 'getRoot',
        device_name: 'local',
        menu_level: this.mMenuLevel,
        MediaType: this.mType,
        path: this.mCurrentPath,
        context: globalThis.context,
      })
    }
  }

  private initWorker() {
    if (this.mWorker == undefined) {
      logInfo(this.TAG, 'initWorker')
      this.mWorker = new worker.Worker('pad/ets/workers/DataWorker.js', { type: 'classic', name: 'filePicker' })
      this.mWorker.onerror = this.onError.bind(this)
      this.mWorker.onmessageerror = this.onMessageError.bind(this)
      this.mWorker.onexit = this.onExit.bind(this)
      this.mWorker.onmessage = this.onMessage.bind(this)
    }
  }

  private onError(e): void {
    logError(this.TAG, 'onError = ' + JSON.stringify(e))
  }

  private onMessageError(e): void {
    logError(this.TAG, 'onMessageError = ' + JSON.stringify(e))
  }

  private onExit(): void{
    logInfo(this.TAG, 'onExit worker')
    this.mWorker = undefined
  }

  private onMessage(result) {
    logInfo(this.TAG, 'main thread onMessage');
    var params = result.data.params;
    if (params.save_name && params.retCode == -17) {
      logInfo(this.TAG, 'main thread onMessage pop-up window');
      AlertDialog.show(
        {
          title: '文件已存在',
          message: '是否覆盖原文件?',
          primaryButton: {
            value: '取消',
            action: () => {
            }
          },
          secondaryButton: {
            value: '确定',
            action: () => {
              logInfo(this.TAG, 'main thread onMessage confirm and conver file');
              this.mWorker.postMessage({
                request_data: 'coverFile',
                device_name: 'local',
                MediaType: this.mType,
                path: this.mCurrentPath,
                cover_name: params.save_name,
                context: globalThis.context,
              })
            }
          },
          cancel: () => {
          }
        }
      )
      return
    }
    var file = JSON.parse(String.fromCharCode.apply(null, new Uint16Array(result.data.data)));
    if (params.request_data == 'createFile' || params.request_data == 'coverFile') {
      if (file.code == undefined) {
        terminateSelfWithResult(TerminateReason.SUCCESS, file)
      } else {
        if (params.path == 'datashare:///media/root') {
          this.code = 2;
          this.customDialogController.open()
        } else {
          this.code = file.code
          this.customDialogController.open()
        }
      }
      return
    }
    if (Array.isArray(file)) {
      let listData: Array<FileInfo> = []
      for (var index = 0; index < file.length; index++) {
        var info = file[index]
        let item = new FileInfo(info.name)
        item.path = info.path
        item.type = info.type
        item.size = info.size
        item.addedTime = info.added_time
        item.modifiedTime = info.modified_time
        if (params.menu_level == MenuLevel.MENU_FIRST_LEVEL_BY_TYPE) {
          if (item.name == 'MEDIA_TYPE_IMAGE') {
            item.name = "图片"
            item.mediaType = MediaName.IMAGE
            item.thumbnail = $r("app.media.ic_type_image")
          } else if (item.name == 'MEDIA_TYPE_VIDEO') {
            item.name = "视频"
            item.mediaType = MediaName.VIDEO
            item.thumbnail = $r("app.media.ic_type_video")
          } else if (item.name == 'MEDIA_TYPE_AUDIO') {
            item.name = "音乐"
            item.mediaType = MediaName.AUDIO
            item.thumbnail = $r("app.media.ic_type_audio")
          } else if (item.name == 'MEDIA_TYPE_FILE') {
            item.name = "文档"
            item.mediaType = MediaName.FILE
            item.thumbnail = $r("app.media.ic_public_file")
          }
        }

        if (params.menu_level == MenuLevel.MENU_THIRD_LEVEL_HAS_DETAIL) {
          if (globalThis.startMode == 'save' && item.type != MediaType.MEDIA_TYPE_ALBUM) {
            break
          }
        }
        listData.push(item)
      }
      if (params.menu_level == MenuLevel.MENU_FIRST_LEVEL_BY_TYPE && params.request_data == 'getRoot') {
        logInfo(this.TAG, 'mFirstMenuData length = ' + listData.length)
        this.mFirstMenuData = listData
      } else if (params.menu_level == MenuLevel.MENU_SECOND_LEVEL_BY_FOLD) {
        logInfo(this.TAG, 'mSecondMenuData length = ' + listData.length + "==" + this.mMenuLevel)
        this.mSecondMenuData = listData
      } else if (params.menu_level == MenuLevel.MENU_THIRD_LEVEL_HAS_PIC) {
        logInfo(this.TAG, 'mThirdMenuPicData length = ' + listData.length)
        this.mThirdMenuPicData = listData
      } else if (params.menu_level == MenuLevel.MENU_THIRD_LEVEL_HAS_DETAIL) {
        logInfo(this.TAG, 'mThirdMenuDetailData length = ' + listData.length)
        if (listData.length > 0) {
          this.mThirdMenuDetailData = listData
        } else {
          this.mThirdMenuDetailData = []
        }
      }
    }
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Start, justifyContent: FlexAlign.Start }) {
      Row() {
        LeftDialogComponent({
          mType: $mType,
          mMenuLevel: $mMenuLevel,
          mCurrentPath: $mCurrentPath,
          mFirstMenuData: $mFirstMenuData
        })

        Column() {
          Row() {
            Row() {
              Image($r("app.media.ic_public_back"))
                .width(PadSize.TWENTY_FOUR)
                .height(PadSize.TWENTY_FOUR)
                .margin({ left: PadSize.TWENTY_FOUR })
                .onClick(() => {
                  if (this.mMenuLevel == MenuLevel.MENU_SECOND_LEVEL_BY_FOLD) {
                    updateTopPathInfo([], '文件类型', '')
                    this.mMenuLevel = MenuLevel.MENU_FIRST_LEVEL_BY_TYPE
                    this.mType = ''
                  } else if (this.mMenuLevel == MenuLevel.MENU_THIRD_LEVEL_HAS_PIC || (this.mMenuLevel == MenuLevel.MENU_THIRD_LEVEL_HAS_DETAIL)) {
                    this.topPathInfo.pop()
                    this.topPathInfo[this.topPathInfo.length - 1].opacity = 1
                    if (this.topPathInfo.length > 1) {
                      var index = this.topPathInfo.length - 1
                      this.mCurrentPath = this.topPathInfo[index].path
                      this.mMenuLevel = MenuLevel.MENU_SECOND_LEVEL_BY_FOLD
                    } else {
                      this.mMenuLevel = MenuLevel.MENU_FIRST_LEVEL_BY_TYPE
                      this.mType = ''
                    }
                  }
                })

              Image($r("app.media.ic_public_next"))
                .width(PadSize.TWENTY_FOUR)
                .height(PadSize.TWENTY_FOUR)
                .margin({ left: PadSize.TWENTY_FOUR, right: PadSize.TWENTY_FOUR })
            }

            Row() {
              List() {
                ForEach(this.topPathInfo, (item: TopPathInfo) => {
                  ListItem() {
                    Row() {
                      if (item.name != '文件类型') {
                        Image($r("app.media.ic_public_right"))
                          .width(PadSize.TWELVE)
                          .height(PadSize.TWENTY_FOUR)
                          .margin({ left: PadSize.EIGHT, right: PadSize.EIGHT })
                      }
                      Text(item.name)
                        .height(PadSize.TWENTY_FOUR)
                        .fontSize(PadSize.SIXTEEN)
                        .fontWeight(FontWeight.Medium)
                        .fontColor('#182431')
                        .opacity(item.opacity)
                        .onClick(() => {
                          if (item.name == '文件类型') {
                            updateTopPathInfo([], '文件类型', '')
                            this.mMenuLevel = MenuLevel.MENU_FIRST_LEVEL_BY_TYPE
                            this.mType = ''
                          } else if (this.mMenuLevel == MenuLevel.MENU_THIRD_LEVEL_HAS_DETAIL) {
                            this.mCurrentPath = item.path
                            logInfo(this.TAG, 'item.path ' + item.path)
                            let topPathInfo: Array<TopPathInfo> = AppStorage.Get('topPathInfo')
                            let index: number = topPathInfo.indexOf(item)
                            if (index == 1) {
                              this.mMenuLevel = MenuLevel.MENU_SECOND_LEVEL_BY_FOLD
                              this.mType = changeStringToType(item.name)
                              this.mCurrentPath = item.path
                            } else if (index > 1) {
                              var deleteNum = topPathInfo.length - index + 1
                              topPathInfo.splice(index + 1, deleteNum)
                              topPathInfo[topPathInfo.length - 1].opacity = 1
                              AppStorage.Set('topPathInfo', topPathInfo)
                            }
                          } else if (this.mMenuLevel == MenuLevel.MENU_THIRD_LEVEL_HAS_PIC) {
                            let topPathInfo: Array<TopPathInfo> = AppStorage.Get('topPathInfo')
                            let index: number = topPathInfo.indexOf(item)
                            if (index != -1) {
                              this.mMenuLevel = MenuLevel.MENU_SECOND_LEVEL_BY_FOLD
                              this.mType = changeStringToType(item.name)
                              this.mCurrentPath = item.path
                            }
                          }
                        })
                    }
                  }
                }, item => item.name)
              }
              .listDirection(Axis.Horizontal)
              .margin({ left: PadSize.TWELVE, top: PadSize.SEVENTEEN, right: PadSize.TWELVE })
            }
            .width(PadSize.FOUR_HUNDRED)
            .height(PadSize.FIFTY_SIX)
            .backdropBlur(PadSize.TWENTY)
            .backgroundColor(Color.White)
            .border({ radius: PadSize.TWENTY_FOUR, width: 0, style: BorderStyle.Solid })
            .shadow({ radius: 20, color: "#14000000" })
          }
          .width(PadSize.RIGHT_DIALOG_WIDTH)
          .height(PadSize.FIFTY_SIX)
          .margin({ top: PadSize.SIXTEEN })

          Row() {
            if (this.mMenuLevel == MenuLevel.MENU_FIRST_LEVEL_BY_TYPE) {
              FirstLevelByType({
                mFileList: this.mFirstMenuData,
                mType: $mType,
                mMenuLevel: $mMenuLevel,
                mCurrentPath: $mCurrentPath,
                mWorker: this.mWorker,
              })
            } else if (this.mMenuLevel == MenuLevel.MENU_SECOND_LEVEL_BY_FOLD) {
              SecondLevelByFold({
                mType: $mType,
                mMenuLevel: $mMenuLevel,
                mCurrentPath: $mCurrentPath,
                mWorker: this.mWorker,
                mFileList: $mSecondMenuData
              })
            } else if (this.mMenuLevel == MenuLevel.MENU_THIRD_LEVEL_HAS_PIC) {
              ThirdLevelHasPic({
                mType: $mType,
                mMenuLevel: $mMenuLevel,
                mCurrentPath: $mCurrentPath,
                mWorker: this.mWorker,
                mFileList: $mThirdMenuPicData
              })
            } else if (this.mMenuLevel == MenuLevel.MENU_THIRD_LEVEL_HAS_DETAIL) {
              ThirdLevelHasDetail({
                mType: $mType,
                mMenuLevel: $mMenuLevel,
                mCurrentPath: $mCurrentPath,
                mWorker: this.mWorker,
                mFileInfo: $mThirdMenuDetailData
              })
            }
          }
          .width(PadSize.RIGHT_DIALOG_WIDTH)
          .height(PadSize.TWO_HUNDRED_AND_SIXTY)
        }
        .width(PadSize.RIGHT_DIALOG_WIDTH)
        .height(PadSize.RIGHT_DIALOG_HEIGHT)
      }
      .width(PadSize.BOTTOM_WIDTH)

      Divider().color('#D8D8D8').opacity(1).strokeWidth(1)

      ButtonArea({
        mWorker: this.mWorker,
        mType: $mType,
        mMenuLevel: $mMenuLevel,
        mCurrentPath: $mCurrentPath
      })

    }
    .width(PadSize.MAIN_DIALOG_WIDTH)
    .height(PadSize.MAIN_DIALOG_HEIGHT)
    .border({ radius: PadSize.TWENTY_FOUR })
  }
}

@Component
struct LeftDialogComponent {
  private TAG: string = 'LeftDialogComponent'
  @Link @Watch('firstMenuDataChange') mFirstMenuData: FileInfo[]
  @Link mType: string
  @Link mMenuLevel: number
  @Link mCurrentPath: string
  @StorageLink('topPathInfo') topPathInfo: Array<TopPathInfo> = updateTopPathInfo([], '文件类型', '')

  aboutToAppear() {
    logInfo(this.TAG, 'aboutToAppear')
  }

  firstMenuDataChange() {
    logInfo(this.TAG, 'firstMenuDataChange')
    if (this.mFirstMenuData.length > 0) {
      this.subscribeMultiInput()
    }
  }

  private subscribeMultiInput() {
    logInfo(this.TAG, 'subscribeMultiInput')
    let mediaTypes: Array<string> = []
    logInfo(this.TAG, 'subscribeMultiInput mFirstMenuData = ' + this.mFirstMenuData.length)
    this.mFirstMenuData.forEach((item: FileInfo) => {
      mediaTypes.push(item.mediaType)
      logInfo(this.TAG, 'subscribeMultiInput mediaType = ' + item.mediaType)
    })

    let tabKeyOption = {
      preKeys: [], finalKey: 2049, isFinalKeyDown: true, finalKeyDownDuration: 0
    }
    let leftKeyOption = {
      preKeys: [], finalKey: 2014, isFinalKeyDown: true, finalKeyDownDuration: 0
    }
    let rightKeyOption = {
      preKeys: [], finalKey: 2015, isFinalKeyDown: true, finalKeyDownDuration: 0
    }
    let upKeyOption = {
      preKeys: [], finalKey: 2012, isFinalKeyDown: true, finalKeyDownDuration: 0
    }
    let downKeyOption = {
      preKeys: [], finalKey: 2013, isFinalKeyDown: true, finalKeyDownDuration: 0
    }

    inputConsumer.on('key', tabKeyOption, (keyOption) => {
      let index: number = mediaTypes.indexOf(this.mType)
      index = (index + 1) % mediaTypes.length
      this.processClickEvent(mediaTypes[index], this.mFirstMenuData[index].path, MenuLevel.MENU_SECOND_LEVEL_BY_FOLD)
    })
    inputConsumer.on('key', upKeyOption, (keyOption) => {
      let index: number = mediaTypes.indexOf(this.mType)
      index = (index - 1 + mediaTypes.length) % mediaTypes.length
      this.processClickEvent(mediaTypes[index], this.mFirstMenuData[index].path, MenuLevel.MENU_SECOND_LEVEL_BY_FOLD)
    })
    inputConsumer.on('key', downKeyOption, (keyOption) => {
      let index: number = mediaTypes.indexOf(this.mType)
      index = (index + 1) % mediaTypes.length
      this.processClickEvent(mediaTypes[index], this.mFirstMenuData[index].path, MenuLevel.MENU_SECOND_LEVEL_BY_FOLD)
    })
  }

  private processClickEvent(mediaType: string, currentPath: string, MenuLevel: number) {
    logInfo(this.TAG, 'processClickEvent')
    updateTopPathInfo([], '文件类型', '')
    updateTopPathInfo(AppStorage.Get('topPathInfo'), changeTypeToString(mediaType), currentPath)

    this.mMenuLevel = MenuLevel
    this.mType = mediaType
    this.mCurrentPath = currentPath
  }

  private checkSelectedItem(name: string): boolean {
    if (this.topPathInfo.length <= 1) {
      return false;
    }
    return this.topPathInfo[1].name == name
  }

  build() {
    Column() {
      Row() {
        Image($r("app.media.ic_public_folder"))
          .width(PadSize.IC_PUBLIC_FOLDER_WIDTH)
          .height(PadSize.IC_PUBLIC_FOLDER_HEIGHT)
          .margin({ left: PadSize.TWELVE })
        Text($r("app.string.file_type"))
          .fontSize(PadSize.SIXTEEN)
          .fontWeight(FontWeight.Medium)
          .fontColor('#182431')
          .margin({ left: PadSize.SIXTEEN })
        Blank()
        Image($r("app.media.ic_public_down"))
          .width(PadSize.TWENTY_FOUR)
          .height(PadSize.TWENTY_FOUR)
          .margin({ right: PadSize.TWENTY })
      }
      .width(PadSize.ONE_HUNDRED_AND_SEVENTY_SIX)
      .height(PadSize.FIFTY_SIX)
      .margin({ left: PadSize.TWELVE, top: PadSize.SIXTEEN, right: PadSize.TWELVE })
      .border({ radius: PadSize.TWENTY })
      .linearGradient({
        angle: 90,
        direction: GradientDirection.Left,
        colors: this.topPathInfo.length <= 1 ? [[0xDCEAF9, 0.0], [0xF1F3F5, 1]] : [[0xF8F9F9, 0.0], [0xF8F9F9, 1.0]]
      })
      .onClick((event: ClickEvent) => {
        if (this.mMenuLevel != MenuLevel.MENU_FIRST_LEVEL_BY_TYPE) {
          updateTopPathInfo([], '文件类型', '')
          this.mMenuLevel = MenuLevel.MENU_FIRST_LEVEL_BY_TYPE
          this.mType = ''
          this.mCurrentPath = ''
        }
      })

      List() {
        ForEach(this.mFirstMenuData, (item: FileInfo) => {
          ListItem() {
            Row() {
              Image(item.thumbnail)
                .width(PadSize.TWENTY_FOUR)
                .height(PadSize.TWENTY_FOUR)
                .margin({ left: PadSize.TWENTY_FOUR })
              Text(item.name)
                .fontSize(PadSize.SIXTEEN)
                .fontWeight(FontWeight.Medium)
                .fontColor('#182431')
                .margin({ left: PadSize.SIXTEEN })
            }
            .width(PadSize.ONE_HUNDRED_AND_SEVENTY_SIX)
            .height(PadSize.FIFTY_SIX)
            .margin({ left: PadSize.TWELVE, right: PadSize.TWELVE })
            .linearGradient({
              angle: 90,
              direction: GradientDirection.Left,
              colors: this.checkSelectedItem(item.name) ? [[0xDCEAF9, 0.0], [0xF1F3F5, 1]] : [[0xF8F9F9, 0.0], [0xF8F9F9, 1.0]]
            })
            .border({ radius: PadSize.TWENTY })
            .onClick((event: ClickEvent) => {
              this.mCurrentPath = item.path
              this.mMenuLevel = MenuLevel.MENU_SECOND_LEVEL_BY_FOLD
              this.mType = item.mediaType
              updateTopPathInfo([], '文件类型', '')
              updateTopPathInfo(AppStorage.Get('topPathInfo'), changeTypeToString(this.mType), this.mCurrentPath)
            })
          }
        }, item => item.name)
      }
    }
    .width(PadSize.LEFT_DIALOG_WIDTH)
    .height(PadSize.LEFT_DIALOG_HEIGHT)
    .backgroundColor("#FFF8F9F9")
  }
}

@Component
struct ButtonArea {
  private TAG: string = 'BottomArea'
  private mWorker
  @State currentChoseFiles: Array<string> = []
  @Link mType: string
  @Link mMenuLevel: number
  @Link mCurrentPath: string
  @StorageLink('choseFiles') @Watch('choseFilesChange') choseFiles: Array<FileInfo> = []

  aboutToAppear() {
    logInfo(this.TAG, 'current choose file number = ' + this.currentChoseFiles.length)
  }

  private choseFilesChange() {
    this.currentChoseFiles = []
    let files: Array<FileInfo> = AppStorage.Get('choseFiles')
    files.forEach((item: FileInfo) => {
      this.currentChoseFiles.push(item.name)
    })
    logInfo(this.TAG, 'current choose file number = ' + this.currentChoseFiles.length)
  }

  build() {
    Column() {
      Row() {
        Text($r("app.string.file_name"))
          .fontSize(PadSize.SIXTEEN)
          .fontWeight(FontWeight.Regular)
          .fontColor('#000000')
          .margin({ left: PadSize.EIGHT })
        Row() {
          if (globalThis.startMode == 'save') {
            Text(globalThis.saveFile)
              .height(PadSize.THIRTY)
              .maxLines(1)
              .margin({ left: PadSize.SIXTEEN })
          } else {
            Text(this.currentChoseFiles.toString())
              .height(PadSize.THIRTY)
              .maxLines(1)
              .margin({ left: PadSize.SIXTEEN })
          }
        }
        .width(PadSize.SIX_HUNDRED_AND_NINE)
        .height(PadSize.THIRTY_TWO)
        .border({ width: PadSize.ONE, radius: PadSize.SIXTEEN })
        .margin({ left: PadSize.SIXTEEN, right: PadSize.TWENTY_FOUR })
        .opacity(0.2)
      }
      .width(PadSize.SEVEN_HUNDRED_AND_FOURTEEN)
      .height(PadSize.SIXTY_FOUR)
      .margin({ left: PadSize.TWENTY_ONE, right: PadSize.NINE })

      Row() {
        Button() {
          Text($r('app.string.btn_cancel'))
            .fontSize(PadSize.SIXTEEN)
            .fontWeight(FontWeight.Medium)
            .fontColor('#007DFF')
        }
        .width(PadSize.ONE_HUNDRED_AND_SEVENTY_EIGHT)
        .height(PadSize.FORTY)
        .backgroundColor('#0C182431')
        .border({ radius: PadSize.TWENTY })
        .margin({ left: PadSize.THREE_HUNDRED_AND_FORTY })
        .onClick(() => {
          terminateSelfWithResult(TerminateReason.NO_ACTION, '')
          AppStorage.Set('choseFiles', [])
        })

        if (globalThis.startMode == 'save') {
          Button() {
            Text($r('app.string.btn_save'))
              .fontSize(PadSize.SIXTEEN)
              .fontWeight(FontWeight.Medium)
              .fontColor(this.mMenuLevel != MenuLevel.MENU_FIRST_LEVEL_BY_TYPE ? '#007DFF' : "#99CBFF")
          }
          .width(PadSize.ONE_HUNDRED_AND_SEVENTY_EIGHT)
          .height(PadSize.FORTY)
          .backgroundColor(this.mMenuLevel != MenuLevel.MENU_FIRST_LEVEL_BY_TYPE ? '#0C182431' : '#EFF2F3')
          .enabled(this.mMenuLevel != MenuLevel.MENU_FIRST_LEVEL_BY_TYPE)
          .border({ radius: PadSize.TWENTY })
          .margin({ left: PadSize.TWENTY_FOUR, right: PadSize.TWENTY_FOUR })
          .stateEffect(this.mMenuLevel == MenuLevel.MENU_FIRST_LEVEL_BY_TYPE ? false : true)
          .onClick(() => {
            logInfo(this.TAG, `path: ${this.mCurrentPath}`)
            this.mWorker.postMessage({
              request_data: 'createFile',
              device_name: 'local',
              MediaType: this.mType,
              path: this.mCurrentPath,
              save_name: globalThis.saveFile,
              context: globalThis.context,
            })
          })
        } else {
          Button() {
            Text($r('app.string.btn_upload'))
              .fontSize(PadSize.SIXTEEN)
              .fontWeight(FontWeight.Medium)
              .fontColor(this.currentChoseFiles.length != 0 ? '#007DFF' : "#99CBFF")
          }
          .width(PadSize.ONE_HUNDRED_AND_SEVENTY_EIGHT)
          .height(PadSize.FORTY)
          .backgroundColor(this.currentChoseFiles.length != 0 ? '#0C182431' : '#EFF2F3')
          .enabled(this.currentChoseFiles.length != 0)
          .border({ radius: PadSize.TWENTY })
          .margin({ left: PadSize.TWENTY_FOUR, right: PadSize.TWENTY_FOUR })
          .stateEffect(this.currentChoseFiles.length != 0)
          .onClick(() => {
            let result: string = ''
            this.choseFiles.forEach((item: FileInfo) => {
              result += item.path + ';'
            })
            terminateSelfWithResult(TerminateReason.SUCCESS, result)
            AppStorage.Set('choseFiles', [])
          })
        }
      }
    }
    .width(PadSize.MAIN_DIALOG_WIDTH)
    .height(PadSize.BOTTOM_HEIGHT)
    .margin({ bottom: PadSize.FOUR })
    .backgroundColor('#FFF8F9F9')
  }
}