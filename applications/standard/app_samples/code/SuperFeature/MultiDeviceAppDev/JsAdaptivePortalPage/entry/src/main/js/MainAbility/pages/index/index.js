/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

import mediaquery from '@ohos.mediaquery';

export default {
  data: {
    subTabList: ["子页签", "页签", "页签", "页签", "页签", "页签", "页签", "页签", "页签", "页签", "页签", "页签", "页签", "页签", "页签", "页签", "页签", "页签", "页签", "页签", "页签"],
    listOne: Array(5).fill(0).map((_, index) => ({
      uri: "", title: "item", id: "item_" + index
    })),
    listTwo: Array(8).fill(0).map((_, index) => ({
      uri: "", title: "item " + index, id: "item_" + index
    })),
    listThree: Array(3).fill(0).map((_, index) => ({
      msg1: "The End Of The World", msg2: "Justin Timberland", id: "item_" + index
    })),
    tabBars: [
      {
        icon: "icon_person_blue.png", text: "页签"
      },
      {
        icon: "icon_person_gray.png", text: "页签"
      },
      {
        icon: "icon_person_gray.png", text: "页签"
      },
      {
        icon: "icon_person_gray.png", text: "页签"
      },
      {
        icon: "icon_person_gray.png", text: "页签"
      },
    ],
    isTablet: false,
    menu_tabs_index: 0
  },
  onInit() {
    mediaquery.matchMediaSync('screen and (min-aspect-ratio: 1.5) or (orientation: landscape)').on("change", (data) => {
      this.isTablet = data.matches
    })
  }
}