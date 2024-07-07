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
const { InterfaceList, getArrayType, getArrayTypeTwo, NumberIncrease,
    enumIndex, isEnum, EnumValueType, getMapType,
    EnumList, getUnionType } = require("../tools/common");
const re = require("../tools/re");
const { NapiLog } = require("../tools/NapiLog");
const { print } = require("../tools/tool");

class LenIncrease { }
LenIncrease.LEN_TO = 1;
LenIncrease.Reset = function () {
    LenIncrease.LEN_TO = 1;
}
LenIncrease.getAndIncrease = function () {
    return LenIncrease.LEN_TO++;
}

function getValueProperty(napiVn, name) {
    return 'pxt->GetValueProperty(%s, "%s")'.format(napiVn, name)
}

function jsToC(dest, napiVn, type, enumType = 0) {
    if (type.indexOf("|") >= 0) {
        return unionTempleteFunc(dest, napiVn, type)
    } else if (type == "string") {
        if (napiVn.indexOf("GetValueProperty") >= 0) {
            let lt = LenIncrease.getAndIncrease()
            return `napi_value tnv%d = %s;\n    if(tnv%d!=nullptr){pxt->SwapJs2CUtf8(tnv%d,%s);}`
                .format(lt, napiVn, lt, lt, dest)
        } else {
            return "pxt->SwapJs2CUtf8(%s, %s);".format(napiVn, dest)
        }           
    } else if (type.substring(type.length - 2) == "[]") {
        return arrTemplete(dest, napiVn, type);
    } else if (type.substring(0, 12) == "NUMBER_TYPE_") {
        return numTempleteFunc (enumType, napiVn, type, dest);
    } else if (InterfaceList.getValue(type)) {
        let tt = ""
        let ifl = InterfaceList.getValue(type)
        for (let i in ifl) {
            let name2 = ifl[i].name
            let type2 = ifl[i].type
            tt += jsToC("%s.%s".format(dest, name2), getValueProperty(napiVn, name2), type2)
        }
        return tt
    } else if (EnumList.getValue(type)) {
        return jsToCEnum(type, dest, napiVn)
    } else if (type.indexOf("Array<") == 0) {
        return arrTemplete(dest, napiVn, type);
    } else if (type == "boolean") {
        return `BOOLEAN_JS_2_C(%s,%s,%s);`.format(napiVn, "bool", dest)
    } else if (type.substring(0, 4) == "Map<" || type.substring(0, 6) == "{[key:") {
        return mapTempleteFunc(dest, napiVn, type);
    } else if (type == "any") {
        return anyTempleteFunc(dest, napiVn, type);
    } else if (type == "Object" || type == "object") {
        return objectTempleteFunc(dest, napiVn);
    }else {
        NapiLog.logError(`do not support to generate jsToC %s,%s,%s`.format(dest, napiVn, type));
    }        
}

function unionTempleteFunc(dest, napiVn, type) {
    let unionType = getUnionType(type)
    let unionTypeString = ''
    unionTypeString += '%s_type = pxt->GetUnionType(%s);\n'.format(dest, napiVn)
    for (let i = 0; i < unionType.length; i++) {
        if (unionType[i] == "string") {
            unionTypeString += `if (%s_type == "string"){
                std::string union_string;
                %s
                %s
            }\n`.format(dest, jsToC("union_string", napiVn, unionType[i]), dest+" = union_string;")
        } else if (unionType[i].substring(0, 12) == "NUMBER_TYPE_") {
            unionTypeString += `if (%s_type == "number"){
                std::uint32_t union_number;
                %s
                %s
            }\n`.format(dest, jsToC("union_number", napiVn, unionType[i]), dest+" = union_number;")
        } else if (unionType[i] == "boolean") {
            unionTypeString += `if (%s_type == "boolean"){
                bool union_boolean;
                %s
                %s
            }\n`.format(dest, jsToC("union_boolean", napiVn, unionType[i]), dest+" = union_boolean;")
        }
    }
    return unionTypeString
}

function jsToCEnum(type, dest, napiVn) {
    let tt = ""
    let ifl = EnumList.getValue(type)
    for (let i in ifl) {
        let type2 = ifl[i].type
        tt += jsToC("%s".format(dest), getValueProperty(napiVn, dest), type2, type)
    }
    return tt
}

function getArrayTypeTemplete(type) {
    let arrayType
    if (type.substring(type.length - 2) == "[]") {
        arrayType = getArrayTypeTwo(type)
    } else {
        arrayType = getArrayType(type)
    }    
    if (arrayType == "string") {
        arrayType = "std::string"
    } else if (arrayType == "boolean") {
        arrayType = "bool"
    } else if (arrayType == "any") {
        arrayType = "any"
    } else if (arrayType == "[key:string]:string" || arrayType == "Map<string,string>") {
        arrayType = "std::map<std::string, std::string>"
    } else if (arrayType.substring(0, arrayType.length-1) == "[key:string]:NUMBER_TYPE_" || 
        arrayType.substring(0, arrayType.length-1) == "Map<string,NUMBER_TYPE_>") {       
        let len = arrayType.length
        let num = arrayType.substring(len-1, len)
        arrayType = "std::map<std::string, NUMBER_TYPE_%s>".format(num)
    } else if (arrayType == "[key:string]:boolean" || arrayType == "Map<string,boolean>") {
        arrayType = "std::map<std::string, bool>"
    } else if (arrayType.substring(0, 14) == "[key:string]:") {
        let valueType = arrayType.substring(14, arrayType.length)
        arrayType = "std::map<std::string, %s>".format(valueType)
    } else if (arrayType.substring(0, 11) == "Map<string,") {
        let valueType = arrayType.substring(11, arrayType.length-1)
        arrayType = "std::map<std::string, %s>".format(valueType)
    }
    return arrayType
}

function arrTemplete(dest, napiVn, type) {  
    let lt = LenIncrease.getAndIncrease()
    let arrayType = getArrayTypeTemplete(type)
    if (arrayType == "any") {
        return anyArrayTempleteFunc(dest, napiVn);
    }
    let arrTemplete = `\
    uint32_t len[replace_lt]=pxt->GetArrayLength(%s);
    for(uint32_t i[replace_lt]=0;i[replace_lt]<len[replace_lt];i[replace_lt]++) {
        %s tt[replace_lt];
        [replace_swap]
        %s.push_back(tt[replace_lt]);

    }\n`.format(napiVn, arrayType == "boolean" ? "bool" : arrayType, dest)
   
    let arrMapTemplete = `\
    uint32_t len[replace_lt]=pxt->GetArrayLength(%s);
    for(uint32_t i[replace_lt]=0;i[replace_lt]<len[replace_lt];i[replace_lt]++) {
        %s tt[replace_lt];        
        
        napi_value mapPara = pxt->GetArrayElement(pxt->GetArgv(0),i[replace_lt]);
        uint32_t len2=pxt->GetMapLength(mapPara);
        for(uint32_t i2=0;i2<len2;i2++) {           
            std::string ttName;          
            pxt->SwapJs2CUtf8(pxt->GetMapElementName(mapPara, i2), ttName);
            napi_value mapValue = pxt->GetMapElementValue(mapPara,ttName.c_str());
            [code_gen]
            
            tt[replace_lt].insert(std::make_pair(ttName, ttValue));             
        }
        %s.push_back(tt[replace_lt]);

    }\n`.format(napiVn, arrayType, dest)
    arrTemplete = arrTemplete.replaceAll("[replace_lt]", lt)    

    let str = "std::map<std::string,"
    let strLen = str.length    
    if (arrayType.substring(0, strLen) == "std::map<std::string,") {  
        let codegen = getMapValueCode(arrayType)
        if (codegen == null) {
            return arrMapTemplete
        }     
        arrMapTemplete = arrMapTemplete.replaceAll("[replace_lt]", lt)
        arrMapTemplete = arrMapTemplete.replaceAll("[code_gen]", codegen)
        return arrMapTemplete
    } else {
        arrTemplete = getArrTempletereplaceSwap(arrTemplete, arrayType, napiVn, lt)
    }
    return arrTemplete
}

function numTempleteFunc(enumType, napiVn, type, dest) {
    if (enumType) {
        if (napiVn.indexOf("GetValueProperty") >= 0) {
            let lt = LenIncrease.getAndIncrease()
            return `napi_value tnv%d = %s;\n    if(tnv%d!=nullptr){NUMBER_JS_2_C_ENUM(tnv%d,%s,%s,%s);}`
            .format(lt, napiVn, lt, lt, type, dest, enumType)
        } else {
            return `NUMBER_JS_2_C_ENUM(%s,%s,%s,%s);`.format(napiVn, type, dest, enumType)
        } 
    } else {
        if (napiVn.indexOf("GetValueProperty") >= 0) {
            let lt = LenIncrease.getAndIncrease()
            return `napi_value tnv%d = %s;\n    if(tnv%d!=nullptr){NUMBER_JS_2_C(tnv%d,%s,%s);}`
            .format(lt, napiVn, lt, lt, type, dest)
        } else {
            return `NUMBER_JS_2_C(%s,%s,%s);`.format(napiVn, type, dest)
        } 
   }
}

function getMapValueCode(arrayType) {
    let valueTypeOut = arrayType.substring(22, arrayType.length-1)
    let strTypeOut = "%s".format(valueTypeOut)
    let codegen
    if (strTypeOut == "std::string") {                 
        codegen = '\
        std::string ttValue;\
        pxt->SwapJs2CUtf8(mapValue, ttValue);'
    } else if (strTypeOut == "bool") {               
        codegen = '\
        bool ttValue;\
        ttValue = pxt->SwapJs2CBool(mapValue);'            
    } else if (strTypeOut.substr(0, 12) == "NUMBER_TYPE_") {            
        codegen = '\
        %s ttValue;\
        NUMBER_JS_2_C(mapValue,%s,ttValue);'.format(strTypeOut, strTypeOut)
    }
    return codegen
}

function getArrTempletereplaceSwap(arrTemplete, arrayType, napiVn, lt) { 
    if (arrayType.substring(0, 12) == "NUMBER_TYPE_") {
        arrTemplete = arrTemplete.replaceAll("[replace_swap]",
            "NUMBER_JS_2_C(pxt->GetArrayElement(%s,i%d),%s,tt%d);".format(napiVn, lt, arrayType, lt))
    } else if (arrayType == "std::string") {
        arrTemplete = arrTemplete.replaceAll("[replace_swap]",
            "pxt->SwapJs2CUtf8(pxt->GetArrayElement(%s,i%d), tt%d);".format(napiVn, lt, lt))
    } else if (InterfaceList.getValue(arrayType)) {
        arrTemplete = arrTemplete.replaceAll("[replace_swap]",
            jsToC("tt" + lt, "pxt->GetArrayElement(%s,i%d)".format(napiVn, lt), arrayType))
    } else if (arrayType == "bool") {
        arrTemplete = arrTemplete.replaceAll("[replace_swap]",
            "tt%d = pxt->SwapJs2CBool(pxt->GetArrayElement(%s,i%d));".format(lt, napiVn, lt))
    }
    return arrTemplete
}
function getMapValueType(strLen, keyType, arrayType){
    let valueTypeIn
    if (keyType == "[key:string]:"){
        valueTypeIn = arrayType.substring(strLen, arrayType.length)
    } else if (keyType == "Map<string,"){
        valueTypeIn = arrayType.substring(strLen, arrayType.length - 1)
    }            
    
    let mapValueType
    if (valueTypeIn == "string") {
        mapValueType = "std::string"
    } else if (valueTypeIn == "boolean") {
        mapValueType = "bool"
    } else {
        mapValueType = valueTypeIn
    }
    return mapValueType
}

function getMapKeyLen(arrayType) {
    let strLen
    if (arrayType.substring(0,13) == "[key:string]:") {
        strLen = "[key:string]:".length
    }else if (arrayType.substring(0,10) == "Map<string") {
        strLen = "Map<string,".length
    }
    return strLen
}

function paramGenerateArray(p, funcValue, param) {
    let type = funcValue.type
    let name = funcValue.name
    let inParamName = funcValue.optional ? "(*vio->in" + p + ")" : "vio->in" + p
    let modifiers = funcValue.optional ? "* " : "&"
    if (type.substring(type.length - 2) == "[]") {
        let arrayType = getArrayTypeTwo(type)
        if (arrayType == "string") arrayType = "std::string"
        if (arrayType == "boolean") arrayType = "bool"
        if (arrayType == "any") {
            return paramGenerateAnyArray(p, name, type, param)
        }
        param.valueIn += funcValue.optional ? "\n    std::vector<%s>* in%d = nullptr;".format(arrayType, p) 
                                            : "\n    std::vector<%s> in%d;".format(arrayType, p)
        param.valueCheckout += jsToC(inParamName, "pxt->GetArgv(%d)".format(p), type)
        param.valueFill += "%svio->in%d".format(param.valueFill.length > 0 ? ", " : "", p)
        param.valueDefine += "%sstd::vector<%s> %s%s".format(param.valueDefine.length > 0 ? ", " 
                            : "", arrayType, modifiers, name)
    } else if (type.substring(0, 6) == "Array<") {        
        let arrayType = getArrayType(type)
        let strLen =  getMapKeyLen(arrayType)
        let keyType = arrayType.substring(0, strLen)
        let suType = arrayType.substring(0,12)
        if (arrayType == "string") {
            arrayType = "std::string"
        } else if (arrayType == "boolean") {
            arrayType = "bool"
        } else if (arrayType == "any") {
            return paramGenerateAnyArray(p, name, type, param)
        }
        else if (keyType == "[key:string]:"|| keyType == "Map<string,") {
            let mapValueType = getMapValueType(strLen, keyType, arrayType);             
            arrayType = "std::map<std::string, %s>".format(mapValueType)
        }
        param.valueIn += funcValue.optional ? "\n    std::vector<%s>* in%d = nullptr;".format(arrayType, p) 
                                            : "\n    std::vector<%s> in%d;".format(arrayType, p)
        let arrValueCheckout = jsToC(inParamName, "pxt->GetArgv(%d)".format(p), type)
        if (funcValue.optional) {
            arrValueCheckout = "if (pxt->GetArgc() > %s) {\n        vio->in%d = new std::vector<%s>;\n"
                .format(p, p, arrayType) + arrValueCheckout + "    }\n"
            param.optionalParamDestory += "C_DELETE(vio->in%d);\n    ".format(p)
        }                                   
        param.valueCheckout += arrValueCheckout
        param.valueFill += "%svio->in%d".format(param.valueFill.length > 0 ? ", " : "", p)
        param.valueDefine += "%sstd::vector<%s> %s%s".format(param.valueDefine.length > 0 ? ", "
            : "", arrayType, modifiers, name)
    } else {
        NapiLog.logError("The current version do not support to this param to generate :", name, "type :", type);
    }
}

function paramGenerateAny(p, name, type, param) {
    param.valueIn += `\n    std::any in%d; 
        std::string in%d_type;`.format(p, p)
    param.valueCheckout += jsToC("vio->in" + p, "pxt->GetArgv(%d)".format(p), type)
    param.valueFill += "%svio->in%d".format(param.valueFill.length > 0 ? ", " : "", p)
    param.valueDefine += "%sstd::any &%s".format(param.valueDefine.length > 0 ? ", " : "", name)
}

function paramGenerateAnyArray(p, name, type, param) {
    param.valueIn += `\n    std::any in%d; 
        std::string in%d_type;`.format(p, p)
    param.valueCheckout += jsToC("vio->in" + p, "pxt->GetArgv(%d)".format(p), type)
    param.valueFill += "%svio->in%d".format(param.valueFill.length > 0 ? ", " : "", p)
    param.valueDefine += "%sstd::any &%s".format(param.valueDefine.length > 0 ? ", " : "", name)
}

function paramGenerateEnum(data, funcValue, param, p) {
    let index = enumIndex(funcValue.type, data)
    if (data.enum[index].body.enumValueType == EnumValueType.ENUM_VALUE_TYPE_NUMBER) {
        funcValue.type = "NUMBER_TYPE_" + NumberIncrease.getAndIncrease()
    } else if (data.enum[index].body.enumValueType == EnumValueType.ENUM_VALUE_TYPE_STRING) {
        funcValue.type = "string"
    } else {
        NapiLog.logError(`paramGenerate is not support`);
        return
    }
    paramGenerate(p, funcValue, param, data)
}

function paramGenerateMap(funcValue, param, p) {
    let type = funcValue.type
    let name = funcValue.name
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
    paramGenerateMap2(funcValue, param, p, mapType, mapTypeString, name)
}

function paramGenerateMap2(funcValue, param, p, mapType, mapTypeString, name) {
    let inParamName = funcValue.optional ? "(*vio->in" + p + ")" : "vio->in" + p
    let modifiers = funcValue.optional ? "*" : "&"
    if (mapType[1] == "any") {
        param.valueIn += funcValue.optional ? `\n    std::map<std::string,%s>* in%d = nullptr;
                                                std::string in%d_type;`.format(mapTypeString, p, p)
                                            : `\n    std::map<std::string,%s> in%d;
                                                std::string in%d_type;`.format(mapTypeString, p, p)
    } else {
        param.valueIn += funcValue.optional ? "\n    std::map<std::string,%s>* in%d = nullptr;".format(mapTypeString, p)
                                        : "\n    std::map<std::string,%s> in%d;".format(mapTypeString, p)
    }
    param.valueCheckout += getValueCheckout(funcValue, param, inParamName, p,
        "std::map<std::string,%s>".format(mapTypeString))
    param.valueFill += "%svio->in%d".format(param.valueFill.length > 0 ? ", " : "", p)
    param.valueDefine += "%sstd::map<std::string,%s>%s %s"
            .format(param.valueDefine.length > 0 ? ", " : "", mapTypeString, modifiers, name)
}

function mapTempleteFunc(dest, napiVn, type) {
    let mapType = getMapType(type)
    let lt = LenIncrease.getAndIncrease()
    let mapTemplete = ""
    if (mapType[1] != undefined && mapType[2] == undefined) {
        mapTemplete = mapValue(mapType, napiVn, dest, lt)
    }
    else if (mapType[2] != undefined) {
        mapTemplete = mapMap(mapType, napiVn, dest, lt)
    }
    else if (mapType[3] != undefined) {
        mapTemplete = mapArray(mapType, napiVn, dest, lt)
    }
    return mapTemplete
}

function anyTempleteFunc(dest) {
    let anyTemplete = `%s_type = pxt->GetAnyType(pxt->GetArgv(0));
    pxt->SetAnyValue(%s_type, pxt->GetArgv(0), %s);`
    .format(dest, dest, dest)
    
    return anyTemplete
}

function anyArrayTempleteFunc(dest, napiVn) {
    let anyArrayTemplete = `%s_type = pxt->GetAnyArrayType(%s);
    pxt->SetAnyValue(%s_type, %s, %s);`
    .format(dest, napiVn, dest, napiVn, dest)
    
    return anyArrayTemplete
}

let mapValueTemplete = `\
uint32_t len[replace_lt]=pxt->GetMapLength(%s);
for(uint32_t i[replace_lt]=0;i[replace_lt]<len[replace_lt];i[replace_lt]++) {
    std::string tt[replace_lt];
    %s tt[replace_lt+1];
    [replace_swap]
    %s.insert(std::make_pair(tt[replace_lt], tt[replace_lt+1]));
}`

function mapInterface(mapTypeString, mapTemplete, napiVn, lt) {
    let interfaceValue = InterfaceList.getValue(mapTypeString)
    let interfaceVarName = ""
    let interfaceFun = ""
    for (let i = 0; i < interfaceValue.length; i++) {
        if (interfaceValue[i].type == 'string') {
            interfaceVarName += `std::string %dName = "%d";\n`.format(interfaceValue[i].name, interfaceValue[i].name)
            interfaceFun +=
                `pxt->%s(pxt->%s(pxt->GetMapElementValue(pxt->GetArgv(0),tt%d.c_str()),%sName.c_str()),tt%d.%s);`
                    .format("SwapJs2CUtf8", "GetMapElementValue",
                        lt, interfaceValue[i].name, lt+1, interfaceValue[i].name)
        }
        else if (interfaceValue[i].type.substring(0, 12) == "NUMBER_TYPE_") {
            interfaceVarName += `std::string %dName = "%d";\n`.format(interfaceValue[i].name, interfaceValue[i].name)
            interfaceFun +=
                `%s(pxt->%s(pxt->GetMapElementValue(pxt->GetArgv(0),tt%d.c_str()),%sName.c_str()),%s,tt%d.%s);\n`
                    .format("NUMBER_JS_2_C", "GetMapElementValue", lt, interfaceValue[i].name,
                        interfaceValue[i].type, lt + 1, interfaceValue[i].name)
        }
        else if (interfaceValue[i].type == 'boolean') {
            interfaceVarName += `std::string %dName = "%d";\n`.format(interfaceValue[i].name, interfaceValue[i].name)
            interfaceFun +=
                `tt%d.%s = pxt->%s(pxt->%s(pxt->GetMapElementValue(pxt->GetArgv(0),tt%d.c_str()),%sName.c_str()));\n`
                    .format(lt + 1, interfaceValue[i].name, "SwapJs2CBool", "GetMapElementValue",
                        lt, interfaceValue[i].name)
        }
    }
    mapTemplete = mapTemplete.replaceAll("[replace_swap]",
        `pxt->SwapJs2CUtf8(pxt->GetMapElementName(%s,i%d), tt%d);
        %d
        %d`.format(napiVn, lt, lt, interfaceVarName, interfaceFun))
    return mapTemplete
}

function mapValue(mapType, napiVn, dest, lt) {
    let mapTypeString
    if (mapType[1] == "string") { mapTypeString = "std::string" }
    else if (mapType[1].substring(0, 12) == "NUMBER_TYPE_") { mapTypeString = mapType[1] }
    else if (mapType[1] == "boolean") { mapTypeString = "bool" }
    else if (mapType[1] == "any") { mapTypeString = "std::any" }
    else if (mapType[1] != null) { mapTypeString = mapType[1] }
    let mapTemplete = mapValueTemplete.format(napiVn, mapTypeString, dest)
    mapTemplete = mapTemplete.replaceAll("[replace_lt]", lt)
    mapTemplete = mapTemplete.replaceAll("[replace_lt+1]", lt + 1)
    if (mapTypeString == "std::string") {
        mapTemplete = mapTemplete.replaceAll("[replace_swap]",
            `pxt->SwapJs2CUtf8(pxt->GetMapElementName(%s,i%d), tt%d);
        pxt->SwapJs2CUtf8(pxt->GetMapElementValue(%s,tt%d.c_str()), tt%d);`
                .format(napiVn, lt, lt, napiVn, lt, lt + 1))
    }
    else if (mapTypeString.substring(0, 12) == "NUMBER_TYPE_") {
        mapTemplete = mapTemplete.replaceAll("[replace_swap]",
            `pxt->SwapJs2CUtf8(pxt->GetMapElementName(%s,i%d), tt%d);
        NUMBER_JS_2_C(pxt->GetMapElementValue(%s,tt%d.c_str()),%s,tt%d);`
                .format(napiVn, lt, lt, napiVn, lt, mapTypeString, lt + 1))
    }
    else if (mapTypeString == "bool") {
        mapTemplete = mapTemplete.replaceAll("[replace_swap]",
            `pxt->SwapJs2CUtf8(pxt->GetMapElementName(%s,i%d), tt%d);
        tt%d = pxt->SwapJs2CBool(pxt->GetMapElementValue(%s,tt%d.c_str()));`
                .format(napiVn, lt, lt, lt + 1, napiVn, lt))
    }
    if (mapTypeString == "std::any") {
        mapTemplete = mapTemplete.replaceAll("[replace_swap]",
            `pxt->SwapJs2CUtf8(pxt->GetMapElementName(%s,i%d), tt%d);
            if (i%d == 0){
                %s_type = pxt->GetAnyType(pxt->GetMapElementValue(%s,tt%d.c_str()));
            }
            pxt->SetAnyValue(%s_type, pxt->GetMapElementValue(%s,tt%d.c_str()), tt%d);`
                .format(napiVn, lt, lt, lt, dest, napiVn, lt, dest, napiVn, lt, lt + 1))
    }
    else if (InterfaceList.getValue(mapTypeString)) {
        mapTemplete = mapInterface(mapTypeString, mapTemplete, napiVn, lt)
    }
    return mapTemplete
}

let mapMapTemplete = `\
uint32_t len[replace_lt]=pxt->GetMapLength(%s);
for(uint32_t i[replace_lt]=0;i[replace_lt]<len[replace_lt];i[replace_lt]++) {
    std::string tt[replace_lt];
    std::map<std::string,%s> tt[replace_lt+1];
    [replace_swap]
    %s.insert(std::make_pair(tt[replace_lt], tt[replace_lt+1]));
}`

function mapMapString (mapTemplete, napiVn, lt) {
    return mapTemplete.replaceAll("[replace_swap]",
    `pxt->SwapJs2CUtf8(pxt->GetMapElementName(%s,i%d), tt%d);
    uint32_t len%d=pxt->GetMapLength(pxt->GetMapElementValue(%s,tt%d.c_str()));
    for(uint32_t i%d=0;i%d<len%d;i%d++){
        std::string tt%d;
        std::string tt%d;
        pxt->SwapJs2CUtf8(pxt->GetMapElementName(pxt->GetMapElementValue(%s,tt%d.c_str()), i%d),tt%d);
        pxt->SwapJs2CUtf8(pxt->GetMapElementValue(pxt->GetMapElementValue(%s,tt%d.c_str()),tt%d.c_str()),tt%d);
        tt%d.insert(std::make_pair(tt%d, tt%d));
    }`.format(napiVn, lt, lt, lt + 1, napiVn, lt, lt + 1, lt + 1, lt + 1, lt + 1,
        lt + 2, lt + 3, napiVn, lt, lt + 1, lt + 2, napiVn, lt, lt + 2, lt + 3, lt + 1, lt + 2, lt + 3))
}

function mapMapBoolean (mapTemplete, napiVn, lt) {
    return mapTemplete.replaceAll("[replace_swap]",
    `pxt->SwapJs2CUtf8(pxt->GetMapElementName(%s,i%d), tt%d);
    uint32_t len%d=pxt->GetMapLength(pxt->GetMapElementValue(%s,tt%d.c_str()));
    for(uint32_t i%d=0;i%d<len%d;i%d++){
        std::string tt%d;
        bool tt%d;
        pxt->SwapJs2CUtf8(pxt->GetMapElementName(pxt->GetMapElementValue(%s,tt%d.c_str()), i%d),tt%d);
        tt%d = pxt->%s(pxt->GetMapElementValue(pxt->GetMapElementValue(%s,tt%d.c_str()),tt%d.c_str()));
        tt%d.insert(std::make_pair(tt%d, tt%d));
    }`.format(napiVn, lt, lt, lt + 1, napiVn, lt, lt + 1, lt + 1, lt + 1, lt + 1,
        lt + 2, lt + 3, napiVn, lt, lt + 1, lt + 2, lt + 3, "SwapJs2CBool"
        ,napiVn, lt, lt + 2, lt + 1, lt + 2, lt + 3))
}

function mapMapNumber (mapTemplete, napiVn, lt, mapTypeString) {
    return mapTemplete.replaceAll("[replace_swap]",
    `pxt->SwapJs2CUtf8(pxt->GetMapElementName(%s,i%d), tt%d);
    uint32_t len%d=pxt->GetMapLength(pxt->GetMapElementValue(%s,tt%d.c_str()));
    for(uint32_t i%d=0;i%d<len%d;i%d++){
        std::string tt%d;
        %s tt%d;
        pxt->SwapJs2CUtf8(pxt->GetMapElementName(pxt->GetMapElementValue(%s,tt%d.c_str()), i%d),tt%d);
        NUMBER_JS_2_C(pxt->GetMapElementValue(pxt->GetMapElementValue(%s,tt%d.c_str()),tt%d.c_str()),%s,tt%d);
        tt%d.insert(std::make_pair(tt%d, tt%d));
    }`.format(napiVn, lt, lt, lt + 1, napiVn, lt, lt + 1, lt + 1, lt + 1, lt + 1,
        lt + 2, mapTypeString, lt + 3, napiVn, lt, lt + 1, lt + 2, napiVn, lt, lt + 2,
        mapTypeString, lt + 3, lt + 1, lt + 2, lt + 3))
}

function mapMap(mapType, napiVn, dest, lt) {
    let mapTypeString
    if (mapType[2] == "string") { mapTypeString = "std::string" }
    else if (mapType[2].substring(0, 12) == "NUMBER_TYPE_") { mapTypeString = mapType[2] }
    else if (mapType[2] == "boolean") { mapTypeString = "bool" }
    let mapTemplete = mapMapTemplete.format(napiVn, mapTypeString, dest)
    mapTemplete = mapTemplete.replaceAll("[replace_lt]", lt)
    mapTemplete = mapTemplete.replaceAll("[replace_lt+1]", lt + 1)
    if (mapType[2] == "string") {
        mapTemplete = mapMapString (mapTemplete, napiVn, lt)
    }
    else if (mapType[2] == "boolean") {
        mapTemplete = mapMapBoolean (mapTemplete, napiVn, lt)
    }
    else if (mapType[2].substring(0, 12) == "NUMBER_TYPE_") {
        mapTemplete = mapMapNumber (mapTemplete, napiVn, lt, mapTypeString)
    }
    return mapTemplete
}

let mapArrayTemplete = `\
uint32_t len[replace_lt]=pxt->GetMapLength(%s);
for(uint32_t i[replace_lt]=0;i[replace_lt]<len[replace_lt];i[replace_lt]++) {
    std::string tt[replace_lt];
    std::vector<%s> tt[replace_lt+1];
    [replace_swap]
    %s.insert(std::make_pair(tt[replace_lt], tt[replace_lt+1]));
}`

function mapArray(mapType, napiVn, dest, lt) {
    let mapTypeString
    if (mapType[3] == "string") { mapTypeString = "std::string" }
    else if (mapType[3].substring(0, 12) == "NUMBER_TYPE_") { mapTypeString = mapType[3] }
    else if (mapType[3] == "boolean") { mapTypeString = "bool" }
    let mapTemplete = mapArrayTemplete.format(napiVn, mapTypeString, dest)
    mapTemplete = mapTemplete.replaceAll("[replace_lt]", lt)
    mapTemplete = mapTemplete.replaceAll("[replace_lt+1]", lt + 1)
    if (mapType[3] == "string") {
        mapTemplete = mapTemplete.replaceAll("[replace_swap]",
            `pxt->SwapJs2CUtf8(pxt->GetMapElementName(%s,i%s), tt%d);
            uint32_t len%s=pxt->GetArrayLength(pxt->GetMapElementValue(%s,tt%d.c_str()));
            for(uint32_t i%d=0;i%d<len%d;i%d++){
                std::string tt%d;
                pxt->SwapJs2CUtf8(pxt->GetArrayElement(pxt->GetMapElementValue(%s,tt%d.c_str()),i%d), tt%d);
                tt%d.push_back(tt%d);
            }`.format(napiVn, lt, lt, lt + 1, napiVn, lt, lt + 1, lt + 1, lt + 1,
                lt + 1, lt + 2, napiVn, lt, lt + 1, lt + 2, lt + 1, lt + 2))
    }
    else if (mapType[3] == "boolean") {
        mapTemplete = mapTemplete.replaceAll("[replace_swap]",
            `pxt->SwapJs2CUtf8(pxt->GetMapElementName(%s,i%s), tt%d);
            uint32_t len%s=pxt->GetArrayLength(pxt->GetMapElementValue(%s,tt%d.c_str()));
            for(uint32_t i%d=0;i%d<len%d;i%d++){
                bool tt%d;
                tt%d = pxt->SwapJs2CBool(pxt->GetArrayElement(pxt->GetMapElementValue(%s,tt%d.c_str()),i%d));
                tt%d.push_back(tt%d);
            }`.format(napiVn, lt, lt, lt + 1, napiVn, lt, lt + 1, lt + 1, lt + 1,
                lt + 1, lt + 2, lt + 2, napiVn, lt, lt, lt + 1, lt + 2))
    }
    else if (mapType[3].substring(0, 12) == "NUMBER_TYPE_") {
        mapTemplete = mapTemplete.replaceAll("[replace_swap]",
            `pxt->SwapJs2CUtf8(pxt->GetMapElementName(%s,i%s), tt%d);
            uint32_t len%s=pxt->GetArrayLength(pxt->GetMapElementValue(%s,tt%d.c_str()));
            for(uint32_t i%d=0;i%d<len%d;i%d++){
                %s tt%d;
                NUMBER_JS_2_C(pxt->GetArrayElement(pxt->GetMapElementValue(%s,tt%d.c_str()),i%d), %s, tt%d);
                tt%d.push_back(tt%d);
            }`.format(napiVn, lt, lt, lt + 1, napiVn, lt, lt + 1, lt + 1, lt + 1,
                lt + 1, mapTypeString, lt + 2, napiVn, lt, lt + 1, mapTypeString, lt + 2, lt + 1, lt + 2))
    }
    return mapTemplete
}

function paramGenerateCallBack(data, funcValue, param, p) {
    let type = funcValue.type
    let arrayType = re.match("(Async)*Callback<(Array<([a-zA-Z_0-9]+)>)>", type)
    let regType
    if (arrayType) {
        regType = re.getReg(type, arrayType.regs[2])
    }

    let arrayType2 = re.match("(Async)*Callback<(([a-zA-Z_0-9]+)\\[\\])>", type)
    if (arrayType2) {
        regType = re.getReg(type, arrayType2.regs[2])
    }

    let tt = re.match("(Async)*Callback<([a-zA-Z_0-9]+)>", type)
    if (tt) {
        regType = re.getReg(type, tt.regs[2])
    }
    if (isEnum(regType, data)) {
        let index = enumIndex(regType, data)
        if (data.enum[index].body.enumValueType == EnumValueType.ENUM_VALUE_TYPE_NUMBER) {
            regType = "NUMBER_TYPE_" + NumberIncrease.getAndIncrease()
        } else if (data.enum[index].body.enumValueType == EnumValueType.ENUM_VALUE_TYPE_STRING) {
            regType = "string"
        } else {
            NapiLog.logError(`paramGenerate is not support`);
            return
        }
    }
    param.callback = {
        type: regType,
        offset: p,
        optional: funcValue.optional,
        isAsync: type.indexOf("AsyncCallback") >= 0
    }
}

function isArrayType(type) {    
    if (type.substring(type.length - 2) == "[]" || type.substring(0, 6) == "Array<") {
        return true;
    }
    return false;
}

function getValueCheckout(funcValue, param, inParamName, p, cType) {
    let valueCheckout = jsToC(inParamName, "pxt->GetArgv(%d)".format(p), funcValue.type) + "\n    "
    if (funcValue.optional) {
        valueCheckout = "if (pxt->GetArgc() > %d) {\n        vio->in%d = new %s;\n        ".format(p, p, cType)
            + valueCheckout + "}\n    "
        param.optionalParamDestory += "C_DELETE(vio->in%d);\n    ".format(p)
    }  
    return valueCheckout; 
}

function paramGenerateUnion(type, param, p, name) {
    param.valueIn += `\n    std::any in%d; 
        std::string in%d_type;`.format(p, p)
    param.valueCheckout += jsToC("vio->in" + p, "pxt->GetArgv(%d)".format(p), type)
    param.valueFill += "%svio->in%d".format(param.valueFill.length > 0 ? ", " : "", p)
    param.valueDefine += "%sstd::any &%s".format(param.valueDefine.length > 0 ? ", " : "", name)
}

function paramGenerateCommon(p, cType, funcValue, param, modifiers, inParamName) {
    param.valueIn += funcValue.optional ? "\n    %s* in%d = nullptr;".format(cType, p)
                                            : "\n    %s in%d;".format(cType, p)
    param.valueCheckout += getValueCheckout(funcValue, param, inParamName, p, cType)
    param.valueFill += "%svio->in%d".format(param.valueFill.length > 0 ? ", " : "", p)
    param.valueDefine += "%s%s%s %s".format(
        param.valueDefine.length > 0 ? ", " : "", cType, modifiers, funcValue.name)
}

let objectTemplete = `\
    uint32_t len[replace_lt]=pxt->GetMapLength(%s);
    for(uint32_t i[replace_lt]=0;i[replace_lt]<len[replace_lt];i[replace_lt]++) {
        std::string tt[replace_lt];
        std::any tt[replace_lt+1];

        pxt->SwapJs2CUtf8(pxt->GetMapElementName(%s,i[replace_lt]), tt[replace_lt]);
        napi_value valueObj = pxt->GetMapElementValue(%s,tt[replace_lt].c_str());
        std::string valueObjType = pxt->GetAnyType(valueObj);

        [replace_swap]
        %s.insert(std::make_pair(tt[replace_lt], tt[replace_lt+1]));
    }`

function objectTempleteFunc(dest, napiVn) {
    let lt = LenIncrease.getAndIncrease()
    let objTemplete = objectTemplete.format(napiVn, napiVn, napiVn, dest)

    objTemplete = objTemplete.replaceAll("[replace_swap]",
        `
        if (valueObjType == "string") {
            std::string tt[replace_lt+2];
            pxt->SwapJs2CUtf8(valueObj, tt[replace_lt+2]);
            tt[replace_lt+1] = tt[replace_lt+2];
        } else if (valueObjType == "boolean") {
            bool tt[replace_lt+2];
            tt[replace_lt+2] = pxt->SwapJs2CBool(valueObj);
            tt[replace_lt+1] = tt[replace_lt+2];
        } else if (valueObjType == "number") {
            NUMBER_JS_2_C(valueObj, NUMBER_TYPE_%d, tt[replace_lt+1]);
        }            
        `).format(lt)
    objTemplete = objTemplete.replaceAll("[replace_lt]", lt)
    objTemplete = objTemplete.replaceAll("[replace_lt+1]", lt + 1)
    objTemplete = objTemplete.replaceAll("[replace_lt+2]", lt + 2)
    return objTemplete
}

function paramGenerateObject(p, funcValue, param) {
    let type = funcValue.type
    let name = funcValue.name
    let inParamName = funcValue.optional ? "(*vio->in" + p + ")" : "vio->in" + p
    let modifiers = funcValue.optional ? "* " : "&"
       
        let arrayType = "std::map<std::string, std::any>"
        param.valueIn += funcValue.optional ? "\n    %s* in%d = nullptr;".format(arrayType, p) 
                                            : "\n    %s in%d;".format(arrayType, p)
        
        let arrValueCheckout = jsToC(inParamName, "pxt->GetArgv(%d)".format(p), type)                                 
        param.valueCheckout += arrValueCheckout
        param.valueFill += "%svio->in%d".format(param.valueFill.length > 0 ? ", " : "", p)
        param.valueDefine += "%s%s %s%s".format(param.valueDefine.length > 0 ? ", "
            : "", arrayType, modifiers, name)
}

// 函数的参数处理
function paramGenerate(p, funcValue, param, data) {
    let type = funcValue.type
    let name = funcValue.name
    let inParamName = funcValue.optional ? "(*vio->in" + p + ")" : "vio->in" + p
    let modifiers = funcValue.optional ? "*" : "&"
    if (type.indexOf("|") >= 0) {
        return paramGenerateUnion(type, param, p, name)
    }
    else if (type == "string") {
        paramGenerateCommon(p, "std::string", funcValue, param, modifiers, inParamName)
    }
    else if (type.substring(0, 12) == "NUMBER_TYPE_" && type.indexOf("[]") < 0) {
        paramGenerateCommon(p, funcValue.type, funcValue, param, modifiers, inParamName)
    }
    else if (InterfaceList.getValue(type)) {
        paramGenerateCommon(p, funcValue.type, funcValue, param, modifiers, inParamName)
    }
    else if (type.substring(0, 9) == "Callback<" || type.substring(0, 14) == "AsyncCallback<") {
        paramGenerateCallBack(data, funcValue, param, p)
    }
    else if (type == "boolean") {
        paramGenerateCommon(p, "bool", funcValue, param, modifiers, inParamName)
    }
    else if (isEnum(type, data)) {
        paramGenerateEnum(data, funcValue, param, p)
    }
    else if (type.substring(0, 4) == "Map<" || type.substring(0, 6) == "{[key:") {
        paramGenerateMap(funcValue, param, p)
    } else if (isArrayType(type)) {
        paramGenerateArray(p, funcValue, param);
    } else if (type == "any") {
        paramGenerateAny(p, name, type, param);
    }  else if (type == "object" || type == "Object") {
        paramGenerateObject(p, funcValue, param);
    } else {
        NapiLog.logError(
            "The current version does not support generating parameter [%s] with type [%s]".format(name, type));
    }
}

// on/off 接口的event名称参数处理
function eventParamGenerate(p, funcValue, param, data) {
    let name = funcValue.name;
    let type = funcValue.type;
    let regName = re.match("'([a-zA-Z_0-9]+)'", type)
    if (regName) {
        param.eventName = re.getReg(type, regName.regs[1])
        param.valueDefine += "%sstd::string &%s".format(param.valueDefine.length > 0 ? ", " : "", name)
    } else if (type.substring(0, 9) == "Callback<" || type.substring(0, 14) == "AsyncCallback<") {
        paramGenerateCallBack(data, funcValue, param, p)
    } else {
        NapiLog.logError("function eventParamGenerate:The current version do not support to this param to generate :"
        , name, "type :", type);
    }
}

module.exports = {
    jsToC,
    jsToCEnum,
    arrTemplete,
    paramGenerate,
    paramGenerateArray,
    paramGenerateMap,
    mapTempleteFunc,
    eventParamGenerate
}