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
const re = require(genDir + "tools/re");
const { analyzeFile } = require(genDir + "analyze");
const { analyzeEnum, analyzeEnumResult } = require(genDir + "analyze/enum");
const { analyzeFunction, analyzeSubInterface, getFuncParaType } = require(genDir + "analyze/function");
const { analyzeInterface } = require(genDir + "analyze/interface");
const { analyzeNamespace, parseNamespace } = require(genDir + "analyze/namespace");
const { parseEnum, parseFunction, parseInterface } = require(genDir + "analyze/namespace");
const { analyzeParams } = require(genDir + "analyze/params");
const { analyzeReturn } = require(genDir + "analyze/return");
const { readFile } = require("../../src/gen/tools/FileRW");

var assert = require("assert");
var correctResult;
function before() {
    let data = readFile("test/unittest/result.json")
    if (data) {
        correctResult = JSON.parse(data);
    }
}

describe('Analyze', function () {
    before(function () {
        before();
    });

    it('test gen/analyze analyzeFile', function () {
        let structOfTs = analyzeFile("test/unittest/@ohos.input_sample.d.ts");
        let retJson = JSON.stringify(structOfTs)
        let retNameSpace = retJson.search("\"name\":\"napitest\"");
        assert.strictEqual(retNameSpace > 0, true);
        let retLicense = retJson.search("Copyright");
        assert.strictEqual(retLicense > 0, true);
    });

    partOfEnum();

    partOfFunction(correctResult);

    it('test gen/analyze/interface analyzeInterface', function () {
        let ret = analyzeInterface(correctResult['ParamIn']['analyzeInterface']);
        let retJson = JSON.stringify(ret)
        let valueArray = retJson.substring(retJson.indexOf("\[") + 1, retJson.indexOf("\]")).split("}");
        let numContain = valueArray[0].indexOf("\"name\":\"num1\",\"type\":\"NUMBER_TYPE_");
        assert.strictEqual(numContain > 0, true);
        assert.strictEqual(valueArray[1], ",{\"name\":\"str1\",\"type\":\"string\"");
        let numArrayCon = valueArray[2].indexOf("\"name\":\"nums\",\"type\":\"Array<NUMBER_TYPE_");
        assert.strictEqual(numArrayCon > 0, true);
        assert.strictEqual(valueArray[3], ",{\"name\":\"strs\",\"type\":\"Array<string>\"");
        let asyncExit = retJson.search("\"name\":\"if_async\",\"type\":4")
        assert.strictEqual(asyncExit > 0, true);
        let asyncArray = retJson.substring(retJson.lastIndexOf("\[") + 1, retJson.lastIndexOf("\]")).split("}");
        assert.strictEqual(asyncArray[0], "{\"name\":\"v1\",\"type\":\"string\",\"optional\":false");
        assert.strictEqual(asyncArray[1], ",{\"name\":\"cb\",\"type\":\"AsyncCallback<string>\",\"optional\":false");
        assert.strictEqual(asyncArray[2], "],\"ret\":\"string\"");
    });

    partOfNamespace(correctResult);

    partOfParam();

    it('test gen/analyze/return analyzeReturn', function () {
        let ret = analyzeReturn("string");
        assert.strictEqual(ret[0], 'string');
        assert.strictEqual(ret[1], false);
    });

    it('test gen/analyze/return analyzeReturn', function () {
        let ret = analyzeReturn("Promise<string>");
        assert.strictEqual(ret[0], 'Promise<string>');
        assert.strictEqual(ret[1], true);
    });

});

function partOfEnum() {
    it('test gen/analyze/enum analyzeNumberEnum', function () {
        let data = '\nDENIED = "-1"';
        let retJson = JSON.stringify(analyzeEnum(data));
        let enumValueType = retJson.search("\"enumValueType\":1");
        assert.strictEqual(enumValueType > 0, true);
        let element = retJson.search("\"name\":\"DENIED\",\"value\":\"-1\",\"type\":\"string\"");
        assert.strictEqual(element > 0, true);
    });

    it('test gen/analyze/enum analyzeStringEnum', function () {
        let data = '\nFAULT = 1,\nSTATISTIC = 2,\nSECURITY = 3,\nBEHAVIOR = 4,\n';
        let retJson = JSON.stringify(analyzeEnum(data));
        let enumValueType = retJson.search("\"enumValueType\":0");
        assert.strictEqual(enumValueType > 0, true);
        let element = retJson.search("\"name\":\"FAULT\",\"value\":\"1\",\"type\":\"NUMBER_TYPE_");
        assert.strictEqual(element > 0, true);
        let retFunc = retJson.substring(retJson.search("function") - 1, retJson.search("function") + 12);
        assert.strictEqual(retFunc, "\"function\":[]");
    });

    it('test gen/analyze/enum analyzeEnumStringResult', function () {
        let result = {
            element: [{ name: "STATUS0", value: "0", type: "string" }],
            function: [],
            enumValueType: 0
        }
        let retJson = JSON.stringify(analyzeEnumResult(result, 'STATUS0 = "1"', '0'))
        let enumValueType = retJson.search("\"enumValueType\":1");
        assert.strictEqual(enumValueType > 0, true);
    });

}

function partOfFunction(correctResult) {
    it('test gen/analyze/function analyzeSubInterface', function () {
        let data = correctResult['ParamIn']['analyzeSubInterface'];
        let retJson = JSON.stringify(analyzeSubInterface(data))
        let retNum = retJson.search("{\"name\":\"num1\",\"type\":\"NUMBER_TYPE_");
        assert.strictEqual(retNum > 0, true);
        let retString = retJson.search("{\"name\":\"str1\",\"type\":\"string\"}");
        assert.strictEqual(retString > 0, true);
        let retArrayNum = retJson.search("{\"name\":\"nums\",\"type\":\"Array<NUMBER_TYPE_");
        assert.strictEqual(retArrayNum > 0, true);
        let retArrayString = retJson.search("{\"name\":\"strs\",\"type\":\"Array<string>\"}");
        assert.strictEqual(retArrayString > 0, true);
    });

    it('test gen/analyze/function getFuncParaType', function () {
        let data = 'if_direct(v1: string, v2: boolean): string;';
        let v = { name: 'v1', type: 'string' };
        let retJson = JSON.stringify(getFuncParaType(v, '', data))
        assert.strictEqual(retJson, "{\"name\":\"v1\",\"type\":\"string\"}");
    });

    it('test gen/analyze/function analyzeDirectFunction', function () {
        let data = "if_direct(v1: string, v2: boolean): string;";
        let ret = analyzeFunction(data, false, `if_direct`, "v1: string, v2: boolean", "asdfgh");
        let retJson = JSON.stringify(ret)
        let str1 = "{\"name\":\"v1\",\"type\":\"string\",\"optional\":false},";
        let str2 = "{\"name\":\"v2\",\"type\":\"boolean\",\"optional\":false}";
        let retValue = retJson.search(str1 + str2)
        assert.strictEqual(retValue > 0, true);
        let retName = retJson.search("\"name\":\"if_direct\"")
        assert.strictEqual(retName > 0, true);
        let retType = retJson.search("\"type\":1")
        assert.strictEqual(retType > 0, true);
    });

    partOfFunctionTwo();
}

function partOfFunctionTwo() {

    it('test gen/analyze/function analyzeAsyncFunction', function () {
        let data = "if_async(v1: string, cb: AsyncCallback<string>): string;";
        let ret = analyzeFunction(data, false, `if_async`, "v1: string, cb: AsyncCallback<string>", "qwerty");
        let retJson = JSON.stringify(ret)
        let str1 = "{\"name\":\"v1\",\"type\":\"string\",\"optional\":false},";
        let str2 = "{\"name\":\"cb\",\"type\":\"AsyncCallback<string>\",\"optional\":false}";
        let retValue = retJson.search(str1 + str2)
        assert.strictEqual(retValue > 0, true);
        let retName = retJson.search("\"name\":\"if_async\"")
        assert.strictEqual(retName > 0, true);
        let retType = retJson.search("\"type\":4")
        assert.strictEqual(retType > 0, true);
    });

    it('test gen/analyze/function analyzeSyncFunction', function () {
        let data = "if_callback(v1: string, cb: Callback<Array<string>>): string;";
        let ret = analyzeFunction(data, false, `if_callback`, "v1: string, cb: Callback<Array<string>>", "zxcvbn");
        let retJson = JSON.stringify(ret)
        let retType = retJson.search("\"type\":2")
        assert.strictEqual(retType > 0, true);
    });

    it('test gen/analyze/function analyzePromiseFunction', function () {
        let data = "if_promise(v1: Array<number>): Promise<boolean>;";
        let ret = analyzeFunction(data, false, `if_promise`, "v1: Array<number>", "Promise<boolean>");
        assert.strictEqual(ret, null);
    });
}

function partOfNamespace(correctResult) {
    it('test gen/analyze/namespace analyzeNamespace', function () {
        let ret = analyzeNamespace(correctResult['ParamIn']['analyzeNamespace']);
        let retJson = JSON.stringify(ret);
        let nameResult = retJson.substring(retJson.search("namespace") - 1, retJson.length - 1);
        let searchExport = nameResult.search("\"exports\"")
        let exportResult = nameResult.substring(searchExport, nameResult.search("\"exports\"") + 20);
        assert.strictEqual(exportResult, "\"exports\":[\"Entity\"]")
        let enumResult = nameResult.substring(nameResult.search("\"enum\""), nameResult.indexOf("\"const\"") - 1);
        assert.strictEqual(enumResult.search("\"name\":\"Entity\"") > 0, true);
        assert.strictEqual(enumResult.search("\"enumValueType\":0") > 0, true);
        let searchInte = nameResult.indexOf("\"interface\"")
        let interResult = nameResult.substring(searchInte, nameResult.indexOf("\"class\"") - 1);
        assert.strictEqual(interResult.search("{\"name\":\"animal\",\"type\":\"string\"}") > 0, true);
        let interFun = interResult.substring(interResult.search("function") - 1, interResult.length - 3);
        let interValue = "\"value\":[{\"name\":\"v1\",\"type\":\"string\",\"optional\":false}],";
        let interRet = "\"ret\":\"string\"}]"
        assert.strictEqual(interFun, "\"function\":[{\"name\":\"fix\",\"type\":1," + interValue + interRet);

    });
    
    it('test gen/analyze/namespace analyzeNamespaceClass', function () {
        let ret = analyzeNamespace('\nnamespace Space3 {\nclass TestClass {\nstatic $fun1(v:string):boolean;\n}\n}\n');
        let retJson = JSON.stringify(ret);
        let nameResult = retJson.substring(retJson.search("namespace"), retJson.length - 2)
        let qiePianStart = nameResult.lastIndexOf("\"class\"") - 1
        let classResult = nameResult.substring(qiePianStart, nameResult.lastIndexOf("\"namespace\"") - 2)
        assert.strictEqual(classResult.search("\"functiontType\":\"static\"") > 0, true)
    });

    it('test gen/analyze/namespace analyzeNamespaceFunction', function () {
        let ret = analyzeNamespace('\nnamespace Space3 {\nfunction fun1(v: string): boolean;\n}\n');
        let retJson = JSON.stringify(ret);
        let nameResult = retJson.substring(retJson.search("namespace"), retJson.length - 2)
        let qiePianStart = nameResult.lastIndexOf("\"function\"");
        let funcResult = nameResult.substring(qiePianStart, nameResult.lastIndexOf("\"interface\"") - 2);
        assert.strictEqual(funcResult.search("\"name\":\"fun1\",\"type\":1") > 0, true)
        assert.strictEqual(funcResult.search("{\"name\":\"v\",\"type\":\"string\",\"optional\":false}") > 0, true)
    });

    partOfNamespaceTwo(correctResult);

    partOfNamespaceThere(correctResult);
}

function partOfNamespaceTwo(correctResult) {
    it('test gen/analyze/namespace parseNamespace', function () {
        let data = correctResult['ParamIn']['parseNamespace'];
        let matchs = re.match(" *\n*", data)
        let result = {
            exports: [],
            enum: [],
            const: [],
            type: [],
            function: [],
            interface: [],
            class: [],
            namespace: [],
        }
        let ret = parseNamespace(matchs, data, result);
        let retJson = JSON.stringify(ret)
        assert.strictEqual(retJson, "\"\\n\"");
    });

    it('test gen/analyze/namespace parseEnum', function () {
        let data = correctResult['ParamIn']['parseEnum']
        let matchs = re.match(" *\n*", data)
        let result = {
            exports: [],
            enum: [],
            const: [],
            type: [],
            function: [],
            interface: [],
            class: [],
            namespace: [],
        }
        let ret = parseEnum(matchs, data, result);
        let retJson = JSON.stringify(ret)
        assert.strictEqual(retJson, "\"\\n\"");
    })
}

function partOfNamespaceThere(correctResult) {
    it('test gen/analyze/namespace parseFunction', function () {
        let data = 'function fun1(name: string, flags: number): boolean;\n';
        let matchs = re.match(" *\n*", data)
        let result = {
            exports: [],
            enum: [],
            const: [],
            type: [],
            function: [],
            interface: [],
            class: [],
            namespace: [],
        }
        let ret = parseFunction(matchs, data, result);
        let retJson = JSON.stringify(ret)
        assert.strictEqual(retJson, "\"\"");
    });

    it('test gen/analyze/namespace parseInterface', function () {
        let data = correctResult['ParamIn']['parseInterface']
        let matchs = re.match(" *\n*", data)
        let result = {
            exports: [],
            enum: [],
            const: [],
            type: [],
            function: [],
            interface: [],
            class: [],
            namespace: [],
        }
        let ret = parseInterface(matchs, data, result);
        let retJson = JSON.stringify(ret)
        assert.strictEqual(retJson, "\"\\nfunction fun1(v: ConfigOption): void\\n\"");
    });
}

function partOfParam() {
    it('test gen/analyze/params analyzeDirectParams', function () {
        let ret = analyzeParams('', 'v1:string,v2:boolean');
        let retJson = JSON.stringify(ret)
        let retJsonpian = retJson.substring(2, retJson.length - 4).split("}");
        assert.strictEqual(retJsonpian[0].indexOf("\"name\":\"v1\",\"type\":\"string\",\"optional\":false") > 0, true);
        assert.strictEqual(retJsonpian[1].indexOf("\"name\":\"v2\",\"type\":\"boolean\",\"optional\":false") > 0, true);
        assert.strictEqual(ret[1], 1);
    });

    it('test gen/analyze/params analyzeAsynctParams', function () {
        let ret = analyzeParams('', 'v2:string,cb:AsyncCallback<string>');
        let retJson = JSON.stringify(ret)
        let retJsonpian = retJson.substring(2, retJson.length - 4).split("}");
        assert.strictEqual(retJsonpian[0].indexOf("\"name\":\"v2\",\"type\":\"string\",\"optional\":false") > 0, true);
        let flag = retJsonpian[1].indexOf("\"name\":\"cb\",\"type\":\"AsyncCallback<string>\",\"optional\":false") > 0
        assert.strictEqual(flag, true);
        assert.strictEqual(ret[1], 4);
    });

    it('test gen/analyze/params analyzeSynctParams', function () {
        let ret = analyzeParams('', 'v2:boolean,cb:Callback<boolean>');
        let retJson = JSON.stringify(ret)
        let retJsonpian = retJson.substring(2, retJson.length - 4).split("}");
        assert.strictEqual(retJsonpian[0].indexOf("\"name\":\"v2\",\"type\":\"boolean\",\"optional\":false") > 0, true);
        let flag = retJsonpian[1].indexOf("\"name\":\"cb\",\"type\":\"Callback<boolean>\",\"optional\":false") > 0
        assert.strictEqual(flag, true);
        assert.strictEqual(ret[1], 2);
    });

    it('test gen/analyze/params analyzeArrayParams', function () {
        let ret = analyzeParams('', "v1: Array<number>,v2:Map<string,boolean>");
        let retJson = JSON.stringify(ret)
        let retJsonpian = retJson.substring(2, retJson.length - 4).split("}");
        let flagArray = retJsonpian[0].indexOf("\"name\":\"v1\",\"type\":\"Array<number>\",\"optional\":false") > 0
        assert.strictEqual(flagArray, true);
        let flag = retJsonpian[1].indexOf("\"name\":\"v2\",\"type\":\"Map<string,boolean>\",\"optional\":false") > 0
        assert.strictEqual(flag, true);
    });

    it('test gen/analyze/params analyzeMapParams', function () {
        let ret = analyzeParams('', "v1: string[],v2:{[key:string]:boolean}");
        let retJson = JSON.stringify(ret)
        let retJsonpian = retJson.substring(2, retJson.length - 4);
        assert.strictEqual(retJsonpian.indexOf("\"name\":\"v2\",\"type\":\"{\[key:string\]:boolean}\"") > 0, true);
        assert.strictEqual(retJsonpian.indexOf("\"name\":\"v1\",\"type\":\"string[]\",\"optional\":false") > 0, true);
    });

}

