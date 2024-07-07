/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef SERVICES_INCLUDE_GLOBAL_H
#define SERVICES_INCLUDE_GLOBAL_H

#include <errno.h>
#include <sys/time.h>
#include <time.h>

#include "hilog/log.h"
#include "ipc_object_stub.h"
#include "iremote_broker.h"
#include "peer_holder.h"
#include "refbase.h"

namespace OHOS {
namespace MiscServices {
#define PLATFORM   OHOS

using BRemoteObject = IPCObjectStub;

#define LOG_INFO(fmt, args...) \
    LogTimeStamp(); printf("I %s:%d  %s - " fmt, basename(__FILE__), __LINE__, __FUNCTION__, ##args)

#define LOG_ERROR(fmt, args...) \
    LogTimeStamp(); printf("E %s:%d  %s - " fmt, basename(__FILE__), __LINE__, __FUNCTION__, ##args)

#define LOG_WARNING(fmt, args...) \
    LogTimeStamp(); printf("W %s:%d  %s - " fmt, basename(__FILE__), __LINE__, __FUNCTION__, ##args)

#if DEBUG
#define LOG_DEBUG(fmt, args...) \
    LogTimeStamp(); printf("D %s:%d  %s - " fmt, basename(__FILE__), __LINE__, __FUNCTION__, ##args)
#else
#define LOG_DEBUG(fmt, args...)
#endif

void LogTimeStamp();

namespace CommonEvent {
    // the events handled in input method management system
    enum {
        COMMON_EVENT_USER_STARTED = 10001, // a user is started.
        COMMON_EVENT_USER_STOPPED = 10002, // a user is stopped.
        COMMON_EVENT_USER_UNLOCKED = 10003, // a user is unlocked.
        COMMON_EVENT_USER_LOCKED = 10004, // a user is locked.
        COMMON_EVENT_SETTING_CHANGED = 10005, // input method setting data is changed.
        COMMON_EVENT_PACKAGE_ADDED = 10006, // a package is installed.
        COMMON_EVENT_PACKAGE_REMOVED = 10007, // a package is removed
    };
}; // namespace CommonEvent

// User State
namespace UserState {
    // The states of a user in the system.
    enum {
        USER_STATE_NOT_AVAILABLE = -1,
        USER_STATE_STARTED = 0, // a user is started.
        USER_STATE_UNLOCKED, // a user is unlocked.
    };
}; // namespace UserState

// Error Code
namespace ErrorCode {
    // Error Code definition in the input method management system
    enum {
        ERROR_STATUS_UNKNOWN_ERROR = (-2147483647 - 1), // unknown error
        ERROR_STATUS_NO_MEMORY = -ENOMEM, // no memory
        ERROR_STATUS_INVALID_OPERATION = -ENOSYS, // invalid operation
        ERROR_STATUS_BAD_VALUE = -EINVAL, // bad value
        ERROR_STATUS_BAD_TYPE = ERROR_STATUS_UNKNOWN_ERROR + 1, // bad type
        ERROR_STATUS_NAME_NOT_FOUND = -ENOENT, // name not found
        ERROR_STATUS_PERMISSION_DENIED = -EPERM, // permission denied
        ERROR_STATUS_NO_INIT = -ENODEV, //  no init
        ERROR_STATUS_ALREADY_EXISTS = -EEXIST,  //  already exist
        ERROR_STATUS_DEAD_OBJECT = -EPIPE, // dead object
        ERROR_STATUS_FAILED_TRANSACTION = ERROR_STATUS_UNKNOWN_ERROR + 2, // failed transaction
        ERROR_STATUS_BAD_INDEX = -EOVERFLOW, // bad index
        ERROR_STATUS_NOT_ENOUGH_DATA = -ENODATA, // not enough data
        ERROR_STATUS_WOULD_BLOCK = -EWOULDBLOCK, // would block
        ERROR_STATUS_TIMED_OUT = -ETIMEDOUT, // time out
        ERROR_STATUS_UNKNOWN_TRANSACTION = -EBADMSG, // unknown transaction
        ERROR_STATUS_FDS_NOT_ALLOWED = ERROR_STATUS_UNKNOWN_ERROR + 7, // fds not allowed
        ERROR_STATUS_UNEXPECTED_NULL = ERROR_STATUS_UNKNOWN_ERROR + 8, // unexpected null,

        // binder exception error code from Status.h
        ERROR_EX_ILLEGAL_ARGUMENT           = -3,   // illegal argument exception
        ERROR_EX_NULL_POINTER               = -4,   // null pointer exception
        ERROR_EX_ILLEGAL_STATE              = -5,   // illegal state exception
        ERROR_EX_NETWORK_MAIN_THREAD        = -6,   // network main thread exception
        ERROR_EX_UNSUPPORTED_OPERATION      = -7,   // unsupported operation exception
        ERROR_EX_SERVICE_SPECIFIC           = -8,   // service specific exception
        ERROR_EX_PARCELABLE                 = -9,   // parcelable exception
        // no error
        NO_ERROR                            = 0,    // no error

        // system service error
        ERROR_NULL_POINTER                  = 1,    // null pointer
        ERROR_BAD_PARAMETERS                = 2,    // bad parameters
        ERROR_USER_NOT_STARTED              = 3,    // user is not started
        ERROR_USER_ALREADY_STARTED          = 4,    // user has already started
        ERROR_USER_NOT_UNLOCKED             = 5,    // user is not unlocked
        ERROR_USER_ALREADY_UNLOCKED         = 6,    // user has already unlocked
        ERROR_USER_NOT_LOCKED               = 7,    // user is not locked

        ERROR_IME_NOT_AVAILABLE             = 8,    // input method engine is not available
        ERROR_SECURITY_IME_NOT_AVAILABLE    = 9,    // security input method engine is not available
        ERROR_TOKEN_CREATE_FAILED           = 10,   // failed to create window token
        ERROR_TOKEN_DESTROY_FAILED          = 11,   // failed to destroy window token
        ERROR_IME_BIND_FAILED               = 12,   // failed to bind IME service
        ERROR_IME_UNBIND_FAILED             = 13,   // failed to unbind IME service
        ERROR_IME_START_FAILED              = 14,   // failed to start IME service
        ERROR_KBD_SHOW_FAILED               = 15,   // failed to show keyboard
        ERROR_KBD_HIDE_FAILED               = 16,   // failed to hide keyboard
        ERROR_IME_NOT_STARTED               = 17,   // input method service is not started
        ERROR_KBD_IS_OCCUPIED               = 18,   // keyboard is showing by other client
        ERROR_KBD_IS_NOT_SHOWING            = 19,   // keyboard is not showing
        ERROR_NOT_IME_PACKAGE               = 20,   // not an IME package
        ERROR_IME_PACKAGE_DUPLICATED        = 21,   // duplicated IME package
        ERROR_SETTING_SAME_VALUE            = 22,   // same setting value
        ERROR_ADD_CLIENT_FAILED             = 23,   // add client failed
        ERROR_SWITCH_IME                    = 24,
        // error from ime
        ERROR_REMOTE_IME_DIED               = 25,   // remote input method service died abnormally
        // error from client
        ERROR_REMOTE_CLIENT_DIED            = 26,   // remote client died abnormally
        ERROR_CLIENT_NOT_FOUND              = 27,   // client is not found
        ERROR_CLIENT_NULL_POINTER           = 28,   // null pointer
        ERROR_SUBSCRIBE_KEYBOARD_EVENT      = 29,
        ERROR_CONTROLLER_INVOKING_FAILED    = 30,
        ERROR_PERSIST_CONFIG                = 31,
        ERROR_PACKAGE_MANAGER               = 32,
        ERROR_SERVICE_START_FAILED          = 33,
        ERROR_NOT_CURRENT_IME               = 34,
    };
    const char *ToString(int errorCode);
}; // namespace ErrorCode

static constexpr HiviewDFX::HiLogLabel g_SMALL_SERVICES_LABEL = {
    LOG_CORE,
    0xD001C00,
    "ImsaKit"
};

#define IMSA_HILOGD(fmt, ...) (void)OHOS::HiviewDFX::HiLog::Debug(OHOS::MiscServices::g_SMALL_SERVICES_LABEL, \
    "line: %{public}d, function: %{public}s," fmt, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define IMSA_HILOGE(fmt, ...) (void)OHOS::HiviewDFX::HiLog::Error(OHOS::MiscServices::g_SMALL_SERVICES_LABEL, \
    "line: %{public}d, function: %{public}s," fmt, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define IMSA_HILOGF(fmt, ...) (void)OHOS::HiviewDFX::HiLog::Fatal(OHOS::MiscServices::g_SMALL_SERVICES_LABEL, \
    "line: %{public}d, function: %{public}s," fmt, __LINE__FILE__, __FUNCTION__, ##__VA_ARGS__)
#define IMSA_HILOGI(fmt, ...) (void)OHOS::HiviewDFX::HiLog::Info(OHOS::MiscServices::g_SMALL_SERVICES_LABEL, \
    "line: %{public}d, function: %{public}s," fmt, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define IMSA_HILOGW(fmt, ...) (void)OHOS::HiviewDFX::HiLog::Warn(OHOS::MiscServices::g_SMALL_SERVICES_LABEL, \
    "line: %{public}d, function: %{public}s," fmt, __LINE__, __FUNCTION__, ##__VA_ARGS__)
} // namespace MiscServices
} // namespace OHOS
#endif // SERVICES_INCLUDE_GLOBAL_H
