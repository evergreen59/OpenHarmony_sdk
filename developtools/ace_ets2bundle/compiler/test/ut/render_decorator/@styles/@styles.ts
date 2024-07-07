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

exports.source = `
@Styles function globalFancy() {
  .backgroundColor(Color.Red)
}

@Entry
@Component
struct FancyUse {
  @State enable: boolean = true
  @Styles componentFancy() {
    .backgroundColor(Color.Blue)
  }
  build() {
    Column({ space: 10 }) {
      Text("Fancy")
        .globalFancy()
        .width(100)
        .height(100)
      Text("Fancy")
        .componentFancy()
        .width(100)
        .height(100)
      Button() {
        Text("Fancy")
      }
      .enabled(this.enable)
      .onClick(() => {
        this.enable = false
      })
      .stateStyles({
        normal: {
          .backgroundColor(Color.Green)
        },
        disabled: this.componentFancy,
        pressed: globalFancy
      })
    }
  }
}`

exports.expectResult =
`"use strict";
let __generate__Id = 0;
function generateId() {
    return "@styles_" + ++__generate__Id;
}
class FancyUse extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__enable = new ObservedPropertySimple(true, this, "enable");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.enable !== undefined) {
            this.enable = params.enable;
        }
    }
    aboutToBeDeleted() {
        this.__enable.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get enable() {
        return this.__enable.get();
    }
    set enable(newValue) {
        this.__enable.set(newValue);
    }
    render() {
        Column.create({ space: 10 });
        Text.create("Fancy");
        Text.backgroundColor(Color.Red);
        Text.width(100);
        Text.height(100);
        Text.pop();
        Text.create("Fancy");
        Text.backgroundColor(Color.Blue);
        Text.width(100);
        Text.height(100);
        Text.pop();
        Button.createWithChild();
        Button.enabled(this.enable);
        Button.onClick(() => {
            this.enable = false;
        });
        ViewStackProcessor.visualState("normal");
        Button.backgroundColor(Color.Green);
        ViewStackProcessor.visualState("disabled");
        Button.backgroundColor(Color.Blue);
        ViewStackProcessor.visualState("pressed");
        Button.backgroundColor(Color.Red);
        ViewStackProcessor.visualState();
        Text.create("Fancy");
        Text.pop();
        Button.pop();
        Column.pop();
    }
}
loadDocument(new FancyUse("1", undefined, {}));
`
