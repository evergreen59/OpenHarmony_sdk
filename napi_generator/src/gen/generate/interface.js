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
const { generateFunctionDirect } = require("./function_direct");
const { generateFunctionSync } = require("./function_sync");
const { generateFunctionAsync } = require("./function_async");
const { FuncType, InterfaceList, getArrayType, getArrayTypeTwo, getMapType, EnumList, jsType2CType } 
    = require("../tools/common");
const { jsToC } = require("./param_generate");
const { cToJs } = require("./return_generate");
const re = require("../tools/re");
const { NapiLog } = require("../tools/NapiLog");
const { addUniqFunc2List, addUniqObj2List } = require("../tools/tool");

let middleBodyTmplete = `
class [className]_middle {
public:
    static napi_value constructor(napi_env env, napi_callback_info info)
    {
        XNapiTool *pxt = new XNapiTool(env, info);

        [className] *p = new [className]();

        napi_value thisvar = pxt->WrapInstance(p, release);

        return thisvar;
    }
    static void release(void *p)
    {
        [className] *p2 = ([className] *)p;
        delete p2;
    }
    [static_funcs]
};`

function getHDefineOfVariable(name, type, variable) {
    if (type.indexOf("|") >= 0) {
        unionTypeString(name, type, variable)
    } else if (type == "string") variable.hDefine += "\n    std::string %s;".format(name)
    else if (InterfaceList.getValue(type)) variable.hDefine += "\n    %s %s;".format(type, name)
    else if (EnumList.getValue(type)) variable.hDefine += "\n    %s %s;".format(type, name)
    else if (type.indexOf("Array<") == 0) {
        let arrayType = getArrayType(type)
        if (arrayType == "any") {
            variable.hDefine += "\n    std::string %s_type; \n    std::any %s;".format(name,name)
        } else {
            let cType = jsType2CType(arrayType)
            variable.hDefine += "\n    std::vector<%s> %s;".format(cType, name)
        }
    } else if (type == "boolean") {
        variable.hDefine += "\n    bool %s;".format(name)
    } else if (type.substring(type.length - 2) == "[]") {
        let arrayType = getArrayTypeTwo(type)
        if (arrayType == "any") {
            variable.hDefine += "\n    std::string %s_type; \n    std::any %s;".format(name,name)
        } else {
            let cType = jsType2CType(arrayType)
            variable.hDefine += "\n    std::vector<%s> %s;".format(cType, name)
        }
    } else if (type.substring(0, 4) == "Map<" || type.indexOf("{[key:") == 0) {
        variable.hDefine += mapTypeString(type, name)
    } else if (type == "any") {
        variable.hDefine += anyTypeString(type, name)
    } else if (type.substring(0, 12) == "NUMBER_TYPE_") {
        variable.hDefine += "\n    %s %s;".format(type, name)
    } else if (type == "Object" || type == "object") {
        variable.hDefine += "\n    std::map<std::string, std::any> %s;".format(name)
    }
    else {
        NapiLog.logError(`
        ---- generateVariable fail %s,%s ----
        `.format(name, type));
    }
}

function generateVariable(value, variable, className) {
    let name = value.name
    let type = value.type
    if (!value.isParentMember) {
        // 只有类/接口自己的成员属性需要在.h中定义， 父类/父接口不需要
        getHDefineOfVariable(name, type, variable)
    }

    variable.middleValue += `
    static napi_value getvalue_%s(napi_env env, napi_callback_info info)
    {
        XNapiTool *pxt = std::make_unique<XNapiTool>(env, info).release();
        %s *p = (%s *)pxt->UnWarpInstance();
        napi_value result = nullptr;
        `.format(name, className, className) + cToJs("p->" + name, type, "result") + `
        delete pxt;
        return result;
    }
    static napi_value setvalue_%s(napi_env env, napi_callback_info info)
    {
        std::shared_ptr<XNapiTool> pxt = std::make_shared<XNapiTool>(env, info);
        %s *p = (%s *)pxt->UnWarpInstance();
        `.format(name, className, className) + jsToC("p->" + name, "pxt->GetArgv(0)", type) + `
        return nullptr;
    }
`
}

function unionTypeString(name, type, variable) {
    variable.hDefine += `std::string %s_type;\n
    std::any %s;`.format(name, name)
}

function mapTypeString(type, name) {
    let mapType = getMapType(type)
    let mapTypeString
    if (mapType[1] != undefined && mapType[2] == undefined) {
        if (mapType[1] == "string") mapTypeString = "std::string,std::string"
        else if (mapType[1] == "boolean") mapTypeString = "std::string,bool"
        else if (mapType[1].substring(0, 12) == "NUMBER_TYPE_") {
            mapTypeString = "std::string,%s".format(mapType[1])
        }
        else if (mapType[1].substring(0, 12) == "any") {
            mapTypeString = `std::string,std::any`.format(mapType[1])
            return `\n    std::map<%s> %s;
            std::string %s_type;`.format(mapTypeString, name, name)
        }
        else if (InterfaceList.getValue(mapType[1])) mapTypeString = "std::string,%s".format(mapType[1])
    }
    if (mapType[2] != undefined) {
        if (mapType[2] == "string") mapTypeString = "std::string,std::map<std::string,std::string>"
        else if (mapType[2] == "boolean") mapTypeString = "std::string,std::map<std::string,bool>"
        else if (mapType[2].substring(0, 12) == "NUMBER_TYPE_") {
            mapTypeString = "std::string,std::map<std::string,%s>".format(mapType[2])
        }
    }
    if (mapType[3] != undefined) {
        if (mapType[3] == "string") mapTypeString = "std::string,std::vector<std::string>"
        else if (mapType[3] == "boolean") mapTypeString = "std::string,std::vector<bool>"
        else if (mapType[3].substring(0, 12) == "NUMBER_TYPE_") {
            mapTypeString = "std::string,std::vector<%s>".format(mapType[3])
        }
    }
    return "\n    std::map<%s> %s;".format(mapTypeString, name);
}

function anyTypeString (type, name) {
    let anyType = `\n    std::string %s_type;
    std::any %s;`

    return anyType.format(name, name)
}

function generateInterface(name, data, inNamespace) {
    let resultConnect = connectResult(data, inNamespace, name)
    let middleFunc = resultConnect[0]
    let implH = resultConnect[1]
    let implCpp = resultConnect[2]
    let middleInit = resultConnect[3]
    let selfNs = ""
    if (inNamespace.length > 0) {
        let nsl = inNamespace.split("::")
        nsl.pop()
        if (nsl.length >= 2) {
            selfNs = ", " + nsl[nsl.length - 1]
        }
    }
    middleInit += `\n    pxt->DefineClass("%s", %s%s_middle::constructor, valueList ,funcList%s);\n}\n`
        .format(name, inNamespace, name, selfNs)
    let extendsStr = (data.parentNameList && data.parentNameList.length > 0) ?
        " : public %s".format(data.parentNameList.join(", public ")) : ""
    let result = {
        implH: `
class %s%s {
public:%s
};\n`.format(name, extendsStr, implH),
        implCpp: implCpp,
        middleBody: middleBodyTmplete.replaceAll("[className]", name).replaceAll("[static_funcs]", middleFunc),
        middleInit: middleInit
    }
    return result
}

// 递归获取接口及接口父类的所有成员属性和方法
function getAllPropties(interfaceBody, properties, isParentClass) {
    for (let i in interfaceBody.value) {
        interfaceBody.value[i].isParentMember = isParentClass
        addUniqObj2List(interfaceBody.value[i], properties.values)
    }
    for (let i in interfaceBody.function) {
        interfaceBody.function[i].isParentMember = isParentClass
        addUniqFunc2List(interfaceBody.function[i], properties.functions)
    }
    if (!isParentClass && interfaceBody.parentNameList && interfaceBody.parentNameList.length > 0) {
        getAllPropties(interfaceBody.parentBody, properties, true)
    }
} 

function connectResult(data, inNamespace, name) {
    let implH = ""
    let implCpp = ""
    let middleFunc = ""
    let middleInit = ""
    let variable = {
        hDefine: "",
        middleValue: "",
    }
    middleInit = `{\n    std::map<const char *,std::map<const char *,napi_callback>> valueList;`
    data.allProperties = {values:[], functions:[]}
    getAllPropties(data, data.allProperties, false)
    for (let i in data.allProperties.values) {
        let v = data.allProperties.values[i]
        generateVariable(v, variable, name)
        middleInit += `
    valueList["%s"]["getvalue"]=%s%s_middle::getvalue_%s;
    valueList["%s"]["setvalue"]=%s%s_middle::setvalue_%s;`
            .format(v.name, inNamespace, name, v.name, v.name, inNamespace, name, v.name)
    }
    implH += variable.hDefine
    middleFunc += variable.middleValue
    middleInit += `\n    std::map<const char *, napi_callback> funcList;`
    for (let i in data.allProperties.functions) {
        let func = data.allProperties.functions[i]
        let tmp;
        switch (func.type) {
            case FuncType.DIRECT:
                tmp = generateFunctionDirect(func, data, name)
                break;
            case FuncType.SYNC:
                tmp = generateFunctionSync(func, data, name)
                break
            case FuncType.ASYNC:
            case FuncType.PROMISE:
                tmp = generateFunctionAsync(func, data, name)
                break
            default:
                return
        }
        middleFunc += tmp[0]
        implH += tmp[1]
        implCpp += tmp[2]
        middleInit += `\n    funcList["%s"] = %s%s_middle::%s_middle;`.format(func.name, inNamespace, name, func.name)
    }
    if (data.childList && data.childList.length > 0) {
        // 如果是父类，增加虚析构函数使其具备泛型特征 (基类必须有虚函数才能正确使用dynamic_cast和typeinfo等方法)
        implH += "\n    virtual ~%s(){};".format(name)
    }
    return [middleFunc, implH, implCpp, middleInit]
}

module.exports = {
    generateInterface,
    connectResult,
    generateVariable,
    mapTypeString
}