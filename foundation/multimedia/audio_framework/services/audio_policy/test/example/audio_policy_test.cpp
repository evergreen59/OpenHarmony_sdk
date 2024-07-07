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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <securec.h>
#include <unistd.h>

#include "audio_errors.h"
#include "audio_system_manager.h"
#include "audio_log.h"

using namespace std;
using namespace OHOS;
using namespace OHOS::AudioStandard;

namespace AudioPolicyTest {
    const int FIRST_ARG = 1;
    const int SECOND_ARG = 2;
    const int THIRD_ARG = 3;
    const int FOURTH_ARG = 4;
    const int FIFTH_ARG = 5;
    const int SIXTH_ARG = 6;
    const int SEVENTH_ARG = 7;
    const int EIGHTH_ARG = 8;
    const int NINTH_ARG = 9;
    const int TENTH_ARG = 10;
    const int ELEVENTH_ARG = 11;
    const int TWELFTH_ARG = 12;
    const int OPT_ARG_BASE = 10;
    const int OPT_SHORT_LEN = 3;
    const int OPT_GET_VOL_FACTOR = 1;
    const int OPT_GET_SS_VOL = 2;
}

static void PrintUsage(void)
{
    cout << "NAME" << endl << endl;
    cout << "\taudio_policy_test - Audio Policy Test " << endl << endl;
    cout << "SYNOPSIS" << endl << endl;
    cout << "\t#include <audio_system_manager.h>" << endl << endl;
    cout << "\t./audio_policy_test [OPTIONS]..." << endl << endl;
    cout << "DESCRIPTION" << endl << endl;
    cout << "\tControls audio volume, audio routing, audio mute" << endl << endl;
    cout << "-V\n\tSets Volume for streams, -S to setStream" << endl << endl;
    cout << "-v\n\tGets Volume for streams, -S to setStream" << endl << endl;
    cout << "-S\n\tSet stream type" << endl << endl;
    cout << "\tSupported Streams are" << endl << endl;
    cout << "\t4\tMUSIC" << endl << endl;
    cout << "\t3\tRING" << endl << endl;
    cout << "-D\n\tSets Device Active" << endl << endl;
    cout << "\tSupported Devices are" << endl << endl;
    cout << "\t2\tSPEAKER" << endl << endl;
    cout << "\t7\tBLUETOOTH_SCO" << endl << endl;
    cout << "-d\n\tGets Device Active" << endl << endl;
    cout << "-M\n\tSets Mute for streams, -S to setStream" << endl << endl;
    cout << "-m\n\tGets Mute for streams, -S to setStream" << endl << endl;
    cout << "-U\n\t Mutes the Microphone" << endl << endl;
    cout << "-u\n\t Checks if the Microphone is muted " << endl << endl;
    cout << "-R\n\tSets RingerMode" << endl << endl;
    cout << "-r\n\tGets RingerMode status" << endl << endl;
    cout << "-C\n\tSets AudioScene" << endl << endl;
    cout << "-c\n\tGets AudioScene status" << endl << endl;
    cout << "-N\n\tSet the discount volume factor" << endl << endl;
    cout << "-n\n\tGet the discount volume factor or Get single stream volume" << endl << endl;
    cout << "-s\n\tGet Stream Status" << endl << endl;
    cout << "AUTHOR" << endl << endl;
    cout << "\tWritten by Sajeesh Sidharthan and Anurup M" << endl << endl;
}

static void ShowAudioDeviceDescriptorsVector(std::vector<sptr<AudioDeviceDescriptor>> &audioDeviceDescriptorsVector)
{
    int vectorLen = audioDeviceDescriptorsVector.size();
    for (int i = 0; i < vectorLen; i ++) {
        cout << "------------show Audio Device Descriptors Vector------------" << endl;
        cout << "deviceRole: "       << audioDeviceDescriptorsVector[i]->deviceRole_ << endl;
        cout << "deviceType: "       << audioDeviceDescriptorsVector[i]->deviceType_ << endl;
        cout << "interruptGroupId: " << audioDeviceDescriptorsVector[i]->interruptGroupId_ << endl;
        cout << "volumeGroupId: "    << audioDeviceDescriptorsVector[i]->volumeGroupId_ << endl;
        cout << "networkId: "        << audioDeviceDescriptorsVector[i]->networkId_ << endl;
    }
}

static void ShowAudioRendererFilter(sptr<AudioRendererFilter> audioRendererFilter)
{
    cout << "------------show Audio Renderer Filter------------" << endl;
    cout << "uid: "           << audioRendererFilter->uid << endl;
    cout << "contentType: "   << audioRendererFilter->rendererInfo.contentType << endl;
    cout << "streamUsage: "   << audioRendererFilter->rendererInfo.streamUsage << endl;
    cout << "rendererFlags: " << audioRendererFilter->rendererInfo.rendererFlags << endl;
    cout << "streamId: "      << audioRendererFilter->streamId << endl;
}

static void HandleGetDevices(int argc, char *argv[], char option)
{
    AudioSystemManager *audioSystemMgr = AudioSystemManager::GetInstance();
    if ((option != 'g' && option != 'G') || argc != AudioPolicyTest::THIRD_ARG) {
        cout << "GetDevices invalid argv["<< argc <<"] "<< endl;
    }
    cout << "GetDevices() flag: " << argv[AudioPolicyTest::SECOND_ARG] << endl;
    int32_t intValue = atoi(argv[AudioPolicyTest::SECOND_ARG]);
    DeviceFlag deviceFlag = static_cast<DeviceFlag>(intValue);
    std::vector<sptr<AudioDeviceDescriptor>> audioDeviceDescriptorsVector;
    audioDeviceDescriptorsVector = audioSystemMgr->GetDevices(deviceFlag);
    cout << "GetDevices(Output Devices) Result: " << endl;
    ShowAudioDeviceDescriptorsVector(audioDeviceDescriptorsVector);
}

static void CallSelectOutputDevice(char option, std::vector<sptr<AudioDeviceDescriptor>> &audioDeviceDescriptorsVector,
    sptr<AudioRendererFilter> audioRendererFilter)
{
    AudioSystemManager *audioSystemMgr = AudioSystemManager::GetInstance();
    if (option == 'o') {
        int result = audioSystemMgr->SelectOutputDevice(audioDeviceDescriptorsVector);
        cout << "SelectOutputDevice Result: " << result << endl;
    } else {
        int result = audioSystemMgr->SelectOutputDevice(audioRendererFilter, audioDeviceDescriptorsVector);
        cout << "SelectOutputDevice by filter Result: " << result << endl;
    }
}

static void CreateAudioDeviceDescriptorVector(char *argv[],
    std::vector<sptr<AudioDeviceDescriptor>> &audioDeviceDescriptorsVector)
{
    sptr<AudioDeviceDescriptor> audioDeviceDescriptor = new(std::nothrow) AudioDeviceDescriptor();
    int32_t intValue = atoi(argv[AudioPolicyTest::SECOND_ARG]);
    audioDeviceDescriptor->deviceRole_ = static_cast<DeviceRole>(intValue);
    intValue = atoi(argv[AudioPolicyTest::THIRD_ARG]);
    audioDeviceDescriptor->deviceType_  = static_cast<DeviceType>(intValue);
    intValue = atoi(argv[AudioPolicyTest::FOURTH_ARG]);
    audioDeviceDescriptor->interruptGroupId_ = intValue;
    intValue = atoi(argv[AudioPolicyTest::FIFTH_ARG]);
    audioDeviceDescriptor->volumeGroupId_   = intValue;
    audioDeviceDescriptor->networkId_   = std::string(argv[AudioPolicyTest::SIXTH_ARG]);
    audioDeviceDescriptorsVector.push_back(audioDeviceDescriptor);
    ShowAudioDeviceDescriptorsVector(audioDeviceDescriptorsVector);
}

static void HandleSelectOutputDevice(int argc, char* argv[], char opt)
{
    if (argc == AudioPolicyTest::SEVENTH_ARG) {
        std::vector<sptr<AudioDeviceDescriptor>> audioDeviceDescriptorsVector;
        CreateAudioDeviceDescriptorVector(argv, audioDeviceDescriptorsVector);

        CallSelectOutputDevice(opt, audioDeviceDescriptorsVector, nullptr);
    } else if (argc == AudioPolicyTest::TWELFTH_ARG) {
        std::vector<sptr<AudioDeviceDescriptor>> audioDeviceDescriptorsVector;
        CreateAudioDeviceDescriptorVector(argv, audioDeviceDescriptorsVector);

        sptr<AudioRendererFilter> audioRendererFilter = new(std::nothrow) AudioRendererFilter();
        int32_t intValue = atoi(argv[AudioPolicyTest::SEVENTH_ARG]);
        audioRendererFilter->uid = static_cast<DeviceRole>(intValue);
        intValue = atoi(argv[AudioPolicyTest::EIGHTH_ARG]);
        audioRendererFilter->rendererInfo.contentType = static_cast<ContentType>(intValue);
        intValue = atoi(argv[AudioPolicyTest::NINTH_ARG]);
        audioRendererFilter->rendererInfo.streamUsage = static_cast<StreamUsage>(intValue);
        intValue = atoi(argv[AudioPolicyTest::TENTH_ARG]);
        audioRendererFilter->rendererInfo.rendererFlags = intValue;
        intValue = atoi(argv[AudioPolicyTest::ELEVENTH_ARG]);
        audioRendererFilter->streamId = intValue;
        ShowAudioRendererFilter(audioRendererFilter);
        CallSelectOutputDevice(opt, audioDeviceDescriptorsVector, audioRendererFilter);
    } else {
        cout << "------------Please input right arg Num------------" << endl;
        cout << "The arg order: " << endl;
        cout << "audioDevice(deviceRole, deviceType, networkId, interruptGroupId, volumeGroupId)" << endl;
        cout << "audioRendererFilter(uid,contentType,streamUsage,rendererFlags,streamId)" << endl;
    }
}

static void CallSelectInputDevice(char option, std::vector<sptr<AudioDeviceDescriptor>> &audioDeviceDescriptorsVector,
    sptr<AudioCapturerFilter> audioCapturerFilter)
{
    AudioSystemManager *audioSystemMgr = AudioSystemManager::GetInstance();
    if (option == 'i') {
        int result = audioSystemMgr->SelectInputDevice(audioDeviceDescriptorsVector);
        cout << "SelectInputDevice Result: " << result << endl;
    } else {
        int result = audioSystemMgr->SelectInputDevice(audioCapturerFilter, audioDeviceDescriptorsVector);
        cout << "SelectInputDevice by filter Result: " << result << endl;
    }
}


static void HandleSelectInputDevice(int argc, char* argv[], char opt)
{
    if (argc == AudioPolicyTest::SEVENTH_ARG) {
        std::vector<sptr<AudioDeviceDescriptor>> audioDeviceDescriptorsVector;
        CreateAudioDeviceDescriptorVector(argv, audioDeviceDescriptorsVector);

        CallSelectInputDevice(opt, audioDeviceDescriptorsVector, nullptr);
    } else if (argc == AudioPolicyTest::EIGHTH_ARG) {
        std::vector<sptr<AudioDeviceDescriptor>> audioDeviceDescriptorsVector;
        CreateAudioDeviceDescriptorVector(argv, audioDeviceDescriptorsVector);

        sptr<AudioCapturerFilter> audioCapturerFilter = new(std::nothrow) AudioCapturerFilter();
        int32_t intValue = atoi(argv[AudioPolicyTest::SEVENTH_ARG]);
        audioCapturerFilter->uid = intValue;
        cout << "------------show Audio Capturer Filter------------" << endl;
        cout << "uid: " << audioCapturerFilter->uid << endl;
        CallSelectInputDevice(opt, audioDeviceDescriptorsVector, audioCapturerFilter);
    } else {
        cout << "------------Please input right arg Num------------" << endl;
        cout << "The arg order: " << endl;
        cout << "audioDevice(deviceRole, deviceType, networkId, interruptGroupId, volumeGroupId)" << endl;
        cout << "audioCapturerFilter(uid)" << endl;
    }
}

static void HandleVolume(int argc, char* argv[], int streamType, char option)
{
    AudioSystemManager* audioSystemMgr = AudioSystemManager::GetInstance();
    std::string networkId = LOCAL_NETWORK_ID;

    if ((option != 'v' && option != 'V') || argc > AudioPolicyTest::FOURTH_ARG) {
        cout << "HandVolume invalid argv[" << argc << "] " << endl;
    }
    if (option == 'v' && argc == AudioPolicyTest::THIRD_ARG) {
        networkId = argv[AudioPolicyTest::SECOND_ARG];
        cout << "handle volume networkId: " << networkId << endl;
        std::vector<sptr<VolumeGroupInfo>> groups = audioSystemMgr->GetVolumeGroups(networkId);
        if (groups.size() > 0) {
            int32_t groupId = groups[0]->volumeGroupId_;
            std::shared_ptr<AudioGroupManager> groupManager = audioSystemMgr->GetGroupManager(groupId);
            float volume = groupManager->GetVolume(static_cast<AudioVolumeType>(streamType));
            cout << "Get Volume : " << volume << endl;
        }
    } else if (option == 'V' && argc == AudioPolicyTest::FOURTH_ARG) {
        networkId = argv[AudioPolicyTest::THIRD_ARG];
        cout << "handle volume networkId: " << networkId << endl;
        std::vector<sptr<VolumeGroupInfo>> groups = audioSystemMgr->GetVolumeGroups(networkId);
        if (groups.size() > 0) {
            int32_t groupId = groups[0]->volumeGroupId_;
            std::shared_ptr<AudioGroupManager> groupManager = audioSystemMgr->GetGroupManager(groupId);

            float volume = strtof(optarg, nullptr);
            cout << "Set Volume : " << volume << endl;
            int32_t result = groupManager->SetVolume(static_cast<AudioVolumeType>(streamType), volume);
            cout << "Set Volume Result: " << result << endl;
        }
    } else {
        cout << "wrong parms " << endl;
    }
}

static void HandleMute(int streamType, char option)
{
    AudioSystemManager *audioSystemMgr = AudioSystemManager::GetInstance();
    if (option == 'm') {
        bool muteStatus = audioSystemMgr->IsStreamMute(static_cast<AudioVolumeType>(streamType));
        cout << "Get Mute : " << muteStatus << endl;
    } else {
        int mute = strtol(optarg, nullptr, AudioPolicyTest::OPT_ARG_BASE);
        cout << "Set Mute : " << mute << endl;
        int32_t result = audioSystemMgr->SetMute(static_cast<AudioVolumeType>(streamType),
            (mute) ? true : false);
        cout << "Set Mute Result: " << result << endl;
    }
}

static void HandleMicMute(char option)
{
    AudioSystemManager *audioSystemMgr = AudioSystemManager::GetInstance();
    if (option == 'u') {
        bool muteStatus = audioSystemMgr->IsMicrophoneMute();
        cout << "Is Mic Mute : " << muteStatus << endl;
    } else {
        int mute = strtol(optarg, nullptr, AudioPolicyTest::OPT_ARG_BASE);
        cout << "Set Mic Mute : " << mute << endl;
        int32_t result = audioSystemMgr->SetMicrophoneMute((mute) ? true : false);
        cout << "Set Mic Mute Result: " << result << endl;
    }
}

static void SetStreamType(int &streamType)
{
    streamType = strtol(optarg, nullptr, AudioPolicyTest::OPT_ARG_BASE);
    cout << "Set Stream : " << streamType << endl;
}

static void IsStreamActive()
{
    AudioSystemManager *audioSystemMgr = AudioSystemManager::GetInstance();
    int streamType = strtol(optarg, nullptr, AudioPolicyTest::OPT_ARG_BASE);
    cout << "Stream Active: " << audioSystemMgr->IsStreamActive(
        static_cast<AudioVolumeType>(streamType)) << endl;
}

static void SetDeviceActive(int argc, char *argv[])
{
    AudioSystemManager *audioSystemMgr = AudioSystemManager::GetInstance();
    int active = -1;
    int device = strtol(optarg, nullptr, AudioPolicyTest::OPT_ARG_BASE);
    cout << "Set Device : " << device << endl;

    if (optind < argc && *argv[optind] != '-') {
        active = strtol(argv[optind], nullptr, AudioPolicyTest::OPT_ARG_BASE);
        optind++;
    }
    cout << "Active : " << active << endl << endl;

    int32_t result = audioSystemMgr->SetDeviceActive(ActiveDeviceType(device),
        (active) ? true : false);
    cout << "Set DeviceActive Result: " << result << endl;
}

static void IsDeviceActive()
{
    AudioSystemManager *audioSystemMgr = AudioSystemManager::GetInstance();
    int device = strtol(optarg, nullptr, AudioPolicyTest::OPT_ARG_BASE);
    bool devActiveStatus = audioSystemMgr->IsDeviceActive(ActiveDeviceType(device));
    cout << "GetDevice Active : " << devActiveStatus << endl;
}

static void SetAudioParamter(int argc, char* argv[])
{
    std::string key = "";
    std::string value = "";
    if (argc == AudioPolicyTest::FOURTH_ARG) {
        key = argv[AudioPolicyTest::SECOND_ARG];
        value = argv[AudioPolicyTest::THIRD_ARG];
        AudioSystemManager* audioSystemMgr = AudioSystemManager::GetInstance();
        audioSystemMgr->SetAudioParameter(key, value);
        cout << "SetAudioParameter for key " << key << "; value: " << value << endl;
    }
}

static void GetAudioParamter(int argc, char* argv[])
{
    std::string key = "";
    if (argc == AudioPolicyTest::THIRD_ARG) {
        key = argv[AudioPolicyTest::SECOND_ARG];
        AudioSystemManager* audioSystemMgr = AudioSystemManager::GetInstance();
        std::string value = audioSystemMgr->GetAudioParameter(key);
        cout << "GetAudioParameter for key " << key << "; result: " << value << endl;
    }
}

static void HandleRingerMode(char option)
{
    AudioSystemManager *audioSystemMgr = AudioSystemManager::GetInstance();
    if (option == 'r') {
        int ringMode = static_cast<int32_t>(audioSystemMgr->GetRingerMode());
        cout << "Get Ringer Mode : " << ringMode << endl;
    } else {
        int ringMode = strtol(optarg, nullptr, AudioPolicyTest::OPT_ARG_BASE);
        cout << "Set Ringer Mode : " << ringMode << endl;
        audioSystemMgr->SetRingerMode(static_cast<AudioRingerMode>(ringMode));
    }
}

static void HandleAudioScene(char option)
{
    AudioSystemManager *audioSystemMgr = AudioSystemManager::GetInstance();
    if (option == 'c') {
        int scene = static_cast<int32_t>(audioSystemMgr->GetAudioScene());
        cout << "Get Audio Scene : " << scene << endl;
    } else {
        int scene = strtol(optarg, nullptr, AudioPolicyTest::OPT_ARG_BASE);
        cout << "Set Audio Scene : " << scene << endl;
        audioSystemMgr->SetAudioScene(static_cast<AudioScene>(scene));
    }
}

static void NoValueError()
{
    char option[AudioPolicyTest::OPT_SHORT_LEN];
    cout << "option ";
    int len = snprintf_s(option, sizeof(option), sizeof(option) - 1, "-%c", optopt);
    if (len <= 0) {
        cout << "NoValueError: snprintf_s error : buffer allocation fails";
        return;
    }

    cout << option << " needs a value" << endl << endl;
    PrintUsage();
}

static void UnknownOptionError()
{
    char option[AudioPolicyTest::OPT_SHORT_LEN];
    int len = snprintf_s(option, sizeof(option), sizeof(option) - 1, "-%c", optopt);
    if (len <= 0) {
        cout << "unknown option: snprintf_s error : buffer allocation fails";
        return;
    }
    cout << "unknown option: " << option << endl;
    PrintUsage();
}

static void HandleUpdateStreamState(int type, char *seg1)
{
    AudioSystemManager *audioSystemMgr = AudioSystemManager::GetInstance();
    cout << "HandleUpdateStreamState : Runing " <<  seg1 << endl;
    
    const int32_t uid = atoi(seg1);
    cout << "HandleUpdateStreamState : uid : " << uid << endl;
    if (uid == 0) {
        return;
    }

    StreamSetState sate = StreamSetState::STREAM_PAUSE;
    AudioStreamType stype = AudioStreamType::STREAM_MEDIA;
    int32_t result = 0;
    if (type == 0) {
        cout << "type :: Stream_Pause :: " << type << endl;
    } else {
        sate = StreamSetState::STREAM_RESUME;
        cout << "type :: Stream_Resume :: " << type << endl;
    }
    result = audioSystemMgr->UpdateStreamState(uid, sate, stype);
    cout << "result :  " << result << endl;
}

static void HandleSingleStreamVolumeOption(int argc, char* argv[], char opt)
{
    if (argc != AudioPolicyTest::FOURTH_ARG) {
        cout << "Incorrect number of test commands." << endl;
        return;
    }

    AudioSystemManager *audioSystemMgr = AudioSystemManager::GetInstance();
    int32_t streamId = atoi(argv[AudioPolicyTest::SECOND_ARG]);
    if (opt == 'N') {
        float volume = atof(argv[AudioPolicyTest::THIRD_ARG]);
        if (volume < 0 || volume > 1.0f) {
            cout << "volume out of range." << endl;
            return;
        }
        audioSystemMgr->SetLowPowerVolume(streamId, volume);
        cout << "Set low power volume :" << volume << endl;
    } else {
        int32_t opt_flag = atoi(argv[AudioPolicyTest::THIRD_ARG]);
        if (opt_flag == AudioPolicyTest::OPT_GET_VOL_FACTOR) {
            float volume = audioSystemMgr->GetLowPowerVolume(streamId);
            cout << "Get discounted volume factor: " << volume << endl;
        } else if (opt_flag == AudioPolicyTest::OPT_GET_SS_VOL) {
            float volume = audioSystemMgr->GetSingleStreamVolume(streamId);
            cout << "Get single stream volume: " << volume << endl;
        } else {
            cout << "invalid operation." << endl;
        }
    }
}

static void HandleGetVolumeGroups(int argc, char* argv[])
{
    AudioSystemManager *audioSystemMgr = AudioSystemManager::GetInstance();
    if (argc == AudioPolicyTest::THIRD_ARG) {
        std::string networkId = argv[AudioPolicyTest::SECOND_ARG];
        cout << "networkId: "<< networkId << endl;
        std::vector<sptr<VolumeGroupInfo>> volumeGroups = audioSystemMgr->GetVolumeGroups(networkId);
        for (auto iter : volumeGroups) {
            cout << "===============id:" << iter->volumeGroupId_ << "=================" << endl;
            cout << "name: " << iter->groupName_ << endl;
            cout << "networkId: " << iter->networkId_ << endl;
            cout << "connectType: " << iter->connectType_ << endl;
            cout << "mappingId: " << iter->mappingId_ << endl;
        }
    }
}
int main(int argc, char* argv[])
{
    int opt = 0;
    if (((argc >= AudioPolicyTest::SECOND_ARG) && !strcmp(argv[AudioPolicyTest::FIRST_ARG], "--help")) ||
        (argc == AudioPolicyTest::FIRST_ARG)) {
        PrintUsage();
        return ERR_INVALID_PARAM;
    }

    int streamType = static_cast<int32_t>(AudioVolumeType::STREAM_MUSIC);
    while ((opt = getopt(argc, argv, ":V:U:S:D:M:R:C:X:Z:d:s:T:vmrucOoIiGgNntp")) != -1) {
        switch (opt) {
            case 'G':
            case 'g':
                HandleGetDevices(argc, argv, opt);
                break;
            case 'O':
            case 'o':
                HandleSelectOutputDevice(argc, argv, opt);
                break;
            case 'I':
            case 'i':
                HandleSelectInputDevice(argc, argv, opt);
                break;
            case 'V':
            case 'v':
                HandleVolume(argc, argv, streamType, opt);
                break;
            case 'M':
            case 'm':
                HandleMute(streamType, opt);
                break;
            case 'U':
            case 'u':
                HandleMicMute(opt);
                break;
            case 'S':
                SetStreamType(streamType);
                break;
            case 's':
                IsStreamActive();
                break;
            case 'D':
                SetDeviceActive(argc, argv);
                break;
            case 'd':
                IsDeviceActive();
                break;
            case 'R':
            case 'r':
                HandleRingerMode(opt);
                break;
            case 'C':
            case 'c':
                HandleAudioScene(opt);
                break;
            case 'X':
                HandleUpdateStreamState(0, optarg);
                break;
            case 'Z':
                HandleUpdateStreamState(1, optarg);
                break;
            case 'N':
            case 'n':
                HandleSingleStreamVolumeOption(argc, argv, opt);
                break;
            case ':':
                NoValueError();
                break;
            case '?':
                UnknownOptionError();
                break;
            case 'T':
                SetAudioParamter(argc, argv);
                break;
            case 't':
                GetAudioParamter(argc, argv);
                break;
            case 'p':
                HandleGetVolumeGroups(argc, argv);
                break;
            default:
                break;
        }
    }

    return 0;
}
