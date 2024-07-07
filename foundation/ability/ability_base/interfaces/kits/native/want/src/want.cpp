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

#include "want.h"

#include <algorithm>
#include <climits>
#include <cstdlib>
#include <regex>
#include <securec.h>

#include "base_object.h"
#include "bool_wrapper.h"
#include "zchar_wrapper.h"
#include "byte_wrapper.h"
#include "short_wrapper.h"
#include "int_wrapper.h"
#include "long_wrapper.h"
#include "float_wrapper.h"
#include "double_wrapper.h"
#include "string_wrapper.h"
#include "zchar_wrapper.h"
#include "array_wrapper.h"
#include "parcel_macro_base.h"
#include "remote_object_wrapper.h"
#include "string_ex.h"
#include "want_params_wrapper.h"

using namespace OHOS::AppExecFwk;
using OHOS::AppExecFwk::ElementName;

namespace OHOS {
namespace AAFwk {
namespace {
const std::regex NUMBER_REGEX("^[-+]?([0-9]+)([.]([0-9]+))?$");
};  // namespace
const std::string Want::ACTION_PLAY("action.system.play");
const std::string Want::ACTION_HOME("action.system.home");

const std::string Want::ENTITY_HOME("entity.system.home");
const std::string Want::ENTITY_VIDEO("entity.system.video");
const std::string Want::FLAG_HOME_INTENT_FROM_SYSTEM("flag.home.intent.from.system");
const std::string Want::ENTITY_MUSIC("entity.app.music");
const std::string Want::ENTITY_EMAIL("entity.app.email");
const std::string Want::ENTITY_CONTACTS("entity.app.contacts");
const std::string Want::ENTITY_MAPS("entity.app.maps");
const std::string Want::ENTITY_BROWSER("entity.app.browser");
const std::string Want::ENTITY_CALENDAR("entity.app.calendar");
const std::string Want::ENTITY_MESSAGING("entity.app.messaging");
const std::string Want::ENTITY_FILES("entity.app.files");
const std::string Want::ENTITY_GALLERY("entity.app.gallery");

const std::string Want::OCT_EQUALSTO("075");   // '='
const std::string Want::OCT_SEMICOLON("073");  // ';'
const std::string Want::MIME_TYPE("mime-type");
const std::string Want::WANT_HEADER("#Intent;");
const std::string Want::WANT_END(";end");

const std::string Want::PARAM_RESV_WINDOW_MODE("ohos.aafwk.param.windowMode");
const std::string Want::PARAM_RESV_DISPLAY_ID("ohos.aafwk.param.displayId");
const std::string Want::PARAM_RESV_CALLER_TOKEN("ohos.aafwk.param.callerToken");
const std::string Want::PARAM_RESV_CALLER_UID("ohos.aafwk.param.callerUid");
const std::string Want::PARAM_RESV_CALLER_PID("ohos.aafwk.param.callerPid");
const std::string Want::PARAM_RESV_FOR_RESULT("ohos.aafwk.param.startAbilityForResult");
const std::string Want::PARAM_RESV_CALL_TO_FOREGROUND("ohos.aafwk.param.callAbilityToForeground");
const std::string Want::PARAM_RESV_REQUEST_PROC_CODE("ohos.aafwk.param.requestProcCode");
const std::string Want::PARAM_RESV_REQUEST_TOKEN_CODE("ohos.aafwk.param.requestTokenCode");
const std::string Want::PARAM_RESV_ABILITY_INFO_CALLBACK("ohos.aafwk.param.abilityInfoCallback");
const std::string Want::PARAM_RESV_START_RECENT("ohos.aafwk.param.startRecent");

const std::string Want::PARAM_ABILITY_RECOVERY_RESTART("ohos.aafwk.param.AbilityRecoveryRestart");
const std::string Want::PARAM_MODULE_NAME("moduleName");

const std::string Want::PARAM_BACK_TO_OTHER_MISSION_STACK("ability.params.backToOtherMissionStack");

/**
 * @description:Default construcotr of Want class, which is used to initialzie flags and URI.
 * @param None
 * @return None
 */
Want::Want()
{}

/**
 * @description: Default deconstructor of Want class
 * @param None
 * @return None
 */
Want::~Want()
{}

/**
 * @description: Copy construcotr of Want class, which is used to initialzie flags, URI, etc.
 * @param want the source instance of Want.
 * @return None
 */
Want::Want(const Want &want)
{
    operation_ = want.operation_;
    parameters_ = want.parameters_;
}

Want &Want::operator=(const Want &want)
{
    operation_ = want.operation_;
    parameters_ = want.parameters_;
    return *this;
}

/**
 * @description: Obtains the description of flags in a Want.
 * @return Returns the flag description in the Want.
 */
unsigned int Want::GetFlags() const
{
    return operation_.GetFlags();
}

/**
 * @description: Sets a flag in a Want.
 * @param flags Indicates the flag to set.
 * @return Returns this Want object containing the flag.
 */
Want &Want::SetFlags(unsigned int flags)
{
    operation_.SetFlags(flags);
    return *this;
}

/**
 * @description: Adds a flag to a Want.
 * @param flags Indicates the flag to add.
 * @return Returns the Want object with the added flag.
 */
Want &Want::AddFlags(unsigned int flags)
{
    operation_.AddFlags(flags);
    return *this;
}

/**
 * @description: Removes the description of a flag from a Want.
 * @param flags Indicates the flag to remove.
 * @return Removes the description of a flag from a Want.
 */
void Want::RemoveFlags(unsigned int flags)
{
    operation_.RemoveFlags(flags);
}

/**
 * @description: Obtains the description of the ElementName object in a Want.
 * @return Returns the ElementName description in the Want.
 */
OHOS::AppExecFwk::ElementName Want::GetElement() const
{
    return ElementName(operation_.GetDeviceId(), operation_.GetBundleName(),
        operation_.GetAbilityName(), operation_.GetModuleName());
}

/**
 * @description: Sets the bundleName and abilityName attributes for this Want object.
 * @param bundleName Indicates the bundleName to set for the operation attribute in the Want.
 * @param abilityName Indicates the abilityName to set for the operation attribute in the Want.
 * @return Returns this Want object that contains the specified bundleName and abilityName attributes.
 */
Want &Want::SetElementName(const std::string &bundleName, const std::string &abilityName)
{
    operation_.SetBundleName(bundleName);
    operation_.SetAbilityName(abilityName);
    return *this;
}

/**
 * @description: Sets the bundleName and abilityName attributes for this Want object.
 * @param deviceId Indicates the deviceId to set for the operation attribute in the Want.
 * @param bundleName Indicates the bundleName to set for the operation attribute in the Want.
 * @param abilityName Indicates the abilityName to set for the operation attribute in the Want.
 * @return Returns this Want object that contains the specified bundleName and abilityName attributes.
 */
Want &Want::SetElementName(const std::string &deviceId, const std::string &bundleName,
    const std::string &abilityName, const std::string &moduleName)
{
    operation_.SetDeviceId(deviceId);
    operation_.SetBundleName(bundleName);
    operation_.SetAbilityName(abilityName);
    operation_.SetModuleName(moduleName);
    SetParam(PARAM_MODULE_NAME, moduleName);
    return *this;
}

/**
 * @description: Sets an ElementName object in a Want.
 * @param element Indicates the ElementName description.
 * @return Returns this Want object containing the ElementName
 */
Want &Want::SetElement(const OHOS::AppExecFwk::ElementName &element)
{
    operation_.SetDeviceId(element.GetDeviceID());
    operation_.SetBundleName(element.GetBundleName());
    operation_.SetAbilityName(element.GetAbilityName());
    operation_.SetModuleName(element.GetModuleName());
    SetParam(PARAM_MODULE_NAME, element.GetModuleName());
    return *this;
}

/**
 * @description: Obtains the description of all entities in a Want.
 * @return Returns a set of entities
 */
const std::vector<std::string> &Want::GetEntities() const
{
    return operation_.GetEntities();
}

/**
 * @description: Adds the description of an entity to a Want
 * @param entity Indicates the entity description to add
 * @return Returns this Want object containing the entity.
 */
Want &Want::AddEntity(const std::string &entity)
{
    operation_.AddEntity(entity);
    return *this;
}

/**
 * @description: Removes the description of an entity from a Want
 * @param entity Indicates the entity description to remove.
 * @return void
 */
void Want::RemoveEntity(const std::string &entity)
{
    operation_.RemoveEntity(entity);
}

/**
 * @description: Checks whether a Want contains the given entity
 * @param entity Indicates the entity to check
 * @return Returns true if the given entity is contained; returns false otherwise
 */
bool Want::HasEntity(const std::string &entity) const
{
    return operation_.HasEntity(entity);
}

/**
 * @description: Obtains the number of entities in a Want
 * @return Returns the entity quantity
 */
int Want::CountEntities()
{
    return operation_.CountEntities();
}

/**
 * @description: Obtains the name of the bundle specified in a Want
 * @return Returns the bundle name specified in the Want
 */
std::string Want::GetBundle() const
{
    return operation_.GetBundleName();
}

/**
 * @description: Sets a bundle name in this Want.
 * If a bundle name is specified in a Want, the Want will match only
 * the abilities in the specified bundle. You cannot use this method and
 * setPicker(ohos.aafwk.content.Want) on the same Want.
 * @param bundleName Indicates the bundle name to set.
 * @return Returns a Want object containing the specified bundle name.
 */
Want &Want::SetBundle(const std::string &bundleName)
{
    operation_.SetBundleName(bundleName);
    return *this;
}

/**
 * @description: Obtains the description of the type in this Want
 * @return Returns the type description in this Want
 */
std::string Want::GetType() const
{
    auto value = parameters_.GetParam(MIME_TYPE);
    IString *ao = IString::Query(value);
    if (ao != nullptr) {
        return String::Unbox(ao);
    }
    return std::string();
}

/**
 * @description: Sets the description of a type in this Want
 * @param type Indicates the type description
 * @return Returns this Want object containing the type
 */
Want &Want::SetType(const std::string &type)
{
    sptr<IString> valueObj = String::Parse(type);
    parameters_.SetParam(MIME_TYPE, valueObj);
    return *this;
}

/**
 * @description: Formats a specified MIME type. This method uses
 * the formatMimeType(java.lang.String) method to format a MIME type
 * and then saves the formatted type to this Want object.
 * @param type Indicates the MIME type to format
 * @return Returns this Want object that contains the formatted type attribute
 */
Want &Want::FormatType(const std::string &type)
{
    std::string typetemp = FormatMimeType(type);
    SetType(typetemp);

    return *this;
}

/**
 * @description: Convert the scheme of URI to lower-case, and return the uri which be converted.
 * @param uri Indicates the URI to format.
 * @return Returns this URI Object.
 */
Uri Want::GetLowerCaseScheme(const Uri &uri)
{
    std::string strUri = const_cast<Uri &>(uri).ToString();
    std::string schemeStr = const_cast<Uri &>(uri).GetScheme();
    if (strUri.empty() || schemeStr.empty()) {
        return uri;
    }

    std::string lowSchemeStr = schemeStr;
    std::transform(lowSchemeStr.begin(), lowSchemeStr.end(), lowSchemeStr.begin(), [](unsigned char c) {
        return std::tolower(c);
    });

    if (schemeStr == lowSchemeStr) {
        return uri;
    }

    std::size_t pos = strUri.find_first_of(schemeStr, 0);
    if (pos != std::string::npos) {
        strUri.replace(pos, schemeStr.length(), lowSchemeStr);
    }

    return Uri(strUri);
}

/**
 * @description: Formats a specified URI and MIME type.
 * This method works in the same way as formatUri(ohos.utils.net.URI)
 * and formatType(java.lang.String).
 * @param uri Indicates the URI to format.
 * @param type Indicates the MIME type to format.
 * @return Returns this Want object that contains the formatted URI and type attributes.
 */
Want &Want::FormatUriAndType(const Uri &uri, const std::string &type)
{
    return SetUriAndType(GetLowerCaseScheme(uri), FormatMimeType(type));
}

/**
 * @description: This method formats data of a specified MIME type
 * by removing spaces from the data and converting the data into
 * lowercase letters. You can use this method to normalize
 * the external data used to create Want information.
 * @param type Indicates the MIME type to format
 * @return Returns this Want object that contains the formatted type attribute
 */
std::string Want::FormatMimeType(const std::string &mimeType)
{
    std::string strMimeType = mimeType;
    strMimeType.erase(std::remove(strMimeType.begin(), strMimeType.end(), ' '), strMimeType.end());

    std::transform(
        strMimeType.begin(), strMimeType.end(), strMimeType.begin(), [](unsigned char c) { return std::tolower(c); });

    std::size_t pos = 0;
    std::size_t begin = 0;
    pos = strMimeType.find_first_of(";", begin);
    if (pos != std::string::npos) {
        strMimeType = strMimeType.substr(begin, pos - begin);
    }

    return strMimeType;
}

/**
 * @description: Obtains the description of an action in a want.
 * @return Returns the action description in the want.
 */
std::string Want::GetAction() const
{
    return operation_.GetAction();
}

/**
 * @description: Sets the description of an action in a want.
 * @param action Indicates the action description.
 * @return Returns this want object containing the action.
 */
Want &Want::SetAction(const std::string &action)
{
    operation_.SetAction(action);
    return *this;
}

/**
 * @description: Obtains the description of the URI scheme in this want.
 * @return Returns the URI scheme description in this want.
 */
const std::string Want::GetScheme() const
{
    return operation_.GetUri().GetScheme();
}

/**
 * @description: Creates a want with its corresponding attributes specified for starting the main ability of an
 * application.
 * @param ElementName  Indicates the ElementName object defining the deviceId, bundleName,
 * and abilityName sub-attributes of the operation attribute in a want.
 * @return Returns the want object used to start the main ability of an application.
 */
Want *Want::MakeMainAbility(const OHOS::AppExecFwk::ElementName &elementName)
{
    Want *want = new (std::nothrow) Want();

    if (want != nullptr) {
        want->SetAction(ACTION_HOME);
        want->AddEntity(ENTITY_HOME);
        want->SetElement(elementName);
    } else {
        return nullptr;
    }

    return want;
}

/**
 * @description: Obtains the description of the WantParams object in a Want
 * @return Returns the WantParams description in the Want
 */
const WantParams &Want::GetParams() const
{
    return parameters_;
}

/**
 * @description: Sets a wantParams object in a want.
 * @param wantParams  Indicates the wantParams description.
 * @return Returns this want object containing the wantParams.
 */
Want &Want::SetParams(const WantParams &wantParams)
{
    parameters_ = wantParams;
    return *this;
}

/**
 * @description: Obtains a bool-type value matching the given key.
 * @param key   Indicates the key of WantParams.
 * @param defaultValue  Indicates the default bool-type value.
 * @return Returns the bool-type value of the parameter matching the given key;
 * returns the default value if the key does not exist.
 */
bool Want::GetBoolParam(const std::string &key, bool defaultValue) const
{
    auto value = parameters_.GetParam(key);
    IBoolean *bo = IBoolean::Query(value);
    if (bo != nullptr) {
        return Boolean::Unbox(bo);
    }
    return defaultValue;
}

/**
 * @description:Obtains a bool-type array matching the given key.
 * @param key   Indicates the key of WantParams.
 * @return Returns the bool-type array of the parameter matching the given key;
 * returns null if the key does not exist.
 */
std::vector<bool> Want::GetBoolArrayParam(const std::string &key) const
{
    std::vector<bool> array;
    auto value = parameters_.GetParam(key);
    IArray *ao = IArray::Query(value);
    if (ao != nullptr && Array::IsBooleanArray(ao)) {
        auto func = [&](IInterface *object) {
            if (object != nullptr) {
                IBoolean *value = IBoolean::Query(object);
                if (value != nullptr) {
                    array.push_back(Boolean::Unbox(value));
                }
            }
        };

        Array::ForEach(ao, func);
    }
    return array;
}

Want& Want::SetParam(const std::string& key, const sptr<IRemoteObject>& remoteObject)
{
    WantParams wp;
    wp.SetParam(AAFwk::TYPE_PROPERTY, AAFwk::String::Box(AAFwk::REMOTE_OBJECT));
    wp.SetParam(AAFwk::VALUE_PROPERTY, AAFwk::RemoteObjectWrap::Box(remoteObject));
    parameters_.SetParam(key, WantParamWrapper::Box(wp));
    return *this;
}

sptr<IRemoteObject> Want::GetRemoteObject(const std::string &key) const
{
    auto value = parameters_.GetParam(key);
    IWantParams* iwp = IWantParams::Query(value);
    if (iwp == nullptr) {
        ABILITYBASE_LOGI("%{public}s is invalid.", key.c_str());
        return nullptr;
    }
    auto wp = WantParamWrapper::Unbox(iwp);

    auto type = wp.GetParam(TYPE_PROPERTY);
    IString* iString = IString::Query(type);
    if (iString == nullptr) {
        ABILITYBASE_LOGI("it not contains TYPE_PROPERTY.");
        return nullptr;
    }
    if (REMOTE_OBJECT != String::Unbox(iString)) {
        ABILITYBASE_LOGE("invalid type.");
        return nullptr;
    }

    auto remoteObjVal = wp.GetParam(VALUE_PROPERTY);
    IRemoteObjectWrap* iRemoteObj = IRemoteObjectWrap::Query(remoteObjVal);
    if (iRemoteObj == nullptr) {
        ABILITYBASE_LOGE("it not contains VALUE_PROPERTY.");
        return nullptr;
    }
    return RemoteObjectWrap::UnBox(iRemoteObj);
}

/**
 * @description: Sets a parameter value of the boolean type.
 * @param key   Indicates the key matching the parameter.
 * @param value Indicates the boolean value of the parameter.
 * @return Returns this want object containing the parameter value.
 */
Want &Want::SetParam(const std::string &key, bool value)
{
    parameters_.SetParam(key, Boolean::Box(value));
    return *this;
}

/**
 * @description: Sets a parameter value of the boolean array type.
 * @param key   Indicates the key matching the parameter.
 * @param value Indicates the boolean array of the parameter.
 * @return Returns this want object containing the parameter value.
 */
Want &Want::SetParam(const std::string &key, const std::vector<bool> &value)
{
    std::size_t size = value.size();
    sptr<IArray> ao = new (std::nothrow) Array(size, g_IID_IBoolean);
    if (ao != nullptr) {
        for (std::size_t i = 0; i < size; i++) {
            ao->Set(i, Boolean::Box(value[i]));
        }
        parameters_.SetParam(key, ao);
    }

    return *this;
}

/**
 * @description: Obtains a byte-type value matching the given key.
 * @param key   Indicates the key of WantParams.
 * @param defaultValue  Indicates the default byte-type value.
 * @return Returns the byte-type value of the parameter matching the given key;
 * returns the default value if the key does not exist.
 */
byte Want::GetByteParam(const std::string &key, const byte defaultValue) const
{
    auto value = parameters_.GetParam(key);
    IByte *bo = IByte::Query(value);
    if (bo != nullptr) {
        return Byte::Unbox(bo);
    }
    return defaultValue;
}

/**
 * @description: Obtains a byte-type array matching the given key.
 * @param key   Indicates the key of WantParams.
 * @return Returns the byte-type array of the parameter matching the given key;
 * returns null if the key does not exist.
 */
std::vector<byte> Want::GetByteArrayParam(const std::string &key) const
{
    std::vector<byte> array;
    auto value = parameters_.GetParam(key);
    IArray *ao = IArray::Query(value);
    if (ao != nullptr && Array::IsByteArray(ao)) {
        auto func = [&](IInterface *object) {
            if (object != nullptr) {
                IByte *value = IByte::Query(object);
                if (value != nullptr) {
                    array.push_back(Byte::Unbox(value));
                }
            }
        };
        Array::ForEach(ao, func);
    }
    return array;
}

/**
 * @description: Sets a parameter value of the byte type.
 * @param key   Indicates the key matching the parameter.
 * @param value Indicates the byte-type value of the parameter.
 * @return Returns this Want object containing the parameter value.
 */
Want &Want::SetParam(const std::string &key, byte value)
{
    parameters_.SetParam(key, Byte::Box(value));
    return *this;
}

/**
 * @description: Sets a parameter value of the byte array type.
 * @param key   Indicates the key matching the parameter.
 * @param value Indicates the byte array of the parameter.
 * @return Returns this Want object containing the parameter value.
 */
Want &Want::SetParam(const std::string &key, const std::vector<byte> &value)
{
    std::size_t size = value.size();
    sptr<IArray> ao = new (std::nothrow) Array(size, g_IID_IByte);
    if (ao == nullptr) {
        return *this;
    }
    for (std::size_t i = 0; i < size; i++) {
        ao->Set(i, Byte::Box(value[i]));
    }
    parameters_.SetParam(key, ao);
    return *this;
}

/**
 * @description: Obtains a char value matching the given key.
 * @param key   Indicates the key of wnatParams.
 * @param value Indicates the default char value.
 * @return Returns the char value of the parameter matching the given key;
 * returns the default value if the key does not exist.
 */
zchar Want::GetCharParam(const std::string &key, zchar defaultValue) const
{
    auto value = parameters_.GetParam(key);
    IChar *ao = IChar::Query(value);
    if (ao != nullptr) {
        return Char::Unbox(ao);
    }
    return defaultValue;
}

/**
 * @description: Obtains a char array matching the given key.
 * @param key   Indicates the key of wantParams.
 * @return Returns the char array of the parameter matching the given key;
 * returns null if the key does not exist.
 */
std::vector<zchar> Want::GetCharArrayParam(const std::string &key) const
{
    std::vector<zchar> array;
    auto value = parameters_.GetParam(key);
    IArray *ao = IArray::Query(value);
    if (ao != nullptr && Array::IsCharArray(ao)) {
        auto func = [&](IInterface *object) {
            if (object != nullptr) {
                IChar *value = IChar::Query(object);
                if (value != nullptr) {
                    array.push_back(Char::Unbox(value));
                }
            }
        };
        Array::ForEach(ao, func);
    }
    return array;
}

/**
 * @description: Sets a parameter value of the char type.
 * @param key   Indicates the key of wantParams.
 * @param value Indicates the char value of the parameter.
 * @return Returns this want object containing the parameter value.
 */
Want &Want::SetParam(const std::string &key, zchar value)
{
    parameters_.SetParam(key, Char::Box(value));
    return *this;
}

/**
 * @description: Sets a parameter value of the char array type.
 * @param key   Indicates the key of wantParams.
 * @param value Indicates the char array of the parameter.
 * @return Returns this want object containing the parameter value.
 */
Want &Want::SetParam(const std::string &key, const std::vector<zchar> &value)
{
    std::size_t size = value.size();
    sptr<IArray> ao = new (std::nothrow) Array(size, g_IID_IChar);
    if (ao == nullptr) {
        return *this;
    }
    for (std::size_t i = 0; i < size; i++) {
        ao->Set(i, Char::Box(value[i]));
    }
    parameters_.SetParam(key, ao);
    return *this;
}

/**
 * @description: Obtains an int value matching the given key.
 * @param key   Indicates the key of wantParams.
 * @param value Indicates the default int value.
 * @return Returns the int value of the parameter matching the given key;
 * returns the default value if the key does not exist.
 */
int Want::GetIntParam(const std::string &key, const int defaultValue) const
{
    auto value = parameters_.GetParam(key);
    IInteger *ao = IInteger::Query(value);
    if (ao != nullptr) {
        return Integer::Unbox(ao);
    }
    return defaultValue;
}

/**
 * @description: Obtains an int array matching the given key.
 * @param key   Indicates the key of wantParams.
 * @return Returns the int array of the parameter matching the given key;
 * returns null if the key does not exist.
 */
std::vector<int> Want::GetIntArrayParam(const std::string &key) const
{
    std::vector<int> array;
    auto value = parameters_.GetParam(key);
    IArray *ao = IArray::Query(value);
    if (ao != nullptr && Array::IsIntegerArray(ao)) {
        auto func = [&](IInterface *object) {
            if (object != nullptr) {
                IInteger *value = IInteger::Query(object);
                if (value != nullptr) {
                    array.push_back(Integer::Unbox(value));
                }
            }
        };
        Array::ForEach(ao, func);
    }
    return array;
}

/**
 * @description: Sets a parameter value of the int type.
 * @param key   Indicates the key matching the parameter.
 * @param value Indicates the int value of the parameter.
 * @return Returns this Want object containing the parameter value.
 */
Want &Want::SetParam(const std::string &key, int value)
{
    parameters_.SetParam(key, Integer::Box(value));
    return *this;
}

/**
 * @description: Sets a parameter value of the int array type.
 * @param key   Indicates the key matching the parameter.
 * @param value Indicates the int array of the parameter.
 * @return Returns this Want object containing the parameter value.
 */
Want &Want::SetParam(const std::string &key, const std::vector<int> &value)
{
    std::size_t size = value.size();
    sptr<IArray> ao = new (std::nothrow) Array(size, g_IID_IInteger);
    if (ao == nullptr) {
        return *this;
    }
    for (std::size_t i = 0; i < size; i++) {
        ao->Set(i, Integer::Box(value[i]));
    }
    parameters_.SetParam(key, ao);
    return *this;
}

/**
 * @description: Obtains a double value matching the given key.
 * @param key   Indicates the key of wantParams.
 * @param defaultValue  Indicates the default double value.
 * @return Returns the double value of the parameter matching the given key;
 * returns the default value if the key does not exist.
 */
double Want::GetDoubleParam(const std::string &key, double defaultValue) const
{
    auto value = parameters_.GetParam(key);
    IDouble *ao = IDouble::Query(value);
    if (ao != nullptr) {
        return Double::Unbox(ao);
    }
    return defaultValue;
}

/**
 * @description: Obtains a double array matching the given key.
 * @param key   Indicates the key of WantParams.
 * @return Returns the double array of the parameter matching the given key;
 * returns null if the key does not exist.
 */
std::vector<double> Want::GetDoubleArrayParam(const std::string &key) const
{
    std::vector<double> array;
    auto value = parameters_.GetParam(key);
    IArray *ao = IArray::Query(value);
    if (ao != nullptr && Array::IsDoubleArray(ao)) {
        auto func = [&](IInterface *object) {
            if (object != nullptr) {
                IDouble *value = IDouble::Query(object);
                if (value != nullptr) {
                    array.push_back(Double::Unbox(value));
                }
            }
        };
        Array::ForEach(ao, func);
    }
    return array;
}

/**
 * @description: Sets a parameter value of the double type.
 * @param key   Indicates the key matching the parameter.
 * @param value Indicates the int value of the parameter.
 * @return Returns this Want object containing the parameter value.
 */
Want &Want::SetParam(const std::string &key, double value)
{
    parameters_.SetParam(key, Double::Box(value));
    return *this;
}

/**
 * @description: Sets a parameter value of the double array type.
 * @param key   Indicates the key matching the parameter.
 * @param value Indicates the double array of the parameter.
 * @return Returns this want object containing the parameter value.
 */
Want &Want::SetParam(const std::string &key, const std::vector<double> &value)
{
    std::size_t size = value.size();
    sptr<IArray> ao = new (std::nothrow) Array(size, g_IID_IDouble);
    if (ao == nullptr) {
        return *this;
    }
    for (std::size_t i = 0; i < size; i++) {
        ao->Set(i, Double::Box(value[i]));
    }
    parameters_.SetParam(key, ao);
    return *this;
}

/**
 * @description: Obtains a float value matching the given key.
 * @param key   Indicates the key of wnatParams.
 * @param value Indicates the default float value.
 * @return Returns the float value of the parameter matching the given key;
 * returns the default value if the key does not exist.
 */
float Want::GetFloatParam(const std::string &key, float defaultValue) const
{
    auto value = parameters_.GetParam(key);
    IFloat *ao = IFloat::Query(value);
    if (ao != nullptr) {
        return Float::Unbox(ao);
    }
    return defaultValue;
}

/**
 * @description: Obtains a float array matching the given key.
 * @param key Indicates the key of WantParams.
 * @return Obtains a float array matching the given key.
 */
std::vector<float> Want::GetFloatArrayParam(const std::string &key) const
{
    std::vector<float> array;
    auto value = parameters_.GetParam(key);
    IArray *ao = IArray::Query(value);
    if (ao != nullptr && Array::IsFloatArray(ao)) {
        auto func = [&](IInterface *object) {
            if (object != nullptr) {
                IFloat *value = IFloat::Query(object);
                if (value != nullptr) {
                    array.push_back(Float::Unbox(value));
                }
            }
        };
        Array::ForEach(ao, func);
    }
    return array;
}

/**
 * @description: Sets a parameter value of the float type.
 * @param key Indicates the key matching the parameter.
 * @param value Indicates the byte-type value of the parameter.
 * @return Returns this Want object containing the parameter value.
 */
Want &Want::SetParam(const std::string &key, float value)
{
    parameters_.SetParam(key, Float::Box(value));
    return *this;
}

/**
 * @description: Sets a parameter value of the float array type.
 * @param key Indicates the key matching the parameter.
 * @param value Indicates the byte-type value of the parameter.
 * @return Returns this Want object containing the parameter value.
 */
Want &Want::SetParam(const std::string &key, const std::vector<float> &value)
{
    std::size_t size = value.size();
    sptr<IArray> ao = new (std::nothrow) Array(size, g_IID_IFloat);
    if (ao == nullptr) {
        return *this;
    }

    for (std::size_t i = 0; i < size; i++) {
        ao->Set(i, Float::Box(value[i]));
    }
    parameters_.SetParam(key, ao);
    return *this;
}

/**
 * @description: Obtains a long value matching the given key.
 * @param key Indicates the key of wantParams.
 * @param value Indicates the default long value.
 * @return Returns the long value of the parameter matching the given key;
 * returns the default value if the key does not exist.
 */
long Want::GetLongParam(const std::string &key, long defaultValue) const
{
    auto value = parameters_.GetParam(key);
    if (ILong::Query(value) != nullptr) {
        return Long::Unbox(ILong::Query(value));
    } else if (IString::Query(value) != nullptr) {
        // Marshalling
        std::string str = String::Unbox(IString::Query(value));
        if (std::regex_match(str, NUMBER_REGEX)) {
            return std::atoll(str.c_str());
        }
    }

    return defaultValue;
}
void ArrayAddData(IInterface *object, std::vector<long> &array)
{
    if (object == nullptr) {
        return;
    }

    IString *o = IString::Query(object);
    if (o != nullptr) {
        std::string str = String::Unbox(o);
        if (std::regex_match(str, NUMBER_REGEX)) {
            array.push_back(std::atoll(str.c_str()));
        }
    }
}
/**
 * @description: Obtains a long array matching the given key.
 * @param key Indicates the key of wantParams.
 * @return Returns the long array of the parameter matching the given key;
 * returns null if the key does not exist.
 */
std::vector<long> Want::GetLongArrayParam(const std::string &key) const
{
    std::vector<long> array;
    auto value = parameters_.GetParam(key);
    IArray *ao = IArray::Query(value);
    if (ao != nullptr && Array::IsLongArray(ao)) {
        auto func = [&](IInterface *object) {
            if (object != nullptr) {
                ILong *value = ILong::Query(object);
                if (value != nullptr) {
                    array.push_back(Long::Unbox(value));
                }
            }
        };
        Array::ForEach(ao, func);
    } else if (ao != nullptr && Array::IsStringArray(ao)) {
        // Marshalling
        auto func = [&](IInterface *object) { ArrayAddData(object, array); };
        Array::ForEach(ao, func);
    }

    return array;
}

/**
 * @description: Sets a parameter value of the long type.
 * @param key Indicates the key matching the parameter.
 * @param value Indicates the byte-type value of the parameter.
 * @return Returns this Want object containing the parameter value.
 */
Want &Want::SetParam(const std::string &key, long value)
{
    parameters_.SetParam(key, Long::Box(value));
    return *this;
}

/**
 * @description: Sets a parameter value of the long array type.
 * @param key Indicates the key matching the parameter.
 * @param value Indicates the byte-type value of the parameter.
 * @return Returns this Want object containing the parameter value.
 */
Want &Want::SetParam(const std::string &key, const std::vector<long> &value)
{
    std::size_t size = value.size();
    sptr<IArray> ao = new (std::nothrow) Array(size, g_IID_ILong);
    if (ao == nullptr) {
        return *this;
    }
    for (std::size_t i = 0; i < size; i++) {
        ao->Set(i, Long::Box(value[i]));
    }
    parameters_.SetParam(key, ao);
    return *this;
}

Want &Want::SetParam(const std::string &key, long long value)
{
    parameters_.SetParam(key, Long::Box(value));
    return *this;
}

/**
 * @description: a short value matching the given key.
 * @param key Indicates the key of wantParams.
 * @param defaultValue Indicates the default short value.
 * @return Returns the short value of the parameter matching the given key;
 * returns the default value if the key does not exist.
 */
short Want::GetShortParam(const std::string &key, short defaultValue) const
{
    auto value = parameters_.GetParam(key);
    IShort *ao = IShort::Query(value);
    if (ao != nullptr) {
        return Short::Unbox(ao);
    }
    return defaultValue;
}

/**
 * @description: Obtains a short array matching the given key.
 * @param key Indicates the key of wantParams.
 * @return Returns the short array of the parameter matching the given key;
 * returns null if the key does not exist.
 */
std::vector<short> Want::GetShortArrayParam(const std::string &key) const
{
    std::vector<short> array;
    auto value = parameters_.GetParam(key);
    IArray *ao = IArray::Query(value);
    if (ao != nullptr && Array::IsShortArray(ao)) {
        auto func = [&](IInterface *object) {
            if (object != nullptr) {
                IShort *value = IShort::Query(object);
                if (value != nullptr) {
                    array.push_back(Short::Unbox(value));
                }
            }
        };
        Array::ForEach(ao, func);
    }
    return array;
}

/**
 * @description: Sets a parameter value of the short type.
 * @param key Indicates the key matching the parameter.
 * @param value Indicates the byte-type value of the parameter.
 * @return Returns this Want object containing the parameter value.
 */
Want &Want::SetParam(const std::string &key, short value)
{
    parameters_.SetParam(key, Short::Box(value));
    return *this;
}

/**
 * @description: Sets a parameter value of the short array type.
 * @param key Indicates the key matching the parameter.
 * @param value Indicates the byte-type value of the parameter.
 * @return Returns this Want object containing the parameter value.
 */
Want &Want::SetParam(const std::string &key, const std::vector<short> &value)
{
    std::size_t size = value.size();
    sptr<IArray> ao = new (std::nothrow) Array(size, g_IID_IShort);
    if (ao == nullptr) {
        return *this;
    }
    for (std::size_t i = 0; i < size; i++) {
        ao->Set(i, Short::Box(value[i]));
    }
    parameters_.SetParam(key, ao);
    return *this;
}

/**
 * @description: Obtains a string value matching the given key.
 * @param key Indicates the key of wantParams.
 * @return Returns the string value of the parameter matching the given key;
 * returns null if the key does not exist.
 */
std::string Want::GetStringParam(const std::string &key) const
{
    auto value = parameters_.GetParam(key);
    IString *ao = IString::Query(value);
    if (ao != nullptr) {
        return String::Unbox(ao);
    }
    return std::string();
}

/**
 * @description: Obtains a string array matching the given key.
 * @param key Indicates the key of wantParams.
 * @return Returns the string array of the parameter matching the given key;
 * returns null if the key does not exist.
 */
std::vector<std::string> Want::GetStringArrayParam(const std::string &key) const
{
    std::vector<std::string> array;
    auto value = parameters_.GetParam(key);
    IArray *ao = IArray::Query(value);
    if (ao != nullptr && Array::IsStringArray(ao)) {
        auto func = [&](IInterface *object) {
            if (object != nullptr) {
                IString *value = IString::Query(object);
                if (value != nullptr) {
                    array.push_back(String::Unbox(value));
                }
            }
        };
        Array::ForEach(ao, func);
    }
    return array;
}

/**
 * @description: Sets a parameter value of the string type.
 * @param key Indicates the key matching the parameter.
 * @param value Indicates the byte-type value of the parameter.
 * @return Returns this Want object containing the parameter value.
 */
Want &Want::SetParam(const std::string &key, const std::string &value)
{
    parameters_.SetParam(key, String::Box(value));
    return *this;
}

/**
 * @description: Sets a parameter value of the string array type.
 * @param key Indicates the key matching the parameter.
 * @param value Indicates the byte-type value of the parameter.
 * @return Returns this Want object containing the parameter value.
 */
Want &Want::SetParam(const std::string &key, const std::vector<std::string> &value)
{
    std::size_t size = value.size();
    sptr<IArray> ao = new (std::nothrow) Array(size, g_IID_IString);
    if (ao == nullptr) {
        return *this;
    }
    for (std::size_t i = 0; i < size; i++) {
        ao->Set(i, String::Box(value[i]));
    }
    parameters_.SetParam(key, ao);
    return *this;
}

/**
 * @description: Gets the description of an operation in a Want.
 * @return Returns the operation included in this Want.
 */
Operation Want::GetOperation() const
{
    return operation_;
}

/**
 * @description: Sets the description of an operation in a Want.
 * @param operation Indicates the operation description.
 */
void Want::SetOperation(const OHOS::AAFwk::Operation &operation)
{
    operation_ = operation;
}

/**
 * @description: Sets the description of an operation in a Want.
 * @param want Indicates the Want object to compare.
 * @return Returns true if the operation components of the two objects are equal; returns false otherwise.
 */
bool Want::OperationEquals(const Want &want)
{
    return (operation_ == want.operation_);
}

/**
 * @description: Creates a Want object that contains only the operation component of this Want.
 * @return Returns the created Want object.
 */
Want *Want::CloneOperation()
{
    Want *want = new (std::nothrow) Want();
    if (want == nullptr) {
        return nullptr;
    }
    want->SetOperation(operation_);
    return want;
}

/**
 * @description: Creates a Want instance by using a given Uniform Resource Identifier (URI).
 * This method parses the input URI and saves it in a Want object.
 * @param uri Indicates the URI to parse.
 * @return Returns a Want object containing the URI.
 */
Want *Want::ParseUri(const std::string &uri)
{
    if (!CheckUri(uri)) {
        return nullptr;
    }

    bool ret = true;
    std::string content;
    std::size_t pos;
    std::size_t begin = WANT_HEADER.length();
    ElementName element;
    Want *want = new (std::nothrow) Want();
    if (want == nullptr) {
        return nullptr;
    }
    bool inPicker = false;
    pos = uri.find_first_of(";", begin);
    while (pos != std::string::npos) {
        content = uri.substr(begin, pos - begin);
        if (content.compare("PICK") == 0) {
            inPicker = true;
            begin = pos + 1;
            pos = uri.find(";", begin);
            break;
        }
        ret = ParseUriInternal(content, element, *want);
        if (!ret) {
            break;
        }
        begin = pos + 1;
        pos = uri.find(";", begin);
    }
    if (inPicker) {
        sptr<Want> pickerWant = new (std::nothrow) Want();
        ElementName pickerElement;
        while (pos != std::string::npos) {
            content = uri.substr(begin, pos - begin);
            ret = ParseUriInternal(content, pickerElement, *pickerWant);
            if (!ret) {
                break;
            }
            begin = pos + 1;
            pos = uri.find(";", begin);
        }
        pickerWant->SetElement(pickerElement);
        if (want->GetBundle().empty()) {
        }
    }
    if (ret) {
        want->SetElement(element);
    } else {
        delete want;
        want = nullptr;
    }
    return want;
}

/**
 * @description: Creates a Want instance by using a given Uniform Resource Identifier (URI).
 * This method parses the input URI and saves it in a Want object.
 * @param uri Indicates the URI to parse.
 * @return Returns a Want object containing the URI.
 */
Want *Want::WantParseUri(const char *uri)
{
    if (uri == nullptr) {
        return nullptr;
    }
    std::string strUri(uri);

    return ParseUri(strUri);
}

/**
 * @description: Obtains the string representation of the URI in this Want.
 * @return Returns the string of the URI.
 */
std::string Want::GetUriString() const
{
    return operation_.GetUri().ToString();
}

/**
 * @description: Obtains the description of a URI in a Want.
 * @return Returns the URI description in the Want.
 */
Uri Want::GetUri() const
{
    return operation_.GetUri();
}

/**
 * @description: Sets the description of a URI in a Want.
 * @param uri Indicates the string of URI description.
 * @return Returns this Want object containing the URI.
 */
Want &Want::SetUri(const std::string &uri)
{
    operation_.SetUri(Uri(uri));
    return *this;
}

/**
 * @description: Sets the description of a URI in a Want.
 * @param uri Indicates the URI description.
 * @return Returns this Want object containing the URI.
 */
Want &Want::SetUri(const Uri &uri)
{
    operation_.SetUri(uri);
    return *this;
}

/**
 * @description: Sets the description of a URI and a type in this Want.
 * @param uri Indicates the URI description.
 * @param type Indicates the type description.
 * @return Returns this Want object containing the URI and the type.
 */
Want &Want::SetUriAndType(const Uri &uri, const std::string &type)
{
    operation_.SetUri(uri);
    return SetType(type);
}

/**
 * @description: Converts a Want into a URI string containing a representation of it.
 * @param want Indicates the want description.--Want.
 * @return  Returns an encoding URI string describing the Want object.
 */
std::string Want::WantToUri(Want &want)
{
    return want.ToUri();
}

/**
 * @description: Converts parameter information in a Want into a URI string.
 * @return Returns the URI string.
 */
std::string Want::ToUri() const
{
    std::string uriString = WANT_HEADER;
    ToUriStringInner(uriString);

    uriString += "end";

    return uriString;
}
void Want::ToUriStringInner(std::string &uriString) const
{
    if (operation_.GetAction().length() > 0) {
        uriString += "action=" + Encode(operation_.GetAction()) + ";";
    }
    if (GetUriString().length() > 0) {
        uriString += "uri=" + Encode(GetUriString()) + ";";
    }
    for (auto entity : operation_.GetEntities()) {
        if (entity.length() > 0) {
            uriString += "entity=" + Encode(entity) + ";";
        }
    }
    if (operation_.GetDeviceId().length() > 0) {
        uriString += "device=" + Encode(operation_.GetDeviceId()) + ";";
    }
    if (operation_.GetBundleName().length() > 0) {
        uriString += "bundle=" + Encode(operation_.GetBundleName()) + ";";
    }
    if (operation_.GetAbilityName().length() > 0) {
        uriString += "ability=" + Encode(operation_.GetAbilityName()) + ";";
    }
    if (operation_.GetFlags() != 0) {
        uriString += "flag=";
        char buf[HEX_STRING_BUF_LEN] {0};
        int len = snprintf_s(buf, HEX_STRING_BUF_LEN, HEX_STRING_BUF_LEN - 1, "0x%08x", operation_.GetFlags());
        if (len == HEX_STRING_LEN) {
            std::string flag = buf;
            uriString += Encode(flag);
            uriString += ";";
        }
    }
    if (!operation_.GetBundleName().empty()) {
        uriString.append("package=");
        uriString.append(Encode(operation_.GetBundleName()));
        uriString.append(";");
    }

    UriStringAppendParam(uriString);
}
/**
 * @description: Formats a specified URI.
 * This method uses the Uri.getLowerCaseScheme() method to format a URI and then saves
 * the formatted URI to this Want object.
 * @param uri Indicates the URI to format.
 * @return Returns this Want object that contains the formatted uri attribute.
 */
Want &Want::FormatUri(const std::string &uri)
{
    return FormatUri(Uri(uri));
}

/**
 * @description: Formats a specified URI.
 * This method uses the GetLowerCaseScheme() method to format a URI and then saves
 * the formatted URI to this Want object.
 * @param uri Indicates the URI to format.
 * @return Returns this Want object that contains the formatted uri attribute.
 */
Want &Want::FormatUri(const Uri &uri)
{
    operation_.SetUri(GetLowerCaseScheme(uri));
    return *this;
}

/**
 * @description: Checks whether a Want contains the parameter matching a given key.
 * @param key Indicates the key.
 * @return Returns true if the Want contains the parameter; returns false otherwise.
 */
bool Want::HasParameter(const std::string &key) const
{
    return parameters_.HasParam(key);
}

/**
 * @description: Replaces parameters in this Want object with those in the given WantParams object.
 * @param wantParams Indicates the WantParams object containing the new parameters.
 * @return Returns this Want object containing the new parameters.
 */
Want *Want::ReplaceParams(WantParams &wantParams)
{
    parameters_ = wantParams;
    return this;
}

/**
 * @description: Replaces parameters in this Want object with those in the given Want object.
 * @param want Indicates the Want object containing the new parameters.
 * @return Returns this Want object containing the new parameters.
 */
Want *Want::ReplaceParams(Want &want)
{
    parameters_ = want.parameters_;
    return this;
}

/**
 * @description: Removes the parameter matching the given key.
 * @param key Indicates the key matching the parameter to be removed.
 */
void Want::RemoveParam(const std::string &key)
{
    parameters_.Remove(key);
}

/**
 * @description: clear the specific want object.
 * @param want Indicates the want to clear
 */
void Want::ClearWant(Want *want)
{
    want->SetType("");
    want->SetAction("");
    want->SetFlags(0);
    OHOS::AppExecFwk::ElementName elementName;
    want->SetElement(elementName);
    OHOS::AAFwk::Operation operation;
    want->SetOperation(operation);
    WantParams parameters;
    want->SetParams(parameters);
}

/**
 * @description: Marshals a Want into a Parcel.
 * Fields in the Want are marshalled separately. If any field fails to be marshalled, false is returned.
 * @param parcel Indicates the Parcel object for marshalling.
 * @return Returns true if the marshalling is successful; returns false otherwise.
 */
bool Want::Marshalling(Parcel &parcel) const
{
    // write action
    if (!parcel.WriteString16(Str8ToStr16(GetAction()))) {
        return false;
    }

    // write uri
    if (!WriteUri(parcel)) {
        return false;
    }

    // write entities
    if (!WriteEntities(parcel)) {
        return false;
    }

    // write flags
    if (!parcel.WriteUint32(GetFlags())) {
        return false;
    }

    // write element
    if (!WriteElement(parcel)) {
        return false;
    }

    // write parameters
    if (!WriteParameters(parcel)) {
        return false;
    }

    // write package
    if (!parcel.WriteString16(Str8ToStr16(GetBundle()))) {
        return false;
    }

    return true;
}

/**
 * @description: Unmarshals a Want from a Parcel.
 * Fields in the Want are unmarshalled separately. If any field fails to be unmarshalled, false is returned.
 * @param parcel Indicates the Parcel object for unmarshalling.
 * @return Returns true if the unmarshalling is successful; returns false otherwise.
 */
Want *Want::Unmarshalling(Parcel &parcel)
{
    Want *want = new (std::nothrow) Want();
    if (want != nullptr && !want->ReadFromParcel(parcel)) {
        delete want;
        want = nullptr;
    }
    return want;
}

bool Want::ReadFromParcel(Parcel &parcel)
{
    // read action
    operation_.SetAction(Str16ToStr8(parcel.ReadString16()));

    // read uri
    if (!ReadUri(parcel)) {
        return false;
    }

    // read entities
    if (!ReadEntities(parcel)) {
        return false;
    }

    // read flags
    unsigned int flags;
    if (!parcel.ReadUint32(flags)) {
        return false;
    }
    operation_.SetFlags(flags);

    // read element
    if (!ReadElement(parcel)) {
        return false;
    }

    // read parameters
    if (!ReadParameters(parcel)) {
        return false;
    }

    // read package
    operation_.SetBundleName(Str16ToStr8(parcel.ReadString16()));

    return true;
}

bool Want::ParseUriInternal(const std::string &content, ElementName &element, Want &want)
{
    static constexpr int TYPE_TAG_SIZE = 2;

    std::string prop;
    std::string value;

    if (content.empty() || content[0] == '=') {
        return true;
    }

    if (!ParseContent(content, prop, value)) {
        return false;
    }

    if (value.empty()) {
        return true;
    }

    if (prop == "action") {
        want.SetAction(value);
    } else if (prop == "entity") {
        want.AddEntity(value);
    } else if (prop == "flag") {
        if (!ParseFlag(value, want)) {
            return false;
        }
    } else if (prop == "device") {
        element.SetDeviceID(value);
    } else if (prop == "bundle") {
        element.SetBundleName(value);
    } else if (prop == "ability") {
        element.SetAbilityName(value);
    } else if (prop == "package") {
        want.SetBundle(Decode(value));
    } else if (prop.length() > TYPE_TAG_SIZE) {
        std::string key = prop.substr(TYPE_TAG_SIZE);
        if (!Want::CheckAndSetParameters(want, key, prop, value)) {
            return false;
        }
        std::string moduleName = want.GetStringParam(PARAM_MODULE_NAME);
        want.SetModuleName(moduleName);
        element.SetModuleName(moduleName);
    }

    return true;
}

bool Want::ParseContent(const std::string &content, std::string &prop, std::string &value)
{
    std::size_t pos = content.find("=");
    if (pos != std::string::npos) {
        std::string subString = content.substr(0, pos);
        prop = Decode(subString);
        subString = content.substr(pos + 1, content.length() - pos - 1);
        value = Decode(subString);
        return true;
    }
    return false;
}

bool Want::ParseFlag(const std::string &content, Want &want)
{
    std::string contentLower = LowerStr(content);
    std::string prefix = "0x";
    if (!contentLower.empty()) {
        if (contentLower.find(prefix) != 0) {
            return false;
        }

        for (std::size_t i = prefix.length(); i < contentLower.length(); i++) {
            if (!isxdigit(contentLower[i])) {
                return false;
            }
        }
        int base = 16;  // hex string
        unsigned int flag = std::stoul(contentLower, nullptr, base);
        want.SetFlags(flag);
    }
    return true;
}

std::string Want::Decode(const std::string &str)
{
    std::string decode;

    for (std::size_t i = 0; i < str.length();) {
        if (str[i] != '\\') {
            decode += str[i];
            i++;
            continue;
        }
        if (++i >= str.length()) {
            decode += "\\";
            break;
        }
        if (str[i] == '\\') {
            decode += "\\";
            i++;
        } else if (str[i] == '0') {
            if (str.compare(i, OCT_EQUALSTO.length(), OCT_EQUALSTO) == 0) {
                decode += "=";
                i += OCT_EQUALSTO.length();
            } else if (str.compare(i, OCT_SEMICOLON.length(), OCT_SEMICOLON) == 0) {
                decode += ";";
                i += OCT_SEMICOLON.length();
            } else {
                decode += "\\" + str.substr(i, 1);
                i++;
            }
        } else {
            decode += "\\" + str.substr(i, 1);
            i++;
        }
    }

    return decode;
}

std::string Want::Encode(const std::string &str)
{
    std::string encode;

    for (std::size_t i = 0; i < str.length(); i++) {
        if (str[i] == '\\') {
            encode += "\\\\";
        } else if (str[i] == '=') {
            encode += "\\" + OCT_EQUALSTO;
        } else if (str[i] == ';') {
            encode += "\\" + OCT_SEMICOLON;
        } else {
            encode += str[i];
        }
    }

    return encode;
}

bool Want::CheckAndSetParameters(Want &want, const std::string &key, std::string &prop, const std::string &value)
{
    sptr<IInterface> valueObj;
    if (prop[0] == String::SIGNATURE && prop[1] == '.') {
        valueObj = String::Parse(value);
    } else if (prop[0] == Boolean::SIGNATURE && prop[1] == '.') {
        valueObj = Boolean::Parse(value);
    } else if (prop[0] == Char::SIGNATURE && prop[1] == '.') {
        valueObj = Char::Parse(value);
    } else if (prop[0] == Byte::SIGNATURE && prop[1] == '.') {
        valueObj = Byte::Parse(value);
    } else if (prop[0] == Short::SIGNATURE && prop[1] == '.') {
        valueObj = Short::Parse(value);
    } else if (prop[0] == Integer::SIGNATURE && prop[1] == '.') {
        valueObj = Integer::Parse(value);
    } else if (prop[0] == Long::SIGNATURE && prop[1] == '.') {
        valueObj = Long::Parse(value);
    } else if (prop[0] == Float::SIGNATURE && prop[1] == '.') {
        valueObj = Float::Parse(value);
    } else if (prop[0] == Double::SIGNATURE && prop[1] == '.') {
        valueObj = Double::Parse(value);
    } else if (prop[0] == Array::SIGNATURE && prop[1] == '.') {
        valueObj = Array::Parse(value);
    } else {
        return true;
    }

    if (valueObj == nullptr) {
        return false;
    }
    want.parameters_.SetParam(key, valueObj);
    return true;
}

void Want::DumpInfo(int level) const
{
    operation_.DumpInfo(level);
    parameters_.DumpInfo(level);
}

nlohmann::json Want::ToJson() const
{
    WantParamWrapper wrapper(parameters_);
    std::string parametersString = wrapper.ToString();

    nlohmann::json entitiesJson;
    std::vector<std::string> entities = GetEntities();
    for (auto entity : entities) {
        entitiesJson.emplace_back(entity);
    }

    nlohmann::json wantJson = nlohmann::json {
        {"deviceId", operation_.GetDeviceId()},
        {"bundleName", operation_.GetBundleName()},
        {"abilityName", operation_.GetAbilityName()},
        {"uri", GetUriString()},
        {"type", GetType()},
        {"flags", GetFlags()},
        {"action", GetAction()},
        {"parameters", parametersString},
        {"entities", entitiesJson},
    };

    return wantJson;
}

bool Want::ReadFromJson(nlohmann::json &wantJson)
{
    const auto &jsonObjectEnd = wantJson.end();
    if ((wantJson.find("deviceId") == jsonObjectEnd)
        || (wantJson.find("bundleName") == jsonObjectEnd)
        || (wantJson.find("abilityName") == jsonObjectEnd)
        || (wantJson.find("uri") == jsonObjectEnd)
        || (wantJson.find("type") == jsonObjectEnd)
        || (wantJson.find("flags") == jsonObjectEnd)
        || (wantJson.find("action") == jsonObjectEnd)
        || (wantJson.find("parameters") == jsonObjectEnd)
        || (wantJson.find("entities") == jsonObjectEnd)) {
        ABILITYBASE_LOGE("Incomplete wantJson");
        return false;
    }

    std::string deviceId = wantJson.at("deviceId").get<std::string>();
    std::string bundleName = wantJson.at("bundleName").get<std::string>();
    std::string abilityName = wantJson.at("abilityName").get<std::string>();
    SetElementName(deviceId, bundleName, abilityName);

    std::string uri = wantJson.at("uri").get<std::string>();
    SetUri(uri);

    std::string type = wantJson.at("type").get<std::string>();
    SetType(type);

    unsigned int flags = wantJson.at("flags").get<unsigned int>();
    SetFlags(flags);

    std::string action = wantJson.at("action").get<std::string>();
    SetAction(action);

    std::string parametersString = wantJson.at("parameters").get<std::string>();
    WantParams parameters = WantParamWrapper::ParseWantParams(parametersString);
    SetParams(parameters);
    std::string moduleName = GetStringParam(PARAM_MODULE_NAME);
    SetModuleName(moduleName);

    if (wantJson.at("entities").is_null()) {
        ABILITYBASE_LOGI("entities is null");
    } else {
        std::vector<std::string> entities;
        wantJson.at("entities").get_to<std::vector<std::string>>(entities);
        for (size_t i = 0; i < entities.size(); i++) {
            AddEntity(entities[i]);
        }
    }
    return true;
}

std::string Want::ToString() const
{
    return ToJson().dump();
}

Want *Want::FromString(std::string &string)
{
    if (string.empty()) {
        ABILITYBASE_LOGE("Invalid string.");
        return nullptr;
    }

    nlohmann::json wantJson = nlohmann::json::parse(string);

    Want *want = new (std::nothrow) Want();
    if (want != nullptr && !want->ReadFromJson(wantJson)) {
        delete want;
        want = nullptr;
    }
    return want;
}

/**
 * @description: Sets a device id in a Want.
 * @param deviceId Indicates the device id to set.
 * @return Returns this Want object containing the flag.
 */
Want &Want::SetDeviceId(const std::string &deviceId)
{
    operation_.SetDeviceId(deviceId);
    return *this;
}

std::string Want::GetDeviceId() const
{
    return operation_.GetDeviceId();
}

Want& Want::SetModuleName(const std::string &moduleName)
{
    operation_.SetModuleName(moduleName);
    SetParam(PARAM_MODULE_NAME, moduleName);
    return *this;
}

std::string Want::GetModuleName() const
{
    return operation_.GetModuleName();
}

bool Want::CheckUri(const std::string &uri)
{
    if (uri.length() <= 0) {
        return false;
    }

    if (uri.find(WANT_HEADER) != 0) {
        return false;
    }
    if (uri.rfind(WANT_END) != (uri.length() - WANT_END.length())) {
        return false;
    }

    return true;
}

void Want::UriStringAppendParam(std::string &uriString) const
{
    auto params = parameters_.GetParams();
    auto iter = params.cbegin();
    while (iter != params.cend()) {
        sptr<IInterface> o = iter->second;
        if (IString::Query(o) != nullptr) {
            uriString += String::SIGNATURE;
        } else if (IBoolean::Query(o) != nullptr) {
            uriString += Boolean::SIGNATURE;
        } else if (IChar::Query(o) != nullptr) {
            uriString += Char::SIGNATURE;
        } else if (IByte::Query(o) != nullptr) {
            uriString += Byte::SIGNATURE;
        } else if (IShort::Query(o) != nullptr) {
            uriString += Short::SIGNATURE;
        } else if (IInteger::Query(o) != nullptr) {
            uriString += Integer::SIGNATURE;
        } else if (ILong::Query(o) != nullptr) {
            uriString += Long::SIGNATURE;
        } else if (IFloat::Query(o) != nullptr) {
            uriString += Float::SIGNATURE;
        } else if (IDouble::Query(o) != nullptr) {
            uriString += Double::SIGNATURE;
        } else if (IArray::Query(o) != nullptr) {
            uriString += Array::SIGNATURE;
        }
        uriString += "." + Encode(iter->first) + "=" + Encode(Object::ToString(*(o.GetRefPtr()))) + ";";
        iter++;
    }
}

bool Want::WriteUri(Parcel &parcel) const
{
    if (GetUriString().empty()) {
        if (!parcel.WriteInt32(VALUE_NULL)) {
            return false;
        }
    } else {
        if (!parcel.WriteInt32(VALUE_OBJECT)) {
            return false;
        }
        if (!parcel.WriteString16(Str8ToStr16(GetUriString()))) {
            return false;
        }
    }

    return true;
}

bool Want::WriteEntities(Parcel &parcel) const
{
    std::vector<std::string> entities = GetEntities();
    if (entities.empty()) {
        if (!parcel.WriteInt32(VALUE_NULL)) {
            return false;
        }
        return true;
    }

    std::vector<std::u16string> entityU16;
    for (std::vector<std::string>::size_type i = 0; i < entities.size(); i++) {
        entityU16.push_back(Str8ToStr16(entities[i]));
    }
    if (!parcel.WriteInt32(VALUE_OBJECT)) {
        return false;
    }
    if (!parcel.WriteString16Vector(entityU16)) {
        return false;
    }
    return true;
}

bool Want::WriteElement(Parcel &parcel) const
{
    ElementName emptyElement;
    ElementName element = GetElement();
    if (element == emptyElement) {
        if (!parcel.WriteInt32(VALUE_NULL)) {
            return false;
        }
    } else {
        if (!parcel.WriteInt32(VALUE_OBJECT)) {
            return false;
        }
        if (!parcel.WriteParcelable(&element)) {
            return false;
        }
    }

    return true;
}

bool Want::WriteParameters(Parcel &parcel) const
{
    if (parameters_.Size() == 0) {
        if (!parcel.WriteInt32(VALUE_NULL)) {
            return false;
        }
    } else {
        if (!parcel.WriteInt32(VALUE_OBJECT)) {
            return false;
        }
        if (!parcel.WriteParcelable(&parameters_)) {
            return false;
        }
    }

    return true;
}

bool Want::ReadUri(Parcel &parcel)
{
    int empty = VALUE_NULL;
    if (!parcel.ReadInt32(empty)) {
        return false;
    }
    if (empty == VALUE_OBJECT) {
        SetUri(Str16ToStr8(parcel.ReadString16()));
    }

    return true;
}

bool Want::ReadEntities(Parcel &parcel)
{
    std::vector<std::u16string> entityU16;
    std::vector<std::string> entities;
    int empty = VALUE_NULL;
    if (!parcel.ReadInt32(empty)) {
        return false;
    }
    if (empty == VALUE_OBJECT) {
        if (!parcel.ReadString16Vector(&entityU16)) {
            return false;
        }
    }
    for (std::vector<std::u16string>::size_type i = 0; i < entityU16.size(); i++) {
        entities.push_back(Str16ToStr8(entityU16[i]));
    }
    operation_.SetEntities(entities);

    return true;
}

bool Want::ReadElement(Parcel &parcel)
{
    int empty = VALUE_NULL;
    if (!parcel.ReadInt32(empty)) {
        return false;
    }

    if (empty == VALUE_OBJECT) {
        auto element = parcel.ReadParcelable<ElementName>();
        if (element != nullptr) {
            SetElement(*element);
            delete element;
        } else {
            return false;
        }
    }

    return true;
}

bool Want::ReadParameters(Parcel &parcel)
{
    int empty = VALUE_NULL;
    if (!parcel.ReadInt32(empty)) {
        return false;
    }

    if (empty == VALUE_OBJECT) {
        auto params = parcel.ReadParcelable<WantParams>();
        if (params != nullptr) {
            parameters_ = *params;
            delete params;
            params = nullptr;
            std::string moduleName = GetStringParam(PARAM_MODULE_NAME);
            SetModuleName(moduleName);
        } else {
            return false;
        }
    }

    return true;
}
}  // namespace AAFwk
}  // namespace OHOS
