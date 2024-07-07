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
struct ctComponent {
  @Prop name: string
  @Prop canPlay: boolean
  @Prop count: number
  private costOfOneAttempt: number

  build() {
    Column() {
      if(this.canPlay) {
        if (this.count > 0) {
          Text(this.name + ' have ' + this.count + ' Nuggets left')
        } else {
          Text('Sorry, ' + this.name + '. Game over!')
        }

        Button() {
          Text('Try again')
        }.onClick(() => {
          this.count -= this.costOfOneAttempt
        })
      }else{
        Text(this.name + ', sorry. You do not play this game')
      }
    }
  }
}
@Entry
@Component
struct PageComponent {
  @State countDownStartValue: number = 10
  build() {
    Column() {
      Text('Grant ' + this.countDownStartValue + ' nuggets to play')
      Button() {
        Text('+1 - Nuggets in New Game')
      }.onClick(() => {
        this.countDownStartValue += 1
      })
      Button() {
        Text('-1  - Nuggets in New Game')
      }.onClick(() => {
        this.countDownStartValue -= 1
      })
      ctComponent({ name: 'xiaoming', canPlay: true, count: this.countDownStartValue, costOfOneAttempt: 2})
    }
  }
}
`
exports.expectResult =
`"use strict";
let __generate__Id = 0;
function generateId() {
    return "@prop_" + ++__generate__Id;
}
class ctComponent extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__name = new SynchedPropertySimpleOneWay(params.name, this, "name");
        this.__canPlay = new SynchedPropertySimpleOneWay(params.canPlay, this, "canPlay");
        this.__count = new SynchedPropertySimpleOneWay(params.count, this, "count");
        this.costOfOneAttempt = undefined;
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        this.name = params.name;
        this.canPlay = params.canPlay;
        this.count = params.count;
        if (params.costOfOneAttempt !== undefined) {
            this.costOfOneAttempt = params.costOfOneAttempt;
        }
    }
    aboutToBeDeleted() {
        this.__name.aboutToBeDeleted();
        this.__canPlay.aboutToBeDeleted();
        this.__count.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get name() {
        return this.__name.get();
    }
    set name(newValue) {
        this.__name.set(newValue);
    }
    get canPlay() {
        return this.__canPlay.get();
    }
    set canPlay(newValue) {
        this.__canPlay.set(newValue);
    }
    get count() {
        return this.__count.get();
    }
    set count(newValue) {
        this.__count.set(newValue);
    }
    render() {
        Column.create();
        If.create();
        if (this.canPlay) {
            If.branchId(0);
            If.create();
            if (this.count > 0) {
                If.branchId(0);
                Text.create(this.name + ' have ' + this.count + ' Nuggets left');
                Text.pop();
            }
            else {
                If.branchId(1);
                Text.create('Sorry, ' + this.name + '. Game over!');
                Text.pop();
            }
            If.pop();
            Button.createWithChild();
            Button.onClick(() => {
                this.count -= this.costOfOneAttempt;
            });
            Text.create('Try again');
            Text.pop();
            Button.pop();
        }
        else {
            If.branchId(1);
            Text.create(this.name + ', sorry. You do not play this game');
            Text.pop();
        }
        If.pop();
        Column.pop();
    }
}
class PageComponent extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__countDownStartValue = new ObservedPropertySimple(10, this, "countDownStartValue");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.countDownStartValue !== undefined) {
            this.countDownStartValue = params.countDownStartValue;
        }
    }
    aboutToBeDeleted() {
        this.__countDownStartValue.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get countDownStartValue() {
        return this.__countDownStartValue.get();
    }
    set countDownStartValue(newValue) {
        this.__countDownStartValue.set(newValue);
    }
    render() {
        Column.create();
        Text.create('Grant ' + this.countDownStartValue + ' nuggets to play');
        Text.pop();
        Button.createWithChild();
        Button.onClick(() => {
            this.countDownStartValue += 1;
        });
        Text.create('+1 - Nuggets in New Game');
        Text.pop();
        Button.pop();
        Button.createWithChild();
        Button.onClick(() => {
            this.countDownStartValue -= 1;
        });
        Text.create('-1  - Nuggets in New Game');
        Text.pop();
        Button.pop();
        let earlierCreatedChild_2 = (this && this.findChildById) ? this.findChildById("2") : undefined;
        if (earlierCreatedChild_2 == undefined) {
            View.create(new ctComponent("2", this, { name: 'xiaoming', canPlay: true, count: this.countDownStartValue, costOfOneAttempt: 2 }));
        }
        else {
            earlierCreatedChild_2.updateWithValueParams({
                name: 'xiaoming', canPlay: true, count: this.countDownStartValue, costOfOneAttempt: 2
            });
            View.create(earlierCreatedChild_2);
        }
        Column.pop();
    }
}
loadDocument(new PageComponent("1", undefined, {}));
`
