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
    export enum GrantStatus {
        PERMISSION_DEFAULT = "",
        PERMISSION_DENIED = "-1",
        PERMISSION_GRANTED = "2",
        PERMISSION_PASS = "3",
    }

    export enum LaunchReason {
        UNKNOWN = 0,
        START_ABILITY = 1,
        CALL = 2,
        CONTINUATION = 3,
    }

    export enum Action {
        ACTION_HOME = "ohos.want.action.home",
        ACTION_DIAL = "ohos.want.action.dial",
        ACTION_SEARCH = "ohos.want.action.search",
    }

    export enum PlayingState {
        STATE_NOT_PLAYING,
        STATE_PLAYING,
    }
    function fun1(v0: string, v1: GrantStatus): GrantStatus;
    function fun2(v0: number, v1: LaunchReason): LaunchReason;
    function fun3(v0: string, v1: Action): Action;
    function fun4(v0: number, v1: PlayingState): PlayingState;
    function fun5(V0: string, callback: Callback<LaunchReason>): void;
    function fun6(v0: string, callback: AsyncCallback<GrantStatus>): void;
    function fun6(v0: string): Promise<GrantStatus>;
    /*function fun7(v0: LaunchReason[], v1: LaunchReason): number;*/

}

export default napitest;

