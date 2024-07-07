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
    function fun1(v: string): string;
    function fun2(v1: string, v2: string[]): string[];
    function fun3(v1: Array<string>, v2: string): Array<string>;
    function fun4(v: { [key: string]: string }): string;
    function fun5(v1: Map<string, string>, v2: string): string;
    function fun6(v1: string, callback: AsyncCallback<string>): void;
    function fun6(v1: string): Promise<string>;
    function fun7(v: string, v1: AsyncCallback<Array<string>>): void;
    function fun7(v: string): Promise<Array<string>>;
    function fun9(v1: string, callback: Callback<string>): void;
    function fun10(v1: Test): Test;
    function fun11(v: string, v1: string, v2: string): void;
    interface Test {
        age: string;
        height: string[];
        width: Array<string>;
    }
    /*
    function fun11(v1: Test1): Test;
    interface Test1 {
        address: { [key: string]: string };
        long: Map<string, string>;
    }
    function fun8(v1: string, callback: AsyncCallback<string[]>): void;
    function fun8(v1: string): Promise<string[]>;*/
}

export default napitest;

