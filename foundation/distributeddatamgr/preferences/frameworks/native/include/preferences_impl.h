/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef PREFERENCES_IMPL_H
#define PREFERENCES_IMPL_H

#include <any>
#include <condition_variable>
#include <filesystem>
#include <list>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <vector>

#include "preferences.h"
#include "preferences_observer.h"
#include "preferences_value.h"

namespace OHOS {
namespace NativePreferences {
class PreferencesImpl : public Preferences, public std::enable_shared_from_this<PreferencesImpl> {
public:
    explicit PreferencesImpl(const std::string &path);
    virtual ~PreferencesImpl();

    int Init();

    PreferencesValue Get(const std::string &key, const PreferencesValue &defValue) override;

    int Put(const std::string &key, const PreferencesValue &value) override;

    int GetInt(const std::string &key, const int &defValue) override
    {
        PreferencesValue preferencesValue = Get(key, defValue);
        if (!preferencesValue.IsInt()) {
            return defValue;
        }
        return preferencesValue;
    }

    std::string GetString(const std::string &key, const std::string &defValue) override
    {
        PreferencesValue preferencesValue = Get(key, defValue);
        if (!preferencesValue.IsString()) {
            return defValue;
        }
        return preferencesValue;
    }

    bool GetBool(const std::string &key, const bool &defValue) override
    {
        PreferencesValue preferencesValue = Get(key, defValue);
        if (!preferencesValue.IsBool()) {
            return defValue;
        }
        return preferencesValue;
    }

    float GetFloat(const std::string &key, const float &defValue) override
    {
        PreferencesValue preferencesValue = Get(key, defValue);
        if (!preferencesValue.IsFloat()) {
            return defValue;
        }
        return preferencesValue;
    }

    double GetDouble(const std::string &key, const double &defValue) override
    {
        PreferencesValue preferencesValue = Get(key, defValue);
        if (!preferencesValue.IsDouble()) {
            return defValue;
        }
        return preferencesValue;
    }

    int64_t GetLong(const std::string &key, const int64_t &defValue) override
    {
        PreferencesValue preferencesValue = Get(key, defValue);
        if (!preferencesValue.IsLong()) {
            return defValue;
        }
        return preferencesValue;
    }

    bool HasKey(const std::string &key) override;

    int PutInt(const std::string &key, int value) override
    {
        return Put(key, value);
    }

    int PutString(const std::string &key, const std::string &value) override
    {
        return Put(key, value);
    }

    int PutBool(const std::string &key, bool value) override
    {
        return Put(key, value);
    }

    int PutLong(const std::string &key, int64_t value) override
    {
        return Put(key, value);
    }

    int PutFloat(const std::string &key, float value) override
    {
        return Put(key, value);
    }

    int PutDouble(const std::string &key, double value) override
    {
        return Put(key, value);
    }

    std::map<std::string, PreferencesValue> GetAll() override;

    int Delete(const std::string &key) override;

    int Clear() override;

    void Flush() override;

    int FlushSync() override;

    void RegisterObserver(std::shared_ptr<PreferencesObserver> preferencesObserver) override;

    void UnRegisterObserver(std::shared_ptr<PreferencesObserver> preferencesObserver) override;

    static std::string MakeBackupPath(const std::string &prefPath);
    static std::string MakeBrokenPath(const std::string &prefPath);

private:
    class MemoryToDiskRequest {
    public:
        MemoryToDiskRequest(const std::map<std::string, PreferencesValue> &writeToDiskMap,
            const std::list<std::string> &keysModified,
            const std::vector<std::weak_ptr<PreferencesObserver>> preferencesObservers, int64_t memStataGeneration);
        ~MemoryToDiskRequest()
        {
        }
        void SetDiskWriteResult(bool wasWritten, int result);

        bool isSyncRequest_;
        int64_t memoryStateGeneration_;
        std::map<std::string, PreferencesValue> writeToDiskMap_;
        std::mutex reqMutex_;
        std::condition_variable reqCond_;
        std::list<std::string> keysModified_;
        std::vector<std::weak_ptr<PreferencesObserver>> preferencesObservers_;

        int writeToDiskResult_;
        bool wasWritten_;
    };

    std::shared_ptr<MemoryToDiskRequest> commitToMemory();
    void notifyPreferencesObserver(const MemoryToDiskRequest &request);
    bool StartLoadFromDisk();
    int CheckKey(const std::string &key);
    int CheckStringValue(const std::string &value);

    /* thread function */
    static void LoadFromDisk(std::shared_ptr<PreferencesImpl> pref);
    void AwaitLoadFile();
    static void WriteToDiskFile(std::shared_ptr<PreferencesImpl> pref, std::shared_ptr<MemoryToDiskRequest> mcr);
    bool CheckRequestValidForStateGeneration(const MemoryToDiskRequest &mcr);

    bool ReadSettingXml(const std::string &prefPath, std::map<std::string, PreferencesValue> &prefMap);
    bool WriteSettingXml(const std::string &prefPath, const std::map<std::string, PreferencesValue> &prefMap);

    bool loaded_;

    /* Current memory state (always increasing) */
    int64_t currentMemoryStateGeneration_;
    /* Latest memory state that was committed to disk */
    int64_t diskStateGeneration_;

    std::mutex mutex_;
    std::condition_variable cond_;

    std::vector<std::weak_ptr<PreferencesObserver>> preferencesObservers_;
    std::map<std::string, PreferencesValue> map_;
    std::list<std::string> modifiedKeys_;

    const std::string filePath_;
    const std::string backupPath_;
    const std::string brokenPath_;
};
} // End of namespace NativePreferences
} // End of namespace OHOS
#endif // End of #ifndef PREFERENCES_IMPL_H
