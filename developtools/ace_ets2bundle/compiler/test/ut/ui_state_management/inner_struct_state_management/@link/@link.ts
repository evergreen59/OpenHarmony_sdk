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
struct linkComp {
  @Link buttonPlaying: boolean
  @Link items: number[]
  @Link obj: {name: string, age: number}[]
  build() {
    Column() {
      Button() {
        Text(this.buttonPlaying ? 'play' : 'pause')
      }.onClick(() => {
        this.buttonPlaying = !this.buttonPlaying
      })
      Row(){
        Button(){
          Text('push 100')
        }.onClick(() => {
          this.items.push(100)
        })
        Button(){
          Text('remove item')
        }.onClick(() => {
          this.items.pop()
        })
      }
      Row(){
        Button(){
          Text('新增一人')
        }.onClick(() => {
          this.obj.push({name: 'xiaohua', age: 18})
        })
      }
    }
  }
}

@Entry
@Component
struct linkPage {
  @State isPlaying: boolean = false
  @State itemsArr: number[] = [1, 2, 3]
  @State peoples: {name: string, age: number}[] = [{name: 'xiaoming', age: 8}]
  build() {
    Column() {
      linkComp({buttonPlaying: $isPlaying, items: $itemsArr, obj: $peoples})
      Text('Player is ' + (this.isPlaying ? '' : 'not') + ' playing')
      ForEach(this.itemsArr, item => Text('' + item), item => item.toString())
      ForEach(this.peoples, item => {
        Text(item.name + ': ' + item.age)
      })
    }
  }
}
`
exports.expectResult =
`"use strict";
let __generate__Id = 0;
function generateId() {
    return "@link_" + ++__generate__Id;
}
class linkComp extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__buttonPlaying = new SynchedPropertySimpleTwoWay(params.buttonPlaying, this, "buttonPlaying");
        this.__items = new SynchedPropertyObjectTwoWay(params.items, this, "items");
        this.__obj = new SynchedPropertyObjectTwoWay(params.obj, this, "obj");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
    }
    aboutToBeDeleted() {
        this.__buttonPlaying.aboutToBeDeleted();
        this.__items.aboutToBeDeleted();
        this.__obj.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get buttonPlaying() {
        return this.__buttonPlaying.get();
    }
    set buttonPlaying(newValue) {
        this.__buttonPlaying.set(newValue);
    }
    get items() {
        return this.__items.get();
    }
    set items(newValue) {
        this.__items.set(newValue);
    }
    get obj() {
        return this.__obj.get();
    }
    set obj(newValue) {
        this.__obj.set(newValue);
    }
    render() {
        Column.create();
        Button.createWithChild();
        Button.onClick(() => {
            this.buttonPlaying = !this.buttonPlaying;
        });
        Text.create(this.buttonPlaying ? 'play' : 'pause');
        Text.pop();
        Button.pop();
        Row.create();
        Button.createWithChild();
        Button.onClick(() => {
            this.items.push(100);
        });
        Text.create('push 100');
        Text.pop();
        Button.pop();
        Button.createWithChild();
        Button.onClick(() => {
            this.items.pop();
        });
        Text.create('remove item');
        Text.pop();
        Button.pop();
        Row.pop();
        Row.create();
        Button.createWithChild();
        Button.onClick(() => {
            this.obj.push({ name: 'xiaohua', age: 18 });
        });
        Text.create('新增一人');
        Text.pop();
        Button.pop();
        Row.pop();
        Column.pop();
    }
}
class linkPage extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__isPlaying = new ObservedPropertySimple(false, this, "isPlaying");
        this.__itemsArr = new ObservedPropertyObject([1, 2, 3], this, "itemsArr");
        this.__peoples = new ObservedPropertyObject([{ name: 'xiaoming', age: 8 }], this, "peoples");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.isPlaying !== undefined) {
            this.isPlaying = params.isPlaying;
        }
        if (params.itemsArr !== undefined) {
            this.itemsArr = params.itemsArr;
        }
        if (params.peoples !== undefined) {
            this.peoples = params.peoples;
        }
    }
    aboutToBeDeleted() {
        this.__isPlaying.aboutToBeDeleted();
        this.__itemsArr.aboutToBeDeleted();
        this.__peoples.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get isPlaying() {
        return this.__isPlaying.get();
    }
    set isPlaying(newValue) {
        this.__isPlaying.set(newValue);
    }
    get itemsArr() {
        return this.__itemsArr.get();
    }
    set itemsArr(newValue) {
        this.__itemsArr.set(newValue);
    }
    get peoples() {
        return this.__peoples.get();
    }
    set peoples(newValue) {
        this.__peoples.set(newValue);
    }
    render() {
        Column.create();
        let earlierCreatedChild_2 = (this && this.findChildById) ? this.findChildById("2") : undefined;
        if (earlierCreatedChild_2 == undefined) {
            View.create(new linkComp("2", this, { buttonPlaying: this.__isPlaying, items: this.__itemsArr, obj: this.__peoples }));
        }
        else {
            earlierCreatedChild_2.updateWithValueParams({});
            View.create(earlierCreatedChild_2);
        }
        Text.create('Player is ' + (this.isPlaying ? '' : 'not') + ' playing');
        Text.pop();
        ForEach.create("3", this, ObservedObject.GetRawObject(this.itemsArr), item => {
            Text.create('' + item);
            Text.pop();
        }, item => item.toString());
        ForEach.pop();
        ForEach.create("4", this, ObservedObject.GetRawObject(this.peoples), item => {
            Text.create(item.name + ': ' + item.age);
            Text.pop();
        });
        ForEach.pop();
        Column.pop();
    }
}
loadDocument(new linkPage("1", undefined, {}));
`
