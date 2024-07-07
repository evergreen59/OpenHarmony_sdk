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

#ifndef OHOS_ABILITY_RUNTIME_BINDABLE_H
#define OHOS_ABILITY_RUNTIME_BINDABLE_H

#include <memory>

namespace OHOS {
namespace AbilityRuntime {
class Runtime;

class BindingObject final {
public:
    template<class T>
    BindingObject(Runtime& runtime, T* ptr) : runtime_(runtime), object_(ptr, SimpleRelease<T>) {}
    ~BindingObject() = default;

    template<class T>
    T* Get()
    {
        return static_cast<T*>(object_.get());
    }

    void Reset()
    {
        object_.reset();
    }

    void Unbind()
    {
        if (object_) {
            object_.release();
        }
    }

    Runtime& GetRuntime()
    {
        return runtime_;
    }

    BindingObject(const BindingObject&) = delete;
    BindingObject& operator=(const BindingObject&) = delete;
    BindingObject(BindingObject&&) = delete;
    BindingObject& operator=(BindingObject&&) = delete;

private:
    template<class T>
    static void SimpleRelease(void* ptr)
    {
        delete static_cast<T*>(ptr);
    }

    Runtime& runtime_;
    std::unique_ptr<void, void(*)(void*)> object_;
};

class Bindable {
public:
    virtual ~Bindable() = default;

    template<class T>
    void Bind(Runtime& runtime, T* object)
    {
        object_ = std::make_unique<BindingObject>(runtime, object);
    }

    void Unbind()
    {
        if (object_) {
            object_->Unbind();
        }
    }

    const std::unique_ptr<BindingObject>& GetBindingObject() const
    {
        return object_;
    }

protected:
    Bindable() = default;

private:
    std::unique_ptr<BindingObject> object_;
};
}  // namespace AbilityRuntime
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_BINDABLE_H
