/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "hal_file.h"
#include "duet_flash_kv.h"
#include "kv_err.h"
#include "kv_conf.h"
#include "kv_store.h"
#include "ohos_errno.h"
#include "utils_file.h"
/* not support file system yet, use kv to get/set */
/* just for param xts */

#define MAX_OPEN_FILES      32
#define MAX_NAME_SIZE       32

struct file {
    int32_t f_pos;
    int32_t f_flag;
    int8_t *f_path;
};
struct file * ftable[MAX_OPEN_FILES];

int HalFileOpen(const char *path, int oflag, int mode)
{
    uint8_t buf[32];
    uint32_t len = 32;
    int32_t ret;
    char dummy[] = "";
    int i;

    for(i = 0; i < MAX_OPEN_FILES; i++) {
        if (ftable[i] == NULL) {
            break;
        }
    }
    if (i >= MAX_OPEN_FILES)
        return -1;

    if (strlen(path) > MAX_NAME_SIZE)
        return -1;

    ret = duet_flash_kv_get(path, (void *)buf, (int32_t *)&len);

    if (ret == KV_ERR_NOT_FOUND) {
        if (oflag & O_CREAT_FS) {
            /* creat kv pair with 1 dummy byte */
           int32_t result = duet_flash_kv_set(path, dummy, 1, 1);
           if (result != KV_OK) {
               printf("Warning:kv write fail. %d\r\n", result);
               return -1;
           }
           /* try read out */
           result = duet_flash_kv_get(path, (void*)buf, (int32_t*)&len);
           if (result != KV_OK) {
               printf("Warning:kv read fail. %d\r\n", result);
               return -1;
           }
        } else {
            return -1;
        }
    }

    if (ret == KV_OK && (oflag & O_EXCL_FS))
        return -1;

    struct file * pf = malloc(sizeof(struct file));
    if (pf == NULL) {
        return -1;
    }
    pf->f_flag = oflag;
    pf->f_pos = 0;
    pf->f_path = path;

    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (ftable[i] == NULL) {
            ftable[i] = pf;
            return i + 1;
        }
    }

    return -1;
}

int HalFileClose(int fd)
{
    int i = fd -1;

    if (i < 0 || i >= MAX_OPEN_FILES || ftable[i] == NULL)
        return -1;

    free(ftable[i]);
    ftable[i] = NULL;

    return 0;
}

int HalFileRead(int fd, char *buf, unsigned int len)
{
    int i = fd - 1;

    if (i < 0 || i >= MAX_OPEN_FILES || ftable[i] == NULL)
        return -1;

    struct file *pf = ftable[i];

    int32_t retval = 0;
    int templen = len + 1;

    unsigned int filesize;
    HalFileStat(pf->f_path, &filesize);

    /* not support seek read, code for acts.*/
    if (pf->f_pos >= filesize || pf->f_pos >= len) {
        return 0;
    }

    retval = duet_flash_kv_get(pf->f_path, (void *)buf, (int32_t *)&templen);
    pf->f_pos += templen;

    if (retval == KV_ERR_INVALID_PARAM || len <= 1) {
        printf("KV_ERR_INVALID_PARAM \n");
        return EC_INVALID;
    }
    if (retval == KV_OK) {
        return (templen - 1);
    }

    return EC_FAILURE;
}

int HalFileWrite(int fd, const char *buf, unsigned int len)
{
    int32_t retval = 0;
    int32_t buflen = strlen(buf) + 1;

    int i = fd -1;

    if (i < 0 || i >= MAX_OPEN_FILES || ftable[i] == NULL)
        return -1;

    struct file *pf = ftable[i];

    if (pf->f_pos != 0) {
    }
    /* not support seek write, for acts.*/
    retval = duet_flash_kv_set(pf->f_path, (void *)buf, buflen, 1);
    if (retval == KV_ERR_INVALID_PARAM) {
        printf("KV_ERR_INVALID_PARAM \n");
        return EC_INVALID;
    }
    if (retval == KV_OK) {
        return buflen - 1;
    }
    if (retval == KV_ERR_NO_SPACE) {
        printf("KV_ERR_NO_SPACE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    }
    //       return EC_NOSPACE;
    printf("kv error %d\n", retval);
    return EC_FAILURE;
}

int HalFileDelete(const char *path)
{
    int32_t retval = 0;

    retval = duet_flash_kv_del(path);
    if (retval == KV_ERR_INVALID_PARAM) {
        printf("KV_ERR_INVALID_PARAM \n");
        return EC_INVALID;
    }
    if (retval == KV_OK) {
        return 0;
    }
    // printf("kv error %d\n",retval);
    return EC_FAILURE;
}

int HalFileStat(const char *path, unsigned int *fileSize)
{
    int len = 150;
    char temp[150] = {0};

    int32_t retval = 0;

    retval = duet_flash_kv_get(path, (void *)temp, (int32_t *)&len);
    if (retval == KV_ERR_INVALID_PARAM || len <= 1) {
        *fileSize = 0;
    }

    if (retval == KV_OK) {
        *fileSize = (len - 1);
        return 0;
    }

    return -1;
}

int HalFileSeek(int fd, int offset, unsigned int whence)
{
    unsigned int filesize;

    int i = fd -1;

    if (i < 0 || i >= MAX_OPEN_FILES || ftable[i] == NULL)
        return -1;

    struct file *pf = ftable[i];

    HalFileStat(pf->f_path, &filesize);

    switch (whence)
    {
    case SEEK_SET_FS:
        if (offset == 0) {
            pf->f_pos = 0;
        } else if (offset < 0 || offset >= filesize) {
            return -1;
        }
        pf->f_pos = offset;
        break;

    case SEEK_END_FS:
        if (offset > 0 || offset + (int)filesize < 0) {
            return -1;
        }
        pf->f_pos = offset + (int)filesize;
        break;

    case SEEK_CUR_FS:
        if ((pf->f_pos + offset) < 0 || (pf->f_pos + offset) > filesize) {
            return -1;
        }
        pf->f_pos = pf->f_pos + offset;
        break;

    default:
        return -1;
    }

    return 0;
}
