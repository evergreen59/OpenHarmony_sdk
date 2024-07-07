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

#ifndef CARDEMULATION_H
#define CARDEMULATION_H

#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace KITS {

class CardEmulation final {
public:
    explicit CardEmulation();
    ~CardEmulation();

    /**
     * @Description Get an object of nfc cardEmulation.
     * @param void
     * @return an object of nfc cardEmulation
     */
    static CardEmulation &GetInstance();

    /**
     * Checks whether a specified type of card emulation is supported.
     * @param feature Indicates the card emulation type, {@code HCE}, {@code UICC}, or {@code ESE}.
     * @return Returns {@code true} if the specified type of card emulation is supported; returns
     * {@code false} otherwise.
     */
    bool IsSupported(FeatureType feature);
};
} // namespace KITS
} // namespace NFC
} // namespace OHOS
#endif // NFC_CONTROLLER_H