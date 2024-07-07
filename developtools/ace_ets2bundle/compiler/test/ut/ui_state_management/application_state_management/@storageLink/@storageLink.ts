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
let varA = AppStorage.Link('varA')
let envLang = AppStorage.Prop('languageCode')

@Entry
@Component
struct MyComponent {
  @StorageLink('varA') varA: number = 2
  @StorageProp('languageCode') lang: string = 'en'
  private label: string = 'count'

  private aboutToAppear() {
    this.label = (this.lang === 'zh') ? '数' : 'Count'
  }

  build() {
    Row({ space: 20 }) {
      Button(this.label + ': ' + this.varA)
        .onClick(() => {
          AppStorage.Set<number>('varA', AppStorage.Get<number>('varA') + 1)
        })
      Button('lang: ' + this.lang)
        .onClick(() => {
          if (this.lang === 'zh') {
            AppStorage.Set<string>('languageCode', 'en')
          } else {
            AppStorage.Set<string>('languageCode', 'zh')
          }
          this.label = (this.lang === 'zh') ? '数' : 'Count'
        })
    }
  }
}
`
exports.expectResult =
`"use strict";
let __generate__Id = 0;
function generateId() {
    return "@storageLink_" + ++__generate__Id;
}
let varA = AppStorage.Link('varA');
let envLang = AppStorage.Prop('languageCode');
class MyComponent extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__varA = AppStorage.SetAndLink('varA', 2, this, "varA");
        this.__lang = AppStorage.SetAndProp('languageCode', 'en', this, "lang");
        this.label = 'count';
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.label !== undefined) {
            this.label = params.label;
        }
    }
    aboutToBeDeleted() {
        this.__varA.aboutToBeDeleted();
        this.__lang.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get varA() {
        return this.__varA.get();
    }
    set varA(newValue) {
        this.__varA.set(newValue);
    }
    get lang() {
        return this.__lang.get();
    }
    set lang(newValue) {
        this.__lang.set(newValue);
    }
    aboutToAppear() {
        this.label = (this.lang === 'zh') ? '数' : 'Count';
    }
    render() {
        Row.create({ space: 20 });
        Button.createWithLabel(this.label + ': ' + this.varA);
        Button.onClick(() => {
            AppStorage.Set('varA', AppStorage.Get('varA') + 1);
        });
        Button.pop();
        Button.createWithLabel('lang: ' + this.lang);
        Button.onClick(() => {
            if (this.lang === 'zh') {
                AppStorage.Set('languageCode', 'en');
            }
            else {
                AppStorage.Set('languageCode', 'zh');
            }
            this.label = (this.lang === 'zh') ? '数' : 'Count';
        });
        Button.pop();
        Row.pop();
    }
}
loadDocument(new MyComponent("1", undefined, {}));
`
