/**
 *****************************************************************************************
 *
 * @file app_log_store.c
 *
 * @brief App Log tore Implementation.
 *
 *****************************************************************************************
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
 *****************************************************************************************
 */
#if APP_LOG_STORE_ENABLE
#include "utility.h"

/*
 * DEFINE
 *****************************************************************************************
 */
#define APP_LOG_STORE_MAGIC              0x47444442   /**< Magic for app log store: "GDDB". */
#define APP_LOG_STORE_TIME_SIZE          26           /**< [00000000.000] */
#define APP_LOG_STORE_TIME_DEFAULT       "[1970/01/01 00:00:00:000] "
#define APP_LOG_STORE_CACHE_SIZE         ((APP_LOG_STORE_LINE_SIZE) * (APP_LOG_STORE_CACHE_NUM))
#define APP_LOG_STORE_ONECE_OP_SIZE      1024
#define APP_LOG_STORE_BUSY_BIT           (0x01 << 0)
#define APP_LOG_STORE_SAVE_BIT           (0x01 << 1)
#define APP_LOG_STORE_DUMP_BIT           (0x01 << 2)

#define OFFSET_0                         0
#define OFFSET_1                         1
#define OFFSET_2                         2


/*
 * STRUCTURES
 *****************************************************************************************
 */
/**@brief App log store head info. */
typedef struct {
    uint32_t magic;         /**< Magic for app log store. */
    uint32_t db_addr;       /**< Start address of app log db flash. */
    uint32_t db_size;       /**< Size of app log db flash. */
    uint32_t offset;        /**< Offset of end log data. */
    uint16_t flip_over;     /**< Is flip over. */
    uint16_t check_sum;     /**< Check sum for head info. */
} log_store_head_t;

/**@brief App log store environment variable. */
struct log_store_env_t {
    bool              initialized;
    uint8_t           store_status;
    log_store_head_t  store_head;
    uint16_t          head_nv_tag;
    uint16_t          blk_size;
};

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
static struct log_store_env_t  s_log_store_env;
static app_log_store_op_t      s_log_store_ops;
static app_log_store_dump_cb_t s_log_store_dump_cb;
static uint32_t                s_log_store_dump_offset;
static ring_buffer_t           s_log_store_rbuf;
static uint8_t                 s_log_store_cache[APP_LOG_STORE_CACHE_SIZE];

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
static uint32_t log_store_check_sum_calc(uint8_t *p_data, uint32_t len)
{
    uint32_t   check_sum = 0;

    if (p_data && len) {
        for (uint8_t i = 0; i < len; i++) {
            check_sum += p_data[i];
        }
    }

    return check_sum;
}

static bool log_store_head_check(log_store_head_t *p_head, uint32_t db_addr, uint32_t db_size)
{
    uint16_t  head_len  = sizeof(log_store_head_t);
    uint8_t  *head_data = (uint8_t *)p_head;

    if (p_head->magic != APP_LOG_STORE_MAGIC ||
            p_head->db_addr != db_addr           ||
            p_head->db_size != db_size           ||
            p_head->offset >  db_size) {
        return false;
    }

    if (p_head->check_sum != log_store_check_sum_calc(head_data, head_len - OFFSET_2)) {
        return false;
    }

    return true;
}

static bool log_store_head_update(uint16_t nv_tag, log_store_head_t *p_head)
{
    uint16_t  head_len  = sizeof(log_store_head_t);
    uint8_t  *head_data = (uint8_t *)p_head;

    p_head->check_sum = log_store_check_sum_calc(head_data, head_len - OFFSET_2);

    if (nvds_put(nv_tag, head_len, (uint8_t *)p_head)) {
        return false;
    }

    return true;
}

static bool log_store_time_stamp_encode(uint8_t *p_buffer, uint8_t buffer_size)
{
    if (buffer_size != APP_LOG_STORE_TIME_SIZE) {
        return false;
    }

    app_log_store_time_t rtc_time = {0};

    s_log_store_ops.time_get(&rtc_time);

    if (APP_LOG_STORE_TIME_SIZE == snprintf_s((char *)p_buffer, APP_LOG_STORE_TIME_SIZE, \
        APP_LOG_STORE_TIME_SIZE, \
        "[%04d/%02d/%02d %02d:%02d:%02d:%03d] ", \
        rtc_time.year, rtc_time.month, rtc_time.day, \
        rtc_time.hour, rtc_time.min, \
        rtc_time.sec, rtc_time.msec)) {
        return true;
    }

    return false;
}

static void log_store_data_flash_write(void)
{
    uint32_t align_num = 0;
    uint32_t read_len;
    uint8_t  read_buff[APP_LOG_STORE_ONECE_OP_SIZE];

    if ((s_log_store_env.store_head.offset % s_log_store_env.blk_size) == 0) {
        if (s_log_store_ops.flash_erase) {
            s_log_store_ops.flash_erase(s_log_store_env.store_head.db_addr + s_log_store_env.store_head.offset,
                                        s_log_store_env.blk_size);
        }
    }

    align_num = ALIGN_NUM(APP_LOG_STORE_ONECE_OP_SIZE, s_log_store_env.store_head.offset);
    if (align_num != s_log_store_env.store_head.offset) {
        read_len = ring_buffer_read(&s_log_store_rbuf, read_buff, align_num - s_log_store_env.store_head.offset);
    } else {
        read_len = ring_buffer_read(&s_log_store_rbuf, read_buff, APP_LOG_STORE_ONECE_OP_SIZE);
    }

    if (s_log_store_ops.flash_write && read_len) {
        s_log_store_ops.flash_write(s_log_store_env.store_head.db_addr + s_log_store_env.store_head.offset, read_buff,
                                    read_len);
        s_log_store_env.store_head.offset += read_len;
    }

    if (s_log_store_env.store_head.offset >= s_log_store_env.store_head.db_size) {
        s_log_store_env.store_head.offset    = 0;
        s_log_store_env.store_head.flip_over = 1;
    }

    log_store_head_update(s_log_store_env.head_nv_tag, &s_log_store_env.store_head);
}

static void log_store_to_flash(void)
{
#if (APP_LOG_STORE_RUN_ON_OS == 0)
    if (s_log_store_env.store_status & APP_LOG_STORE_BUSY_BIT) {
        return;
    }

    s_log_store_env.store_status |= APP_LOG_STORE_BUSY_BIT;
#endif

    log_store_data_flash_write();

    s_log_store_env.store_status &= ~APP_LOG_STORE_SAVE_BIT;
#if (APP_LOG_STORE_RUN_ON_OS == 0)
    s_log_store_env.store_status &= ~APP_LOG_STORE_BUSY_BIT;
#endif
}

static void log_dump_from_flash(void)
{
    uint8_t  dump_buffer[APP_LOG_STORE_ONECE_OP_SIZE];
    uint16_t dump_len;

    if (s_log_store_env.store_status & APP_LOG_STORE_BUSY_BIT) {
        return;
    }

    s_log_store_env.store_status |= APP_LOG_STORE_BUSY_BIT;

    if (s_log_store_ops.flash_read) {
        uint32_t align_num = ALIGN_NUM(APP_LOG_STORE_ONECE_OP_SIZE, s_log_store_env.store_head.offset);
        if (align_num != s_log_store_env.store_head.offset &&
                (s_log_store_dump_offset + APP_LOG_STORE_ONECE_OP_SIZE) == align_num) {
            dump_len = (s_log_store_env.store_head.offset + APP_LOG_STORE_ONECE_OP_SIZE - align_num);
        } else {
            dump_len = APP_LOG_STORE_ONECE_OP_SIZE;
        }

        s_log_store_ops.flash_read(s_log_store_dump_offset + s_log_store_env.store_head.db_addr, dump_buffer, dump_len);

        if (s_log_store_dump_cb) {
            s_log_store_dump_cb(dump_buffer, dump_len);
        }

        s_log_store_dump_offset += dump_len;
    } else {
        s_log_store_env.store_status &= ~APP_LOG_STORE_DUMP_BIT;
    }

    if (s_log_store_env.store_head.db_size == s_log_store_dump_offset) {
        s_log_store_dump_offset = 0;
    }

    if (s_log_store_env.store_head.offset == s_log_store_dump_offset) {
        s_log_store_dump_offset = 0;
        s_log_store_env.store_status &= ~APP_LOG_STORE_DUMP_BIT;
    }

    s_log_store_env.store_status &= ~APP_LOG_STORE_BUSY_BIT;
}


/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
uint16_t app_log_store_init(app_log_store_info_t *p_info, app_log_store_op_t *p_op_func)
{
    uint16_t head_len = sizeof(log_store_head_t);

    if (s_log_store_env.initialized) {
        return SDK_ERR_DISALLOWED;
    }

    if (p_info == NULL ||
        p_op_func == NULL ||
        p_op_func->flash_init == NULL ||
        p_op_func->flash_read == NULL ||
        p_op_func->flash_write == NULL ||
        p_op_func->flash_erase == NULL ||
        p_info->db_size == 0 ||
        p_info->blk_size == 0 ||
        (p_info->db_addr % p_info->blk_size) != 0) {
        return SDK_ERR_INVALID_PARAM;
    }

    nvds_get(p_info->nv_tag, &head_len, (uint8_t *)&s_log_store_env.store_head);

    if (!log_store_head_check(&s_log_store_env.store_head, p_info->db_addr, p_info->db_size)) {
        s_log_store_env.store_head.magic     = APP_LOG_STORE_MAGIC;
        s_log_store_env.store_head.db_addr   = p_info->db_addr;
        s_log_store_env.store_head.db_size   = p_info->db_size;
        s_log_store_env.store_head.offset    = 0;
        s_log_store_env.store_head.flip_over = 0;

        if (!log_store_head_update(p_info->nv_tag, &s_log_store_env.store_head)) {
            return SDK_ERR_SDK_INTERNAL;
        }
    }

    ring_buffer_init(&s_log_store_rbuf, s_log_store_cache, APP_LOG_STORE_CACHE_SIZE);

    memcpy_s(&s_log_store_ops, sizeof (s_log_store_ops), p_op_func, sizeof(s_log_store_ops));

    s_log_store_env.head_nv_tag = p_info->nv_tag;
    s_log_store_env.blk_size    = p_info->blk_size;
    s_log_store_env.initialized = true;

    p_op_func->flash_init();

    return SDK_SUCCESS;
}


uint16_t app_log_store_save(const uint8_t *p_data, const uint16_t length)
{
    uint8_t  time_encode[APP_LOG_STORE_TIME_SIZE] = APP_LOG_STORE_TIME_DEFAULT;

    if (!s_log_store_env.initialized) {
        return SDK_ERR_DISALLOWED;
    }

    if (s_log_store_env.store_status & APP_LOG_STORE_BUSY_BIT) {
        return SDK_ERR_BUSY;
    }

    s_log_store_env.store_status |= APP_LOG_STORE_BUSY_BIT;

    if (s_log_store_ops.time_get) {
        log_store_time_stamp_encode(time_encode, APP_LOG_STORE_TIME_SIZE);
        time_encode[APP_LOG_STORE_TIME_SIZE - 1] = ' ';
    }

    ring_buffer_write(&s_log_store_rbuf, time_encode, APP_LOG_STORE_TIME_SIZE);
    ring_buffer_write(&s_log_store_rbuf, p_data, length);

    if (ring_buffer_items_count_get(&s_log_store_rbuf >= APP_LOG_STORE_ONECE_OP_SIZE)) {
        s_log_store_env.store_status |= APP_LOG_STORE_SAVE_BIT;
#if APP_LOG_STORE_RUN_ON_OS
        log_store_to_flash();
#endif
    }

    s_log_store_env.store_status &= ~APP_LOG_STORE_BUSY_BIT;

    return SDK_SUCCESS;
}


void app_log_store_flush(void)
{
    uint32_t items_count = 0;

    if (!s_log_store_env.initialized) {
        return;
    }

    do {
        items_count = ring_buffer_items_count_get(&s_log_store_rbuf);
        if (items_count) {
            log_store_data_flash_write();
        }
    } while (items_count >= APP_LOG_STORE_ONECE_OP_SIZE);
}

uint16_t app_log_store_dump(app_log_store_dump_cb_t dump_cb)
{
    if (!s_log_store_env.initialized) {
        return SDK_ERR_DISALLOWED;
    }

    if (s_log_store_env.store_status & APP_LOG_STORE_DUMP_BIT) {
        return SDK_ERR_BUSY;
    }

    if (dump_cb == NULL) {
        return SDK_ERR_POINTER_NULL;
    }

    s_log_store_dump_cb = dump_cb;

    app_log_store_flush();

    if (s_log_store_env.store_head.flip_over == 0 && s_log_store_env.store_head.offset == 0) {
        return SDK_SUCCESS;
    }

    if (s_log_store_env.store_head.flip_over) {
        uint32_t align_num;

        align_num = ALIGN_NUM(s_log_store_env.blk_size, s_log_store_env.store_head.offset);

        s_log_store_dump_offset = align_num >= s_log_store_env.store_head.db_size ? 0 : align_num;
    } else {
        s_log_store_dump_offset = 0;
    }

    s_log_store_env.store_status |= APP_LOG_STORE_DUMP_BIT;

    return SDK_SUCCESS;
}

void app_log_store_clear(void)
{
    s_log_store_env.store_head.offset    = 0;
    s_log_store_env.store_head.flip_over = 0;

    log_store_head_update(s_log_store_env.head_nv_tag, &s_log_store_env.store_head);
}

bool app_log_store_dump_ongoing(void)
{
    return (s_log_store_env.store_status & APP_LOG_STORE_DUMP_BIT) ? true : false;
}

void app_log_store_schedule(void)
{
#if APP_LOG_STORE_RUN_ON_OS == 0
    if (s_log_store_env.store_status & APP_LOG_STORE_SAVE_BIT) {
        log_store_to_flash();
    }
#endif

    if (s_log_store_env.store_status & APP_LOG_STORE_DUMP_BIT) {
        log_dump_from_flash();
    }
}
#endif

