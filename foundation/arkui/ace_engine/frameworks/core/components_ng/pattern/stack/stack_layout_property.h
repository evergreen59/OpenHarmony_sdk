/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_STACK_STACK_LAYOUT_PROPERTY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_STACK_STACK_LAYOUT_PROPERTY_H

#include "core/components_ng/layout/layout_property.h"

namespace OHOS::Ace::NG {
class ACE_EXPORT StackLayoutProperty final : public LayoutProperty {
    DECLARE_ACE_TYPE(StackLayoutProperty, LayoutProperty);

public:
    StackLayoutProperty() = default;

    ~StackLayoutProperty() override = default;

    RefPtr<LayoutProperty> Clone() const override
    {
        auto layoutProperty = MakeRefPtr<StackLayoutProperty>();
        layoutProperty->UpdateLayoutProperty(this);
        layoutProperty->propAlignmentContent_ = CloneAlignmentContent();
        layoutProperty->propAlignment_ = CloneAlignment();
        return layoutProperty;
    }

    void Reset() override
    {
        LayoutProperty::Reset();
        ResetAlignmentContent();
        ResetAlignment();
    }

    void ToJsonValue(std::unique_ptr<JsonValue>& json) const override
    {
        LayoutProperty::ToJsonValue(json);
        json->Put("alignContent",
            propAlignmentContent_.value_or(Alignment::CENTER).GetAlignmentStr(TextDirection::LTR).c_str());
    }

    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(AlignmentContent, Alignment, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(Alignment, Alignment, PROPERTY_UPDATE_MEASURE);

private:
    ACE_DISALLOW_COPY_AND_MOVE(StackLayoutProperty);
};
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_STACK_STACK_LAYOUT_PROPERTY_H