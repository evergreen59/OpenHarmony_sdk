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

#ifndef GST_MSG_CONVERTER_DEFAULT_H
#define GST_MSG_CONVERTER_DEFAULT_H

#include <gst/gst.h>
#include "inner_msg_define.h"
#include "nocopyable.h"

namespace OHOS {
namespace Media {
class IGstMsgConverter {
public:
    virtual ~IGstMsgConverter() = default;
    virtual int32_t ConvertToInnerMsg(GstMessage &gstMsg, InnerMessage &innerMsg) const = 0;
};

class GstMsgConverterDefault : public IGstMsgConverter, public NoCopyable {
public:
    GstMsgConverterDefault() = default;
    ~GstMsgConverterDefault() = default;
    int32_t ConvertToInnerMsg(GstMessage &gstMsg, InnerMessage &innerMsg) const override;
};
} // namespace Media
} // namespace OHOS
#endif // GST_MSG_CONVERTER_DEFAULT_H
