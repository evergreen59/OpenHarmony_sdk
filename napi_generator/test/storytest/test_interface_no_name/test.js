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
const { fun1,fun2, fun3, fun4, fun5, fun6, fun7} = require("./out/build/Release/napitest")
var assert = require("assert");

describe('interface_no_name', function () {    
    let fp2 = {
        anchor:"abc",
        align:"testfun2", 
        left:{
            test1:"fun2", 
            test2: "t2"
        }
    }
    let value = {
        xOffset: 100, 
        animation: { 
            duration: 20,
            curve: "tst"
        }
    }

    it('test interface_no_name fun1', function () {        
        let ret = fun1({"name":"abc","age":20});
        assert.strictEqual(ret, '');
    });
    it('test interface_no_name fun2', function () {        
        let ret = fun2(fp2);
        //assert.strictEqual(ret, '');
    });    
    
    it('test interface_no_name fun3', function () {        
        let ret = fun3("fun3p1", {nm:"abc",age:20});
        //assert.strictEqual(ret, '');
    });
    
    it('test interface_no_name fun4', function () {        
        let ret = fun4("name");
        //assert.strictEqual(ret.read, 0);
    });
    
    it('test interface_no_name fun5', function () {
        fun5(value);   
        //let ret = fun5(value);
        //assert.strictEqual(ret, void);
    });
    it('test interface_no_name fun6', function () {     
        fun6("name", value);

    });
    it('test interface_no_name fun7', function () {
        fun7("name", value);
    });
});

