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
struct TransitionExample {
  @State btnW: number = 50
  @State btnH: number = 50
  @State btn1: boolean = false
  @State show: string = "show"
  @State color: Color = Color.Red;
  @State width1: number = 100;
  @State height1: number = 100;
  @State opacity1: number = 1;
  @State borderRaius1: number = 10
  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center,}) {
      Button(this.show)
        .onClick(() => {
          animateTo({ duration: 1000 }, () => {
            this.btn1 = !this.btn1
            if(this.btn1){
              this.show = "hide"
            }else{
              this.show = "show"
            }
          })
        })
      if (this.btn1) {
        Button()
          .width("80%").height(30)
          .transition({ type: TransitionType.Insert, scale: {x:0,y:1.0}})
          .transition({ type: TransitionType.Delete, scale: { x: 1.0, y: 0.0 }})
      }
      Button('animation')
        .width(this.btnW).height(this.btnH)
        .onClick(() => {
          this.btnW += 50
        })
        .animation({
          duration: 1000, // 动画时长
          curve: Curve.EaseOut, // 动画曲线
          delay: 200, // 动画延迟
          iterations: 1, // 播放次数
          playMode: PlayMode.Normal // 动画模式
        })
      Column({space: 5}) {
        Column()
          .opacity(this.opacity1)
          .backgroundColor(this.color)
          .animation({duration: 1000})
          .width(this.width1)
          .animation({duration: 2000})
          .height(this.height1)
          .borderRadius(this.borderRaius1)
          .animation({duration: 3000})
          .onClick(() => {
            this.color = Color.Green
            this.borderRaius1 = 20
            this.opacity1 = 0.5
            this.height1 = 200
            this.width1 = 200
          })
      }
      .width("100%")
      .height("100%")
    }.height(400).width("100%").padding({top:100})
  }
}
`

exports.expectResult =
`"use strict";
let __generate__Id = 0;
function generateId() {
    return "animateTo_" + ++__generate__Id;
}
class TransitionExample extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__btnW = new ObservedPropertySimple(50, this, "btnW");
        this.__btnH = new ObservedPropertySimple(50, this, "btnH");
        this.__btn1 = new ObservedPropertySimple(false, this, "btn1");
        this.__show = new ObservedPropertySimple("show", this, "show");
        this.__color = new ObservedPropertyObject(Color.Red, this, "color");
        this.__width1 = new ObservedPropertySimple(100, this, "width1");
        this.__height1 = new ObservedPropertySimple(100, this, "height1");
        this.__opacity1 = new ObservedPropertySimple(1, this, "opacity1");
        this.__borderRaius1 = new ObservedPropertySimple(10, this, "borderRaius1");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.btnW !== undefined) {
            this.btnW = params.btnW;
        }
        if (params.btnH !== undefined) {
            this.btnH = params.btnH;
        }
        if (params.btn1 !== undefined) {
            this.btn1 = params.btn1;
        }
        if (params.show !== undefined) {
            this.show = params.show;
        }
        if (params.color !== undefined) {
            this.color = params.color;
        }
        if (params.width1 !== undefined) {
            this.width1 = params.width1;
        }
        if (params.height1 !== undefined) {
            this.height1 = params.height1;
        }
        if (params.opacity1 !== undefined) {
            this.opacity1 = params.opacity1;
        }
        if (params.borderRaius1 !== undefined) {
            this.borderRaius1 = params.borderRaius1;
        }
    }
    aboutToBeDeleted() {
        this.__btnW.aboutToBeDeleted();
        this.__btnH.aboutToBeDeleted();
        this.__btn1.aboutToBeDeleted();
        this.__show.aboutToBeDeleted();
        this.__color.aboutToBeDeleted();
        this.__width1.aboutToBeDeleted();
        this.__height1.aboutToBeDeleted();
        this.__opacity1.aboutToBeDeleted();
        this.__borderRaius1.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get btnW() {
        return this.__btnW.get();
    }
    set btnW(newValue) {
        this.__btnW.set(newValue);
    }
    get btnH() {
        return this.__btnH.get();
    }
    set btnH(newValue) {
        this.__btnH.set(newValue);
    }
    get btn1() {
        return this.__btn1.get();
    }
    set btn1(newValue) {
        this.__btn1.set(newValue);
    }
    get show() {
        return this.__show.get();
    }
    set show(newValue) {
        this.__show.set(newValue);
    }
    get color() {
        return this.__color.get();
    }
    set color(newValue) {
        this.__color.set(newValue);
    }
    get width1() {
        return this.__width1.get();
    }
    set width1(newValue) {
        this.__width1.set(newValue);
    }
    get height1() {
        return this.__height1.get();
    }
    set height1(newValue) {
        this.__height1.set(newValue);
    }
    get opacity1() {
        return this.__opacity1.get();
    }
    set opacity1(newValue) {
        this.__opacity1.set(newValue);
    }
    get borderRaius1() {
        return this.__borderRaius1.get();
    }
    set borderRaius1(newValue) {
        this.__borderRaius1.set(newValue);
    }
    render() {
        Flex.create({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, });
        Flex.height(400);
        Flex.width("100%");
        Flex.padding({ top: 100 });
        Button.createWithLabel(this.show);
        Button.onClick(() => {
            Context.animateTo({ duration: 1000 }, () => {
                this.btn1 = !this.btn1;
                if (this.btn1) {
                    this.show = "hide";
                }
                else {
                    this.show = "show";
                }
            });
        });
        Button.pop();
        If.create();
        if (this.btn1) {
            If.branchId(0);
            Button.createWithLabel();
            Button.width("80%");
            Button.height(30);
            Button.transition({ type: TransitionType.Insert, scale: { x: 0, y: 1.0 } });
            Button.transition({ type: TransitionType.Delete, scale: { x: 1.0, y: 0.0 } });
            Button.pop();
        }
        If.pop();
        Button.createWithLabel('animation');
        Context.animation({
            duration: 1000,
            curve: Curve.EaseOut,
            delay: 200,
            iterations: 1,
            playMode: PlayMode.Normal // 动画模式
        });
        Button.width(this.btnW);
        Button.height(this.btnH);
        Button.onClick(() => {
            this.btnW += 50;
        });
        Context.animation(null);
        Button.pop();
        Column.create({ space: 5 });
        Column.width("100%");
        Column.height("100%");
        Column.create();
        Context.animation({ duration: 1000 });
        Column.opacity(this.opacity1);
        Column.backgroundColor(ObservedObject.GetRawObject(this.color));
        Context.animation(null);
        Context.animation({ duration: 2000 });
        Column.width(this.width1);
        Context.animation(null);
        Context.animation({ duration: 3000 });
        Column.height(this.height1);
        Column.borderRadius(this.borderRaius1);
        Context.animation(null);
        Column.onClick(() => {
            this.color = Color.Green;
            this.borderRaius1 = 20;
            this.opacity1 = 0.5;
            this.height1 = 200;
            this.width1 = 200;
        });
        Column.pop();
        Column.pop();
        Flex.pop();
    }
}
loadDocument(new TransitionExample("1", undefined, {}));
`
