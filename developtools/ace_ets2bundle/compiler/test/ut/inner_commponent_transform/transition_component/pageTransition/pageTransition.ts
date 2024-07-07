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
struct PageTransitionExample1 {
  @State scale: number = 1
  @State opacity: number = 1
  @State active: boolean = false
  build() {
    Column() {
      Navigator({ target: 'pages/page1', type: NavigationType.Push }) {
        Text('page transition').width("100%").height("100%")
      }
      .onClick(() => {
        this.active = true
      })
    }.scale({ x: this.scale }).opacity(this.opacity)
  }
  pageTransition() {
    PageTransitionEnter({ duration: 1200, curve: Curve.Linear })
      .onEnter((type: RouteType, progress: number) => {
        this.scale = 1
        this.opacity = progress
      })
    PageTransitionExit({ duration: 1500, curve: Curve.Ease })
      .onExit((type: RouteType, progress: number) => {
        this.scale = 1 - progress
        this.opacity = 1
      })
  }
}
`

exports.expectResult =
`"use strict";
let __generate__Id = 0;
function generateId() {
    return "pageTransition_" + ++__generate__Id;
}
class PageTransitionExample1 extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__scale = new ObservedPropertySimple(1, this, "scale");
        this.__opacity = new ObservedPropertySimple(1, this, "opacity");
        this.__active = new ObservedPropertySimple(false, this, "active");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.scale !== undefined) {
            this.scale = params.scale;
        }
        if (params.opacity !== undefined) {
            this.opacity = params.opacity;
        }
        if (params.active !== undefined) {
            this.active = params.active;
        }
    }
    aboutToBeDeleted() {
        this.__scale.aboutToBeDeleted();
        this.__opacity.aboutToBeDeleted();
        this.__active.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get scale() {
        return this.__scale.get();
    }
    set scale(newValue) {
        this.__scale.set(newValue);
    }
    get opacity() {
        return this.__opacity.get();
    }
    set opacity(newValue) {
        this.__opacity.set(newValue);
    }
    get active() {
        return this.__active.get();
    }
    set active(newValue) {
        this.__active.set(newValue);
    }
    render() {
        Column.create();
        Column.scale({ x: this.scale });
        Column.opacity(this.opacity);
        Navigator.create({ target: 'pages/page1', type: NavigationType.Push });
        Navigator.onClick(() => {
            this.active = true;
        });
        Text.create('page transition');
        Text.width("100%");
        Text.height("100%");
        Text.pop();
        Navigator.pop();
        Column.pop();
    }
    pageTransition() {
        PageTransition.create();
        PageTransitionEnter.create({ duration: 1200, curve: Curve.Linear });
        PageTransitionEnter.onEnter((type, progress) => {
            this.scale = 1;
            this.opacity = progress;
        });
        PageTransitionExit.create({ duration: 1500, curve: Curve.Ease });
        PageTransitionExit.onExit((type, progress) => {
            this.scale = 1 - progress;
            this.opacity = 1;
        });
        PageTransition.pop();
    }
}
loadDocument(new PageTransitionExample1("1", undefined, {}));
`
