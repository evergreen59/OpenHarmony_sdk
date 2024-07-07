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
struct HomePreviewComponent {
  private value: string = "hello world"

  build() {
    Text(this.value)
        .fontSize(50)
  }
}

@Preview
@Component
struct HomePreviewComponent_Preview {
  build() {
    Column() {
      HomePreviewComponent();
    }
  }
}
`

exports.expectResult =
`"use strict";
let __generate__Id = 0;
function generateId() {
    return "@preview_" + ++__generate__Id;
}
class HomePreviewComponent extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.value = "hello world";
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.value !== undefined) {
            this.value = params.value;
        }
    }
    aboutToBeDeleted() {
        SubscriberManager.Get().delete(this.id());
    }
    render() {
        Text.create(this.value);
        Text.fontSize(50);
        Text.pop();
    }
}
class HomePreviewComponent_Preview extends View {
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
        let earlierCreatedChild_2 = (this && this.findChildById) ? this.findChildById("2") : undefined;
        if (earlierCreatedChild_2 == undefined) {
            View.create(new HomePreviewComponent("2", this, {}));
        }
        else {
            earlierCreatedChild_2.updateWithValueParams({});
            if (!earlierCreatedChild_2.needsUpdate()) {
                earlierCreatedChild_2.markStatic();
            }
            View.create(earlierCreatedChild_2);
        }
        Column.pop();
    }
}
loadDocument(new HomePreviewComponent("1", undefined, {}));
`
