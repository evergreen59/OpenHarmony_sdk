/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef TELEPHONY_IMS_CALL_INTERFACE_H
#define TELEPHONY_IMS_CALL_INTERFACE_H

#include "cellular_call_data_struct.h"
#include "ims_call_callback_interface.h"
#include "ims_call_types.h"
#include "ims_feature.h"
#include "iremote_broker.h"

namespace OHOS {
namespace Telephony {
class ImsCallInterface : public IRemoteBroker {
public:
    enum {
        /****************** basic ******************/
        IMS_DIAL = 5000,
        IMS_HANG_UP,
        IMS_REJECT_WITH_REASON,
        IMS_ANSWER,
        IMS_HOLD,
        IMS_UN_HOLD,
        IMS_SWITCH,
        IMS_COMBINE_CONFERENCE,
        IMS_INVITE_TO_CONFERENCE,
        IMS_KICK_OUT_CONFERENCE,
        IMS_UPDATE_CALL_MEDIA_MODE,
        IMS_CALL_STATE_CHANGE,
        IMS_GET_CALL_DATA,
        IMS_GET_LAST_CALL_FAIL_REASON,

        /****************** dtmf rtt ******************/
        IMS_START_DTMF = 5100,
        IMS_SEND_DTMF,
        IMS_STOP_DTMF,
        IMS_START_RTT,
        IMS_STOP_RTT,

        /****************** ims config ******************/
        IMS_SET_DOMAIN_PREFERENCE_MODE = 5200,
        IMS_GET_DOMAIN_PREFERENCE_MODE,
        IMS_SET_SWITCH_STATUS,
        IMS_GET_SWITCH_STATUS,
        IMS_SET_IMS_CONFIG_STRING,
        IMS_SET_IMS_CONFIG_INT,
        IMS_GET_IMS_CONFIG,
        IMS_SET_IMS_FEATURE,
        IMS_GET_IMS_FEATURE,
        IMS_SET_MUTE,
        IMS_GET_MUTE,
        IMS_SET_IMS_REG_ERROR_REPORT,
        IMS_UPDATE_CAPABILITY,

        /****************** video settings ******************/
        IMS_CTRL_CAMERA = 5300,
        IMS_SET_PREVIEW_WINDOW,
        IMS_SET_DISPLAY_WINDOW,
        IMS_SET_CAMERA_ZOOM,
        IMS_SET_PAUSE_IMAGE,
        IMS_SET_DEVICE_DIRECTION,

        /****************** Supplement ******************/
        IMS_GET_CLIP = 5400,
        IMS_SET_CLIP,
        IMS_SET_CLIR,
        IMS_GET_CLIR,
        IMS_SET_CALL_TRANSFER,
        IMS_GET_CALL_TRANSFER,
        IMS_SET_CALL_RESTRICTION,
        IMS_GET_CALL_RESTRICTION,
        IMS_SET_CALL_WAITING,
        IMS_GET_CALL_WAITING,
        IMS_SET_COLR,
        IMS_GET_COLR,
        IMS_SET_COLP,
        IMS_GET_COLP,

        /****************** callback ******************/
        IMS_CALL_REGISTER_CALLBACK = 5500,
    };

    /**
     * IMS dial interface
     *
     * @param ImsCallInfo
     * @param CLIRMode
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t Dial(const ImsCallInfo &callInfo, CLIRMode mode) = 0;

    /**
     * IMS HangUp interface
     *
     * @param ImsCallInfo
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t HangUp(const ImsCallInfo &callInfo) = 0;

    /**
     * IMS Reject with reason interface
     *
     * @param ImsCallInfo
     * @param ImsRejectReason
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t RejectWithReason(const ImsCallInfo &callInfo, const ImsRejectReason &reason) = 0;

    /**
     * IMS Answer interface
     *
     * @param ImsCallInfo
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t Answer(const ImsCallInfo &callInfo) = 0;

    /**
     * IMS HoldCall interface
     *
     * @param slotId
     * @param callType
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t HoldCall(int32_t slotId, int32_t callType) = 0;

    /**
     * IMS UnHoldCall interface
     *
     * @param slotId
     * @param callType
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t UnHoldCall(int32_t slotId, int32_t callType) = 0;

    /**
     * IMS SwitchCall interface
     *
     * @param slotId
     * @param callType
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SwitchCall(int32_t slotId, int32_t callType) = 0;

    /**
     * IMS CombineConference interface
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t CombineConference(int32_t slotId) = 0;

    /**
     * IMS InviteToConference interface
     *
     * @param slotId
     * @param numberList
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t InviteToConference(int32_t slotId, const std::vector<std::string> &numberList) = 0;

    /**
     * IMS KickOutFromConference interface
     *
     * @param slotId
     * @param numberList
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t KickOutFromConference(int32_t slotId, const std::vector<std::string> &numberList) = 0;

    /**
     * IMS UpdateImsCallMode interface
     *
     * @param ImsCallInfo
     * @param ImsCallMode
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t UpdateImsCallMode(const ImsCallInfo &callInfo, ImsCallMode mode) = 0;

    /**
     * Get Ims Calls Data Request
     *
     * @param slotId
     * @param lastCallsDataFlag
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetImsCallsDataRequest(int32_t slotId, int64_t lastCallsDataFlag) = 0;

    /**
     * Get Last Call Fail Reason
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetLastCallFailReason(int32_t slotId) = 0;

    /**
     * IMS StartDtmf interface
     *
     * @param slotId
     * @param cDTMFCode
     * @param index
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t StartDtmf(int32_t slotId, char cDtmfCode, int32_t index) = 0;

    /**
     * IMS SendDtmf interface
     *
     * @param slotId
     * @param cDtmfCode
     * @param index
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SendDtmf(int32_t slotId, char cDtmfCode, int32_t index) = 0;

    /**
     * IMS StopDtmf interface
     *
     * @param slotId
     * @param index
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t StopDtmf(int32_t slotId, int32_t index) = 0;

    /**
     * IMS StartRtt interface
     *
     * @param slotId
     * @param msg
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t StartRtt(int32_t slotId, const std::string &msg) = 0;

    /**
     * IMS StopRtt interface
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t StopRtt(int32_t slotId) = 0;

    /**
     * IMS SetDomainPreferenceMode interface
     *
     * @param slotId
     * @param mode
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetDomainPreferenceMode(int32_t slotId, int32_t mode) = 0;

    /**
     * IMS GetDomainPreferenceMode interface
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetDomainPreferenceMode(int32_t slotId) = 0;

    /**
     * IMS SetImsSwitchStatus interface
     *
     * @param slotId
     * @param active 1: enable ims, 0: disable ims
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetImsSwitchStatus(int32_t slotId, int32_t active) = 0;

    /**
     * IMS GetImsSwitchStatus interface
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetImsSwitchStatus(int32_t slotId) = 0;

    /**
     * IMS SetImsConfig interface
     *
     * @param ImsConfigItem
     * @param value
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetImsConfig(ImsConfigItem item, const std::string &value) = 0;

    /**
     * IMS SetImsConfig interface
     *
     * @param ImsConfigItem
     * @param value
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetImsConfig(ImsConfigItem item, int32_t value) = 0;

    /**
     * IMS GetImsConfig interface
     *
     * @param ImsConfigItem
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetImsConfig(ImsConfigItem item) = 0;

    /**
     * IMS SetImsFeatureValue interface
     *
     * @param FeatureType
     * @param value
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetImsFeatureValue(FeatureType type, int32_t value) = 0;

    /**
     * IMS GetImsFeatureValue interface
     *
     * @param FeatureType Indicate which feature type to query.
     * @param value Indicate the return value of the query feature type.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetImsFeatureValue(FeatureType type, int32_t &value) = 0;

    /**
     * IMS SetMute interface
     *
     * @param mute 0: not mute, 1:mute
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetMute(int32_t slotId, int32_t mute) = 0;

    /**
     * IMS GetMute interface
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t GetMute(int32_t slotId) = 0;

    /**
     * IMS CtrlCamera interface
     *
     * @param cameraId
     * @param callingUid
     * @param callingPid
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t CtrlCamera(const std::u16string &cameraId, int32_t callingUid, int32_t callingPid) = 0;

    /**
     * IMS SetPreviewWindow interface
     *
     * @param x
     * @param y
     * @param z
     * @param width
     * @param height
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetPreviewWindow(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height) = 0;

    /**
     * IMS SetDisplayWindow interface
     *
     * @param x
     * @param y
     * @param z
     * @param width
     * @param height
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetDisplayWindow(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height) = 0;

    /**
     * IMS SetCameraZoom interface
     *
     * @param zoomRatio
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetCameraZoom(float zoomRatio) = 0;

    /**
     * IMS SetPauseImage interface
     *
     * @param path
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetPauseImage(const std::u16string &path) = 0;

    /**
     * IMS SetDeviceDirection interface
     *
     * @param rotation
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SetDeviceDirection(int32_t rotation) = 0;

    /**
     * @brief SetClip IMS SetClip interface
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param action Indicates the action for SetClip,
     * 1, means turn on clip; 0, means turn off clip.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    virtual int32_t SetClip(int32_t slotId, int32_t action) = 0;

    /**
     * @brief GetClip IMS GetClip interface
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    virtual int32_t GetClip(int32_t slotId) = 0;

    /**
     * @brief SetClir IMS SetClir interface
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param action Indicates the action for SetClir,
     * 1, means turn on clir; 0, means turn off clir.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    virtual int32_t SetClir(int32_t slotId, int32_t action) = 0;

    /**
     * @brief GetClir IMS GetClir interface
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    virtual int32_t GetClir(int32_t slotId) = 0;

    /**
     * @brief SetCallTransfer IMS SetCallTransfer interface
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param reason Indicates the reason of the set call transfer.
     * @param mode Indicates the mode of the set call transfer.
     * @param transferNum Indicates the target phone number to transfer calls to.
     * @param classType Indicates a sum of service class for setting call transfer.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    virtual int32_t SetCallTransfer(
        int32_t slotId, int32_t reason, int32_t mode, const std::string &transferNum, int32_t classType) = 0;

    /**
     * @brief GetCallTransfer IMS GetCallTransfer interface
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param reason Indicates the reason of the get call transfer.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    virtual int32_t GetCallTransfer(int32_t slotId, int32_t reason) = 0;

    /**
     * @brief SetCallRestriction IMS SetCallRestriction interface
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param fac Indicates the facility of the set call restriction.
     * @param mode Indicates the mode of the set call restriction.
     * @param pw Indicates password or "" if not required.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    virtual int32_t SetCallRestriction(
        int32_t slotId, const std::string &fac, int32_t mode, const std::string &pw) = 0;

    /**
     * @brief GetCallRestriction IMS GetCallRestriction interface
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param fac Indicates the facility of the get call restriction.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    virtual int32_t GetCallRestriction(int32_t slotId, const std::string &fac) = 0;

    /**
     * @brief SetCallWaiting IMS SetCallWaiting interface
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param activate Indicates the action for SetCallWaiting,
     * true, means turn on CallWaiting; false, means turn off CallWaiting.
     * @param classType Call waiting and conditions +CCWA,
     * the value was {@code ServiceClassType}, See 3GPP TS 22.083.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    virtual int32_t SetCallWaiting(int32_t slotId, bool activate, int32_t classType) = 0;

    /**
     * @brief GetCallWaiting IMS GetCallWaiting interface
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    virtual int32_t GetCallWaiting(int32_t slotId) = 0;

    /**
     * @brief SetColr IMS SetColr interface
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param presentation Indicates the action for SetColr,
     * 1, means turn on colr; 0, means turn off colr.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    virtual int32_t SetColr(int32_t slotId, int32_t presentation) = 0;

    /**
     * @brief GetColr IMS GetColr interface
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    virtual int32_t GetColr(int32_t slotId) = 0;

    /**
     * @brief SetColp IMS SetColp interface
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param action Indicates the action for SetColp,
     * 1, means turn on colp; 0, means turn off colp.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    virtual int32_t SetColp(int32_t slotId, int32_t action) = 0;

    /**
     * @brief GetColp IMS GetColp interface
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    virtual int32_t GetColp(int32_t slotId) = 0;

    /**
     * Register CallBack
     *
     * @param sptr<ImsCallback>
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t RegisterImsCallCallback(const sptr<ImsCallCallbackInterface> &callback) = 0;

    /**
     * Update Ims Capabilities
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param imsCapabilityList Indicates the related ims capability
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t UpdateImsCapabilities(int32_t slotId, const ImsCapabilityList &imsCapabilityList) = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Telephony.ImsCallInterface");
};
} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_IMS_CALL_INTERFACE_H
