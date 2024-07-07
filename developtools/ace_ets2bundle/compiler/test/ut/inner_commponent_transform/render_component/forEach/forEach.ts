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
class Month {
  year: number
  month: number
  days: Array<number>

  constructor(year, month, days) {
    this.year = year;
    this.month = month;
    this.days = days;
  }
}

@Entry
@Component
struct MyComponent {
  private languages: string[] = ['ets', 'js', 'java']
  private weekNames: string[] = ['日', '一', '二', '三', '四', '五', '六']
  @State calendar: Month[] = [
    new Month(2020, 1, [...Array(31).keys()]),
    new Month(2020, 2, [...Array(28).keys()]),
    new Month(2020, 3, [...Array(31).keys()]),
    new Month(2020, 4, [...Array(30).keys()]),
    new Month(2020, 5, [...Array(31).keys()]),
    new Month(2020, 6, [...Array(30).keys()]),
  ]

  build() {
    Column() {
      Row(){
        ForEach(
          this.languages,
          (item: string, index: number) => {
            GridItem() {
              Text(item)
                .fontSize(30)
            }
          },
          (item: any, index: number) => item)
      }
      Row(){
        ForEach(
          this.weekNames,
          (item, index) => {
            GridItem() {
              if(item === '日'){
                Text(item)
                  .fontSize(20)
                  .fontColor(Color.Red)
              }else{
                Text(item)
                  .fontSize(10)
              }
            }
          })
      }
      Row() {
        Button('next month')
          .onClick(() => {
            this.calendar.shift()
            this.calendar.push({
              year: 2020,
              month: 7,
              days: [...Array(31)
                .keys()]
            })
          })
        ForEach(this.calendar,
          (item: Month) => {
            Text('month:' + item.month)
            Grid() {
              ForEach(item.days,
                (day: number) => {
                  GridItem() {
                    Text((day + 1).toString())
                      .fontSize(30)
                  }
                },
                (day: number) => day.toString())
            }
            .rowsGap(20)
          },
          (item: Month) => (item.year * 12 + item.month).toString())
      }
    }.width(100)
  }
}`

exports.expectResult =
`"use strict";
let __generate__Id = 0;
function generateId() {
    return "forEach_" + ++__generate__Id;
}
class Month {
    constructor(year, month, days) {
        this.year = year;
        this.month = month;
        this.days = days;
    }
}
class MyComponent extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.languages = ['ets', 'js', 'java'];
        this.weekNames = ['日', '一', '二', '三', '四', '五', '六'];
        this.__calendar = new ObservedPropertyObject([
            new Month(2020, 1, [...Array(31).keys()]),
            new Month(2020, 2, [...Array(28).keys()]),
            new Month(2020, 3, [...Array(31).keys()]),
            new Month(2020, 4, [...Array(30).keys()]),
            new Month(2020, 5, [...Array(31).keys()]),
            new Month(2020, 6, [...Array(30).keys()]),
        ], this, "calendar");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.languages !== undefined) {
            this.languages = params.languages;
        }
        if (params.weekNames !== undefined) {
            this.weekNames = params.weekNames;
        }
        if (params.calendar !== undefined) {
            this.calendar = params.calendar;
        }
    }
    aboutToBeDeleted() {
        this.__calendar.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get calendar() {
        return this.__calendar.get();
    }
    set calendar(newValue) {
        this.__calendar.set(newValue);
    }
    render() {
        Column.create();
        Column.width(100);
        Row.create();
        ForEach.create("2", this, ObservedObject.GetRawObject(this.languages), (item, index) => {
            GridItem.create();
            Text.create(item);
            Text.fontSize(30);
            Text.pop();
            GridItem.pop();
        }, (item, index) => item);
        ForEach.pop();
        Row.pop();
        Row.create();
        ForEach.create("3", this, ObservedObject.GetRawObject(this.weekNames), (item, index) => {
            GridItem.create();
            If.create();
            if (item === '日') {
                If.branchId(0);
                Text.create(item);
                Text.fontSize(20);
                Text.fontColor(Color.Red);
                Text.pop();
            }
            else {
                If.branchId(1);
                Text.create(item);
                Text.fontSize(10);
                Text.pop();
            }
            If.pop();
            GridItem.pop();
        });
        ForEach.pop();
        Row.pop();
        Row.create();
        Button.createWithLabel('next month');
        Button.onClick(() => {
            this.calendar.shift();
            this.calendar.push({
                year: 2020,
                month: 7,
                days: [...Array(31)
                        .keys()]
            });
        });
        Button.pop();
        ForEach.create("5", this, ObservedObject.GetRawObject(this.calendar), (item) => {
            Text.create('month:' + item.month);
            Text.pop();
            Grid.create();
            Grid.rowsGap(20);
            ForEach.create("4", this, ObservedObject.GetRawObject(item.days), (day) => {
                GridItem.create();
                Text.create((day + 1).toString());
                Text.fontSize(30);
                Text.pop();
                GridItem.pop();
            }, (day) => day.toString());
            ForEach.pop();
            Grid.pop();
        }, (item) => (item.year * 12 + item.month).toString());
        ForEach.pop();
        Row.pop();
        Column.pop();
    }
}
loadDocument(new MyComponent("1", undefined, {}));
`
