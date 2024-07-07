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

#ifndef RECORDER_H
#define RECORDER_H

#include <cstdint>
#include <string>
#include "format.h"
#include "surface.h"
#include "av_common.h"

namespace OHOS {
namespace Media {
/**
 * @brief Enumerates video source types.
 *
 * @since 1.0
 * @version 1.0
 */
enum VideoSourceType : int32_t {
    /** Unsupported App Usage. */
    /** YUV video data provided through {@link Surface} */
    VIDEO_SOURCE_SURFACE_YUV = 0,
    /** Raw encoded data provided through {@link Surface} */
    VIDEO_SOURCE_SURFACE_ES,
    /** RGBA video data provided through {@link Surface} */
    VIDEO_SOURCE_SURFACE_RGBA,
    /** Invalid value */
    VIDEO_SOURCE_BUTT
};

/**
 * @brief Enumerates audio source types.
 *
 * @since 1.0
 * @version 1.0
 */
enum AudioSourceType : int32_t {
    /** Invalid audio source */
    AUDIO_SOURCE_INVALID = -1,
    /** Default audio source */
    AUDIO_SOURCE_DEFAULT = 0,
    /** Microphone */
    AUDIO_MIC = 1,
};

/**
 * Unsupported app usage.
 * @brief Enumerates data source types.
 *
 * @since 1.0
 * @version 1.0
 */
enum DataSourceType : int32_t {
    /** meta data source */
    METADATA = 0
};

/**
 * @brief Enumerates output format types.
 *
 * @since 3.1
 * @version 3.1
 */
enum OutputFormatType : int32_t {
    /** Default format */
    FORMAT_DEFAULT = 0,
    /** MPEG4 format */
    FORMAT_MPEG_4 = 2,
    /** M4A format */
    FORMAT_M4A = 6,
    /** BUTT */
    FORMAT_BUTT,
};

/**
 * @brief Enumerates video codec formats.
 *
 * @since 3.1
 * @version 3.1
 */
enum VideoCodecFormat : int32_t {
    /** Default format */
    VIDEO_DEFAULT = 0,
    /** H.264 */
    H264 = 2,
    /** MPEG4 */
    MPEG4 = 6,
    VIDEO_CODEC_FORMAT_BUTT,
};

/**
 * @brief Enumerates audio codec formats.
 *
 * @since 3.1
 * @version 3.1
 */
enum AudioCodecFormat : int32_t {
    /** Default format */
    AUDIO_DEFAULT = 0,
    /** Advanced Audio Coding Low Complexity (AAC-LC) */
    AAC_LC = 3,
    /** Invalid value */
    AUDIO_CODEC_FORMAT_BUTT,
};

/**
 * Unsupported App Usage.
 * @brief Enumerates file split types.
 *
 * @since 1.0
 * @version 1.0
 */
enum FileSplitType : int32_t {
    /** Delayed/Backward split */
    FILE_SPLIT_POST = 0,
    /** Advanced/Forward split */
    FILE_SPLIT_PRE,
    /** Normal split */
    FILE_SPLIT_NORMAL,
    /** Invalid value */
    FILE_SPLIT_BUTT,
};

/**
 * @brief Enumerates recording information types.
 *
 * @since 1.0
 * @version 1.0
 */
enum RecorderInfoType : int32_t {
    /**
     * The recording duration is reaching the threshold specified by {@link SetMaxDuration}. This type of
     * information is reported when only one second or 10% is left to reach the allowed duration.
     */
    RECORDER_INFO_MAX_DURATION_APPROACHING = 0,
    /**
     * The recorded file size is reaching the threshold specified by {@link SetMaxFileSize}. This type of
     * information is reported when only 100 KB or 10% is left to reach the allowed size.
     */
    RECORDER_INFO_MAX_FILESIZE_APPROACHING,
    /**
     * The threshold specified by {@link SetMaxDuration} is reached, and the recording ends.
     * Before calling {@link SetNextOutputFile}, you must close the file.
     */
    RECORDER_INFO_MAX_DURATION_REACHED,
    /**
     * The threshold specified by {@link SetMaxFileSize} is reached, and the recording ends.
     * Before calling {@link SetNextOutputFile}, you must close the file.
     */
    RECORDER_INFO_MAX_FILESIZE_REACHED,
    /** Recording started for the next output file. */
    RECORDER_INFO_NEXT_OUTPUT_FILE_STARTED,
    /** Manual file split completed. */
    RECORDER_INFO_FILE_SPLIT_FINISHED,
    /** The start time position of the recording file is not supported. */
    RECORDER_INFO_FILE_START_TIME_MS,
    /** Next file fd is needed but not set. */
    RECORDER_INFO_NEXT_FILE_FD_NOT_SET,

    /** warnings, and the err code passed by the 'extra' argument, the code see "MediaServiceErrCode". */
    RECORDER_INFO_INTERNEL_WARNING,

     /** extend info start,The extension information code agreed upon by the plug-in and
         the application will be transparently transmitted by the service. */
    RECORDER_INFO_EXTEND_START = 0X10000,
};

/**
 * @brief Enumerates recording error types.
 *
 * @since 1.0
 * @version 1.0
 */
enum RecorderErrorType : int32_t {
    /* internal errors, error code passed by the errorCode, and definition see "MediaServiceErrCode" */
    RECORDER_ERROR_INTERNAL,

     /** extend error start,The extension error code agreed upon by the plug-in and
         the application will be transparently transmitted by the service. */
    RECORDER_ERROR_EXTEND_START = 0X10000,
};

/**
 * @brief Provides listeners for recording errors and information events.
 *
 * @since 1.0
 * @version 1.0
 */
class RecorderCallback {
public:
    virtual ~RecorderCallback() = default;

    /**
     * @brief Called when an error occurs during recording. This callback is used to report recording errors.
     *
     * @param errorType Indicates the error type. For details, see {@link RecorderErrorType}.
     * @param errorCode Indicates the error code.
     * @since 1.0
     * @version 1.0
     */
    virtual void OnError(RecorderErrorType errorType, int32_t errorCode) = 0;

    /**
     * @brief Called when an information event occurs during recording. This callback is used to report recording
     * information.
     *
     * @param type Indicates the information type. For details, see {@link RecorderInfoType}.
     * @param extra Indicates other information, for example, the start time position of a recording file.
     * @since 1.0
     * @version 1.0
     */
    virtual void OnInfo(int32_t type, int32_t extra) = 0;
};

/**
 * @brief Provides functions for audio and video recording.
 *
 * @since 1.0
 * @version 1.0
 */
class Recorder {
public:
    virtual ~Recorder() = default;

    /**
     * @brief Sets a video source for recording.
     *
     * If this function is not called, the output file does not contain the video track.
     *
     * @param source Indicates the video source type. For details, see {@link VideoSourceType}.
     * @param sourceId Indicates the video source ID. The value <b>-1</b> indicates an invalid ID and the setting fails.
     *
     * @return Returns {@link MSERR_OK} if the setting is successful; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetVideoSource(VideoSourceType source, int32_t &sourceId) = 0;

    /**
     * @brief Sets the audio source for recording.
     *
     * If this function is not called, the output file does not contain the audio track.
     *
     * @param source Indicates the audio source type. For details, see {@link AudioSourceType}.
     * @param sourceId Indicates the audio source ID. The value <b>-1</b> indicates an invalid ID and the setting fails.
     * @return Returns {@link MSERR_OK} if the setting is successful; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetAudioSource(AudioSourceType source, int32_t &sourceId) = 0;

    /**
     * Unsupported App Usage.
     * @brief Sets a data source for recording.
     *
     * If this function is not called, the output file does not contain the data track.
     *
     * @param sourceId Indicates the data source ID. The value <b>-1</b> indicates an invalid ID and the setting fails.
     *
     * @return Returns {@link MSERR_OK} if the setting is successful; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetDataSource(DataSourceType dataType, int32_t &sourceId) = 0;

    /**
     * @brief Sets the output file format.
     *
     * This function must be called before {@link Prepare} and after after all required sources have been set. After
     * this function called, no more source settings allowed.
     *
     * @param format Indicates the output file format. For details, see {@link OutputFormatType}.
     * @return Returns {@link MSERR_OK} if the setting is successful; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetOutputFormat(OutputFormatType format) = 0;

    /**
     * @brief Sets a video encoder for recording.
     *
     * If this function is not called, the output file does not contain the video track when the video source is
     * YUV or RGB.
     * This function must be called after {@link SetOutputFormat} but before {@link Prepare}.
     *
     * @param sourceId Indicates the video source ID, which can be obtained from {@link SetVideoSource}.
     * @param encoder Indicates the video encoder to set. For details, see {@link VideoCodecFormat}.
     * @return Returns {@link MSERR_OK} if the setting is successful; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetVideoEncoder(int32_t sourceId, VideoCodecFormat encoder) = 0;

    /**
     * @brief Sets the width and height of the video to record.
     *
     * This function must be called after {@link SetOutputFormat} but before {@link Prepare}.
     *
     * @param sourceId Indicates the video source ID, which can be obtained from {@link SetVideoSource}.
     * @param width Indicates the video width to set.
     * @param height Indicates the video height to set.
     * @return Returns {@link MSERR_OK} if the setting is successful; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetVideoSize(int32_t sourceId, int32_t width, int32_t height) = 0;

    /**
     * Unsupported App Usage.
     * @brief Sets the frame rate of the video to record.
     *
     * This function must be called after {@link SetOutputFormat} but before {@link Prepare}.
     *
     * @param sourceId Indicates the video source ID, which can be obtained from {@link SetVideoSource}.
     * @param frameRate Indicates the frame rate to set.
     * @return Returns {@link MSERR_OK} if the setting is successful; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetVideoFrameRate(int32_t sourceId, int32_t frameRate) = 0;

    /**
     * @brief Sets the encoding bit rate of the video to record.
     *
     * This function must be called after {@link SetOutputFormat} but before {@link Prepare}.
     *
     * @param sourceId Indicates the video source ID, which can be obtained from {@link SetVideoSource}.
     * @param rate Indicates the encoding bit rate to set.
     * @return Returns {@link MSERR_OK} if the setting is successful; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetVideoEncodingBitRate(int32_t sourceId, int32_t rate) = 0;

    /**
     * Unsupported App Usage.
     * @brief Sets the video capture rate.
     *
     * This function must be called after {@link SetOutputFormat} but before {@link Prepare}. It is valid when the
     * video source is YUV or RGB.
     *
     * @param sourceId Indicates the video source ID, which can be obtained from {@link SetVideoSource}.
     * @param fps Indicates the rate at which frames are captured per second.
     * @return Returns {@link MSERR_OK} if the setting is successful; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetCaptureRate(int32_t sourceId, double fps) = 0;

    /**
     * @brief Obtains the surface of the video source. This function can only be called after {@link Prepare} and
     * before {@link Stop}.
     *
     * @param sourceId Indicates the video source ID, which can be obtained from {@link SetVideoSource}.
     * @return Returns the pointer to the surface.
     * @since 1.0
     * @version 1.0
     */
    virtual sptr<OHOS::Surface> GetSurface(int32_t sourceId) = 0;

    /**
     * @brief Sets an audio encoder for recording.
     *
     * If this function is not called, the output file does not contain the audio track.
     * This function must be called after {@link SetOutputFormat} but before {@link Prepare}.
     *
     * @param sourceId Indicates the audio source ID, which can be obtained from {@link SetAudioSource}.
     * @param encoder Indicates the audio encoder to set.
     * @return Returns {@link MSERR_OK} if the setting is successful; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetAudioEncoder(int32_t sourceId, AudioCodecFormat encoder) = 0;

    /**
     * @brief Sets the audio sampling rate for recording.
     *
     * This function must be called after {@link SetOutputFormat} but before {@link Prepare}.
     *
     * @param sourceId Indicates the audio source ID, which can be obtained from {@link SetAudioSource}.
     * @param rate Indicates the sampling rate of the audio per second.
     * @return Returns {@link MSERR_OK} if the setting is successful; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetAudioSampleRate(int32_t sourceId, int32_t rate) = 0;

    /**
     * @brief Sets the number of audio channels to record.
     *
     * This function must be called after {@link SetOutputFormat} but before {@link Prepare}.
     *
     * @param sourceId Indicates the audio source ID, which can be obtained from {@link SetAudioSource}.
     * @param num Indicates the number of audio channels to set.
     * @return Returns {@link MSERR_OK} if the setting is successful; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetAudioChannels(int32_t sourceId, int32_t num) = 0;

    /**
     * @brief Sets the encoding bit rate of the audio to record.
     *
     * This function must be called after {@link SetOutputFormat} but before {@link Prepare}.
     *
     * @param sourceId Indicates the audio source ID, which can be obtained from {@link SetAudioSource}.
     * @param bitRate Indicates the audio encoding bit rate, in bit/s.
     * @return Returns {@link MSERR_OK} if the setting is successful; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetAudioEncodingBitRate(int32_t sourceId, int32_t bitRate) = 0;

    /**
     * Unsupported App Usage.
     * @brief Sets the maximum duration of a recorded file, in seconds.
     *
     * This function must be called after {@link SetOutputFormat} but before {@link Prepare}. If the setting is valid,
     * {@link RECORDER_INFO_MAX_DURATION_APPROACHING} is reported through {@link OnInfo} in the {@link RecorderCallback}
     * class when only one second or 10% is left to reach the allowed duration.
     * If the recording output file is set by calling {@link SetOutputFile}, call {@link SetNextOutputFile} to set the
     * next output file. Otherwise, the current file will be overwritten when the allowed duration is reached.
     *
     * @param duration Indicates the maximum recording duration to set. If the value is <b>0</b> or a negative number,
     * a failure message is returned. The default duration is 60s.
     * @return Returns {@link MSERR_OK} if the setting is successful; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetMaxDuration(int32_t duration) = 0;

    /**
     * Unsupported App Usage.
     * @brief Sets the maximum size of a recorded file, in bytes.
     *
     * This function must be called after {@link SetOutputFormat} but before {@link Prepare}. If the setting is valid,
     * {@link RECORDER_INFO_MAX_DURATION_APPROACHING} is reported through {@link OnInfo} in the {@link RecorderCallback}
     * class when only 100 KB or 10% is left to reach the allowed size.
     * If the recording output file is set by calling {@link SetOutputFile}, call {@link SetNextOutputFile} to set the
     * next output file. Otherwise, when the allowed size is reached, the current file will be overwritten. If
     * <b>MaxDuration</b> is also set by calling {@link SetMaxDuration}, <b>MaxDuration</b> or <b>MaxFileSize</b>
     * prevails depending on which of them is first satisfied.
     *
     * @param size Indicates the maximum file size to set. If the value is <b>0</b> or a negative number, a failure
     * message is returned.
     * By default, the maximum size of a single file supported by the current file system is used as the limit.
     * @return Returns {@link MSERR_OK} if the setting is successful; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetMaxFileSize(int64_t size) = 0;

    /**
     * @brief Sets the file descriptor (FD) of the output file.
     *
     * This function must be called after {@link SetOutputFormat} but before {@link Prepare}
     *
     * @param fd Indicates the FD of the file.
     * @return Returns {@link MSERR_OK} if the setting is successful; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetOutputFile(int32_t fd) = 0;

    /**
     * Unsupported App Usage.
     * @brief Sets the FD of the next output file.
     *
     * If {@link SetOutputFile} is successful, call this function to set the FD of the next output file after
     * {@link RECORDER_INFO_MAX_DURATION_APPROACHING} or {@link RECORDER_INFO_MAX_FILESIZE_APPROACHING} is received.
     *
     * @param fd Indicates the FD of the next output file.
     * @return Returns {@link MSERR_OK} if the setting is successful; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetNextOutputFile(int32_t fd) = 0;

    /**
     * @brief Set and store the geodata (latitude and longitude) in the output file.
     * This method should be called before prepare(). The geodata is stored in udta box if
     * the output format is OutputFormat.THREE_GPP or OutputFormat.MPEG_4,
     * and is ignored for other output formats.
     *
     * @param latitude float: latitude in degrees. Its value must be in the range [-90, 90].
     * @param longitude float: longitude in degrees. Its value must be in the range [-180, 180].
     * @since openharmony 3.1
     * @version 1.0
     */
    virtual void SetLocation(float latitude, float longitude) = 0;

    /**
     * @brief set the orientation hint in output file, and for the file to playback. mp4 support.
     * the range of orientation should be {0, 90, 180, 270}, default is 0.
     *
     * @param rotation int32_t: should be {0, 90, 180, 270}, default is 0.
     * @since openharmony 3.1
     * @version 1.0
     */
    virtual void SetOrientationHint(int32_t rotation) = 0;

    /**
     * @brief Registers a recording listener.
     *
     * This function must be called after {@link SetOutputFormat} but before {@link Prepare}
     *
     * @param callback Indicates the recording listener to register. For details, see {@link RecorderCallback}.
     * @return Returns {@link MSERR_OK} if the setting is successful; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetRecorderCallback(const std::shared_ptr<RecorderCallback> &callback) = 0;

    /**
     * @brief Prepares for recording.
     *
     * This function must be called before {@link Start}.
     *
     * @return Returns {@link MSERR_OK} if the preparation is successful; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t Prepare() = 0;

    /**
     * @brief Starts recording.
     *
     * This function must be called after {@link Prepare}.
     *
     * @return Returns {@link MSERR_OK} if the recording is started; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t Start() = 0;

    /**
     * @brief Pauses recording.
     *
     * After {@link Start} is called, you can call this function to pause recording.
     *
     * @return Returns {@link MSERR_OK} if the recording is paused; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t Pause() = 0;

    /**
    * @brief Resumes recording.
    *
    * You can call this function to resume recording after {@link Pause} is called.
     *
     * @return Returns {@link MSERR_OK} if the recording is resumed; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t Resume() = 0;

    /**
     * @brief Stops recording.
     *
     * @param block Indicates the stop mode. The value <b>true</b> indicates that the processing stops after all caches
     * are processed, and <b>false</b> indicates that the processing stops immediately and all caches are discarded.
     * After the recording stopped, all sources and parameters must be set again to restore recording. The function is
     * like to {@link Reset}, except that the block parameter is allowed to be specified.
     * @return Returns {@link MSERR_OK} if the recording is stopped; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t Stop(bool block) = 0;

    /**
     * @brief Resets the recording.
     *
     * After the function is called, add a recording source by calling {@link SetVideoSource} or {@link SetAudioSource},
     * set related parameters, and call {@link Start} to start recording again after {@link Prepare} is called.
     *
     * @return Returns {@link MSERR_OK} if the recording is stopped; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t Reset() = 0;

    /**
     * @brief Releases recording resources. After this function called, none of interfaces of {@link Recorder}
     * can be used.
     *
     * @return Returns {@link MSERR_OK} if the recording is stopped; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t Release() = 0;

    /**
     * Unsupported App Usage.
     * @brief Manually splits a video.
     *
     * This function must be called after {@link Start}. After this function is called, the file is split based on the
     * manual split type. After the manual split is complete, the initial split type is used. This function can be
     * called again only after {@link RECORDER_INFO_FILE_SPLIT_FINISHED} is reported.
     *
     * @param type Indicates the file split type. For details, see {@link FileSplitType}.
     * @param timestamp Indicates the file split timestamp. This parameter is not supported currently and can be set to
     * <b>-1</b>. The recording module splits a file based on the call time.
     * @param duration Indicates the duration for splitting the file.
     * @return Returns {@link MSERR_OK} if the recording is stopped; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetFileSplitDuration(FileSplitType type, int64_t timestamp, uint32_t duration) = 0;

    /**
     * @brief Sets an extended parameter for recording, for example, {@link RECORDER_PRE_CACHE_DURATION}.
     *
     * This function must be called after {@link Prepare}.
     *
     * @param sourceId Indicates the data source ID. The value <b>-1</b> indicates all sources.
     * @param format Indicates the string key and value. For details, see {@link Format} and
     * {@link RECORDER_PRE_CACHE_DURATION}.
     * @return Returns {@link MSERR_OK} if the recording is stopped; returns an error code otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetParameter(int32_t sourceId, const Format &format) = 0;
};

class __attribute__((visibility("default"))) RecorderFactory {
public:
#ifdef UNSUPPORT_RECORDER
    static std::shared_ptr<Recorder> CreateRecorder()
    {
        return nullptr;
    }
#else
    static std::shared_ptr<Recorder> CreateRecorder();
#endif
private:
    RecorderFactory() = default;
    ~RecorderFactory() = default;
};
} // namespace Media
} // namespace OHOS
#endif // RECORDER_H
