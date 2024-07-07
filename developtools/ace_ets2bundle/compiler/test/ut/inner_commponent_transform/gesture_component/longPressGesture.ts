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
struct LongPressGestureExample {
  @State count: number = 0

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceBetween }) {
      Text('LongPress onAction:' + this.count)
    }
    .height(200).width(300).padding(60).border({ width:1 }).margin(30)
    .gesture(
    LongPressGesture({ repeat: true })
      .onAction((event: GestureEvent) => {
        if (event.repeat) { this.count++ }
      })
      .onActionEnd(() => {
        this.count = 0
      })
    )
  }
}`

exports.expectResult =
`"use strict";
let __generate__Id = 0;
function generateId() {
    return "longPressGesture_" + ++__generate__Id;
}
class LongPressGestureExample extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__count = new ObservedPropertySimple(0, this, "count");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.count !== undefined) {
            this.count = params.count;
        }
    }
    aboutToBeDeleted() {
        this.__count.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get count() {
        return this.__count.get();
    }
    set count(newValue) {
        this.__count.set(newValue);
    }
    render() {
        Flex.create({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceBetween });
        Flex.height(200);
        Flex.width(300);
        Flex.padding(60);
        Flex.border({ width: 1 });
        Flex.margin(30);
        Gesture.create(GesturePriority.Low);
        LongPressGesture.create({ repeat: true });
        LongPressGesture.onAction((event) => {
            if (event.repeat) {
                this.count++;
            }
        });
        LongPressGesture.onActionEnd(() => {
            this.count = 0;
        });
        LongPressGesture.pop();
        Gesture.pop();
        Text.create('LongPress onAction:' + this.count);
        Text.pop();
        Flex.pop();
    }
}
loadDocument(new LongPressGestureExample("1", undefined, {}));
`
