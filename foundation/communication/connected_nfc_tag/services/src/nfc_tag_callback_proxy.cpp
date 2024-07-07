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
#include "nfc_tag_callback_proxy.h"
#include "define.h"
#include "log.h"

namespace OHOS {
namespace NFC {
NfcTagCallBackProxy::NfcTagCallBackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<INfcTagCallback>(impl)
{}

void NfcTagCallBackProxy::OnNotify(int nfcRfState)
{
    HILOGD("NfcTagCallBackProxy::OnNotify");
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return;
    }
    data.WriteInt32(0);
    data.WriteInt32(nfcRfState);

    int error = Remote()->SendRequest(CMD_ON_NFC_TAG_NOTIFY, data, reply, option);
    if (error != ERR_NONE) {
        HILOGE("Set Attr(%{public}d) failed,error code is %{public}d", CMD_ON_NFC_TAG_NOTIFY, error);
        return;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        HILOGE("notify CMD_ON_NFC_TAG_NOTIFY state change failed!");
    }
    return;
}
}  // namespace NFC
}  // namespace OHOS