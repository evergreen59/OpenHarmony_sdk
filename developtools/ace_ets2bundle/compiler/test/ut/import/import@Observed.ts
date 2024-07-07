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
import { ClassB as ClassA } from './test/pages/import@Observed'
@Component
struct ViewA {
  label: string = 'ViewA1'
  @ObjectLink a: ClassA

  build() {
    Row() {
      Button('ViewA'+ JSON.stringify(this.label) + 'this.a.c='+JSON.stringify(this.a.c))
        .onClick(() => {
          this.a.c += 1
        })
    }.margin({ top: 10 })
  }
}

@Entry
@Component
struct ViewB {
  @State arrA: ClassA[] = [new ClassA(0), new ClassA(0)]

  build() {
    Column() {
      ForEach(this.arrA, (item) => {
        ViewA({ label: JSON.stringify(item.id), a: item })
      }, (item) => item.id.toString())
      ViewA({ label: JSON.stringify(this.arrA[0]), a: this.arrA[0] })
      ViewA({ label: JSON.stringify(this.arrA[this.arrA.length - 1]), a: this.arrA[this.arrA.length - 1] })

      Button('ViewB: reset array')
        .margin({ top: 10 })
        .onClick(() => {
          this.arrA = [new ClassA(0), new ClassA(0)]
        })
      Button('ViewB: push')
        .margin({ top: 10 })
        .onClick(() => {
          this.arrA.push(new ClassA(0))
        })
      Button('ViewB: shift')
        .margin({ top: 10 })
        .onClick(() => {
          this.arrA.shift()
        })
    }.width('100%')
  }
}
`

exports.expectResult =
`"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
let __generate__Id = 0;
function generateId() {
    return "import@Observed_" + ++__generate__Id;
}
const import_Observed_1 = require("./test/pages/import@Observed");
class ViewA extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.label = 'ViewA1';
        this.__a = new SynchedPropertyNesedObject(params.a, this, "a");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.label !== undefined) {
            this.label = params.label;
        }
        this.__a.set(params.a);
    }
    aboutToBeDeleted() {
        this.__a.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get a() {
        return this.__a.get();
    }
    render() {
        Row.create();
        Row.margin({ top: 10 });
        Button.createWithLabel('ViewA' + JSON.stringify(this.label) + 'this.a.c=' + JSON.stringify(this.a.c));
        Button.onClick(() => {
            this.a.c += 1;
        });
        Button.pop();
        Row.pop();
    }
}
class ViewB extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__arrA = new ObservedPropertyObject([new import_Observed_1.ClassB(0), new import_Observed_1.ClassB(0)], this, "arrA");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.arrA !== undefined) {
            this.arrA = params.arrA;
        }
    }
    aboutToBeDeleted() {
        this.__arrA.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get arrA() {
        return this.__arrA.get();
    }
    set arrA(newValue) {
        this.__arrA.set(newValue);
    }
    render() {
        Column.create();
        Column.width('100%');
        ForEach.create("3", this, ObservedObject.GetRawObject(this.arrA), (item) => {
            let earlierCreatedChild_2 = (this && this.findChildById) ? this.findChildById("2") : undefined;
            if (earlierCreatedChild_2 == undefined) {
                View.create(new ViewA("2", this, { label: JSON.stringify(item.id), a: item }));
            }
            else {
                earlierCreatedChild_2.updateWithValueParams({
                    label: JSON.stringify(item.id), a: item
                });
                View.create(earlierCreatedChild_2);
            }
        }, (item) => item.id.toString());
        ForEach.pop();
        let earlierCreatedChild_4 = (this && this.findChildById) ? this.findChildById("4") : undefined;
        if (earlierCreatedChild_4 == undefined) {
            View.create(new ViewA("4", this, { label: JSON.stringify(this.arrA[0]), a: this.arrA[0] }));
        }
        else {
            earlierCreatedChild_4.updateWithValueParams({
                label: JSON.stringify(this.arrA[0]), a: this.arrA[0]
            });
            View.create(earlierCreatedChild_4);
        }
        let earlierCreatedChild_5 = (this && this.findChildById) ? this.findChildById("5") : undefined;
        if (earlierCreatedChild_5 == undefined) {
            View.create(new ViewA("5", this, { label: JSON.stringify(this.arrA[this.arrA.length - 1]), a: this.arrA[this.arrA.length - 1] }));
        }
        else {
            earlierCreatedChild_5.updateWithValueParams({
                label: JSON.stringify(this.arrA[this.arrA.length - 1]), a: this.arrA[this.arrA.length - 1]
            });
            View.create(earlierCreatedChild_5);
        }
        Button.createWithLabel('ViewB: reset array');
        Button.margin({ top: 10 });
        Button.onClick(() => {
            this.arrA = [new import_Observed_1.ClassB(0), new import_Observed_1.ClassB(0)];
        });
        Button.pop();
        Button.createWithLabel('ViewB: push');
        Button.margin({ top: 10 });
        Button.onClick(() => {
            this.arrA.push(new import_Observed_1.ClassB(0));
        });
        Button.pop();
        Button.createWithLabel('ViewB: shift');
        Button.margin({ top: 10 });
        Button.onClick(() => {
            this.arrA.shift();
        });
        Button.pop();
        Column.pop();
    }
}
loadDocument(new ViewB("1", undefined, {}));
`
