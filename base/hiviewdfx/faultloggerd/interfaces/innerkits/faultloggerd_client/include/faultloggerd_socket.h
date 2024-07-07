/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

/* This files contains process dump write log to file module. */

#ifndef DFX_FAULTLOGGERD_SOCKET_H
#define DFX_FAULTLOGGERD_SOCKET_H

#include <cinttypes>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif
bool StartConnect(int& sockfd, const char* path, const int timeout);
bool StartListen(int& sockfd, const char* name, const int listenCnt);

bool RecvMsgCredFromSocket(int sockfd, struct ucred* pucred);

bool SendMsgIovToSocket(int sockfd, void *iovBase, const int iovLen);

bool SendFileDescriptorToSocket(int sockfd, int fd);
int ReadFileDescriptorFromSocket(int sockfd);
#ifdef __cplusplus
}
#endif
#endif
