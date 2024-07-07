/**
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

import Log from './Log';
import audio from '@ohos.multimedia.audio';

const TAG = "SingleInstanceHelper";
const AUDIO_MANAGER_KEY = 'MultiMediaAudioManager';

export default function createOrGet<T>(objectClass: { new(): T }, storageKey: string): T {
  if (!globalThis[storageKey]) {
    globalThis[storageKey] = new objectClass();
    Log.showDebug(TAG, `Create key of ${storageKey}`);
  }
  return globalThis[storageKey];
}

export function getAudioManager() {
  if (!globalThis[AUDIO_MANAGER_KEY]) {
    globalThis[AUDIO_MANAGER_KEY] = audio.getAudioManager();
    Log.showInfo(TAG, `Create Audio Manager.`);
  }
  return globalThis[AUDIO_MANAGER_KEY];
}
