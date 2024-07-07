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
package ohos;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.logging.Formatter;
import java.util.logging.LogRecord;

/**
 * Pack for matter
 *
 */
public class PackFormatter extends Formatter {
    private static final int LOG_MAX_LIMIT = 1000;
    private static final DateFormat DF = new SimpleDateFormat("yyyy/MM/dd hh:mm:ss.SSS");

    @Override

    /**
     * format builder
     *
     * @param Log limit size
     * @return string builder
     */
    public String format(LogRecord record) {
        StringBuilder builder = new StringBuilder(LOG_MAX_LIMIT);
        builder.append(DF.format(new Date(record.getMillis()))).append(" - ");
        builder.append(formatMessage(record));
        builder.append(System.lineSeparator());
        return builder.toString();
    }
}