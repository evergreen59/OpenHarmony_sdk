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

import { DialogDataModel } from '../model/ShareModel'

export const dialogData: Array<DialogDataModel> = [
  {
    src: $r('app.media.chat'),
    text: $r('app.string.chat'),
    id: 0
  },
  {
    src: $r('app.media.wChat'),
    text: $r('app.string.video_sharing'),
    id: 1
  },
  {
    src: $r('app.media.line'),
    text: $r('app.string.copy_link'),
    id: 2
  },
  {
    src: $r("app.media.canvas"),
    text: $r('app.string.generate_poster'),
    id: 3
  }
]