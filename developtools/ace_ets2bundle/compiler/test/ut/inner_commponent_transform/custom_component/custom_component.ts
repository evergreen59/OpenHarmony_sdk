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
@Component
@Entry
struct MyComponent {
  build() {
    Column() {
      Banner()
      Banner()
       .width(100)
      Banner()
       .width(100)
       .height(200)
      Banner({value: "Hello"})
      Banner({value: "Hello"})
       .width(100)
      Banner({value: "Hello"})
       .width(100)
       .height(200)
    }
  }
}

@Component
struct Banner {
  value: string = "Hello"
  build() {
    Column() {
      Text(this.value)
    }
  }
}`

exports.expectResult =
`"use strict";
let __generate__Id = 0;
function generateId() {
    return "custom_component_" + ++__generate__Id;
}
class MyComponent extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
    }
    aboutToBeDeleted() {
        SubscriberManager.Get().delete(this.id());
    }
    render() {
        Column.create();
        let earlierCreatedChild_2 = (this && this.findChildById) ? this.findChildById("2") : undefined;
        if (earlierCreatedChild_2 == undefined) {
            View.create(new Banner("2", this, {}));
        }
        else {
            earlierCreatedChild_2.updateWithValueParams({});
            if (!earlierCreatedChild_2.needsUpdate()) {
                earlierCreatedChild_2.markStatic();
            }
            View.create(earlierCreatedChild_2);
        }
        __Common__.create();
        __Common__.width(100);
        let earlierCreatedChild_3 = (this && this.findChildById) ? this.findChildById("3") : undefined;
        if (earlierCreatedChild_3 == undefined) {
            View.create(new Banner("3", this, {}));
        }
        else {
            earlierCreatedChild_3.updateWithValueParams({});
            if (!earlierCreatedChild_3.needsUpdate()) {
                earlierCreatedChild_3.markStatic();
            }
            View.create(earlierCreatedChild_3);
        }
        __Common__.pop();
        __Common__.create();
        __Common__.width(100);
        __Common__.height(200);
        let earlierCreatedChild_4 = (this && this.findChildById) ? this.findChildById("4") : undefined;
        if (earlierCreatedChild_4 == undefined) {
            View.create(new Banner("4", this, {}));
        }
        else {
            earlierCreatedChild_4.updateWithValueParams({});
            if (!earlierCreatedChild_4.needsUpdate()) {
                earlierCreatedChild_4.markStatic();
            }
            View.create(earlierCreatedChild_4);
        }
        __Common__.pop();
        let earlierCreatedChild_5 = (this && this.findChildById) ? this.findChildById("5") : undefined;
        if (earlierCreatedChild_5 == undefined) {
            View.create(new Banner("5", this, { value: "Hello" }));
        }
        else {
            earlierCreatedChild_5.updateWithValueParams({
                value: "Hello"
            });
            if (!earlierCreatedChild_5.needsUpdate()) {
                earlierCreatedChild_5.markStatic();
            }
            View.create(earlierCreatedChild_5);
        }
        __Common__.create();
        __Common__.width(100);
        let earlierCreatedChild_6 = (this && this.findChildById) ? this.findChildById("6") : undefined;
        if (earlierCreatedChild_6 == undefined) {
            View.create(new Banner("6", this, { value: "Hello" }));
        }
        else {
            earlierCreatedChild_6.updateWithValueParams({
                value: "Hello"
            });
            if (!earlierCreatedChild_6.needsUpdate()) {
                earlierCreatedChild_6.markStatic();
            }
            View.create(earlierCreatedChild_6);
        }
        __Common__.pop();
        __Common__.create();
        __Common__.width(100);
        __Common__.height(200);
        let earlierCreatedChild_7 = (this && this.findChildById) ? this.findChildById("7") : undefined;
        if (earlierCreatedChild_7 == undefined) {
            View.create(new Banner("7", this, { value: "Hello" }));
        }
        else {
            earlierCreatedChild_7.updateWithValueParams({
                value: "Hello"
            });
            if (!earlierCreatedChild_7.needsUpdate()) {
                earlierCreatedChild_7.markStatic();
            }
            View.create(earlierCreatedChild_7);
        }
        __Common__.pop();
        Column.pop();
    }
}
class Banner extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.value = "Hello";
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.value !== undefined) {
            this.value = params.value;
        }
    }
    aboutToBeDeleted() {
        SubscriberManager.Get().delete(this.id());
    }
    render() {
        Column.create();
        Text.create(this.value);
        Text.pop();
        Column.pop();
    }
}
loadDocument(new MyComponent("1", undefined, {}));
`