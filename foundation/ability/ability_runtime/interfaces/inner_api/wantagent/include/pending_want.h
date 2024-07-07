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

#ifndef OHOS_ABILITY_RUNTIME_PENDING_WANT_H
#define OHOS_ABILITY_RUNTIME_PENDING_WANT_H

#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include "cancel_listener.h"
#include "context/application_context.h"
#include "completed_dispatcher.h"
#include "event_handler.h"
#include "want.h"
#include "want_agent_constant.h"
#include "want_params.h"
#include "want_receiver_stub.h"
#include "want_sender_info.h"
#include "want_sender_stub.h"

namespace OHOS::AbilityRuntime::WantAgent {
class PendingWant final : public std::enable_shared_from_this<PendingWant>, public Parcelable {
public:
    PendingWant() {};
    PendingWant(const sptr<AAFwk::IWantSender> &target);
    PendingWant(const sptr<AAFwk::IWantSender> &target, const sptr<IRemoteObject> whitelistToken);
    virtual ~PendingWant() = default;

    WantAgentConstant::OperationType GetType(const sptr<AAFwk::IWantSender> &target);

    /**
     * Retrieve a PendingWant that will start a new ability.
     *
     * @param context The Context in which this PendingWant should start
     * the ability.
     * @param requestCode Private request code for the sender
     * @param want Want of the ability to be launched.
     * @param flags May be FLAG_ONE_SHOT, FLAG_NO_CREATE},
     * FLAG_CANCEL_CURRENT, FLAG_UPDATE_CURRENT.
     *
     * @return Returns an existing or new PendingWant matching the given
     * parameters.  May return null only if FLAG_NO_CREATE has been
     * supplied.
     */
    static std::shared_ptr<PendingWant> GetAbility(
        const std::shared_ptr<OHOS::AbilityRuntime::ApplicationContext> &context, int requestCode,
        const std::shared_ptr<AAFwk::Want> &want, unsigned int flags);

    /**
     * Retrieve a PendingWant that will start a new ability.
     *
     * @param context The Context in which this PendingWant should start
     * the ability.
     * @param requestCode Private request code for the sender
     * @param want Want of the ability to be launched.
     * @param flags May be FLAG_ONE_SHOT, FLAG_NO_CREATE,
     * FLAG_CANCEL_CURRENT, FLAG_UPDATE_CURRENT.
     *
     * @return Returns ERR_OK If get PendingWant correctly.
     */
    static ErrCode GetAbility(
        const std::shared_ptr<OHOS::AbilityRuntime::ApplicationContext> &context, int requestCode,
        const std::shared_ptr<AAFwk::Want> &want, unsigned int flags,
        const std::shared_ptr<AAFwk::WantParams> &options,
        std::shared_ptr<PendingWant> &pendingWant);

    /**
     * Like GetAbility(Context, int, Want, int)}, but allows an
     * array of Wants to be supplied.  The last Want in the array is
     * taken as the primary key for the PendingWant, like the single Want
     * given to GetAbility(Context, int, Want, int).
     *
     * @param context The Context in which this PendingWant should start
     * the ability.
     * @param requestCode Private request code for the sender
     * @param wants Array of Wants of the abilities to be launched.
     * @param flags May be FLAG_ONE_SHOT, FLAG_NO_CREATE,
     * FLAG_CANCEL_CURRENT, FLAG_UPDATE_CURRENT.
     *
     * @return Returns an existing or new PendingWant matching the given
     * parameters.  May return null only if FLAG_NO_CREATE has been
     * supplied.
     */
    static std::shared_ptr<PendingWant> GetAbilities(
        const std::shared_ptr<OHOS::AbilityRuntime::ApplicationContext> &context, int requestCode,
        std::vector<std::shared_ptr<AAFwk::Want>> &wants, unsigned int flags);

    /**
     * Like GetAbility(Context, int, Want, int)}, but allows an
     * array of Wants to be supplied.  The last Want in the array is
     * taken as the primary key for the PendingWant, like the single Want
     * given to GetAbility(Context, int, Want, int).
     *
     * @param context The Context in which this PendingWant should start
     * the ability.
     * @param requestCode Private request code for the sender
     * @param wants Array of Wants of the abilities to be launched.
     * @param flags May be FLAG_ONE_SHOT, FLAG_NO_CREATE,
     * FLAG_CANCEL_CURRENT, FLAG_UPDATE_CURRENT.
     *
     * @return Returns ERR_OK If get PendingWant correctly.
     */
    static ErrCode GetAbilities(
        const std::shared_ptr<OHOS::AbilityRuntime::ApplicationContext> &context, int requestCode,
        std::vector<std::shared_ptr<AAFwk::Want>> &wants, unsigned int flags,
        const std::shared_ptr<AAFwk::WantParams> &options,
        std::shared_ptr<PendingWant> &pendingWant);

    /**
     * Retrieve a PendingWant that will perform a common event.
     *
     * @param context The Context in which this PendingWant should perform
     * the common event.
     * @param requestCode Private request code for the sender
     * @param want The Want to be common event.
     * @param flags May be FLAG_ONE_SHOT, FLAG_NO_CREATE,
     * FLAG_CANCEL_CURRENT, FLAG_UPDATE_CURRENT,
     * FLAG_IMMUTABLE.
     *
     * @return Returns ERR_OK If get PendingWant correctly.
     */
    static ErrCode GetCommonEvent(
        const std::shared_ptr<OHOS::AbilityRuntime::ApplicationContext> &context, int requestCode,
        const std::shared_ptr<AAFwk::Want> &want, unsigned int flags,
        std::shared_ptr<PendingWant> &pendingWant);

    /**
     * Note that current user will be interpreted at the time the
     * common event is sent, not when the pending want is created.
     */
    static ErrCode GetCommonEventAsUser(
        const std::shared_ptr<OHOS::AbilityRuntime::ApplicationContext> &context,
        int requestCode, const std::shared_ptr<AAFwk::Want> &want, unsigned int flags, int uid,
        std::shared_ptr<PendingWant> &pendingWant);

    /**
     * Retrieve a PendingWant that will start a service.
     *
     * @param context The Context in which this PendingWant should start
     * the service.
     * @param requestCode Private request code for the sender
     * @param want An Want describing the service to be started.
     * @param flags May be FLAG_ONE_SHOT, FLAG_NO_CREATE,
     * FLAG_CANCEL_CURRENT, FLAG_UPDATE_CURRENT,
     * FLAG_IMMUTABLE.
     *
     * @return Returns ERR_OK If get PendingWant correctly.
     */
    static ErrCode GetService(
        const std::shared_ptr<OHOS::AbilityRuntime::ApplicationContext> &context,
        int requestCode, const std::shared_ptr<AAFwk::Want> &want, unsigned int flags,
        std::shared_ptr<PendingWant> &pendingWant);

    /**
     * Retrieve a PendingWant that will start a foreground service.
     *
     * @param context The Context in which this PendingWant should start
     * the service.
     * @param requestCode Private request code for the sender
     * @param want An Want describing the service to be started.
     * @param flags May be FLAG_ONE_SHOT, FLAG_NO_CREATE,
     * FLAG_CANCEL_CURRENT, FLAG_UPDATE_CURRENT,
     * FLAG_IMMUTABLE .
     *
     * @return Returns ERR_OK If get PendingWant correctly.
     */
    static ErrCode GetForegroundService(
        const std::shared_ptr<OHOS::AbilityRuntime::ApplicationContext> &context,
        int requestCode, const std::shared_ptr<AAFwk::Want> &want, unsigned int flags,
        std::shared_ptr<PendingWant> &pendingWant);

    /**
     * @description: Marshals a Want into a Parcel.
     * Fields in the Want are marshalled separately. If any field fails to be marshalled, false is returned.
     * @param parcel Indicates the Parcel object for marshalling.
     * @return Returns true if the marshalling is successful; returns false otherwise.
     */
    virtual bool Marshalling(Parcel &parcel) const;

    /**
     * @description: Unmarshals a Want from a Parcel.
     * Fields in the Want are unmarshalled separately. If any field fails to be unmarshalled, false is returned.
     * @param parcel Indicates the Parcel object for unmarshalling.
     * @return Returns true if the unmarshalling is successful; returns false otherwise.
     */
    static PendingWant *Unmarshalling(Parcel &parcel);

    static ErrCode IsEquals(
        const std::shared_ptr<PendingWant> &targetPendingWant, const std::shared_ptr<PendingWant> &otherPendingWant);

    ErrCode Cancel(const sptr<AAFwk::IWantSender> &target);

    void Send(const sptr<AAFwk::IWantSender> &target);

    void Send(int resultCode, const sptr<AAFwk::IWantSender> &target);

    void Send(int resultCode,
        const std::shared_ptr<AAFwk::Want> &want, const sptr<AAFwk::IWantSender> &target);

    void Send(int resultCode, const sptr<CompletedDispatcher> &onCompleted, const sptr<AAFwk::IWantSender> &target);

    void Send(int resultCode,
        const std::shared_ptr<AAFwk::Want> &want, const sptr<CompletedDispatcher> &onCompleted,
        const sptr<AAFwk::IWantSender> &target);

    void Send(int resultCode,
        const std::shared_ptr<AAFwk::Want> &want, const sptr<CompletedDispatcher> &onCompleted,
        const std::string &requiredPermission, const sptr<AAFwk::IWantSender> &target);

    ErrCode Send(int resultCode, const std::shared_ptr<AAFwk::Want> &want,
        const sptr<CompletedDispatcher> &onCompleted, const std::string &requiredPermission,
        const std::shared_ptr<AAFwk::WantParams> &options, const sptr<AAFwk::IWantSender> &target);

    int SendAndReturnResult(int resultCode, const std::shared_ptr<AAFwk::Want> &want,
        const sptr<CompletedDispatcher> &onCompleted, const std::string &requiredPermission,
        const std::shared_ptr<AAFwk::WantParams> &options, const sptr<AAFwk::IWantSender> &target);

    ErrCode GetBundleName(const sptr<AAFwk::IWantSender> &target, std::string &bundleName);

    ErrCode GetUid(const sptr<AAFwk::IWantSender> &target, int32_t &uid);

    sptr<AAFwk::IWantSender> GetTarget();

    void SetTarget(const sptr<AAFwk::IWantSender> &target);

    void RegisterCancelListener(
        const std::shared_ptr<CancelListener> &cancelListener, const sptr<AAFwk::IWantSender> &target);

    void NotifyCancelListeners(int32_t resultCode);

    void UnregisterCancelListener(
        const std::shared_ptr<CancelListener> &cancelListener, const sptr<AAFwk::IWantSender> &target);

    ErrCode GetHashCode(const sptr<AAFwk::IWantSender> &target, int &code);

    std::shared_ptr<AAFwk::Want> GetWant(const sptr<AAFwk::IWantSender> &target);

    std::shared_ptr<AAFwk::WantSenderInfo> GetWantSenderInfo(const sptr<AAFwk::IWantSender> &target);

    // add return error code interface
    ErrCode GetType(const sptr<AAFwk::IWantSender> &target, int32_t &operType);
    ErrCode GetWant(const sptr<AAFwk::IWantSender> &target, std::shared_ptr<AAFwk::Want> &want);

private:
    std::mutex lock_object;
    sptr<AAFwk::IWantSender> target_;
    sptr<AAFwk::IWantReceiver> cancelReceiver_;
    sptr<IRemoteObject> whitelistToken_;
    std::vector<std::shared_ptr<CancelListener>> cancelListeners_;

    class CancelReceiver : public AAFwk::WantReceiverStub {
    public:
        explicit CancelReceiver(const std::weak_ptr<PendingWant> &outerInstance);
        virtual ~CancelReceiver() = default;

        void Send(const int32_t resultCode) override;
        void PerformReceive(const AAFwk::Want &want, int resultCode, const std::string &data,
            const AAFwk::WantParams &extras, bool serialized, bool sticky, int sendingUser) override;

    private:
        std::weak_ptr<PendingWant> outerInstance_;
    };

    static ErrCode BuildServicePendingWant(
        const std::shared_ptr<OHOS::AbilityRuntime::ApplicationContext> &context,
        int requestCode, const std::shared_ptr<AAFwk::Want> &want, unsigned int flags,
        WantAgentConstant::OperationType serviceKind,
        std::shared_ptr<PendingWant> &pendingWant);
};
}  // namespace OHOS::AbilityRuntime::WantAgent
#endif  // OHOS_ABILITY_RUNTIME_PENDING_WANT_H
