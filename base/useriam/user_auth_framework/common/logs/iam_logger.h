
/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef IAM_LOGGER_H
#define IAM_LOGGER_H

#include "hilog/log.h"
namespace OHOS {
namespace UserIam {
namespace Common {
#ifdef __FILE_NAME__
#define IAM_LOG_FILE __FILE_NAME__
#else
#define IAM_LOG_FILE (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#ifdef LOG_LABEL
#undef LOG_LABEL
#endif

#define ARGS(fmt, ...) "[%{public}s@%{public}s:%{public}d] " fmt, __FUNCTION__, IAM_LOG_FILE, __LINE__, ##__VA_ARGS__
#define IAM_LOGD(...) OHOS::HiviewDFX::HiLog::Debug(LOG_LABEL, ARGS(__VA_ARGS__))
#define IAM_LOGI(...) OHOS::HiviewDFX::HiLog::Info(LOG_LABEL, ARGS(__VA_ARGS__))
#define IAM_LOGW(...) OHOS::HiviewDFX::HiLog::Warn(LOG_LABEL, ARGS(__VA_ARGS__))
#define IAM_LOGE(...) OHOS::HiviewDFX::HiLog::Error(LOG_LABEL, ARGS(__VA_ARGS__))
#define IAM_LOGF(...) OHOS::HiviewDFX::HiLog::Fatal(LOG_LABEL, ARGS(__VA_ARGS__))

using HiLogLabel = OHOS::HiviewDFX::HiLogLabel;

// common
constexpr unsigned int IAM_DOMAIN_ID_COMMON = 0xD002400;
constexpr HiLogLabel LABEL_IAM_COMMON = {LOG_CORE, IAM_DOMAIN_ID_COMMON, "IAM_COMMON"};

// user idm
constexpr unsigned int IAM_DOMAIN_ID_IDM = 0xD002401;
constexpr HiLogLabel LABEL_USER_IDM_NAPI = {LOG_CORE, IAM_DOMAIN_ID_IDM, "USER_IDM_NAPI"};
constexpr HiLogLabel LABEL_USER_IDM_SDK = {LOG_CORE, IAM_DOMAIN_ID_IDM, "USER_IDM_SDK"};
constexpr HiLogLabel LABEL_USER_IDM_SA = {LOG_CORE, IAM_DOMAIN_ID_IDM, "USER_IDM_SA"};

// user auth
constexpr unsigned int IAM_DOMAIN_ID_USER = 0xD002421;
constexpr HiLogLabel LABEL_USER_AUTH_NAPI = {LOG_CORE, IAM_DOMAIN_ID_USER, "USER_AUTH_NAPI"};
constexpr HiLogLabel LABEL_USER_AUTH_SDK = {LOG_CORE, IAM_DOMAIN_ID_USER, "USER_AUTH_SDK"};
constexpr HiLogLabel LABEL_USER_AUTH_SA = {LOG_CORE, IAM_DOMAIN_ID_USER, "USER_AUTH_SA"};
constexpr HiLogLabel LABEL_USER_AUTH_HDI = {LOG_CORE, IAM_DOMAIN_ID_USER, "USER_AUTH_HDI"};
constexpr HiLogLabel LABEL_USER_AUTH_EXECUTOR = {LOG_CORE, IAM_DOMAIN_ID_USER, "USER_AUTH_EXECUTOR"};

// auth executor manager
constexpr unsigned int IAM_DOMAIN_ID_AUTH_EXECUTOR_MGR = 0xD002431;
constexpr HiLogLabel LABEL_AUTH_EXECUTOR_MGR_SDK = {
    LOG_CORE, IAM_DOMAIN_ID_AUTH_EXECUTOR_MGR, "AUTH_EXECUTOR_MGR_SDK"};
constexpr HiLogLabel LABEL_AUTH_EXECUTOR_MGR_SA = {
    LOG_CORE, IAM_DOMAIN_ID_AUTH_EXECUTOR_MGR, "AUTH_EXECUTOR_MGR_SA"};

// pin
constexpr unsigned int IAM_DOMAIN_ID_PIN = 0xD002441;
constexpr HiLogLabel LABEL_PIN_AUTH_NAPI = {LOG_CORE, IAM_DOMAIN_ID_PIN, "PIN_AUTH_NAPI"};
constexpr HiLogLabel LABEL_PIN_AUTH_SDK = {LOG_CORE, IAM_DOMAIN_ID_PIN, "PIN_AUTH_SDK"};
constexpr HiLogLabel LABEL_PIN_AUTH_SA = {LOG_CORE, IAM_DOMAIN_ID_PIN, "PIN_AUTH_SA"};
constexpr HiLogLabel LABEL_PIN_AUTH_HDI = {LOG_CORE, IAM_DOMAIN_ID_PIN, "PIN_AUTH_HDI"};
constexpr HiLogLabel LABEL_PIN_AUTH_IMPL = {LOG_CORE, IAM_DOMAIN_ID_PIN, "PIN_AUTH_IMPL"};

// face
constexpr unsigned int IAM_DOMAIN_ID_FACE = 0xD002442;
constexpr HiLogLabel LABEL_FACE_AUTH_NAPI = {LOG_CORE, IAM_DOMAIN_ID_FACE, "FACE_AUTH_NAPI"};
constexpr HiLogLabel LABEL_FACE_AUTH_SDK = {LOG_CORE, IAM_DOMAIN_ID_FACE, "FACE_AUTH_SDK"};
constexpr HiLogLabel LABEL_FACE_AUTH_SA = {LOG_CORE, IAM_DOMAIN_ID_FACE, "FACE_AUTH_SA"};
constexpr HiLogLabel LABEL_FACE_AUTH_HDI = {LOG_CORE, IAM_DOMAIN_ID_FACE, "FACE_AUTH_HDI"};
constexpr HiLogLabel LABEL_FACE_AUTH_IMPL = {LOG_CORE, IAM_DOMAIN_ID_FACE, "FACE_AUTH_IMPL"};

// fingerprint
constexpr unsigned int IAM_DOMAIN_ID_FINGERPRINT = 0xD002443;
constexpr HiLogLabel LABEL_FINGERPRINT_AUTH_SA = {LOG_CORE, IAM_DOMAIN_ID_FINGERPRINT, "FINGERPRINT_AUTH_SA"};
constexpr HiLogLabel LABEL_FINGERPRINT_AUTH_HDI = {LOG_CORE, IAM_DOMAIN_ID_FINGERPRINT, "FINGERPRINT_AUTH_HDI"};
constexpr HiLogLabel LABEL_FINGERPRINT_AUTH_IMPL = {
    LOG_CORE, IAM_DOMAIN_ID_FINGERPRINT, "FINGERPRINT_AUTH_IMPL"};
} // namespace Common
} // namespace UserIam
} // namespace OHOS

#endif // IAM_LOGGER_H