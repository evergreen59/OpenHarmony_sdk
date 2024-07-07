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

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from '@ohos/hypium'
import ddm from '@ohos.data.distributedKVStore';
export default function FieldNodeTest(){
describe('FieldNodeTest', function() {

    /**
     * @tc.name SUB_DDM_DKV_FIELDNODE_APPENDCHILD_0100
     * @tc.desc  Test Js Api FieldNode.AppendChild() testcase 001
     * @tc.type: FUNC
     * @tc.number: SUB_DDM_DKV_FIELDNODE_APPENDCHILD_0100
     */
    it('SUB_DDM_DKV_FIELDNODE_APPENDCHILD_0100', 0, async function(done) {
        try {
            let node = new ddm.FieldNode("root");
            let child1 = new ddm.FieldNode("child1");
            let child2 = new ddm.FieldNode("child2");
            let child3 = new ddm.FieldNode("child3");
            node.appendChild(child1);
            node.appendChild(child2);
            node.appendChild(child3);
            child1 = null;
            child2 = null;
            child3 = null;
            node = null;
        } catch (e) {
            console.info("SUB_DDM_DKV_FIELDNODE_APPENDCHILD_0100 " + e);
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.name SUB_DDM_DKV_FIELDNODE_APPENDCHILD_0200
     * @tc.desc  Test Js Api FieldNode.AppendChild() testcase 002
     * @tc.type: FUNC
     * @tc.number: SUB_DDM_DKV_FIELDNODE_APPENDCHILD_0200
     */
    it('SUB_DDM_DKV_FIELDNODE_APPENDCHILD_0200', 0, async function(done) {
        try {
            let node = new ddm.FieldNode("root");
            let child = new ddm.FieldNode("child");
            node.appendChild(child);
            child = null;
            node = null;
        } catch (e) {
            console.info("SUB_DDM_DKV_FIELDNODE_APPENDCHILD_0200 " + e);
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.name SUB_DDM_DKV_FIELDNODE_APPENDCHILD_0300
     * @tc.desc  Test Js Api FieldNode.AppendChild() testcase 003
     * @tc.type: FUNC
     * @tc.number: SUB_DDM_DKV_FIELDNODE_APPENDCHILD_0300
     */
    it('SUB_DDM_DKV_FIELDNODE_APPENDCHILD_0300', 0, async function(done) {
        try {
            let node = new ddm.FieldNode("root");
            let child = new ddm.FieldNode();
            node.appendChild(child);
            expect(null).assertFail();
        } catch (e) {
            console.info("SUB_DDM_DKV_FIELDNODE_APPENDCHILD_0300 is ok :" + e);
        }
        done();
    })

    /**
     * @tc.name SUB_DDM_DKV_FIELDNODE_TOJSON_0100
     * @tc.desc  Test Js Api FieldNode.ToJson() testcase 001
     * @tc.type: FUNC
     * @tc.number: SUB_DDM_DKV_FIELDNODE_TOJSON_0100
     */
    it('SUB_DDM_DKV_FIELDNODE_TOJSON_0100', 0, async function(done) {
        try {
            let node = new ddm.FieldNode("root");
            let child = new ddm.FieldNode("child");
            node.appendChild(child);
        } catch (e) {
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.name SUB_DDM_DKV_FIELDNODE_TOJSON_0200
     * @tc.desc  Test Js Api FieldNode.ToJson() testcase 002
     * @tc.type: FUNC
     * @tc.number: SUB_DDM_DKV_FIELDNODE_TOJSON_0200
     */
    it('SUB_DDM_DKV_FIELDNODE_TOJSON_0200', 0, async function(done) {
        try {
            let node = new ddm.FieldNode("root");
            let child = new ddm.FieldNode("child");
            node.appendChild(child);
        } catch (e) {
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.name SUB_DDM_DKV_FIELDNODE_TOJSON_0300
     * @tc.desc  Test Js Api FieldNode.ToJson() testcase 003
     * @tc.type: FUNC
     * @tc.number: SUB_DDM_DKV_FIELDNODE_TOJSON_0300
     */
    it('SUB_DDM_DKV_FIELDNODE_TOJSON_0300', 0, async function(done) {
        try {
            let node = new ddm.FieldNode();
            let child = new ddm.FieldNode();
            node.appendChild(child);
            expect(null).assertFail();
        } catch (e) {
            console.info("SUB_DDM_DKV_FIELDNODE_TOJSON_0300 is ok : " + e);
        }
        done();
    })

    /**
     * @tc.name SUB_DDM_DKV_FIELDNODE_DEFAULT_0100
     * @tc.desc  Test Js Api FieldNode.default testcase 001
     * @tc.type: FUNC
     * @tc.number: SUB_DDM_DKV_FIELDNODE_DEFAULT_0100
     */
     it('SUB_DDM_DKV_FIELDNODE_DEFAULT_0100', 0, async function(done) {
        try {
            let node = new ddm.FieldNode('first');
            node.default = 'first name';
            console.info('defaultValue = ' + node.default);
            expect(node.default === 'first name').assertTrue()
        } catch (e) {
            console.info("SUB_DDM_DKV_FIELDNODE_DEFAULT_0100 fail on exception: " + e);
        }
        done();
    })

    /**
     * @tc.name SUB_DDM_DKV_FIELDNODE_DEFAULT_0200
     * @tc.desc  Test Js Api FieldNode.nullable testcase 001
     * @tc.type: FUNC
     * @tc.number: SUB_DDM_DKV_FIELDNODE_DEFAULT_0200
     */
     it('SUB_DDM_DKV_FIELDNODE_DEFAULT_0200', 0, async function(done) {
        try {
            let node = new ddm.FieldNode('first');
            node.nullable = false;
            console.info('nullable = ' + node.nullable);
            expect(node.nullable === false).assertTrue()
        } catch (e) {
            console.info("SUB_DDM_DKV_FIELDNODE_DEFAULT_0200 fail on exception: " + e);
        }
        done();
    })

    /**
     * @tc.name SUB_DDM_DKV_FIELDNODE_TYPE_0100
     * @tc.desc  Test Js Api FieldNode.type testcase 001
     * @tc.type: FUNC
     * @tc.number: SUB_DDM_DKV_FIELDNODE_TYPE_0100
     */
     it('SUB_DDM_DKV_FIELDNODE_TYPE_0100', 0, async function(done) {
        try {
            let node = new ddm.FieldNode('first');
            node.type = ddm.ValueType.STRING;
            console.info('SUB_DDM_DKV_FIELDNODE_TYPE_0100 type = ' + node.type);
            expect(node.type === ddm.ValueType.STRING).assertTrue();
        } catch (e) {
            console.info("SUB_DDM_DKV_FIELDNODE_TYPE_0100 fail on exception: " + e);
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.name SUB_DDM_DKV_FIELDNODE_TYPE_0200
     * @tc.desc  Test Js Api FieldNode.type testcase 002
     * @tc.type: FUNC
     * @tc.number: SUB_DDM_DKV_FIELDNODE_TYPE_0200
     */
     it('SUB_DDM_DKV_FIELDNODE_TYPE_0200', 0, async function(done) {
        try {
            let node = new ddm.FieldNode('first');
            node.type = ddm.ValueType.INTEGER;
            console.info('SUB_DDM_DKV_FIELDNODE_TYPE_0200 type = ' + node.type);
            expect(node.type === ddm.ValueType.INTEGER).assertTrue();
        } catch (e) {
            console.info("SUB_DDM_DKV_FIELDNODE_TYPE_0200 fail on exception: " + e);
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.name SUB_DDM_DKV_FIELDNODE_TYPE_0300
     * @tc.desc  Test Js Api FieldNode.type testcase 003
     * @tc.type: FUNC
     * @tc.number: SUB_DDM_DKV_FIELDNODE_TYPE_0300
     */
     it('SUB_DDM_DKV_FIELDNODE_TYPE_0300', 0, async function(done) {
        try {
            let node = new ddm.FieldNode('first');
            node.type = ddm.ValueType.FLOAT;
            console.info('SUB_DDM_DKV_FIELDNODE_TYPE_0300 type = ' + node.type);
            expect(node.type === ddm.ValueType.FLOAT).assertTrue();
        } catch (e) {
            console.info("SUB_DDM_DKV_FIELDNODE_TYPE_0300 fail on exception: " + e);
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.name SUB_DDM_DKV_FIELDNODE_TYPE_0400
     * @tc.desc  Test Js Api FieldNode.type testcase 004
     * @tc.type: FUNC
     * @tc.number: SUB_DDM_DKV_FIELDNODE_TYPE_0400
     */
     it('SUB_DDM_DKV_FIELDNODE_TYPE_0400', 0, async function(done) {
        try {
            let node = new ddm.FieldNode('first');
            node.type = ddm.ValueType.BYTE_ARRAY;
            console.info('SUB_DDM_DKV_FIELDNODE_TYPE_0400 type = ' + node.type);
            expect(node.type === ddm.ValueType.BYTE_ARRAY).assertTrue();
        } catch (e) {
            console.info("SUB_DDM_DKV_FIELDNODE_TYPE_0400 fail on exception: " + e);
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.name SUB_DDM_DKV_FIELDNODE_TYPE_0500
     * @tc.desc  Test Js Api FieldNode.type testcase 005
     * @tc.type: FUNC
     * @tc.number: SUB_DDM_DKV_FIELDNODE_TYPE_0500
     */
     it('SUB_DDM_DKV_FIELDNODE_TYPE_0500', 0, async function(done) {
        try {
            let node = new ddm.FieldNode('first');
            node.type = ddm.ValueType.BOOLEAN;
            console.info('SUB_DDM_DKV_FIELDNODE_TYPE_0500 type = ' + node.type);
            expect(node.type === ddm.ValueType.BOOLEAN).assertTrue();
        } catch (e) {
            console.info("SUB_DDM_DKV_FIELDNODE_TYPE_0500 fail on exception: " + e);
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.name SUB_DDM_DKV_FIELDNODE_TYPE_0600
     * @tc.desc  Test Js Api FieldNode.type testcase 006
     * @tc.type: FUNC
     * @tc.number: SUB_DDM_DKV_FIELDNODE_TYPE_0600
     */
     it('SUB_DDM_DKV_FIELDNODE_TYPE_0600', 0, async function(done) {
        try {
            let node = new ddm.FieldNode('first');
            node.type = ddm.ValueType.DOUBLE;
            console.info('SUB_DDM_DKV_FIELDNODE_TYPE_0600 type = ' + node.type);
            expect(node.type === ddm.ValueType.DOUBLE).assertTrue();
        } catch (e) {
            console.info("SUB_DDM_DKV_FIELDNODE_TYPE_0600 fail on exception: " + e);
            expect(null).assertFail();
        }
        done();
    })
})
}
