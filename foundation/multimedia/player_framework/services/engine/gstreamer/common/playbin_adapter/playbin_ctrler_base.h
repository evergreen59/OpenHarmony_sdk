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

#ifndef PLAYBIN_CTRLER_BASE_H
#define PLAYBIN_CTRLER_BASE_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <gst/gst.h>
#include "nocopyable.h"
#include "i_playbin_ctrler.h"
#include "state_machine.h"
#include "gst_msg_processor.h"
#include "task_queue.h"

namespace OHOS {
namespace Media {
enum GstPlayerStatus : int32_t {
    GST_PLAYER_STATUS_IDLE = 0,
    GST_PLAYER_STATUS_BUFFERING,
    GST_PLAYER_STATUS_PAUSED,
    GST_PLAYER_STATUS_PLAYING,
};

class PlayBinCtrlerBase
    : public IPlayBinCtrler,
      public StateMachine,
      public std::enable_shared_from_this<PlayBinCtrlerBase> {
public:
    explicit PlayBinCtrlerBase(const PlayBinCreateParam &createParam);
    virtual ~PlayBinCtrlerBase();

    int32_t Init();
    int32_t SetSource(const std::string &url)  override;
    int32_t SetSource(const std::shared_ptr<GstAppsrcWrap> &appsrcWrap) override;
    int32_t Prepare() override;
    int32_t PrepareAsync() override;
    int32_t Play() override;
    int32_t Pause() override;
    int32_t Seek(int64_t timeUs, int32_t seekOption) override;
    int32_t Stop(bool needWait) override;
    int32_t SetRate(double rate) override;
    double GetRate() override;
    int64_t QueryPosition() override;
    int32_t SetLoop(bool loop) override;
    void SetVolume(const float &leftVolume, const float &rightVolume) override;
    void SetAudioInterruptMode(const int32_t interruptMode) override;
    int32_t SetAudioRendererInfo(const uint32_t rendererInfo, const int32_t rendererFlag) override;
    int32_t SelectBitRate(uint32_t bitRate) override;

    void SetElemSetupListener(ElemSetupListener listener) final;
    void SetElemUnSetupListener(ElemSetupListener listener) final;
    void SetAutoPlugSortListener(AutoPlugSortListener listener) final;
    void RemoveGstPlaySinkVideoConvertPlugin() final;
    void SetNotifier(PlayBinMsgNotifier notifier) final;
protected:
    virtual int32_t OnInit() = 0;

    GstPipeline *playbin_ = nullptr;

private:
    class BaseState;
    class IdleState;
    class InitializedState;
    class PreparingState;
    class PreparedState;
    class PlayingState;
    class PausedState;
    class StoppedState;
    class StoppingState;
    class PlaybackCompletedState;

    int32_t EnterInitializedState();
    void ExitInitializedState();
    int32_t PrepareAsyncInternal();
    int32_t SeekInternal(int64_t timeUs, int32_t seekOption);
    int32_t StopInternal();
    int32_t SetRateInternal(double rate);
    void SetupCustomElement();
    GstSeekFlags ChooseSetRateFlags(double rate);
    int32_t SetupSignalMessage();
    int32_t SetupElementUnSetupSignal();
    void QueryDuration();
    void ProcessEndOfStream();
    static void ElementSetup(const GstElement *playbin, GstElement *elem, gpointer userData);
    static void ElementUnSetup(const GstElement *playbin, GstElement *subbin, GstElement *child, gpointer userData);
    static void SourceSetup(const GstElement *playbin, GstElement *elem, gpointer userData);
    static void OnBitRateParseCompleteCb(const GstElement *playbin, uint32_t *bitrateInfo,
        uint32_t bitrateNum, gpointer userData);
    static GValueArray *AutoPlugSort(const GstElement *uriDecoder, GstPad *pad, GstCaps *caps,
        GValueArray *factories, gpointer userData);
    static void OnInterruptEventCb(const GstElement *audioSink, const uint32_t eventType, const uint32_t forceType,
        const uint32_t hintType, gpointer userData);
    static void OnAudioStateEventCb(const GstElement *audioSink, const uint32_t audioState, gpointer userData);
    void SetupInterruptEventCb();
    void SetupAudioStateEventCb();
    void OnElementSetup(GstElement &elem);
    void OnElementUnSetup(GstElement &elem);
    void OnSourceSetup(const GstElement *playbin, GstElement *src,
        const std::shared_ptr<PlayBinCtrlerBase> &playbinCtrl);
    bool OnVideoDecoderSetup(GstElement &elem);
    void OnAppsrcErrorMessageReceived(int32_t errorCode);
    void OnMessageReceived(const InnerMessage &msg);
    void OnSinkMessageReceived(const PlayBinMessage &msg);
    GValueArray *OnAutoPlugSort(GValueArray &factories);
    void ReportMessage(const PlayBinMessage &msg);
    int32_t Reset() noexcept;
    bool IsLiveSource() const;
    int32_t DoInitializeForDataSource();
    void DoInitializeForHttp();
    void HandleCacheCtrl(int32_t percent);
    void HandleCacheCtrlCb(const InnerMessage &msg);
    void HandleCacheCtrlWhenNoBuffering(int32_t percent);
    void HandleCacheCtrlWhenBuffering(int32_t percent);

    std::mutex mutex_;
    std::mutex cacheCtrlMutex_;
    std::mutex listenerMutex_;
    std::mutex appsrcMutex_;
    std::unique_ptr<TaskQueue> msgQueue_;
    PlayBinRenderMode renderMode_ = PlayBinRenderMode::DEFAULT_RENDER;
    PlayBinMsgNotifier notifier_;
    ElemSetupListener elemSetupListener_;
    ElemSetupListener elemUnSetupListener_;
    AutoPlugSortListener autoPlugSortListener_;
    std::shared_ptr<PlayBinSinkProvider> sinkProvider_;
    std::unique_ptr<GstMsgProcessor> msgProcessor_;
    std::string uri_;

    struct SignalInfo {
        GstElement *element;
        gulong signalId;
    };
    std::vector<SignalInfo> signalIds_;
    std::vector<uint32_t> bitRateVec_;
    bool isInitialized_ = false;

    bool isErrorHappened_ = false;
    std::condition_variable preparingCond_;
    std::condition_variable preparedCond_;
    std::condition_variable stoppingCond_;
    
    PlayBinSinkProvider::SinkPtr audioSink_ = nullptr;
    PlayBinSinkProvider::SinkPtr videoSink_ = nullptr;

    int64_t duration_ = 0;
    double rate_ = 0;
    int64_t seekPos_ = 0;
    int64_t lastTime_ = 0;

    bool isSeeking_ = false;
    bool isRating_ = false;
    bool isBuffering_ = false;
    bool isNetWorkPlay_ = false;
    bool isUserSetPlay_ = false;
    bool isUserSetPause_ = false;
    bool isReplay_ = false;
    uint32_t rendererInfo_ = 0;
    int32_t rendererFlag_ = 0;
    int32_t cachePercent_ = 100; // 100% cache percent

    std::atomic<bool> isDuration_ = false;
    std::atomic<bool> enableLooping_ = false;
    std::shared_ptr<GstAppsrcWrap> appsrcWrap_ = nullptr;

    std::shared_ptr<IdleState> idleState_;
    std::shared_ptr<InitializedState> initializedState_;
    std::shared_ptr<PreparingState> preparingState_;
    std::shared_ptr<PreparedState> preparedState_;
    std::shared_ptr<PlayingState> playingState_;
    std::shared_ptr<PausedState> pausedState_;
    std::shared_ptr<StoppedState> stoppedState_;
    std::shared_ptr<StoppingState> stoppingState_;
    std::shared_ptr<PlaybackCompletedState> playbackCompletedState_;
};
} // namespace Media
} // namespace OHOS
#endif // PLAYBIN_CTRLER_BASE_H