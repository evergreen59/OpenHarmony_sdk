/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef REQUEST_CONFIG_H
#define REQUEST_CONFIG_H

#include <parcel.h>
#include <string>

namespace OHOS {
namespace Location {
class RequestConfig : public Parcelable {
public:
    RequestConfig();
    explicit RequestConfig(const int scenario);
    ~RequestConfig() override = default;

    inline int GetScenario() const
    {
        return scenario_;
    }

    inline void SetScenario(int scenario)
    {
        scenario_ = scenario;
    }

    inline void SetPriority(int priority)
    {
        priority_ = priority;
    }

    inline int GetPriority()
    {
        return priority_;
    }

    inline int GetTimeInterval() const
    {
        return timeInterval_;
    }

    inline void SetTimeInterval(int timeInterval)
    {
        timeInterval_ = timeInterval;
    }

    inline int GetDistanceInterval() const
    {
        return distanceInterval_;
    }

    inline void SetDistanceInterval(int distanceInterval)
    {
        distanceInterval_ = distanceInterval;
    }

    inline float GetMaxAccuracy() const
    {
        return maxAccuracy_;
    }

    inline void SetMaxAccuracy(float maxAccuracy)
    {
        maxAccuracy_ = maxAccuracy;
    }

    inline void SetFixNumber(int fixNumber)
    {
        fixNumber_ = fixNumber;
    }

    inline int GetFixNumber()
    {
        return fixNumber_;
    }

    inline void SetTimeOut(int time)
    {
        timeOut_ = time;
    }

    inline int GetTimeOut()
    {
        return timeOut_;
    }

    void ReadFromParcel(Parcel& parcel);
    bool Marshalling(Parcel& parcel) const override;
    std::string ToString() const;
    static std::unique_ptr<RequestConfig> Unmarshalling(Parcel& parcel);
    void Set(RequestConfig& requestConfig);
    bool IsSame(RequestConfig& requestConfig);
private:
    int scenario_;
    int timeInterval_; /* Units are seconds */
    int distanceInterval_;
    float maxAccuracy_;
    int fixNumber_;
    int priority_;
    int timeOut_;
};
} // namespace Location
} // namespace OHOS
#endif // REQUEST_CONFIG_H
