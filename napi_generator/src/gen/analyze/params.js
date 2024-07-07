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
const re = require("../tools/re");
const { checkOutBody, print } = require("../tools/tool");
const { FuncType } = require("../tools/common");
const { NapiLog } = require("../tools/NapiLog");

/**函数参数解析 */
function analyzeParams(funcName, values) {
    let result = []
    let funcType = FuncType.DIRECT
    let optionalParamCount = 0; //可选参数的个数
    while (values.length > 0) {
        let v = checkOutBody(values, 0, ["", ","])
        if (v == null)
            v = values
        values = values.substring(v.length, values.length)
        let matchs = re.match("([a-zA-Z_0-9\\.]+)(\\?*): *([a-zA-Z<,>|_0-9\\[\\]\\(\\):='{}]+)", v)
        if (matchs != null) {
            let type = re.getReg(v, matchs.regs[3])
            if (type.indexOf("Map") < 0) {
                type = type.replace(/,/g, "")
            }

            let optionalFlag = re.getReg(v, matchs.regs[2]) == '?' ? true : false;
            let checkParamOk = true;
            if (optionalFlag) {
                optionalParamCount++;
            } else if (optionalParamCount > 0) {
                // 可选参数之后不能再有必选参数，必选都是可选参数。
                NapiLog.logError("Invalid parameter [%s] of function [%s],".format(v, funcName) 
                        + " the required parameter cannot follow an optional parameter.");
                checkParamOk = false;
            } 
            if (checkParamOk) {
                result.push({ "name": re.getReg(v, matchs.regs[1]), "type": type , "optional": optionalFlag})
                if (type.indexOf("AsyncCallback") >= 0)
                    funcType = FuncType.ASYNC
                if (funcType == FuncType.DIRECT && type.indexOf("Callback") >= 0 && type.indexOf("AsyncCallback") < 0)
                    funcType = FuncType.SYNC
            }
        }
        else {
            NapiLog.logError("方法[%s]的参数列表[%s]解析失败。".format(funcName, v));
            NapiLog.logError("Failed to analyse parameter [%s] of function [%s].".format(v, funcName));
        }
    }
    return [result, funcType]
}

module.exports = {
    analyzeParams
}