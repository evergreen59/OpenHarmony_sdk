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
const test = require("./out/build/Release/napitest")
var assert = require("assert");

describe('[]', function () {
    it('test testArray', function () {
        let ret = test.testArray(['kkk', 'hhh']);
        assert.deepStrictEqual(ret, []);
    });

    it('test testArray1', function () {
        let ret = test.testArray1([15, 18]);
        assert.deepStrictEqual(ret, []);
    });

    it('test testArray2', function () {
        let ret = test.testArray2([true, false]);
        assert.deepStrictEqual(ret, []);
    });

    it('test fun4', function () {
        let ret = test.fun4(
            ['kkk', 'hhh'], [{ 'name': 'kkk', 'age': 18 }, 
            { 'name': 'kkk', 'age': 18 }]);
        assert.strictEqual(JSON.stringify(ret), '[]');
    });

    it('test fun5', function () {
        let ret = test.fun5([12, 18], 
            [{ 'name': 'kkk', 'age': 18 }, { 'name': 'kkk', 'age': 18 }]);
        assert.strictEqual(JSON.stringify(ret), '[]');
    });

    it('test fun6', function () {
        let ret = test.fun6([true, false], 
            [{ 'name': 'kkk', 'age': 18 }, { 'name': 'kkk', 'age': 18 }]);
        assert.strictEqual(JSON.stringify(ret), '[]');
    });

    it('test fun21', function () {
        let ret = test.fun21('sdfghjk',
            [{ key: [11,12],value: ['aa','bb'],isExit: [true,true]},
            { key: [13,14],value: ['cc','dd'],isExit: [false,false]}]);
        assert.strictEqual(JSON.stringify(ret),'[]');
    });
});
