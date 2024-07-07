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
@Entry
@Component
struct Index {
  @State WIDTH_AND_HEIGHT: Array<{ w: number, h: number }> = [
    { w: 10, h: 10 },
    { w: 20, h: 20 },
    { w: 30, h: 30 },
    { w: 40, h: 40 },
    { w: 50, h: 50 }
  ]
  build() {
    Row() {
      Column() {
        ForEach(this.WIDTH_AND_HEIGHT, ({ w, h }) => {
          Button()
            .width(w)
            .height(h)
        }, item => item.toString())
      }
      .width('100%')
    }
    .height('100%')
  }
}`

exports.expectResult =
`"use strict";
let __generate__Id = 0;
function generateId() {
    return "forEachTwo_" + ++__generate__Id;
}
class Index extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__WIDTH_AND_HEIGHT = new ObservedPropertyObject([
            { w: 10, h: 10 },
            { w: 20, h: 20 },
            { w: 30, h: 30 },
            { w: 40, h: 40 },
            { w: 50, h: 50 }
        ], this, "WIDTH_AND_HEIGHT");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.WIDTH_AND_HEIGHT !== undefined) {
            this.WIDTH_AND_HEIGHT = params.WIDTH_AND_HEIGHT;
        }
    }
    aboutToBeDeleted() {
        this.__WIDTH_AND_HEIGHT.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get WIDTH_AND_HEIGHT() {
        return this.__WIDTH_AND_HEIGHT.get();
    }
    set WIDTH_AND_HEIGHT(newValue) {
        this.__WIDTH_AND_HEIGHT.set(newValue);
    }
    render() {
        Row.create();
        Row.height('100%');
        Column.create();
        Column.width('100%');
        ForEach.create("2", this, ObservedObject.GetRawObject(this.WIDTH_AND_HEIGHT), ({ w, h }) => {
            Button.createWithLabel();
            Button.width(w);
            Button.height(h);
            Button.pop();
        }, item => item.toString());
        ForEach.pop();
        Column.pop();
        Row.pop();
    }
}
loadDocument(new Index("1", undefined, {}));
`
