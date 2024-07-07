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
let genDir = "../../src/gen/"

const { analyzeFile } = require(genDir + "analyze");
var assert = require("assert");
const re = require(genDir + "/tools/re");
const { info, Console } = require("console");
const { JsxEmit } = require("typescript");
const { readFile } = require(genDir + "tools/FileRW");
const { generateEnum } = require(genDir + "generate/enum");
const { jsToC, jsToCEnum, paramGenerate, paramGenerateArray } = require(genDir + "generate/param_generate");
const { paramGenerateMap, mapTempleteFunc } = require(genDir + "generate/param_generate");
const { cToJs, cToJsForInterface, returnGenerate } = require(genDir + "generate/return_generate");
const { generateInterface, generateVariable } = require(genDir + "generate/interface");
const { mapTypeString, connectResult } = require(genDir + "generate/interface");
const { generateNamespace, formatMiddleInit } = require(genDir + "generate/namespace");
const { generateEnumResult, generateFunction } = require(genDir + "generate/namespace");
const { generateFunctionAsync } = require(genDir + "generate/function_async");
const { generateFunctionDirect } = require(genDir + "generate/function_direct");
const { generateFunctionSync } = require(genDir + "generate/function_sync");

var correctResult

function before() {
    let data = readFile("test/unittest/result.json")

    if (data) {
        correctResult = JSON.parse(data)
    }

    result = analyzeFile("test/unittest/@ohos.input_sample.d.ts")
}

describe('Generate', function () {
    before(function () {
        before()
    });

    it('test gen/generate/enum generateEnum', function () {
        let data = {
            element: [{ name: "STATUS0", value: "0", type: "NUMBER_TYPE_1" }],
            function: [],
            enumValueType: 0
        }
        let ret = generateEnum('GrantStatus', data);
        let retJson = JSON.stringify(ret);
        let result = { "implH": "\nenum GrantStatus {\n    STATUS0 = 0,\n};\n", "implCpp": "" };
        assert.strictEqual(retJson, JSON.stringify(result));
    });

    it('test gen/generate/function_async generateFunctionAsync', function () {
        let retJson = funcAsyncAssert();
        let struct = retJson.substring(retJson.indexOf("{"),retJson.indexOf("}")+1)
        assert.strictEqual(struct,"{\\n    std::string in0;\\n    uint32_t outErrCode = 0;\\n    std::string out;\\n}")
        let execute = retJson.substring(retJson.indexOf("if_async_execute"),retJson.indexOf("*data")+6)
        assert.strictEqual(execute,"if_async_execute(XNapiTool *pxt, void *data)")
        let complete = retJson.substring(retJson.indexOf("if_async_complete"),retJson.lastIndexOf("*data")+6)
        assert.strictEqual(complete,"if_async_complete(XNapiTool *pxt, void *data)")
        let middle = retJson.substring(retJson.indexOf("if_async_middle"),retJson.indexOf("info)")+5)
        assert.strictEqual(middle,"if_async_middle(napi_env env, napi_callback_info info)")
    });

    it('test gen/generate/function_direct generateFunctionDirect', function () {
        let retJson = funcDirectAssert();
        let struct = retJson.substring(retJson.indexOf("{"), retJson.indexOf("}") + 1)
        assert.strictEqual(struct, "{\\n    std::string in0;\\n    \\n    std::string out;\\n}")
        let middle = retJson.substring(retJson.indexOf("if_direct_middle"), retJson.indexOf("info)") + 5)
        assert.strictEqual(middle, "if_direct_middle(napi_env env, napi_callback_info info)")
    });

    it('test gen/generate/function_sync generateFunctionSync', function () {
        let retJson = funcSyncAssert();
        let struct = retJson.substring(retJson.indexOf("{"), retJson.indexOf("}") + 1)
        assert.strictEqual(struct, "{\\n    std::string in0;\\n    \\n    std::string out;\\n}")
        let middle = retJson.substring(retJson.indexOf("if_callback_middle"), retJson.indexOf("info)") + 5)
        assert.strictEqual(middle, "if_callback_middle(napi_env env, napi_callback_info info)")
    });

    partOfInterface(correctResult);

    partOfTest();

    partOfNamespace(correctResult)

    partofParamGenerate(correctResult)

});

function funcAsyncAssert() {
    let valueFi = { name: 'v1', type: 'string' };
    let value1Se = { name: 'cb', type: 'AsyncCallback<string>' };
    let funParam = { name: 'if_async', type: 4, value: [valueFi, value1Se], ret: 'string' }
    let data = {
        class: [],
        const: [],
        enum: [],
        exports: [],
        function: [{ name: 'if_async', type: 4, value: [valueFi, value1Se], ret: 'string' }],
        interface: [],
        namespace: [],
        type: [],
    }
    let ret = generateFunctionAsync(funParam, data, 'TestClass1');
    let retJson = JSON.stringify(ret);
    return retJson
}


function funcDirectAssert() {
    let valueFi = { name: 'v1', type: 'string' };
    let funParam = { name: 'if_direct', type: 1, value: [valueFi], ret: 'string' };
    let data = {
        class: [],
        const: [],
        enum: [],
        exports: [],
        function: [{ name: 'if_direct', type: 1, value: [valueFi], ret: 'string' }],
        interface: [],
        namespace: [],
        type: [],
    }
    let ret = generateFunctionDirect(funParam, data, 'TestClass1');
    let retJson = JSON.stringify(ret);
    return retJson
}

function funcSyncAssert() {
    let valueFi = { name: 'v1', type: 'string' };
    let value1Se = { name: 'cb', type: 'Callback<string>' };
    let funParam = { name: 'if_callback', type: 2, value: [valueFi, value1Se], ret: 'string' };
    let data = {
        class: [],
        const: [],
        enum: [],
        exports: [],
        function: [{ name: 'if_callback', type: 2, value: [valueFi, value1Se], ret: 'string' }],
        interface: [],
        namespace: [],
        type: [],
    }
    let ret = generateFunctionSync(funParam, data, 'TestClass1');
    let retJson = JSON.stringify(ret);
    return retJson
}

function partOfInterface() {

    it('test gen/generate/interface generateVariable', function () {
        variable = {
            hDefine: '',
            middleValue: ''
        };
        let ret = generateVariable('disable', 'boolean', variable, 'ConfigOption');
        assert.strictEqual(JSON.stringify(ret), undefined);
    });

    it('test gen/generate/interface mapTypeString', function () {
        let ret = mapTypeString("Map<string,string>", "map1");
        let result = "\n    std::map<std::string,std::string> map1;";
        assert.strictEqual(JSON.stringify(ret), JSON.stringify(result));
    });

    it('test gen/generate/interface generateInterface', function () {
        let data = {
            function: [],
            value: [{ name: "disable", type: "boolean" }, { name: "map1", type: "Map<string,string>" }]
        }
        let ret = generateInterface('ConfigOption', data, 'napitest::');
        let retJson = JSON.stringify(ret).substring(1, JSON.stringify(ret).length);
        let configOption = retJson.substring(retJson.indexOf("{"), retJson.indexOf("}") + 1);
        let configResult = "{\\npublic:\\n    bool disable;\\n    std::map<std::string,std::string> map1;\\n}";
        assert.strictEqual(configOption, configResult);
        assert.strictEqual(retJson.search("ConfigOption_middle") > 0, true)
        assert.strictEqual(retJson.search("middleInit") > 0, true)
    });


    it('test gen/generate/interface connectResult', function () {
        let data = {
            function: [],
            value: [{ name: "disable", type: "boolean" }, { name: "map1", type: "Map<string,string>" }]
        }
        let ret = connectResult(data, 'napitest::', 'ConfigOption');
        let retJson = JSON.stringify(ret);
        let getDisable = retJson.search("static napi_value getvalue_disable");
        assert.strictEqual(getDisable > 0, true)
        let setDisable = retJson.search("static napi_value setvalue_disable");
        assert.strictEqual(setDisable > 0, true)
        let getMap1 = retJson.search("static napi_value getvalue_map1");
        assert.strictEqual(getMap1 > 0, true)
        let setMap1 = retJson.search("static napi_value setvalue_map1");
        assert.strictEqual(setMap1 > 0, true)
    });
}

function partOfTest() {
    it('test gen/generate/param_generate jsToC', function () {
        assert.strictEqual(jsToC("a", "b", "string"), "pxt->SwapJs2CUtf8(b, a);");

        assert.strictEqual(jsToC("a", "b", "NUMBER_TYPE_1"), "NUMBER_JS_2_C(b,NUMBER_TYPE_1,a);");

        assert.strictEqual(jsToC("a", "b", "boolean"), "BOOLEAN_JS_2_C(b,bool,a);");

        let retJsToC = JSON.stringify(jsToC("a", "b", "Array<string>"));
        retJsToC = re.replaceAll(retJsToC, "len[0-9]*", "len");
        retJsToC = re.replaceAll(retJsToC, "i[0-9]*", "i");
        retJsToC = re.replaceAll(retJsToC, "tt[0-9]*", "tt");
        assert.strictEqual(retJsToC, JSON.stringify(jsToCParam()));

        let retJsToC1 = JSON.stringify(jsToC("a", "b", "string[]"));
        retJsToC1 = re.replaceAll(retJsToC1, "len[0-9]*", "len");
        retJsToC1 = re.replaceAll(retJsToC1, "i[0-9]*", "i");
        retJsToC1 = re.replaceAll(retJsToC1, "tt[0-9]*", "tt");
        assert.strictEqual(retJsToC1, JSON.stringify(jsToCParamArray()));

        let retJsToC2 = JSON.stringify(jsToC("a", "b", "{[key:string]:boolean}"));
        retJsToC2 = re.replaceAll(retJsToC2, "len[0-9]*", "len");
        retJsToC2 = re.replaceAll(retJsToC2, "i[0-9]*", "i");
        retQiepian = retJsToC2.substring(retJsToC2.indexOf("tt"), retJsToC2.indexOf("tt") + 3)
        retJsToC2 = re.replaceAll(retJsToC2, retQiepian, "tt");
        retJsToC2 = re.replaceAll(retJsToC2, "tt[0-9]+", "tt1");
        assert.strictEqual(retJsToC2, JSON.stringify(jsToCParamMap()));

        let retJsToC3 = JSON.stringify(jsToC("a", "b", "Map<string,number>"));
        retJsToC3 = re.replaceAll(retJsToC3, "len[0-9]*", "len");
        retJsToC3 = re.replaceAll(retJsToC3, "i[0-9]*", "i");
        retQiepian = retJsToC3.substring(retJsToC3.indexOf("tt"), retJsToC3.indexOf("tt") + 3)
        retJsToC3 = re.replaceAll(retJsToC3, retQiepian, "tt");
        retJsToC3 = re.replaceAll(retJsToC3, "tt[0-9]+", "tt1");
        assert.strictEqual(retJsToC3, JSON.stringify(jsToCParamMap1()));
    });

    it('test gen/generate/param_generate jsToCEnum', function () {
        let ret = jsToCEnum('string', 'vio->in0', 'pxt->GetArgv(0)')
        let retJson = JSON.stringify(ret)
        assert.strictEqual(retJson, '""')
    });

    partOfTestTwo()
}

function partOfTestTwo(){

    it('test gen/generate/return_generate cToJs', function () {
        assert.strictEqual(cToJs("a", "string", "b", 1), "b = pxt->SwapC2JsUtf8(a.c_str());")

        ret = cToJs("a", "NUMBER_TYPE_1", "b", 1)
        assert.strictEqual(ret, "b = NUMBER_C_2_JS(pxt, a);")

        ret1 = cToJs("a", "boolean", "b", 1)
        assert.strictEqual(ret1, "b = pxt->SwapC2JsBool(a);")

        let retcToJs = JSON.stringify(cToJs("a", "Array<string>", "b", 1))
        retcToJs = re.replaceAll(retcToJs, "len[0-9]*", "len")
        retcToJs = re.replaceAll(retcToJs, "i[0-9]*", "i")
        retcToJs = re.replaceAll(retcToJs, "tnv[0-9]*", "tnv")
        assert.strictEqual(retcToJs, JSON.stringify(cToJsParam()))

        let retcToJs1 = JSON.stringify(cToJs("a", "string[]", "b", 1))
        retcToJs1 = re.replaceAll(retcToJs1, "len[0-9]*", "len")
        retcToJs1 = re.replaceAll(retcToJs1, "i[0-9]*", "i")
        retcToJs1 = re.replaceAll(retcToJs1, "tnv[0-9]*", "tnv")
        assert.strictEqual(retcToJs1, JSON.stringify(cToJsParamArray()))

        let retcToJs2 = JSON.stringify(cToJs("a", "Map<string,string>", "b", 1))
        retcToJs2 = re.replaceAll(retcToJs2, "i[0-9]*", "i")
        retQiepian = retcToJs2.substring(retcToJs2.indexOf("tnv"), retcToJs2.indexOf("tnv") + 4)
        retcToJs2 = re.replaceAll(retcToJs2, retQiepian, "tnv")
        retcToJs2 = re.replaceAll(retcToJs2, "tnv[0-9]+", "tnv1")
        assert.strictEqual(retcToJs2, JSON.stringify(cToJsParamMap()))

        let retcToJs3 = JSON.stringify(cToJs("a", "{[key:string]:string}", "b", 1))
        retcToJs3 = re.replaceAll(retcToJs3, "i[0-9]*", "i")
        retQiepian = retcToJs3.substring(retcToJs3.indexOf("tnv"), retcToJs3.indexOf("tnv") + 4)
        retcToJs3 = re.replaceAll(retcToJs3, retQiepian, "tnv")
        retcToJs3 = re.replaceAll(retcToJs3, "tnv[0-9]+", "tnv1")
        assert.strictEqual(retcToJs3, JSON.stringify(cToJsParamMap1()))
    });

    it('test gen/generate/return_generate cToJsForInterface', function () {
        let ret = cToJsForInterface('vio->out', 'ConfigOption', 'result', 1)
        let retJson = JSON.stringify(ret)
        assert.strictEqual(retJson, '""')
    });
}

function cToJsParam() {
    let value = 'uint32_t len=a.size();\n' +
        '    for(uint32_t i=0;i<len;i++) {\n' +
        '        napi_value tnv = nullptr;\n' +
        '        tnv = pxt->SwapC2JsUtf8(a[i].c_str());\n' +
        '        pxt->SetArrayElement(b, i, tnv);\n' +
        '    }'
    return value
}

function cToJsParamArray() {
    let value = 'uint32_t len=a.size();\n' +
        '    for(uint32_t i=0;i<len;i++) {\n' +
        '        napi_value tnv = nullptr;\n' +
        '        tnv = pxt->SwapC2JsUtf8(a[i].c_str());\n' +
        '        pxt->SetArrayElement(b, i, tnv);\n' +
        '    }'
    return value
}

function cToJsParamMap() {
    let value = 'result = nullptr;\n' +
        '    for (auto i = a.begin(); i != a.end(); i++)\n' +
        '        {\n' +
        '            const char * tnv;\n' +
        '            napi_value tnv1 = nullptr;\n' +
        '            tnv = (i -> first).c_str();\n' +
        '        tnv1 = pxt->SwapC2JsUtf8(i->second.c_str());\n' +
        '            pxt->SetMapElement(b, tnv, tnv1);\n' +
        '        }'
    return value
}

function cToJsParamMap1() {
    let value = 'result = nullptr;\n' +
        '    for (auto i = a.begin(); i != a.end(); i++)\n' +
        '        {\n' +
        '            const char * tnv;\n' +
        '            napi_value tnv1 = nullptr;\n' +
        '            tnv = (i -> first).c_str();\n' +
        '        tnv1 = pxt->SwapC2JsUtf8(i->second.c_str());\n' +
        '            pxt->SetMapElement(b, tnv, tnv1);\n' +
        '        }'
    return value
}

function jsToCParam() {
    let value = '    uint32_t len=pxt->GetArrayLength(b);\n' +
        '    for(uint32_t i=0;i<len;i++) {\n' +
        '        std::string tt;\n' +
        '        pxt->SwapJs2CUtf8(pxt->GetArrayElement(b,i), tt);\n' +
        '        a.push_back(tt);\n' +
        '\n' +
        '    }\n'
    return value
}

function jsToCParamArray() {
    let value = '    uint32_t len=pxt->GetArrayLength(b);\n' +
        '    for(uint32_t i=0;i<len;i++) {\n' +
        '        std::string tt;\n' +
        '        pxt->SwapJs2CUtf8(pxt->GetArrayElement(b,i), tt);\n' +
        '        a.push_back(tt);\n' +
        '\n' +
        '    }\n'
    return value
}

function jsToCParamMap() {
    let value = 'uint32_t len=pxt->GetMapLength(b);\n' +
        'for(uint32_t i=0;i<len;i++) {\n' +
        '    std::string tt;\n' +
        '    bool tt1;\n' +
        '    pxt->SwapJs2CUtf8(pxt->GetMapElementName(b,i), tt);\n' +
        '        tt1 = pxt->SwapJs2CBool(pxt->GetMapElementValue(b,tt.c_str()));\n' +
        '    a.insert(std::make_pair(tt, tt1));\n' +
        '}'
    return value
}

function jsToCParamMap1() {
    let value = 'uint32_t len=pxt->GetMapLength(b);\n' +
        'for(uint32_t i=0;i<len;i++) {\n' +
        '    std::string tt;\n' +
        '    number tt1;\n' +
        '    [replace_swap]\n' +
        '    a.insert(std::make_pair(tt, tt1));\n' +
        '}'
    return value
}

function partOfNamespace(correctResult) {

    it('test gen/generate/namespace generateNamespace', function () {
        let enumElement = [{ name: "name", value: "", type: "string" }];
        let interfaceBody = { function: [], value: [{ name: "age", type: "NUMBER_TYPE_1" }] }
        let data = {
            class: [],
            const: [],
            enum: [{ name: "TestEnum", body: { element: enumElement, function: [], enumValueType: 0 } }],
            export: [],
            function: [{ name: "fun1", type: 1, value: [{ name: "v", type: "Array<string>" }], ret: "string" }],
            interface: [{ name: "TestInterface", body: interfaceBody }],
            namespace: []
        };
        let retJson = JSON.stringify(generateNamespace('napitest', data, inNamespace = ""));
        retJson = re.replaceAll(retJson, " ", "")
        retJson = re.replaceAll(retJson, "\\n", "")
        assert.strictEqual(retJson, correctResult['Generate']['generateNamespace']);
    });

    it('test gen/generate/namespace generateEnumResult', function () {
        let data = {
            "exports": ["GrantStatus"],
            "enum": [{
                "name": "GrantStatus",
                "body": {
                    "element": [
                        { "name": "PERMISSION_DEFAULT", "value": "", "type": "string" }],
                    "function": [],
                    "enumValueType": 1
                }
            }],
            "const": [],
            "type": [],
            "function": [],
            "interface": [],
            "class": [],
            "namespace": []
        };
        let retJson = JSON.stringify(generateEnumResult(data));
        let result = "{\"implH\":\"\\nenum GrantStatus {\\n    PERMISSION_DEFAULT = 0,\\n};\\n\",\"implCpp\":\"\"}";
        assert.strictEqual(retJson, result);
    });

    partOfNamespaceTwo();
}
function partOfNamespaceTwo(){
    it('test gen/generate/namespace generateFunction', function () {
        let func = { name: "fun1", type: 4, value: [{ name: "cb", type: "AsyncCallback<string>" }] };
        let funcValue = [{ name: "cb", type: "AsyncCallback<string>" }]
        let data = {
            class: [],
            const: [],
            enum: [],
            export: [],
            function: [{ name: "fun1", type: 4, value: funcValue, ret: "string" }],
            interface: [],
            namespace: []
        };
        let retJson = JSON.stringify(generateFunction(func, data));
        retJson = re.replaceAll(retJson, "  ", "")
        assert.strictEqual(retJson, correctResult['Generate']['generateFunction']);
    });

    it('test gen/generate/namespace formatMiddleInit', function () {
        let ret = formatMiddleInit('', 'napitest');
        assert.strictEqual(JSON.stringify(ret), '""');
    })
}

function partofParamGenerate(correctResult) {

    it('test gen/generate/param_generate paramGenerateArray', function () {
        let param = {
            valueCheckout: '',
            valueDefine: '',
            valueFill: '',
            valueIn: '',
            valueOut: '',
            valuePackage: ''
        }
        let funcValue = { name: 'v', type: 'Array<string>' }
        paramGenerateArray('0', funcValue, param);
        let retParam = JSON.stringify(param);
        retParam = re.replaceAll(retParam,"  ","");
        retParam = re.replaceAll(retParam,"len[0-9]+","len")  
        retParam = re.replaceAll(retParam,"i[0-9]+","i") 
        retParam = re.replaceAll(retParam,"tt[0-9]+","tt")
        assert.strictEqual(retParam, correctResult['Generate']['paramGenerateArray'])
    });

    it('test gen/generate/param_generate paramGenerateMap', function () {
        let param1 = {
            optionalParamDestory: '',
            valueCheckout: '',
            valueDefine: '',
            valueFill: '',
            valueIn: '',
            valueOut: '',
            valuePackage: ''
        }
        let funcVlaue = { name: 'v', type: '{[key:string]:string}', optional: false };
        paramGenerateMap(funcVlaue, param1, '0');
        let retParam1 = JSON.stringify(param1);
        retParam1 = re.replaceAll(retParam1,"  ","");
        retParam1 = re.replaceAll(retParam1,"len[0-9]+","len")  
        retParam1 = re.replaceAll(retParam1,"i[0-9]+","i") 
        let parmQiepian = retParam1.substring(retParam1.indexOf("tt"),retParam1.indexOf("tt")+3)
        retParam1 = re.replaceAll(retParam1,parmQiepian,"tt")
        retParam1 = re.replaceAll(retParam1,"tt[0-9]+","tt1")
        assert.strictEqual(retParam1, correctResult['Generate']['paramGenerateMap'])
    });

    partmapTempleteFunc()

    it('test gen/generate/param_generate ParamGenerate', function () {
        paramGenerateResult(correctResult);
    });

    it('test gen/generate/return_generate returnGenerate', function () {
        returnGenerateParam(correctResult);
    });
}
function partmapTempleteFunc(){
    it('test gen/generate/param_generate mapTempleteFunc', function () {
        let ret = mapTempleteFunc('vio->in0', 'pxt->GetArgv(0)', '{[key:string]:string}')
        let retJson = JSON.stringify(ret)
        retJson = re.replaceAll(retJson,"  ","");
        retJson = re.replaceAll(retJson,"len[0-9]+","len");
        retJson = re.replaceAll(retJson,"i[0-9]+","i"); 
        let parmQiepian = retJson.substring(retJson.indexOf("tt"),retJson.indexOf("tt")+4)
        retJson = re.replaceAll(retJson,parmQiepian,"kk1");
        retJson = re.replaceAll(retJson,"tt[0-9]+","kk2");
        retJson = re.replaceAll(retJson,"\n","")
        assert.strictEqual(retJson, correctResult['Generate']['mapTempleteFunc'])
    })
}

function returnGenerateParam(correctResult) {
    let retJson = returnGenerateAndAssert("string")
    retJson = re.replaceAll(retJson,"  ","")

    assert.strictEqual(retJson, correctResult['Generate']['returnGenerate'])

    let retJson1 = returnGenerateAndAssert("NUMBER_TYPE_1")
    retJson1 = re.replaceAll(retJson1,"  ","")

    assert.strictEqual(retJson1, correctResult['Generate']['returnGenerate1'])

    let retJson2 = returnGenerateAndAssert("boolean")
    retJson2 = re.replaceAll(retJson2,"  ","")
    assert.strictEqual(retJson2, correctResult['Generate']['returnGenerate2'])

    let retJson3 = returnGenerateAndAssert("Array<string>")
    retJson3 = re.replaceAll(retJson3,"  ","")
    retJson3 = re.replaceAll(retJson3,"len[0-9]*","len")
    retJson3 = re.replaceAll(retJson3,"i[0-9]*","i")
    retJson3 = re.replaceAll(retJson3,"tnv[0-9]*","tnv")

    assert.strictEqual(retJson3, correctResult['Generate']['returnGenerate3'])

    let retJson4 = returnGenerateAndAssert("Array<NUMBER_TYPE_1>")
    retJson4 = re.replaceAll(retJson4,"  ","")
    retJson4 = re.replaceAll(retJson4,"len[0-9]*","len")
    retJson4 = re.replaceAll(retJson4,"i[0-9]*","i")
    retJson4 = re.replaceAll(retJson4,"tnv[0-9]*","tnv")
    assert.strictEqual(retJson4, correctResult['Generate']['returnGenerate4'])

    let retJson5 = returnGenerateAndAssert("Array<boolean>")
    retJson5 = re.replaceAll(retJson5,"  ","")
    retJson5 = re.replaceAll(retJson5,"len[0-9]*","len")
    retJson5 = re.replaceAll(retJson5,"i[0-9]*","i")
    retJson5 = re.replaceAll(retJson5,"tnv[0-9]*","tnv")
    assert.strictEqual(retJson5, correctResult['Generate']['returnGenerate5'])   

    returnGenerateParamTwo(correctResult);
}

function returnGenerateParamTwo(correctResult){

    let retJson6 = returnGenerateAndAssert("string[]")
    retJson6 = re.replaceAll(retJson6,"  ","")
    retJson6 = re.replaceAll(retJson6,"len[0-9]*","len")
    retJson6 = re.replaceAll(retJson6,"i[0-9]*","i")
    retJson6 = re.replaceAll(retJson6,"tnv[0-9]*","tnv")
    assert.strictEqual(retJson6, correctResult['Generate']['returnGenerate6'])

    let retJson7 = returnGenerateAndAssert("boolean[]")
    retJson7 = re.replaceAll(retJson7,"  ","")
    retJson7 = re.replaceAll(retJson7,"len[0-9]*","len")
    retJson7 = re.replaceAll(retJson7,"i[0-9]*","i")
    retJson7 = re.replaceAll(retJson7,"tnv[0-9]*","tnv")
    assert.strictEqual(retJson7, correctResult['Generate']['returnGenerate7'])

    let retJson8 = returnGenerateAndAssert("NUMBER_TYPE_1[]")
    retJson8 = re.replaceAll(retJson8,"  ","")
    retJson8 = re.replaceAll(retJson8,"len[0-9]*","len")
    retJson8 = re.replaceAll(retJson8,"i[0-9]*","i")
    retJson8 = re.replaceAll(retJson8,"tnv[0-9]*","tnv")
    assert.strictEqual(retJson8, correctResult['Generate']['returnGenerate8'])
}

function returnGenerateAndAssert(dataType) {
    param = {
        optionalParamDestory:'',
        valueCheckout:'',
        valueDefine:'',
        valueFill:'',
        valueIn:'',
        valueOut:'',
        valuePackage:''
    }
    data = {
        class: [],
        const: [],
        enum:  [],
        exports: [],
        function: [{name: 'fun1', type: 1, value: [], ret: dataType}],
        interface: [],
        namespace: []
    }
    let returnInfo = { type: dataType, optional: false }
    if (null != data) {
        returnGenerate(returnInfo, param, data)
    } else {
        returnGenerate(returnInfo, param)
    }
    let result = JSON.stringify(param);
    return result
}
function paramGenerateResult(correctResult) {
    let retJson = JSON.stringify(paramGenerateAndAssert("string"))
    retJson = re.replaceAll(retJson,"  ", "")
    assert.strictEqual(retJson, correctResult['Generate']['ParamGenerate'])

    let retJson1 = JSON.stringify(paramGenerateAndAssert("NUMBER_TYPE_1"));
    retJson1 = re.replaceAll(retJson1,"  ","")
    let qiepian = retJson1.substring(retJson1.indexOf("NUMBER_TYPE_"),retJson1.indexOf("NUMBER_TYPE_")+13);
    retJson1 = re.replaceAll(retJson1,qiepian,"NUMBER_TYPE_1")
    assert.strictEqual(retJson1, correctResult['Generate']['ParamGenerate1'])

    let retJson2 = JSON.stringify(paramGenerateAndAssert("boolean"))
    retJson2 = re.replaceAll(retJson2,"  ","")
    assert.strictEqual(retJson2, correctResult['Generate']['ParamGenerate2'])

    let retJson3 = JSON.stringify(paramGenerateAndAssert("Array<string>"))
    retJson3 = re.replaceAll(retJson3,"  ","")
    retJson3 = re.replaceAll(retJson3,"i[0-9]+","i")
    retJson3 = re.replaceAll(retJson3,"len[0-9]+","len")
    retJson3 = re.replaceAll(retJson3,"tt[0-9]+","tt")
    assert.strictEqual(retJson3, correctResult['Generate']['ParamGenerate3'])

    let retJson4 = JSON.stringify(paramGenerateAndAssert("Array<boolean>"))
    retJson4 = re.replaceAll(retJson4,"  ","")
    retJson4 = re.replaceAll(retJson4,"i[0-9]+","i")
    retJson4 = re.replaceAll(retJson4,"len[0-9]+","len")
    retJson4 = re.replaceAll(retJson4,"tt[0-9]+","tt")
    assert.strictEqual(retJson4, correctResult['Generate']['ParamGenerate4'])

    paramGenerateResultTwo(correctResult)
}

function paramGenerateResultTwo(correctResult){
    let retJson5 = JSON.stringify(paramGenerateAndAssert("Array<number>"));
    retJson5 = re.replaceAll(retJson5,"  ","")
    retJson5 = re.replaceAll(retJson5,"i[0-9]+","i")
    retJson5 = re.replaceAll(retJson5,"len[0-9]+","len")
    retJson5 = re.replaceAll(retJson5,"tt[0-9]+","tt")
    assert.strictEqual(retJson5, correctResult['Generate']['ParamGenerate5'])

    let retJson6 = JSON.stringify(paramGenerateAndAssert("string[]"))
    retJson6 = re.replaceAll(retJson6,"  ","")
    retJson6 = re.replaceAll(retJson6,"i[0-9]+","i")
    retJson6 = re.replaceAll(retJson6,"len[0-9]+","len")
    retJson6 = re.replaceAll(retJson6,"tt[0-9]+","tt")
    assert.strictEqual(retJson6, correctResult['Generate']['ParamGenerate6'])

    let retJson7 = JSON.stringify(paramGenerateAndAssert("boolean[]"));
    retJson7 = re.replaceAll(retJson7,"  ","")
    retJson7 = re.replaceAll(retJson7,"i[0-9]+","i")
    retJson7 = re.replaceAll(retJson7,"len[0-9]+","len")
    retJson7 = re.replaceAll(retJson7,"tt[0-9]+","tt")
    assert.strictEqual(retJson7, correctResult['Generate']['ParamGenerate7'])

    let retJson8 = JSON.stringify(paramGenerateAndAssert("number[]"))
    retJson8 = re.replaceAll(retJson8,"  ","")
    retJson8 = re.replaceAll(retJson8,"i[0-9]+","i")
    retJson8 = re.replaceAll(retJson8,"len[0-9]+","len")
    retJson8 = re.replaceAll(retJson8,"tt[0-9]+","tt")
    assert.strictEqual(retJson8, correctResult['Generate']['ParamGenerate8'])
}

function paramGenerateAndAssert(dataType) {
    param = {
        optionalParamDestory:'',
        valueCheckout:'',
        valueDefine:'',
        valueFill:'',
        valueIn:'',
        valueOut:'',
        valuePackage:''
    }
    data = {
        class: [],
        const: [],
        enum:  [],
        exports: [],
        function: [
            {name: 'fun1', 
             type: 1, 
             value: [{name: 'v', type: dataType, optional: false}],
             ret: 'void'}],
        interface: [],
        namespace: []
    }
    let funcValue = { name: "v", type: dataType, optional: false }
    if (null != data) {
        paramGenerate(0, funcValue, param, data)
    } else {
        paramGenerate(0, funcValue, param)
    }
    let result = JSON.stringify(param);
    return result
}