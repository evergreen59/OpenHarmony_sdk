/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "contexts_trie.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "selinux_error.h"
#include "selinux_share_mem.h"

static const char DEFAULT_CONTEXT[] = "u:object_r:default_param:s0";
static const char EMPTY_STRING[] = "";
static const size_t CONTEXTS_LENGTH_MIN = 16; // sizeof("x u:object_r:x:s0")
static const size_t CONTEXTS_LENGTH_MAX = 1024;
static const uint32_t SELINUX_PARAM_SPACE = 1024 * 80;
static const uint32_t MAX_LEN = 255;

static ParamHashNode *GetGroupNode(ParamContextsTrie *root, const char *name, uint32_t len)
{
    HashNode *node = HashMapGet(root->handle, name, len);
    if (node == NULL) {
        return NULL;
    }
    return HASHMAP_ENTRY(node, ParamHashNode, hashNode);
}

static ParamHashNode *AddGroupNode(ParamContextsTrie *root, const char *name, ParamContextsTrie *child)
{
    uint32_t nameLen = (uint32_t)strlen(name);
    ParamHashNode *groupNode = GetGroupNode(root, name, nameLen);
    if (groupNode != NULL) {
        return groupNode;
    }

    groupNode = (ParamHashNode *)calloc(1, sizeof(ParamHashNode));
    if (groupNode == NULL) {
        return NULL;
    }
    groupNode->nameLen = nameLen;
    groupNode->name = (char *)calloc(1, nameLen + 1);
    if (groupNode->name == NULL) {
        free(groupNode);
        return NULL;
    }
    memcpy(groupNode->name, name, nameLen + 1);
    groupNode->childPtr = child;

    HashMapAdd(root->handle, &groupNode->hashNode);
    return groupNode;
}

static void ReleaseParamContextsTrieNode(ParamContextsTrie **node)
{
    if (*node == NULL) {
        return;
    }
    if ((*node)->handle != NULL) {
        HashMapDestroy((*node)->handle);
    }
    free(*node);
    *node = NULL;
}

static bool InsertElementToTrie(ParamContextsTrie *root, const char *element, ParamContextsTrie **child)
{
    uint32_t nameLen = (uint32_t)strlen(element);
    ParamHashNode *childNode = GetGroupNode(root, element, nameLen);
    if (childNode != NULL) {
        *child = childNode->childPtr;
        return true;
    }
    ParamContextsTrie *childPtr = (ParamContextsTrie *)calloc(1, sizeof(ParamContextsTrie));
    if (childPtr == NULL) {
        return false;
    }
    childPtr->prefixLabel = EMPTY_STRING;
    childPtr->matchLabel = EMPTY_STRING;
    childPtr->labeled = UNLABELED;
    if (HashMapCreate(&childPtr->handle) != 0) {
        ReleaseParamContextsTrieNode(&childPtr);
        return false;
    }
    if (AddGroupNode(root, element, childPtr) == NULL) {
        ReleaseParamContextsTrieNode(&childPtr);
        return false;
    }
    *child = childPtr;
    return true;
}

static bool InsertParamToTrie(ParamContextsTrie *root, const char *paramPrefix, const char *contexts)
{
    if (root == NULL || paramPrefix == NULL || contexts == NULL) {
        return false;
    }
    char *tmpPrefix = strdup(paramPrefix);
    if (tmpPrefix == NULL) {
        return false;
    }
    char *rest = NULL;
    char *element = strtok_r(tmpPrefix, ".", &rest);
    while (element != NULL) {
        ParamContextsTrie *child = NULL;
        if (!InsertElementToTrie(root, element, &child)) {
            free(tmpPrefix);
            return false;
        }
        root = child;
        element = strtok_r(NULL, ".", &rest);
    }
    if (paramPrefix[strlen(paramPrefix) - 1] == '.') {
        root->prefixLabel = contexts;
        root->labeled = PREFIX_LABELED;
    } else {
        root->matchLabel = contexts;
        root->labeled = MATCH_LABELED;
    }

    free(tmpPrefix);
    return true;
}

const char *SearchFromParamTrie(ParamContextsTrie *root, const char *paraName)
{
    const char *updateCurLabel = EMPTY_STRING;
    const char *tmpName = paraName;
    ParamHashNode *childNode = NULL;

    const char *bar = strchr(tmpName, '.');
    while (bar != NULL) {
        childNode = GetGroupNode(root, tmpName, bar - tmpName);
        if (childNode == NULL) {
            goto nomatch;
        }
        if (root->labeled == PREFIX_LABELED) {
            updateCurLabel = root->prefixLabel;
        }

        root = childNode->childPtr;
        tmpName = bar + 1;
        bar = strchr(tmpName, '.');
    }

    childNode = GetGroupNode(root, tmpName, strlen(tmpName));
    if (childNode != NULL) {
        ParamContextsTrie *match = childNode->childPtr;
        if (match->labeled == MATCH_LABELED) {
            return match->matchLabel;
        }
    }

nomatch:
    if (root->labeled == PREFIX_LABELED) {
        return root->prefixLabel;
    } else if (strcmp(updateCurLabel, EMPTY_STRING) != 0) {
        return updateCurLabel;
    } else {
        return DEFAULT_CONTEXT;
    }
}

static bool CouldSkip(const char *line)
{
    size_t len = strlen(line);
    if (len < CONTEXTS_LENGTH_MIN || len > CONTEXTS_LENGTH_MAX) {
        return true;
    }
    int i = 0;
    while (isspace(line[i])) {
        i++;
    }
    if (line[i] == '#') {
        return true;
    }
    return false;
}

static bool InsertContextsList(ParamContextsList **head, const char *param, const char *context)
{
    if (head == NULL || param == NULL || context == NULL) {
        return false;
    }
    ParamContextsList *node = (ParamContextsList *)calloc(1, sizeof(ParamContextsList));
    if (node == NULL) {
        return false;
    }

    node->info.paraName = param;
    node->info.paraContext = context;
    node->next = NULL;
    (*head)->next = node;
    *head = (*head)->next;
    return true;
}

bool ReadParamFromSharedMem(ParamContextsTrie **trieRoot, ParamContextsList **listHead)
{
    SharedMem *memPtr = (SharedMem *)InitSharedMem("/dev/__parameters__/param_selinux", SELINUX_PARAM_SPACE, true);
    if (memPtr == NULL) {
        return false;
    }
    SharedMem *memHead = memPtr;
    ParamContextsTrie *root = (ParamContextsTrie *)calloc(1, sizeof(ParamContextsTrie));
    if (root == NULL) {
        UnmapSharedMem((char *)memHead, SELINUX_PARAM_SPACE);
        return false;
    }
    root->prefixLabel = EMPTY_STRING;
    root->matchLabel = EMPTY_STRING;
    if (HashMapCreate(&root->handle) != 0) {
        ReleaseParamContextsTrieNode(&root);
        UnmapSharedMem((char *)memHead, SELINUX_PARAM_SPACE);
        return false;
    }
    ParamContextsList *listPtr = (ParamContextsList *)calloc(1, sizeof(ParamContextsList));
    if (listPtr == NULL) {
        HashMapDestroy(root->handle);
        ReleaseParamContextsTrieNode(&root);
        UnmapSharedMem((char *)memHead, SELINUX_PARAM_SPACE);
        return false;
    }
    uint32_t currentPos = 0;
    ParamContextsList *tmpHead = listPtr;
    while (memPtr != NULL && memPtr->paramNameSize > 0) {
        char *paramName = ReadSharedMem(memPtr->data, memPtr->paramNameSize);
        char *context = ReadSharedMem(memPtr->data + memPtr->paramNameSize + 1, memPtr->paramLabelSize);
        if (!InsertParamToTrie(root, paramName, context) || !InsertContextsList(&listPtr, paramName, context)) {
            continue;
        }
        uint32_t dataLen = memPtr->paramNameSize + memPtr->paramLabelSize + 2; // 2 bytes for '\0'
        uint32_t readSize = dataLen + sizeof(SharedMem);                       // space used for read SharedMem struct
        currentPos += readSize;
        if (currentPos > SELINUX_PARAM_SPACE) { // no space to read
            break;
        }
        memPtr = (SharedMem *)((char *)memPtr + readSize);
    }
    listPtr = tmpHead->next;
    free(tmpHead);
    *listHead = listPtr;
    *trieRoot = root;
    return true;
}

static int WriteParamToSharedMem(char *paramName, char *context, uint32_t *currentPos, SharedMem **memPtr)
{
    uint32_t paramLen = strlen(paramName);
    uint32_t contextLen = strlen(context);
    if (paramLen > MAX_LEN || contextLen > MAX_LEN) { // too long, ignore
        return 0;
    }
    uint32_t dataLen = paramLen + contextLen + 2;        // 2 bytes for write '\0'
    uint32_t writeSize = dataLen + sizeof(SharedMem);    // space used for write SharedMem struct
    if (*currentPos + writeSize > SELINUX_PARAM_SPACE) { // no space to write
        return -1;
    }
    *currentPos += writeSize;

    SharedMem *tmPtr = *memPtr;
    tmPtr->paramNameSize = paramLen;
    tmPtr->paramLabelSize = contextLen;
    char *writePtr = tmPtr->data;
    WriteSharedMem(writePtr, paramName, paramLen);
    writePtr[paramLen] = '\0';
    writePtr = tmPtr->data + paramLen + 1;
    WriteSharedMem(writePtr, context, contextLen);
    writePtr[contextLen] = '\0';
    *memPtr = (SharedMem *)((char *)tmPtr + writeSize); // get the next SharedMem ptr
    return 0;
}

int LoadParameterContextsToSharedMem(void)
{
    char buffer[512] = {0};
    FILE *fp = fopen("/system/etc/selinux/targeted/contexts/parameter_contexts", "r");
    if (fp == NULL) {
        return -SELINUX_CONTEXTS_FILE_LOAD_ERROR;
    }
    SharedMem *memPtr = (SharedMem *)InitSharedMem("/dev/__parameters__/param_selinux", SELINUX_PARAM_SPACE, false);
    if (memPtr == NULL) {
        (void)fclose(fp);
        return -SELINUX_PTR_NULL;
    }
    SharedMem *head = memPtr;
    uint32_t currentPos = 0;
    while (fgets(buffer, sizeof(buffer) - 1, fp) != NULL) {
        size_t n = strlen(buffer);
        if (buffer[n - 1] == '\n') {
            buffer[n - 1] = '\0';
        }
        if (CouldSkip(buffer)) {
            continue;
        }
        char *rest = NULL;
        char split[] = " \t";
        char *paramName = strtok_r(buffer, split, &rest);
        if (paramName == NULL) {
            continue;
        }
        char *context = strtok_r(NULL, split, &rest);
        if (context == NULL) {
            continue;
        }
        if (WriteParamToSharedMem(paramName, context, &currentPos, &memPtr) != 0) {
            break;
        }
    }
    UnmapSharedMem((char *)head, SELINUX_PARAM_SPACE);
    (void)fclose(fp);
    return 0;
}
