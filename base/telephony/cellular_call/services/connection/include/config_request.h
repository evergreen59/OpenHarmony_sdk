/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef CELLULAR_CALL_CONFIG_REQUEST_H
#define CELLULAR_CALL_CONFIG_REQUEST_H

#include "base_request.h"
#include "ims_feature.h"
#include "module_service_utils.h"
#include "telephony_log_wrapper.h"
#include "telephony_types.h"

namespace OHOS {
namespace Telephony {
class ConfigRequest : BaseRequest {
public:
    /**
     * Set Domain Preference Mode Request
     *
     * @param slotId
     * @param mode
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetDomainPreferenceModeRequest(int32_t slotId, int32_t mode);

    /**
     * Get Domain Preference Mode Request
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t GetDomainPreferenceModeRequest(int32_t slotId);

    /**
     * Set Lte Ims Switch Status Request
     *
     * @param slotId
     * @param active
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetImsSwitchStatusRequest(int32_t slotId, bool active);

    /**
     * Get Lte Ims Switch Status Request
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t GetImsSwitchStatusRequest(int32_t slotId);

    /**
     * Set Ims Config Request
     *
     * @param ImsConfigItem
     * @param value
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetImsConfigRequest(ImsConfigItem item, const std::string &value);

    /**
     * Set Ims Config Request
     *
     * @param ImsConfigItem
     * @param value
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetImsConfigRequest(ImsConfigItem item, int32_t value);

    /**
     * Get Ims Config Request
     *
     * @param ImsConfigItem
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t GetImsConfigRequest(ImsConfigItem item);

    /**
     * Set Ims Feature Value Request
     *
     * @param FeatureType
     * @param value
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetImsFeatureValueRequest(FeatureType type, int32_t value);

    /**
     * Get Ims Feature Value Request
     *
     * @param FeatureType Indicate which feature type to query.
     * @param value Indicate the return value of the query feature type.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t GetImsFeatureValueRequest(FeatureType type, int32_t &value);

    /**
     * Ctrl Camera Request
     *
     * @param cameraId
     * @param callingUid
     * @param callingPid
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t CtrlCameraRequest(const std::u16string &cameraId, int32_t callingUid, int32_t callingPid);

    /**
     * Set Preview Window Request
     *
     * @param x
     * @param y
     * @param z
     * @param width
     * @param height
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetPreviewWindowRequest(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height);

    /**
     * Set Display Window Request
     *
     * @param x
     * @param y
     * @param z
     * @param width
     * @param height
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetDisplayWindowRequest(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height);

    /**
     * Set Camera Zoom Request
     *
     * @param zoomRatio
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetCameraZoomRequest(float zoomRatio);

    /**
     * Set Pause Image Request
     *
     * @param path
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetPauseImageRequest(const std::u16string &path);

    /**
     * Set Device Direction Request
     *
     * @param rotation
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetDeviceDirectionRequest(int32_t rotation);

    /**
     * SetMuteRequest
     *
     * @param slotId
     * @param mute
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetMuteRequest(int32_t slotId, int32_t mute);

    /**
     * GetMuteRequest
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t GetMuteRequest(int32_t slotId);

    /**
     * GetEmergencyCallListRequest
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t GetEmergencyCallListRequest(int32_t slotId);

     /**
     * SetEmergencyCallListRequest
     *
     * @param slotId
     * @param eccVec
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t SetEmergencyCallListRequest(int32_t slotId, std::vector<EmergencyCall>  &eccVec);

    /**
     * Update Ims Capabilities
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param imsCapabilityList Indicates the related ims capability
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t UpdateImsCapabilities(int32_t slotId, const ImsCapabilityList &imsCapabilityList);

private:
    ModuleServiceUtils moduleUtils_;
};
} // namespace Telephony
} // namespace OHOS
#endif // CELLULAR_CALL_CONFIG_REQUEST_H
