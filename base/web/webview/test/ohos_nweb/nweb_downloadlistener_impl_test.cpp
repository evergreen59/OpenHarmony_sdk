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

#include "nweb_downloadlistener_impl_test.h"

#include <iostream>
#include "nweb_test_log.h"

namespace OHOS::NWeb {
void NWebDownloadListenerImplTest::OnDownloadStart(const std::string &url, const std::string &userAgent,
                                                   const std::string &contentDisposition,
                                                   const std::string &mimetype,
                                                   long contentLength)
{
    TESTLOG_I("NWebDownloadListenerImplTest::onDownloadStart");
    TESTLOG_I("url=%{public}s", url.c_str());
    TESTLOG_I("userAgent=%{public}s", userAgent.c_str());
    TESTLOG_I("contentDisposition=%{public}s", contentDisposition.c_str());
    TESTLOG_I("mimetype=%{public}s", mimetype.c_str());
    TESTLOG_I("contentLength=%{public}ld", contentLength);
}
} // namespace OHOS::NWeb