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
import { AsyncCallback, Callback } from './../basic';

declare namespace napitest {
    interface TestClass1 {
        fun11(cb: Callback<number>): void;
        fun12(cb: Callback<void>): void;
        fun13(cb: AsyncCallback<number>): void;
        fun14(cb: AsyncCallback<void>): void;
    }

    function fun1(cb: Callback<number>): void;
    function fun2(cb: Callback<void>): void;
    function fun3(cb: AsyncCallback<number>): void;
    function fun4(cb: AsyncCallback<void>): void;
}

export default napitest;
