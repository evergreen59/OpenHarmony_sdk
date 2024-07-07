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
const re = require("./re");
let vscode = null;
try {
    vscode = require('vscode');
}
catch (err) {
    vscode = null;
}

function print(...args) {
    if (vscode) {
        vscode.window.showInformationMessage(...args);
    }
    console.log(...args);
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

String.prototype.replaceAll = function (...args) {
    let result = this;
    while (result.indexOf(args[0]) >= 0) {
        result = result.replace(args[0], args[1])
    }
    return result;
}

function checkOutBody(body, off, flag, binside) {
    off = off || 0;
    flag = flag || ["{", "}"];
    binside = binside || false;
    let idx = {
        "(": ")",
        "{": "}",
        "<": ">",
        //"<": "<",
        //">": ">",
    };
    let csl = {};
    let csr = {};
    for (let f in idx) {
        csl[f] = 0
        csr[idx[f]] = 0
    }
    let cs1 = 0
    if (flag[0].length > 0 && body.substring(off, off + flag[0].length) != flag[0]) {
        return null;
    }

    for (let i = off + flag[0].length; i < body.length; i++) {
        if (body[i] == '"') cs1 += 1
        if (cs1 % 2 == 0) {
            let tb1 = true;
            for (let k in csl) {
                if (csl[k] != csr[idx[k]]) {
                    tb1 = false;
                    break;
                }
            }
            if (tb1 && body.substring(i, i + flag[1].length) == flag[1]) {
                if (binside)
                    return body.substring(off + flag[0].length, i);
                return body.substring(off, i + flag[1].length);
            }

            if (body[i] in csl) {
                csl[body[i]] += 1;
                if (body[i] in csr) csr[body[i]] += 1;
            }
            if (body[i] in csr) {
                if (!(body[i] == '>' && body[i-1] == '=')) { // 尖括号匹配时忽略关键字 "=>"
                    csr[body[i]] += 1;
                }
            }
        }
    }

    return null;
}

function removeExplains(data) {
    while (data.indexOf("/*") >= 0) {
        let i1 = data.indexOf("/*")
        let i2 = data.indexOf("*/") + 2
        data = data.substring(0, i1) + data.substring(i2, data.length)
    }
    while (true) {
        let tt = re.search("\n *//([a-zA-Z .]+)\n", data)
        if (tt != null) {
            data = data.substring(0, tt.regs[0][0]) + data.substring(tt.regs[0][1], data.length)
        }
        else break;
    }
    return data
}

function getLicense(data) {
    while (data.indexOf("/*") >= 0) {
        let i1 = data.indexOf("/*")
        let i2 = data.indexOf("*/") + 2
        let licenseData = data.substring(i1, i2)
        if (licenseData.search("Copyright")) {
            return licenseData
        }
    }
}

function removeEmptyLine(data) {
    while (data.indexOf("\r") >= 0) {
        data = data.replace("\r", "")
    }
    while (data.indexOf(" \n") >= 0) {
        data = data.replace(" \n", "\n")
    }
    while (data.indexOf("\n ") >= 0) {
        data = data.replace("\n ", "\n")
    }
    while (data.indexOf("\n\n") >= 0) {
        data = data.replace("\n\n", "\n")
    }
    while (data.indexOf("\n") == 0) {
        data = data.substring(1, data.length)
    }
    while (data.indexOf(" ") == 0) {
        data = data.substring(1, data.length)
    }
    return data
}

function replaceTab(data) {
    while (data.indexOf("\t") >= 0) {
        data = data.replace("\t", "    ")
    }
    return data
}

function removeEmptyLine2(data) {
    while (data.indexOf(" \n"))
        data = data.replace(" \n", "\n")
    while (data.indexOf("\n\n\n"))
        data = data.replace("\n\n\n", "\n\n")
    return data
}

function replaceAll(s, sfrom, sto) {
    while (s.indexOf(sfrom) >= 0) {
        s = s.replace(sfrom, sto)
    }
    return s;
}

/**
 * 比较两个方法是否完全相同
 * @param func1 方法1
 * @param func2 方法2
 * @returns 方法名称与形参是否完全相同
 */
 function isSameFunc(func1, func2) {
    if (func1.name != func2.name) { // 判断方法名称是否相同
        return false;
    }

    let func1ParamCount = func1.value.length
    if (func1ParamCount != func2.value.length) { // 判断方法形参个数是否一样
        return false;
    }

    for (let i in func1.value) { // 判断方法每个形参数据类型是否相同
        if (func1.value[i].type != func2.value[i].type) { 
            if (!(func1.value[i].type.indexOf("NUMBER_TYPE_") >= 0 &&
                func2.value[i].type.indexOf("NUMBER_TYPE_") >= 0)) {
                return false;
            }
        }
    }

    // 以上全部相同，判定为相同方法
    return true;
}

/**
 * 将方法对象插入列表（重复的方法对象不插入）
 * @param obj 待插入的方法对象
 * @param list 目标列表
 * @returns void
 */
 function addUniqFunc2List(obj, list) {
    for (let i in list) {
        if (isSameFunc(obj, list[i])) {
            return
        }
    }
    list.push(obj)
}

/**
 * 将对象插入列表（名称重复的属性对象不插入）
 * @param obj 待插入的对象
 * @param list 目标列表
 * @returns void
 */
 function addUniqObj2List(obj, list) {
    for (let i in list) {
        if (list[i].name === obj.name) {
            return
        }
    }
    list.push(obj)
}

/**
 * 如果方法所在的类为基类，生成的c++函数定义为虚函数
 * @param data 方法所在的类信息
 * @param isStatic ts方法是否定义为静态方法
 * return tabStr 缩进，staticStr 静态函数关键词，virtualStr 虚函数关键词
 */
 function getPrefix(data, isStatic) {
    let tabStr = ""
    let virtualStr = ""
    let staticStr = isStatic ? "static " : ""
    if (data.childList) {
        tabStr = "    " // 类中的方法增加一个缩进
        virtualStr = (data.childList.length > 0 && !isStatic) ? "virtual " : "" //如果是基类中的非静态方法，定义为虚函数
    }
    return [tabStr, staticStr, virtualStr]
}

module.exports = {
    checkOutBody,
    removeExplains,
    removeEmptyLine,
    removeEmptyLine2,
    replaceAll,
    print,
    getLicense,
    replaceTab,
    addUniqObj2List,
    addUniqFunc2List,
    getPrefix
}
