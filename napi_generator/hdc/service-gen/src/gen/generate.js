
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
const { NapiLog } = require("../tools/NapiLog");
const { replaceAll, getTab } = require("../tools/tool");
const re = require("../tools/re");
const { iServiceHTemplate, proxyHTemplate, stubHTemplate, serviceHTemplate, proxyCppTemplate,
    proxyFuncTemplate, stubCppTemplate, stubInnerFuncTemplate, serviceCppTemplate, serviceFuncImplTemplate,
    clientCppTemplate, buildGnTemplate, bundleJsonTemplate, profileGnTemplate, profileXmlTemplate, serviceCfgTemplate,
    serviceCfgGnTemplate } = require("./fileTemplate");
const { DATA_WRITE_MAP, DATA_READ_MAP, getParcelType } = require("../tools/common");

let fileContent = {
    "iServiceHFile": {},
    "proxyHFile": {},
    "stubHFile": {},
    "serviceHFile": {},
    "proxyCppFile": {},
    "stubCppFile": {},
    "serviceCppFile": {},
    "clientCppFile": {},
    "buildGnFile": {},
    "bundleJsonFile": {},
    "profileGnFile": {},
    "profileXmlFile": {},
    "serviceCfgFile": {},
    "serviceCfgGnFile": {}
};

function getIncludeStr(includeList) {
    let includeStr = "";
    for (let i = 0; i < includeList.length; ++i) {
        includeStr += "#include " + includeList[i] + "\n";
    }
    return includeStr;
}

function getFuncParamStr(params) {
    let paramStr = "";
    for (let i = 0; i < params.length; ++i) {
        paramStr += (i == 0) ? "" : ", ";
        paramStr += params[i].type + " " + params[i].name;
    }
    return paramStr;
}

/**
 * 生成c参数写入remote消息buffer(parcel data)的代码段
 * @param {} srcName 待写入的c变量名
 * @param {*} destName 写入目标(parcel data)变量的名称
 * @param {*} vType c变量类型
 * @returns 生成的代码段
 */
function genWriteString(srcName, destName, vType) {
    let matchs = re.match("(std::)?vector<([\x21-\x7e]+)>", vType);
    if (matchs) { // write data of std::vector
        let rowType = re.getReg(vType, matchs.regs[2]);
        let parcelType = getParcelType(rowType);
        let wFunc = DATA_WRITE_MAP.get(parcelType);
        if (!wFunc) {
            NapiLog.logError("Unsupport writing with type: " + vType);
            return "";
        }
        // use function Parcel::WriteVector(const std::vector<T1> &val, bool (Parcel::*Write)(T2))
        return "%s.WriteVector(%s, &(%s.%s));".format(destName, srcName, destName, wFunc);
    }

    let parcelType = getParcelType(vType);
    let wFunc = DATA_WRITE_MAP.get(parcelType);
    if (!wFunc) {
        NapiLog.logError("Unsupport writing with type: " + vType);
        return "";
    }
    return "%s.%s(%s);".format(destName, wFunc, srcName);
}

/**
 * 生成从remote消息buffer(parcel data)读取c参数的代码段
 * @param {*} srcName 待读取的parcel data变量名称
 * @param {*} destName 读取出的内容写入的c变量名称
 * @param {*} vType c变量类型
 * @returns 生成的代码段
 */
function genReadString(srcName, destName, vType) {
    let matchs = re.match("(std::)?vector<([\x21-\x7e]+)>", vType);
    if (matchs) { // write data of std::vector
        let rowType = re.getReg(vType, matchs.regs[2]);
        let parcelType = getParcelType(rowType);
        let rFunc = DATA_READ_MAP.get(parcelType);
        if (!rFunc) {
            NapiLog.logError("Unsupport reading with type: " + vType);
            return "";
        }
        // use function Parcel::ReadVector(std::vector<T> *val, bool (Parcel::*Read)(T &))
        return "%s.ReadVector(&(%s), &(%s.%s));".format(srcName, destName, srcName, rFunc);
    }

    let parcelType = getParcelType(vType);
    let rFunc = DATA_READ_MAP.get(parcelType);
    if (!rFunc) {
        NapiLog.logError("Unsupport reading with type: " + vType);
        return "";
    }
    return "%s = %s.%s();".format(destName, srcName, rFunc);
}

function genProxyFunc(funcInfo, className, paramStr) {
    let proxyFunc = replaceAll(proxyFuncTemplate, "[className]", className);
    proxyFunc = replaceAll(proxyFunc, "[funcName]", funcInfo.name);
    proxyFunc = replaceAll(proxyFunc, "[params]", paramStr);
    proxyFunc = replaceAll(proxyFunc, "[retType]", funcInfo.retType);
    proxyFunc = replaceAll(proxyFunc, "[funcEnum]", funcInfo.funcEnum);

    // 入参处理
    let writeDataStr = "";
    let tab = getTab(1);
    for (let i = 0; i < funcInfo.params.length; ++i) {
        let param = funcInfo.params[i];
        writeDataStr += (i == 0) ? "" : "\n" + tab;
        writeDataStr += genWriteString(param.name, "data", param.type);
    }
    proxyFunc = replaceAll(proxyFunc, "[writeData]", writeDataStr);

    // 返回值处理
    let readReplyStr = "";
    if (funcInfo.retType != "void") {
        readReplyStr = "%s result;".format(funcInfo.retType);
        readReplyStr += "\n" + tab + genReadString("reply", "result", funcInfo.retType);
        readReplyStr += "\n" + tab + "return result;";
    }
    proxyFunc = replaceAll(proxyFunc, "[readReply]", readReplyStr);

    return proxyFunc;
}

function genStubInnerFunc(funcInfo, className) {
    let innerFunc = replaceAll(stubInnerFuncTemplate, "[className]", className);
    innerFunc = replaceAll(innerFunc, "[funcName]", funcInfo.name);

    // 入参处理
    let readDataStr = ""; // 生成服务端读取客户端传参的代码段
    let tab = getTab(1);
    let innerParamStr = ""; // 调用业务方法时传入的入参列表
    for (let i = 0; i < funcInfo.params.length; ++i) {
        let param = funcInfo.params[i];
        let innerParamName = param.name + "Val";
        if (i > 0) {
            readDataStr += "\n" + tab;
            innerParamStr += " ,";
        }
        
        //将remote请求中的参数值读取到内部参数变量中
        readDataStr += "%s %s;".format(param.type, innerParamName); // 定义内部参数变量
        readDataStr += "\n" + tab + genReadString("data", param.name + "Val", param.type);
        innerParamStr += innerParamName;
    }
    innerFunc = replaceAll(innerFunc, "[readData]", readDataStr);

    // 调用service的实际业务逻辑实现方法
    let writeReplyStr = ""; // 生成调用服务端实现并返回结果的代码段
    if (funcInfo.retType === "void") {
        writeReplyStr += "%s(%s); // call business implementation".format(funcInfo.name, innerParamStr);
        writeReplyStr += "\n" + tab + "reply.WriteInt32(retCode);";
    } else {
        writeReplyStr += "%s retVal = %s(%s);  // call business implementation".format(
            funcInfo.retType, funcInfo.name, innerParamStr);
        writeReplyStr += "\n" + tab + "reply.WriteInt32(retCode);";
        writeReplyStr += "\n" + tab + genWriteString("retVal", "reply", funcInfo.retType);
    }
    innerFunc = replaceAll(innerFunc, "[writeReply]", writeReplyStr);
    return innerFunc;
}

function genServiceFunc(funcInfo, className, paramStr) {
    let serviceFunc = replaceAll(serviceFuncImplTemplate, "[retType]", funcInfo.retType);
    serviceFunc = replaceAll(serviceFunc, "[className]", className);
    serviceFunc = replaceAll(serviceFunc, "[funcName]", funcInfo.name);
    serviceFunc = replaceAll(serviceFunc, "[params]", paramStr);
    return serviceFunc;
}

function genFunctions(classInfo, files) {
    let res = genFunctionCode(classInfo);
    files.iServiceH = replaceAll(files.iServiceH, "[funcEnum]", res.funcEnumStr);
    files.iServiceH = replaceAll(files.iServiceH, "[functions]", res.iServiceFuncH);
    files.proxyH = replaceAll(files.proxyH, "[functions]", res.proxyFuncH);
    files.stubH = replaceAll(files.stubH, "[innerFuncDef]", res.stubInnerFuncH);
    files.serviceH = replaceAll(files.serviceH, "[functions]", res.proxyFuncH);
    files.proxyCpp = replaceAll(files.proxyCpp, "[remoteFuncImpl]", res.proxyFuncCpp);
    files.stubCpp = replaceAll(files.stubCpp, "[innerFuncMap]", res.stubInnerFuncMap);
    files.stubCpp = replaceAll(files.stubCpp, "[innerFuncImpl]", res.stubInnerFuncCpp);
    files.serviceCpp = replaceAll(files.serviceCpp, "[serviceFuncImpl]", res.serviceFuncCpp);
    files.clientCpp = replaceAll(files.clientCpp, "[clientFuncInvoke]", res.clientFuncCpp);
}

function genFilesByTemplate(upperServiceName, lowServiceName, rootInfo) {
    let files = {};
    // 按模板生成.h和.cpp文件内容框架
    files.iServiceH = replaceAll(iServiceHTemplate, "[marcoName]", upperServiceName);
    files.proxyH = replaceAll(proxyHTemplate, "[marcoName]", upperServiceName);
    files.stubH = replaceAll(stubHTemplate, "[marcoName]", upperServiceName);
    files.serviceH = replaceAll(serviceHTemplate, "[marcoName]", upperServiceName);
    files.proxyCpp = proxyCppTemplate;
    files.stubCpp = stubCppTemplate;
    files.serviceCpp = replaceAll(serviceCppTemplate, "[marcoName]", upperServiceName);
    files.clientCpp = replaceAll(clientCppTemplate, "[marcoName]", upperServiceName);

    // 按模板生成资源配置文件内容框架
    files.buildGn = replaceAll(buildGnTemplate, "[lowServiceName]", lowServiceName);
    files.buildGn = replaceAll(files.buildGn, "[stubCppFile]", fileContent.stubCppFile.name);
    files.buildGn = replaceAll(files.buildGn, "[serviceCppFile]", fileContent.serviceCppFile.name);
    files.buildGn = replaceAll(files.buildGn, "[proxyCppFile]", fileContent.proxyCppFile.name);
    files.buildGn = replaceAll(files.buildGn, "[clientCppFile]", fileContent.clientCppFile.name);
    files.bundleJson = replaceAll(bundleJsonTemplate, "[lowServiceName]", lowServiceName);
    files.profileGn = replaceAll(profileGnTemplate, "[lowServiceName]", lowServiceName);
    files.profileGn = replaceAll(files.profileGn, "[serviceId]", rootInfo.serviceId);
    files.profileXml = replaceAll(profileXmlTemplate, "[lowServiceName]", lowServiceName);
    files.profileXml = replaceAll(files.profileXml, "[serviceId]", rootInfo.serviceId);
    files.serviceCfg = replaceAll(serviceCfgTemplate, "[lowServiceName]", lowServiceName);
    files.serviceGnCfg = replaceAll(serviceCfgGnTemplate, "[lowServiceName]", lowServiceName);
    return files;
}

function replaceClassName(files, classInfo) {
    files.iServiceH = replaceAll(files.iServiceH, "[className]", classInfo.name);
    files.proxyH = replaceAll(files.proxyH, "[className]", classInfo.name);
    files.stubH = replaceAll(files.stubH, "[className]", classInfo.name);
    files.serviceH = replaceAll(files.serviceH, "[className]", classInfo.name);
    files.proxyCpp = replaceAll(files.proxyCpp, "[className]", classInfo.name);
    files.stubCpp = replaceAll(files.stubCpp, "[className]", classInfo.name);
    files.serviceCpp = replaceAll(files.serviceCpp, "[className]", classInfo.name);
    files.clientCpp = replaceAll(files.clientCpp, "[className]", classInfo.name);
}

function replaceServiceName(files, rootInfo) {
    files.iServiceH = replaceAll(files.iServiceH, "[serviceName]", rootInfo.serviceName);
    files.proxyH = replaceAll(files.proxyH, "[serviceName]", rootInfo.serviceName);
    files.stubH = replaceAll(files.stubH, "[serviceName]", rootInfo.serviceName);
    files.serviceH = replaceAll(files.serviceH, "[serviceName]", rootInfo.serviceName);
    files.proxyCpp = replaceAll(files.proxyCpp, "[serviceName]", rootInfo.serviceName);
    files.stubCpp = replaceAll(files.stubCpp, "[serviceName]", rootInfo.serviceName);
    files.serviceCpp = replaceAll(files.serviceCpp, "[serviceName]", rootInfo.serviceName);
    files.clientCpp = replaceAll(files.clientCpp, "[serviceName]", rootInfo.serviceName);
}

function replaceIncludes(files, rootInfo) {
    files.iServiceH = replaceAll(files.iServiceH, "[includes]", getIncludeStr(rootInfo.includes));
    files.proxyH = replaceAll(files.proxyH, "[iServiceHInclude]", fileContent.iServiceHFile.name);
    files.stubH = replaceAll(files.stubH, "[iServiceHInclude]", fileContent.iServiceHFile.name);
    files.serviceH = replaceAll(files.serviceH, "[stubHInclude]", fileContent.stubHFile.name);
    files.proxyCpp = replaceAll(files.proxyCpp, "[proxyHInclude]", fileContent.proxyHFile.name);
    files.stubCpp = replaceAll(files.stubCpp, "[stubHInclude]", fileContent.stubHFile.name);
    files.serviceCpp = replaceAll(files.serviceCpp, "[serviceHInclude]", fileContent.serviceHFile.name);
    files.clientCpp = replaceAll(files.clientCpp, "[proxyHInclude]", fileContent.proxyHFile.name);
}

function genFileNames(lowServiceName, rootInfo) {
    fileContent.iServiceHFile.name = "i_%s_service.h".format(lowServiceName);
    fileContent.proxyHFile.name = "%s_service_proxy.h".format(lowServiceName);
    fileContent.stubHFile.name = "%s_service_stub.h".format(lowServiceName);
    fileContent.serviceHFile.name = "%s_service.h".format(lowServiceName);
    fileContent.proxyCppFile.name = "%s_service_proxy.cpp".format(lowServiceName);
    fileContent.stubCppFile.name = "%s_service_stub.cpp".format(lowServiceName);
    fileContent.serviceCppFile.name = "%s_service.cpp".format(lowServiceName);
    fileContent.clientCppFile.name = "%s_client.cpp".format(lowServiceName);
    fileContent.buildGnFile.name = "BUILD.gn";
    fileContent.bundleJsonFile.name = "bundle.json";
    fileContent.profileGnFile.name = "BUILD.gn";
    fileContent.profileXmlFile.name = rootInfo.serviceId + ".xml";
    fileContent.serviceCfgFile.name = "%s_service.cfg".format(lowServiceName);
    fileContent.serviceCfgGnFile.name = "BUILD.gn";
}

function genFunctionCode(classInfo) {
    let funcList = classInfo.functions;
    let genResult = {}
    genResult.funcEnumStr = "";
    genResult.iServiceFuncH = ""; //i_service.h 方法定义
    genResult.proxyFuncH = ""; //proxy.h 方法定义
    genResult.stubInnerFuncH = ""; // stub.h 的inner方法定义
    genResult.proxyFuncCpp = ""; //proxy.cpp 方法实现
    genResult.stubInnerFuncMap = ""; // stub.cpp 的inner方法映射表
    genResult.stubInnerFuncCpp = ""; // stub.cpp 的inner方法实现
    genResult.serviceFuncCpp = ""; // service.cpp的方法实现
    genResult.clientFuncCpp = ""; // client.cpp 的inner方法定义

    let enumTab = getTab(2);
    let funcTab = getTab(1);
    for (var i = 0; i < funcList.length; ++i) {
        funcList[i].funcEnum = funcList[i].name.toUpperCase(); // remote方法的枚举值
        genResult.funcEnumStr += (i == 0) ? "" : ",\n" + enumTab;
        genResult.funcEnumStr += funcList[i].funcEnum;

        let paramStr = getFuncParamStr(funcList[i].params);
        genResult.iServiceFuncH += (i == 0) ? "" : "\n" + funcTab;
        genResult.iServiceFuncH += "virtual %s %s(%s) = 0;".format(funcList[i].retType, funcList[i].name, paramStr);

        genResult.proxyFuncH += (i == 0) ? "" : "\n" + funcTab;
        genResult.proxyFuncH += "%s %s(%s) override;".format(funcList[i].retType, funcList[i].name, paramStr);

        genResult.stubInnerFuncH += (i == 0) ? "" : "\n" + funcTab;
        genResult.stubInnerFuncH += 
            "ErrCode %sInner(MessageParcel &data, MessageParcel &reply);".format(funcList[i].name);

        genResult.proxyFuncCpp += genProxyFunc(funcList[i], classInfo.name, paramStr);

        genResult.stubInnerFuncMap += (i == 0) ? "" : "\n" + funcTab;
        genResult.stubInnerFuncMap += "innerFuncs_[%s] = &%sStub::%sInner;".format(
            funcList[i].funcEnum, classInfo.name, funcList[i].name);

        genResult.stubInnerFuncCpp += genStubInnerFunc(funcList[i], classInfo.name);
        genResult.serviceFuncCpp += genServiceFunc(funcList[i], classInfo.name, paramStr);

        genResult.clientFuncCpp += (i == 0) ? "" : "\n" + funcTab;
        genResult.clientFuncCpp += "// proxy->%s(%s);".format(funcList[i].name, paramStr);
    }
    return genResult;
}

function doGenerate(rootInfo) {
    let lowServiceName = rootInfo.serviceName.toLowerCase();
    let upperServiceName = rootInfo.serviceName.toUpperCase();

    // 生成文件名
    genFileNames(lowServiceName, rootInfo);

    // 按模板生成.h和.cpp文件内容框架
    let files = genFilesByTemplate(upperServiceName, lowServiceName, rootInfo);

    // 替换文件includes
    replaceIncludes(files, rootInfo);

    // 替换namespace
    replaceServiceName(files, rootInfo);

    // 替换类名
    let classInfo = rootInfo.class[0]
    replaceClassName(files, classInfo);

    // 生成函数定义与实现
    genFunctions(classInfo, files);

    // 文件内容汇总
    fileContent.iServiceHFile.content = files.iServiceH;
    fileContent.proxyHFile.content = files.proxyH;
    fileContent.stubHFile.content = files.stubH;
    fileContent.serviceHFile.content = files.serviceH;
    fileContent.proxyCppFile.content = files.proxyCpp;
    fileContent.stubCppFile.content = files.stubCpp;
    fileContent.serviceCppFile.content = files.serviceCpp;
    fileContent.clientCppFile.content = files.clientCpp;
    fileContent.buildGnFile.content = files.buildGn;
    fileContent.bundleJsonFile.content = files.bundleJson;
    fileContent.profileGnFile.content = files.profileGn;
    fileContent.profileXmlFile.content = files.profileXml;
    fileContent.serviceCfgFile.content = files.serviceCfg;
    fileContent.serviceCfgGnFile.content = files.serviceGnCfg;
    return fileContent;
}

module.exports = {
    doGenerate
}
