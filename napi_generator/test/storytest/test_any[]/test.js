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
const { fun1, fun2, fun4, fun5 } = require("./out/build/Release/napitest")
var assert = require("assert");

describe('any[]', function () {
    it('test fun1', function () {
        let ret = fun1(['a', 'b', 'c', 'd']);
        assert.strictEqual(ret, 0);
    });

    it('test fun1', function () {
        let ret = fun1([1, 2, 3, 4]);
        assert.strictEqual(ret, 0);
    });

    it('test fun2', function () {
        let ret = fun2([true, true, false, false]);
        assert.strictEqual(ret, 0);
    });

    it('test fun4', function () {
        let ret = fun4(['a', 'b', 'c', 'd'], 0);
        assert.strictEqual(ret, 0);
    });

    it('test fun4_1', function () {
        let ret = fun4([1, 2, 3, 4], 0);
        assert.strictEqual(ret, 0);
    });

    it('test fun5', function () {
        let ret = fun5(0, ['a', 'b', 'c', 'd']);
        assert.strictEqual(ret, 0);
    });
});

