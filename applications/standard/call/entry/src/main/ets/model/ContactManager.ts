/**
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

/**
 * @file: Contact management
 */
import dataShare from '@ohos.data.dataShare';
import dataSharePredicates from '@ohos.data.dataSharePredicates';
import LogUtils from '../common/utils/LogUtils';
import CallManager from '../model/CallManager';
import GlobalThisHelper from '../common/utils/GlobalThisHelper';
import Constants from '../common/utils/Constants';

const TAG = "ContactManager";
const DBbaseUri = 'datashare:///com.ohos.contactsdataability';
const DBUri = DBbaseUri + '/contacts/contact_data';

/**
 * class ContactManager
 */
export default class ContactManager {
  /**
   * get contact info
   *
   * @param { Object } callData -Object
   */
  async getContactInfo(callData) {
    if (callData.contactName) {
      return;
    }
    try {
      const columns = ['id', 'display_name', 'detail_info'];
      const predicates = new dataSharePredicates.DataSharePredicates();
      predicates.equalTo('detail_info', callData.accountNumber);
      predicates.equalTo('is_deleted', 0);
      const dataAbilityHelper = await dataShare.createDataShareHelper(GlobalThisHelper.get<any>(Constants.GLOBALTHIS_CONTEXT), DBbaseUri);
      const resSet = await dataAbilityHelper.query(DBUri, predicates, columns);
      LogUtils.i(TAG, "getContactInfo resSet : " + JSON.stringify(resSet.rowCount))
      if (resSet.rowCount > 0) {
        resSet.goToFirstRow();
        callData.contactName = resSet.getString(resSet.getColumnIndex('display_name'));
        CallManager.getInstance().update(callData);
      }
    } catch (err) {
      LogUtils.i(TAG, "getContactInfo catch err : %s" + JSON.stringify(err))
    }
  }
}