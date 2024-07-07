/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { Log } from '../utils/Log';
import { Broadcast } from '../utils/Broadcast';
import createOrGet from '../utils/SingleInstanceUtils';

const TAG = "BroadcastManager"

export class BroadcastManager {
    // The global Broadcast of the application process. Event registration and destruction should be paired
    private appBroadcast: Broadcast;

    constructor() {
        Log.info(TAG, 'constructor');
        this.appBroadcast = new Broadcast();
    }

    public getBroadcast(): Broadcast {
        return this.appBroadcast;
    }
}

let broadcastManager = createOrGet(BroadcastManager, TAG);

export default broadcastManager as BroadcastManager;
