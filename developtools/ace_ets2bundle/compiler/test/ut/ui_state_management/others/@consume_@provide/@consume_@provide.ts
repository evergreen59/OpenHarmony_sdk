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
struct CompA {
  @Provide("reviewVote") reviewVotes : number = 0;

  build() {
    Column() {
      CompB()
      Button() {
        Text('' + this.reviewVotes)
          .fontSize(30)
      }
      .onClick(() => {
        this.reviewVotes += 1;
      })
    }
  }
}

@Component
struct CompB {
  build() {
    Column() {
      CompC()
    }
  }
}

@Component
struct CompC {
  @Consume("reviewVote") reviewVotes : number;
  build() {
    Column() {
      Button() {
        Text('' + this.reviewVotes)
          .fontSize(30)
      }
      .onClick(() => {
        this.reviewVotes += 1;
      })
    }
  }
}
`
exports.expectResult =
`"use strict";
let __generate__Id = 0;
function generateId() {
    return "@consume_@provide_" + ++__generate__Id;
}
class CompA extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__reviewVotes = new ObservedPropertySimple(0, this, "reviewVotes");
        this.addProvidedVar("reviewVote", this.__reviewVotes);
        this.addProvidedVar("reviewVotes", this.__reviewVotes);
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.reviewVotes !== undefined) {
            this.reviewVotes = params.reviewVotes;
        }
    }
    aboutToBeDeleted() {
        this.__reviewVotes.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get reviewVotes() {
        return this.__reviewVotes.get();
    }
    set reviewVotes(newValue) {
        this.__reviewVotes.set(newValue);
    }
    render() {
        Column.create();
        let earlierCreatedChild_2 = (this && this.findChildById) ? this.findChildById("2") : undefined;
        if (earlierCreatedChild_2 == undefined) {
            View.create(new CompB("2", this, {}));
        }
        else {
            earlierCreatedChild_2.updateWithValueParams({});
            if (!earlierCreatedChild_2.needsUpdate()) {
                earlierCreatedChild_2.markStatic();
            }
            View.create(earlierCreatedChild_2);
        }
        Button.createWithChild();
        Button.onClick(() => {
            this.reviewVotes += 1;
        });
        Text.create('' + this.reviewVotes);
        Text.fontSize(30);
        Text.pop();
        Button.pop();
        Column.pop();
    }
}
class CompB extends View {
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
        let earlierCreatedChild_3 = (this && this.findChildById) ? this.findChildById("3") : undefined;
        if (earlierCreatedChild_3 == undefined) {
            View.create(new CompC("3", this, {}));
        }
        else {
            earlierCreatedChild_3.updateWithValueParams({});
            View.create(earlierCreatedChild_3);
        }
        Column.pop();
    }
}
class CompC extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__reviewVotes = this.initializeConsume("reviewVote", "reviewVotes");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
    }
    aboutToBeDeleted() {
        this.__reviewVotes.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get reviewVotes() {
        return this.__reviewVotes.get();
    }
    set reviewVotes(newValue) {
        this.__reviewVotes.set(newValue);
    }
    render() {
        Column.create();
        Button.createWithChild();
        Button.onClick(() => {
            this.reviewVotes += 1;
        });
        Text.create('' + this.reviewVotes);
        Text.fontSize(30);
        Text.pop();
        Button.pop();
        Column.pop();
    }
}
loadDocument(new CompA("1", undefined, {}));
`
