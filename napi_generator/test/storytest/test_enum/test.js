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
const { fun1, fun2, fun3 } = require("./out/build/Release/napitest")
const { fun4, fun5, fun6 } = require("./out/build/Release/napitest")
const test = require("./out/build/Release/napitest")
var assert = require("assert");

describe('enum', function () {
    it('test fun1', function () {
        let ret = fun1('aaaa', test.GrantStatus.PERMISSION_DENIED);
        assert.strictEqual(ret, test.GrantStatus.PERMISSION_DEFAULT);
    });

    it('test fun2', function () {
        let ret = fun2(18, test.LaunchReason.START_ABILITY);
        assert.strictEqual(ret, test.LaunchReason.UNKNOWN);
    });

    it('test fun3', function () {
        let ret = fun3('ggg', test.Action.ACTION_DIAL);
        assert.strictEqual(ret, '');
    });

    it('test fun4', function () {
        let ret = fun4(18, test.PlayingState.STATE_PLAYING);
        assert.strictEqual(ret, 0);
    });

    function abc(ret) {
        assert.deepStrictEqual(ret, test.LaunchReason.UNKNOWN);
    }

    it('test fun5', function () {
        fun5('aaa', abc);
    });

    function asynFun1(err, ret) {
        assert.deepStrictEqual(err.code, 0)
        assert.deepStrictEqual(ret, '')
    }

    it('test fun6', function () {
        fun6('hhh', asynFun1);
        fun6('hhh').then(def1);
    });

    function def1(ret) {
        assert.deepStrictEqual(ret, '');
    }

    it('test fun6', function () {
        let promiseObj = fun6('hhh');
        promiseObj.then(ret => { def1(ret) });
    });
});

