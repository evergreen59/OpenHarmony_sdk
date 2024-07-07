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
import { CustomDialogExample1 as CustomDialogExample } from './test/pages/import@CustomDialog'

@Entry
@Component
struct CustomDialogUser {
  @State textValue: string = ''
  @State inputValue: string = 'click me'
  dialogController: CustomDialogController = new CustomDialogController({
    builder: CustomDialogExample({
      cancel: this.onCancel,
      confirm: this.onAccept,
      textValue: $textValue,
      inputValue: $inputValue
    }),
    cancel: this.existApp,
    autoCancel: true,
    alignment: DialogAlignment.Default,
    offset: { dx: 0, dy: -20 },
    gridCount: 4,
    customStyle: false
  })

  onCancel() {
    console.info('Callback when the first button is clicked')
  }

  onAccept() {
    console.info('Callback when the second button is clicked')
  }

  existApp() {
    console.info('Click the callback in the blank area')
  }

  build() {
    Column() {
      Button(this.inputValue)
        .onClick(() => {
          this.dialogController.open()
        }).backgroundColor(0x317aff)
    }.width('100%').margin({ top: 5 })
  }
}
`

exports.expectResult =
`"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
let __generate__Id = 0;
function generateId() {
    return "import@CustomDialog_" + ++__generate__Id;
}
const import_CustomDialog_1 = require("./test/pages/import@CustomDialog");
class CustomDialogUser extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__textValue = new ObservedPropertySimple('', this, "textValue");
        this.__inputValue = new ObservedPropertySimple('click me', this, "inputValue");
        this.dialogController = new CustomDialogController({
            builder: () => {
                let jsDialog = new import_CustomDialog_1.CustomDialogExample1("2", this, {
                    cancel: this.onCancel,
                    confirm: this.onAccept,
                    textValue: this.__textValue,
                    inputValue: this.__inputValue
                });
                jsDialog.setController(this.dialogController);
                View.create(jsDialog);
            },
            cancel: this.existApp,
            autoCancel: true,
            alignment: DialogAlignment.Default,
            offset: { dx: 0, dy: -20 },
            gridCount: 4,
            customStyle: false
        }, this);
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.textValue !== undefined) {
            this.textValue = params.textValue;
        }
        if (params.inputValue !== undefined) {
            this.inputValue = params.inputValue;
        }
        if (params.dialogController !== undefined) {
            this.dialogController = params.dialogController;
        }
    }
    aboutToBeDeleted() {
        this.__textValue.aboutToBeDeleted();
        this.__inputValue.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get textValue() {
        return this.__textValue.get();
    }
    set textValue(newValue) {
        this.__textValue.set(newValue);
    }
    get inputValue() {
        return this.__inputValue.get();
    }
    set inputValue(newValue) {
        this.__inputValue.set(newValue);
    }
    onCancel() {
        console.info('Callback when the first button is clicked');
    }
    onAccept() {
        console.info('Callback when the second button is clicked');
    }
    existApp() {
        console.info('Click the callback in the blank area');
    }
    render() {
        Column.create();
        Column.width('100%');
        Column.margin({ top: 5 });
        Button.createWithLabel(this.inputValue);
        Button.onClick(() => {
            this.dialogController.open();
        });
        Button.backgroundColor(0x317aff);
        Button.pop();
        Column.pop();
    }
}
loadDocument(new CustomDialogUser("1", undefined, {}));
`
