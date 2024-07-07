/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "want_params.h"

#include "ability_base_log_wrapper.h"
#include "base_interfaces.h"
#include "base_object.h"
#include "bool_wrapper.h"
#include "byte_wrapper.h"
#include "double_wrapper.h"
#include "float_wrapper.h"
#include "int_wrapper.h"
#include "long_wrapper.h"
#include "short_wrapper.h"
#include "string_wrapper.h"
#include "zchar_wrapper.h"
#include "remote_object_wrapper.h"
#include "array_wrapper.h"
#include "want_params_wrapper.h"
#include "parcel.h"
#include "securec.h"
#include "string_ex.h"

namespace OHOS {
namespace AAFwk {
const char* FD = "FD";
const char* REMOTE_OBJECT = "RemoteObject";
const char* TYPE_PROPERTY = "type";
const char* VALUE_PROPERTY = "value";
constexpr int32_t MAX_RECURSION_DEPTH = 100;
UnsupportedData::~UnsupportedData()
{
    if (buffer != nullptr) {
        delete[] buffer;
        buffer = nullptr;
    }
}

UnsupportedData::UnsupportedData() = default;

UnsupportedData::UnsupportedData(const UnsupportedData &other) : key(other.key), type(other.type), size(other.size)
{
    buffer = new uint8_t[size];
    if (memcpy_s(buffer, size, other.buffer, size) != EOK) {
        ABILITYBASE_LOGI("copy construct fail due to memcpy");

        key.clear();
        type = 0;
        size = 0;
        delete[] buffer;
        buffer = nullptr;
    }
}

UnsupportedData::UnsupportedData(UnsupportedData &&other)
    : key(std::move(other.key)), type(other.type), size(other.size), buffer(other.buffer)
{
    other.type = 0;
    other.size = 0;
    other.buffer = nullptr;
}

UnsupportedData &UnsupportedData::operator=(const UnsupportedData &other)
{
    if (this == &other) {
        return *this;
    }
    key = other.key;
    type = other.type;
    size = other.size;
    buffer = new uint8_t[size];
    if (memcpy_s(buffer, size, other.buffer, size) != EOK) {
        ABILITYBASE_LOGI("copy assignment fail due to memcpy");

        key.clear();
        type = 0;
        size = 0;
        delete[] buffer;
        buffer = nullptr;
    }
    return *this;
}

UnsupportedData &UnsupportedData::operator=(UnsupportedData &&other)
{
    key = std::move(other.key);
    type = other.type;
    size = other.size;
    std::swap(buffer, other.buffer);

    other.type = 0;
    other.size = 0;
    if (other.buffer) {
        delete[] other.buffer;
        other.buffer = nullptr;
    }
    return *this;
}

std::string WantParams::GetStringByType(const sptr<IInterface> iIt, int typeId)
{
    if (typeId == VALUE_TYPE_BOOLEAN) {
        return static_cast<Boolean *>(IBoolean::Query(iIt))->ToString();
    } else if (typeId == VALUE_TYPE_BYTE) {
        return static_cast<Byte *>(IByte::Query(iIt))->ToString();
    } else if (typeId == VALUE_TYPE_CHAR) {
        return static_cast<Char *>(IChar::Query(iIt))->ToString();
    } else if (typeId == VALUE_TYPE_SHORT) {
        return static_cast<Short *>(IShort::Query(iIt))->ToString();
    } else if (typeId == VALUE_TYPE_INT) {
        return static_cast<Integer *>(IInteger::Query(iIt))->ToString();
    } else if (typeId == VALUE_TYPE_LONG) {
        return static_cast<Long *>(ILong::Query(iIt))->ToString();
    } else if (typeId == VALUE_TYPE_FLOAT) {
        return static_cast<Float *>(IFloat::Query(iIt))->ToString();
    } else if (typeId == VALUE_TYPE_DOUBLE) {
        return static_cast<Double *>(IDouble::Query(iIt))->ToString();
    } else if (typeId == VALUE_TYPE_STRING) {
        return static_cast<String *>(IString::Query(iIt))->ToString();
    } else if (typeId == VALUE_TYPE_ARRAY) {
        return static_cast<Array *>(IArray::Query(iIt))->ToString();
    } else if (typeId == VALUE_TYPE_WANTPARAMS) {
        return static_cast<WantParamWrapper *>(IWantParams::Query(iIt))->ToString();
    } else {
        return "";
    }
    return "";
}
template<typename T1, typename T2, typename T3>
static void SetNewArray(const AAFwk::InterfaceID &id, AAFwk::IArray *orgIArray, sptr<AAFwk::IArray> &ao);
/**
 * @description: A constructor used to create an WantParams instance by using the parameters of an existing
 * WantParams object.
 * @param wantParams  Indicates the existing WantParams object.
 */
WantParams::WantParams(const WantParams &wantParams)
{
    params_.clear();
    NewParams(wantParams, *this);
}

WantParams::WantParams(WantParams && other) noexcept
{
    *this = std::move(other);
}

// inner use function
bool WantParams::NewParams(const WantParams &source, WantParams &dest)
{
    // Deep copy
    for (auto it = source.params_.begin(); it != source.params_.end(); it++) {
        sptr<IInterface> o = it->second;
        if (IString::Query(o) != nullptr) {
            dest.params_[it->first] = String::Box(String::Unbox(IString::Query(o)));
        } else if (IBoolean::Query(o) != nullptr) {
            dest.params_[it->first] = Boolean::Box(Boolean::Unbox(IBoolean::Query(o)));
        } else if (IByte::Query(o) != nullptr) {
            dest.params_[it->first] = Byte::Box(Byte::Unbox(IByte::Query(o)));
        } else if (IChar::Query(o) != nullptr) {
            dest.params_[it->first] = Char::Box(Char::Unbox(IChar::Query(o)));
        } else if (IShort::Query(o) != nullptr) {
            dest.params_[it->first] = Short::Box(Short::Unbox(IShort::Query(o)));
        } else if (IInteger::Query(o) != nullptr) {
            dest.params_[it->first] = Integer::Box(Integer::Unbox(IInteger::Query(o)));
        } else if (ILong::Query(o) != nullptr) {
            dest.params_[it->first] = Long::Box(Long::Unbox(ILong::Query(o)));
        } else if (IFloat::Query(o) != nullptr) {
            dest.params_[it->first] = Float::Box(Float::Unbox(IFloat::Query(o)));
        } else if (IDouble::Query(o) != nullptr) {
            dest.params_[it->first] = Double::Box(Double::Unbox(IDouble::Query(o)));
        } else if (IRemoteObjectWrap::Query(o) != nullptr) {
            dest.params_[it->first] = RemoteObjectWrap::Box(RemoteObjectWrap::UnBox(IRemoteObjectWrap::Query(o)));
        } else if (IWantParams::Query(o) != nullptr) {
            dest.params_[it->first] = WantParamWrapper::Box(WantParamWrapper::Unbox(IWantParams::Query(o)));
        } else if (IArray::Query(o) != nullptr) {
            sptr<IArray> destAO = nullptr;
            if (!NewArrayData(IArray::Query(o), destAO)) {
                continue;
            }
            dest.params_[it->first] = destAO;
        }
    }
    return true;
}  // namespace AAFwk
// inner use
bool WantParams::NewArrayData(IArray *source, sptr<IArray> &dest)
{
    if (Array::IsBooleanArray(source)) {
        SetNewArray<bool, AAFwk::Boolean, AAFwk::IBoolean>(AAFwk::g_IID_IBoolean, source, dest);
    } else if (Array::IsCharArray(source)) {
        SetNewArray<char, AAFwk::Char, AAFwk::IChar>(AAFwk::g_IID_IChar, source, dest);
    } else if (Array::IsByteArray(source)) {
        SetNewArray<byte, AAFwk::Byte, AAFwk::IByte>(AAFwk::g_IID_IByte, source, dest);
    } else if (Array::IsShortArray(source)) {
        SetNewArray<short, AAFwk::Short, AAFwk::IShort>(AAFwk::g_IID_IShort, source, dest);
    } else if (Array::IsIntegerArray(source)) {
        SetNewArray<int, AAFwk::Integer, AAFwk::IInteger>(AAFwk::g_IID_IInteger, source, dest);
    } else if (Array::IsLongArray(source)) {
        SetNewArray<long, AAFwk::Long, AAFwk::ILong>(AAFwk::g_IID_ILong, source, dest);
    } else if (Array::IsFloatArray(source)) {
        SetNewArray<float, AAFwk::Float, AAFwk::IFloat>(AAFwk::g_IID_IFloat, source, dest);
    } else if (Array::IsDoubleArray(source)) {
        SetNewArray<double, AAFwk::Double, AAFwk::IDouble>(AAFwk::g_IID_IDouble, source, dest);
    } else if (Array::IsStringArray(source)) {
        SetNewArray<std::string, AAFwk::String, AAFwk::IString>(AAFwk::g_IID_IString, source, dest);
    } else if (Array::IsWantParamsArray(source)) {
        SetNewArray<WantParams, AAFwk::WantParamWrapper, AAFwk::IWantParams>(AAFwk::g_IID_IWantParams, source, dest);
    } else {
        return false;
    }

    if (dest == nullptr) {
        return false;
    }

    return true;
}
/**
 * @description: A WantParams used to
 *
 * @param other  Indicates the existing WantParams object.
 */
WantParams &WantParams::operator=(const WantParams &other)
{
    if (this != &other) {
        params_.clear();
        NewParams(other, *this);
    }
    return *this;
}

WantParams &WantParams::operator=(WantParams &&other) noexcept
{
    if (this != &other) {
        // free existing resources.
        params_.clear();
        params_ = other.params_;
        // free other resources.
        other.params_.clear();
    }
    return *this;
}

bool WantParams::operator==(const WantParams &other)
{
    if (this->params_.size() != other.params_.size()) {
        return false;
    }
    for (auto itthis : this->params_) {
        auto itother = other.params_.find(itthis.first);
        if (itother == other.params_.end()) {
            return false;
        }
        if (!CompareInterface(itother->second, itthis.second, WantParams::GetDataType(itother->second))) {
            return false;
        }
    }
    return true;
}

int WantParams::GetDataType(const sptr<IInterface> iIt)
{
    if (iIt != nullptr && IBoolean::Query(iIt) != nullptr) {
        return VALUE_TYPE_BOOLEAN;
    } else if (iIt != nullptr && IByte::Query(iIt) != nullptr) {
        return VALUE_TYPE_BYTE;
    } else if (iIt != nullptr && IChar::Query(iIt) != nullptr) {
        return VALUE_TYPE_CHAR;
    } else if (iIt != nullptr && IShort::Query(iIt) != nullptr) {
        return VALUE_TYPE_SHORT;
    } else if (iIt != nullptr && IInteger::Query(iIt) != nullptr) {
        return VALUE_TYPE_INT;
    } else if (iIt != nullptr && ILong::Query(iIt) != nullptr) {
        return VALUE_TYPE_LONG;
    } else if (iIt != nullptr && IFloat::Query(iIt) != nullptr) {
        return VALUE_TYPE_FLOAT;
    } else if (iIt != nullptr && IDouble::Query(iIt) != nullptr) {
        return VALUE_TYPE_DOUBLE;
    } else if (iIt != nullptr && IString::Query(iIt) != nullptr) {
        return VALUE_TYPE_STRING;
    } else if (iIt != nullptr && IArray::Query(iIt) != nullptr) {
        return VALUE_TYPE_ARRAY;
    } else if (iIt != nullptr && IWantParams::Query(iIt) != nullptr) {
        return VALUE_TYPE_WANTPARAMS;
    }

    return VALUE_TYPE_NULL;
}

sptr<IInterface> WantParams::GetInterfaceByType(int typeId, const std::string &value)
{
    if (typeId == VALUE_TYPE_BOOLEAN) {
        return Boolean::Parse(value);
    } else if (typeId == VALUE_TYPE_BYTE) {
        return Byte::Parse(value);
    } else if (typeId == VALUE_TYPE_CHAR) {
        return Char::Parse(value);
    } else if (typeId == VALUE_TYPE_SHORT) {
        return Short::Parse(value);
    } else if (typeId == VALUE_TYPE_INT) {
        return Integer::Parse(value);
    } else if (typeId == VALUE_TYPE_LONG) {
        return Long::Parse(value);
    } else if (typeId == VALUE_TYPE_FLOAT) {
        return Float::Parse(value);
    } else if (typeId == VALUE_TYPE_DOUBLE) {
        return Double::Parse(value);
    } else if (typeId == VALUE_TYPE_STRING) {
        return String::Parse(value);
    } else if (typeId == VALUE_TYPE_ARRAY) {
        return Array::Parse(value);
    }

    return nullptr;
}

bool WantParams::CompareInterface(const sptr<IInterface> iIt1, const sptr<IInterface> iIt2, int typeId)
{
    bool flag = false;
    switch (typeId) {
        case VALUE_TYPE_BOOLEAN:
            flag =
                static_cast<Boolean *>(IBoolean::Query(iIt1))->Equals(*(static_cast<Boolean *>(IBoolean::Query(iIt1))));
            break;
        case VALUE_TYPE_BYTE:
            flag = static_cast<Byte *>(IByte::Query(iIt1))->Equals(*(static_cast<Byte *>(IByte::Query(iIt1))));
            break;
        case VALUE_TYPE_CHAR:
            flag = static_cast<Char *>(IChar::Query(iIt1))->Equals(*(static_cast<Char *>(IChar::Query(iIt1))));
            break;
        case VALUE_TYPE_SHORT:
            flag = static_cast<Short *>(IShort::Query(iIt1))->Equals(*(static_cast<Short *>(IShort::Query(iIt1))));
            break;
        case VALUE_TYPE_INT:
            flag =
                static_cast<Integer *>(IInteger::Query(iIt1))->Equals(*(static_cast<Integer *>(IInteger::Query(iIt1))));
            break;
        case VALUE_TYPE_LONG:
            flag = static_cast<Long *>(ILong::Query(iIt1))->Equals(*(static_cast<Long *>(ILong::Query(iIt1))));
            break;
        case VALUE_TYPE_FLOAT:
            flag = static_cast<Float *>(IFloat::Query(iIt1))->Equals(*(static_cast<Float *>(IFloat::Query(iIt1))));
            break;
        case VALUE_TYPE_DOUBLE:
            flag = static_cast<Double *>(IDouble::Query(iIt1))->Equals(*(static_cast<Double *>(IDouble::Query(iIt1))));
            break;
        case VALUE_TYPE_STRING:
            flag = static_cast<String *>(IString::Query(iIt1))->Equals(*(static_cast<String *>(IString::Query(iIt1))));
            break;
        case VALUE_TYPE_ARRAY:
            flag = static_cast<Array *>(IArray::Query(iIt1))->Equals(*(static_cast<Array *>(IArray::Query(iIt1))));
            break;
        case VALUE_TYPE_WANTPARAMS:
            flag = static_cast<WantParamWrapper *>(IWantParams::Query(iIt1))
                       ->Equals(*(static_cast<WantParamWrapper *>(IWantParams::Query(iIt1))));
            break;
        default:
            break;
    }
    return flag;
}

/**
 * @description: Sets a parameter in key-value pair format.
 * @param key Indicates the key matching the parameter.
 */
void WantParams::SetParam(const std::string &key, IInterface *value)
{
    params_[key] = value;
}

/**
 * @description: Obtains the parameter value based on a given key.
 * @param key Indicates the key matching the parameter.
 * @return Returns the value matching the given key.
 */
sptr<IInterface> WantParams::GetParam(const std::string &key) const
{
    auto it = params_.find(key);
    if (it == params_.cend()) {
        return nullptr;
    }
    return it->second;
}

WantParams WantParams::GetWantParams(const std::string& key) const
{
    auto value = GetParam(key);
    IWantParams *wp = IWantParams::Query(value);
    if (wp != nullptr) {
        return WantParamWrapper::Unbox(wp);
    }
    return WantParams();
}

std::string WantParams::GetStringParam(const std::string& key) const
{
    auto value = GetParam(key);
    IString *ao = IString::Query(value);
    if (ao != nullptr) {
        return String::Unbox(ao);
    }
    return std::string();
}

int WantParams::GetIntParam(const std::string& key, const int defaultValue) const
{
    auto value = GetParam(key);
    IInteger *ao = IInteger::Query(value);
    if (ao != nullptr) {
        return Integer::Unbox(ao);
    }
    return defaultValue;
}

/**
 * @description: Obtains the parameter value based on a given key.
 * @param key Indicates the key matching the parameter.
 * @return Returns the value matching the given key.
 */

const std::map<std::string, sptr<IInterface>> &WantParams::GetParams() const
{
    return params_;
}

/**
 * @description: Obtains a set of the keys of all parameters.
 * @param
 * @return Returns a set of keys.
 */
const std::set<std::string> WantParams::KeySet() const
{
    std::set<std::string> keySet;
    keySet.clear();

    for (auto it : params_) {
        keySet.emplace(it.first);
    }

    return keySet;
}

/**
 * @description: Removes the parameter matching the given key.
 * @param key Indicates the key matching the parameter to be removed.
 */
void WantParams::Remove(const std::string &key)
{
    params_.erase(key);
}

/**
 * @description: Checks whether the Want contains the given key.
 * @param key Indicates the key to check.
 * @return Returns true if the Want contains the key; returns false otherwise.
 */
bool WantParams::HasParam(const std::string &key) const
{
    return (params_.count(key) > 0);
}

/**
 * @description: Obtains the number of parameters contained in this WantParams object.
 * @return Returns the number of parameters.
 */
int WantParams::Size() const
{
    return params_.size();
}

/**
 * @description: Checks whether this WantParams object contains no parameters.
 * @return Returns true if this object does not contain any parameters; returns false otherwise.
 */
bool WantParams::IsEmpty() const
{
    return (params_.size() == 0);
}

bool WantParams::WriteToParcelString(Parcel &parcel, sptr<IInterface> &o) const
{
    std::string value = String::Unbox(IString::Query(o));
    if (!parcel.WriteInt32(VALUE_TYPE_STRING)) {
        return false;
    }
    return parcel.WriteString16(Str8ToStr16(value));
}

bool WantParams::WriteToParcelBool(Parcel &parcel, sptr<IInterface> &o) const
{
    bool value = Boolean::Unbox(IBoolean::Query(o));
    if (!parcel.WriteInt32(VALUE_TYPE_BOOLEAN)) {
        return false;
    }
    return parcel.WriteInt8(value);
}

bool WantParams::WriteToParcelWantParams(Parcel &parcel, sptr<IInterface> &o, int depth) const
{
    WantParams value = WantParamWrapper::Unbox(IWantParams::Query(o));

    auto type = value.GetParam(TYPE_PROPERTY);
    AAFwk::IString *typeP = AAFwk::IString::Query(type);
    if (typeP != nullptr) {
        std::string typeValue = AAFwk::String::Unbox(typeP);
        if (typeValue == FD) {
            return WriteToParcelFD(parcel, value);
        }
        if (typeValue == REMOTE_OBJECT) {
            return WriteToParcelRemoteObject(parcel, value);
        }
    }

    if (!parcel.WriteInt32(VALUE_TYPE_WANTPARAMS)) {
        return false;
    }
    return value.DoMarshalling(parcel, depth + 1);
}

bool WantParams::WriteToParcelFD(Parcel &parcel, const WantParams &value) const
{
    ABILITYBASE_LOGI("%{public}s called.", __func__);
    if (!parcel.WriteInt32(VALUE_TYPE_FD)) {
        return false;
    }

    auto fdWrap = value.GetParam(VALUE_PROPERTY);
    AAFwk::IInteger *fdIWrap = AAFwk::IInteger::Query(fdWrap);
    if (fdIWrap != nullptr) {
        int fd = AAFwk::Integer::Unbox(fdIWrap);
        auto messageParcel = static_cast<MessageParcel*>(&parcel);
        if (messageParcel == nullptr) {
            return false;
        }
        bool ret = messageParcel->WriteFileDescriptor(fd);
        ABILITYBASE_LOGI("%{public}s, WriteFileDescriptor fd:%{public}d, ret:%{public}d.", __func__, fd, ret);
        return ret;
    }

    return false;
}

bool WantParams::WriteToParcelRemoteObject(Parcel &parcel, const WantParams &value) const
{
    ABILITYBASE_LOGI("%{public}s called.", __func__);
    if (!parcel.WriteInt32(VALUE_TYPE_REMOTE_OBJECT)) {
        return false;
    }

    auto remoteObjectWrap = value.GetParam(VALUE_PROPERTY);
    AAFwk::IRemoteObjectWrap *remoteObjectIWrap = AAFwk::IRemoteObjectWrap::Query(remoteObjectWrap);
    if (remoteObjectIWrap != nullptr) {
        auto remoteObject = AAFwk::RemoteObjectWrap::UnBox(remoteObjectIWrap);
        auto messageParcel = static_cast<MessageParcel*>(&parcel);
        if (messageParcel == nullptr) {
            return false;
        }
        bool ret = messageParcel->WriteRemoteObject(remoteObject);
        ABILITYBASE_LOGI("%{public}s, WriteRemoteObject ret:%{public}d.", __func__, ret);
        return ret;
    }
    return false;
}

bool WantParams::WriteToParcelByte(Parcel &parcel, sptr<IInterface> &o) const
{
    byte value = Byte::Unbox(IByte::Query(o));
    if (!parcel.WriteInt32(VALUE_TYPE_BYTE)) {
        return false;
    }
    return parcel.WriteInt8(value);
}

bool WantParams::WriteToParcelChar(Parcel &parcel, sptr<IInterface> &o) const
{
    zchar value = Char::Unbox(IChar::Query(o));
    if (!parcel.WriteInt32(VALUE_TYPE_CHAR)) {
        return false;
    }
    return parcel.WriteInt32(value);
}

bool WantParams::WriteToParcelShort(Parcel &parcel, sptr<IInterface> &o) const
{
    short value = Short::Unbox(IShort::Query(o));
    if (!parcel.WriteInt32(VALUE_TYPE_SHORT)) {
        return false;
    }
    return parcel.WriteInt16(value);
}

bool WantParams::WriteToParcelInt(Parcel &parcel, sptr<IInterface> &o) const
{
    int value = Integer::Unbox(IInteger::Query(o));
    if (!parcel.WriteInt32(VALUE_TYPE_INT)) {
        return false;
    }
    return parcel.WriteInt32(value);
}

bool WantParams::WriteToParcelLong(Parcel &parcel, sptr<IInterface> &o) const
{
    long value = Long::Unbox(ILong::Query(o));
    if (!parcel.WriteInt32(VALUE_TYPE_LONG)) {
        return false;
    }
    return parcel.WriteInt64(value);
}

bool WantParams::WriteToParcelFloat(Parcel &parcel, sptr<IInterface> &o) const
{
    float value = Float::Unbox(IFloat::Query(o));
    if (!parcel.WriteInt32(VALUE_TYPE_FLOAT)) {
        return false;
    }
    return parcel.WriteFloat(value);
}

bool WantParams::WriteToParcelDouble(Parcel &parcel, sptr<IInterface> &o) const
{
    double value = Double::Unbox(IDouble::Query(o));
    if (!parcel.WriteInt32(VALUE_TYPE_DOUBLE)) {
        return false;
    }
    return parcel.WriteDouble(value);
}

bool WantParams::WriteMarshalling(Parcel &parcel, sptr<IInterface> &o, int depth) const
{
    if (IString::Query(o) != nullptr) {
        return WriteToParcelString(parcel, o);
    } else if (IBoolean::Query(o) != nullptr) {
        return WriteToParcelBool(parcel, o);
    } else if (IByte::Query(o) != nullptr) {
        return WriteToParcelByte(parcel, o);
    } else if (IChar::Query(o) != nullptr) {
        return WriteToParcelChar(parcel, o);
    } else if (IShort::Query(o) != nullptr) {
        return WriteToParcelShort(parcel, o);
    } else if (IInteger::Query(o) != nullptr) {
        return WriteToParcelInt(parcel, o);
    } else if (ILong::Query(o) != nullptr) {
        return WriteToParcelLong(parcel, o);
    } else if (IFloat::Query(o) != nullptr) {
        return WriteToParcelFloat(parcel, o);
    } else if (IDouble::Query(o) != nullptr) {
        return WriteToParcelDouble(parcel, o);
    } else if (IWantParams::Query(o) != nullptr) {
        return WriteToParcelWantParams(parcel, o, depth);
    } else {
        IArray *ao = IArray::Query(o);
        if (ao != nullptr) {
            sptr<IArray> array(ao);
            return WriteArrayToParcel(parcel, array, depth);
        } else {
            return true;
        }
    }
}

bool WantParams::DoMarshalling(Parcel &parcel, int depth) const
{
    if (depth >= MAX_RECURSION_DEPTH) {
        return false;
    }
    size_t size = params_.size();
    if (!cachedUnsupportedData_.empty()) {
        size += cachedUnsupportedData_.size();
    }

    if (!parcel.WriteInt32(size)) {
        return false;
    }

    auto iter = params_.cbegin();
    while (iter != params_.cend()) {
        std::string key = iter->first;
        sptr<IInterface> o = iter->second;
        if (!parcel.WriteString16(Str8ToStr16(key))) {
            return false;
        }
        if (!WriteMarshalling(parcel, o, depth)) {
            return false;
        }
        iter++;
    }

    if (!cachedUnsupportedData_.empty()) {
        for (const UnsupportedData &data : cachedUnsupportedData_) {
            if (!parcel.WriteString16(data.key)) {
                return false;
            }
            if (!parcel.WriteInt32(data.type)) {
                return false;
            }
            if (!parcel.WriteInt32(data.size)) {
                return false;
            }
            // Corresponding to Parcel#writeByteArray() in Java.
            if (!parcel.WriteInt32(data.size)) {
                return false;
            }
            if (!parcel.WriteBuffer(data.buffer, data.size)) {
                return false;
            }
        }
    }
    return true;
}

/**
 * @description: Marshals an WantParams object into a Parcel.
 * @param Key-value pairs in the WantParams are marshalled separately.
 * @return If any key-value pair fails to be marshalled, false is returned.
 */
bool WantParams::Marshalling(Parcel &parcel) const
{
    return DoMarshalling(parcel);
}

template<typename dataType, typename className>
static bool SetArray(const InterfaceID &id, const std::vector<dataType> &value, sptr<IArray> &ao)
{
    typename std::vector<dataType>::size_type size = value.size();
    ao = new (std::nothrow) Array(size, id);
    if (ao != nullptr) {
        for (typename std::vector<dataType>::size_type i = 0; i < size; i++) {
            ao->Set(i, className::Box(value[i]));
        }
        return true;
    }
    return false;
}

template<typename T1, typename T2, typename T3>
static void FillArray(IArray *ao, std::vector<T1> &array)
{
    auto func = [&](IInterface *object) {
        if (object != nullptr) {
            T3 *value = T3::Query(object);
            if (value != nullptr) {
                array.push_back(T2::Unbox(value));
            }
        }
    };
    Array::ForEach(ao, func);
}
// inner use template function
template<typename T1, typename T2, typename T3>
static void SetNewArray(const AAFwk::InterfaceID &id, AAFwk::IArray *orgIArray, sptr<AAFwk::IArray> &ao)
{
    if (orgIArray == nullptr) {
        return;
    }
    std::vector<T1> array;
    auto func = [&](IInterface *object) {
        if (object != nullptr) {
            T3 *value = T3::Query(object);
            if (value != nullptr) {
                array.push_back(T2::Unbox(value));
            }
        }
    };
    Array::ForEach(orgIArray, func);

    typename std::vector<T1>::size_type size = array.size();
    if (size > 0) {
        ao = new (std::nothrow) AAFwk::Array(size, id);
        if (ao != nullptr) {
            for (typename std::vector<T1>::size_type i = 0; i < size; i++) {
                ao->Set(i, T2::Box(array[i]));
            }
        }
    }
}

bool WantParams::WriteArrayToParcelString(Parcel &parcel, IArray *ao) const
{
    if (ao == nullptr) {
        return false;
    }

    std::vector<std::u16string> array;
    auto func = [&](IInterface *object) {
        std::string s = String::Unbox(IString::Query(object));
        array.push_back(Str8ToStr16(s));
    };

    Array::ForEach(ao, func);

    if (!parcel.WriteInt32(VALUE_TYPE_STRINGARRAY)) {
        return false;
    }
    return parcel.WriteString16Vector(array);
}

bool WantParams::WriteArrayToParcelBool(Parcel &parcel, IArray *ao) const
{
    if (ao == nullptr) {
        return false;
    }

    std::vector<int8_t> array;
    std::vector<int32_t> intArray;
    FillArray<int8_t, Boolean, IBoolean>(ao, array);
    if (!parcel.WriteInt32(VALUE_TYPE_BOOLEANARRAY)) {
        return false;
    }

    for (std::vector<int8_t>::size_type i = 0; i < array.size(); i++) {
        ABILITYBASE_LOGI("%{public}s bool of array: %{public}d", __func__, array[i]);
        intArray.push_back(array[i]);
    }
    return parcel.WriteInt32Vector(intArray);
}

bool WantParams::WriteArrayToParcelByte(Parcel &parcel, IArray *ao) const
{
    if (ao == nullptr) {
        return false;
    }

    std::vector<int8_t> array;
    FillArray<int8_t, Byte, IByte>(ao, array);
    if (!parcel.WriteInt32(VALUE_TYPE_BYTEARRAY)) {
        return false;
    }
    return parcel.WriteInt8Vector(array);
}

bool WantParams::WriteArrayToParcelChar(Parcel &parcel, IArray *ao) const
{
    if (ao == nullptr) {
        return false;
    }

    std::vector<int32_t> array;
    FillArray<int32_t, Char, IChar>(ao, array);
    if (!parcel.WriteInt32(VALUE_TYPE_CHARARRAY)) {
        return false;
    }
    return parcel.WriteInt32Vector(array);
}

bool WantParams::WriteArrayToParcelShort(Parcel &parcel, IArray *ao) const
{
    if (ao == nullptr) {
        return false;
    }

    std::vector<short> array;
    FillArray<short, Short, IShort>(ao, array);
    if (!parcel.WriteInt32(VALUE_TYPE_SHORTARRAY)) {
        return false;
    }
    return parcel.WriteInt16Vector(array);
}

bool WantParams::WriteArrayToParcelInt(Parcel &parcel, IArray *ao) const
{
    if (ao == nullptr) {
        return false;
    }

    std::vector<int> array;
    FillArray<int, Integer, IInteger>(ao, array);
    if (!parcel.WriteInt32(VALUE_TYPE_INTARRAY)) {
        return false;
    }
    return parcel.WriteInt32Vector(array);
}

bool WantParams::WriteArrayToParcelLong(Parcel &parcel, IArray *ao) const
{
    if (ao == nullptr) {
        return false;
    }

    std::vector<int64_t> array;
    FillArray<int64_t, Long, ILong>(ao, array);
    if (!parcel.WriteInt32(VALUE_TYPE_LONGARRAY)) {
        return false;
    }
    return parcel.WriteInt64Vector(array);
}

bool WantParams::WriteArrayToParcelFloat(Parcel &parcel, IArray *ao) const
{
    if (ao == nullptr) {
        return false;
    }

    std::vector<float> array;
    FillArray<float, Float, IFloat>(ao, array);
    if (!parcel.WriteInt32(VALUE_TYPE_FLOATARRAY)) {
        return false;
    }
    return parcel.WriteFloatVector(array);
}

bool WantParams::WriteArrayToParcelDouble(Parcel &parcel, IArray *ao) const
{
    if (ao == nullptr) {
        return false;
    }

    std::vector<double> array;
    FillArray<double, Double, IDouble>(ao, array);
    if (!parcel.WriteInt32(VALUE_TYPE_DOUBLEARRAY)) {
        return false;
    }
    return parcel.WriteDoubleVector(array);
}

bool WantParams::WriteArrayToParcelWantParams(Parcel &parcel, IArray *ao, int depth) const
{
    if (ao == nullptr) {
        return false;
    }
    if (!parcel.WriteInt32(VALUE_TYPE_WANTPARAMSARRAY)) {
        return false;
    }
    std::vector<WantParams> array;
    auto func = [&](AAFwk::IInterface *object) {
        if (object != nullptr) {
            IWantParams *value = AAFwk::IWantParams::Query(object);
            if (value != nullptr) {
                array.push_back(AAFwk::WantParamWrapper::Unbox(value));
            }
        }
    };
    AAFwk::Array::ForEach(ao, func);
    if (!parcel.WriteInt32(array.size())) {
        return false;
    }

    for (const auto& wp : array) {
        if (!wp.DoMarshalling(parcel, depth + 1)) {
            return false;
        }
    }
    return true;
}

bool WantParams::WriteArrayToParcel(Parcel &parcel, IArray *ao, int depth) const
{
    if (Array::IsStringArray(ao)) {
        return WriteArrayToParcelString(parcel, ao);
    } else if (Array::IsBooleanArray(ao)) {
        return WriteArrayToParcelBool(parcel, ao);
    } else if (Array::IsByteArray(ao)) {
        return WriteArrayToParcelByte(parcel, ao);
    } else if (Array::IsCharArray(ao)) {
        return WriteArrayToParcelChar(parcel, ao);
    } else if (Array::IsShortArray(ao)) {
        return WriteArrayToParcelShort(parcel, ao);
    } else if (Array::IsIntegerArray(ao)) {
        return WriteArrayToParcelInt(parcel, ao);
    } else if (Array::IsLongArray(ao)) {
        return WriteArrayToParcelLong(parcel, ao);
    } else if (Array::IsFloatArray(ao)) {
        return WriteArrayToParcelFloat(parcel, ao);
    } else if (Array::IsDoubleArray(ao)) {
        return WriteArrayToParcelDouble(parcel, ao);
    } else if (Array::IsWantParamsArray(ao)) {
        return WriteArrayToParcelWantParams(parcel, ao, depth);
    } else {
        return true;
    }
}

bool WantParams::ReadFromParcelArrayString(Parcel &parcel, sptr<IArray> &ao)
{
    std::vector<std::u16string> value;
    if (!parcel.ReadString16Vector(&value)) {
        ABILITYBASE_LOGI("%{public}s read string of array fail.", __func__);
        return false;
    }

    std::vector<std::u16string>::size_type size = value.size();
    ao = new (std::nothrow) Array(size, g_IID_IString);
    if (ao != nullptr) {
        for (std::vector<std::u16string>::size_type i = 0; i < size; i++) {
            ao->Set(i, String::Box(Str16ToStr8(value[i])));
        }
        return true;
    } else {
        ABILITYBASE_LOGI("%{public}s create string of array fail.", __func__);
    }
    return false;
}

bool WantParams::ReadFromParcelArrayBool(Parcel &parcel, sptr<IArray> &ao)
{
    std::vector<int32_t> value;
    std::vector<int8_t> boolValue;
    if (!parcel.ReadInt32Vector(&value)) {
        ABILITYBASE_LOGI("%{public}s read bool of array fail.", __func__);
        return false;
    }

    std::vector<int32_t>::size_type size = value.size();
    for (std::vector<int32_t>::size_type i = 0; i < size; i++) {
        boolValue.push_back(value[i]);
    }
    return SetArray<int8_t, Boolean>(g_IID_IBoolean, boolValue, ao);
}

bool WantParams::ReadFromParcelArrayByte(Parcel &parcel, sptr<IArray> &ao)
{
    std::vector<int8_t> value;
    if (!parcel.ReadInt8Vector(&value)) {
        ABILITYBASE_LOGI("%{public}s read byte of array fail.", __func__);
        return false;
    }
    return SetArray<int8_t, Byte>(g_IID_IByte, value, ao);
}

bool WantParams::ReadFromParcelArrayChar(Parcel &parcel, sptr<IArray> &ao)
{
    std::vector<int32_t> value;
    if (!parcel.ReadInt32Vector(&value)) {
        ABILITYBASE_LOGI("%{public}s char bool of array fail.", __func__);
        return false;
    }
    return SetArray<int32_t, Char>(g_IID_IChar, value, ao);
}

bool WantParams::ReadFromParcelArrayShort(Parcel &parcel, sptr<IArray> &ao)
{
    std::vector<short> value;
    if (!parcel.ReadInt16Vector(&value)) {
        ABILITYBASE_LOGI("%{public}s read short of array fail.", __func__);
        return false;
    }
    return SetArray<short, Short>(g_IID_IShort, value, ao);
}

bool WantParams::ReadFromParcelArrayInt(Parcel &parcel, sptr<IArray> &ao)
{
    std::vector<int> value;
    if (!parcel.ReadInt32Vector(&value)) {
        ABILITYBASE_LOGI("%{public}s read int of array fail.", __func__);
        return false;
    }
    return SetArray<int, Integer>(g_IID_IInteger, value, ao);
}

bool WantParams::ReadFromParcelArrayLong(Parcel &parcel, sptr<IArray> &ao)
{
    std::vector<int64_t> value;
    if (!parcel.ReadInt64Vector(&value)) {
        ABILITYBASE_LOGI("%{public}s read long of array fail.", __func__);
        return false;
    }

#ifdef WANT_PARAM_USE_LONG
    return SetArray<int64_t, Long>(g_IID_ILong, value, ao);
#else
    std::vector<std::string> strList;
    for (size_t i = 0; i < value.size(); i++) {
        strList.push_back(std::to_string(value[i]));
    }
    return SetArray<std::string, String>(g_IID_IString, strList, ao);
#endif
}

bool WantParams::ReadFromParcelArrayFloat(Parcel &parcel, sptr<IArray> &ao)
{
    std::vector<float> value;
    if (!parcel.ReadFloatVector(&value)) {
        ABILITYBASE_LOGI("%{public}s read float of array fail.", __func__);
        return false;
    }
    return SetArray<float, Float>(g_IID_IFloat, value, ao);
}

bool WantParams::ReadFromParcelArrayDouble(Parcel &parcel, sptr<IArray> &ao)
{
    std::vector<double> value;
    if (!parcel.ReadDoubleVector(&value)) {
        ABILITYBASE_LOGI("%{public}s read double of array fail.", __func__);
        return false;
    }
    return SetArray<double, Double>(g_IID_IDouble, value, ao);
}

bool WantParams::ReadFromParcelArrayWantParams(Parcel &parcel, sptr<IArray> &ao, int depth)
{
    int32_t size = parcel.ReadInt32();
    static constexpr int32_t maxAllowedSize = 1024;
    if (size < 0 || size > maxAllowedSize) {
        ABILITYBASE_LOGE("%{public}s invalid size: %{public}d", __func__, size);
        return false;
    }
    std::vector<sptr<IInterface>> arrayWantParams;
    for (int32_t i = 0; i < size; ++i) {
        sptr<WantParams> value(Unmarshalling(parcel, depth + 1));
        if (value != nullptr) {
            sptr<IInterface> interface = WantParamWrapper::Box(*value);
            if (interface != nullptr) {
                arrayWantParams.push_back(interface);
            }
        }
    }

    ao = new (std::nothrow) AAFwk::Array(arrayWantParams.size(), AAFwk::g_IID_IWantParams);
    if (ao != nullptr) {
        for (size_t i = 0; i < arrayWantParams.size(); i++) {
            ao->Set(i, arrayWantParams[i]);
        }
        return true;
    }
    return false;
}

bool WantParams::ReadArrayToParcel(Parcel &parcel, int type, sptr<IArray> &ao, int depth)
{
    switch (type) {
        case VALUE_TYPE_STRINGARRAY:
        case VALUE_TYPE_CHARSEQUENCEARRAY:
            return ReadFromParcelArrayString(parcel, ao);
        case VALUE_TYPE_BOOLEANARRAY:
            return ReadFromParcelArrayBool(parcel, ao);
        case VALUE_TYPE_BYTEARRAY:
            return ReadFromParcelArrayByte(parcel, ao);
        case VALUE_TYPE_CHARARRAY:
            return ReadFromParcelArrayChar(parcel, ao);
        case VALUE_TYPE_SHORTARRAY:
            return ReadFromParcelArrayShort(parcel, ao);
        case VALUE_TYPE_INTARRAY:
            return ReadFromParcelArrayInt(parcel, ao);
        case VALUE_TYPE_LONGARRAY:
            return ReadFromParcelArrayLong(parcel, ao);
        case VALUE_TYPE_FLOATARRAY:
            return ReadFromParcelArrayFloat(parcel, ao);
        case VALUE_TYPE_DOUBLEARRAY:
            return ReadFromParcelArrayDouble(parcel, ao);
        case VALUE_TYPE_WANTPARAMSARRAY:
            return ReadFromParcelArrayWantParams(parcel, ao, depth);
        default:
            break;
    }

    return true;
}

bool WantParams::ReadFromParcelString(Parcel &parcel, const std::string &key)
{
    std::u16string value = parcel.ReadString16();
    std::string strValue(Str16ToStr8(value));
    sptr<IInterface> intf = String::Box(Str16ToStr8(value));
    if (intf) {
        SetParam(key, intf);
    } else {
        ABILITYBASE_LOGI("%{public}s read data fail: key=%{public}s", __func__, key.c_str());
    }
    return true;
}

bool WantParams::ReadFromParcelBool(Parcel &parcel, const std::string &key)
{
    int8_t value;
    if (parcel.ReadInt8(value)) {
        sptr<IInterface> intf = Boolean::Box(value);
        if (intf) {
            SetParam(key, intf);
        } else {
            ABILITYBASE_LOGI("%{public}s insert param fail: key=%{public}s", __func__, key.c_str());
        }
        return true;
    } else {
        ABILITYBASE_LOGI("%{public}s read data fail: key=%{public}s", __func__, key.c_str());
        return false;
    }
}

bool WantParams::ReadFromParcelInt8(Parcel &parcel, const std::string &key)
{
    int8_t value;
    if (parcel.ReadInt8(value)) {
        sptr<IInterface> intf = Byte::Box(value);
        if (intf) {
            SetParam(key, intf);
        } else {
            ABILITYBASE_LOGI("%{public}s insert param fail: key=%{public}s", __func__, key.c_str());
        }
        return true;
    } else {
        ABILITYBASE_LOGI("%{public}s read data fail: key=%{public}s", __func__, key.c_str());
        return false;
    }
}

bool WantParams::ReadFromParcelChar(Parcel &parcel, const std::string &key)
{
    int32_t value;
    if (parcel.ReadInt32(value)) {
        sptr<IInterface> intf = Char::Box(value);
        if (intf) {
            SetParam(key, intf);
        } else {
            ABILITYBASE_LOGI("%{public}s insert param fail: key=%{public}s", __func__, key.c_str());
        }
        return true;
    } else {
        ABILITYBASE_LOGI("%{public}s read data fail: key=%{public}s", __func__, key.c_str());
        return false;
    }
}

bool WantParams::ReadFromParcelShort(Parcel &parcel, const std::string &key)
{
    short value;
    if (parcel.ReadInt16(value)) {
        sptr<IInterface> intf = Short::Box(value);
        if (intf) {
            SetParam(key, intf);
        } else {
            ABILITYBASE_LOGI("%{public}s insert param fail: key=%{public}s", __func__, key.c_str());
        }
        return true;
    } else {
        ABILITYBASE_LOGI("%{public}s read data fail: key=%{public}s", __func__, key.c_str());
        return false;
    }
}

bool WantParams::ReadFromParcelInt(Parcel &parcel, const std::string &key)
{
    int value;
    if (parcel.ReadInt32(value)) {
        sptr<IInterface> intf = Integer::Box(value);
        if (intf) {
            SetParam(key, intf);
        } else {
            ABILITYBASE_LOGI("%{public}s insert param fail: key=%{public}s", __func__, key.c_str());
        }
        return true;
    } else {
        ABILITYBASE_LOGI("%{public}s read data fail: key=%{public}s", __func__, key.c_str());
        return false;
    }
}

bool WantParams::ReadFromParcelWantParamWrapper(Parcel &parcel, const std::string &key, int type, int depth)
{
    
    if (type == VALUE_TYPE_FD) {
        return ReadFromParcelFD(parcel, key);
    }

    if (type == VALUE_TYPE_REMOTE_OBJECT) {
        return ReadFromParcelRemoteObject(parcel, key);
    }

    sptr<WantParams> value(Unmarshalling(parcel, depth + 1));
    if (value != nullptr) {
        sptr<IInterface> intf = WantParamWrapper::Box(*value);
        if (intf) {
            SetParam(key, intf);
        }
    }

    return true;
}

bool WantParams::ReadFromParcelFD(Parcel &parcel, const std::string &key)
{
    ABILITYBASE_LOGI("%{public}s called.", __func__);
    auto messageParcel = static_cast<MessageParcel*>(&parcel);
    if (messageParcel == nullptr) {
        return false;
    }
    auto fd = messageParcel->ReadFileDescriptor();
    ABILITYBASE_LOGI("%{public}s fd:%{public}d.", __func__, fd);
    WantParams wp;
    wp.SetParam(TYPE_PROPERTY, String::Box(FD));
    wp.SetParam(VALUE_PROPERTY, Integer::Box(fd));
    sptr<AAFwk::IWantParams> pWantParams = AAFwk::WantParamWrapper::Box(wp);
    SetParam(key, pWantParams);
    return true;
}

bool WantParams::ReadFromParcelRemoteObject(Parcel &parcel, const std::string &key)
{
    ABILITYBASE_LOGI("%{public}s called.", __func__);
    auto messageParcel = static_cast<MessageParcel*>(&parcel);
    if (messageParcel == nullptr) {
        return false;
    }
    auto remoteObject = messageParcel->ReadRemoteObject();
    WantParams wp;
    wp.SetParam(TYPE_PROPERTY, String::Box(REMOTE_OBJECT));
    wp.SetParam(VALUE_PROPERTY, RemoteObjectWrap::Box(remoteObject));
    sptr<AAFwk::IWantParams> pWantParams = AAFwk::WantParamWrapper::Box(wp);
    SetParam(key, pWantParams);
    return true;
}

bool WantParams::ReadFromParcelLong(Parcel &parcel, const std::string &key)
{
    int64_t value;
    if (parcel.ReadInt64(value)) {
        std::string strValue(std::to_string(value));
#ifdef WANT_PARAM_USE_LONG
        sptr<IInterface> intf = Long::Box(value);
#else
        sptr<IInterface> intf = String::Box(std::to_string(value));
#endif
        if (intf) {
            SetParam(key, intf);
        } else {
            ABILITYBASE_LOGI("%{public}s insert param fail: key=%{public}s", __func__, key.c_str());
        }
        return true;
    } else {
        ABILITYBASE_LOGI("%{public}s read data fail: key=%{public}s", __func__, key.c_str());
        return false;
    }
}

bool WantParams::ReadFromParcelFloat(Parcel &parcel, const std::string &key)
{
    float value;
    if (parcel.ReadFloat(value)) {
        sptr<IInterface> intf = Float::Box(value);
        if (intf) {
            SetParam(key, intf);
        } else {
            ABILITYBASE_LOGI("%{public}s insert param fail: key=%{public}s", __func__, key.c_str());
        }
        return true;
    } else {
        ABILITYBASE_LOGI("%{public}s read data fail: key=%{public}s", __func__, key.c_str());
        return false;
    }
}

bool WantParams::ReadFromParcelDouble(Parcel &parcel, const std::string &key)
{
    double value;
    if (parcel.ReadDouble(value)) {
        sptr<IInterface> intf = Double::Box(value);
        if (intf) {
            SetParam(key, intf);
        } else {
            ABILITYBASE_LOGI("%{public}s insert param fail: key=%{public}s", __func__, key.c_str());
        }
        return true;
    } else {
        ABILITYBASE_LOGI("%{public}s read data fail: key=%{public}s", __func__, key.c_str());
        return false;
    }
}

bool WantParams::ReadUnsupportedData(Parcel &parcel, const std::string &key, int type)
{
    int32_t bufferSize = 0;
    if (!parcel.ReadInt32(bufferSize)) {
        return false;
    }
    static constexpr int32_t maxAllowedSize = 100 * 1024 * 1024;
    if (bufferSize < 0 || bufferSize > maxAllowedSize) {
        ABILITYBASE_LOGE("%{public}s invalid size: %{public}d", __func__, bufferSize);
        return false;
    }

    // Corresponding to Parcel#writeByteArray() in Java.
    int32_t length = 0;
    if (!parcel.ReadInt32(length)) {
        return false;
    }
    const uint8_t *bufferP = parcel.ReadUnpadBuffer(bufferSize);
    if (bufferP == nullptr) {
        return false;
    }

    UnsupportedData data;
    data.key = Str8ToStr16(key);
    data.type = type;
    data.size = bufferSize;
    data.buffer = new (std::nothrow) uint8_t[bufferSize];
    if (data.buffer == nullptr) {
        return false;
    }

    if (memcpy_s(data.buffer, bufferSize, bufferP, bufferSize) != EOK) {
        return false;
    }
    cachedUnsupportedData_.emplace_back(std::move(data));
    return true;
}

bool WantParams::ReadFromParcelParam(Parcel &parcel, const std::string &key, int type, int depth)
{
    if (depth >= MAX_RECURSION_DEPTH) {
        return false;
    }
    switch (type) {
        case VALUE_TYPE_CHARSEQUENCE:
        case VALUE_TYPE_STRING:
            return ReadFromParcelString(parcel, key);
        case VALUE_TYPE_BOOLEAN:
            return ReadFromParcelBool(parcel, key);
        case VALUE_TYPE_BYTE:
            return ReadFromParcelInt8(parcel, key);
        case VALUE_TYPE_CHAR:
            return ReadFromParcelChar(parcel, key);
        case VALUE_TYPE_SHORT:
            return ReadFromParcelShort(parcel, key);
        case VALUE_TYPE_INT:
            return ReadFromParcelInt(parcel, key);
        case VALUE_TYPE_LONG:
            return ReadFromParcelLong(parcel, key);
        case VALUE_TYPE_FLOAT:
            return ReadFromParcelFloat(parcel, key);
        case VALUE_TYPE_DOUBLE:
            return ReadFromParcelDouble(parcel, key);
        case VALUE_TYPE_WANTPARAMS:
        case VALUE_TYPE_FD:
        case VALUE_TYPE_REMOTE_OBJECT:
            return ReadFromParcelWantParamWrapper(parcel, key, type, depth);
        case VALUE_TYPE_NULL:
            break;
        case VALUE_TYPE_PARCELABLE:
        case VALUE_TYPE_PARCELABLEARRAY:
        case VALUE_TYPE_SERIALIZABLE:
        case VALUE_TYPE_LIST:
            if (!ReadUnsupportedData(parcel, key, type)) {
                return false;
            }
            break;
        default: {
            // handle array
            sptr<IArray> ao = nullptr;
            if (!ReadArrayToParcel(parcel, type, ao, depth)) {
                return false;
            }
            sptr<IInterface> intf = ao;
            if (intf) {
                SetParam(key, intf);
            }
            break;
        }
    }
    return true;
}

bool WantParams::ReadFromParcel(Parcel &parcel, int depth)
{
    int32_t size;
    if (!parcel.ReadInt32(size)) {
        ABILITYBASE_LOGI("%{public}s read size fail.", __func__);
        return false;
    }
    for (int32_t i = 0; i < size; i++) {
        std::u16string key = parcel.ReadString16();
        int type;
        if (!parcel.ReadInt32(type)) {
            ABILITYBASE_LOGI("%{public}s read type fail.", __func__);
            return false;
        }
        if (!ReadFromParcelParam(parcel, Str16ToStr8(key), type, depth)) {
            ABILITYBASE_LOGI("%{public}s get i=%{public}d fail.", __func__, i);
            return false;
        }
    }
    return true;
}

/**
 * @description: Unmarshals an WantParams object from a Parcel.
 * @param Key-value pairs in the WantParams are unmarshalled separately.
 * @return If any key-value pair fails to be unmarshalled, false is returned.
 */
WantParams *WantParams::Unmarshalling(Parcel &parcel, int depth)
{
    WantParams *wantParams = new (std::nothrow) WantParams();
    if (!wantParams->ReadFromParcel(parcel, depth)) {
        delete wantParams;
        wantParams = nullptr;
    }
    return wantParams;
}

void WantParams::DumpInfo(int level) const
{
    for (auto it : params_) {
        int typeId = WantParams::GetDataType(it.second);
        if (typeId != VALUE_TYPE_NULL) {
            std::string value = WantParams::GetStringByType(it.second, typeId);
            ABILITYBASE_LOGI("=WantParams[%{public}s]:%{private}s =======", it.first.c_str(), value.c_str());
        } else {
            ABILITYBASE_LOGI("=WantParams[%{public}s]:type error =======", it.first.c_str());
        }
    }
}
}  // namespace AAFwk
}  // namespace OHOS
