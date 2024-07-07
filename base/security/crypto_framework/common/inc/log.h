/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HCF_LOG_H
#define HCF_LOG_H

#ifdef HILOG_ENABLE

typedef enum {
    HCF_LOG_LEVEL_DEBUG = 0,
    HCF_LOG_LEVEL_INFO = 1,
    HCF_LOG_LEVEL_WARN = 2,
    HCF_LOG_LEVEL_ERROR = 3
} HcfLogLevel;

#ifdef __cplusplus
extern "C" {
#endif

void HcfLogPrint(HcfLogLevel level, const char *funName, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#include "hilog/log.h"

#ifndef CRYPTO_LOG_DOMAIN
#define CRYPTO_LOG_DOMAIN 0xD002F00 /* Security subsystem's domain id */
#endif

#define LOGD(fmt, ...) (HcfLogPrint(HCF_LOG_LEVEL_DEBUG, __FUNCTION__, fmt, ##__VA_ARGS__))
#define LOGI(fmt, ...) (HcfLogPrint(HCF_LOG_LEVEL_INFO, __FUNCTION__, fmt, ##__VA_ARGS__))
#define LOGW(fmt, ...) (HcfLogPrint(HCF_LOG_LEVEL_WARN, __FUNCTION__, fmt, ##__VA_ARGS__))
#define LOGE(fmt, ...) (HcfLogPrint(HCF_LOG_LEVEL_ERROR, __FUNCTION__, fmt, ##__VA_ARGS__))

#define HCF_LOG_DEBUG(fmt, ...) HiLogPrint(LOG_CORE, LOG_DEBUG, CRYPTO_LOG_DOMAIN, "[HCF]", "%{public}s", buf)
#define HCF_LOG_INFO(buf) HiLogPrint(LOG_CORE, LOG_INFO, CRYPTO_LOG_DOMAIN, "[HCF]", "%{public}s", buf)
#define HCF_LOG_WARN(buf) HiLogPrint(LOG_CORE, LOG_WARN, CRYPTO_LOG_DOMAIN, "[HCF]", "%{public}s", buf)
#define HCF_LOG_ERROR(buf) HiLogPrint(LOG_CORE, LOG_ERROR, CRYPTO_LOG_DOMAIN, "[HCF]", "%{public}s", buf)

#else

#include <stdio.h>

#define LOGD(fmt, ...) printf("[HCF][D][%s]: " fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#define LOGI(fmt, ...) printf("[HCF][I][%s]: " fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#define LOGW(fmt, ...) printf("[HCF][W][%s]: " fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#define LOGE(fmt, ...) printf("[HCF][E][%s]: " fmt "\n", __FUNCTION__, ##__VA_ARGS__)

#endif
#endif
