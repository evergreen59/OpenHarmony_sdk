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
const path = require('path');

class Tool {
    constructor() {

    }
    static CURRENT_TOOL_PATH = null;//生成工具或插件所在路径，用于找到res
    static OHOS_PROJECT_PATH = "/home/xx/ohos";
    static OHOS_PORTING_TO = "third_party/opencv";
    static OHOS_PRODUCT_OUTPUT_PATH = "out/rk3568-khdvk";//输出目录
    static OHOS_SUBSYSTEM_NAME = "common";
    static OHOS_PART_NAME = "common";

    static getResAbsPath(respath) {
        return path.join(Tool.CURRENT_TOOL_PATH, respath);
    }
    static getCMakeToolchain() {
        switch (process.platform) {
            case 'win32':
                return path.join(Tool.CURRENT_TOOL_PATH, "res/win64/ohos.toolchain.cmake");
            case 'linux':
                return path.join(Tool.CURRENT_TOOL_PATH, "res/linux/ohos.toolchain.cmake");
            default:
                Logger.err("不支持 %s 平台".format(process.platform));
                return "";
        }
    }
    static getMakeRaw() {
        switch (process.platform) {
            case 'win32':
                return path.join(Tool.CURRENT_TOOL_PATH, "res/win64/bin/make_raw.exe");
            case 'linux':
                return path.join(Tool.CURRENT_TOOL_PATH, "res/linux/bin/make_raw");
            default:
                Logger.err("不支持 %s 平台".format(process.platform));
                return "";
        }
    }
    static getMake() {
        switch (process.platform) {
            case 'win32':
                return path.join(Tool.CURRENT_TOOL_PATH, "res/win64/bin/make.exe");
            case 'linux':
                return path.join(Tool.CURRENT_TOOL_PATH, "res/linux/bin/make");
            default:
                Logger.err("不支持 %s 平台".format(process.platform));
                return "";
        }
    }
    static getCMake() {
        switch (process.platform) {
            case 'win32':
                return path.join(Tool.OHOS_PROJECT_PATH, "prebuilts/cmake/windows-x86/bin/cmake.exe");
            case 'linux':
                return path.join(Tool.OHOS_PROJECT_PATH, "prebuilts/cmake/linux-x86/bin/cmake");
            default:
                Logger.err("不支持 %s 平台".format(process.platform));
                return "";
        }
        
    }
    static swapPath(p, swapd) {
        while (p.indexOf("\\") >= 0) {
            p = p.replace("\\", "/");
        }
        return p;
    }

    static DIRECTORY_STACK = [];
    static pushd(path) {
        path = Tool.swapPath(path, false);
        Tool.DIRECTORY_STACK.push(process.cwd());
        process.chdir(path);
    }
    static popd() {
        let d = Tool.DIRECTORY_STACK.pop();
        if (d) {
            process.chdir(d);
        }
    }
    static BACKUP_DIRECTORY = [];
    static backupd(bkp) {
        Tool.BACKUP_DIRECTORY[bkp] = Tool.DIRECTORY_STACK.concat([process.cwd()]);
    }
    static recoverd(bkp) {
        Tool.DIRECTORY_STACK = [].concat(Tool.BACKUP_DIRECTORY[bkp])
        Tool.popd();
    }

    static TARGET_TYPE = {
        NONE: 0,
        MAKE: 1,
        GN: 2,
        CMAKE: 3,
        SCONS: 4,
        VS: 5,
    }
    static GENERATE_TARGET_TYPE = Tool.TARGET_TYPE.GN;

    static setTarget(type) {//设置生成目标
        Tool.GENERATE_TARGET_TYPE = type;
    }
    static generateTarget(projectPath, analyzeResult) {
        switch (Tool.GENERATE_TARGET_TYPE) {
            case Tool.TARGET_TYPE.NONE:
                break;
            case Tool.TARGET_TYPE.GN:
                const { GenerateGn } = require('./generate_gn');
                GenerateGn.generate(projectPath, analyzeResult);
                break;
            default:
                Logger.err("generate target not support");
                break;
        }
    }

    static MOCK_ENUM = {
        NO_MOCK: 1,
        MOCK_RECORD: 2,
        MOCK_RUN: 3,
    };
    static MOCK_TYPE = Tool.MOCK_ENUM.NO_MOCK;
}

String.prototype.format = function (...args) {
    var result = this;
    let reg = new RegExp("%[sd]{1}")
    for (let i = 0; i < args.length; i++) {
        let p = result.search(reg)
        if (p < 0) break;
        result = result.substring(0, p) + args[i] + result.substring(p + 2, result.length)
    }
    return result;
}

try {
    Tool.VSCODE_INST = require('vscode');
}
catch (err) {
    Tool.VSCODE_INST = null;
}

module.exports = {
    Tool
}


const Logger = require('./logger');