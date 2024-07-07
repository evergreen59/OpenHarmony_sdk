/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef FOUNDATION_ACE_THIRD_PARTY_QUICKJS_MESSAGE_QUEUE_H
#define FOUNDATION_ACE_THIRD_PARTY_QUICKJS_MESSAGE_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

const char* QueueFront(int client);
void QueuePop(int client);
int QueueIsEmpty(int client);
void *DBG_StartAgent(void *args);
bool DBG_CopyComponentNameFromAce(const char *srcStr, char *destStr, int maxLen);
void DBG_SetComponentNameAndInstanceId(const char *name, int size, int instanceId);
void DBG_SetNeedDebugBreakPoint(bool needDebugBreakPoint);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif