/*
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

import account_osAccount from '@ohos.account.osAccount';
import logger from './logger'
import utils from './utils'

const TAG = 'AccountManager';

export class AccountManager {
    async getAccountUserId(accountInfo) : Promise<boolean> {
        let userId = await account_osAccount.getAccountManager().queryCurrentOsAccount();
        if (!utils.isValid(userId)) {
            logger.warn(TAG, 'getAccountUserId queryCurrentOsAccount fail! userId is null');
            return false;
        }
        accountInfo.localId = userId.localId;
        logger.info(TAG, 'getAccountUserId userId.localId=' + userId.localId);
        return true;
    }
}

let accountManager = new AccountManager();

export default accountManager as AccountManager;