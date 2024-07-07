/*
* Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd. 
* Licensed under the Apache License, Version 2.0 (the "License"); 
* you may not use this file except in compliance with the License. 
* You may obtain a copy of the License at 
*
* http://www.apache.org/licenses/LICENSE-2.0 
*
* Unless required by applicable law or agreed to in writing, software 
* distributed under the License is distributed on an "AS IS" BASIS, 
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
* See the License for the specific language governing permissions and 
* limitations under the License. 
*/
const main = require("./main");
const tsMain = require("../tsGen/tsMain");
const re = require("./tools/re");
const { checkFileError } = require("./tools/common");
const { NapiLog } = require("./tools/NapiLog");
const path = require("path");
const stdio = require("stdio");
var fs = require('fs');
const { print } = require("./tools/tool");

let ops = stdio.getopt({
    'filename': { key: 'f', args: 1, description: ".d.ts file", default: "" },
    'directory': { key: 'd', args: 1, description: ".d.ts directory", default: "" },
    'imports': { key: 'i', args: 1, description: "enable or disable support imports self-define file", default: false },
    'out': { key: 'o', args: 1, description: "output directory", default: "." },
    'loglevel': { key: 'l', args: 1, description: "Log Level : 0~3", default: "1" },
    // 新增控制number类型转C++类型参数
    'numbertype':{key: 'n', args: 1, description: "optional elemtype: basic cpp elemtype", default: "uint32_t"}
});

NapiLog.init(ops.loglevel, path.join("" + ops.out, "napi_gen.log"))

let fileNames = ops.filename;
var pathDir = ops.directory;
var imports = ops.imports;
if (fileNames == null && pathDir == null) {
    NapiLog.logInfo("fileNames and pathDir both cannot be empty at the same time");
} else if (pathDir != '') {
    readDirFiles();
} else if (fileNames != '') {
    readFiles();
}

function readFiles() {
    fileNames = fileNames.replace(/(^\s*)|(\s*$)/g, ''); // trim before and after espace
    let regex = ',';
    let filenameArray = fileNames.toString().split(regex);

    let n = filenameArray.length;
    for (let i = 0; i < n; i++) {
        let fileName = filenameArray[i];
        if (fileName !== ' ') {
            fileName = fileName.replace(/(^\s*)|(\s*$)/g, '');
            checkGenerate(fileName);
        }
    }
}

function readDirFiles() {
    fs.readdir(pathDir + '', function (err, files) {
        if (err) {
            NapiLog.logError('readdir file error' + err);
            return;
        }
        (function iterator(i) {
            if (i === files.length) {
                return;
            }
            fs.stat(path.join(pathDir + '', files[i]), function (err, data) {
                if (err) {
                    NapiLog.logError('read file error' + err);
                    return;
                }
                if (data.isFile()) {
                    let fileName = files[i];
                    checkGenerate(pathDir + '/' + fileName);
                }
                iterator(i + 1);
            });
        })(0);
    });
}

function checkGenerate(fileName) {
    NapiLog.logInfo("check file []".format(fileName))
    let suffix = fileName.split('.').pop().toLowerCase();
    if (suffix === 'h') {
        NapiLog.logInfo("convert .h file to .ts file...")
        tsMain.doGenerate(fileName, ops.out);
        return;
    }
    let fn = re.getFileInPath(fileName);
    let tt = re.match('@ohos([.a-z_A-Z0-9]+).d.ts', fn);
    if (tt) {
        let result = checkFileError(fileName);
        if (result[0]) {
            main.doGenerate(fileName, ops.out, imports, ops.numbertype);
        }
        else {
            NapiLog.logError(result[1]);
        }

    }
    else {
        NapiLog.logError('file name ' + fn + ' format invalid in function of checkGenerate!');
    }
}

let ret = NapiLog.getResult();
if (ret[0]) {
    print('success');
    NapiLog.logInfo('success');
}
else {
    print('fail\n' + ret[1]);
    NapiLog.logInfo('fail\n' + ret[1]);
}
