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
import { tExtend, tStyles, DivideTest, Base } from './test/pages/ImportNestAll';

@Entry
@Component
struct ImportTest {
  @State testText1: string = 'Hello'
  @State testText2: string = 'World'
  @State testText3: string = 'Test'
  @State testText4: string = 'Component'

  @State testState1: string = 'Base'
  @State testState2: number = 0
  @State testState3: object = { name: 'Base' }
  @State testState4: number = 3
  @State testState5: number = 10

  build() {
    Column() {
      Text(this.testText1)
        .fontSize(50)
      tExtend(20)
      Text(this.testText2)
      tStyles()
      Button(this.testText3)
      Text(this.testText4)
        .fontSize(50)

      Base({
        testStr: $testState1,
        testNum: $testState2,
        testObj: $testState3
      })
      DivideTest({
        testNum1: $testState4,
        testNum2: $testState5
      })
    }
  }
}
`

exports.expectResult =
`"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
let __generate__Id = 0;
function generateId() {
    return "importExportNest_" + ++__generate__Id;
}
const ImportNestAll_1 = require("./test/pages/ImportNestAll");
class ImportTest extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__testText1 = new ObservedPropertySimple('Hello', this, "testText1");
        this.__testText2 = new ObservedPropertySimple('World', this, "testText2");
        this.__testText3 = new ObservedPropertySimple('Test', this, "testText3");
        this.__testText4 = new ObservedPropertySimple('Component', this, "testText4");
        this.__testState1 = new ObservedPropertySimple('Base', this, "testState1");
        this.__testState2 = new ObservedPropertySimple(0, this, "testState2");
        this.__testState3 = new ObservedPropertyObject({ name: 'Base' }, this, "testState3");
        this.__testState4 = new ObservedPropertySimple(3, this, "testState4");
        this.__testState5 = new ObservedPropertySimple(10, this, "testState5");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.testText1 !== undefined) {
            this.testText1 = params.testText1;
        }
        if (params.testText2 !== undefined) {
            this.testText2 = params.testText2;
        }
        if (params.testText3 !== undefined) {
            this.testText3 = params.testText3;
        }
        if (params.testText4 !== undefined) {
            this.testText4 = params.testText4;
        }
        if (params.testState1 !== undefined) {
            this.testState1 = params.testState1;
        }
        if (params.testState2 !== undefined) {
            this.testState2 = params.testState2;
        }
        if (params.testState3 !== undefined) {
            this.testState3 = params.testState3;
        }
        if (params.testState4 !== undefined) {
            this.testState4 = params.testState4;
        }
        if (params.testState5 !== undefined) {
            this.testState5 = params.testState5;
        }
    }
    aboutToBeDeleted() {
        this.__testText1.aboutToBeDeleted();
        this.__testText2.aboutToBeDeleted();
        this.__testText3.aboutToBeDeleted();
        this.__testText4.aboutToBeDeleted();
        this.__testState1.aboutToBeDeleted();
        this.__testState2.aboutToBeDeleted();
        this.__testState3.aboutToBeDeleted();
        this.__testState4.aboutToBeDeleted();
        this.__testState5.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get testText1() {
        return this.__testText1.get();
    }
    set testText1(newValue) {
        this.__testText1.set(newValue);
    }
    get testText2() {
        return this.__testText2.get();
    }
    set testText2(newValue) {
        this.__testText2.set(newValue);
    }
    get testText3() {
        return this.__testText3.get();
    }
    set testText3(newValue) {
        this.__testText3.set(newValue);
    }
    get testText4() {
        return this.__testText4.get();
    }
    set testText4(newValue) {
        this.__testText4.set(newValue);
    }
    get testState1() {
        return this.__testState1.get();
    }
    set testState1(newValue) {
        this.__testState1.set(newValue);
    }
    get testState2() {
        return this.__testState2.get();
    }
    set testState2(newValue) {
        this.__testState2.set(newValue);
    }
    get testState3() {
        return this.__testState3.get();
    }
    set testState3(newValue) {
        this.__testState3.set(newValue);
    }
    get testState4() {
        return this.__testState4.get();
    }
    set testState4(newValue) {
        this.__testState4.set(newValue);
    }
    get testState5() {
        return this.__testState5.get();
    }
    set testState5(newValue) {
        this.__testState5.set(newValue);
    }
    render() {
        Column.create();
        Text.create(this.testText1);
        Text.fontSize(50);
        Text.pop();
        ImportNestAll_1.tExtend(20, this);
        Text.create(this.testText2);
        Text.pop();
        ImportNestAll_1.tStyles(this);
        Button.createWithLabel(this.testText3);
        Button.pop();
        Text.create(this.testText4);
        Text.fontSize(50);
        Text.pop();
        let earlierCreatedChild_2 = (this && this.findChildById) ? this.findChildById("2") : undefined;
        if (earlierCreatedChild_2 == undefined) {
            View.create(new ImportNestAll_1.Base("2", this, {
                testStr: this.__testState1,
                testNum: this.__testState2,
                testObj: this.__testState3
            }));
        }
        else {
            earlierCreatedChild_2.updateWithValueParams({});
            View.create(earlierCreatedChild_2);
        }
        let earlierCreatedChild_3 = (this && this.findChildById) ? this.findChildById("3") : undefined;
        if (earlierCreatedChild_3 == undefined) {
            View.create(new ImportNestAll_1.DivideTest("3", this, {
                testNum1: this.__testState4,
                testNum2: this.__testState5
            }));
        }
        else {
            earlierCreatedChild_3.updateWithValueParams({});
            View.create(earlierCreatedChild_3);
        }
        Column.pop();
    }
}
loadDocument(new ImportTest("1", undefined, {}));
`
