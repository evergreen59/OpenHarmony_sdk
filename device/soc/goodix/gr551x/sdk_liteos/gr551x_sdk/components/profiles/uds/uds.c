/**
 ****************************************************************************************
 *
 * @file uds.c
 *
 * @brief User Data Service implementation.
 *
 ****************************************************************************************
 * @attention
  #####Copyright (c) 2019 GOODIX
  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of GOODIX nor the names of its contributors may be used
    to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "uds.h"
#include "wss_db.h"
#include "ble_prf_types.h"
#include "ble_prf_utils.h"
#include "utility.h"
#include "app_log.h"

#define HEIGHT_165 165
#define HEIGHT_1650 1650
#define YEAR_2000_2000 2000
#define MONTH_01 01
#define DAY_01 01
#define INDEX_2 2
#define INDEX_3 3
#define INDEX_5 5
#define AGE_18 18
#define YEAR_LOW 1582
#define YEAR_HIGHT 9999
#define ROLL_NUM 63
#define MONTH_12 12
#define DAY_31 31
#define HEIGHT_40 40
#define HEIGHT_250 250
#define USER_DATA_OFFSET 2
#define NAME_LEN_OFFSET 3
/*
 * ENUMERATIONS
 ****************************************************************************************
 */
/**@brief User Data Service Attributes Indexes. */
enum {
    // User Data Service
    UDS_IDX_SVC,

    // Database Change Increment Characteristic
    UDS_IDX_DB_CHANGE_INCR_CHAR,
    UDS_IDX_DB_CHANGE_INCR_VAL,
    UDS_IDX_DB_CHANGE_INCR_NTF_CFG,

    // User Index Characteristic
    UDS_IDX_USER_INDEX_CHAR,
    UDS_IDX_USER_INDEX_VAL,

    // User Control Point Characteristic
    UDS_IDX_CTRL_POINT_CHAR,
    UDS_IDX_CTRL_POINT_VAL,
    UDS_IDX_CTRL_POINT_IND_CFG,

    // Age Characteristic
    UDS_IDX_AGE_CHAR,
    UDS_IDX_AGE_VAL,

    // Date of Birth Characteristics
    UDS_IDX_DATE_OF_BIRTH_CHAR,
    UDS_IDX_DATE_OF_BIRTH_VAL,

    // First Name Characteristic
    UDS_IDX_FIRST_NAME_CHAR,
    UDS_IDX_FIRST_NAME_VAL,

    // Height Characteristic
    UDS_IDX_HEIGHT_CHAR,
    UDS_IDX_HEIGHT_VAL,

    // Gender Characteristic
    UDS_IDX_GENDER_CHAR,
    UDS_IDX_GENDER_VAL,

    // Registered User Characteristic
    UDS_IDX_REGIST_USER_CHAR,
    UDS_IDX_REGIST_USER_VAL,
    UDS_IDX_REGIST_USER_IND_CFG,

    UDS_IDX_NB
};

/*
 * STRUCTURES
 *****************************************************************************************
 */
/**@brief User Data Service environment variable. */
struct uds_env_t {
    uds_init_t
    uds_init;                                    /**< User Data Service initialization variables. */
    uint16_t                 start_hdl;                                   /**< User Data Service start handle. */
    bool
    ucp_in_progress;                             /**< A previously triggered Control Point operationis
                                                 still in progress. */
    uint8_t
    segm_head_roll_num;                          /**< Rolling Segment Number of Segmentation Header. */
    uint8_t                  consent_try_num;                             /**< The number of consent tries. */
    uint16_t
    db_change_incr_ntf_cfg[UDS_CONNECTION_MAX];  /**< The configuration of Current Time Notification
                                                 which is configured by the peer devices. */
    uint16_t
    ctrl_point_ind_cfg[UDS_CONNECTION_MAX];      /**< The configuration of SC Control Point Notification
                                                 which is configured by the peer devices. */
    uint16_t
    regist_user_ind_cfg[UDS_CONNECTION_MAX];     /**< The configuration of SC Control Point Notification
                                                 which is configured by the peer devices. */
};

/*
 * LOCAL FUNCTION DECLARATION
 *****************************************************************************************
 */
static sdk_err_t   uds_init(void);
static void        uds_disconnect_cb(uint8_t conn_idx, uint8_t reason);

static void        uds_read_att_cb(uint8_t conidx, const gatts_read_req_cb_t *p_param);
static void        uds_write_att_cb(uint8_t conidx, const gatts_write_req_cb_t *p_param);
static void        uds_gatts_prep_write_cb(uint8_t conn_idx, const gatts_prep_write_req_cb_t *p_prep_req);
static void        uds_cccd_set_cb(uint8_t conn_idx, uint16_t handle, uint16_t cccd_value);
static void        uds_gatts_ntf_ind_cb(uint8_t conn_idx, uint8_t status, const ble_gatts_ntf_ind_t *p_ntf_ind);
static void        uds_age_write_handler(const uint8_t *p_data, uint16_t length, gatts_write_cfm_t *p_cfm);
static void        uds_date_of_birth_write_handler(const uint8_t *p_data, uint16_t length, gatts_write_cfm_t *p_cfm);
static void        uds_first_name_write_handler(const uint8_t *p_data, uint16_t length, gatts_write_cfm_t *p_cfm);
static void        uds_height_write_handler(const uint8_t *p_data, uint16_t length, gatts_write_cfm_t *p_cfm);
static void        uds_gender_write_handler(const uint8_t *p_data, uint16_t length, gatts_write_cfm_t *p_cfm);
static void        uds_db_change_incr_write_handler(const uint8_t *p_data, uint16_t length, gatts_write_cfm_t *p_cfm);

static void        uds_receive_ucp_handler(uint8_t conn_idx, const uint8_t *p_data, uint16_t length);
static void        uds_regist_user_value_encode(uint8_t conn_idx);
static sdk_err_t   uds_indicate_user_char_val_chunk(uint8_t conn_idx);

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
static struct uds_env_t            s_uds_env;
static uds_regi_user_data_stream_t s_regi_user_char_val_stream;

/**@brief Full UDS Database Description - Used to add attributes into the database. */
static const attm_desc_t uds_attr_tab[UDS_IDX_NB] = {
    // User Data Service Declaration
    [UDS_IDX_SVC]                    = {BLE_ATT_DECL_PRIMARY_SERVICE, READ_PERM_UNSEC, 0, 0},

    // Database Change Increment Characteristic - Declaration
    [UDS_IDX_DB_CHANGE_INCR_CHAR]    = {BLE_ATT_DECL_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0},
    // Database Change Increment Characteristic - Value
    [UDS_IDX_DB_CHANGE_INCR_VAL]     = {
        BLE_ATT_CHAR_DATABASE_CHANGE_INCREMENT,
        READ_PERM_UNSEC | NOTIFY_PERM_UNSEC | WRITE_REQ_PERM_UNSEC,
        ATT_VAL_LOC_USER,
        UDS_DB_CHANGE_INCR_VAL_LEN_MAX
    },
    // Database Change Increment Characteristic - Client Characteristic Configuration Descriptor
    [UDS_IDX_DB_CHANGE_INCR_NTF_CFG] = {
        BLE_ATT_DESC_CLIENT_CHAR_CFG,
        READ_PERM_UNSEC | WRITE_REQ_PERM_UNSEC,
        0,
        0
    },

    // User Index Characteristic - Declaration
    [UDS_IDX_USER_INDEX_CHAR]        = {BLE_ATT_DECL_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0},
    // User Index Characteristic - Value
    [UDS_IDX_USER_INDEX_VAL]         = {
        BLE_ATT_CHAR_USER_INDEX,
        READ_PERM_UNSEC,
        ATT_VAL_LOC_USER,
        UDS_USER_INDEX_VAL_LEN_MAX
    },

    // User Control Point Characteristic - Declaration
    [UDS_IDX_CTRL_POINT_CHAR]        = {BLE_ATT_DECL_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0},
    // User Control Point Characteristic - Value
    [UDS_IDX_CTRL_POINT_VAL]         = {
        BLE_ATT_CHAR_USER_CONTROL_POINT,
        WRITE_REQ_PERM_UNSEC | INDICATE_PERM(UNAUTH),
        ATT_VAL_LOC_USER,
        UDS_CTRL_PT_VAL_LEN_MAX
    },
    // User Control Point Characteristic - Client Characteristic Configuration Descriptor
    [UDS_IDX_CTRL_POINT_IND_CFG]     = {
        BLE_ATT_DESC_CLIENT_CHAR_CFG,
        READ_PERM_UNSEC | WRITE_REQ_PERM_UNSEC,
        0,
        0
    },

    // Age Characteristic - Declaration
    [UDS_IDX_AGE_CHAR]               = {BLE_ATT_DECL_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0},
    // Age Characteristic - Value
    [UDS_IDX_AGE_VAL]                = {
        BLE_ATT_CHAR_AGE,
        READ_PERM_UNSEC | WRITE_REQ_PERM_UNSEC,
        ATT_VAL_LOC_USER,
        UDS_AGE_VAL_LEN_MAX
    },

    // Date of Birth Characteristic - Declaration
    [UDS_IDX_DATE_OF_BIRTH_CHAR]     = {BLE_ATT_DECL_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0},
    // Date of Birth Characteristic - Value
    [UDS_IDX_DATE_OF_BIRTH_VAL]      = {
        BLE_ATT_CHAR_DATE_OF_BIRTH,
        READ_PERM_UNSEC | WRITE_REQ_PERM_UNSEC,
        ATT_VAL_LOC_USER,
        UDS_DATE_OF_BIRTH_VAL_LEN_MAX
    },

    // First Name Characteristic - Declaration
    [UDS_IDX_FIRST_NAME_CHAR]        = {BLE_ATT_DECL_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0},
    // First Name Characteristic - Value
    [UDS_IDX_FIRST_NAME_VAL]         = {
        BLE_ATT_CHAR_FIRST_NAME,
        READ_PERM_UNSEC | WRITE_REQ_PERM_UNSEC,
        ATT_VAL_LOC_USER,
        UDS_FIRST_NAME_VAL_LEN_MAX
    },

    // Height Characteristic - Declaration
    [UDS_IDX_HEIGHT_CHAR]            = {BLE_ATT_DECL_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0},
    // Height Characteristic - Value
    [UDS_IDX_HEIGHT_VAL]             = {
        BLE_ATT_CHAR_HEIGHT,
        READ_PERM_UNSEC | WRITE_REQ_PERM_UNSEC,
        ATT_VAL_LOC_USER,
        UDS_HEIGHT_VAL_LEN_MAX
    },

    // Gender Characteristic - Declaration
    [UDS_IDX_GENDER_CHAR]            = {BLE_ATT_DECL_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0},
    // Gender Characteristic - Value
    [UDS_IDX_GENDER_VAL]             = {
        BLE_ATT_CHAR_GENDER,
        READ_PERM_UNSEC | WRITE_REQ_PERM_UNSEC,
        ATT_VAL_LOC_USER,
        UDS_GENDER_VAL_LEN_MAX
    },

    // Registered User Characteristic Declaration
    [UDS_IDX_REGIST_USER_CHAR]       = {BLE_ATT_DECL_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0},
    // Registered User Characteristic Declaration value
    [UDS_IDX_REGIST_USER_VAL]        = {
        BLE_ATT_CHAR_REGISTERED_USER,
        INDICATE_PERM_UNSEC,
        ATT_VAL_LOC_USER,
        UDS_REGI_USER_VAL_LEN_MAX
    },
    // Registered User Characteristic Declaration  - Client Characteristic Configuration Descriptor
    [UDS_IDX_REGIST_USER_IND_CFG]    = {
        BLE_ATT_DESC_CLIENT_CHAR_CFG,
        READ_PERM_UNSEC | WRITE_REQ_PERM_UNSEC,
        0,
        0
    },
};

/**@brief UDS Task interface required by profile manager. */
static ble_prf_manager_cbs_t uds_task_cbs = {
    (prf_init_func_t) uds_init,
    NULL,
    uds_disconnect_cb
};

/**@brief UDS Task Callbacks. */
static gatts_prf_cbs_t uds_cb_func = {
    uds_read_att_cb,
    uds_write_att_cb,
    uds_gatts_prep_write_cb,
    uds_gatts_ntf_ind_cb,
    uds_cccd_set_cb
};

/**@brief UDS Information. */
static const prf_server_info_t uds_prf_info = {
    .max_connection_nb = UDS_CONNECTION_MAX,
    .manager_cbs       = &uds_task_cbs,
    .gatts_prf_cbs     = &uds_cb_func
};

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 * @brief Initialize User Data service and create db in att
 *
 * @return Error code to know if profile initialization succeed or not.
 *****************************************************************************************
 */
static sdk_err_t uds_init(void)
{
    // The start handle must be set with PRF_INVALID_HANDLE to be allocated automatically by BLE Stack.
    uint16_t          start_hdl      = PRF_INVALID_HANDLE;
    const uint8_t     uds_svc_uuid[] = BLE_ATT_16_TO_16_ARRAY(BLE_ATT_SVC_USER_DATA);
    sdk_err_t         error_code;
    gatts_create_db_t gatts_db;

    error_code = memset_s(&gatts_db, sizeof(gatts_db), 0, sizeof(gatts_db));
    if (error_code < 0) {
        return error_code;
    }

    gatts_db.shdl                 = &start_hdl;
    gatts_db.uuid                 = uds_svc_uuid;
    gatts_db.attr_tab_cfg         = (uint8_t *)&(s_uds_env.uds_init.char_mask);
    gatts_db.max_nb_attr          = UDS_IDX_NB;
    gatts_db.srvc_perm            = 0;
    gatts_db.attr_tab_type        = SERVICE_TABLE_TYPE_16;
    gatts_db.attr_tab.attr_tab_16 = uds_attr_tab;

    error_code = ble_gatts_srvc_db_create(&gatts_db);
    if (SDK_SUCCESS == error_code) {
        s_uds_env.start_hdl = *gatts_db.shdl;
    }

    return error_code;
}

/**
 *****************************************************************************************
 * @brief Handles reception of the disconnection event.
 *
 * @param[in] conn_idx: Connection index.
 * @param[in] reason:   Reason of disconnection.
 *****************************************************************************************
 */
static void uds_disconnect_cb(uint8_t conn_idx, uint8_t reason)
{
    s_uds_env.ucp_in_progress = false;
}

/**
 *****************************************************************************************
 * @brief Handles reception of the attribute info request message.
 *
 * @param[in] conn_idx: Connection index
 * @param[in] p_param:  Pointer to the parameters of the read request.
 *****************************************************************************************
 */
static void uds_read_att_cb(uint8_t conn_idx, const gatts_read_req_cb_t *p_param)
{
    gatts_read_cfm_t  cfm;
    uint8_t           handle    = p_param->handle;
    uint8_t           tab_index = prf_find_idx_by_handle(handle,
                                  s_uds_env.start_hdl,
                                  UDS_IDX_NB,
                                  (uint8_t *)&s_uds_env.uds_init.char_mask);
    cfm.handle = handle;
    cfm.status = BLE_SUCCESS;

    wss_rec_t loc_rec;

    switch (tab_index) {
        case UDS_IDX_AGE_VAL: {
            if (s_uds_env.uds_init.user_index == UDS_UNKNOWN_USER) {
                cfm.status = BLE_ATT_ERR_READ_NOT_PERMITTED;
                cfm.length = sizeof(uint8_t);
                cfm.value  = (uint8_t *)UDS_ERROR_UD_ACCESS_NOT_PERMIT;
            } else {
                wss_db_record_get(s_uds_env.uds_init.user_index, &loc_rec);
                cfm.length = sizeof(uint8_t);
                cfm.value  = (uint8_t *)&loc_rec.age;
            }
            break;
        }

        case UDS_IDX_DATE_OF_BIRTH_VAL: {
            if (s_uds_env.uds_init.user_index == UDS_UNKNOWN_USER) {
                cfm.status = BLE_ATT_ERR_READ_NOT_PERMITTED;
                cfm.length = sizeof(uint8_t);
                cfm.value  = (uint8_t *)UDS_ERROR_UD_ACCESS_NOT_PERMIT;
            } else {
                wss_db_record_get(s_uds_env.uds_init.user_index, &loc_rec);
                cfm.length = sizeof(birth_date_t);
                cfm.value  = (uint8_t *)&loc_rec.date_of_birth;
            }
            break;
        }

        case UDS_IDX_FIRST_NAME_VAL: {
            if (s_uds_env.uds_init.user_index == UDS_UNKNOWN_USER) {
                cfm.status = BLE_ATT_ERR_READ_NOT_PERMITTED;
                cfm.length = sizeof(uint8_t);
                cfm.value  = (uint8_t *)UDS_ERROR_UD_ACCESS_NOT_PERMIT;
            } else {
                wss_db_record_get(s_uds_env.uds_init.user_index, &loc_rec);
                cfm.length = loc_rec.name_length;
                cfm.value  = (uint8_t *)loc_rec.first_name;
            }
            break;
        }

        case UDS_IDX_HEIGHT_VAL: {
            if (s_uds_env.uds_init.user_index == UDS_UNKNOWN_USER) {
                cfm.status = BLE_ATT_ERR_READ_NOT_PERMITTED;
                cfm.length = sizeof(uint8_t);
                cfm.value  = (uint8_t *)UDS_ERROR_UD_ACCESS_NOT_PERMIT;
            } else {
                wss_db_record_get(s_uds_env.uds_init.user_index, &loc_rec);
                cfm.length = sizeof(uint16_t);
                cfm.value  = (uint8_t *)&loc_rec.height;
            }
            break;
        }

        case UDS_IDX_GENDER_VAL: {
            if (s_uds_env.uds_init.user_index == UDS_UNKNOWN_USER) {
                cfm.status = BLE_ATT_ERR_READ_NOT_PERMITTED;
                cfm.length = sizeof(uint8_t);
                cfm.value  = (uint8_t *)UDS_ERROR_UD_ACCESS_NOT_PERMIT;
            } else {
                wss_db_record_get(s_uds_env.uds_init.user_index, &loc_rec);
                cfm.length = sizeof(uint8_t);
                cfm.value  = (uint8_t *)&loc_rec.gender;
            }
            break;
        }

        case UDS_IDX_DB_CHANGE_INCR_VAL: {
            uint8_t   user_index = s_uds_env.uds_init.user_index;
            if (UDS_UNKNOWN_USER == user_index) {
                uint32_t db_change_incr_val = UDS_DB_CHANGE_INCR_DEFAULT_VAL;
                cfm.length = sizeof(uint32_t);
                cfm.value  = (uint8_t *)&db_change_incr_val;
            } else {
                if (wss_db_record_get(user_index, &loc_rec)) {
                    cfm.length = sizeof(uint32_t);
                    cfm.value  = (uint8_t *)&loc_rec.db_change_incr_val;
                }
            }
            break;
        }

        case UDS_IDX_DB_CHANGE_INCR_NTF_CFG:
            cfm.length = sizeof(uint16_t);
            cfm.value  = (uint8_t *)&s_uds_env.db_change_incr_ntf_cfg[conn_idx];
            break;

        case UDS_IDX_USER_INDEX_VAL: {
            cfm.length = sizeof(uint8_t);
            cfm.value  = (uint8_t *)&s_uds_env.uds_init.user_index;
            break;
        }

        case UDS_IDX_REGIST_USER_IND_CFG: {
            cfm.length = sizeof(uint16_t);
            cfm.value  = (uint8_t *)&s_uds_env.regist_user_ind_cfg[conn_idx];
            break;
        }

        case UDS_IDX_CTRL_POINT_IND_CFG: {
            cfm.length = sizeof(uint16_t);
            cfm.value  = (uint8_t *)&s_uds_env.ctrl_point_ind_cfg[conn_idx];
            break;
        }

        default:
            cfm.length = 0;
            cfm.status = BLE_ATT_ERR_INVALID_HANDLE;
            break;
    }

    ble_gatts_read_cfm(conn_idx, &cfm);
}

/**
 *****************************************************************************************
 * @brief Handles reception of the write request.
 *
 * @param[in]: conn_idx: Connection index
 * @param[in]: p_param:  Pointer to the parameters of the write request.
 *****************************************************************************************
 */
static void uds_write_att_cb(uint8_t conn_idx, const gatts_write_req_cb_t *p_param)
{
    uint16_t          handle      = p_param->handle;
    uint16_t          tab_index   = 0;
    uint16_t          cccd_value  = 0;
    bool              ucp_evt     = false;
    uds_evt_t         event;
    gatts_write_cfm_t cfm;

    tab_index      = prf_find_idx_by_handle(handle,
                                            s_uds_env.start_hdl,
                                            UDS_IDX_NB,
                                            (uint8_t *)&s_uds_env.uds_init.char_mask);
    cfm.handle     = handle;
    cfm.status     = BLE_SUCCESS;
    event.evt_type = UDS_EVT_INVALID;
    event.conn_idx = conn_idx;

    switch (tab_index) {
        case UDS_IDX_AGE_VAL:
            if (UDS_UNKNOWN_USER == s_uds_env.uds_init.user_index) {
                cfm.status = UDS_ERROR_UD_ACCESS_NOT_PERMIT;
            }

            event.evt_type = UDS_EVT_AGE_SET_BY_PEER;
            event.p_data   = p_param->value;
            event.length   = p_param->length;
            uds_age_write_handler(p_param->value, p_param->length, &cfm);
            break;

        case UDS_IDX_DATE_OF_BIRTH_VAL:
            if (UDS_UNKNOWN_USER  == s_uds_env.uds_init.user_index) {
                cfm.status = UDS_ERROR_UD_ACCESS_NOT_PERMIT;
            }
            event.evt_type = UDS_EVT_DATE_OF_BIRTH_SET_BY_PEER;
            event.p_data   = p_param->value;
            event.length   = p_param->length;
            uds_date_of_birth_write_handler(p_param->value, p_param->length, &cfm);
            break;

        case UDS_IDX_FIRST_NAME_VAL:
            if (UDS_UNKNOWN_USER  == s_uds_env.uds_init.user_index) {
                cfm.status = UDS_ERROR_UD_ACCESS_NOT_PERMIT;
            }
            event.evt_type = UDS_EVT_FIRST_NAME_SET_BY_PEER;
            event.p_data   = p_param->value;
            event.length   = p_param->length;
            uds_first_name_write_handler(p_param->value, p_param->length, &cfm);
            break;

        case UDS_IDX_HEIGHT_VAL:
            if (UDS_UNKNOWN_USER  == s_uds_env.uds_init.user_index) {
                cfm.status = UDS_ERROR_UD_ACCESS_NOT_PERMIT;
            }
            event.evt_type = UDS_EVT_HEIGHT_SET_BY_PEER;
            event.p_data   = p_param->value;
            event.length   = p_param->length;
            uds_height_write_handler(p_param->value, p_param->length, &cfm);
            break;

        case UDS_IDX_GENDER_VAL:
            if (UDS_UNKNOWN_USER  == s_uds_env.uds_init.user_index) {
                cfm.status = UDS_ERROR_UD_ACCESS_NOT_PERMIT;
            }
            event.evt_type = UDS_EVT_GENDER_SET_BY_PEER;
            event.p_data   = p_param->value;
            event.length   = p_param->length;
            uds_gender_write_handler(p_param->value, p_param->length, &cfm);
            break;

        case UDS_IDX_DB_CHANGE_INCR_VAL:
            if (UDS_UNKNOWN_USER  == s_uds_env.uds_init.user_index) {
                cfm.status = UDS_ERROR_UD_ACCESS_NOT_PERMIT;
            }
            event.evt_type = UDS_EVT_DB_CHANGE_INCR_SET_BY_PEER;
            event.p_data   = p_param->value;
            event.length   = p_param->length;
            uds_db_change_incr_write_handler(p_param->value, p_param->length, &cfm);
            break;

        case UDS_IDX_DB_CHANGE_INCR_NTF_CFG:
            cccd_value     = le16toh(&p_param->value[0]);
            event.evt_type = ((PRF_CLI_START_NTF == cccd_value) ? \
                              UDS_EVT_DB_CHANGE_INCR_NOTIFICATION_ENABLE : \
                              UDS_EVT_DB_CHANGE_INCR_NOTIFICATION_DISABLE);
            s_uds_env.db_change_incr_ntf_cfg[conn_idx] = cccd_value;
            break;

        case UDS_IDX_REGIST_USER_IND_CFG:
            cccd_value     = le16toh(&p_param->value[0]);
            event.evt_type = ((PRF_CLI_START_IND == cccd_value) ? \
                              UDS_EVT_REGIST_USER_INDICATION_ENABLE : \
                              UDS_EVT_REGIST_USER_INDICATION_DISABLE);
            s_uds_env.regist_user_ind_cfg[conn_idx] = cccd_value;
            break;

        case UDS_IDX_CTRL_POINT_VAL:
            if (PRF_CLI_START_IND != s_uds_env.ctrl_point_ind_cfg[conn_idx]) {
                cfm.status = UDS_ERROR_CCCD_INVALID;
                break;
            } else if (s_uds_env.ucp_in_progress) {
                cfm.status = UDS_ERROR_PROC_IN_PROGRESS;
            } else if (PRF_CLI_START_IND == s_uds_env.ctrl_point_ind_cfg[conn_idx]) {
                ucp_evt = true;
                s_uds_env.ucp_in_progress = true;
            }
            break;

        case UDS_IDX_CTRL_POINT_IND_CFG:
            cccd_value     = le16toh(&p_param->value[0]);
            event.evt_type = ((PRF_CLI_START_IND == cccd_value) ? \
                              UDS_EVT_CTRL_POINT_INDICATION_ENABLE : \
                              UDS_EVT_CTRL_POINT_INDICATION_DISABLE);
            s_uds_env.ctrl_point_ind_cfg[conn_idx] = cccd_value;
            break;

        default:
            cfm.status = BLE_ATT_ERR_INVALID_HANDLE;
            break;
    }

    ble_gatts_write_cfm(conn_idx, &cfm);

    if (ucp_evt) {
        uds_receive_ucp_handler(conn_idx, p_param->value, p_param->length);
    }
    if (BLE_ATT_ERR_INVALID_HANDLE != cfm.status && UDS_EVT_INVALID != event.evt_type &&
        s_uds_env.uds_init.evt_handler) {
        s_uds_env.uds_init.evt_handler(&event);
    }
}

/**
 *****************************************************************************************
 * @brief Handles Prepare write value callback function.
 *
 * @param[in]: conn_idx:      Connection index
 * @param[in]: p_prep_req:    Pointer to the handle of cccd attribute.
 *****************************************************************************************
 */
static void uds_gatts_prep_write_cb(uint8_t conn_idx, const gatts_prep_write_req_cb_t *p_prep_req)
{
    gatts_prep_write_cfm_t cfm;

    cfm.handle = p_prep_req->handle;
    cfm.length = UDS_REGI_USER_VAL_LEN_MAX;
    cfm.status = BLE_SUCCESS;

    ble_gatts_prepare_write_cfm(conn_idx, &cfm);
}

/**
 *****************************************************************************************
 * @brief Handles reception of the cccd recover request.
 *
 * @param[in]: conn_idx:   Connection index
 * @param[in]: handle:     The handle of cccd attribute.
 * @param[in]: cccd_value: The value of cccd attribute.
 *****************************************************************************************
 */
static void uds_cccd_set_cb(uint8_t conn_idx, uint16_t handle, uint16_t cccd_value)
{
    uint16_t          tab_index   = 0;
    uds_evt_t         event;

    if (!prf_is_cccd_value_valid(cccd_value)) {
        return;
    }

    tab_index  = prf_find_idx_by_handle(handle,
                                        s_uds_env.start_hdl,
                                        UDS_IDX_NB,
                                        (uint8_t *)&s_uds_env.uds_init.char_mask);

    event.evt_type = UDS_EVT_INVALID;
    event.conn_idx = conn_idx;

    switch (tab_index) {
        case UDS_IDX_DB_CHANGE_INCR_NTF_CFG:
            event.evt_type = ((PRF_CLI_START_NTF == cccd_value) ? \
                              UDS_EVT_DB_CHANGE_INCR_NOTIFICATION_ENABLE : \
                              UDS_EVT_DB_CHANGE_INCR_NOTIFICATION_DISABLE);
            s_uds_env.db_change_incr_ntf_cfg[conn_idx] = cccd_value;
            break;

        case UDS_IDX_CTRL_POINT_IND_CFG:
            event.evt_type = ((PRF_CLI_START_IND == cccd_value) ? \
                              UDS_EVT_CTRL_POINT_INDICATION_ENABLE : \
                              UDS_EVT_CTRL_POINT_INDICATION_DISABLE);
            s_uds_env.ctrl_point_ind_cfg[conn_idx] = cccd_value;
            break;

        case UDS_IDX_REGIST_USER_IND_CFG:
            event.evt_type = ((PRF_CLI_START_IND == cccd_value) ? \
                              UDS_EVT_REGIST_USER_INDICATION_ENABLE : \
                              UDS_EVT_REGIST_USER_INDICATION_DISABLE);
            s_uds_env.regist_user_ind_cfg[conn_idx] = cccd_value;
            break;

        default:
            break;
    }

    if (UDS_EVT_INVALID != event.evt_type && s_uds_env.uds_init.evt_handler) {
        s_uds_env.uds_init.evt_handler(&event);
    }
}

/**
 *****************************************************************************************
 * @brief Handles reception of the complete event.
 *
 * @param[in] conn_idx:   Connection index.
 * @param[in] status:     Complete event status.
 * @param[in] p_ntf_ind:  Pointer to the parameters of the complete event.
 *****************************************************************************************
 */
static void uds_gatts_ntf_ind_cb(uint8_t conn_idx, uint8_t status, const ble_gatts_ntf_ind_t *p_ntf_ind)
{
    uds_evt_t event;

    event.evt_type = UDS_EVT_INVALID;
    event.conn_idx = conn_idx;

    uint8_t tab_index;
    tab_index = prf_find_idx_by_handle(p_ntf_ind->handle,
                                       s_uds_env.start_hdl,
                                       UDS_IDX_NB,
                                       (uint8_t *)&s_uds_env.uds_init.char_mask);

    if (BLE_GAP_ERR_TIMEOUT == status) {
        ble_gap_disconnect(conn_idx);
    }

    if (s_uds_env.uds_init.evt_handler && SDK_SUCCESS == status) {
        if (BLE_GATT_NOTIFICATION == p_ntf_ind->type) {
            event.evt_type = UDS_EVT_DB_CHANGE_INCR_SEND_CPLT;
            s_uds_env.uds_init.evt_handler(&event);
        } else if (BLE_GATT_INDICATION == p_ntf_ind->type && (UDS_IDX_REGIST_USER_VAL == tab_index)) {
            uds_indicate_user_char_val_chunk(conn_idx);
        } else if (BLE_GATT_INDICATION == p_ntf_ind->type && (UDS_IDX_CTRL_POINT_VAL == tab_index)) {
            event.evt_type = UDS_EVT_CTRL_POINT_RSP_CPLT;
            s_uds_env.ucp_in_progress = false;
            s_uds_env.uds_init.evt_handler(&event);
        }
    }
}

/**
 *****************************************************************************************
 * @brief Change the endian of Registered User Name.
 *
 * @param[in] user_index:  index.
 * @param[in] p_param:     Pointer to the parameters of the complete event.
 *****************************************************************************************
 */

/**
 *****************************************************************************************
 * @brief Handle Age Characteristic write event.
 *
 * @param[in]  p_data: Pointer to the age characteristic value.
 * @param[in]  length: The length of data.
 * @param[in]  p_cfm:  Pointer of the write confirmation.
 *****************************************************************************************
 */
static void uds_age_write_handler(const uint8_t *p_data, uint16_t length, gatts_write_cfm_t *p_cfm)
{
    if (length != 1 || p_data[0] > 100) {
        p_cfm->status = BLE_GATT_ERR_WRITE;
    } else {
        uint8_t age        = p_data[0];
        uint8_t user_index = s_uds_env.uds_init.user_index;

        wss_db_record_age_set(user_index, age);
    }
}

/**
 *****************************************************************************************
 * @brief Handle Date of Birth Characteristic write event.
 *
 * @param[in]  p_data: Pointer to Date of Birth Characteristic value.
 * @param[in]  length: The length of data.
 * @param[in]  p_cfm:  Pointer of the write confirmation.
 *****************************************************************************************
 */
static void uds_date_of_birth_write_handler(const uint8_t *p_data, uint16_t length, gatts_write_cfm_t *p_cfm)
{
    if (length != 4) {
        p_cfm->status = BLE_GATT_ERR_WRITE;
    } else {
        birth_date_t  date_of_birth;
        date_of_birth.year  = BUILD_U16(p_data[0], p_data[1]);
        date_of_birth.month = p_data[INDEX_2];
        date_of_birth.day   = p_data[INDEX_3];

        if (date_of_birth.month > MONTH_12 || (date_of_birth.day > DAY_31) ||\
                ((date_of_birth.year < YEAR_LOW || date_of_birth.year > YEAR_HIGHT) && (date_of_birth.year != 0))) {
            p_cfm->status = BLE_GATT_ERR_WRITE;
        } else {
            uint8_t user_index = s_uds_env.uds_init.user_index;
            wss_db_record_date_of_birth_set(user_index, &date_of_birth);
        }
    }
}

/**
 *****************************************************************************************
 * @brief Handle First Name Characteristic write event.
 *
 * @param[in]  p_data:  Pointer to First Name Characteristic value.
 * @param[in]  length:  The length of data.
 * @param[in]  p_cfm:   Pointer of the write confirmation.
 *****************************************************************************************
 */
static void uds_first_name_write_handler(const uint8_t *p_data, uint16_t length, gatts_write_cfm_t *p_cfm)
{
    uint8_t  user_index = s_uds_env.uds_init.user_index;
    wss_db_record_first_name_set(user_index, p_data, length);
}

/**
 *****************************************************************************************
 * @brief Handle Height Characteristic write event.
 *
 * @param[in]  p_data: Pointer to Height Characteristic value.
 * @param[in]  length: The length of data.
 * @param[in]  p_cfm:  Pointer of the write confirmation.
 *****************************************************************************************
 */
static void uds_height_write_handler(const uint8_t *p_data, uint16_t length, gatts_write_cfm_t *p_cfm)
{
    if (length != INDEX_2) {
        p_cfm->status = BLE_GATT_ERR_WRITE;
    } else {
        uint16_t height = BUILD_U16(p_data[0], p_data[1]);
        if (height < HEIGHT_40 || height > HEIGHT_250) {
            p_cfm->status = BLE_GATT_ERR_WRITE;
        } else {
            uint8_t  user_index = s_uds_env.uds_init.user_index;
            wss_db_record_height_set(user_index, height);
        }
    }
}

/**
 *****************************************************************************************
 * @brief Handle Gender Characteristic write event.
 *
 * @param[in]  p_data: Pointer to Gender Characteristic value.
 * @param[in]  length: The length of data.
 * @param[in]  p_cfm:  Pointer of the write confirmation.
 *****************************************************************************************
 */
static void uds_gender_write_handler(const uint8_t *p_data, uint16_t length, gatts_write_cfm_t *p_cfm)
{
    if (length != 1 || (p_data[0] > INDEX_2)) {
        p_cfm->status = BLE_GATT_ERR_WRITE;
    } else {
        uint8_t gender     = p_data[0];
        uint8_t user_index = s_uds_env.uds_init.user_index;

        wss_db_record_gender_set(user_index, gender);
    }
}

/**
 *****************************************************************************************
 * @brief Handle Database Change Increment value write event.
 *
 * @param[in]  p_data: Pointer to Database Change Increment Characteristic value.
 * @param[in]  length: Length of Database Change Increment Characteristic value.
 * @param[in]  p_cfm:  Pointer of the write confirmation.
 *****************************************************************************************
 */
static void uds_db_change_incr_write_handler(const uint8_t *p_data, uint16_t length, gatts_write_cfm_t *p_cfm)
{
    if (length != UDS_DB_CHANGE_INCR_VAL_LEN_MAX) {
        p_cfm->status = BLE_GATT_ERR_WRITE;
    } else {
        uint8_t user_index = s_uds_env.uds_init.user_index;

        uint32_t db_change_incr_val = BUILD_U32(p_data[0], p_data[1], p_data[INDEX_2], p_data[INDEX_3]);
        bool status = wss_db_record_db_change_incr_val_set(user_index, db_change_incr_val);
    }
}

/**
 *****************************************************************************************
 * @brief User Control Point Register New User handler.
 *
 * @param[in] conn_idx: Connection index.
 * @param[in] p_data:   Pointer to data.
 * @param[in] length:   Length of data.
 *****************************************************************************************
 */
static void uds_op_regist_new_user_handler(uint8_t conn_idx, const uint8_t *p_data, uint16_t length)
{
    uds_evt_t   event;
    uint8_t     rsp[UDS_CTRL_PT_RSP_LEN_MAX];

    rsp[0] = UDS_CTRL_PT_OP_RSP_CODE;
    rsp[1] = UDS_CTRL_PT_OP_REGIST_NEW_USER;
    rsp[INDEX_2] = UDS_CTRL_PT_RSP_INVALID_PARAM;

    uint16_t consent_code = BUILD_U16(p_data[0], p_data[1]);
    if ((sizeof(uint16_t) != length) || (UDS_CONSENT_CODE_VAL_MAX < consent_code)) {
        uds_ctrl_pt_rsp_send(conn_idx, rsp, UDS_CTRL_PT_RSP_LEN_MIN);
    } else {
        uds_chars_val_t uds_chars_val;
        uint8_t         user_nums;
        uint8_t         db_change_incr_val;
        uint8_t         new_user_index;

        user_nums = (uint8_t)wss_db_records_num_get();
        if (WSS_DB_RECORDS_MAX <= user_nums) {
            wss_db_record_delete(1);
            new_user_index = 0x01;
        } else {
            new_user_index = user_nums;
        }

        uint8_t first_name[]              = "tom";
        db_change_incr_val                = UDS_DB_CHANGE_INCR_DEFAULT_VAL;
        uds_chars_val.age                 = AGE_18;
        uds_chars_val.height              = HEIGHT_165;  // The unit is cm.
        uds_chars_val.gender              = 0x00;
        uds_chars_val.date_of_birth.year  = YEAR_2000;
        uds_chars_val.date_of_birth.month = MONTH_01;
        uds_chars_val.date_of_birth.day   = DAY_01;
        uds_chars_val.p_first_name        = &first_name[0];
        uds_chars_val.name_length         = sizeof(first_name);
        rsp[INDEX_3]                            = new_user_index;

        if (wss_db_record_add(new_user_index, consent_code, db_change_incr_val, &uds_chars_val)
                && s_uds_env.uds_init.evt_handler) {
            rsp[INDEX_2] = UDS_CTRL_PT_RSP_SUCCESS;

            event.conn_idx = conn_idx;
            event.evt_type = UDS_EVT_REGIST_NEW_USER;
            event.p_data   = &new_user_index;
            event.length   = length;
            s_uds_env.uds_init.evt_handler(&event);
            s_uds_env.uds_init.user_index = new_user_index;

            uds_ctrl_pt_rsp_send(conn_idx, rsp, UDS_CTRL_PT_RSP_LEN_MIN + 1);
        } else {
            rsp[INDEX_2] = UDS_CTRL_PT_RSP_FAILED;
            uds_ctrl_pt_rsp_send(conn_idx, rsp, UDS_CTRL_PT_RSP_LEN_MIN);
        }
    }
}

/**
 *****************************************************************************************
 * @brief User Control Point Consent handler.
 *
 * @param[in] conn_idx: Connection index.
 * @param[in] p_data:   Pointer to data.
 * @param[in] length:   Length of data.
 *****************************************************************************************
 */
static void uds_op_consent_handler(uint8_t conn_idx, const uint8_t *p_data, uint16_t length)
{
    uds_evt_t  event;
    uint8_t    rsp[UDS_CTRL_PT_RSP_LEN_MIN];

    rsp[0] = UDS_CTRL_PT_OP_RSP_CODE;
    rsp[1] = UDS_CTRL_PT_OP_CONSENT;
    rsp[INDEX_2] = UDS_CTRL_PT_RSP_INVALID_PARAM;

    uint16_t consent_code = BUILD_U16(p_data[1], p_data[INDEX_2]);

    if (UDS_CONSENT_TRY_NUM_MAX <= s_uds_env.consent_try_num) {
#if defined(PTS_AUTO_TEST)
        wss_db_record_clear();
#endif
        rsp[INDEX_2] = UDS_CTRL_PT_RSP_FAILED;
    } else if (((sizeof(uint16_t)+sizeof(uint8_t)) == length) && \
               (UDS_UNKNOWN_USER > p_data[0]) && \
               (UDS_CONSENT_CODE_VAL_MAX >= consent_code)) {
        uint8_t   user_index   = p_data[0];
        wss_rec_t loc_rec;

        bool status = wss_db_record_get(user_index, &loc_rec);
        if (consent_code != loc_rec.consent_code || (!status)) {
            rsp[INDEX_2] = UDS_CTRL_PT_RSP_USER_NOT_AUTH;
            s_uds_env.consent_try_num++;

#if defined(PTS_AUTO_TEST)
            wss_db_record_delete(s_uds_env.uds_init.user_index);
#endif
        } else {
            rsp[INDEX_2] = UDS_CTRL_PT_RSP_SUCCESS;
            event.conn_idx            = conn_idx;
            event.evt_type            = UDS_EVT_USER_GRANT_ACCESS;
            event.p_data              = p_data;
            event.length              = length;

            s_uds_env.uds_init.evt_handler(&event);
            s_uds_env.uds_init.user_index = user_index;
            s_uds_env.consent_try_num     = 0;
            s_uds_env.uds_init.evt_handler(&event);
        }
    }
    uds_ctrl_pt_rsp_send(conn_idx, rsp, UDS_CTRL_PT_RSP_LEN_MIN);
}

/**
 *****************************************************************************************
 * @brief User Control Point Delete User Data handler.
 *
 * @param[in] conn_idx: Connection index.
 * @param[in] p_data:   Pointer to data.
 * @param[in] length:   Length of data.
 *****************************************************************************************
 */
static void uds_op_del_user_data_handler(uint8_t conn_idx, const uint8_t *p_data, uint16_t length)
{
    uds_evt_t  event;
    uint8_t    rsp[UDS_CTRL_PT_RSP_LEN_MAX];

    rsp[0] = UDS_CTRL_PT_OP_RSP_CODE;
    rsp[1] = UDS_CTRL_PT_OP_DEL_USER_DATA;
    rsp[INDEX_2] = UDS_CTRL_PT_RSP_INVALID_PARAM;

    if (UDS_UNKNOWN_USER == s_uds_env.uds_init.user_index) {
        rsp[INDEX_2] = UDS_CTRL_PT_RSP_USER_NOT_AUTH;
    } else {
        if (wss_db_record_delete(s_uds_env.uds_init.user_index)) {
            rsp[INDEX_2] = UDS_CTRL_PT_RSP_SUCCESS;

            event.conn_idx           = conn_idx;
            event.evt_type           = UDS_EVT_DEL_USER_DATA;
            event.p_data             = p_data;
            event.length             = length;
            s_uds_env.uds_init.evt_handler(&event);
            s_uds_env.uds_init.user_index = UDS_UNKNOWN_USER;
        } else {
            rsp[INDEX_2] = UDS_CTRL_PT_RSP_FAILED;
        }
    }
    uds_ctrl_pt_rsp_send(conn_idx, rsp, UDS_CTRL_PT_RSP_LEN_MIN);
}

/**
 *****************************************************************************************
 * @brief User Control Point List All Users handler.
 *
 * @param[in] conn_idx: Connection index.
 * @param[in] p_data:   Pointer to data.
 * @param[in] length:   Length of data.
 *****************************************************************************************
 */
static void uds_op_list_all_users_handler(uint8_t conn_idx, const uint8_t *p_data, uint16_t length)
{
    uds_evt_t  event;
    uint8_t    rsp[UDS_CTRL_PT_RSP_LEN_MAX];

    rsp[0] = UDS_CTRL_PT_OP_RSP_CODE;
    rsp[1] = UDS_CTRL_PT_OP_LIST_ALL_USERS;
    rsp[INDEX_2] = UDS_CTRL_PT_RSP_SUCCESS;

    uint16_t user_num = wss_db_records_num_get();
    if (UDS_UNKNOWN_USER > user_num && 0 < user_num) {
        event.conn_idx = conn_idx;
        event.evt_type = UDS_EVT_CTRL_POINT_SET_BY_PEER;
        event.p_data   = p_data;
        event.length   = length;
        s_uds_env.uds_init.evt_handler(&event);

        s_uds_env.regist_user_ind_cfg[conn_idx] = UDS_EVT_CTRL_POINT_INDICATION_ENABLE;

        uds_regist_user_value_encode(0);  // start indicate the value of the registered user char.
        uds_indicate_user_char_val_chunk(conn_idx);
    } else if (user_num == 0) {
        rsp[INDEX_3] = user_num;
        uds_ctrl_pt_rsp_send(conn_idx, rsp, UDS_CTRL_PT_RSP_LEN_MIN + 1);
    }
}

/**
 *****************************************************************************************
 * @brief User Control Point Delete User(s) handler.
 *
 * @param[in] conn_idx: Connection index.
 * @param[in] p_data:   Pointer to data.
 * @param[in] length:   Length of data.
 *****************************************************************************************
 */
static void uds_op_del_users_handler(uint8_t conn_idx, const uint8_t *p_data, uint16_t length)
{
    uds_evt_t  event;
    uint8_t    rsp[UDS_CTRL_PT_RSP_LEN_MIN+1];

    rsp[0] = UDS_CTRL_PT_OP_RSP_CODE;
    rsp[1] = UDS_CTRL_PT_OP_DEL_USERS;
    rsp[INDEX_2] = UDS_CTRL_PT_RSP_INVALID_PARAM;

    if (sizeof(uint8_t) != length || UDS_UNKNOWN_USER < p_data[0] || (s_uds_env.uds_init.user_index < p_data[0]
            && p_data[0] < UDS_UNKNOWN_USER)) {
        s_uds_env.uds_init.uds_regi_user_data_flag.regi_user_name_present = false;
        s_uds_env.uds_init.uds_regi_user_data_flag.user_name_truncated    = false;
        uds_ctrl_pt_rsp_send(conn_idx, rsp, UDS_CTRL_PT_RSP_LEN_MIN);
    } else {
        rsp[INDEX_2] = UDS_CTRL_PT_RSP_SUCCESS;

        uint8_t user_index = p_data[0];
        if (UDS_UNKNOWN_USER == user_index) {
            wss_db_record_clear();
            rsp[INDEX_3] = UDS_UNKNOWN_USER;
            s_uds_env.uds_init.user_index = UDS_UNKNOWN_USER;
        } else {
            if (!wss_db_record_delete(user_index)) {
                rsp[INDEX_2] = UDS_CTRL_PT_RSP_FAILED;
            } else {
                rsp[INDEX_3] = user_index;
            }
        }

        event.conn_idx = conn_idx;
        event.evt_type = UDS_EVT_DEL_USERS;
        event.p_data   = p_data;
        event.length   = length;
        if (s_uds_env.uds_init.user_index == p_data[0]) {
            s_uds_env.uds_init.user_index = UDS_UNKNOWN_USER;
        }
        s_uds_env.uds_init.evt_handler(&event);
        uds_ctrl_pt_rsp_send(conn_idx, rsp, UDS_CTRL_PT_RSP_LEN_MIN+1);
    }
}

/**
 *****************************************************************************************
 * @brief User Control Point receive handler.
 *
 * @param[in] conn_idx: Connection index.
 * @param[in] p_data:   Pointer to data.
 * @param[in] length:   Length of data.
 *****************************************************************************************
 */
static void uds_receive_ucp_handler(uint8_t conn_idx, const uint8_t *p_data, uint16_t length)
{
    uint8_t rsp[UDS_CTRL_PT_RSP_LEN_MAX];

    switch (p_data[0]) {
        case UDS_CTRL_PT_OP_REGIST_NEW_USER:
            uds_op_regist_new_user_handler(conn_idx, &p_data[1], length - 1);
            break;

        case UDS_CTRL_PT_OP_CONSENT:
            uds_op_consent_handler(conn_idx, &p_data[1], length - 1);
            break;

        case UDS_CTRL_PT_OP_DEL_USER_DATA:
            uds_op_del_user_data_handler(conn_idx, &p_data[1], length - 1);
            break;

        case UDS_CTRL_PT_OP_LIST_ALL_USERS:
            uds_op_list_all_users_handler(conn_idx, &p_data[1], length - 1);
            break;

        case UDS_CTRL_PT_OP_DEL_USERS:
            uds_op_del_users_handler(conn_idx, &p_data[1], length - 1);
            break;

        default:
            rsp[0] = UDS_CTRL_PT_OP_RSP_CODE;
            rsp[1] = p_data[0];
            rsp[INDEX_2] = UDS_CTRL_PT_RSP_NOT_SUP;
            uds_ctrl_pt_rsp_send(conn_idx, rsp, UDS_CTRL_PT_RSP_LEN_MIN);
            break;
    }
}

/**
 *****************************************************************************************
 * @brief Indicate the Registered User Characteristic value.
 *
 * @param[in] conn_idx:  Connection index.
 *****************************************************************************************
 */
static sdk_err_t uds_indicate_user_char_val_chunk(uint8_t conn_idx)
{
    uint8_t          chunk_len;
    gatts_noti_ind_t uds_ind;
    sdk_err_t        error_code;

    chunk_len = s_regi_user_char_val_stream.p_segm_length[s_regi_user_char_val_stream.segm_num -
                                                     s_regi_user_char_val_stream.segm_offset];

    if (s_regi_user_char_val_stream.segm_offset == 0) {
        s_regi_user_char_val_stream.p_data        = NULL;
        s_regi_user_char_val_stream.offset        = 0;
        s_regi_user_char_val_stream.length        = 0;
        s_regi_user_char_val_stream.p_segm_length = NULL;
        s_regi_user_char_val_stream.segm_num      = 0;
        s_regi_user_char_val_stream.segm_offset   = 0;

        uds_evt_t event;
        event.evt_type = UDS_EVT_REGIST_USER_RSP_CPLT;
        s_uds_env.uds_init.evt_handler(&event);

        // After indicating all users, then indicate the number of users.
        uint8_t   rsp[UDS_CTRL_PT_RSP_LEN_MIN+1];
        uint16_t  user_num = wss_db_records_num_get();

        rsp[0] = UDS_CTRL_PT_OP_RSP_CODE;
        rsp[1] = UDS_CTRL_PT_OP_LIST_ALL_USERS;
        rsp[INDEX_2] = UDS_CTRL_PT_RSP_SUCCESS;
        rsp[INDEX_3] = user_num;
        uds_ctrl_pt_rsp_send(conn_idx, rsp, UDS_CTRL_PT_RSP_LEN_MIN+1);

        return SDK_SUCCESS;
    }

    uds_ind.type   = BLE_GATT_INDICATION;
    uds_ind.handle = prf_find_handle_by_idx(UDS_IDX_REGIST_USER_VAL,
                                            s_uds_env.start_hdl,
                                            (uint8_t *)&s_uds_env.uds_init.char_mask);
    uds_ind.length = chunk_len;
    uds_ind.value  = (uint8_t *)s_regi_user_char_val_stream.p_data + s_regi_user_char_val_stream.offset;

    error_code     = ble_gatts_noti_ind(conn_idx, &uds_ind);
    if (SDK_SUCCESS == error_code) {
        s_regi_user_char_val_stream.offset += chunk_len;
        s_regi_user_char_val_stream.segm_offset--;
    }

    return error_code;
}

/**
 *****************************************************************************************
 * @brief Encode the Registered User Characteristic value.
 *
 * @param[in] conn_idx:  Connection index.
 *****************************************************************************************
 */
static void uds_regist_user_value_encode(uint8_t conn_idx)
{
    uint8_t ret;
    ret = memset_s(&s_regi_user_char_val_stream, sizeof(s_regi_user_char_val_stream),
                   0, sizeof(s_regi_user_char_val_stream));
    if (ret < 0) {
        return;
    }

    s_uds_env.segm_head_roll_num = 0;

    static uint8_t   local_buf[UDS_REGI_USER_VAL_LEN_MAX * INDEX_5 * INDEX_3] = {0};
    static uint16_t  local_segm_length[10] = {0};
    uint16_t  length = 0;
    uint16_t  chunk_len = 0;
    uint16_t  segm_total_num = 0;
    uint8_t   regi_users_num = wss_db_records_num_get();
    uint8_t   ret;

    for (uint8_t i = 0; i < regi_users_num; i++) {
        wss_rec_t loc_rec;
        uint16_t  segm_num_per_user = 0;
        uint16_t  name_offset_per_user = 0;
        if (wss_db_record_get(i, &loc_rec)) {
            continue;
        }

        do {
            chunk_len = loc_rec.name_length - name_offset_per_user;

            if (UDS_REGI_USER_DATA_LEN_MAX - USER_DATA_OFFSET >= chunk_len && segm_num_per_user == 0) {
                uint8_t segm_header = 0;

                if (UDS_UNKNOWN_USER == s_uds_env.uds_init.user_index) {
                    s_uds_env.uds_init.uds_regi_user_data_flag.regi_user_name_present = false;
                }

                uint8_t regi_user_data_flags = 0;
                uint8_t regi_user_index;

                segm_header |= (s_uds_env.segm_head_roll_num << USER_DATA_OFFSET);
                segm_header |= UDS_ONLY_REGI_USER_SEGM;  // the only packet.

                if (s_uds_env.uds_init.uds_regi_user_data_flag.regi_user_name_present) {
                    regi_user_data_flags |= UDS_REGI_USER_NAME_PRESENT;
                }

                if (s_uds_env.uds_init.uds_regi_user_data_flag.user_name_truncated) {
                    regi_user_data_flags |= UDS_USER_NAME_TRUNCATED;
                }

                regi_user_index = i;

                local_buf[length++] = segm_header;
                local_buf[length++] = regi_user_data_flags;
                local_buf[length++] = regi_user_index;

                ret = memcpy_s(&local_buf[length], loc_rec.name_length,
                               loc_rec.first_name, loc_rec.name_length);
                if (ret < 0) {
                    return;
                }
                length += loc_rec.name_length;

                local_segm_length[segm_total_num++] = loc_rec.name_length + NAME_LEN_OFFSET;
                s_regi_user_char_val_stream.length += (loc_rec.name_length + NAME_LEN_OFFSET);
                name_offset_per_user += loc_rec.name_length;
            } else if (UDS_REGI_USER_DATA_LEN_MAX >= chunk_len && segm_num_per_user != 0) {
                uint8_t segm_header = 0;
                segm_header |= (s_uds_env.segm_head_roll_num << USER_DATA_OFFSET);
                segm_header |= UDS_LAST_REGI_USER_SEGM;  // the last packet.
                local_buf[length++] = segm_header;

                ret = memcpy_s(&local_buf[length], chunk_len, loc_rec.first_name + name_offset_per_user, chunk_len);
                if (ret < 0) {
                    return;
                }
                length += chunk_len;

                local_segm_length[segm_total_num++] = chunk_len + 1;
                s_regi_user_char_val_stream.length += (chunk_len + 1);
                name_offset_per_user += chunk_len;
            } else if (UDS_REGI_USER_DATA_LEN_MAX < chunk_len) {
                if (segm_num_per_user == 0) {
                    uint8_t segm_header = 0;
                    if (UDS_UNKNOWN_USER == s_uds_env.uds_init.user_index) {
                        s_uds_env.uds_init.uds_regi_user_data_flag.regi_user_name_present = false;
                    }
                    uint8_t regi_user_data_flags = 0;
                    uint8_t regi_user_index;

                    segm_header |= (s_uds_env.segm_head_roll_num << USER_DATA_OFFSET);
                    segm_header |= UDS_FIRST_REGI_USER_SEGM;  // the first packet.
                    if (s_uds_env.uds_init.uds_regi_user_data_flag.regi_user_name_present) {
                        regi_user_data_flags |= UDS_REGI_USER_NAME_PRESENT;
                    }

                    if (s_uds_env.uds_init.uds_regi_user_data_flag.user_name_truncated) {
                        regi_user_data_flags |= UDS_USER_NAME_TRUNCATED;
                    }

                    regi_user_index = i;

                    local_buf[length++] = segm_header;
                    local_buf[length++] = regi_user_data_flags;
                    local_buf[length++] = regi_user_index;

                    ret = memcpy_s(&local_buf[length], UDS_REGI_USER_DATA_LEN_MAX - USER_DATA_OFFSET,
                                   loc_rec.first_name, UDS_REGI_USER_DATA_LEN_MAX - USER_DATA_OFFSET);
                    if (ret < 0) {
                        return;
                    }
                    length += UDS_REGI_USER_DATA_LEN_MAX - USER_DATA_OFFSET;

                    local_segm_length[segm_total_num++] = UDS_REGI_USER_VAL_LEN_MAX;
                    s_regi_user_char_val_stream.length += UDS_REGI_USER_VAL_LEN_MAX;
                    name_offset_per_user += UDS_REGI_USER_DATA_LEN_MAX - USER_DATA_OFFSET;
                } else {
                    uint8_t segm_header = 0;
                    segm_header |= (s_uds_env.segm_head_roll_num << USER_DATA_OFFSET);
                    segm_header |= UDS_MIDDLE_REGI_USER_SEGM;  // the middle packet.

                    local_buf[length++] = segm_header;
                    ret = memcpy_s(&local_buf[length], UDS_REGI_USER_DATA_LEN_MAX - USER_DATA_OFFSET,
                                   loc_rec.first_name  + name_offset_per_user,
                                   UDS_REGI_USER_DATA_LEN_MAX - USER_DATA_OFFSET);
                    if (ret < 0) {
                        return;
                    }
                    length += UDS_REGI_USER_DATA_LEN_MAX - USER_DATA_OFFSET;

                    local_segm_length[segm_total_num++] = UDS_REGI_USER_VAL_LEN_MAX;
                    s_regi_user_char_val_stream.length += UDS_REGI_USER_VAL_LEN_MAX;
                    name_offset_per_user += UDS_REGI_USER_DATA_LEN_MAX - USER_DATA_OFFSET;
                }
            }
            segm_num_per_user++;
            if (s_uds_env.segm_head_roll_num == ROLL_NUM) {
                s_uds_env.segm_head_roll_num = 0;
            } else {
                s_uds_env.segm_head_roll_num++;
            }
        } while (name_offset_per_user != loc_rec.name_length);
    }

    s_regi_user_char_val_stream.segm_offset   = segm_total_num;
    s_regi_user_char_val_stream.segm_num      = segm_total_num;
    s_regi_user_char_val_stream.p_segm_length = local_segm_length;
    s_regi_user_char_val_stream.p_data        = local_buf;
    s_regi_user_char_val_stream.offset        = 0;
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
void uds_set_cur_user_index(uint8_t conn_idx, uint8_t user_index)
{
    s_uds_env.uds_init.user_index = user_index;
}

uint8_t uds_get_cur_user_index(uint8_t conn_idx)
{
    return s_uds_env.uds_init.user_index;
}

sdk_err_t uds_regi_user_val_send(uint8_t conn_idx)
{
    uds_regist_user_value_encode(0);
    return uds_indicate_user_char_val_chunk(conn_idx);
}

sdk_err_t uds_db_change_incr_val_send(uint8_t conn_idx, uint8_t user_index)
{
    sdk_err_t        error_code = SDK_ERR_NTF_DISABLED;
    uint16_t         length;
    gatts_noti_ind_t uds_ntf;
    wss_rec_t        loc_rec;

    if (UDS_UNKNOWN_USER != user_index) {
        if (wss_db_record_get(user_index, &loc_rec)) {
            length = sizeof(loc_rec.db_change_incr_val);
            if (PRF_CLI_START_NTF == s_uds_env.db_change_incr_ntf_cfg[conn_idx]) {
                uds_ntf.type   = BLE_GATT_NOTIFICATION;
                uds_ntf.handle = prf_find_handle_by_idx(UDS_IDX_DB_CHANGE_INCR_VAL,
                                                        s_uds_env.start_hdl,
                                                        (uint8_t *)&s_uds_env.uds_init.char_mask);
                uds_ntf.length = length;
                uds_ntf.value  = (uint8_t *)&loc_rec.db_change_incr_val;
                error_code     = ble_gatts_noti_ind(conn_idx, &uds_ntf);
            }
        }
    }
    return error_code;
}

sdk_err_t uds_ctrl_pt_rsp_send(uint8_t conn_idx, uint8_t *p_data, uint16_t length)
{
    sdk_err_t        error_code = SDK_ERR_IND_DISABLED;
    gatts_noti_ind_t ctrl_pt_rsp;

    if (PRF_CLI_START_IND == s_uds_env.ctrl_point_ind_cfg[conn_idx]) {
        ctrl_pt_rsp.type   = BLE_GATT_INDICATION;
        ctrl_pt_rsp.handle = prf_find_handle_by_idx(UDS_IDX_CTRL_POINT_VAL,
            s_uds_env.start_hdl,
            (uint8_t *)&s_uds_env.uds_init.char_mask);
        ctrl_pt_rsp.length = length;
        ctrl_pt_rsp.value  = p_data;
        error_code         = ble_gatts_noti_ind(conn_idx, &ctrl_pt_rsp);
    }

    return error_code;
}

sdk_err_t uds_service_init(uds_init_t *p_uds_init)
{
    sdk_err_t ret;
    if (p_uds_init == NULL) {
        return SDK_ERR_POINTER_NULL;
    }

    ret = memset_s(&s_uds_env, sizeof(s_uds_env), 0, sizeof(s_uds_env));
    if (ret < 0) {
        return ret;
    }
    ret = memcpy_s(&s_uds_env.uds_init, sizeof(uds_init_t), p_uds_init, sizeof(uds_init_t));
    if (ret < 0) {
        return ret;
    }

    return ble_server_prf_add(&uds_prf_info);
}

#if defined(PTS_AUTO_TEST)
void uds_regist_new_user(uint8_t conn_idx)
{
    uint16_t consent_code = 0x0001;

    uds_chars_val_t uds_chars_val;
    uint8_t         user_index;
    uint8_t         db_change_incr_val;

    user_index                        = (uint8_t)wss_db_records_num_get();
    s_uds_env.uds_init.user_index     = user_index;
    uint8_t first_name[]              = "tom";
    db_change_incr_val                = UDS_DB_CHANGE_INCR_DEFAULT_VAL;
    uds_chars_val.age                 = AGE_18;
    uds_chars_val.height              = HEIGHT_1650;
    uds_chars_val.gender              = 0x00;
    uds_chars_val.date_of_birth.year  = YEAR_2000;
    uds_chars_val.date_of_birth.month = MONTH_01;
    uds_chars_val.date_of_birth.day   = DAY_01;
    uds_chars_val.p_first_name        = &first_name[0];
    uds_chars_val.name_length         = sizeof(first_name);

    wss_db_record_add(user_index, consent_code, db_change_incr_val, &uds_chars_val);
}

void uds_del_users(uint8_t conn_idx)
{
    uint8_t user_index = 0x00;
    wss_db_record_delete(user_index);
}
#endif
