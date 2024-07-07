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
const { EnumValueType } = require("../tools/common");
const { NapiLog } = require("../tools/NapiLog");
function generateEnum(name, data) {
    let implH = ""
    let implCpp = ""
    let midInitEnum = ""

    if (data.enumValueType == EnumValueType.ENUM_VALUE_TYPE_STRING) {
        implH = `\nclass %s {\npublic:\n`.format(name, implH)
    } else if (data.enumValueType == EnumValueType.ENUM_VALUE_TYPE_NUMBER){
        implH = `\nenum %s {\n`.format(name, implH)
    } else {
        NapiLog.logError(`The enum type[%s] is not support`.format(data.enumValueType));
        return {implH: "", implCpp: ""}
    }
    for (let i in data.element) {
        let v = data.element[i]
        if(midInitEnum == "") {                
            midInitEnum += '    std::map<const char *, std::any> enumMap%s;\n'.format(name)
        }
        
        if (data.enumValueType == EnumValueType.ENUM_VALUE_TYPE_STRING) {
            implH += `    static const std::string %s;\n`.format(v.name)
            implCpp += `\nconst std::string %s::%s = "%s";\n`.format(name, v.name, v.value)            
            midInitEnum += '    enumMap%s["%s"] = "%s";\n'.format(name, v.name, v.value)
        } else {
            if (v.value == '') {
                v.value = 0
            }
            implH += `    %s = %s,\n`.format(v.name, v.value)
            midInitEnum += '    enumMap%s["%s"] = %s;\n'.format(name, v.name, v.value)            
        }
    }
    midInitEnum += '    pxt->CreateEnumObject("%s", enumMap%s);\n'.format(name, name)
    implH += `};\n`
    let result = {
        implH: implH,
        implCpp: implCpp,
        midInitEnum: midInitEnum
    }
    return result
}
module.exports = {
    generateEnum
}