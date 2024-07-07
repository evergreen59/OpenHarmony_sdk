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
const tsc = require("../../node_modules/typescript");

function checkFileError(ifname) {
    let program = tsc.createProgram([ifname], {target: tsc.ScriptTarget.Latest,})
    let emitResult = program.emit();
    let allDiagnostics = tsc.getPreEmitDiagnostics(program).concat(emitResult.diagnostics);

    let errorMsg = ''
    allDiagnostics.forEach(diagnostic => {
        if (diagnostic.file) {
            let { line, character } = tsc.getLineAndCharacterOfPosition(diagnostic.file, diagnostic.start);
            let message = tsc.flattenDiagnosticMessageText(diagnostic.messageText, "\n");
            errorMsg += `${diagnostic.file.fileName} (${line + 1},${character + 1}): ${message}\n`;
        } else {
            errorMsg += tsc.flattenDiagnosticMessageText(diagnostic.messageText, "\n") + "\n";
        }
    });

    if (allDiagnostics.length > 0) {
        return [false, errorMsg];
    }
    return [true, ""];
}

class FuncType { }
FuncType.DIRECT = 1
FuncType.SYNC = 2
FuncType.ASYNC = 4
FuncType.PROMISE = 8
FuncType.ToString = function (type) {
    if (type == FuncType.DIRECT) return "DIRECT";
    else if (type == FuncType.SYNC) return "SYNC";
    else if (type == FuncType.ASYNC) return "ASYNC";
    else if (type == FuncType.PROMISE) return "PROMISE";
    return "UNKNOW";
}

class NumberIncrease { }
NumberIncrease.num = 1
NumberIncrease.getAndIncrease = function () {
    return NumberIncrease.num++;
}
NumberIncrease.get = function () {
    return NumberIncrease.num;
}
NumberIncrease.reset = function () {
    NumberIncrease.num = 1
}

class InterfaceList { }
InterfaceList.interfacess_ = [];
InterfaceList.push = function (ifs) {
    InterfaceList.interfacess_.push(ifs)
}
InterfaceList.pop = function () {
    InterfaceList.interfacess_.pop()
}
InterfaceList.getValue = function (name) {
    let ifs = InterfaceList.interfacess_[InterfaceList.interfacess_.length - 1]
    for (let i in ifs) {
        if (ifs[i].name == name) {
            return ifs[i].body.allProperties.values;
        }
    }
    return null;
}

class EnumList { }
EnumList.enum_ = [];
EnumList.push = function (ifs) {
    EnumList.enum_.push(ifs)
}
EnumList.pop = function () {
    EnumList.enum_.pop()
}
EnumList.getValue = function (name) {
    let ifs = EnumList.enum_[EnumList.enum_.length - 1]
    for (let i in ifs) {
        if (ifs[i].name == name) {
            return ifs[i].body.element;
        }
    }
    return null;
}

function getArrayType(type) {
    let tt = re.match("Array<([a-zA-Z_0-9]+)>", type)
    if (tt != null) {
        return re.getReg(type, tt.regs[1])
    } 
    
    tt = re.match("Array<{([[a-z:]+)([a-z:]]+)([a-zA-Z_1-9:]+)", type)
    if (tt != null) {
        let res = ''
        let len = tt.regs.length
        for (let i=1; i<len; i++) {
            let regs1 = re.getReg(type, tt.regs[i])
            res += regs1
        }          
        return res
    }
    
    tt = re.match("Array<map<string", type)
    if (tt != null) {
        let preStr = 'Array<'
        let preStrLen = preStr.length        
        let res = type.substring(preStrLen, type.length-1)              
        return res
    }

    tt = re.match("Array<Map<string", type)
    if (tt != null) {
        let preStr = 'Array<'
        let preStrLen = preStr.length        
        let res = type.substring(preStrLen, type.length-1)              
        return res
    } 
    return null
}

function getArrayTypeTwo(type) {
    let tt = re.match("([a-zA-Z_0-9]+)", type)
    return re.getReg(type, tt.regs[1])
}

function jsType2CType(jsTypeName) {
    if (jsTypeName == "string") {
        return "std::string"
    } else if (jsTypeName == "boolean") {
        return "bool"
    } else {
        return jsTypeName
    }
}

class EnumValueType { }
EnumValueType.ENUM_VALUE_TYPE_NUMBER = 0
EnumValueType.ENUM_VALUE_TYPE_STRING = 1

function isEnum(type, data) {
    let isEnum = false
    if (null == data) {
        return isEnum
    }
    for (let i in data.enum) {
        let enumm = data.enum[i]
        if (type == enumm.name) {
            isEnum = true
        }
    }
    return isEnum
}

function enumIndex(type, data) {
    let index;
    if (null == data) {
        return index
    }
    for (let i in data.enum) {
        let enumm = data.enum[i]
        if (type == enumm.name) {
            index = i
        }
    }
    return index
}

function getMapType(type) {
    type = type.replace(/\s*/g,"")
    let ttKey = re.search("Map<([a-zA-Z_0-9]+),", type)
    let ttValue = re.search(",([a-zA-Z_0-9<>]+)>", type)
    let ttMap = re.search(",([a-zA-Z_0-9]+)>>", type)
    let ttArray = re.search("Array<([a-zA-Z_0-9]+)>", type)

    if(ttArray == null) {
        ttArray = re.search("([a-zA-Z_0-9]+)\\[\\]>", type)
    }
    
    let valueType
    let valueMapType
    let valueArrayType
    if (ttKey == null && ttValue == null && ttMap == null) {
        ttKey = re.search("key:([a-zA-Z_0-9]+)", type)
        ttValue = re.search(":([a-zA-Z_0-9]+)}", type)
        ttMap = re.search(":([a-zA-Z_0-9]+)}}", type)
        ttArray = re.search("Array<([a-zA-Z_0-9]+)>", type)
        if (ttArray == null) {
            ttArray = re.search(":([a-zA-Z_0-9]+)\\[\\]}", type)
        }        
    }
    
    if (ttValue != null) {
        valueType = re.getReg(type, ttValue.regs[1])
        if (valueType.indexOf("Array<") == 0) {
            valueArrayType = re.getReg(valueType, ttArray.regs[1])
            valueType = undefined
        } else if (ttMap != undefined) {
            valueMapType = re.getReg(type, ttMap.regs[1])
            valueType = undefined
        }
    }
    if (ttMap != null) {
        valueMapType = re.getReg(type, ttMap.regs[1])
    }
    if (ttArray != null) {
        valueArrayType = re.getReg(type, ttArray.regs[1])
    }
    return [re.getReg(type, ttKey.regs[1]), valueType, valueMapType, valueArrayType]
}

function getUnionType(type) {
    type = type.replace(/\s*/g,"")
    var typeArr = new Array()
    typeArr = type.split("|")
    return typeArr
}


module.exports = {
    FuncType,
    EnumValueType,
    NumberIncrease,
    InterfaceList,
    getArrayType,
    getArrayTypeTwo,
    checkFileError,
    isEnum,
    enumIndex,
    getMapType,
    EnumList,
    jsType2CType,
    getUnionType
}