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
#ifndef OHOS_DISTRIBUTED_FRAMEWORK_COMMON_BLOCK_OBJECT_H
#define OHOS_DISTRIBUTED_FRAMEWORK_COMMON_BLOCK_OBJECT_H
#include <condition_variable>
#include <mutex>
namespace OHOS {
template<typename T>
class BlockObject {
public:
    explicit BlockObject(uint32_t interval, const T &invalid = T()) : interval_(interval), data_(invalid)
    {
    }
    ~BlockObject() = default;

    void SetValue(T data)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        data_ = std::move(data);
        isSet_ = true;
        cv_.notify_one();
    }

    T GetValue()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait_for(lock, std::chrono::seconds(interval_), [this]() { return isSet_; });
        isSet_ = false;
        T data = std::move(data_);
        cv_.notify_one();
        return data;
    }

    void SetInterval(uint32_t interval)
    {
        interval_ = interval;
    }

private:
    uint32_t interval_;
    bool isSet_ = false;
    std::mutex mutex_;
    std::condition_variable cv_;
    T data_;
};
} // namespace OHOS

#endif // OHOS_DISTRIBUTED_FRAMEWORK_COMMON_BLOCK_OBJECT_H
