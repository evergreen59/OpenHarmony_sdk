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
@CustomDialog
struct DialogExample {
  @Prop count: number
  @Link isPlaying: boolean
  controller: CustomDialogController;
  termsToAccept: string = ""
  action1: () => void;
  action2: (x: number, s: string) => void;

  build() {
    Column(){
      Row(){
        Button('current count is: ' + this.count)
          .onClick(() => {
            this.count++;
          })
      }
      Row(){
        Button(this.isPlaying ? 'play' : 'pause')
          .onClick(() => {
            this.isPlaying = !this.isPlaying;
          })
      }
      Row() {
        Button ("Option A")
          .onClick(() => {
            this.controller.close();
            this.action1();
          })
        Button ("Option B")
          .onClick(() => {
            this.controller.close();
            this.action2(47, "Option B is great choice");
          })
      }
    }
  }
}

@Entry
@Component
struct CustomDialogUser {
  @State countInitValue: number = 10;
  @State playingInitValue: boolean = false;
  dialogController : CustomDialogController = new CustomDialogController({
    builder: DialogExample({
      termsToAccept: "Please accept the terms.",
      action1: this.onAccept,
      action2: this.existApp,
      count: this.countInitValue,
      isPlaying: $playingInitValue
    }),
    cancel: this.existApp,
    autoCancel: false
  });

  onAccept() {
    console.log("onAccept");
  }
  existApp() {
    console.log("Cancel dialog!");
  }

  build() {
    Column() {
      Text('current countInitValue is: ' + this.countInitValue)
        .fontSize(20)
      Text('current playingInitValue is: ' + this.playingInitValue)
        .fontSize(20)
      Button("Click to open Dialog -1")
        .onClick(() => {
          this.countInitValue--;
          this.dialogController.open()
        })
      Button("Click to close Dialog +1")
        .onClick(() => {
          this.countInitValue++;
          this.dialogController.close()
        })
    }
  }
}
`

exports.expectResult =
`"use strict";
let __generate__Id = 0;
function generateId() {
    return "@customDialog_" + ++__generate__Id;
}
class DialogExample extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__count = new SynchedPropertySimpleOneWay(params.count, this, "count");
        this.__isPlaying = new SynchedPropertySimpleTwoWay(params.isPlaying, this, "isPlaying");
        this.controller = undefined;
        this.termsToAccept = "";
        this.action1 = undefined;
        this.action2 = undefined;
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        this.count = params.count;
        if (params.controller !== undefined) {
            this.controller = params.controller;
        }
        if (params.termsToAccept !== undefined) {
            this.termsToAccept = params.termsToAccept;
        }
        if (params.action1 !== undefined) {
            this.action1 = params.action1;
        }
        if (params.action2 !== undefined) {
            this.action2 = params.action2;
        }
    }
    aboutToBeDeleted() {
        this.__count.aboutToBeDeleted();
        this.__isPlaying.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get count() {
        return this.__count.get();
    }
    set count(newValue) {
        this.__count.set(newValue);
    }
    get isPlaying() {
        return this.__isPlaying.get();
    }
    set isPlaying(newValue) {
        this.__isPlaying.set(newValue);
    }
    setController(ctr) {
        this.controller = ctr;
    }
    render() {
        Column.create();
        Row.create();
        Button.createWithLabel('current count is: ' + this.count);
        Button.onClick(() => {
            this.count++;
        });
        Button.pop();
        Row.pop();
        Row.create();
        Button.createWithLabel(this.isPlaying ? 'play' : 'pause');
        Button.onClick(() => {
            this.isPlaying = !this.isPlaying;
        });
        Button.pop();
        Row.pop();
        Row.create();
        Button.createWithLabel("Option A");
        Button.onClick(() => {
            this.controller.close();
            this.action1();
        });
        Button.pop();
        Button.createWithLabel("Option B");
        Button.onClick(() => {
            this.controller.close();
            this.action2(47, "Option B is great choice");
        });
        Button.pop();
        Row.pop();
        Column.pop();
    }
}
class CustomDialogUser extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__countInitValue = new ObservedPropertySimple(10, this, "countInitValue");
        this.__playingInitValue = new ObservedPropertySimple(false, this, "playingInitValue");
        this.dialogController = new CustomDialogController({
            builder: () => {
                let jsDialog = new DialogExample("2", this, {
                    termsToAccept: "Please accept the terms.",
                    action1: this.onAccept,
                    action2: this.existApp,
                    count: this.countInitValue,
                    isPlaying: this.__playingInitValue
                });
                jsDialog.setController(this.dialogController);
                View.create(jsDialog);
            },
            cancel: this.existApp,
            autoCancel: false
        }, this);
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.countInitValue !== undefined) {
            this.countInitValue = params.countInitValue;
        }
        if (params.playingInitValue !== undefined) {
            this.playingInitValue = params.playingInitValue;
        }
        if (params.dialogController !== undefined) {
            this.dialogController = params.dialogController;
        }
    }
    aboutToBeDeleted() {
        this.__countInitValue.aboutToBeDeleted();
        this.__playingInitValue.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get countInitValue() {
        return this.__countInitValue.get();
    }
    set countInitValue(newValue) {
        this.__countInitValue.set(newValue);
    }
    get playingInitValue() {
        return this.__playingInitValue.get();
    }
    set playingInitValue(newValue) {
        this.__playingInitValue.set(newValue);
    }
    onAccept() {
        console.log("onAccept");
    }
    existApp() {
        console.log("Cancel dialog!");
    }
    render() {
        Column.create();
        Text.create('current countInitValue is: ' + this.countInitValue);
        Text.fontSize(20);
        Text.pop();
        Text.create('current playingInitValue is: ' + this.playingInitValue);
        Text.fontSize(20);
        Text.pop();
        Button.createWithLabel("Click to open Dialog -1");
        Button.onClick(() => {
            this.countInitValue--;
            this.dialogController.open();
        });
        Button.pop();
        Button.createWithLabel("Click to close Dialog +1");
        Button.onClick(() => {
            this.countInitValue++;
            this.dialogController.close();
        });
        Button.pop();
        Column.pop();
    }
}
loadDocument(new CustomDialogUser("1", undefined, {}));
`
