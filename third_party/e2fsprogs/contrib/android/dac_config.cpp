/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#define __cpluscplus
#include "dac_config.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <linux/capability.h>

using namespace std;

namespace {
struct DacConfig {
    unsigned int uid;
    unsigned int gid;
    unsigned int mode;
    uint64_t capabilities;
    string path;

    DacConfig() : uid(0), gid(0), mode(0), capabilities(0), path("") {}
    DacConfig(unsigned int m, unsigned int u, unsigned int g, uint64_t c, const string &p) :
        uid(u),
        gid(g),
        mode(m),
        capabilities(c),
        path(p) {}

    void SetDefault(unsigned int m, unsigned int u, unsigned int g, uint64_t c, const string &p)
    {
        this->uid = u;
        this->gid = g;
        this->mode = m;
        this->capabilities = c;
        this->path = p;
    }
};

unordered_map<string, DacConfig> g_configMap;

static string Trim(const string& s)
{
    if (s.size() == 0) {
        return s;
    }

    size_t start = 0;
    size_t end = s.size() - 1;

    while (start < s.size() && isspace(s[start])) {
        start++;
    }

    while (end >= start && isspace(s[end])) {
        end--;
    }

    if (end < start) {
        return "";
    }

    return s.substr(start, end - start + 1);
}

unordered_map<string, unsigned int> g_capStrCapNum = {
    { "CAP_CHOWN", CAP_CHOWN },
    { "CAP_DAC_OVERRIDE", CAP_DAC_OVERRIDE },
    { "CAP_DAC_READ_SEARCH", CAP_DAC_READ_SEARCH },
    { "CAP_FOWNER", CAP_FOWNER },
    { "CAP_FSETID", CAP_FSETID },
    { "CAP_KILL", CAP_KILL },
    { "CAP_SETGID", CAP_SETGID },
    { "CAP_SETUID", CAP_SETUID },
    { "CAP_LINUX_IMMUTABLE", CAP_LINUX_IMMUTABLE },
    { "CAP_NET_BIND_SERVICE", CAP_NET_BIND_SERVICE },
    { "CAP_NET_BROADCAST", CAP_NET_BROADCAST },
    { "CAP_NET_ADMIN", CAP_NET_ADMIN },
    { "CAP_NET_RAW", CAP_NET_RAW },
    { "CAP_IPC_LOCK", CAP_IPC_LOCK },
    { "CAP_IPC_OWNER", CAP_IPC_OWNER },
    { "CAP_SYS_MODULE", CAP_SYS_MODULE },
    { "CAP_SYS_RAWIO", CAP_SYS_RAWIO },
    { "CAP_SYS_CHROOT", CAP_SYS_CHROOT },
    { "CAP_SYS_PTRACE", CAP_SYS_PTRACE },
    { "CAP_SYS_PACCT", CAP_SYS_PACCT },
    { "CAP_SYS_ADMIN", CAP_SYS_ADMIN },
    { "CAP_SYS_ROOT", CAP_SYS_BOOT },
    { "CAP_SYS_NICE", CAP_SYS_NICE },
    { "CAP_SYS_RESOURCE", CAP_SYS_RESOURCE },
    { "CAP_SYS_TIME", CAP_SYS_TIME },
    { "CAP_SYS_TTY_CONFIG", CAP_SYS_TTY_CONFIG },
    { "CAP_MKNOD", CAP_MKNOD },
    { "CAP_LEASE", CAP_LEASE },
    { "CAP_AUDIT_WRITE", CAP_AUDIT_WRITE },
    { "CAP_AUDIT_CONTROL", CAP_AUDIT_CONTROL },
    { "CAP_SETFCAP", CAP_SETFCAP },
    { "CAP_MAC_OVERRIDE", CAP_MAC_OVERRIDE },
    { "CAP_MAC_ADMIN", CAP_MAC_ADMIN },
    { "CAP_SYSLOG", CAP_SYSLOG },
    { "CAP_WAKE_ALARM", CAP_WAKE_ALARM },
    { "CAP_BLOCK_SUSPEND", CAP_BLOCK_SUSPEND },
};

static uint64_t GetCap(string cap)
{
    if (isdigit(cap[0])) {
        return stoll(cap);
    }

    stringstream ss(cap);
    string value;
    uint64_t c = 0;
    while (getline(ss, value, '|')) {
        value = Trim(value);
        if (g_capStrCapNum.count(value)) {
            c |= (1ULL << g_capStrCapNum[value]);
        }
    }

    return c;
}

enum {
    DAC_PATH_IDX = 0,
    DAC_MODE_IDX,
    DAC_UID_IDX,
    DAC_GID_IDX,
    DAC_CAP_IDX,
    DAC_NUM
};

extern "C" {
    int LoadDacConfig(const char* fn)
    {
        ifstream readFile(fn);
        if (readFile.fail()) {
            return -1;
        }

        string str;
        vector<string> values(DAC_NUM, ""); // path, mode, uid, gid, cap
        while (getline(readFile, str)) {
            str = Trim(str);
            if (str.empty() || str[0] == '#') {
                continue;
            }

            stringstream ss(str);
            string value;
            int i = 0;
            while (getline(ss, value, ',')) {
                if (i >= DAC_NUM) {
                    break;
                }

                value = Trim(value);
                if (value.empty()) {
                    continue;
                }
                values[i++] = value;
            }

            if (i != DAC_NUM) {
                continue;
            }

            int uid = 0;
            if (isdigit(values[DAC_UID_IDX][0])) {
                uid = stoi(values[DAC_UID_IDX]);
            }

            int gid = 0;
            if (isdigit(values[DAC_GID_IDX][0])) {
                uid = stoi(values[DAC_GID_IDX]);
            }

            uint64_t cap = GetCap(values[DAC_CAP_IDX]);
            DacConfig dacConfig(stoi(values[DAC_MODE_IDX], 0, 8), uid, gid, cap, values[DAC_PATH_IDX]); // 8 oct
            g_configMap[dacConfig.path] = dacConfig;
        }

        return 0;
    }

    void GetDacConfig(const char* path, int dir, char*,
            unsigned* uid, unsigned* gid, unsigned* mode,
            uint64_t* capabilities)
    {
        string str = (path != nullptr && *path == '/') ? path + 1 : path;
        DacConfig dacConfig(00755, 0, 0, 0, "");

        if (dir == 0) {
            dacConfig.SetDefault(00644, 0, 0, 0, "");
        }

        auto it = g_configMap.find(str);
        if (it != g_configMap.end()) {
            dacConfig = it->second;
        } else if (dir == 0 && !str.empty()) {
            for (int i = static_cast<int>(str.size()) - 1; i >= 0; i--) {
                if (str[i] == '/') {
                    break;
                } else {
                    it = g_configMap.find(str.substr(0, i) + "*");
                    if (it != g_configMap.end()) {
                        dacConfig = it->second;
                        break;
                    }
                }
            }
        }

        *uid = dacConfig.uid;
        *gid = dacConfig.gid;
        *mode = dacConfig.mode;
        *capabilities = dacConfig.capabilities;
    }
}
}
