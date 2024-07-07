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
//生成BUILD.gn
//生成tool_utility.h，生成tool_utility.cpp
const { replaceAll } = require("./tools/tool");
const { generateNamespace } = require("./generate/namespace");
const { writeFile } = require("./tools/FileRW");
const re = require("./tools/re");
const { generateGYP } = require("./extend/binding_gyp");
const { generateGN } = require("./extend/build_gn");
const { generateBase } = require("./extend/tool_utility");
const { NumberIncrease } = require("./tools/common");
var fs = require('fs');

let moduleCppTmplete = `\
#include <cstring>
#include <string>
#include <memory>
#include <vector>
#include <node_api.h>
#include <any>
#include "tool_utility.h"
#include "[implName].h"

#define NUMBER_JS_2_C(napi_v, type, dest)      \\
    if (typeid(type) == typeid(int32_t))  {    \\
        dest = pxt->SwapJs2CInt32(napi_v);     \\
    }                                          \\
    else if (typeid(type) == typeid(uint32_t)){\\
        dest = pxt->SwapJs2CUint32(napi_v);    \\
    }                                          \\
    else if (typeid(type) == typeid(int64_t)){ \\
        dest = pxt->SwapJs2CInt64(napi_v);     \\
    }                                          \\
    else if (typeid(type) == typeid(double_t)){\\
        dest = pxt->SwapJs2CDouble(napi_v);    \\
    } 

#define NUMBER_JS_2_C_ENUM(napi_v, type, dest, enum_type)      \\
    if (typeid(type) == typeid(int32_t))  {    \\
        dest = static_cast<enum_type>(pxt->SwapJs2CInt32(napi_v));     \\
    }                                          \\
    else if (typeid(type) == typeid(uint32_t)){\\
        dest = static_cast<enum_type>(pxt->SwapJs2CUint32(napi_v));    \\
    }                                          \\
    else if (typeid(type) == typeid(int64_t)){ \\
        dest = static_cast<enum_type>(pxt->SwapJs2CInt64(napi_v));     \\
    }                                          \\
    else if (typeid(type) == typeid(double_t)){\\
        dest = static_cast<enum_type>(pxt->SwapJs2CDouble(napi_v));    \\
    } 
    
#define BOOLEAN_JS_2_C(napi_v, type, dest){    \\
    dest = pxt->SwapC2JsBool(napi_v);          \\
}

#define C_DELETE(p)  \\
    if (p) {         \\
        delete p;    \\
    }

__attribute__((unused)) static napi_value number_c_to_js(XNapiTool *pxt, const std::type_info &n, void *num)
{
    if (n == typeid(int32_t))
        return pxt->SwapC2JsInt32(*(int32_t *)num);
    else if (n == typeid(uint32_t))
        return pxt->SwapC2JsUint32(*(uint32_t *)num);
    else if (n == typeid(int64_t))
        return pxt->SwapC2JsInt64(*(int64_t *)num);
    else if (n == typeid(double_t))
        return pxt->SwapC2JsDouble(*(double_t *)num);
    return nullptr;
}
#define NUMBER_C_2_JS(pxt, n) \\
    number_c_to_js(pxt, typeid(n), &n)

[body_replace]

static napi_value init(napi_env env, napi_value exports)
{
    std::shared_ptr<XNapiTool> pxt = std::make_shared<XNapiTool>(env, exports);

    [init_replace]

    return exports;
}

static napi_module g_[implName]_Module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = init,
    .nm_modname = "[modulename]",
    .nm_priv = ((void *)0),
    .reserved = {(void *)0},
};

extern "C" __attribute__((constructor)) void Register_[implName]_Module(void)
{
    napi_module_register(&g_[implName]_Module);
}
`

let implHTemplete = `\
#ifndef IMPL_[impl_name_upper]_H
#define IMPL_[impl_name_upper]_H

#include <string>
#include <memory>
#include <vector>
#include <cmath>
#include <map>
#include <any>
[importTs]

[numberUsing]

[implH_detail]

#endif // IMPL_[impl_name_upper]_H
`

let implCppTemplete = `\
#include "[implName].h"

[implCpp_detail]
`

function generateAll(structOfTs, destDir, moduleName, numberType) {
    let ns0 = structOfTs.declareNamespace[0];
    let license = structOfTs.declareLicense[0];
    let result = generateNamespace(ns0.name, ns0.body)
    let numberUsing = ""
    var numbertype = "uint32_t";
    if(numberType != ""){
        numbertype = numberType;
    }
    for (let i = 1; i < NumberIncrease.get(); i++) {
        numberUsing += "using NUMBER_TYPE_%d = ".format(i) + numbertype + ";\n"
    }
    let middleCpp = replaceAll(moduleCppTmplete, "[body_replace]", result.middleBody);
    middleCpp = replaceAll(middleCpp, "[init_replace]", result.middleInit);
    middleCpp = replaceAll(middleCpp, "[implName]", ns0.name);
    middleCpp = replaceAll(middleCpp, "[modulename]", moduleName);
    writeFile(re.pathJoin(destDir, "%s_middle.cpp".format(ns0.name)),
        null != license ? (license + "\n" + middleCpp) : middleCpp)

    let implH = replaceAll(implHTemplete, "[impl_name_upper]", ns0.name.toUpperCase())
    implH = implH.replaceAll("[numberUsing]", numberUsing);
    implH = replaceAll(implH, "[implH_detail]", result.implH)
    let imports = ''
    for (let i = 0; i < structOfTs.imports.length; i++){
        imports += structOfTs.imports[i]
    }
    implH = replaceAll(implH, "[importTs]", imports)
    writeFile(re.pathJoin(destDir, "%s.h".format(ns0.name)), null != license ? (license + "\n" + implH) : implH)

    let implCpp = implCppTemplete.replaceAll("[implName]", ns0.name)
    implCpp = implCpp.replaceAll("[implCpp_detail]", result.implCpp)
    writeFile(re.pathJoin(destDir, "%s.cpp".format(ns0.name)), null != license ? (license + "\n" + implCpp) : implCpp)

    let partName = moduleName.replace('.', '_')
    generateGYP(destDir, ns0.name, license)//生成ubuntu下测试的编译脚本
    generateGN(destDir, ns0.name, license, partName)//生成BUILD.gn for ohos
    generateBase(destDir, license)//tool_utility.h/cpp
}

module.exports = {
    generateAll
}
