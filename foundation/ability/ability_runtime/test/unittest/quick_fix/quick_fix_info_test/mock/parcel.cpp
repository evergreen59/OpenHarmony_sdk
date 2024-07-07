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

#include "parcel.h"

namespace OHOS {
namespace {
bool g_mockWriteUint32 = true;
bool g_mockWriteString = true;
bool g_mockWriteParcelable = true;
bool g_mockReadParcelable = true;
uint8_t g_mockWriteStringCount = 0;
uint8_t g_mockWriteStringMax = 0;
}

void MockWriteUint32(bool state)
{
    g_mockWriteUint32 = state;
}

void MockWriteString(bool state, uint8_t count)
{
    g_mockWriteString = state;
    g_mockWriteStringMax = count;
}

void MockWriteParcelable(bool state)
{
    g_mockWriteParcelable = state;
}

void MockReadParcelable(bool state)
{
    g_mockReadParcelable = state;
}

void ResetParcelState()
{
    g_mockWriteUint32 = true;
    g_mockWriteString = true;
    g_mockWriteParcelable = true;
    g_mockReadParcelable = true;
    g_mockWriteStringCount = 0;
    g_mockWriteStringMax = 0;
}

bool GetMockReadParcelableState()
{
    return g_mockReadParcelable;
}

bool Parcel::WriteUint32(uint32_t) const
{
    return g_mockWriteUint32;
}

bool Parcel::WriteString(const std::string &) const
{
    if (g_mockWriteStringCount < g_mockWriteStringMax) {
        g_mockWriteStringCount++;
        return !g_mockWriteString;
    }
    return g_mockWriteString;
}

bool Parcel::WriteParcelable(const Parcelable *) const
{
    return g_mockWriteParcelable;
}

uint32_t Parcel::ReadUint32() const
{
    return 0;
}

std::string Parcel::ReadString() const
{
    return "";
}

} // namespace OHOS
