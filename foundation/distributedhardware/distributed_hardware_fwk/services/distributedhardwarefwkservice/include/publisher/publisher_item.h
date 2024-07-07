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

#ifndef OHOS_PUBLISHER_ITEM_H
#define OHOS_PUBLISHER_ITEM_H
#include <mutex>
#include <string>
#include <set>

#include "refbase.h"

#include "ipublisher_listener.h"
#include "single_instance.h"

namespace OHOS {
namespace DistributedHardware {
class PublisherItem {
REMOVE_NO_USE_CONSTRUCTOR(PublisherItem);
public:
    PublisherItem();
    explicit PublisherItem(DHTopic topic);
    virtual ~PublisherItem();
    void AddListener(const sptr<IPublisherListener> &listener);
    void RemoveListener(const sptr<IPublisherListener> &listener);
    void PublishMessage(const std::string &message);
private:
    DHTopic topic_;
    std::mutex mutex_;
    std::set<sptr<IPublisherListener>> listeners_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_PUBLISHER_ITEM_H