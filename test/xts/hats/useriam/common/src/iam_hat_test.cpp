/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "iam_hat_test.h"

#include <cstdint>
#include <string>
#include <vector>

#include "parcel.h"
#include "securec.h"
using namespace std;


#define LOG_LABEL UserIam::Common::LABEL_IAM_COMMON

namespace OHOS {
    namespace UserIam {
        namespace Common {
            namespace {
                constexpr int32_t MAX_DATA_LEN = 200;
                void FillTestBuffer(Parcel &parcel, void *p, uint32_t len)
                {
                    if (len == 0) {
                        return;
                    }

                    auto buffer = parcel.ReadBuffer(len);
                    if (buffer == nullptr) {
                        cout << "ReadBuffer len " << len << " fail!" << endl;
                        return;
                    }

                    if (memcpy_s(p, len, buffer, len) != EOK) {
                        cout << "memcpy_s fail" << endl;
                        return;
                    }

                    return;
                }
            } // namespace

            void FillTestUint8Vector(Parcel &parcel, std::vector<uint8_t> &data)
            {
                uint32_t len = parcel.ReadUint32() % MAX_DATA_LEN;
                uint32_t memLen = len * sizeof(uint8_t);
                data.resize(len);
                FillTestBuffer(parcel, static_cast<void *>(&data[0]), memLen);
                cout << "fill vector len " << len << "ok" << endl;
            }

            void FillTestInt8Vector(Parcel &parcel, std::vector<int8_t> &data)
            {
                uint32_t len = parcel.ReadUint32() % MAX_DATA_LEN;
                uint32_t memLen = len * sizeof(int8_t);
                data.resize(len);
                FillTestBuffer(parcel, static_cast<void *>(&data[0]), memLen);
                cout << "fill vector len " << len << "ok" << endl;
            }

            void FillTestUint32Vector(Parcel &parcel, std::vector<uint32_t> &data)
            {
                uint32_t len = parcel.ReadUint32() % MAX_DATA_LEN;
                uint32_t memLen = len * sizeof(uint32_t);
                data.resize(len);
                FillTestBuffer(parcel, static_cast<void *>(&data[0]), memLen);
                cout << "fill vector len " << len << "ok" << endl;
            }

            void FillTestUint64Vector(Parcel &parcel, std::vector<uint64_t> &data)
            {
                uint32_t len = parcel.ReadUint32() % MAX_DATA_LEN;
                uint32_t memLen = len * sizeof(uint64_t);
                data.resize(len);
                FillTestBuffer(parcel, static_cast<void *>(&data[0]), memLen);
                cout << "fill vector len " << len << "ok" << endl;
            }

            void FillTestString(Parcel &parcel, std::string &str)
            {
                uint32_t len = parcel.ReadUint32() % MAX_DATA_LEN + 1;
                uint32_t memLen = len * sizeof(char);
                std::vector<char> data(len, 0);
                FillTestBuffer(parcel, static_cast<void *>(&data[0]), memLen - 1);
                str = std::string(&data[0]);
                cout << "fill string len " << (len - 1) << " ok" << endl;
            }
        } // namespace Common
    }     // namespace UserIam
} // namespace OHOS