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

#ifndef PLAYER_H
#define PLAYER_H

#include <cstdint>
#ifdef SUPPORT_AUDIO_ONLY
#else
#include "surface.h"
#endif
#include "format.h"
#include "media_data_source.h"

namespace OHOS {
namespace Media {
class PlayerKeys {
public:
    static constexpr std::string_view PLAYER_STATE_CHANGED_REASON = "state_changed_reason";
    static constexpr std::string_view PLAYER_VOLUME_LEVEL = "volume_level";
    static constexpr std::string_view PLAYER_TRACK_INDEX = "track_index";
    static constexpr std::string_view PLAYER_TRACK_TYPE = "track_type";
    static constexpr std::string_view PLAYER_WIDTH = "width";
    static constexpr std::string_view PLAYER_HEIGHT = "height";
    static constexpr std::string_view PLAYER_MIME = "codec_mime";
    static constexpr std::string_view PLAYER_BITRATE = "bitrate";
    static constexpr std::string_view PLAYER_FRAMERATE = "frame_rate";
    static constexpr std::string_view PLAYER_LANGUGAE = "language_code";
    static constexpr std::string_view PLAYER_SAMPLE_RATE = "sample_rate";
    static constexpr std::string_view PLAYER_CHANNELS = "channel_count";
    static constexpr std::string_view PLAYER_BUFFERING_START = "buffering_start";
    static constexpr std::string_view PLAYER_BUFFERING_END = "buffering_end";
    static constexpr std::string_view PLAYER_BUFFERING_PERCENT = "buffering_percent";
    static constexpr std::string_view PLAYER_CACHED_DURATION = "cached_duration";
    static constexpr std::string_view CONTENT_TYPE = "content_type";
    static constexpr std::string_view STREAM_USAGE = "stream_usage";
    static constexpr std::string_view RENDERER_FLAG = "renderer_flag";
    static constexpr std::string_view VIDEO_SCALE_TYPE = "video_scale_type";
    static constexpr std::string_view AUDIO_INTERRUPT_MODE = "audio_interrupt_mode";
    static constexpr std::string_view AUDIO_INTERRUPT_TYPE = "audio_interrupt_type";
    static constexpr std::string_view AUDIO_INTERRUPT_FORCE = "audio_interrupt_force";
    static constexpr std::string_view AUDIO_INTERRUPT_HINT = "audio_interrupt_hint";
};

enum BufferingInfoType : int32_t {
    /* begin to b buffering */
    BUFFERING_START = 1,
    /* end to buffering */
    BUFFERING_END = 2,
    /* buffering percent */
    BUFFERING_PERCENT = 3,
    /* cached duration in milliseconds */
    CACHED_DURATION = 4,
};

enum PlayerErrorType : int32_t {
    /* Valid error, error code reference defined in media_errors.h */
    PLAYER_ERROR,
    /* Unknown error */
    PLAYER_ERROR_UNKNOWN,
    /* extend error type start,The extension error type agreed upon by the plug-in and
       the application will be transparently transmitted by the service. */
    PLAYER_ERROR_EXTEND_START = 0X10000,
};

enum PlayerMessageType : int32_t {
    /* unknown info */
    PLAYER_INFO_UNKNOWN = 0,
    /* first video frame start to render. */
    PLAYER_INFO_VIDEO_RENDERING_START,
    /* network bandwidth, uint is KB and passed by "extra"(arg 2). */
    PLAYER_INFO_NETWORK_BANDWIDTH,
    /* not fatal errors accured, errorcode see "media_errors.h" and passed by "extra"(arg 2). */
    PLAYER_INFO_WARNING,
    /* system new info type should be added here.
       extend start. App and plugins or PlayerEngine extended info type start. */
    PLAYER_INFO_EXTEND_START = 0X1000,
};

enum PlayerOnInfoType : int32_t {
    /* return the message when seeking done. */
    INFO_TYPE_SEEKDONE = 1,
    /* return the message when speeding done. */
    INFO_TYPE_SPEEDDONE,
    /* return the message when select bitrate done */
    INFO_TYPE_BITRATEDONE,
    /* return the message when playback is end of steam. */
    INFO_TYPE_EOS,
    /* return the message when PlayerStates changed. */
    INFO_TYPE_STATE_CHANGE,
    /* return the current posion of playback automatically. */
    INFO_TYPE_POSITION_UPDATE,
    /* return the playback message. */
    INFO_TYPE_MESSAGE,
    /* return the message when volume changed. */
    INFO_TYPE_VOLUME_CHANGE,
    /* return the message when video size is first known or updated. */
    INFO_TYPE_RESOLUTION_CHANGE,
    /* return multiqueue buffering time. */
    INFO_TYPE_BUFFERING_UPDATE,
    /* return hls bitrate.
       Bitrate is to convert data into uint8_t array storage,
       which needs to be forcibly converted to uint32_t through offset access. */
    INFO_TYPE_BITRATE_COLLECT,
    /* return the message when audio focus changed. */
    INFO_TYPE_INTERRUPT_EVENT,
    /* return the message when PlayerStates changed by audio. */
    INFO_TYPE_STATE_CHANGE_BY_AUDIO,
    /* return the message with extra information in format. */
    INFO_TYPE_EXTRA_FORMAT,
    /* return the duration of playback. */
    INFO_TYPE_DURATION_UPDATE,
};

enum PlayerStates : int32_t {
    /* error states */
    PLAYER_STATE_ERROR = 0,
    /* idle states */
    PLAYER_IDLE = 1,
    /* initialized states(Internal states) */
    PLAYER_INITIALIZED = 2,
    /* preparing states(Internal states) */
    PLAYER_PREPARING = 3,
    /* prepared states */
    PLAYER_PREPARED = 4,
    /* started states */
    PLAYER_STARTED = 5,
    /* paused states */
    PLAYER_PAUSED = 6,
    /* stopped states */
    PLAYER_STOPPED = 7,
    /* Play to the end states */
    PLAYER_PLAYBACK_COMPLETE = 8,
    /* released states */
    PLAYER_RELEASED = 9,
};

enum PlayerSeekMode : int32_t {
    /* sync to keyframes after the time point. */
    SEEK_NEXT_SYNC = 0,
    /* sync to keyframes before the time point. */
    SEEK_PREVIOUS_SYNC,
    /* sync to closest keyframes. */
    SEEK_CLOSEST_SYNC,
    /* seek to frames closest the time point. */
    SEEK_CLOSEST,
};

enum PlaybackRateMode : int32_t {
    /* Video playback at 0.75x normal speed */
    SPEED_FORWARD_0_75_X,
    /* Video playback at normal speed */
    SPEED_FORWARD_1_00_X,
    /* Video playback at 1.25x normal speed */
    SPEED_FORWARD_1_25_X,
    /* Video playback at 1.75x normal speed */
    SPEED_FORWARD_1_75_X,
    /* Video playback at 2.0x normal speed */
    SPEED_FORWARD_2_00_X,
};

enum VideoScaleType : int32_t {
    /**
     * The content is stretched to the fit the display surface rendering area. When
     * the aspect ratio of the content is not same as the display surface, the aspect
     * of the content is not maintained. This is the default scale type.
     */
    VIDEO_SCALE_TYPE_FIT = 0,

    /**
     * The content is stretched to the fit the display surface rendering area. When
     * the aspect ratio of the content is not the same as the display surface, content's
     * aspect ratio is maintained and the content is cropped to fit the display surface.
     */
    VIDEO_SCALE_TYPE_FIT_CROP,
};

class PlayerCallback {
public:
    virtual ~PlayerCallback() = default;
    /**
     * Called when an error occurred for versions older than api9
     *
     * @param errorType Error type. For details, see {@link PlayerErrorType}.
     * @param errorCode Error code.
     */
    __attribute__((deprecated)) virtual void OnError(PlayerErrorType errorType, int32_t errorCode)
    {
        (void)errorType;
        (void)errorCode;
    }

    /**
     * Called when a player message or alarm is received.
     *
     * @param type Indicates the information type. For details, see {@link PlayerOnInfoType}.
     * @param extra Indicates other information, for example, the start time position of a playing file.
     * @param infoBody According to the info type, the information carrier passed.Is an optional parameter.
     */
    virtual void OnInfo(PlayerOnInfoType type, int32_t extra, const Format &infoBody) = 0;

    /**
     * Called when an error occurred for versions above api9
     *
     * @param errorCode Error code.
     * @param errorMsg Error message.
     */
    virtual void OnError(int32_t errorCode, const std::string &errorMsg)
    {
        (void)errorCode;
        (void)errorMsg;
    }
};

class Player {
public:
    virtual ~Player() = default;

    /**
     * @brief Sets the playback source for the player. The corresponding source can be http url
     *
     * @param url Indicates the playback source.
     * @return Returns {@link MSERR_OK} if the url is set successfully; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetSource(const std::string &url) = 0;

    /**
     * @brief Sets the playback media data source for the player.
     *
     * @param dataSrc Indicates the media data source. in {@link media_data_source.h}
     * @return Returns {@link MSERR_OK} if the mediadatasource is set successfully; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetSource(const std::shared_ptr<IMediaDataSource> &dataSrc) = 0;

    /**
     * @brief Sets the playback media file descriptor source for the player.
     *
     * @param fd Indicates the file descriptor of media source.
     * @param offset Indicates the offset of media source in file descriptor.
     * @param size Indicates the size of media source.
     * @return Returns {@link MSERR_OK} if the fd source is set successfully; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetSource(int32_t fd, int64_t offset = 0, int64_t size = 0) = 0;

    /**
     * @brief Start playback.
     *
     * This function must be called after {@link Prepare}. If the player state is <b>Prepared</b>,
     * this function is called to start playback.
     *
     * @return Returns {@link MSERR_OK} if the playback is started; otherwise returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t Play() = 0;

    /**
     * @brief Prepares the playback environment and buffers media data asynchronous.
     *
     * This function must be called after {@link SetSource}.
     *
     * @return Returns {@link MSERR_OK} if the playback is prepared; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    __attribute__((deprecated)) virtual int32_t Prepare() = 0;

    /**
     * @brief Prepare the playback environment and buffers media data asynchronous.
     *
     * This function must be called after {@link SetSource}.
     *
     * @return Returns {@link MSERR_OK} if the playback is preparing; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t PrepareAsync() = 0;

    /**
     * @brief Pauses playback.
     *
     * @return Returns {@link MSERR_OK} if the playback is paused; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t Pause() = 0;

    /**
     * @brief Stop playback.
     *
     * @return Returns {@link MSERR_OK} if the playback is stopped; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t Stop() = 0;

    /**
     * @brief Restores the player to the initial state.
     *
     * After the function is called, add a playback source by calling {@link SetSource},
     * call {@link Play} to start playback again after {@link Prepare} is called.
     *
     * @return Returns {@link MSERR_OK} if the playback is reset; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t Reset() = 0;

    /**
     * @brief Releases player resources async
     *
     *  Asynchronous release guarantees the performance
     *  but cannot ensure whether the surfacebuffer is released.
     *  The caller needs to ensure the life cycle security of the sufrace
     *
     * @return Returns {@link MSERR_OK} if the playback is released; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t Release() = 0;

    /**
     * @brief Sets the volume of the player.
     *
     * This function can be used during playback or pause. The value <b>0</b> indicates no sound,
     * and <b>1</b> indicates the original volume. If no audio device is started or no audio
     * stream exists, the value <b>-1</b> is returned.
     *
     * @param leftVolume Indicates the target volume of the left audio channel to set,
     *        ranging from 0 to 1. each step is 0.01.
     * @param rightVolume Indicates the target volume of the right audio channel to set,
     *        ranging from 0 to 1. each step is 0.01.
     * @return Returns {@link MSERR_OK} if the volume is set; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetVolume(float leftVolume, float rightVolume) = 0;

    /**
     * @brief Changes the playback position.
     *
     * This function can be used during play or pause.
     *
     * @param mSeconds Indicates the target playback position, accurate to milliseconds.
     * @param mode Indicates the player seek mode. For details, see {@link PlayerSeekMode}.
     * @return Returns {@link MSERR_OK} if the seek is done; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
    */
    virtual int32_t Seek(int32_t mSeconds, PlayerSeekMode mode) = 0;

    /**
     * @brief Obtains the playback position, accurate to millisecond.
     *
     * @param currentTime Indicates the playback position.
     * @return Returns {@link MSERR_OK} if the current position is get; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t GetCurrentTime(int32_t &currentTime) = 0;

    /**
     * @brief Obtains the video track info, contains mimeType, bitRate, width, height, frameRata.
     *
     * @param video track info vec.
     * @return Returns {@link MSERR_OK} if the track info is get; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t GetVideoTrackInfo(std::vector<Format> &videoTrack) = 0;

    /**
     * @brief Obtains the audio track info, contains mimeType, bitRate, sampleRate, channels, language.
     *
     * @param audio track info vec.
     * @return Returns {@link MSERR_OK} if the track info is get; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t GetAudioTrackInfo(std::vector<Format> &audioTrack) = 0;

    /**
     * @brief get the video width.
     *
     * @return Returns width if success; else returns 0
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t GetVideoWidth() = 0;

    /**
     * @brief get the video height.
     *
     * @return Returns height if success; else returns 0
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t GetVideoHeight() = 0;

    /**
     * @brief Obtains the total duration of media files, accurate to milliseconds.
     *
     * @param duration Indicates the total duration of media files.
     * @return Returns {@link MSERR_OK} if the current duration is get; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t GetDuration(int32_t &duration) = 0;

    /**
     * @brief set the player playback rate
     *
     * @param mode the rate mode {@link PlaybackRateMode} which can set.
     * @return Returns {@link MSERR_OK} if the playback rate is set successful; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetPlaybackSpeed(PlaybackRateMode mode) = 0;

    /**
     * @brief get the current player playback rate
     *
     * @param mode the rate mode {@link PlaybackRateMode} which can get.
     * @return Returns {@link MSERR_OK} if the current player playback rate is get; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t GetPlaybackSpeed(PlaybackRateMode &mode) = 0;

    /**
     * @brief set the bit rate use for hls player
     * the playback bitrate expressed in bits per second, expressed in bits per second,
     * which is only valid for HLS protocol network flow. By default,
     * the player will select the appropriate bit rate and speed according to the network connection.
     * report the effective bit rate linked list by "INFO_TYPE_BITRATE_COLLECT"
     * set and select the specified bit rate, and select the bit rate that is less than and closest
     * to the specified bit rate for playback. When ready, read it to query the currently selected bit rate.
     * @param bitRate the bit rate, The unit is bps.
     * @return Returns {@link MSERR_OK} if the bit rate is set successfully; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SelectBitRate(uint32_t bitRate) = 0;

#ifdef SUPPORT_AUDIO_ONLY
#else
    /**
     * @brief Method to set the surface.
     *
     * @param surface pointer of the surface.
     * @return Returns {@link MSERR_OK} if the surface is set; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetVideoSurface(sptr<Surface> surface) = 0;
#endif

    /**
     * @brief Checks whether the player is playing.
     *
     * @return Returns true if the playback is playing; false otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual bool IsPlaying() = 0;

    /**
     * @brief Returns the value whether single looping is enabled or not .
     *
     * @return Returns true if the playback is single looping; false otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual bool IsLooping() = 0;

    /**
     * @brief Enables single looping of the media playback.
     *
     * @return Returns {@link MSERR_OK} if the single looping is set; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetLooping(bool loop) = 0;

    /**
     * @brief Method to set player callback.
     *
     * @param callback object pointer.
     * @return Returns {@link MSERR_OK} if the playercallback is set; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetPlayerCallback(const std::shared_ptr<PlayerCallback> &callback) = 0;

    /**
     * @brief Sets an extended parameter for player
     *
     * @param format Indicates the string key and value. For details, see {@link Format}
     * @return Returns {@link MSERR_OK} if the parameters are set; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetParameter(const Format &param) = 0;

    /**
     * @brief Releases player resources sync
     *
     * Synchronous release ensures effective release of surfacebuffer
     * but this interface will take a long time (when the engine is not idle state)
     * requiring the caller to design an asynchronous mechanism by itself
     *
     * @return Returns {@link MSERR_OK} if the playback is released; returns an error code defined
     * in {@link media_errors.h} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t ReleaseSync() = 0;
};

class __attribute__((visibility("default"))) PlayerFactory {
public:
#ifdef UNSUPPORT_PLAYER
    static std::shared_ptr<Player> CreatePlayer()
    {
        return nullptr;
    }
#else
    static std::shared_ptr<Player> CreatePlayer();
#endif
private:
    PlayerFactory() = default;
    ~PlayerFactory() = default;
};
} // namespace Media
} // namespace OHOS
#endif // PLAYER_H
