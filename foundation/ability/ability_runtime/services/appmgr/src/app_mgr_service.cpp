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

#include "app_mgr_service.h"

#include <chrono>
#include <thread>

#include <nlohmann/json.hpp>
#include <sys/types.h>

#include "datetime_ex.h"
#include "ipc_skeleton.h"
#include "system_ability_definition.h"

#include "app_death_recipient.h"
#include "app_mgr_constants.h"
#include "hilog_wrapper.h"
#include "perf_profile.h"
#include "xcollie/watchdog.h"

#include "permission_constants.h"
#include "permission_verification.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
using namespace std::chrono_literals;
#ifdef ABILITY_COMMAND_FOR_TEST
static const int APP_MS_BLOCK = 65;
#endif
const std::string TASK_ATTACH_APPLICATION = "AttachApplicationTask";
const std::string TASK_APPLICATION_FOREGROUNDED = "ApplicationForegroundedTask";
const std::string TASK_APPLICATION_BACKGROUNDED = "ApplicationBackgroundedTask";
const std::string TASK_APPLICATION_TERMINATED = "ApplicationTerminatedTask";
const std::string TASK_ABILITY_CLEANED = "AbilityCleanedTask";
const std::string TASK_ADD_APP_DEATH_RECIPIENT = "AddAppRecipientTask";
const std::string TASK_CLEAR_UP_APPLICATION_DATA = "ClearUpApplicationDataTask";
const std::string TASK_STARTUP_RESIDENT_PROCESS = "StartupResidentProcess";
const std::string TASK_ADD_ABILITY_STAGE_DONE = "AddAbilityStageDone";
const std::string TASK_START_USER_TEST_PROCESS = "StartUserTestProcess";
const std::string TASK_FINISH_USER_TEST = "FinishUserTest";
const std::string TASK_ATTACH_RENDER_PROCESS = "AttachRenderTask";
}  // namespace

REGISTER_SYSTEM_ABILITY_BY_ID(AppMgrService, APP_MGR_SERVICE_ID, true);

AppMgrService::AppMgrService()
{
    appMgrServiceInner_ = std::make_shared<AppMgrServiceInner>();
    HILOG_INFO("instance created with no para");
    PerfProfile::GetInstance().SetAmsLoadStartTime(GetTickCount());
}

AppMgrService::AppMgrService(const int32_t serviceId, bool runOnCreate) : SystemAbility(serviceId, runOnCreate)
{
    appMgrServiceInner_ = std::make_shared<AppMgrServiceInner>();
    HILOG_INFO("instance created");
    PerfProfile::GetInstance().SetAmsLoadStartTime(GetTickCount());
}

AppMgrService::~AppMgrService()
{
    HILOG_INFO("instance destroyed");
}

void AppMgrService::OnStart()
{
    HILOG_INFO("ready to start service");
    if (appMgrServiceState_.serviceRunningState == ServiceRunningState::STATE_RUNNING) {
        HILOG_WARN("failed to start service since it's already running");
        return;
    }

    ErrCode errCode = Init();
    if (FAILED(errCode)) {
        HILOG_ERROR("init failed, errCode: %{public}08x", errCode);
        return;
    }
    appMgrServiceState_.serviceRunningState = ServiceRunningState::STATE_RUNNING;
    HILOG_INFO("start service success");
    PerfProfile::GetInstance().SetAmsLoadEndTime(GetTickCount());
    PerfProfile::GetInstance().Dump();
}

void AppMgrService::OnStop()
{
    HILOG_INFO("ready to stop service");
    appMgrServiceState_.serviceRunningState = ServiceRunningState::STATE_NOT_START;
    handler_.reset();
    runner_.reset();
    if (appMgrServiceInner_) {
        appMgrServiceInner_->OnStop();
    }
    HILOG_INFO("stop service success");
}

void AppMgrService::SetInnerService(const std::shared_ptr<AppMgrServiceInner> &innerService)
{
    appMgrServiceInner_ = innerService;
}

AppMgrServiceState AppMgrService::QueryServiceState()
{
    if (appMgrServiceInner_) {
        appMgrServiceState_.connectionState = appMgrServiceInner_->QueryAppSpawnConnectionState();
    }
    return appMgrServiceState_;
}

ErrCode AppMgrService::Init()
{
    HILOG_INFO("ready to init");
    // start main thread message loop.
    runner_ = EventRunner::Create(Constants::APP_MGR_SERVICE_NAME);
    if (!runner_) {
        HILOG_ERROR("init failed due to create runner error");
        return ERR_INVALID_OPERATION;
    }
    if (!appMgrServiceInner_) {
        HILOG_ERROR("init failed without inner service");
        return ERR_INVALID_OPERATION;
    }

    handler_ = std::make_shared<AMSEventHandler>(runner_, appMgrServiceInner_);
    appMgrServiceInner_->SetEventHandler(handler_);
    appMgrServiceInner_->Init();

    ErrCode openErr = appMgrServiceInner_->OpenAppSpawnConnection();
    if (FAILED(openErr)) {
        HILOG_WARN("failed to connect to AppSpawnDaemon! errCode: %{public}08x", openErr);
    }
    if (!Publish(this)) {
        HILOG_ERROR("failed to publish app mgr service to systemAbilityMgr");
        return ERR_APPEXECFWK_SERVICE_NOT_CONNECTED;
    }
    amsMgrScheduler_ = new (std::nothrow) AmsMgrScheduler(appMgrServiceInner_, handler_);
    if (!amsMgrScheduler_) {
        HILOG_ERROR("init failed without ams scheduler");
        return ERR_INVALID_OPERATION;
    }
    std::string threadName = Constants::APP_MGR_SERVICE_NAME + "(" + std::to_string(runner_->GetThreadId()) + ")";
    if (HiviewDFX::Watchdog::GetInstance().AddThread(threadName, handler_) != 0) {
        HILOG_ERROR("HiviewDFX::Watchdog::GetInstance AddThread Fail");
    }
    HILOG_INFO("init success");
    return ERR_OK;
}

int32_t AppMgrService::CheckPermission(
    [[maybe_unused]] const int32_t recordId, [[maybe_unused]] const std::string &permission)
{
    HILOG_INFO("check application's permission");

    return ERR_OK;
}

void AppMgrService::AttachApplication(const sptr<IRemoteObject> &app)
{
    if (!IsReady()) {
        HILOG_ERROR("AttachApplication failed, not ready.");
        return;
    }

    pid_t pid = IPCSkeleton::GetCallingPid();
    AddAppDeathRecipient(pid);
    std::function<void()> attachApplicationFunc =
        std::bind(&AppMgrServiceInner::AttachApplication, appMgrServiceInner_, pid, iface_cast<IAppScheduler>(app));
    handler_->PostTask(attachApplicationFunc, TASK_ATTACH_APPLICATION);
}

void AppMgrService::ApplicationForegrounded(const int32_t recordId)
{
    if (!IsReady()) {
        return;
    }
    if (!JudgeSelfCalledByRecordId(recordId)) {
        return;
    }
    std::function<void()> applicationForegroundedFunc =
        std::bind(&AppMgrServiceInner::ApplicationForegrounded, appMgrServiceInner_, recordId);
    handler_->PostTask(applicationForegroundedFunc, TASK_APPLICATION_FOREGROUNDED);
}

void AppMgrService::ApplicationBackgrounded(const int32_t recordId)
{
    if (!IsReady()) {
        return;
    }
    if (!JudgeSelfCalledByRecordId(recordId)) {
        return;
    }
    std::function<void()> applicationBackgroundedFunc =
        std::bind(&AppMgrServiceInner::ApplicationBackgrounded, appMgrServiceInner_, recordId);
    handler_->PostTask(applicationBackgroundedFunc, TASK_APPLICATION_BACKGROUNDED);
}

void AppMgrService::ApplicationTerminated(const int32_t recordId)
{
    if (!IsReady()) {
        return;
    }
    if (!JudgeSelfCalledByRecordId(recordId)) {
        return;
    }
    std::function<void()> applicationTerminatedFunc =
        std::bind(&AppMgrServiceInner::ApplicationTerminated, appMgrServiceInner_, recordId);
    handler_->PostTask(applicationTerminatedFunc, TASK_APPLICATION_TERMINATED);
}

void AppMgrService::AbilityCleaned(const sptr<IRemoteObject> &token)
{
    if (!IsReady()) {
        return;
    }
    std::function<void()> abilityCleanedFunc =
        std::bind(&AppMgrServiceInner::AbilityTerminated, appMgrServiceInner_, token);
    handler_->PostTask(abilityCleanedFunc, TASK_ABILITY_CLEANED);
}

bool AppMgrService::IsReady() const
{
    if (!appMgrServiceInner_) {
        HILOG_ERROR("appMgrServiceInner is null");
        return false;
    }
    if (!handler_) {
        HILOG_ERROR("handler is null");
        return false;
    }
    return true;
}

void AppMgrService::AddAppDeathRecipient(const pid_t pid) const
{
    if (!IsReady()) {
        return;
    }
    sptr<AppDeathRecipient> appDeathRecipient = new AppDeathRecipient();
    appDeathRecipient->SetEventHandler(handler_);
    appDeathRecipient->SetAppMgrServiceInner(appMgrServiceInner_);
    std::function<void()> addAppRecipientFunc =
        std::bind(&AppMgrServiceInner::AddAppDeathRecipient, appMgrServiceInner_, pid, appDeathRecipient);
    handler_->PostTask(addAppRecipientFunc, TASK_ADD_APP_DEATH_RECIPIENT);
}

void AppMgrService::StartupResidentProcess(const std::vector<AppExecFwk::BundleInfo> &bundleInfos)
{
    if (!IsReady()) {
        return;
    }
    pid_t callingPid = IPCSkeleton::GetCallingPid();
    pid_t pid = getpid();
    if (callingPid != pid) {
        HILOG_ERROR("Not this process call.");
        return;
    }
    HILOG_INFO("Notify start resident process");
    std::function <void()> startupResidentProcess =
        std::bind(&AppMgrServiceInner::LoadResidentProcess, appMgrServiceInner_, bundleInfos);
    handler_->PostTask(startupResidentProcess, TASK_STARTUP_RESIDENT_PROCESS);
}

sptr<IAmsMgr> AppMgrService::GetAmsMgr()
{
    return amsMgrScheduler_;
}

int32_t AppMgrService::ClearUpApplicationData(const std::string &bundleName)
{
    auto isSaCall = AAFwk::PermissionVerification::GetInstance()->IsSACall();
    if (!isSaCall) {
        auto isCallingPerm = AAFwk::PermissionVerification::GetInstance()->VerifyCallingPermission(
            AAFwk::PermissionConstants::PERMISSION_CLEAN_APPLICATION_DATA);
        if (!isCallingPerm) {
            HILOG_ERROR("%{public}s: Permission verification failed", __func__);
            return ERR_PERMISSION_DENIED;
        }
    }

    if (!IsReady()) {
        return ERR_INVALID_OPERATION;
    }
    int32_t uid = IPCSkeleton::GetCallingUid();
    pid_t pid = IPCSkeleton::GetCallingPid();
    std::function<void()> clearUpApplicationDataFunc =
        std::bind(&AppMgrServiceInner::ClearUpApplicationData, appMgrServiceInner_, bundleName, uid, pid);
    handler_->PostTask(clearUpApplicationDataFunc, TASK_CLEAR_UP_APPLICATION_DATA);
    return ERR_OK;
}

int32_t AppMgrService::GetAllRunningProcesses(std::vector<RunningProcessInfo> &info)
{
    if (!IsReady()) {
        return ERR_INVALID_OPERATION;
    }
    return appMgrServiceInner_->GetAllRunningProcesses(info);
}

int32_t AppMgrService::GetProcessRunningInfosByUserId(std::vector<RunningProcessInfo> &info, int32_t userId)
{
    if (!IsReady()) {
        return ERR_INVALID_OPERATION;
    }
    return appMgrServiceInner_->GetProcessRunningInfosByUserId(info, userId);
}

int32_t AppMgrService::GetProcessRunningInformation(RunningProcessInfo &info)
{
    if (!IsReady()) {
        return ERR_INVALID_OPERATION;
    }
    return appMgrServiceInner_->GetProcessRunningInformation(info);
}

int32_t AppMgrService::NotifyMemoryLevel(int32_t level)
{
    if (!IsReady()) {
        return ERR_INVALID_OPERATION;
    }
    return appMgrServiceInner_->NotifyMemoryLevel(level);
}

void AppMgrService::AddAbilityStageDone(const int32_t recordId)
{
    if (!IsReady()) {
        return;
    }
    if (!JudgeSelfCalledByRecordId(recordId)) {
        return;
    }
    std::function <void()> addAbilityStageDone =
        std::bind(&AppMgrServiceInner::AddAbilityStageDone, appMgrServiceInner_, recordId);
    handler_->PostTask(addAbilityStageDone, TASK_ADD_ABILITY_STAGE_DONE);
}

int32_t AppMgrService::RegisterApplicationStateObserver(const sptr<IApplicationStateObserver> &observer,
    const std::vector<std::string> &bundleNameList)
{
    HILOG_INFO("%{public}s begin", __func__);
    if (!IsReady()) {
        HILOG_ERROR("%{public}s begin, not ready", __func__);
        return ERR_INVALID_OPERATION;
    }
    return appMgrServiceInner_->RegisterApplicationStateObserver(observer, bundleNameList);
}

int32_t AppMgrService::UnregisterApplicationStateObserver(const sptr<IApplicationStateObserver> &observer)
{
    HILOG_INFO("%{public}s begin", __func__);
    if (!IsReady()) {
        HILOG_ERROR("%{public}s begin, not ready", __func__);
        return ERR_INVALID_OPERATION;
    }
    return appMgrServiceInner_->UnregisterApplicationStateObserver(observer);
}

int32_t AppMgrService::GetForegroundApplications(std::vector<AppStateData> &list)
{
    HILOG_INFO("%{public}s begin", __func__);
    if (!IsReady()) {
        HILOG_ERROR("%{public}s begin, not ready", __func__);
        return ERR_INVALID_OPERATION;
    }
    return appMgrServiceInner_->GetForegroundApplications(list);
}

int AppMgrService::StartUserTestProcess(const AAFwk::Want &want, const sptr<IRemoteObject> &observer,
    const AppExecFwk::BundleInfo &bundleInfo, int32_t userId)
{
    if (!IsReady()) {
        HILOG_ERROR("%{public}s begin, not ready", __func__);
        return ERR_INVALID_OPERATION;
    }
    std::function<void()> startUserTestProcessFunc =
        std::bind(&AppMgrServiceInner::StartUserTestProcess, appMgrServiceInner_, want, observer, bundleInfo, userId);
    handler_->PostTask(startUserTestProcessFunc, TASK_START_USER_TEST_PROCESS);
    return ERR_OK;
}

int AppMgrService::FinishUserTest(const std::string &msg, const int64_t &resultCode, const std::string &bundleName)
{
    if (!IsReady()) {
        HILOG_ERROR("%{public}s begin, not ready", __func__);
        return ERR_INVALID_OPERATION;
    }
    pid_t callingPid = IPCSkeleton::GetCallingPid();
    std::function<void()> finishUserTestProcessFunc =
        std::bind(&AppMgrServiceInner::FinishUserTest, appMgrServiceInner_, msg, resultCode, bundleName, callingPid);
    handler_->PostTask(finishUserTestProcessFunc, TASK_FINISH_USER_TEST);
    return ERR_OK;
}

int AppMgrService::Dump(int fd, const std::vector<std::u16string>& args)
{
    if (!IsReady()) {
        HILOG_ERROR("%{public}s, not ready.", __func__);
        return ERR_APPEXECFWK_HIDUMP_ERROR;
    }

    std::string result;
    Dump(args, result);
    int ret = dprintf(fd, "%s\n", result.c_str());
    if (ret < 0) {
        HILOG_ERROR("%{public}s, dprintf error.", __func__);
        return ERR_APPEXECFWK_HIDUMP_ERROR;
    }
    return ERR_OK;
}

void AppMgrService::Dump(const std::vector<std::u16string>& args, std::string& result) const
{
    auto size = args.size();
    if (size == 0) {
        ShowHelp(result);
        return;
    }

    std::string optionKey = Str16ToStr8(args[0]);
    if (optionKey != "-h") {
        result.append("error: unkown option.\n");
    }
    ShowHelp(result);
}

void AppMgrService::ShowHelp(std::string& result) const
{
    result.append("Usage:\n")
        .append("-h                          ")
        .append("help text for the tool\n");
}

void AppMgrService::ScheduleAcceptWantDone(const int32_t recordId, const AAFwk::Want &want, const std::string &flag)
{
    if (!IsReady()) {
        HILOG_ERROR("%{public}s begin, not ready", __func__);
        return;
    }
    if (!JudgeSelfCalledByRecordId(recordId)) {
        return;
    }
    auto task = [=]() { appMgrServiceInner_->ScheduleAcceptWantDone(recordId, want, flag); };
    handler_->PostTask(task);
}

int AppMgrService::GetAbilityRecordsByProcessID(const int pid, std::vector<sptr<IRemoteObject>> &tokens)
{
    if (!IsReady()) {
        HILOG_ERROR("%{public}s begin, not ready", __func__);
        return ERR_INVALID_OPERATION;
    }
    auto isSaCall = AAFwk::PermissionVerification::GetInstance()->IsSACall();
    if (!isSaCall) {
        HILOG_ERROR("Not SA call.");
        return ERR_INVALID_OPERATION;
    }
    return appMgrServiceInner_->GetAbilityRecordsByProcessID(pid, tokens);
}

int32_t AppMgrService::PreStartNWebSpawnProcess()
{
    HILOG_INFO("PreStartNWebSpawnProcess");
    if (!IsReady()) {
        HILOG_ERROR("PreStartNWebSpawnProcess failed, AppMgrService not ready.");
        return ERR_INVALID_OPERATION;
    }

    return appMgrServiceInner_->PreStartNWebSpawnProcess(IPCSkeleton::GetCallingPid());
}

int32_t AppMgrService::StartRenderProcess(const std::string &renderParam, int32_t ipcFd,
    int32_t sharedFd, pid_t &renderPid)
{
    if (!IsReady()) {
        HILOG_ERROR("StartRenderProcess failed, AppMgrService not ready.");
        return ERR_INVALID_OPERATION;
    }

    return appMgrServiceInner_->StartRenderProcess(IPCSkeleton::GetCallingPid(),
        renderParam, ipcFd, sharedFd, renderPid);
}

void AppMgrService::AttachRenderProcess(const sptr<IRemoteObject> &scheduler)
{
    HILOG_DEBUG("AttachRenderProcess called.");
    if (!IsReady()) {
        HILOG_ERROR("AttachRenderProcess failed, not ready.");
        return;
    }

    auto pid = IPCSkeleton::GetCallingPid();
    auto fun = std::bind(&AppMgrServiceInner::AttachRenderProcess,
        appMgrServiceInner_, pid, iface_cast<IRenderScheduler>(scheduler));
    handler_->PostTask(fun, TASK_ATTACH_RENDER_PROCESS);
}

int32_t AppMgrService::GetRenderProcessTerminationStatus(pid_t renderPid, int &status)
{
    if (!IsReady()) {
        HILOG_ERROR("GetRenderProcessTerminationStatus failed, AppMgrService not ready.");
        return ERR_INVALID_OPERATION;
    }

    return appMgrServiceInner_->GetRenderProcessTerminationStatus(renderPid, status);
}

int32_t AppMgrService::GetConfiguration(Configuration& config)
{
    if (!IsReady()) {
        HILOG_ERROR("GetConfiguration failed, AppMgrService not ready.");
        return ERR_INVALID_OPERATION;
    }
    config = *(appMgrServiceInner_->GetConfiguration());
    return ERR_OK;
}

int32_t AppMgrService::UpdateConfiguration(const Configuration& config)
{
    if (!IsReady()) {
        HILOG_ERROR("UpdateConfiguration failed, AppMgrService not ready.");
        return ERR_INVALID_OPERATION;
    }
    return appMgrServiceInner_->UpdateConfiguration(config);
}

int32_t AppMgrService::RegisterConfigurationObserver(const sptr<IConfigurationObserver> &observer)
{
    if (!IsReady()) {
        HILOG_ERROR("RegisterConfigurationObserver failed, AppMgrService not ready.");
        return ERR_INVALID_OPERATION;
    }
    return appMgrServiceInner_->RegisterConfigurationObserver(observer);
}

int32_t AppMgrService::UnregisterConfigurationObserver(const sptr<IConfigurationObserver> &observer)
{
    if (!IsReady()) {
        HILOG_ERROR("UnregisterConfigurationObserver failed, AppMgrService not ready.");
        return ERR_INVALID_OPERATION;
    }
    return appMgrServiceInner_->UnregisterConfigurationObserver(observer);
}

#ifdef ABILITY_COMMAND_FOR_TEST
int AppMgrService::BlockAppService()
{
    HILOG_DEBUG("%{public}s begin", __func__);
    if (!IsReady()) {
        return ERR_INVALID_OPERATION;
    }
    auto task = [=]() {
        while (1) {
            HILOG_DEBUG("%{public}s begin block app service", __func__);
            std::this_thread::sleep_for(APP_MS_BLOCK*1s);
        }
    };
    handler_->PostTask(task);
    return ERR_OK;
}
#endif

bool AppMgrService::GetAppRunningStateByBundleName(const std::string &bundleName)
{
    if (!IsReady()) {
        HILOG_ERROR("AppMgrService is not ready.");
        return false;
    }

    return appMgrServiceInner_->GetAppRunningStateByBundleName(bundleName);
}

int32_t AppMgrService::NotifyLoadRepairPatch(const std::string &bundleName, const sptr<IQuickFixCallback> &callback)
{
    if (!IsReady()) {
        HILOG_ERROR("AppMgrService is not ready.");
        return ERR_INVALID_OPERATION;
    }
    auto isSaCall = AAFwk::PermissionVerification::GetInstance()->IsSACall();
    if (!isSaCall) {
        return ERR_INVALID_OPERATION;
    }
    return appMgrServiceInner_->NotifyLoadRepairPatch(bundleName, callback);
}

int32_t AppMgrService::NotifyHotReloadPage(const std::string &bundleName, const sptr<IQuickFixCallback> &callback)
{
    if (!IsReady()) {
        HILOG_ERROR("AppMgrService is not ready.");
        return ERR_INVALID_OPERATION;
    }
    auto isSaCall = AAFwk::PermissionVerification::GetInstance()->IsSACall();
    if (!isSaCall) {
        return ERR_INVALID_OPERATION;
    }
    return appMgrServiceInner_->NotifyHotReloadPage(bundleName, callback);
}

#ifdef BGTASKMGR_CONTINUOUS_TASK_ENABLE
int32_t AppMgrService::SetContinuousTaskProcess(int32_t pid, bool isContinuousTask)
{
    if (!IsReady()) {
        HILOG_ERROR("AppMgrService is not ready.");
        return ERR_INVALID_OPERATION;
    }

    return appMgrServiceInner_->SetContinuousTaskProcess(pid, isContinuousTask);
}
#endif

int32_t AppMgrService::NotifyUnLoadRepairPatch(const std::string &bundleName, const sptr<IQuickFixCallback> &callback)
{
    if (!IsReady()) {
        HILOG_ERROR("AppMgrService is not ready.");
        return ERR_INVALID_OPERATION;
    }
    auto isSaCall = AAFwk::PermissionVerification::GetInstance()->IsSACall();
    if (!isSaCall) {
        return ERR_INVALID_OPERATION;
    }
    return appMgrServiceInner_->NotifyUnLoadRepairPatch(bundleName, callback);
}

bool AppMgrService::JudgeSelfCalledByRecordId(int32_t recordId)
{
    auto isSaCall = AAFwk::PermissionVerification::GetInstance()->IsSACall();
    if (isSaCall) {
        return true;
    }

    if (appMgrServiceInner_ == nullptr) {
        return false;
    }

    auto callingTokenId = IPCSkeleton::GetCallingTokenID();
    std::shared_ptr<AppRunningRecord> appRecord = appMgrServiceInner_->GetAppRunningRecordByAppRecordId(recordId);
    if (appRecord == nullptr || ((appRecord->GetApplicationInfo())->accessTokenId) != callingTokenId) {
        HILOG_ERROR("Is not self, not enabled");
        return false;
    }

    return true;
}
}  // namespace AppExecFwk
}  // namespace OHOS
