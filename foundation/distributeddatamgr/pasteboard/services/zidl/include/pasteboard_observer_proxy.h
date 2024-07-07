/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef PASTE_BOARD_OBERVER_PROXY_H
#define PASTE_BOARD_OBERVER_PROXY_H

#include <functional>

#include "i_pasteboard_observer.h"
#include "iremote_broker.h"
#include "iremote_object.h"
#include "iremote_proxy.h"
#include "nocopyable.h"
#include "refbase.h"

namespace OHOS {
namespace MiscServices {
class PasteboardObserverProxy : public IRemoteProxy<IPasteboardChangedObserver> {
public:
    explicit PasteboardObserverProxy(const sptr<IRemoteObject> &object);
    ~PasteboardObserverProxy() = default;
    DISALLOW_COPY_AND_MOVE(PasteboardObserverProxy);
    void OnPasteboardChanged() override;
    void OnPasteboardEvent(std::string bundleName, int32_t status) override;
private:
    static inline BrokerDelegator<PasteboardObserverProxy> delegator_;
};
} // namespace MiscServices
} // namespace OHOS
#endif // PASTE_BOARD_OBERVER_PROXY_H
