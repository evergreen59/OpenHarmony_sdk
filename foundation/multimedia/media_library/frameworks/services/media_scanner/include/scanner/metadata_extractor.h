/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef METADATA_EXTRACTOR_H
#define METADATA_EXTRACTOR_H

#include <fcntl.h>
#include <sstream>
#include <string.h>
#include <unistd.h>

#include "avmetadatahelper.h"
#include "image_source.h"
#include "image_type.h"
#include "metadata.h"

namespace OHOS {
namespace Media {
class MetadataExtractor {
public:
    static int32_t Extract(std::unique_ptr<Metadata> &data);
    static int32_t ExtractAVMetadata(std::unique_ptr<Metadata> &data);
    static int32_t ExtractImageMetadata(std::unique_ptr<Metadata> &data);

private:
    MetadataExtractor() = delete;
    ~MetadataExtractor() = delete;

    static void FillExtractedMetadata(const std::unordered_map<int32_t, std::string> &metadataMap,
        std::unique_ptr<Metadata> &data);
};
} // namespace Media
} // namespace OHOS
#endif /* METADATA_EXTRACTOR_H */
