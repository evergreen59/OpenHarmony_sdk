/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_FORM_FWK_FORM_CONSTANTS_H
#define OHOS_FORM_FWK_FORM_CONSTANTS_H

#include <map>
#include <string>

namespace OHOS {
namespace AppExecFwk {
namespace Constants {
    constexpr const char* PERMISSION_REQUIRE_FORM = "ohos.permission.REQUIRE_FORM";
    constexpr const char* PERMISSION_KEEP_BACKGROUND_RUNNING = "ohos.permission.KEEP_BACKGROUND_RUNNING";
    constexpr const char* PARAM_FORM_IDENTITY_KEY = "ohos.extra.param.key.form_identity";
    constexpr const char* PARAM_FORM_CALLING_IDENTITY_KEY = "ohos.extra.param.key.form_calling_identity";
    constexpr const char* PARAM_MODULE_NAME_KEY = "ohos.extra.param.key.module_name";
    constexpr const char* PARAM_BUNDLE_NAME_KEY = "ohos.extra.param.key.bundle_name";
    constexpr const char* PARAM_ABILITY_NAME_KEY = "ohos.extra.param.key.ability_name";
    constexpr const char* PARAM_FORM_NAME_KEY = "ohos.extra.param.key.form_name";
    constexpr const char* PARAM_FORM_DIMENSION_KEY = "ohos.extra.param.key.form_dimension";
    constexpr const char* PARAM_MESSAGE_KEY = "ohos.extra.param.key.message";
    constexpr const char* PARAM_FORM_TEMPORARY_KEY = "ohos.extra.param.key.form_temporary";
    constexpr const char* PARAM_FORM_WIDTH_KEY = "ohos.extra.param.key.form_width";
    constexpr const char* PARAM_FORM_HEIGHT_KEY = "ohos.extra.param.key.form_height";
    constexpr int32_t ONE_HOST_MAX_FORM_SIZE = 256;
    constexpr const char* RECREATE_FORM_KEY = "ohos.extra.param.key.recreate";
    constexpr const char* PARAM_FORM_CUSTOMIZE_KEY = "ohos.extra.param.key.form_customize";
    constexpr const char* PARAM_FORM_ORIENTATION_KEY = "ohos.extra.param.key.form_orientation";
    constexpr int32_t ORIENTATION_PORTRAIT = 1;
    constexpr int32_t ORIENTATION_LANDSCAPE = 2;
    constexpr const char* PARAM_FORM_ABILITY_NAME_KEY = "abilityName";
    constexpr const char* PARAM_FORM_USER_ID = "ohos.extra.param.key.form_user_id";
    constexpr const char* KEY_IS_TIMER = "isCountTimerRefresh";
    constexpr const char* KEY_TIMER_REFRESH = "isTimerRefresh";
    constexpr const char* SYSTEM_PARAM_FORM_UPDATE_TIME = "persist.sys.fms.form.update.time";
    constexpr const char* SYSTEM_PARAM_FORM_REFRESH_MIN_TIME = "persist.sys.fms.form.refresh.min.time";
    constexpr const char* ACTION_UPDATEATTIMER = "form_update_at_timer";
    constexpr const char* KEY_WAKEUP_TIME = "wakeUpTime";
    constexpr const char* KEY_WAKEUP_TIME_LEFT = "wakeUpTimeLeft";
    constexpr const char* KEY_WAKEUP_TIME_RIGHT = "wakeUpTimeRight";
    constexpr const char* KEY_ACTION_TYPE = "form_update_action_type";
    constexpr const char* ABS_CODE_PATH = "/data/app/el1/bundle/public";
    constexpr const char* LOCAL_CODE_PATH = "/data/storage/el1/bundle";
    constexpr const char* LOCAL_BUNDLES = "/data/bundles";
    constexpr const char* FORM_PUBLISH_ACTION = "action.form.publish";
    constexpr const char* PARAM_DEVICE_ID_KEY = "ohos.extra.param.key.device_id";
    constexpr const char* PARAM_FORM_HOST_TOKEN = "ohos.extra.param.form.host.token";
    constexpr const char* PARAM_FORM_ID = "formID";
    constexpr int32_t TYPE_RESET_LIMIT = 1;
    constexpr int32_t TYPE_STATIC_UPDATE = 2;
    constexpr int32_t TYPE_DYNAMIC_UPDATE = 3;
    const long ABS_REFRESH_MS = 2500;

    // The form events type which means that the form becomes visible.
    constexpr int32_t FORM_VISIBLE = 1;
    // The form events type which means that the form becomes invisible.
    constexpr int32_t FORM_INVISIBLE = 2;

    // The default user id
    constexpr int32_t DEFAULT_USER_ID = 0;

    // The max uid of system app.
    constexpr int32_t MAX_SYSTEM_APP_UID = 10000;

    constexpr int32_t MIN_TIME_SPEED = 1;
    constexpr int32_t MAX_TIME_SPEED = 1000;
    constexpr int32_t MAX_HOUR = 23;
    constexpr int32_t MAX_MINUTE = 59;
    constexpr int32_t MAX_SECOND = 60;
    constexpr int32_t MIN_TIME = 0;
    constexpr int32_t HOUR_PER_DAY = 24;
    constexpr int32_t MIN_PER_HOUR = 60;
    constexpr int32_t SEC_PER_MIN = 60;
    constexpr int32_t MS_PER_DAY = 24 * 60 * 60 * 1000;
    const long TIME_1000 = 1000;
    const long TIME_1000000 = 1000000;
    const long TIME_CONVERSION = 30 * 60 * TIME_1000;
    constexpr int32_t MIN_CONFIG_DURATION = 1; // 1 means 30 min
    constexpr int32_t MAX_CONFIG_DURATION = 2 * 24 * 7; // one week
    const long MIN_PERIOD = MIN_CONFIG_DURATION * TIME_CONVERSION; // 30 min in ms unit
    const long MAX_PERIOD = MAX_CONFIG_DURATION * TIME_CONVERSION; // 1 week in ms unit
    const long ABS_TIME = 5 * TIME_1000; // 5s abs time
    constexpr int32_t WORK_POOL_SIZE = 4;
    constexpr const char* TIME_DELIMETER = ":";
    constexpr int32_t UPDATE_AT_CONFIG_COUNT = 2;

    constexpr int32_t LIMIT_COUNT = 50;
    constexpr int32_t MIN_NEXT_TIME = 300; // seconds

    // The max retry times of reconnection.
    constexpr int32_t MAX_RETRY_TIME = 30;
    // The time interval between reconnections(milliseconds).
    constexpr int32_t SLEEP_TIME = 1000;

    constexpr int64_t MS_PER_SECOND = 1000;

    // form host bundlename
    constexpr const char* PARAM_FORM_HOST_BUNDLENAME_KEY = "form_host_bundlename";

    // form manager service bundlename
    constexpr const char* PARAM_FORM_MANAGER_SERVICE_BUNDLENAME_KEY = "form_manager_service_bundlename";

    // PROVIDER_FLAG false:ProviderFormInfo is null;true: ProviderFormInfo not null
    constexpr const char* PROVIDER_FLAG = "provider_flag";
    constexpr const char* FORM_CONNECT_ID = "form_connect_id";
    constexpr const char* FORM_SUPPLY_INFO = "form_supply_info";
    constexpr const char* FORM_SHARE_REQUEST_CODE = "form_share_request_code";
    // the delimiter between bundleName and abilityName
    constexpr const char* NAME_DELIMITER = "::";

    constexpr const char* PARAM_FORM_ADD_COUNT = "form.add.count";

    constexpr const char* FORM_SUPPLY_UID = "form_supply_uid";
    constexpr const char* FORM_RENDER_COMP_ID = "form_render_comp_id";

    const size_t MAX_LAYOUT = 8;
    constexpr int32_t MAX_FORMS = 512;
    constexpr int32_t MAX_RECORD_PER_APP = 256;
    constexpr int32_t MAX_TEMP_FORMS = 256;
    constexpr int32_t MAX_FORM_DATA_SIZE = 1024;

    constexpr int32_t NOT_IN_RECOVERY = 0;
    constexpr int32_t RECOVER_FAIL = 1;
    constexpr int32_t IN_RECOVERING = 2;

    constexpr int32_t FLAG_HAS_OBJECT = 1;
    constexpr int32_t FLAG_NO_OBJECT = 0;

    constexpr int32_t DEATH_RECIPIENT_FLAG = 0;
    constexpr int32_t MAX_VISIBLE_NOTIFY_LIST = 32;

    constexpr const char* ACQUIRE_TYPE = "form_acquire_form";
    constexpr int32_t ACQUIRE_TYPE_CREATE_FORM = 1;
    constexpr int32_t ACQUIRE_TYPE_RECREATE_FORM = 2;

    constexpr int32_t DELETE_FORM = 3;
    constexpr int32_t RELEASE_FORM = 8;
    constexpr int32_t RELEASE_CACHED_FORM = 9;

    constexpr int64_t INVALID_UDID_HASH = 0L;

    enum class Dimension : int8_t {
        DIMENSION_MIN = 1,
        DIMENSION_1_2 = DIMENSION_MIN,
        DIMENSION_2_2,
        DIMENSION_2_4,
        DIMENSION_4_4,
        DIMENSION_2_1,
        DIMENSION_MAX = DIMENSION_2_1
    };

    const std::map<Dimension, std::string> DIMENSION_MAP = {
        {Dimension::DIMENSION_1_2, "1*2"},
        {Dimension::DIMENSION_2_2, "2*2"},
        {Dimension::DIMENSION_2_4, "2*4"},
        {Dimension::DIMENSION_4_4, "4*4"},
        {Dimension::DIMENSION_2_1, "2*1"}
    };

    // rdb
    constexpr const char* FORM_RDB_NAME = "/formdb.db";
    constexpr const char* FORM_RDB_TABLE_NAME = "form_table";
    constexpr const char* FORM_MANAGER_SERVICE_PATH = "/data/service/el1/public/database/form_storage";
    constexpr const char* FORM_JOURNAL_MODE = "WAL";
    constexpr const char* FORM_SYNC_MODE = "FULL";
    constexpr int32_t FORM_RDB_VERSION = 1;
    constexpr const char* FORM_RDB_FILE_TYPE = "";
    constexpr const char* FORM_RENDER_TYPE_KEY = "form_render_type";
    constexpr int32_t RENDER_FORM = 0; // default value
    constexpr int32_t UPDATE_RENDERING_FORM = 1;
    constexpr const char* FORM_COMPILE_MODE_KEY = "form_compile_mode";
    constexpr const char* FORM_COMPATIBLE_VERSION_CODE_KEY = "form_compatible_version_code";

    //ets card
    constexpr const char* FORM_COMP_ID = "ohos.extra.param.key.form_comp_id";
    constexpr const char* FORM_DENSITY = "ohos.extra.param.key.form_density";
    constexpr const char* FORM_PROCESS_ON_ADD_SURFACE = "ohos.extra.param.key.process_on_add_surface";
    constexpr const char* FORM_ALLOW_UPDATE = "allowUpdate";

    //CallEvent
    constexpr const char* FORM_CALL_EVENT_PARAMS = "params";
    constexpr const char* PARAM_FORM_CALL_EVENT_METHOD_KEY = "method";
    constexpr int32_t EVENT_CALL_NOTIFY = 1;
}  // namespace Constants
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_FORM_FWK_FORM_CONSTANTS_H
