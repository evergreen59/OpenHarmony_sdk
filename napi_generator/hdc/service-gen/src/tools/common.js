/*
* Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd. 
* Licensed under the Apache License, Version 2.0 (the "License"); 
* you may not use this file except in compliance with the License. 
* You may obtain a copy of the License at 
*
* http://www.apache.org/licenses/LICENSE-2.0 
*
* Unless required by applicable law or agreed to in writing, software 
* distributed under the License is distributed on an "AS IS" BASIS, 
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
* See the License for the specific language governing permissions and 
* limitations under the License. 
*/
// remote消息中变量类型名(key)与对应的读parcel方法名(value)的映射（参考parcel.h）
const DATA_READ_MAP = new Map(
    [["int8_t", "ReadInt8Unaligned"], ["uint8_t", "ReadUint8Unaligned"],
     ["int16_t", "ReadInt16Unaligned"], ["uint16_t", "ReadUint16Unaligned"],
     ["int32_t", "ReadInt32"], ["uint32_t", "ReadUint32"], ["int64_t", "ReadInt64"], ["uint64_t", "ReadUint64"],
     ["float", "ReadFloat"], ["double", "ReadDouble"], ["char *", "ReadCString"], ["std::string", "ReadString"],
     ["string", "ReadString"], ["bool", "ReadBoolUnaligned"]
]);

// remote消息中变量类型名(key)与对应的写parcel方法名(value)的映射（参考parcel.h）
const DATA_WRITE_MAP = new Map(
    [["int8_t", "WriteInt8Unaligned"], ["uint8_t", "ReadUint8Unaligned"],
     ["int16_t", "WriteInt16Unaligned"], ["uint16_t", "WriteUint16Unaligned"],
     ["int32_t", "WriteInt32"], ["uint32_t", "WriteUint32"], ["int64_t", "WriteInt64"], ["uint64_t", "WriteUint64"],
     ["float", "WriteFloat"], ["double", "WriteDouble"], ["char *", "WriteCString"], ["std::string", "WriteString"],
     ["string", "WriteString"], ["bool", "WriteBoolUnaligned"]
]);

// 常用类型转换表, 将C语言常见类型(key)转换为remote data读写函数使用的类型(value)
// 例如 ErrCode 类型在框架中的系统原型为int类型，这里映射成int32_t，
// 因为int32_t类型在 DATA_WRITE_MAP/DATA_READ_MAP 表中有对应的读写数据方法(WriteInt32/ReadInt32)
const TYPE_DEF_MAP = new Map(
    [["ErrCode", "int32_t"], ["char", "int8_t"], ["short", "int16_t"], ["int", "int32_t"], ["long", "int64_t"],
    ["unsigned char", "uint8_t"], ["unsigned short", "uint16_t"], ["unsigned int", "uint32_t"], 
    ["unsigned long", "uint64_t"]
]);

function getParcelType(srcType) {
    let parcelType = TYPE_DEF_MAP.get(srcType);
    return parcelType === undefined ? srcType : parcelType;
}

module.exports = {
    DATA_WRITE_MAP, DATA_READ_MAP, getParcelType
}