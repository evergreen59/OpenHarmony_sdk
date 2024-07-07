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
let storage = LocalStorage.GetShared();

class ClassA {
    public id: number = 1;
    public type: number = 2;
    public a: string = "aaa";
    constructor(a: string){
        this.a = a;
    }
}

@Entry(storage)
@Component
struct LocalStorageComponent {
    @LocalStorageLink("storageSimpleProp") simpleVarName: number = 0;
    @LocalStorageProp("storageObjectProp") objectName: ClassA = new ClassA("x");
    build() {
        Column() {
            Text(this.objectName.a)
                .onClick(()=>{
                    this.simpleVarName +=1;
                    this.objectName.a = this.objectName.a === 'x' ? 'yex' : 'no';
                })
        }
        .height(500)
    }
}
`
exports.expectResult =
`"use strict";
let __generate__Id = 0;
function generateId() {
    return "localStorage_" + ++__generate__Id;
}
let storage = LocalStorage.GetShared();
class ClassA {
    constructor(a) {
        this.id = 1;
        this.type = 2;
        this.a = "aaa";
        this.a = a;
    }
}
class LocalStorageComponent extends View {
    constructor(compilerAssignedUniqueChildId, parent, params, localStorage) {
        super(compilerAssignedUniqueChildId, parent, localStorage);
        this.__simpleVarName = this.localStorage_.setAndLink("storageSimpleProp", 0, this, "simpleVarName");
        this.__objectName = this.localStorage_.setAndProp("storageObjectProp", new ClassA("x"), this, "objectName");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
    }
    aboutToBeDeleted() {
        this.__simpleVarName.aboutToBeDeleted();
        this.__objectName.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get simpleVarName() {
        return this.__simpleVarName.get();
    }
    set simpleVarName(newValue) {
        this.__simpleVarName.set(newValue);
    }
    get objectName() {
        return this.__objectName.get();
    }
    set objectName(newValue) {
        this.__objectName.set(newValue);
    }
    render() {
        Column.create();
        Column.height(500);
        Text.create(this.objectName.a);
        Text.onClick(() => {
            this.simpleVarName += 1;
            this.objectName.a = this.objectName.a === 'x' ? 'yex' : 'no';
        });
        Text.pop();
        Column.pop();
    }
}
loadDocument(new LocalStorageComponent("1", undefined, {}, storage));
`
