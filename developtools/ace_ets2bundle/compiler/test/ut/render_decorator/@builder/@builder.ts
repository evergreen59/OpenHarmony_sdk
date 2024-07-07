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
@Builder function noParam(){
  Row(){
    Text('this is a no param builder')
  }
}
@Builder function specificParam(label1: string, label2: string) {
  Column() {
    Text(label1)
    Text(label2)
  }
}
@Entry
@Component
struct MyComponent {
  private arr: number[] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
  private controller: TabsController = new TabsController()
  @State hideBar: boolean = true
  @Builder textBuilder() {
    Text("文本")
      .fontSize(30)
  }
  @Builder NavigationTitlePara(label:string) {
    Column() {
      Text(label)
        .width(80)
        .bindMenu(this.textBuilder)
    }
  }
  @Builder MenuBuilder() {
    Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.Center, alignItems: ItemAlign.Center }) {
      Text('Test menu item 1')
        .fontSize(20)
      Divider().height(10)
      Text('Test menu item 2')
        .fontSize(20)
    }.width(100)
  }

  build() {
    Column() {
      Row(){
        Text("Drag Me")
          .onDragStart((event: DragEvent, extraParams: string) => {
            console.log('Text onDragStarts, ' + extraParams)
          })
        specificParam('test1', 'test2')
      }
      .padding(10)
      .bindMenu(this.NavigationTitlePara("111"))
      Row(){
        Text('Test Text')
      }
      .padding(10)
      .bindPopup(false, {
        builder: this.MenuBuilder,
        onStateChange: (e) => {
          if(!e.isVisible){
            console.warn(JSON.stringify(e.isVisible))
          }
        }
      })
      Row() {
        Text('rightclick for menu')
      }
      .padding(10)
      .bindContextMenu(this.MenuBuilder, ResponseType.RightClick)
      Row(){
        Navigation() {
          List({ space: 5, initialIndex: 0 }) {
            ForEach(this.arr, (item) => {
              ListItem() {
                Text('' + item)
                  .width('90%')
                  .height(80)
                  .backgroundColor('#3366CC')
                  .borderRadius(15)
                  .fontSize(16)
                  .textAlign(TextAlign.Center)
              }.editable(true)
            }, item => item)
          }
          .listDirection(Axis.Vertical)
          .height(300)
          .margin({ top: 10, left: 18 })
          .width('100%')
          Button(this.hideBar ? "tool bar" : "hide bar")
            .onClick(() => {
              this.hideBar = !this.hideBar
            })
            .margin({ left: 135, top: 60 })
        }
        .title(noParam)
        .menus(this.textBuilder)
        .toolBar({ items: [
          { value: 'app', text: 'Grid', action: () => {
            console.log("app")
          } },
          { value: 'add', text: 'Add', action: () => {
            console.log("add")
          } },
          { value: 'collect', text: 'Collect', action: () => {
            console.log("collect")
          } }] })
        .hideToolBar(this.hideBar)
      }
      .padding(10)
      Row(){
        Tabs({ barPosition: BarPosition.Start, controller: this.controller }) {
          TabContent() {
            Text('111').width('100%').height('20').backgroundColor(Color.Pink)
          }
          .tabBar('pink')
          TabContent() {
            Text('222').width('100%').height('20').backgroundColor(Color.Yellow)
          }
          .tabBar('yellow')
          TabContent() {
            Text('333').width('100%').height('20').backgroundColor(Color.Blue)
          }
          .tabBar('blue')
        }
      }
      .padding(10)
    }
  }
}
`
exports.expectResult =
`"use strict";
let __generate__Id = 0;
function generateId() {
    return "@builder_" + ++__generate__Id;
}
function noParam(parent = null) {
    Row.create();
    Text.create('this is a no param builder');
    Text.pop();
    Row.pop();
}
function specificParam(label1, label2, parent = null) {
    Column.create();
    Text.create(label1);
    Text.pop();
    Text.create(label2);
    Text.pop();
    Column.pop();
}
class MyComponent extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.arr = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9];
        this.controller = new TabsController();
        this.__hideBar = new ObservedPropertySimple(true, this, "hideBar");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.arr !== undefined) {
            this.arr = params.arr;
        }
        if (params.controller !== undefined) {
            this.controller = params.controller;
        }
        if (params.hideBar !== undefined) {
            this.hideBar = params.hideBar;
        }
    }
    aboutToBeDeleted() {
        this.__hideBar.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get hideBar() {
        return this.__hideBar.get();
    }
    set hideBar(newValue) {
        this.__hideBar.set(newValue);
    }
    textBuilder(parent = null) {
        Text.create("文本");
        Text.fontSize(30);
        Text.pop();
    }
    NavigationTitlePara(label, parent = null) {
        Column.create();
        Text.create(label);
        Text.width(80);
        Text.bindMenu({ builder: this.textBuilder.bind(this) });
        Text.pop();
        Column.pop();
    }
    MenuBuilder(parent = null) {
        Flex.create({ direction: FlexDirection.Column, justifyContent: FlexAlign.Center, alignItems: ItemAlign.Center });
        Flex.width(100);
        Text.create('Test menu item 1');
        Text.fontSize(20);
        Text.pop();
        Divider.create();
        Divider.height(10);
        Text.create('Test menu item 2');
        Text.fontSize(20);
        Text.pop();
        Flex.pop();
    }
    render() {
        Column.create();
        Row.create();
        Row.padding(10);
        Row.bindMenu({ builder: () => {
                this.NavigationTitlePara.call(this, "111");
            } });
        Text.create("Drag Me");
        Text.onDragStart((event, extraParams) => {
            console.log('Text onDragStarts, ' + extraParams);
        });
        Text.pop();
        specificParam('test1', 'test2', this);
        Row.pop();
        Row.create();
        Row.padding(10);
        Row.bindPopup(false, {
            builder: { builder: this.MenuBuilder.bind(this) },
            onStateChange: (e) => {
                if (!e.isVisible) {
                    console.warn(JSON.stringify(e.isVisible));
                }
            }
        });
        Text.create('Test Text');
        Text.pop();
        Row.pop();
        Row.create();
        Row.padding(10);
        Row.bindContextMenu({ builder: this.MenuBuilder.bind(this) }, ResponseType.RightClick);
        Text.create('rightclick for menu');
        Text.pop();
        Row.pop();
        Row.create();
        Row.padding(10);
        Navigation.create();
        Navigation.title({ builder: noParam.bind(this) });
        Navigation.menus({ builder: this.textBuilder.bind(this) });
        Navigation.toolBar({ items: [
                { value: 'app', text: 'Grid', action: () => {
                        console.log("app");
                    } },
                { value: 'add', text: 'Add', action: () => {
                        console.log("add");
                    } },
                { value: 'collect', text: 'Collect', action: () => {
                        console.log("collect");
                    } }
            ] });
        Navigation.hideToolBar(this.hideBar);
        List.create({ space: 5, initialIndex: 0 });
        List.listDirection(Axis.Vertical);
        List.height(300);
        List.margin({ top: 10, left: 18 });
        List.width('100%');
        ForEach.create("2", this, ObservedObject.GetRawObject(this.arr), (item) => {
            ListItem.create();
            ListItem.editable(true);
            Text.create('' + item);
            Text.width('90%');
            Text.height(80);
            Text.backgroundColor('#3366CC');
            Text.borderRadius(15);
            Text.fontSize(16);
            Text.textAlign(TextAlign.Center);
            Text.pop();
            ListItem.pop();
        }, item => item);
        ForEach.pop();
        List.pop();
        Button.createWithLabel(this.hideBar ? "tool bar" : "hide bar");
        Button.onClick(() => {
            this.hideBar = !this.hideBar;
        });
        Button.margin({ left: 135, top: 60 });
        Button.pop();
        Navigation.pop();
        Row.pop();
        Row.create();
        Row.padding(10);
        Tabs.create({ barPosition: BarPosition.Start, controller: this.controller });
        TabContent.create();
        TabContent.tabBar('pink');
        Text.create('111');
        Text.width('100%');
        Text.height('20');
        Text.backgroundColor(Color.Pink);
        Text.pop();
        TabContent.pop();
        TabContent.create();
        TabContent.tabBar('yellow');
        Text.create('222');
        Text.width('100%');
        Text.height('20');
        Text.backgroundColor(Color.Yellow);
        Text.pop();
        TabContent.pop();
        TabContent.create();
        TabContent.tabBar('blue');
        Text.create('333');
        Text.width('100%');
        Text.height('20');
        Text.backgroundColor(Color.Blue);
        Text.pop();
        TabContent.pop();
        Tabs.pop();
        Row.pop();
        Column.pop();
    }
}
loadDocument(new MyComponent("1", undefined, {}));
`
