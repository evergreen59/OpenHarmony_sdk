/**
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

import fileio from '@ohos.fileio';
import mediaLibrary from '@ohos.multimedia.mediaLibrary';

export class FileIoModel {
  media;
  constructor() {
    this.media = mediaLibrary.getMediaLibrary(globalThis.certManagerAbilityContext);
  }

  getMediaFileSuffix(mediaUri, callback) {
    console.log('getMediaFileSuffix start  uri: ' + mediaUri);
    let fileKeyObj = mediaLibrary.FileKey;
    let fileType = mediaLibrary.MediaType.FILE;
    let imageType = mediaLibrary.MediaType.IMAGE;
    let videoType = mediaLibrary.MediaType.VIDEO;
    let audioType = mediaLibrary.MediaType.AUDIO;
    let imagesFetchOp = {
      selections: fileKeyObj.MEDIA_TYPE + '= ? or ' + fileKeyObj.MEDIA_TYPE + '= ? or ' +
        fileKeyObj.MEDIA_TYPE + '= ? or ' + fileKeyObj.MEDIA_TYPE + '= ?',
      selectionArgs: [fileType.toString(), imageType.toString(), videoType.toString(), audioType.toString()],
      uri: mediaUri,
    };
    let suffix;

    this.media.getFileAssets(imagesFetchOp, (error, fetchFileResult) => {
      console.log("getMediaFile getFileAssets callback in fetchFileResult: ");
      if (fetchFileResult != undefined) {
        fetchFileResult.getFirstObject(async (error, fileAsset) => {
          console.log("getMediaFile getFileAssets callback  getFirstObject: ");
          if (fileAsset) {
            console.log("getMediaFile getFileAssets fileAsset: " + fileAsset.displayName);
            suffix = fileAsset.displayName.substring(fileAsset.displayName.lastIndexOf(".") + 1);
            callback(suffix);
          } else {
            console.log("getMediaFile getFileAssets callback  getFirstObject else");
          }
        })
      } else {
        console.log("getMediaFile getFileAssets callback in else: ");
        callback(undefined);
      }
    })
  }

  getMediaFileData(mediaUri, callback) {
    console.log('getMediaFile start');
    let fileKeyObj = mediaLibrary.FileKey;
    let fileType = mediaLibrary.MediaType.FILE;
    let imageType = mediaLibrary.MediaType.IMAGE;
    let videoType = mediaLibrary.MediaType.VIDEO;
    let audioType = mediaLibrary.MediaType.AUDIO;
    let imagesFetchOp = {
      selections: fileKeyObj.MEDIA_TYPE + '= ? or ' + fileKeyObj.MEDIA_TYPE + '= ? or ' +
        fileKeyObj.MEDIA_TYPE + '= ? or ' + fileKeyObj.MEDIA_TYPE + '= ?',
      selectionArgs: [fileType.toString(), imageType.toString(), videoType.toString(), audioType.toString()],
      uri: mediaUri,
    };
    let fd;

    this.media.getFileAssets(imagesFetchOp, (error, fetchFileResult) => {
      if (fetchFileResult != undefined) {
        fetchFileResult.getFirstObject(async (error, fileAsset) => {
          console.log("getMediaFile getFileAssets fileAsset: " + fileAsset.displayName);

          if (fileAsset) {
            let buf = new ArrayBuffer(fileAsset.size);
            fd = await fileAsset.open("r");
            fileio.read(fd, buf, (err, data) => {
              if (data) {
                console.info("getMediaFile read file data exist ");
                callback(new Uint8Array(data.buffer));
              } else {
                console.info("getMediaFile read file data: null");
              }
            });
          }
        })
      }
    })
  }
}

let fileIoModel = new FileIoModel();

export default fileIoModel as FileIoModel;
