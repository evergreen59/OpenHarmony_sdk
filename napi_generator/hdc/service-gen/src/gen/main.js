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
const path = require("path");
const stdio = require("stdio");
const fs = require('fs');

const { NapiLog } = require("../tools/NapiLog");
const { print } = require("../tools/tool");
const analyze = require("./analyze");

let ops = stdio.getopt({
    'filename': { key: 'f', args: 1, description: ".d.ts file", default: "" },
    'directory': { key: 'd', args: 1, description: ".d.ts directory", default: "" },
    'out': { key: 'o', args: 1, description: "output directory", default: "." },
    'loglevel': { key: 'l', args: 1, description: "Log Level: 0~3", default: "1" },
    'serviceId': { key: 's', args: 1, description: "service register id: 9000~16777214", default: "9000" }
});

NapiLog.init(ops.loglevel, path.join("" + ops.out, "napi_gen.log"));

let fileNames = ops.filename;
var pathDir = ops.directory;
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
    NapiLog.logInfo("check file []".format(fileName));
    let suffix = fileName.split('.').pop().toLowerCase();
    if (suffix === 'h') {
        NapiLog.logInfo("Generating service code from file " + fileName);
        analyze.doAnalyze(fileName, ops);
    } else {
        NapiLog.logError('Only .h file is supported.');
    }
}

let ret = NapiLog.getResult();
if (ret[0]) {
    print('success');
    NapiLog.logInfo('success');
}
else {
    print('Finish with error: ' + ret[1]);
    NapiLog.logInfo('Finish with error: ' + ret[1]);
}
