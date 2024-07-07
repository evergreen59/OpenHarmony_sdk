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
class Model {
  value: string
  constructor(value: string) {
    this.value = value
  }
}
@Component
struct MyComponent {
  @State title: Model = {value: 'Hello World'}
  @State count: number = 0
  private toggle: string = 'Hello World'
  private increaseBy: number = 1

  build() {
    Column() {
      Text('title.value: ' + this.title.value)
      Button() {
        Text('Click to change title')
      }.onClick(() => {
        this.title.value = (this.toggle == this.title.value) ? 'Hello World' : 'Hello UI'
      })

      Button() {
        Text('Click to increase count=' + this.count)
      }.onClick(() => {
        this.count += this.increaseBy
      })
    }
  }
}
@Entry
@Component
struct EntryComponent {
  build() {
    Column() {
      MyComponent({count: 1, increaseBy: 2})
      MyComponent({title: {value: 'Hello, World 2'}, count: 7})
    }
  }
}
`
exports.expectResult =
`"use strict";
let __generate__Id = 0;
function generateId() {
    return "@state_" + ++__generate__Id;
}
class Model {
    constructor(value) {
        this.value = value;
    }
}
class MyComponent extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__title = new ObservedPropertyObject({ value: 'Hello World' }, this, "title");
        this.__count = new ObservedPropertySimple(0, this, "count");
        this.toggle = 'Hello World';
        this.increaseBy = 1;
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.title !== undefined) {
            this.title = params.title;
        }
        if (params.count !== undefined) {
            this.count = params.count;
        }
        if (params.toggle !== undefined) {
            this.toggle = params.toggle;
        }
        if (params.increaseBy !== undefined) {
            this.increaseBy = params.increaseBy;
        }
    }
    aboutToBeDeleted() {
        this.__title.aboutToBeDeleted();
        this.__count.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get title() {
        return this.__title.get();
    }
    set title(newValue) {
        this.__title.set(newValue);
    }
    get count() {
        return this.__count.get();
    }
    set count(newValue) {
        this.__count.set(newValue);
    }
    render() {
        Column.create();
        Text.create('title.value: ' + this.title.value);
        Text.pop();
        Button.createWithChild();
        Button.onClick(() => {
            this.title.value = (this.toggle == this.title.value) ? 'Hello World' : 'Hello UI';
        });
        Text.create('Click to change title');
        Text.pop();
        Button.pop();
        Button.createWithChild();
        Button.onClick(() => {
            this.count += this.increaseBy;
        });
        Text.create('Click to increase count=' + this.count);
        Text.pop();
        Button.pop();
        Column.pop();
    }
}
class EntryComponent extends View {
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
            View.create(new MyComponent("2", this, { count: 1, increaseBy: 2 }));
        }
        else {
            earlierCreatedChild_2.updateWithValueParams({
                count: 1, increaseBy: 2
            });
            View.create(earlierCreatedChild_2);
        }
        let earlierCreatedChild_3 = (this && this.findChildById) ? this.findChildById("3") : undefined;
        if (earlierCreatedChild_3 == undefined) {
            View.create(new MyComponent("3", this, { title: { value: 'Hello, World 2' }, count: 7 }));
        }
        else {
            earlierCreatedChild_3.updateWithValueParams({
                title: { value: 'Hello, World 2' }, count: 7
            });
            View.create(earlierCreatedChild_3);
        }
        Column.pop();
    }
}
loadDocument(new EntryComponent("1", undefined, {}));
`
