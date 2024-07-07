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
const { TestClass1 } = require("./out/build/Release/napitest")
const test = require("./out/build/Release/napitest")
var assert = require("assert");
const { consumers } = require("stream");

describe('Interface', function () {
    it('test TestClass1 fun1', function () {
        let tc1 = new TestClass1();
        let ret = tc1.fun1(1);
        assert.strictEqual(ret, 0);
    });

    it('test TestClass1 fun2', function () {
        let tc = new TestClass1();
        let ret = tc.fun2([1, 2, 3], { name: 'haha', age: 20 });
        let retJson = JSON.stringify(ret);
        assert.strictEqual(retJson, '{"name":"","age":0}');
    });

    it('test TestClass1 fun3', function () {
        let tc = new TestClass1();
        let ret = tc.fun3(2,'aaa',true);
        assert.strictEqual(ret, false);
    });

    it('test TestClass1 fun4', function () {
        let tc = new test.TestClass1();
        let ret = tc.fun4({ 'name': 'haha', 'age': '20' });
        let retJson = JSON.stringify(ret);
        assert.strictEqual(retJson, '[]');
        ret = tc.fun4({ 'name': 'haha', 'age': '20' },'aaa');
        retJson = JSON.stringify(ret);
        assert.strictEqual(retJson, '[]');
    });
});

describe('Interface', function () {
    it('test TestClass1 fun5', function () {
        let tc = new test.TestClass1();
        let ret = tc.fun5(
            [{ name: 'haha', age: 20 }, { name: 'houhou', age: 23 }]);
        let retJson = JSON.stringify(ret);
        assert.strictEqual(retJson, '{"name":"","age":0}');
    });

    it('test TestClass1 fun6', function () {
        let tc = new test.TestClass1();
        let ret = tc.fun6(['11','22','33'],{'isExit':true,'isTrue':false});
        let retJson = JSON.stringify(ret);
        assert.strictEqual(retJson, '[]');
    });

    it('test TestClass1 fun8', function () {
        let tc = new test.TestClass1();
        let ret = tc.fun8();
        assert.deepStrictEqual(typeof ret, 'undefined');
    });
});



