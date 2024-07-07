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

import fileAccess from '@ohos.file.fileAccess'
import fileExtensionInfo from '@ohos.file.fileExtensionInfo';
import worker from '@ohos.worker'
import { logInfo, logError } from '../../../../../../common/src/main/ets/components/Utils/LogUtils'

var TAG = 'DataWorker'
const parentPort = worker.parentPort
parentPort.onmessage = function (e) {
  logInfo(TAG, 'DataWorker onMessage');
  let data = e.data
  logInfo(TAG, 'DataWorker onMessage ' + data.request_data);

  let wantInfos = getFileAccessAbilityInfo();
  logInfo(TAG, 'DataWorker onMessage getFileAccessAbilityInfo' + JSON.stringify(wantInfos));

  let wantInfo = {
    bundleName: "com.ohos.medialibrary.medialibrarydata",
    abilityName: "FileExtensionAbility",
  };
  let fileAccessHelper = createFileAccessHelper(data, [wantInfo]);
  logInfo(TAG, 'DataWorker onMessage createFileAccessHelper' + JSON.stringify(fileAccessHelper));

  logInfo(TAG, 'DataWorker onMessage ' + data.request_data);
  if (data.request_data == 'getRoot') {
    getRoots(data, fileAccessHelper);
  } else if (data.request_data == 'listFile') {
    getFileInfo(data.path, fileAccessHelper).then((files) => {
      listFile(data, files);
    });
  } else if (data.request_data == 'createFile') {
    createFile(data, fileAccessHelper);
  } else if (data.request_data == 'coverFile') {
    coverFile(data, fileAccessHelper);
  }
}

function getFileAccessAbilityInfo() {
  let wantInfos = [];
  try {
    logInfo(TAG, 'fileAccess.getFileAccessAbilityInfo - start');
    fileAccess.getFileAccessAbilityInfo().then((wants) => {
      logInfo(TAG, 'fileAccess.getFileAccessAbilityInfo - end');
      wantInfos = wants;
    }).catch((error) => {
      logError(TAG, 'getFileAccessAbilityInfo error: ' + error);
    });
  } catch (error) {
    logError(TAG, 'getFileAccessAbilityInfo error: ' + error);
  }
  return wantInfos;
}

function createFileAccessHelper(data, wantInfos) {
  logInfo(TAG, 'createFileAccessHelper : context ' + data.context + ' wantInfos ' + wantInfos);
  let fileAccessHelper = null;
  try {
    logInfo(TAG, 'fileAccess.createFileAccessHelper - start');
    fileAccessHelper = fileAccess.createFileAccessHelper(data.context, wantInfos);
    logInfo(TAG, 'fileAccess.createFileAccessHelper - end');
  } catch (error) {
    logError(TAG, 'createFileAccessHelper error: ' + error);
    return null;
  }
  return fileAccessHelper;
}

parentPort.onmessageerror = function () {
  logInfo(TAG, 'onmessageerror');
}

parentPort.onerror = function (data) {
  logInfo(TAG, 'onerror');
}

async function getFileInfo(path, fileAccessHelper) {
  logInfo(TAG, 'getFileInfo path = ' + path);

  let firstFileInfos = await getFirstFileInfos(fileAccessHelper);
  let index = firstFileInfos.findIndex((fileInfo) => {
    return fileInfo.uri == path;
  })
  if (index != -1) {
    return firstFileInfos[index];
  }

  let fileInfo = findOtherFileInfo(fileAccessHelper, path, firstFileInfos);
  return fileInfo;
}

async function getFirstFileInfos(fileAccessHelper) {
  let rootIterator = null;
  let rootInfos = [];
  let isDone = false;
  let rootInfo = null;
  let fileInfos = [];
  try {
    logInfo(TAG, `fileAccessHelper.getRoots - start`);
    rootIterator = await fileAccessHelper.getRoots();
    logInfo(TAG, `fileAccessHelper.getRoots - end`);
    if (!rootIterator) {
      logInfo(TAG, `getFirstFileInfos return undefined`);
    }
    while (!isDone) {
      let result = rootIterator.next();
      isDone = result.done;
      if (!isDone) {
        rootInfos.push(result.value);
      }
    }

    rootInfo = rootInfos[0];
    isDone = false;
    try {
      logInfo(TAG, `rootInfo.listFile - start`);
      let fileIterator = rootInfo.listFile();
      logInfo(TAG, `rootInfo.listFile - end`);
      if (!fileIterator) {
        logInfo(TAG, `getFirstFileInfo fileIterator return undefined`);
      }
      while (!isDone) {
        let result = fileIterator.next();
        isDone = result.done;
        if (!isDone) {
          fileInfos.push(result.value);
        }
      }
    } catch (error) {
      logError(TAG, `getFirstFileInfo fileIterator error : ` + error);
    }
  } catch (error) {
    logError(TAG, `getFirstFileInfo error : ` + error);
  }
  return fileInfos;
}

function findOtherFileInfo(fileAccessHelper, path, fileInfos) {
  if (fileInfos.length == 0) {
    return null;
  }
  logInfo(TAG, 'findOtherFileInfo' + path + fileInfos.length);
  let index = fileInfos.findIndex((fileInfo) => {
    return fileInfo.uri == path;
  });
  if (index != -1) {
    return fileInfos[index];
  }

  let arr = fileInfos;
  for (let i = 0; i < arr.length; i++) {
    let fileInfo = arr[i];
    let files = getOtherFileInfos(fileInfo);
    if (files.length == 0) {
      continue;
    }
    let file = findOtherFileInfo(fileAccessHelper, path, files);
    if (JSON.stringify(file) !== 'undefined') {
      return file;
    }
  }
}

function getOtherFileInfos(fileInfo) {
  logInfo(TAG, `getOtherFileInfos ` + JSON.stringify(fileInfo) + fileInfo.uri);
  let subFileInfos = [];
  let isDone = false;
  if ((fileInfo.mode & fileExtensionInfo.DocumentFlag.REPRESENTS_FILE) == fileExtensionInfo.DocumentFlag.REPRESENTS_FILE) {
    logInfo(TAG, `getOtherFileInfos file cannot listFile`);
  }
  try {
    logInfo(TAG, `fileInfo.listFile - start`);
    let fileIterator = fileInfo.listFile();
    logInfo(TAG, `fileInfo.listFile - end`);
    if (!fileIterator) {
      logInfo(TAG, `getOtherFileInfos fileInfo return undefined`);
    }
    while (!isDone) {
      let result = fileIterator.next();
      isDone = result.done;
      if (!isDone) {
        subFileInfos.push(result.value);
      }
    }
  } catch (error) {
    logError(TAG, `getOtherFileInfos fileInfo error : ` + error);
  }
  return subFileInfos;
}

function getRoots(data, fileAccessHelper) {
  logInfo(TAG, `getRoots ` + JSON.stringify(data));
  let rootIterator = null;
  let rootInfos = [];
  let isDone = false;
  let rootInfo = null;
  try {
    rootIterator = null;
    logInfo(TAG, `fileAccessHelper.getRoots - start`);
    fileAccessHelper.getRoots().then(iterator => {
      logInfo(TAG, `fileAccessHelper.getRoots - end`);
      rootIterator = iterator;
      if (!rootIterator) {
        logInfo(TAG, `getRoots rootIterator return undefined`);
      }
      while (!isDone) {
        let result = rootIterator.next();
        isDone = result.done;
        if (!isDone) {
          rootInfos.push(result.value);
        }
      }

      rootInfo = rootInfos[0];
      let fileInfos = [];
      let files = [];
      isDone = false;
      try {
        logInfo(TAG, `rootInfo.listFile - start`);
        let fileIterator = rootInfo.listFile();
        logInfo(TAG, `rootInfo.listFile - start`);
        if (!fileIterator) {
          logInfo(TAG, `getListFile fileIterator return undefined`);
        }
        while (!isDone) {
          let result = fileIterator.next();
          isDone = result.done;
          if (!isDone) {
            fileInfos.push(result.value);
            files.push({
              name: result.value.fileName,
              path: result.value.uri,
              type: result.value.mimeType,
              size: result.value.size,
              addedTime: result.value.mTime,
              modifiedTime: result.value.mTime,
              mode: result.value.mode,
            })
          }
        }
      } catch (error) {
        logError(TAG, `getListFileData fileIterator error : ` + error)
      }
      handleData(files, data);
    }).catch((error) => {
      logError(TAG, 'getRoot error' + error.message)
    });
  } catch (error) {
    logError(TAG, `getRoots error : ` + error)
  }
}

function listFile(data, fileInfo) {
  logInfo(TAG, 'listFile start path = ' + data.path + " type = " + data.MediaType + " offset = " + data.offset)
  let subFileInfos = [];
  let files = [];
  let isDone = false;
  if ((fileInfo.mode & fileExtensionInfo.DocumentFlag.REPRESENTS_FILE) == fileExtensionInfo.DocumentFlag.REPRESENTS_FILE) {
    logInfo(TAG, `getListFileData file cannot listFile`);
  }
  try {
    logInfo(TAG, `fileInfo.listFile - start`);
    let fileIterator = fileInfo.listFile();
    logInfo(TAG, `fileInfo.listFile - end`);
    if (!fileIterator) {
      logInfo(TAG, `getListFile fileIterator return undefined`);
    }
    while (!isDone) {
      let result = fileIterator.next();
      isDone = result.done;
      if (!isDone) {
        subFileInfos.push(result.value);
        files.push({
          name: result.value.fileName,
          path: result.value.uri,
          type: result.value.mimeType,
          size: result.value.size,
          addedTime: result.value.mTime,
          modifiedTime: result.value.mTime,
          mode: result.value.mode,
        })
      }
    }
    handleData(files, data);
  } catch (error) {
    logError(TAG, `getListFileData fileInfo error : ` + error)
    handleData([], data)
  }
}

function createFile(data, fileAccessHelper) {
  logInfo(TAG, 'createFile path = ' + data.path + ' files = ' + data.save_name)

  logInfo(TAG, `createFile - start`);
  fileAccessHelper.createFile(data.path, data.save_name, (ret, uri) => {
    logInfo(TAG, `createFile - end`);
    data.retCode = ret.code;
    if (ret.code == 0) {
      handleData(uri, data)
    } else if (ret.code == -2002 || ret.code == -3000) {
      logError(TAG, 'created file type does not match the directory')
    } else {
      logError(TAG, 'createFile error ' + ret.code)
      handleData([], data)
    }
  })
}

function coverFile(data, fileAccessHelper) {
  logInfo(TAG, 'coverFile path = ' + data.path + ' files = ' + data.cover_name)

  getFileInfo(data.path, fileAccessHelper).then((file) => {
    let subFileInfos = getOtherFileInfos(file);
    let index = subFileInfos.findIndex((fileInfo) => {
      return fileInfo.fileName == data.cover_name;
    });
    let delFile = null;
    logInfo(TAG, `coverFile - delFile` + index);
    if (index != -1) {
      delFile = subFileInfos[index];
    }

    logInfo(TAG, `delete - start` + delFile.uri + " - " + delFile.fileName);
    fileAccessHelper.delete(delFile.uri, (ret, uri) => {
      logInfo(TAG, `delete - end` + ret.code + uri);
      data.retCode = ret.code;
      if (ret.code == 0) {
        data.save_name = data.cover_name;
        data.cover_name = "";
        createFile(data, fileAccessHelper);
      } else {
        logError(TAG, 'coverFile error ' + ret.code)
        handleData([], data)
      }
    })
  });
}

function handleData(file, data) {
  logInfo(TAG, 'handleData data' + JSON.stringify(data))
  var info = JSON.stringify(file)
  logInfo(TAG, `info = ${info}`)
  var buf = new ArrayBuffer(info.length * 2)
  var bufView = new Uint16Array(buf)
  for (var index = 0; index < info.length; index++) {
    bufView[index] = info.charCodeAt(index)
  }
  parentPort.postMessage({ data: bufView, params: data })
}

