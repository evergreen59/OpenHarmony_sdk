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
@Builder
function ComB(param: string[]) {
  ForEach(param, item => {
    ComA().backgroundColor('red')
  })
}

@Entry
@Component
struct Index {
  @State arr: string[] = ['1', '2', '3', '4', '5']

  build() {
    Column() {
      ComB(this.arr);
    }
  }
}

@Component
struct ComA {
  build() {
    Row() {
      Text('自定义组件')
        .fontSize(30)
    }
  }
}
`
exports.expectResult =
`"use strict";
let __generate__Id = 0;
function generateId() {
    return "@builderWithForEach_" + ++__generate__Id;
}
function ComB(param, parent = null) {
    ForEach.create("3", parent ? parent : this, ObservedObject.GetRawObject(param), item => {
        __Common__.create();
        __Common__.backgroundColor('red');
        let earlierCreatedChild_2 = ((parent ? parent : this) && (parent ? parent : this).findChildById) ? (parent ? parent : this).findChildById(generateId()) : undefined;
        if (earlierCreatedChild_2 == undefined) {
            View.create(new ComA("@builderWithForEach_" + __generate__Id, parent ? parent : this, {}));
        }
        else {
            earlierCreatedChild_2.updateWithValueParams({});
            if (!earlierCreatedChild_2.needsUpdate()) {
                earlierCreatedChild_2.markStatic();
            }
            View.create(earlierCreatedChild_2);
        }
        __Common__.pop();
    });
    ForEach.pop();
}
class Index extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__arr = new ObservedPropertyObject(['1', '2', '3', '4', '5'], this, "arr");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.arr !== undefined) {
            this.arr = params.arr;
        }
    }
    aboutToBeDeleted() {
        this.__arr.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get arr() {
        return this.__arr.get();
    }
    set arr(newValue) {
        this.__arr.set(newValue);
    }
    render() {
        Column.create();
        ComB(this.arr, this);
        Column.pop();
    }
}
class ComA extends View {
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
        Row.create();
        Text.create('自定义组件');
        Text.fontSize(30);
        Text.pop();
        Row.pop();
    }
}
loadDocument(new Index("1", undefined, {}));
`
