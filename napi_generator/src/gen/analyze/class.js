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
const { NumberIncrease } = require("../tools/common");

const { analyzeFunction } = require("./function");

/**interface解析 */
function analyzeClass(data) {
    // # replace(" ", "").
    let body = re.replaceAll(data, "\n", "").split(";")
    let result = {
        value: [],
        function: []
    }
    for (let i in body) {
        let classBody = body[i]
        // 去除前面的空格
        while (classBody.length > 0 && classBody[0] == ' ') {
            classBody = classBody.substring(1, classBody.length)
        }
        // 去除后面的空格
        while (classBody.length > 0 && classBody[-1] == ' ') {
            classBody = classBody.substring(0, classBody.length - 1)
        }
        // 如果t为空直接返回
        if (classBody == "") break
        let matcher = re.match(" *([a-zA-Z0-9_]+) *: *([a-zA-Z_0-9<>]+)", classBody)
        if (matcher) {
            let valueName = re.getReg(classBody, matcher.regs[1])
            let valueType = re.getReg(classBody, matcher.regs[2])
            if (valueType.indexOf("number") >= 0) {
                valueType = valueType.replace("number", "NUMBER_TYPE_" + NumberIncrease.getAndIncrease())
            }
            result.value.push({
                name: valueName,
                type: valueType
            })
        }
        let rules = "(static )? *([A-Za-z0-9_]+)\\(([\n a-zA-Z:;=,_0-9?<>{}|]*)\\) *: *([A-Za-z0-9_<>{}:, .]+)";
        matcher = re.match(rules, classBody)
        if (matcher) {
            let funcDetail = analyzeFunction(data, 
                re.getReg(classBody, matcher.regs[1]) != '', re.getReg(classBody, matcher.regs[2]),
                re.getReg(classBody, matcher.regs[3]), re.getReg(classBody, matcher.regs[4]))
            if (funcDetail != null)
                result.function.push(funcDetail)
        }
    }
    return result
}

module.exports = {
    analyzeClass
}