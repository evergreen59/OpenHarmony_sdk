/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "player_demo.h"
#include <iostream>
#include "media_data_source_demo_noseek.h"
#include "media_data_source_demo_seekable.h"
#include "string_ex.h"
#include "media_errors.h"
#include "directory_ex.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_surface_node.h"
#include "window_option.h"

using namespace OHOS;
using namespace OHOS::Media;
using namespace MediaDemo;
using namespace std;
namespace {
const std::string SURFACE_STRIDE_ALIGNMENT = "SURFACE_STRIDE_ALIGNMENT";
const std::string SURFACE_FORMAT_KEY = "SURFACE_FORMAT";
constexpr float EPSINON = 0.0001;
constexpr float SPEED_0_75_X = 0.75;
constexpr float SPEED_1_00_X = 1.00;
constexpr float SPEED_1_25_X = 1.25;
constexpr float SPEED_1_75_X = 1.75;
constexpr float SPEED_2_00_X = 2.00;
constexpr uint32_t PERCENT = 1;
constexpr uint32_t TIME = 2;
}

// PlayerCallback override
void PlayerCallbackDemo::OnError(int32_t errorCode, const std::string &errorMsg)
{
    cout << "Error received, errorCode: " << errorCode << "errorMsg: " << errorMsg << endl;
}

void PlayerCallbackDemo::OnInfo(PlayerOnInfoType type, int32_t extra, const Format &infoBody)
{
    (void)infoBody;
    switch (type) {
        case INFO_TYPE_SEEKDONE:
            cout << "PlayerCallback: OnSeekDone currentPositon is " << extra << endl;
            break;
        case INFO_TYPE_SPEEDDONE:
            cout << "PlayerCallback: SpeedDone " << endl;
            break;
        case INFO_TYPE_BITRATEDONE:
            cout << "PlayerCallback: BitRateDone " << endl;
            break;
        case INFO_TYPE_EOS:
            cout << "PlayerCallback: OnEndOfStream isLooping is " << extra << endl;
            break;
        case INFO_TYPE_BUFFERING_UPDATE:
            PrintBufferingUpdate(infoBody);
            break;
        case INFO_TYPE_BITRATE_COLLECT:
            PrintBitRate(infoBody);
            break;
        case INFO_TYPE_STATE_CHANGE:
            state_ = static_cast<PlayerStates>(extra);
            PrintState(state_);
            break;
        case INFO_TYPE_POSITION_UPDATE:
            if (updateCount_ == POSITION_UPDATE_INTERVAL) {
                cout << "OnPositionUpdated position is " << extra << endl;
                updateCount_ = 0;
            }
            updateCount_++;
            break;
        case INFO_TYPE_MESSAGE:
            cout << "PlayerCallback: OnMessage is " << extra << endl;
            break;
        case INFO_TYPE_RESOLUTION_CHANGE:
            PrintResolution(infoBody);
            break;
        case INFO_TYPE_VOLUME_CHANGE:
            cout << "PlayerCallback: volume changed" << endl;
            break;
        default:
            break;
    }
}

void PlayerCallbackDemo::SetBufferingOut(int32_t bufferingOut)
{
    bufferingOut_ = bufferingOut;
}

void PlayerCallbackDemo::PrintState(PlayerStates state) const
{
    if (STATE_MAP.count(state) != 0) {
        cout << "State:" << (STATE_MAP.at(state)).c_str() << endl;
    } else {
        cout << "Invalid state" << endl;
    }
}

void PlayerCallbackDemo::PrintResolution(const Format &infoBody) const
{
    int32_t width = 0;
    int32_t height = 0;
    (void)infoBody.GetIntValue(std::string(PlayerKeys::PLAYER_WIDTH), width);
    (void)infoBody.GetIntValue(std::string(PlayerKeys::PLAYER_HEIGHT), height);
    cout << "PlayerCallback: OnResolution changed width " << width << " height " << height << endl;
}

void PlayerCallbackDemo::PrintBufferingUpdate(const Format &infoBody) const
{
    int32_t value = 0;
    if (infoBody.ContainKey(std::string(PlayerKeys::PLAYER_BUFFERING_START))) {
        infoBody.GetIntValue(std::string(PlayerKeys::PLAYER_BUFFERING_START), value);
        cout << "PlayerCallback: OnMessage is buffering start" << endl;
    } else if (infoBody.ContainKey(std::string(PlayerKeys::PLAYER_BUFFERING_END))) {
        infoBody.GetIntValue(std::string(PlayerKeys::PLAYER_BUFFERING_END), value);
        cout << "PlayerCallback: OnMessage is buffering end" << endl;
    } else if (infoBody.ContainKey(std::string(PlayerKeys::PLAYER_BUFFERING_PERCENT))) {
        infoBody.GetIntValue(std::string(PlayerKeys::PLAYER_BUFFERING_PERCENT), value);
        if ((static_cast<uint32_t>(bufferingOut_) & PERCENT) == PERCENT) {
            cout << "OnBufferingPercent update is " << value << "%" << endl;
        }
    } else if (infoBody.ContainKey(std::string(PlayerKeys::PLAYER_CACHED_DURATION))) {
        infoBody.GetIntValue(std::string(PlayerKeys::PLAYER_CACHED_DURATION), value);
        if ((static_cast<uint32_t>(bufferingOut_) & TIME) == TIME) {
            cout << "OnCachedDuration update is " << value << "ms" << endl;
        }
    }
}

void PlayerCallbackDemo::PrintBitRate(const Format &infoBody) const
{
    uint8_t *addr = nullptr;
    size_t size  = 0;
    uint32_t bitrate = 0;
    if (infoBody.ContainKey(std::string(PlayerKeys::PLAYER_BITRATE))) {
        infoBody.GetBuffer(std::string(PlayerKeys::PLAYER_BITRATE), &addr, size);
        if (addr == nullptr) {
            return;
        }

        while (size > 0) {
            if ((size - sizeof(uint32_t)) < 0) {
                break;
            }

            bitrate = *(static_cast<uint32_t *>(static_cast<void *>(addr)));
            cout << "hls bitrate : " << bitrate << endl;

            addr += sizeof(uint32_t);
            size -= sizeof(uint32_t);
        }
    }
}

PlayerDemo::PlayerDemo()
{
}

PlayerDemo::~PlayerDemo()
{
    if (previewWindow_ != nullptr) {
        previewWindow_->Destroy();
        previewWindow_ = nullptr;
    }
}

sptr<Surface> PlayerDemo::GetSubWindowSurface()
{
    if (SetSurfaceSize() != 0) {
        cout << "SetSurface Size fail" << endl;
        return nullptr;
    }

    sptr<Rosen::WindowOption> option = new Rosen::WindowOption();
    option->SetWindowRect({ 0, 0, width_, height_ });
    option->SetWindowType(Rosen::WindowType::WINDOW_TYPE_APP_LAUNCHING);
    option->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    previewWindow_ = Rosen::Window::Create("xcomponent_window", option);
    if (previewWindow_ == nullptr || previewWindow_->GetSurfaceNode() == nullptr) {
        cout << "previewWindow_ is nullptr" << endl;
        return nullptr;
    }

    previewWindow_->Show();
    auto surfaceNode = previewWindow_->GetSurfaceNode();
    surfaceNode->SetFrameGravity(Rosen::Gravity::RESIZE);
    Rosen::RSTransaction::FlushImplicitTransaction();
    return surfaceNode->GetSurface();
}

sptr<Surface> PlayerDemo::GetVideoSurface()
{
    cout << "Please enter the number of mode(default no window):" << endl;
    cout << "0:no window" << endl;
    cout << "2:sub window" << endl;
    string mode;
    (void)getline(cin, mode);
    sptr<Surface> producerSurface = nullptr;
    if (mode == "0" || mode == "") {
        return nullptr;
    } else if (mode == "2") {
        producerSurface = GetSubWindowSurface();
    }

    if (producerSurface == nullptr) {
        cout << "producerSurface is nullptr" << endl;
        return nullptr;
    }
    (void)producerSurface->SetUserData(SURFACE_FORMAT_KEY, std::to_string(static_cast<int>(PIXEL_FMT_RGBA_8888)));
    cout << "GetVideoSurface ok" << endl;
    return producerSurface;
}

void PlayerDemo::Seek(const std::string &cmd)
{
    int32_t time = -1;
    if (!StrToInt(cmd, time) || time < 0) {
        cout << "You need to configure the seek time parameter" << endl;
        return;
    }

    cout << "Please enter the seek mode(seek previous sync):" << endl;
    cout << "0:seek previous sync" << endl;
    cout << "1:seek next sync" << endl;
    cout << "2:seek closest sync" << endl;
    cout << "3:seek closest" << endl;
    string mode;
    PlayerSeekMode seekMode = SEEK_PREVIOUS_SYNC;
    (void)getline(cin, mode);
    if (mode == "1") {
        seekMode = SEEK_NEXT_SYNC;
    } else if (mode == "2") {
        seekMode = SEEK_CLOSEST_SYNC;
    } else if (mode == "3") {
        seekMode = SEEK_CLOSEST;
    }

    if (player_->Seek(time, seekMode) != 0) {
        cout << "Operation Failed" << endl;
    } else {
        cout << "Operation OK" << endl;
    }
}

int32_t PlayerDemo::ChangeModeToSpeed(const PlaybackRateMode &mode, double &rate) const
{
    if (mode == SPEED_FORWARD_0_75_X) {
        rate = SPEED_0_75_X;
    } else if (mode == SPEED_FORWARD_1_00_X) {
        rate = SPEED_1_00_X;
    } else if (mode == SPEED_FORWARD_1_25_X) {
        rate = SPEED_1_25_X;
    } else if (mode == SPEED_FORWARD_1_75_X) {
        rate = SPEED_1_75_X;
    } else if (mode == SPEED_FORWARD_2_00_X) {
        rate = SPEED_2_00_X;
    } else {
        return -1;
    }
    return 0;
}

int32_t PlayerDemo::ChangeSpeedToMode(const double &rate, PlaybackRateMode &mode) const
{
    if (abs(rate - SPEED_0_75_X) < EPSINON) {
        mode = SPEED_FORWARD_0_75_X;
    } else if (abs(rate - SPEED_1_00_X) < EPSINON) {
        mode = SPEED_FORWARD_1_00_X;
    } else if (abs(rate - SPEED_1_25_X) < EPSINON) {
        mode = SPEED_FORWARD_1_25_X;
    } else if (abs(rate - SPEED_1_75_X) < EPSINON) {
        mode = SPEED_FORWARD_1_75_X;
    } else if (abs(rate - SPEED_2_00_X) < EPSINON) {
        mode = SPEED_FORWARD_2_00_X;
    } else {
        return -1;
    }
    return  0;
}

int32_t PlayerDemo::GetVideoTrackInfo()
{
    std::vector<Format> videoTrack;
    int32_t ret = player_->GetVideoTrackInfo(videoTrack);
    if (ret == 0) {
        cout << "Video Track cnt: " << videoTrack.size() << endl;
        std::string mime = "";
        int32_t bitrate = -1;
        int32_t width = -1;
        int32_t height = -1;
        int32_t framerate = -1;
        int32_t type = -1;
        int32_t index = -1;
        for (auto iter = videoTrack.begin(); iter != videoTrack.end(); iter++) {
            iter->GetStringValue(std::string(PlayerKeys::PLAYER_MIME), mime);
            iter->GetIntValue(std::string(PlayerKeys::PLAYER_BITRATE), bitrate);
            iter->GetIntValue(std::string(PlayerKeys::PLAYER_WIDTH), width);
            iter->GetIntValue(std::string(PlayerKeys::PLAYER_HEIGHT), height);
            iter->GetIntValue(std::string(PlayerKeys::PLAYER_FRAMERATE), framerate);
            iter->GetIntValue(std::string(PlayerKeys::PLAYER_TRACK_TYPE), type);
            iter->GetIntValue(std::string(PlayerKeys::PLAYER_TRACK_INDEX), index);
            cout << "mime: " << mime.c_str() << ", bitrate: " << bitrate << ", width: " << width << ", height: " <<
                height << ", framerate: " << framerate << ", type: " << type << ", index: " << index << endl;
        }
    }
    return 0;
}
int32_t PlayerDemo::GetAudioTrackInfo()
{
    std::vector<Format> audioTrack;
    int32_t ret = player_->GetAudioTrackInfo(audioTrack);
    if (ret == 0) {
        cout << "Audio Track cnt: " << audioTrack.size() << endl;
        std::string mime = "";
        int32_t bitrate = -1;
        int32_t sampleRate = -1;
        int32_t channels = -1;
        int32_t type = -1;
        int32_t index = -1;
        std::string language = "";
        for (auto iter = audioTrack.begin(); iter != audioTrack.end(); iter++) {
            iter->GetStringValue(std::string(PlayerKeys::PLAYER_MIME), mime);
            iter->GetIntValue(std::string(PlayerKeys::PLAYER_BITRATE), bitrate);
            iter->GetIntValue(std::string(PlayerKeys::PLAYER_SAMPLE_RATE), sampleRate);
            iter->GetIntValue(std::string(PlayerKeys::PLAYER_CHANNELS), channels);
            iter->GetStringValue(std::string(PlayerKeys::PLAYER_LANGUGAE), language);
            iter->GetIntValue(std::string(PlayerKeys::PLAYER_TRACK_TYPE), type);
            iter->GetIntValue(std::string(PlayerKeys::PLAYER_TRACK_INDEX), index);
            cout << "mime: " << mime.c_str() << ", bitrate: " << bitrate << ", samplerate: " << sampleRate <<
                ", channels: " << channels << ", language: " << language.c_str()  << ", type: " << type <<
                ", index: " << index << endl;
        }
    }
    return 0;
}
int32_t PlayerDemo::GetTrackInfo()
{
    GetVideoTrackInfo();
    GetAudioTrackInfo();
    return 0;
}

int32_t PlayerDemo::GetPlaybackSpeed() const
{
    PlaybackRateMode mode;
    double rate;
    (void)player_->GetPlaybackSpeed(mode);
    if (ChangeModeToSpeed(mode, rate) != 0) {
        cout << "Change mode " << mode << " to speed failed" << endl;
    } else {
        cout << "Speed:" << rate << endl;
    }
    return 0;
}

void PlayerDemo::SetPlaybackSpeed(const std::string &cmd) const
{
    PlaybackRateMode mode;
    if (!cmd.empty()) {
        double rate = std::stod(cmd.c_str());
        if (ChangeSpeedToMode(rate, mode) != 0) {
            cout << "Change speed " << rate << " to mode failed" << endl;
            return;
        }

        if (player_->SetPlaybackSpeed(mode) != 0) {
            cout << "Operation Failed" << endl;
        } else {
            cout << "Operation OK" << endl;
        }
    }
}

void PlayerDemo::SelectBitRate(const std::string &cmd) const
{
    int32_t bitRate = 0;
    if (!StrToInt(cmd, bitRate)) {
        cout << "You need to configure the loop parameter" << endl;
        return;
    }

    if (player_->SelectBitRate(bitRate) != 0) {
        cout << "Operation Failed" << endl;
    } else {
        cout << "Operation OK" << endl;
    }
}

void PlayerDemo::SetVideoScaleType(const std::string &cmd) const
{
    int32_t videoScaleType = 0;
    if (!StrToInt(cmd, videoScaleType)) {
        cout << "video scale type input invaild" << endl;
        return;
    }
    Format format;
    format.PutIntValue(PlayerKeys::VIDEO_SCALE_TYPE, videoScaleType);
    if (player_->SetParameter(format) != 0) {
        cout << "Operation Failed" << endl;
    } else {
        cout << "Operation OK" << endl;
    }
}
void PlayerDemo::SetLoop(const std::string &cmd)
{
    int32_t loopEn = -1;
    if (!StrToInt(cmd, loopEn)) {
        cout << "You need to configure the loop parameter" << endl;
        return;
    }
    if (player_->SetLooping(static_cast<bool>(loopEn)) != 0) {
        cout << "Operation Failed" << endl;
    } else {
        cout << "Operation OK" << endl;
    }
}

int32_t PlayerDemo::GetPlaying()
{
    bool isPlay = player_->IsPlaying();
    cout << "Playing:" << isPlay << endl;
    return 0;
}

int32_t PlayerDemo::GetLooping()
{
    bool isLoop = player_->IsLooping();
    cout << "Looping:" << isLoop << endl;
    return 0;
}

void PlayerDemo::GetCurrentTime()
{
    int32_t time = -1;
    (void)player_->GetCurrentTime(time);
    cout << "GetCurrentTime:" << time << endl;
}

void PlayerDemo::DoCmd(const std::string &cmd)
{
    if (cmd.find("source ") != std::string::npos) {
        (void)SelectSource(cmd.substr(cmd.find("source ") + std::string("source ").length()));
    } else if (cmd.find("seek ") != std::string::npos) {
        Seek(cmd.substr(cmd.find("seek ") + std::string("seek ").length()));
    } else if (cmd.find("volume ") != std::string::npos) {
        std::string volume = cmd.substr(cmd.find("volume ") + std::string("volume ").length());
        if (!volume.empty()) {
            (void)player_->SetVolume(std::stof(volume.c_str()), std::stof(volume.c_str()));
        }
    } else if (cmd.find("duration") != std::string::npos) {
        int32_t duration = -1;
        (void)player_->GetDuration(duration);
        cout << "GetDuration:" << duration << endl;
    } else if (cmd.find("time") != std::string::npos) {
        GetCurrentTime();
    } else if (cmd.find("loop ") != std::string::npos) {
        SetLoop(cmd.substr(cmd.find("loop ") + std::string("loop ").length()));
    } else if (cmd.find("speed ") != std::string::npos) {
        SetPlaybackSpeed(cmd.substr(cmd.find("speed ") + std::string("speed ").length()));
    } else if (cmd.find("trackinfo") != std::string::npos) {
        GetTrackInfo();
    } else if (cmd.find("videosize") != std::string::npos) {
        cout << "video width: " << player_->GetVideoWidth() << ", height: " << player_->GetVideoHeight();
    } else if (cmd.find("bitrate ") != std::string::npos) {
        SelectBitRate(cmd.substr(cmd.find("bitrate ") + std::string("bitrate ").length()));
    } else if (cmd.find("videoscaletype ") != std::string::npos) {
        SetVideoScaleType(cmd.substr(cmd.find("videoscaletype ") + std::string("videoscaletype ").length()));
    }
}

void PlayerDemo::DoNext()
{
    std::string cmd;
    while (std::getline(std::cin, cmd)) {
        auto iter = playerTable_.find(cmd);
        if (iter != playerTable_.end()) {
            auto func = iter->second;
            if (func() != 0) {
                cout << "Operation error" << endl;
            }
            if (cmd.find("stop") != std::string::npos && dataSrc_ != nullptr) {
                dataSrc_->Reset();
            }
            continue;
        } else if (cmd.find("quit") != std::string::npos || cmd == "q") {
            break;
        } else {
            DoCmd(cmd);
            continue;
        }
    }
}

void PlayerDemo::RegisterTable()
{
    (void)playerTable_.emplace("prepare", std::bind(&Player::Prepare, player_));
    (void)playerTable_.emplace("prepareasync", std::bind(&Player::PrepareAsync, player_));
    (void)playerTable_.emplace("", std::bind(&Player::Play, player_)); // ENTER -> play
    (void)playerTable_.emplace("play", std::bind(&Player::Play, player_));
    (void)playerTable_.emplace("pause", std::bind(&Player::Pause, player_));
    (void)playerTable_.emplace("stop", std::bind(&Player::Stop, player_));
    (void)playerTable_.emplace("reset", std::bind(&Player::Reset, player_));
    (void)playerTable_.emplace("release", std::bind(&Player::Release, player_));
    (void)playerTable_.emplace("isplaying", std::bind(&PlayerDemo::GetPlaying, this));
    (void)playerTable_.emplace("isloop", std::bind(&PlayerDemo::GetLooping, this));
    (void)playerTable_.emplace("speed", std::bind(&PlayerDemo::GetPlaybackSpeed, this));
}

int32_t PlayerDemo::SetDataSrc(const string &path, bool seekable)
{
    cout << "Please enter the size of buffer:" << endl;
    cout << "0:default" << endl;
    string sizeStr;
    (void)getline(cin, sizeStr);
    int32_t size = -1;
    if (!StrToInt(sizeStr, size) || size < 0) {
        cout << "default size" << endl;
    } else {
        cout << "buffer size:" << size << endl;
    }
    if (seekable) {
        dataSrc_ = MediaDataSourceDemoSeekable::Create(path, size);
    } else {
        dataSrc_ = MediaDataSourceDemoNoSeek::Create(path, size);
    }
    return player_->SetSource(dataSrc_);
}

int32_t PlayerDemo::SetFdSource(const string &path)
{
    int32_t fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        cout << "Open file failed" << endl;
        return -1;
    }
    int32_t offset = 0;

    struct stat64 buffer;
    if (fstat64(fd, &buffer) != 0) {
        cout << "Get file state failed" << endl;
        return -1;
    }
    int64_t length = static_cast<int64_t>(buffer.st_size);
    cout << "fd = " << fd << ", offset = " << offset << ", length = " << length << endl;

    int32_t ret = player_->SetSource(fd, offset, length);
    (void)close(fd);
    return ret;
}

int32_t PlayerDemo::SelectSource(const string &pathOuter)
{
    string path;
    int32_t ret = -1;
    if (pathOuter == "") {
        cout << "Please enter the video/audio path: " << endl;
        (void)getline(cin, path);
    } else {
        path = pathOuter;
    }

    cout << "Path is " << path << endl;
    cout << "Please enter the number of source mode(default LOCAL):" << endl;
    cout << "0:local file source" << endl;
    cout << "1:stream file source with no seek" << endl;
    cout << "2:stream file source with seekable" << endl;
    cout << "3:file descriptor source" << endl;
    string srcMode;
    (void)getline(cin, srcMode);
    if (srcMode == "" || srcMode == "0") {
        cout << "source mode is LOCAL" << endl;
        ret = player_->SetSource(path);
    } else if (srcMode == "1") {
        cout << "source mode is stream NO seek" << endl;
        ret = SetDataSrc(path, false);
    } else if (srcMode == "2") {
        cout << "source mode is stream seekable" << endl;
        ret = SetDataSrc(path, true);
    } else if (srcMode == "3") {
        cout << "source mode is FD" << endl;
        ret = SetFdSource(path);
    } else {
        cout << "unknown mode" << endl;
    }
    return ret;
}

void PlayerDemo::SetVideoScaleType()
{
    cout << "Please select video scale type(default 0):" << endl;
    cout << "0:VIDEO_SCALE_TYPE_FIT" << endl;
    cout << "1:VIDEO_SCALE_TYPE_FIT_CROP" << endl;
    string type;
    (void)getline(cin, type);
    SetVideoScaleType(type);
}
int32_t PlayerDemo::SelectBufferingOut()
{
    cout << "Please enter the number of mode(no buffering info):" << endl;
    cout << "0:no buffering info" << endl;
    cout << "1:percent" << endl;
    cout << "2:time" << endl;
    cout << "3:percent and time" << endl;
    string mode;
    (void)getline(cin, mode);
    if (mode == "1") {
        return PERCENT;
    } else if (mode == "2") {
        return TIME;
    } else if (mode == "3") {
        return (PERCENT | TIME);
    } else {
        return 0;
    }
}

int32_t PlayerDemo::SelectRendererMode()
{
    cout << "Please select renderer mode (default no renderer)" << endl;
    cout << "0:no renderer" << endl;
    cout << "1:please set more details" << endl;
    string mode;
    (void)getline(cin, mode);
    if (mode == "1") {
        return SetRendererInfo();
    } else {
        return 0;
    }
}

int32_t PlayerDemo::SetRendererInfo()
{
    cout << "Please enter contentType(int)" << endl;
    int32_t contentType;
    cin >> contentType;
    cout << "Please enter streamUsage(int)" << endl;
    int32_t streamUsage;
    cin >> streamUsage;
    cout << "Please enter rendererFlags(int)" << endl;
    int32_t rendererFlags;
    cin >> rendererFlags;
    Format format;
    (void)format.PutIntValue(PlayerKeys::CONTENT_TYPE, contentType);
    (void)format.PutIntValue(PlayerKeys::STREAM_USAGE, streamUsage);
    (void)format.PutIntValue(PlayerKeys::RENDERER_FLAG, rendererFlags);
    return player_->SetParameter(format);
}


int32_t PlayerDemo::SetSurfaceSize()
{
    int32_t ret = 0;
    cout << "Please enter surface size(width x height):" << endl;
    cout << "0:1920 x 1080" << endl;
    cout << "1:640 x 360" << endl;
    string mode;
    (void)getline(cin, mode);
    if (mode == "" || mode == "0") {
        width_ = 1920; // 1920 for width
        height_ = 1080; // 1080 for height
    } else if (mode == "1") {
        width_ = 640; // 640 for width
        height_ = 360; // 360 for height
    } else {
        ret = -1;
    }
    return ret;
}

void PlayerDemo::RunCase(const string &path)
{
    player_ = OHOS::Media::PlayerFactory::CreatePlayer();
    if (player_ == nullptr) {
        cout << "player_ is null" << endl;
        return;
    }
    RegisterTable();
    std::shared_ptr<PlayerCallbackDemo> cb = std::make_shared<PlayerCallbackDemo>();
    cb->SetBufferingOut(SelectBufferingOut());

    int32_t ret = player_->SetPlayerCallback(cb);
    if (ret != 0) {
        cout << "SetPlayerCallback fail" << endl;
    }
    if (SelectSource(path) != 0) {
        cout << "SetSource fail" << endl;
        return;
    }
    sptr<Surface> producerSurface = nullptr;
    producerSurface = GetVideoSurface();
    if (producerSurface != nullptr) {
        ret = player_->SetVideoSurface(producerSurface);
        if (ret != 0) {
            cout << "SetVideoSurface fail" << endl;
        }
    }
    SetVideoScaleType();
    if (SelectRendererMode() != 0) {
        cout << "set renderer info fail" << endl;
    }
    ret = player_->PrepareAsync();
    if (ret !=  0) {
        cout << "PrepareAsync fail" << endl;
        return;
    }
    cout << "Enter your step:" << endl;
    DoNext();
}
