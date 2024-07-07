/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

const path = require("path");
const fs = require('fs');

function checkEntry(url) {
  let result = '';
  const sourceDirname = __dirname;
  __dirname = 'interface/sdk-js/build-tools/api_check_plugin';
  const mdFilesPath = path.resolve(sourceDirname, '../../../../', 'all_files.txt');
  try {
    const execSync = require("child_process").execSync;
    execSync("cd interface/sdk-js/build-tools/api_check_plugin && npm install");
    const { scanEntry } = require(path.resolve(__dirname, "./src/api_check_plugin"));
    result = ['api_check: true'];
    const { removeDir } = require(path.resolve(__dirname, "./src/utils"));
    removeDir(path.resolve(__dirname, "node_modules"));
  } catch (error) {
    // catch error
    result = `API_CHECK_ERROR : ${error}`;
  } finally {
    const { writeResultFile } = require('./src/utils');
    writeResultFile(result, path.resolve(__dirname, './Result.txt'), {});
  }
}
checkEntry(process.argv[2]);
