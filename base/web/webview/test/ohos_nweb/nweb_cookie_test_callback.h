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

#ifndef NWEB_COOKIE_TEST_CALLBACK_H
#define NWEB_COOKIE_TEST_CALLBACK_H

#include "nweb_cookie_manager.h"
#include "nweb_test_log.h"

namespace OHOS::NWeb {
class SetCookieTestCallback : public NWebValueCallback<bool> {
public:
    void OnReceiveValue(bool value)
    {
        TESTLOG_I("SetCookieTestCallback result:%{public}d", value);
    }
};

class ExistCookieTestCallback : public NWebValueCallback<bool> {
public:
    void OnReceiveValue(bool value)
    {
        TESTLOG_I("ExistCookieTestCallback result:%{public}d", value);
    }
};

class StoreCookieTestCallback : public NWebValueCallback<bool> {
public:
    void OnReceiveValue(bool value)
    {
        TESTLOG_I("StoreCookieTestCallback result:%{public}d", value);
    }
};

class DeleteCookieTestCallback : public NWebValueCallback<bool> {
public:
    void OnReceiveValue(bool value)
    {
        TESTLOG_I("DeleteCookieTestCallback result:%{public}d", value);
    }
};

class ReturnCookieTestCallback : public NWebValueCallback<std::string> {
    void OnReceiveValue(std::string value)
    {
        TESTLOG_I("ReturnCookieTestCallback result:%{public}s", value.c_str());
    }
};
} // namespace OHOS::NWeb

#endif // NWEB_COOKIE_TEST_CALLBACK_H