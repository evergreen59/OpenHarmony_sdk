// @ts-nocheck
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

import Log from "./Log";
import hiSysEvent from '@ohos.hiSysEvent'

const TAG = 'SystemFaultLogger';
const APP_DOMAIN: string = "SYSTEMUI_APP";
const APP_LOG_NAME: string = "SYSTEMUI_FAULT";
export enum FaultID {
  META_DIAGRAM_JUMP = "META_DIAGRAM_JUMP",
  WORKER_ERROR = "WORKER_ABNORMAL_OCCURRENCE",
  NOTIFICATION_ADD = "FAILED_NOTIFICATION_ADD"
}

export function writeFaultLog(logParam: object) {
  const sysEventInfo = {
    domain: APP_DOMAIN,
    name: APP_LOG_NAME,
    eventType: hiSysEvent.EventType.FAULT,
    params: logParam
  }
  Log.showDebug(TAG, "fault log params is : " + JSON.stringify(sysEventInfo))
  hiSysEvent.write(sysEventInfo, (err, val) => {
    Log.showDebug(TAG, "fault log params is : " + JSON.stringify(sysEventInfo))
    Log.showInfo(TAG, `write fault log result: ${val}`)
  })
}

export function SysFaultLogger(logParam: object) {
  return function (target: any, propertyKey: string, descriptor: PropertyDescriptor) {
    const originalFunc = descriptor.value;
    descriptor.value = function(...args) {
      try {
        originalFunc.apply(this, args);
      }  catch (err: any) {
        Log.showInfo(TAG, "catch error in execute: " + propertyKey);
        writeFaultLog(logParam);
      }
    };
  };
}