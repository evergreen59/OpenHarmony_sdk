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
const { isMappedTypeNode } = require("typescript");
const { InterfaceList, getArrayType, NumberIncrease, enumIndex,
    isEnum, EnumValueType, getArrayTypeTwo, getMapType, EnumList,
    jsType2CType, getUnionType } = require("../tools/common");
const { NapiLog } = require("../tools/NapiLog");
const { print } = require("../tools/tool");

const specialPrefixArr = ["p->", "vio->out."];

/**
 * Get the real value name by deleting prefix like "p->", "vio->out.", e.g.
 * @param {*} valueName, example: p->xxx, vio->out.yyy
 * @returns the real value without prefix, example: xxx, yyy
 */
function delPrefix(valueName) {
    for ( var i in specialPrefixArr) {
        if (valueName.indexOf(specialPrefixArr[i]) == 0) {
            // Find special prefix and delete it.
            return valueName.substring(specialPrefixArr[i].length, valueName.length);
        }
    }
    // Without special prefix, nothing is changed.
    return valueName;
}

function cToJsForInterface(value, type, dest, deep) {
    let lt = deep
    let result = ""
    let ifl = InterfaceList.getValue(type)
    for (let i in ifl) {
        let name2 = ifl[i].name
        let type2 = ifl[i].type
        let isSubEnum = EnumList.getValue(type2) ? true : false;
        let subDest = isSubEnum ? dest : "tnv%d".format(lt)
        let interfaceType = cToJs("%s.%s".format(value, name2), type2, subDest, deep + 1)
        if (isSubEnum) {
            // interface include enum properties
            result += interfaceType 
        } else {
            result += "{\nnapi_value tnv%d = nullptr;\n".format(lt) +
            interfaceType + `\npxt->SetValueProperty(%s,"%s",tnv%d);\n}\n`
                .format(dest, name2, lt)
        }
    }
    return result
}

function cToJs(value, type, dest, deep = 1) {
    var propertyName = delPrefix(value);
    if (type.indexOf("|") >= 0) {
        return unionTempleteFunc(value, type, dest);
    } else if (type == "void")
        return "%s = pxt->UndefinedValue();".format(dest);
    else if (type == "boolean")
        return "%s = pxt->SwapC2JsBool(%s);".format(dest, value);
    else if (type == "string")
        return `%s = pxt->SwapC2JsUtf8(%s.c_str());`.format(dest, value)
    else if (InterfaceList.getValue(type)) {
        return cToJsForInterface(value, type, dest, deep);
    }
    else if(EnumList.getValue(type)){
        let lt = deep
        let result = ""
        let ifl = EnumList.getValue(type)
        let type2 = ifl[0].type
        let enumCtoJsStr = cToJs("enumInt%d".format(lt), type2, "tnv%d".format(lt), deep + 1)
        result += "{\nnapi_value tnv%d = nullptr;\n".format(lt) + "int enumInt%d = %s;\n".format(lt, value) + 
                enumCtoJsStr + `\npxt->SetValueProperty(%s,"%s",tnv%d);\n}\n`
                    .format(dest, propertyName, lt)
        return result
    }
    else if (type.substring(0, 6) == "Array<" || type.substring(type.length - 2) == "[]") {
        let arrayType = checkArrayParamType(type)
        return arrayTempleteFunc(arrayType, deep, dest, value)
    }
    else if (type.substring(0, 4) == "Map<" || type.indexOf("{[key:") == 0) {
        return mapTempleteFunc(type, deep, dest, value)
    }
    else if (type.substring(0, 12) == "NUMBER_TYPE_") {
        return `%s = NUMBER_C_2_JS(pxt, %s);`.format(dest, value)
    } 
    else if (type == "any") {
        return anyTempleteFunc(value)
    }
    else if (type == "Object" || type == "object") { 
        return objectTempleteFuncReturn(value)
    }
    else {
        NapiLog.logError(`\n---- This type do not generate cToJs %s,%s,%s ----\n`.format(value, type, dest));
    }
}

function objectTempleteFuncReturn(value) {
    let objectTemplete = `pxt->GetObjectValue(result, %s);`
        .format(value)
        return objectTemplete
}

function unionTempleteFunc(value, type, dest){
    let unionType = getUnionType(type)
    let unionTypeString = ''
    for (let i = 0; i < unionType.length; i++) {
        if (unionType[i] == "string") {
            unionTypeString += `if (%s_type == "string"){
                %s
                %s
            }\n`.format(value, "std::string union_string = std::any_cast<std::string>("+value+");",
            cToJs("union_string", unionType[i], dest))
        } else if (unionType[i].substring(0, 12) == "NUMBER_TYPE_") {
            unionTypeString += `if (%s_type == "number"){
                %s
                %s
            }\n`.format(value, "std::uint32_t union_number = std::any_cast<std::uint32_t>("+value+");",
            cToJs("union_number", unionType[i], dest))
        } else if (unionType[i] == "boolean") {
            unionTypeString += `if (%s_type == "boolean"){
                %s
                %s
            }\n`.format(value, "bool union_boolean = std::any_cast<bool>("+value+");",
            cToJs("union_boolean", unionType[i], dest))
        }
    }
    return unionTypeString
}

function checkArrayParamType(type) {
    let arrayType
    if (type.substring(type.length - 2) == "[]") {
        arrayType = getArrayTypeTwo(type)
    }
    else {
        arrayType = getArrayType(type)
    }
    return arrayType
}

function arrayTempleteFunc(arrayType, deep, dest, value) {
    let lt = deep
    let tnv = dest
    let tnvdef = `pxt->CreateArray(%s);
    uint32_t outLen%d = %s.size();
    for(uint32_t i = 0; i < outLen%d; i++) {
        napi_value tnv%d = nullptr;
        [calc_out]
        pxt->SetArrayElement(%s, i, tnv%d);
    }`.format(tnv, lt, value, lt, lt, tnv, lt)
    let ret = ""
    if (arrayType.substring(0, 12) == "NUMBER_TYPE_") {
        ret = tnvdef.replaceAll("[calc_out]", `tnv%d = NUMBER_C_2_JS(pxt,%s[i]);`.format(lt, value))
    }
    else if (arrayType == "string") {
        ret = tnvdef.replaceAll("[calc_out]", `tnv%d = pxt->SwapC2JsUtf8(%s[i].c_str());`.format(lt, value))
    }
    else if (arrayType == "boolean") {
        ret = tnvdef.replaceAll("[calc_out]", `tnv%d = pxt->SwapC2JsBool(%s[i]);`.format(lt, value))
    }
    else if (arrayType == "any") {
        return anyArrayTempleteFuncReturn(value)
    }
    else if (InterfaceList.getValue(arrayType)) {
        ret = tnvdef.replaceAll("[calc_out]", cToJs(value + "[i]", arrayType, "tnv" + lt, deep + 1))
    }
    return ret
}

function mapTempleteFunc(type, deep, dest, value) {
    let mapType = getMapType(type)
    let lt = deep
    let tnv = dest
    let tnvdef = `result = nullptr;
    for (auto i = %s.begin(); i != %s.end(); i++)
        {
            const char * tnv%d;
            napi_value tnv%d = nullptr;
            [calc_out]
            pxt->SetMapElement(%s, tnv%d, tnv%d);
        }`.format(value, value, lt, lt + 1, tnv, lt, lt + 1)
    let ret = ""
    if (mapType[1] != undefined && mapType[2] == undefined) {
        ret = mapTempleteValue(mapType, tnvdef, lt, value, tnv)
    }
    else if (mapType[2] != undefined) {
        ret = mapTempleteMap(mapType, tnvdef, lt)
    }
    else if (mapType[3] != undefined) {
        ret = mapTempleteArray(mapType, tnvdef, lt)
    }
    return ret
}

function anyTempleteFunc(value) {
    let anyTemplete = `pxt->GetAnyValue(%s_type, result, %s);`
        .format(value, value)
    
    return anyTemplete
}

function anyArrayTempleteFuncReturn(value) {
    let anyTemplete = `pxt->GetAnyValue(%s_type, result, %s);`
        .format(value, value)
    
    return anyTemplete
}

function mapInterface(value, lt, tnv, mapType) {
    let ret
    let tnvdefInterface = `result = nullptr;
    for (auto i = %s.begin(); i != %s.end(); i++)
    {
        const char *tnv%d;
        [calc_out]
    }`.format(value, value, lt, lt + 1, tnv, lt, lt + 1)
    let interfaceValue = InterfaceList.getValue(mapType[1])
    let interfaceVarName = ""
    let interfaceVar = ""
    let interfaceFun = ""
    for (let i = 0; i < interfaceValue.length; i++) {
        if (interfaceValue[i].type == 'string') {
            interfaceVarName += `const char * tnv_%s_name;
                napi_value tnv_%s = nullptr;\n`.format(interfaceValue[i].name, interfaceValue[i].name)
            interfaceVar += `tnv_%s_name = "%s";
                tnv_%s = pxt->SwapC2JsUtf8(i->second.%s.c_str());\n`
                .format(interfaceValue[i].name, interfaceValue[i].name, interfaceValue[i].name, interfaceValue[i].name)
            interfaceFun += `pxt->SetMapElement(result_obj, tnv_%s_name, tnv_%s);\n`
                .format(interfaceValue[i].name, interfaceValue[i].name, interfaceValue[i].name)
        }
        else if (interfaceValue[i].type.substring(0, 12) == "NUMBER_TYPE_") {
            interfaceVarName += `const char * tnv_%s_name;
                napi_value tnv_%s = nullptr;\n`.format(interfaceValue[i].name, interfaceValue[i].name)
            interfaceVar += `tnv_%s_name = "%s";
                tnv_%s = NUMBER_C_2_JS(pxt,i->second.%s);\n`
                .format(interfaceValue[i].name, interfaceValue[i].name, interfaceValue[i].name, interfaceValue[i].name)
            interfaceFun += `pxt->SetMapElement(result_obj, tnv_%s_name, tnv_%s);\n`
                .format(interfaceValue[i].name, interfaceValue[i].name, interfaceValue[i].name)
        }
        else if (interfaceValue[i].type == 'boolean') {
            interfaceVarName += `const char * tnv_%s_name;
                napi_value tnv_%s = nullptr;\n`.format(interfaceValue[i].name, interfaceValue[i].name)
            interfaceVar += `tnv_%s_name = "%s";
                tnv_%s = pxt->SwapC2JsBool(i->second.%s);\n`
                .format(interfaceValue[i].name, interfaceValue[i].name, interfaceValue[i].name, interfaceValue[i].name)
            interfaceFun += `pxt->SetMapElement(result_obj, tnv_%s_name, tnv_%s);\n`
                .format(interfaceValue[i].name, interfaceValue[i].name, interfaceValue[i].name)
        }
    }
    ret = tnvdefInterface.replaceAll("[calc_out]", `tnv%d = (i -> first).c_str();
        napi_value result_obj = nullptr;
        %s
        %s
        %s
        pxt->SetMapElement(result, tnv%d, result_obj);`
        .format(lt, interfaceVarName, interfaceVar, interfaceFun, lt))
    return ret
}

function mapTempleteValue(mapType, tnvdef, lt, value, tnv) {
    let ret
    if (mapType[1] == "string") {
        ret = tnvdef.replaceAll("[calc_out]", `tnv%d = (i -> first).c_str();
        tnv%d = pxt->SwapC2JsUtf8(i->second.c_str());`.format(lt, lt + 1))
    } else if (mapType[1] == "boolean") {
        ret = tnvdef.replaceAll("[calc_out]", `tnv%d = (i -> first).c_str();
        tnv%d = pxt->SwapC2JsBool(i->second);`.format(lt, lt + 1))
    } else if (mapType[1].substring(0, 12) == "NUMBER_TYPE_") {
        ret = tnvdef.replaceAll("[calc_out]", `tnv%d = (i -> first).c_str();
        tnv%d = NUMBER_C_2_JS(pxt,i->second);`.format(lt, lt + 1))
    } else if (mapType[1] == "any") {
        ret = tnvdef.replaceAll("[calc_out]", `tnv%d = (i -> first).c_str();
        pxt->GetAnyValue(%s_type, tnv%d, i->second);`.format(lt, value, lt + 1))
    }
    else if (InterfaceList.getValue(mapType[1])) {
        ret = mapInterface(value, lt, tnv, mapType)
    }
    else
        NapiLog.logError(`This type do not generate cToJs %s,%s,%s`.format(value, type, dest));
    return ret
}

function mapTempleteMap(mapType, tnvdef, lt) {
    let ret
    if (mapType[2] == "string") {
        ret = tnvdef.replaceAll("[calc_out]", `tnv%d = i->first.c_str();
        for(auto j = i->second.begin(); j != i->second.end(); j++){
            const char * tt%d;
            napi_value tt%d;
            tt%d = j->first.c_str();
            tt%d = pxt->SwapC2JsUtf8(j->second.c_str());
            pxt->SetMapElement(tnv%d, tt%d, tt%d);
        }`.format(lt, lt + 2, lt + 3, lt + 2, lt + 3, lt + 1, lt + 2, lt + 3))
    }
    else if (mapType[2] == "boolean") {
        ret = tnvdef.replaceAll("[calc_out]", `tnv%d = i->first.c_str();
        for(auto j = i->second.begin(); j != i->second.end(); j++){
            const char * tt%d;
            napi_value tt%d;
            tt%d = j->first.c_str();
            tt%d = pxt->SwapC2JsBool(j->second);
            pxt->SetMapElement(tnv%d, tt%d, tt%d);
        }`.format(lt, lt + 2, lt + 3, lt + 2, lt + 3, lt + 1, lt + 2, lt + 3))
    }
    if (mapType[2].substring(0, 12) == "NUMBER_TYPE_") {
        ret = tnvdef.replaceAll("[calc_out]", `tnv%d = i->first.c_str();
        for(auto j = i->second.begin(); j != i->second.end(); j++){
            const char * tt%d;
            napi_value tt%d;
            tt%d = j->first.c_str();
            tt%d = NUMBER_C_2_JS(pxt,j->second);
            pxt->SetMapElement(tnv%d, tt%d, tt%d);
        }`.format(lt, lt + 2, lt + 3, lt + 2, lt + 3, lt + 1, lt + 2, lt + 3))
    }
    return ret
}

function mapTempleteArray(mapType, tnvdef, lt) {
    let ret
    if (mapType[3] == "string") {
        ret = tnvdef.replaceAll("[calc_out]", `napi_value tnv%d = nullptr;
        pxt->CreateArray(tnv%d);
        tnv%d = (i -> first).c_str();
        uint32_t len%d = i->second.size();
        for(uint32_t j=0;j<len%d;j++) {
            tnv%d = pxt->SwapC2JsUtf8(i->second[j].c_str());
            pxt->SetArrayElement(tnv%d, j, tnv%d);
        }`.format(lt + 2, lt + 2, lt, lt, lt, lt + 2, lt + 1, lt + 2))
    } else if (mapType[3] == "boolean") {
        ret = tnvdef.replaceAll("[calc_out]", `napi_value tnv%d = nullptr;
        pxt->CreateArray(tnv%d);
        tnv%d = (i -> first).c_str();
        uint32_t len%d = i->second.size();
        for(uint32_t j=0;j<len%d;j++) {
            tnv%d = pxt->SwapC2JsBool(i->second[j]);
            pxt->SetArrayElement(tnv%d, j, tnv%d);
        }`.format(lt + 2, lt + 2, lt, lt, lt, lt + 2, lt + 1, lt + 2))
    } else if (mapType[3].substring(0, 12) == "NUMBER_TYPE_") {
        ret = tnvdef.replaceAll("[calc_out]", `napi_value tnv%d = nullptr;
        pxt->CreateArray(tnv%d);
        tnv%d = (i -> first).c_str();
        uint32_t len%d = i->second.size();
        for(uint32_t j=0;j<len%d;j++) {
            tnv%d = NUMBER_C_2_JS(pxt,i->second[j]);
            pxt->SetArrayElement(tnv%d, j, tnv%d);
        }`.format(lt + 2, lt + 2, lt, lt, lt, lt + 2, lt + 1, lt + 2))
    }
    return ret
}

function returnGenerateMap(returnInfo, param) {
    let type = returnInfo.type
    let mapType = getMapType(type)
    let mapTypeString
    if (mapType[1] != undefined && mapType[2] == undefined) {
        if (mapType[1] == "string") { mapTypeString = "std::string" }
        else if (mapType[1].substring(0, 12) == "NUMBER_TYPE_") { mapTypeString = mapType[1] }
        else if (mapType[1] == "boolean") { mapTypeString = "bool" }
        else if (mapType[1] == "any") { mapTypeString = "std::any" }
        else { mapTypeString = mapType[1] }
    }
    else if (mapType[2] != undefined) {
        if (mapType[2] == "string") { mapTypeString = "std::map<std::string,std::string>" }
        else if (mapType[2].substring(0, 12) == "NUMBER_TYPE_") { "std::map<std::string,"+mapType[2]+">" }
        else if (mapType[2] == "boolean") { mapTypeString = "std::map<std::string,bool>" }
    }
    else if (mapType[3] != undefined) {
        if (mapType[3] == "string") { mapTypeString = "std::vector<std::string>" }
        else if (mapType[3].substring(0, 12) == "NUMBER_TYPE_") { mapTypeString = "std::vector<"+mapType[3]+">" }
        else if (mapType[3] == "boolean") { mapTypeString = "std::vector<bool>" }
    }
    let modifiers = returnInfo.optional ? "*" : "&"
    param.valueOut = returnInfo.optional ? "std::map<std::string,%s>* out = nullptr;".format(mapTypeString)
                                         : "std::map<std::string,%s> out;".format(mapTypeString)
        param.valueDefine += "%sstd::map<std::string,%s>%s out"
            .format(param.valueDefine.length > 0 ? ", " : "", mapTypeString, modifiers)
}

function returnGenerateUnion (param) {
    param.valueOut = `std::any out;
            std::string out_type;`
    param.valueDefine += "%sstd::any &out".format(param.valueDefine.length > 0 ? ", " : "")
}

function returnGenerateObject(returnInfo, param, data) {
    param.valueOut = `std::map<std::string, std::any> out;`            
    param.valueDefine += "%sstd::map<std::string, std::any> &out".format(param.valueDefine.length > 0 ? ", " : "")
   
}

/**
 * 获取方法返回参数的填充代码
 * @param returnInfo 方法的返回参数信息
 * @param param 方法的所有参数信息
 * @returns 返回参数的填充代码123 返回测试的值
 */
function getReturnFill(returnInfo, param) {
    let type = returnInfo.type
    let valueFillStr = ""
    if (param.callback) { // callback方法的返回参数处理
        if (param.callback.isAsync) {
            // 异步callback方法返回的是一个结构体，包含errcode和data两部分， 详见basic.d.ts中AsyncCallback的定义
            valueFillStr = "vio->outErrCode"
            param.valueDefine += "%suint32_t& outErrCode".format(param.valueDefine.length > 0 ? ", " : "")
        }

        if (type != "void") {
            // callback<xxx> 中的xxx不是void时，生成的capp代码才需要用户填充out参数
            valueFillStr += "%svio->out".format(valueFillStr.length > 0 ? ", " : "")
        }
    } else {  // 普通方法的返回参数处理
        valueFillStr = "vio->out"
    }
    return valueFillStr
}

function isObjectType(type) {
    if(type == "Object" || type == "object") {
        return true;
    }
    return false;
}

function returnGenerate(returnInfo, param, data) {
    let type = returnInfo.type
    let valueFillStr = getReturnFill(returnInfo, param)
    param.valueFill += ("%s" + valueFillStr).format(param.valueFill.length > 0 ? ", " : "")
    let outParam = returnInfo.optional ? "(*vio->out)" : "vio->out"
    let modifiers = returnInfo.optional ? "*" : "&"
    if (returnInfo.optional) {
        param.optionalParamDestory += "C_DELETE(vio->out);\n    "
    }

    if (!isEnum(type, data)) {
        param.valuePackage = cToJs(outParam, type, "result")
    } else if (type.indexOf("|") >= 0) {
        returnGenerateUnion(param)
    }

    if (type == "string") {
        param.valueOut = returnInfo.optional ? "std::string* out = nullptr;" : "std::string out;"
        param.valueDefine += "%sstd::string%s out".format(param.valueDefine.length > 0 ? ", " : "", modifiers)
    }
    else if (type == "void") {
        NapiLog.logInfo("The current void type don't need generate");
    }
    else if (type == "boolean") {
        param.valueOut = returnInfo.optional ? "bool* out = nullptr;" : "bool out;"
        param.valueDefine += "%sbool%s out".format(param.valueDefine.length > 0 ? ", " : "", modifiers)
    }
    else if (isEnum(type, data)) {
        returnGenerateEnum(data, returnInfo, param)
    }
    else if(generateType(type)){
        returnGenerate2(returnInfo, param, data)
    }
    else if (type.substring(0, 12) == "NUMBER_TYPE_") {
        param.valueOut = type + (returnInfo.optional ? "* out = nullptr;" : " out;")
        param.valueDefine += "%s%s%s out".format(param.valueDefine.length > 0 ? ", " : "", type, modifiers)
    }
    else if (isObjectType(type)) {
        returnGenerateObject(returnInfo, param, data)
    }
    else {
        NapiLog.logError("Do not support returning the type [%s].".format(type));
    }
}

function generateType(type){
    if (InterfaceList.getValue(type)) {
        return true
    }
    else if (type.substring(0, 6) == "Array<") {
        return true
    }
    else if (type.substring(type.length - 2) == "[]") {
        return true
    }
    else if (type.substring(0, 4) == "Map<" || type.indexOf("{[key:") == 0) {
        return true
    }
    else if (type == "any" || type == "Object" || type == "object") {
        return true
    }
    else {
        return false
    }
}
function isMapType(type) {
    if(type.substring(0, 4) == "Map<" || type.indexOf("{[key:") == 0) {
        return true;
    }
    return false;
}

function returnGenerate2(returnInfo, param, data){
    let type = returnInfo.type
    let modifiers = returnInfo.optional ? "*" : "&"

    if (InterfaceList.getValue(type)) {
        param.valueOut = type + (returnInfo.optional ? "* out = nullptr;" : " out;")
        param.valueDefine += "%s%s%s out".format(param.valueDefine.length > 0 ? ", " : "", type, modifiers)
    }
    else if (type.substring(0, 6) == "Array<") {
        let arrayType = getArrayType(type)
        arrayType = jsType2CType(arrayType)
        if (arrayType == "any") {
            param.valueOut = `std::any out;
            std::string out_type;`
            param.valueDefine += "%sstd::any &out".format(param.valueDefine.length > 0 ? ", " : "")
        } else {
            param.valueOut = returnInfo.optional ? "std::vector<%s>* out = nullptr;".format(arrayType)
                                             : "std::vector<%s> out;".format(arrayType)
            param.valueDefine += "%sstd::vector<%s>%s out".format(
            param.valueDefine.length > 0 ? ", ": "", arrayType, modifiers)
        }
    }
    else if (type.substring(type.length - 2) == "[]") {
        let arrayType = getArrayTypeTwo(type)
        arrayType = jsType2CType(arrayType)
        if (arrayType == "any") {
            param.valueOut = `std::any out;
            std::string out_type;`
            param.valueDefine += "%sstd::any &out".format(param.valueDefine.length > 0 ? ", " : "")
        } else {
            param.valueOut = returnInfo.optional ? "std::vector<%s>* out = nullptr;".format(arrayType)
                                             : "std::vector<%s> out;".format(arrayType)
            param.valueDefine += "%sstd::vector<%s>%s out".format(
            param.valueDefine.length > 0 ? ", " : "", arrayType, modifiers)
        }
    }
    else if (isMapType(type)) {
        returnGenerateMap(returnInfo, param)
    }
    else if (type == "any") {
        param.valueOut = `std::any out;
            std::string out_type;`
        param.valueDefine += "%sstd::any &out".format(param.valueDefine.length > 0 ? ", " : "")
    }
    else if (isObjectType(type)) {
        param.valueOut = `std::map<std::string, std::any> out;`
        param.valueDefine += "%sstd::map<std::string, std::any> &out".format(param.valueDefine.length > 0 ? ", " : "")
    }
}

function returnGenerateEnum(data, returnInfo, param) {
    let type = returnInfo.type
    let index = enumIndex(type, data)
    let modifiers = returnInfo.optional ? "*" : "&"
    if (data.enum[index].body.enumValueType == EnumValueType.ENUM_VALUE_TYPE_NUMBER) {
        type = "NUMBER_TYPE_" + NumberIncrease.getAndIncrease()
    } else if (data.enum[index].body.enumValueType == EnumValueType.ENUM_VALUE_TYPE_STRING) {
        type = "string"
    } else {
        NapiLog.logError(`function returnGenerateEnum:this type is not support %s`.format(type));
        return
    }
    param.valuePackage = cToJs("vio->out", type, "result")
    if (type == "string") {
        param.valueOut = returnInfo.optional ? "std::string* out = nullptr;" : "std::string out;"
        param.valueDefine += "%sstd::string%s out".format(param.valueDefine.length > 0 ? ", " : "", modifiers)
    }
    else if (type.substring(0, 12) == "NUMBER_TYPE_") {
        param.valueOut = type + " out;"
        param.valueDefine += "%s%s%s out".format(param.valueDefine.length > 0 ? ", " : "", type, modifiers)
    }
}

module.exports = {
    cToJs,
    cToJsForInterface,
    returnGenerate,
    returnGenerateEnum
}
