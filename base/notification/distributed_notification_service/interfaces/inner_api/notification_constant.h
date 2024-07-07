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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_CONSTANT_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_CONSTANT_H

#include <string>

namespace OHOS {
namespace Notification {
class NotificationConstant {
public:
    enum InputEditType {
        EDIT_AUTO,      // Indicates that the system determines whether to allow the user to edit the options before
                        // they are sent to the application.
        EDIT_DISABLED,  // Indicates that the user is not allowed to edit an option before the option is sent to the
                        // application.
        EDIT_ENABLED,   // Indicates that the user is allowed to edit an option before the option is sent to the
                        // application.
    };

    enum InputsSource {
        FREE_FORM_INPUT,  // Indicates that the user manually input the content.
        OPTION,           // Indicates that the user selected one of the provided options.
    };

    enum SemanticActionButton {
        NONE_ACTION_BUTTON,         // Indicates that no WantAgent is associated.
        REPLY_ACTION_BUTTON,        // Indicates the action of replying to a conversation.
        READ_ACTION_BUTTON,         // Indicates the action of marking the content as read.
        UNREAD_ACTION_BUTTON,       // Indicates the action of marking the content as unread.
        DELETE_ACTION_BUTTON,       // Indicates the action of deleting the content associated with the notification.
        ARCHIVE_ACTION_BUTTON,      // Indicates the action of archiving the content associated with the notification.
        MUTE_ACTION_BUTTON,         // Indicates the action of muting the content associated with the notification.
        UNMUTE_ACTION_BUTTON,       // Indicates the action of unmuting the content associated with the notification.
        THUMBS_UP_ACTION_BUTTON,    // Indicates the action of marking the content with a thumbs-up.
        THUMBS_DOWN_ACTION_BUTTON,  // Indicates the action of marking the content with a thumbs-down.
        CALL_ACTION_BUTTON,         // Indicates the action of making a call.
    };

    enum SubscribeResult : uint32_t {
        SUCCESS,
        PREMISSION_FAIL,
        RESOURCES_FAIL,
    };

    enum SlotType {
        SOCIAL_COMMUNICATION,   // the notification type is social communication
        SERVICE_REMINDER,       // the notification type is service reminder
        CONTENT_INFORMATION,    // the notificatin type is content information
        OTHER,                  // the notificatin type is other
        CUSTOM,                 // the notification type is custom
    };

    enum class VisiblenessType {
        /**
         * the notification display effect has not been set by NotificationRequest::setVisibleness().
         * This method is usually not used.
         */
        NO_OVERRIDE,
        /**
         * only the basic information, such as application icon and application name is displayed on the lock screen.
         */
        PRIVATE,
        /**
         * contents of a notification are displayed on the lock screen.
         */
        PUBLIC,
        /**
         * notifications are not displayed on the lock screen.
         */
        SECRET
    };

    enum class DoNotDisturbType {
        NONE    = 0,
        ONCE    = 1,    // only once
        DAILY   = 2,    // every day
        CLEARLY = 3,    // time period
    };

    enum class RemindType {
        NONE                       = -1,
        DEVICE_IDLE_DONOT_REMIND   = 0, // The device is not in use, no reminder
        DEVICE_IDLE_REMIND         = 1, // The device is not in use, remind
        DEVICE_ACTIVE_DONOT_REMIND = 2, // The device is in use, no reminder
        DEVICE_ACTIVE_REMIND       = 3, // The device is in use, reminder
    };

    enum class DistributedReminderPolicy {
        DEFAULT,
        ALWAYS_REMIND,
        DO_NOT_REMIND,
    };

    enum class SourceType {
        /**
         * general notification.
         */
        TYPE_NORMAL = 0x00000000,
        /**
         * long-term task notification.
         */
        TYPE_CONTINUOUS = 0x00000001,
        /**
         * timed notification.
         */
        TYPE_TIMER = 0x00000002
    };

    enum class FlagStatus {
        NONE,
        OPEN,
        CLOSE
    };

    /**
     * Indicates that a notification is deleted because it is clicked.
     */
    static const int32_t CLICK_REASON_DELETE = 1;

    /**
     * Indicates that a notification is deleted because the user clears it.
     */
    static const int32_t CANCEL_REASON_DELETE = 2;

    /**
     * Indicates that a notification is deleted because the user clears all notifications.
     */
    static const int32_t CANCEL_ALL_REASON_DELETE = 3;

    /**
     * Indicates that a notification is deleted because of a UI error.
     */
    static const int32_t ERROR_REASON_DELETE = 4;

    /**
     * Indicates that a notification is deleted because a change has been made to the application.
     */
    static const int32_t PACKAGE_CHANGED_REASON_DELETE = 5;

    /**
     * Indicates that a notification is deleted because the application context is stopped.
     */
    static const int32_t USER_STOPPED_REASON_DELETE = 6;

    /**
     * Indicates that a notification is deleted because the application is banned from sending notifications.
     */
    static const int32_t PACKAGE_BANNED_REASON_DELETE = 7;

    /**
     * Indicates that a notification is deleted because the application cancels it.
     */
    static const int32_t APP_CANCEL_REASON_DELETE = 8;

    /**
     * Indicates that a notification is deleted because the application cancels all notifications.
     */
    static const int32_t APP_CANCEL_ALL_REASON_DELETE = 9;

    /**
     * Indicates that a notification is deleted for other reasons.
     */
    static const int32_t APP_CANCEL_REASON_OTHER = 10;

    /**
     * The key indicates input source.
     */
    static const std::string EXTRA_INPUTS_SOURCE;

    // rdb
    constexpr static const char* NOTIFICATION_RDB_NAME = "/notificationdb.db";
    constexpr static const char* NOTIFICATION_RDB_TABLE_NAME = "notification_table";
    constexpr static const char* NOTIFICATION_RDB_PATH = "/data/service/el1/public/database/notification_service";
    constexpr static const char* NOTIFICATION_JOURNAL_MODE = "WAL";
    constexpr static const char* NOTIFICATION_SYNC_MODE = "FULL";
    constexpr static int32_t NOTIFICATION_RDB_VERSION = 1;
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_CONSTANT_H