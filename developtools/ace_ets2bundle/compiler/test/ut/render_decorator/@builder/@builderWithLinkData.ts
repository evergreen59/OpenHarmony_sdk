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
struct TitleComp {
  @Link title: string
  build() {
    Text(this.title)
  }
}

@Entry
@Component
struct TestPage{
  @State value: string = 'hello world'
  @Builder
  TitleCompView() {
    TitleComp({title: $value})
  }
  build() {
    Flex() {
      this.TitleCompView()
    }
  }
}
`
exports.expectResult =
`"use strict";
let __generate__Id = 0;
function generateId() {
    return "@builderWithLinkData_" + ++__generate__Id;
}
class TitleComp extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__title = new SynchedPropertySimpleTwoWay(params.title, this, "title");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
    }
    aboutToBeDeleted() {
        this.__title.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get title() {
        return this.__title.get();
    }
    set title(newValue) {
        this.__title.set(newValue);
    }
    render() {
        Text.create(this.title);
        Text.pop();
    }
}
class TestPage extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__value = new ObservedPropertySimple('hello world', this, "value");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.value !== undefined) {
            this.value = params.value;
        }
    }
    aboutToBeDeleted() {
        this.__value.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get value() {
        return this.__value.get();
    }
    set value(newValue) {
        this.__value.set(newValue);
    }
    TitleCompView(parent = null) {
        let earlierCreatedChild_2 = ((parent ? parent : this) && (parent ? parent : this).findChildById) ? (parent ? parent : this).findChildById(generateId()) : undefined;
        if (earlierCreatedChild_2 == undefined) {
            View.create(new TitleComp("@builderWithLinkData_" + __generate__Id, parent ? parent : this, { title: this.__value }));
        }
        else {
            earlierCreatedChild_2.updateWithValueParams({});
            View.create(earlierCreatedChild_2);
        }
    }
    render() {
        Flex.create();
        this.TitleCompView(this);
        Flex.pop();
    }
}
loadDocument(new TestPage("1", undefined, {}));
`
