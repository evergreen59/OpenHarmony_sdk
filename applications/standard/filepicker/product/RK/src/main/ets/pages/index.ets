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
import { FirstLevelByType } from './FirstLevelByType'
import { SecondLevelByFold } from './SecondLevelByFold'
import { ThirdLevelHasDetail } from '../pages/ThirdLevelHasDetail'
import { ThirdLevelHasPic } from '../pages/ThirdLevelHasPic'
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
import { logInfo, logDebug, logError } from '../../../../../../common/src/main/ets/components/Utils/LogUtils'
import { changeStringToType, updateTopPathInfo } from '../../../../../../common/src/main/ets/components/Utils/Utils'
import { WarningDialog } from '../../../../../../common/src/main/ets/components/View/WarningDialog'

@Entry
@Component
struct allIndex {
  private TAG: string = 'Main'

  build() {
    Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.End, justifyContent: FlexAlign.SpaceBetween }) {
      Stack({ alignContent: Alignment.Bottom }) {
        Column() {
        }
        .onClick(() => {
          globalThis.context.terminateSelf((err, data) => {
            logInfo(this.TAG, 'terminateSelf success')
          })
        })
        .width('100%')
        .height('100%')

        Index()
      }
      .width('100%')
      .height('100%')
    }
    .backgroundColor('rgba(0,0,0,0.1)')
    .position({ x: 0, y: 0 })
    .width('100%')
    .height('100%')
  }
}

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
  @StorageLink('startMode') mCurrentMode: string = 'choose'
  @StorageLink('topPathInfo') topPathInfo: Array<TopPathInfo> = updateTopPathInfo([], '文件类型', '')
  scroller: Scroller = new Scroller()

  aboutToAppear() {
    if (globalThis.debugMode) {
      this.mFirstMenuData = getFirstMenu('')
      logDebug(this.TAG, 'mFirstMenuData length = ' + this.mFirstMenuData.length)
    } else {
      logInfo(this.TAG, 'aboutToAppear')
      this.initWork()
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

  private initWork() {
    if (this.mWorker == undefined) {
      logInfo(this.TAG, 'initWorker')
      this.mWorker = new worker.Worker('RK/ets/workers/DataWorker.js', { type: 'classic', name: 'filePicker' })
      this.mWorker.onerror = this.onError.bind(this)
      this.mWorker.onmessageerror = this.onMessageError.bind(this)
      this.mWorker.onexit = this.onExit.bind(this)
      this.mWorker.onmessage = this.onMessage.bind(this)
    }
  }

  private onError(e): void{
    logError(this.TAG, 'onError = ' + JSON.stringify(e))
  }

  private onMessageError(e): void{
    logError(this.TAG, 'onMessageError = ' + JSON.stringify(e))
  }

  private onExit(): void{
    logInfo(this.TAG, 'onExit')
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
      for (var index = 0;index < file.length; index++) {
        var info = file[index]
        let item = new FileInfo(info.name)
        item.path = info.path
        item.type = info.type
        item.size = info.size
        item.addedTime = info.added_time
        item.modifiedTime = info.modified_time
        if (params.menu_level == MenuLevel.MENU_FIRST_LEVEL_BY_TYPE) {
          if (item.name == 'MEDIA_TYPE_IMAGE') {
            item.name = '图片'
            item.mediaType = 'image'
            item.thumbnail = $r("app.media.ic_type_image")
          } else if (item.name == 'MEDIA_TYPE_VIDEO') {
            item.name = '视频'
            item.mediaType = 'video'
            item.thumbnail = $r("app.media.ic_type_video")
          } else if (item.name == 'MEDIA_TYPE_AUDIO') {
            item.name = '音乐'
            item.mediaType = 'audio'
            item.thumbnail = $r("app.media.ic_type_audio")
          } else if (item.name == 'MEDIA_TYPE_FILE') {
            item.name = '文档'
            item.mediaType = 'file'
            item.thumbnail = $r("app.media.ic_type_file")
          }
        }

        if (params.menu_level == MenuLevel.MENU_THIRD_LEVEL_HAS_DETAIL) {
          if (this.mCurrentMode == 'save' && item.type != MediaType.MEDIA_TYPE_ALBUM) {
            break
          }
        }
        listData.push(item)
      }
      logInfo(this.TAG, 'onMessage menu_level = ' + params.menu_level + ' request_data = ' + params.request_data)
      if (params.menu_level == MenuLevel.MENU_FIRST_LEVEL_BY_TYPE && params.request_data == 'getRoot') {
        logInfo(this.TAG, 'mFirstMenuData length = ' + listData.length)
        this.mFirstMenuData = listData
      } else if (params.menu_level == MenuLevel.MENU_SECOND_LEVEL_BY_FOLD) {
        logInfo(this.TAG, 'mSecondMenuData length = ' + listData.length)
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
    Column() {
      TopBarArea()

      Column() {
        Column() {
          Row() {
            List() {
              ForEach(this.topPathInfo, (item: TopPathInfo) => {
                ListItem() {
                  Row() {
                    if (item.name != '文件类型') {
                      Image($r("app.media.ic_public_right"))
                        .width(12 * 1.3)
                        .height(24 * 1.3)
                        .margin({
                          left: 4 * 1.3,
                          right: 4 * 1.3
                        })
                    }

                    Text(item.name)
                      .height(24 * 1.3)
                      .fontColor('#000000')
                      .fontSize(16 * 1.3)
                      .fontWeight(FontWeight.Medium)
                      .opacity(item.opacity)
                      .onClick(() => {
                        logInfo(this.TAG, item.name + ' onClick')
                        if (item.name == '文件类型') {
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
                  }
                }
              }, item => item.name.toString())
            }
            .padding({ right: 24 * 1.3 })
            .listDirection(Axis.Horizontal)
          }
          .alignItems(VerticalAlign.Center)
          .width('100%')
          .height(48 * 1.3)
          .margin({ left: 12 * 1.3, top: 4 * 1.3 })
          .padding({ top: 13 * 1.3, bottom: 13 * 1.3 })

          Scroll(this.scroller) {
            Column() {
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
              Row() {
              }.width('100%').height(53 * 1.3)
            }
            .alignItems(HorizontalAlign.Start)
          }
          .scrollBar(BarState.Off)
          .scrollable(ScrollDirection.Vertical)
        }
        .alignItems(HorizontalAlign.Start)
      }
      .alignItems(HorizontalAlign.Start)
      .height(globalThis.mainDialogHeight - 140 * 1.3)

      BottomBar({
        mCurrentPath: $mCurrentPath,
        mMenuLevel: $mMenuLevel,
        mType: $mType,
        mWorker: this.mWorker
      })
    }
    .padding({ left: 12 * 1.3, right: 12 * 1.3 })
    .width('100%')
    .height(globalThis.mainDialogHeight)
    .border({ radius: 24 })
    .backgroundColor('#F1F3F5')
  }
}

@Component
struct TopBarArea {
  build() {
    Row() {
      Text('文件类型')
        .width(98 * 1.3)
        .height(33 * 1.3)
        .fontSize(24 * 1.3)
        .fontColor('#182431')
        .fontWeight(FontWeight.Medium)
        .margin({ left: 12 * 1.3 })
    }
    .height(64 * 1.3)
    .width('100%')
    .border({ radius: 24 })
    .padding({ top: 8 * 1.3 })
  }
}

@Component
struct BottomBar {
  private TAG: string = 'BottomBar'
  private mWorker
  @State currentChoseFiles: Array<string> = []
  @Link mType: string
  @Link mMenuLevel: number
  @Link mCurrentPath: string
  @StorageLink('choseFiles') @Watch('choseFileChange') choseFiles: Array<FileInfo> = []

  aboutToAppear() {
    logInfo(this.TAG, 'current choose file number = ' + this.currentChoseFiles.length)
  }

  private choseFileChange() {
    this.currentChoseFiles = []
    let files: Array<FileInfo> = AppStorage.Get('choseFiles')
    files.forEach((item: FileInfo) => {
      this.currentChoseFiles.push(item.name)
    })
    logInfo(this.TAG, 'current choose file number = ' + this.currentChoseFiles.length)
  }

  build() {
    Row() {
      Button() {
        Text('取消')
          .width(110 * 1.3)
          .height(22 * 1.3)
          .fontSize(16 * 1.3)
          .fontWeight(FontWeight.Medium)
          .fontColor('#007DFF')
          .textAlign(TextAlign.Center)
      }
      .width(150 * 1.3)
      .height(40 * 1.3)
      .margin({ left: 12 * 1.3, top: 12 * 1.3, bottom: 24 * 1.3 })
      .backgroundColor('#E5E8EA')
      .onClick((event: ClickEvent) => {
        terminateSelfWithResult(TerminateReason.NO_ACTION, '')
        AppStorage.Set('choseFiles', [])
      })

      if (globalThis.startMode == 'save') {
        Button() {
          Text($r("app.string.btn_save"))
            .width(110 * 1.3)
            .height(22 * 1.3)
            .fontSize(16 * 1.3)
            .fontWeight(FontWeight.Medium)
            .fontColor('#007DFF')
            .textAlign(TextAlign.Center)
            .opacity(this.mMenuLevel != MenuLevel.MENU_FIRST_LEVEL_BY_TYPE ? 1 : 0.4)
        }
        .backgroundColor(this.mMenuLevel != MenuLevel.MENU_FIRST_LEVEL_BY_TYPE ? '#E5E8EA' : '#ECEEF0')
        .margin({
          left: 12 * 1.3,
          top: 12 * 1.3,
          bottom: 24 * 1.3
        })
        .height(40 * 1.3)
        .width(150 * 1.3)
        .stateEffect(this.mMenuLevel == MenuLevel.MENU_FIRST_LEVEL_BY_TYPE ? false : true)
        .onClick((event: ClickEvent) => {
          this.mWorker.postMessage({
            request_data: 'createFile',
            device_name: 'local',
            menu_level: this.mMenuLevel,
            MediaType: this.mType,
            path: this.mCurrentPath,
            save_name: globalThis.saveFile,
            context: globalThis.context,
          })
        })
      } else {
        Button() {
          Text($r("app.string.btn_upload"))
            .width(110 * 1.3)
            .height(22 * 1.3)
            .fontSize(16 * 1.3)
            .fontWeight(FontWeight.Medium)
            .fontColor('#007DFF')
            .textAlign(TextAlign.Center)
            .opacity(this.choseFiles.length != 0 ? 1 : 0.4)
        }
        .backgroundColor(this.choseFiles.length != 0 ? '#E5E8EA' : '#ECEEF0')
        .stateEffect(this.choseFiles.length != 0)
        .margin({
          left: 12 * 1.3,
          top: 12 * 1.3,
          bottom: 24 * 1.3
        })
        .height(40 * 1.3)
        .width(150 * 1.3)
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
    .height(76 * 1.3)
    .width('100%')
    .backgroundColor('#F1F3F5')
  }
}