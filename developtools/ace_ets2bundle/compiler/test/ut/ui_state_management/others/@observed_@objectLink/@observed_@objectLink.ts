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
let NextID : number = 0;

@Observed class ClassA {
  public id : number;
  public c: number;
  constructor(c: number) {
    this.id = NextID++;
    this.c = c;
  }
}

@Observed class ClassB {
  public a: ClassA;
  constructor(a: ClassA) {
    this.a = a;
  }
}

@Component
struct ViewA {
  @ObjectLink varA : ClassA;
  build() {
    Row() {
      Text('ViewA-' + this.varA.id)
    }
  }
}

@Entry
@Component
struct ViewB {
  @State varB : ClassB = new ClassB(new ClassA(0));
  build() {
    Column() {
      Row() {
        ViewA({ varA: this.varB.a })
        Text('ViewB')
      }
    }
  }
}
`
exports.expectResult =
`"use strict";
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
let __generate__Id = 0;
function generateId() {
    return "@observed_@objectLink_" + ++__generate__Id;
}
let NextID = 0;
let ClassA = class ClassA {
    constructor(c) {
        this.id = NextID++;
        this.c = c;
    }
};
ClassA = __decorate([
    Observed
], ClassA);
let ClassB = class ClassB {
    constructor(a) {
        this.a = a;
    }
};
ClassB = __decorate([
    Observed
], ClassB);
class ViewA extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__varA = new SynchedPropertyNesedObject(params.varA, this, "varA");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        this.__varA.set(params.varA);
    }
    aboutToBeDeleted() {
        this.__varA.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get varA() {
        return this.__varA.get();
    }
    render() {
        Row.create();
        Text.create('ViewA-' + this.varA.id);
        Text.pop();
        Row.pop();
    }
}
class ViewB extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__varB = new ObservedPropertyObject(new ClassB(new ClassA(0)), this, "varB");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.varB !== undefined) {
            this.varB = params.varB;
        }
    }
    aboutToBeDeleted() {
        this.__varB.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get varB() {
        return this.__varB.get();
    }
    set varB(newValue) {
        this.__varB.set(newValue);
    }
    render() {
        Column.create();
        Row.create();
        let earlierCreatedChild_2 = (this && this.findChildById) ? this.findChildById("2") : undefined;
        if (earlierCreatedChild_2 == undefined) {
            View.create(new ViewA("2", this, { varA: this.varB.a }));
        }
        else {
            earlierCreatedChild_2.updateWithValueParams({
                varA: this.varB.a
            });
            View.create(earlierCreatedChild_2);
        }
        Text.create('ViewB');
        Text.pop();
        Row.pop();
        Column.pop();
    }
}
loadDocument(new ViewB("1", undefined, {}));
`
