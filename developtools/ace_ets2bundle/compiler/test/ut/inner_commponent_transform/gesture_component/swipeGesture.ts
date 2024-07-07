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
struct SwipeGestureExample {
  @State rotateAngle : number = 0
  @State speed : number = 1

  build() {
    Column() {
      Text("SwipGesture speed ： " + this.speed)
      Text("SwipGesture angle ： " + this.rotateAngle)
    }
    .border({width:2})
    .width(260).height(260)
    .rotate({x: 0, y: 0, z: 1, angle: this.rotateAngle})
    .gesture(
    SwipeGesture({fingers: 1, direction:SwipeDirection.Vertical})
      .onAction((event: GestureEvent) => {
        this.speed = event.speed
        this.rotateAngle = event.angle
      })
    )
  }
}`

exports.expectResult =
`"use strict";
let __generate__Id = 0;
function generateId() {
    return "swipeGesture_" + ++__generate__Id;
}
class SwipeGestureExample extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__rotateAngle = new ObservedPropertySimple(0, this, "rotateAngle");
        this.__speed = new ObservedPropertySimple(1, this, "speed");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.rotateAngle !== undefined) {
            this.rotateAngle = params.rotateAngle;
        }
        if (params.speed !== undefined) {
            this.speed = params.speed;
        }
    }
    aboutToBeDeleted() {
        this.__rotateAngle.aboutToBeDeleted();
        this.__speed.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get rotateAngle() {
        return this.__rotateAngle.get();
    }
    set rotateAngle(newValue) {
        this.__rotateAngle.set(newValue);
    }
    get speed() {
        return this.__speed.get();
    }
    set speed(newValue) {
        this.__speed.set(newValue);
    }
    render() {
        Column.create();
        Column.border({ width: 2 });
        Column.width(260);
        Column.height(260);
        Column.rotate({ x: 0, y: 0, z: 1, angle: this.rotateAngle });
        Gesture.create(GesturePriority.Low);
        SwipeGesture.create({ fingers: 1, direction: SwipeDirection.Vertical });
        SwipeGesture.onAction((event) => {
            this.speed = event.speed;
            this.rotateAngle = event.angle;
        });
        SwipeGesture.pop();
        Gesture.pop();
        Text.create("SwipGesture speed ： " + this.speed);
        Text.pop();
        Text.create("SwipGesture angle ： " + this.rotateAngle);
        Text.pop();
        Column.pop();
    }
}
loadDocument(new SwipeGestureExample("1", undefined, {}));
`
