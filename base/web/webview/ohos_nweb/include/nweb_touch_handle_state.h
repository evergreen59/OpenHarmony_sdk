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

#ifndef NWEB_TOUCH_HANDLE_STATE_H
#define NWEB_TOUCH_HANDLE_STATE_H

#include <string>

#include "nweb_export.h"
namespace OHOS::NWeb {
class OHOS_NWEB_EXPORT NWebTouchHandleState {
public:
    enum TouchHandleType {
        INSERT_HANDLE,
        SELECTION_BEGIN_HANDLE,
        SELECTION_END_HANDLE,
        INVALID_HANDLE
    };

    NWebTouchHandleState() = default;

    virtual ~NWebTouchHandleState() = default;

    virtual int32_t GetTouchHandleId() const = 0;

    virtual int32_t GetX() const = 0;

    virtual int32_t GetY() const = 0;

    virtual TouchHandleType GetTouchHandleType() const = 0;

    virtual bool IsEnable() const = 0;

    virtual float GetAlpha() const = 0;

    virtual float GetEdgeHeight() const = 0;
};
}

#endif // NWEB_TOUCH_HANDLE_STATE_H