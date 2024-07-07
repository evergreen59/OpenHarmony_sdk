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

#include "cell_info.h"
#include "telephony_log_wrapper.h"
#include "network_search_manager.h"
#include "network_search_notify.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t ZERO_VALUE = 0;
constexpr int32_t SIGNAL_LEVEL_INVALID = 0;
constexpr int32_t SIGNAL_RSSI_MAXIMUM = -1;
constexpr int32_t SIGNAL_FOUR_BARS = 4;
constexpr int32_t SIGNAL_FIVE_BARS = 5;
const int32_t *GSM_SIGNAL_THRESHOLD = SignalInformation::GSM_SIGNAL_THRESHOLD_5BAR;
const int32_t *CDMA_SIGNAL_THRESHOLD = SignalInformation::CDMA_SIGNAL_THRESHOLD_5BAR;
const int32_t *LTE_SIGNAL_THRESHOLD = SignalInformation::LTE_SIGNAL_THRESHOLD_5BAR;
const int32_t *WCDMA_SIGNAL_THRESHOLD = SignalInformation::WCDMA_SIGNAL_THRESHOLD_5BAR;
const int32_t *TD_SCDMA_SIGNAL_THRESHOLD = SignalInformation::TD_SCDMA_SIGNAL_THRESHOLD_5BAR;
const int32_t *NR_SIGNAL_THRESHOLD = SignalInformation::NR_SIGNAL_THRESHOLD_5BAR;
int32_t CellInfo::signalBar_ = SIGNAL_FIVE_BARS;

const std::map<RatType, CellInfo::CallInfoFunc> CellInfo::memberFuncMap_ = {
    {RatType::NETWORK_TYPE_GSM, &CellInfo::ProcessNeighboringCellGsm},
    {RatType::NETWORK_TYPE_CDMA, &CellInfo::ProcessNeighboringCellCdma},
    {RatType::NETWORK_TYPE_WCDMA, &CellInfo::ProcessNeighboringCellWcdma},
    {RatType::NETWORK_TYPE_TDSCDMA, &CellInfo::ProcessNeighboringCellTdscdma},
    {RatType::NETWORK_TYPE_LTE, &CellInfo::ProcessNeighboringCellLte},
    {RatType::NETWORK_TYPE_NR, &CellInfo::ProcessNeighboringCellNr}};

CellInfo::CellInfo(std::weak_ptr<NetworkSearchManager> networkSearchManager, int32_t slotId)
    : networkSearchManager_(networkSearchManager), slotId_(slotId)
{
    InitCellSignalBar();
}

void CellInfo::InitCellSignalBar(const int32_t bar)
{
    if (bar == SIGNAL_FOUR_BARS) {
        GSM_SIGNAL_THRESHOLD = SignalInformation::GSM_SIGNAL_THRESHOLD_4BAR;
        CDMA_SIGNAL_THRESHOLD = SignalInformation::CDMA_SIGNAL_THRESHOLD_4BAR;
        LTE_SIGNAL_THRESHOLD = SignalInformation::LTE_SIGNAL_THRESHOLD_4BAR;
        WCDMA_SIGNAL_THRESHOLD = SignalInformation::WCDMA_SIGNAL_THRESHOLD_4BAR;
        TD_SCDMA_SIGNAL_THRESHOLD = SignalInformation::TD_SCDMA_SIGNAL_THRESHOLD_4BAR;
        NR_SIGNAL_THRESHOLD = SignalInformation::NR_SIGNAL_THRESHOLD_4BAR;
        signalBar_ = SIGNAL_FOUR_BARS;
    } else {
        GSM_SIGNAL_THRESHOLD = SignalInformation::GSM_SIGNAL_THRESHOLD_5BAR;
        CDMA_SIGNAL_THRESHOLD = SignalInformation::CDMA_SIGNAL_THRESHOLD_5BAR;
        LTE_SIGNAL_THRESHOLD = SignalInformation::LTE_SIGNAL_THRESHOLD_5BAR;
        WCDMA_SIGNAL_THRESHOLD = SignalInformation::WCDMA_SIGNAL_THRESHOLD_5BAR;
        TD_SCDMA_SIGNAL_THRESHOLD = SignalInformation::TD_SCDMA_SIGNAL_THRESHOLD_5BAR;
        NR_SIGNAL_THRESHOLD = SignalInformation::NR_SIGNAL_THRESHOLD_5BAR;
        signalBar_ = SIGNAL_FIVE_BARS;
    }
}

void CellInfo::ProcessNeighboringCellInfo(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGD("CellInfo::ProcessNeighboringCellInfo cell info start...... slotId:%{public}d", slotId_);
    std::lock_guard<std::mutex> lock(mutex_);
    if (event == nullptr) {
        TELEPHONY_LOGE("CellInfo::ProcessNeighboringCellInfo event is nullptr slotId:%{public}d", slotId_);
        return;
    }

    CellListNearbyInfo *cellInfo = event->GetSharedObject<CellListNearbyInfo>().get();
    if (cellInfo == nullptr) {
        TELEPHONY_LOGE("CellInfo::ProcessNeighboringCellInfo rssi is nullptr slotId:%{public}d", slotId_);
        return;
    }

    int32_t cellSize = cellInfo->itemNum >= CellInformation::MAX_CELL_NUM ? CellInformation::MAX_CELL_NUM :
                                                                            cellInfo->itemNum;
    if (cellSize > 0) {
        currentCellInfo_ = nullptr;
        cellInfos_.clear();
    } else {
        return;
    }

    TELEPHONY_LOGI(
        "CellInfo::ProcessNeighboringCellInfo cell size:%{public}d, cur size:%{public}zu slotId:%{public}d",
        cellInfo->itemNum, cellInfos_.size(), slotId_);
    std::vector<CellNearbyInfo> cell = cellInfo->cellNearbyInfo;
    for (int32_t i = 0; i < cellSize; ++i) {
        int32_t type = cell[i].ratType;
        auto itFunc = memberFuncMap_.find(static_cast<RatType>(type));
        if (itFunc != memberFuncMap_.end()) {
            auto memberFunc = itFunc->second;
            if (memberFunc != nullptr) {
                (this->*memberFunc)(&cell[i]);
            }
        }
    }
}

void CellInfo::ProcessCurrentCellInfo(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGD("CellInfo::ProcessCurrentCellInfo cell info start... slotId:%{public}d", slotId_);
    std::lock_guard<std::mutex> lock(mutex_);
    if (event == nullptr) {
        TELEPHONY_LOGE("CellInfo::ProcessCurrentCellInfo event is nullptr slotId:%{public}d", slotId_);
        return;
    }
    CellListCurrentInfo *cellInfoList = event->GetSharedObject<CellListCurrentInfo>().get();
    if (cellInfoList == nullptr) {
        TELEPHONY_LOGE("CellInfo::ProcessCurrentCellInfo rssi is nullptr");
        return;
    }

    int32_t cellSize = cellInfoList->itemNum >= CellInformation::MAX_CELL_NUM ? CellInformation::MAX_CELL_NUM :
                                                                                cellInfoList->itemNum;
    if (cellSize <= 0) {
        TELEPHONY_LOGE("CellInfo::ProcessCurrentCellInfo rssi is nullptr");
        return;
    }

    TELEPHONY_LOGI("CellInfo::ProcessCurrentCellInfo cell size:%{public}d, cur size:%{public}zu",
        cellInfoList->itemNum, cellInfos_.size());
    currentCellInfo_ = nullptr;
    cellInfos_.clear();
    std::vector<CurrentCellInfo> cell = cellInfoList->cellCurrentInfo;
    for (int32_t i = 0; i < cellSize; ++i) {
        CurrentCellInfo currentCell = cell[i];
        ProcessCurrentCell(&currentCell);
    }
    NotifyCellInfoUpdated();
}

void CellInfo::NotifyCellInfoUpdated() const
{
    if (cellInfos_.size() > 0) {
        DelayedSingleton<NetworkSearchNotify>::GetInstance()->NotifyCellInfoUpdated(slotId_, cellInfos_);
    } else {
        TELEPHONY_LOGI("CellInfo::NotifyCellInfoUpdated no notify slotId:%{public}d", slotId_);
    }
}

void CellInfo::UpdateCellLocation(int32_t techType, int32_t cellId, int32_t lac)
{
    CellInformation::CellType type = ConvertTechToCellType(static_cast<RadioTech>(techType));
    if (type == CellInformation::CellType::CELL_TYPE_NONE) {
        TELEPHONY_LOGE("CellInfo::UpdateCellLocation type error");
        return;
    }

    for (auto &cell : cellInfos_) {
        if (cell->GetNetworkType() == type) {
            if (ProcessCellLocation(cell, type, cellId, lac)) {
                UpdateSignalLevel(cell, type);
                NotifyCellInfoUpdated();
                cell->SetIsCamped(true);
                currentCellInfo_ = cell;
            }
            break;
        }
    }
}

bool CellInfo::ProcessCellLocation(
    sptr<CellInformation> &cell, CellInformation::CellType type, int32_t cellId, int32_t lac)
{
    switch (type) {
        case CellInformation::CellType::CELL_TYPE_GSM: {
            GsmCellInformation *gsm = reinterpret_cast<GsmCellInformation *>(cell.GetRefPtr());
            if (gsm->GetCellId() != cellId || gsm->GetLac() != lac) {
                gsm->UpdateLocation(cellId, lac);
                return true;
            }
            break;
        }
        case CellInformation::CellType::CELL_TYPE_LTE: {
            LteCellInformation *lte = reinterpret_cast<LteCellInformation *>(cell.GetRefPtr());
            if (lte->GetCellId() != cellId || lte->GetTac() != lac) {
                lte->UpdateLocation(cellId, lac);
                return true;
            }
            break;
        }
        case CellInformation::CellType::CELL_TYPE_WCDMA: {
            WcdmaCellInformation *wcdma = reinterpret_cast<WcdmaCellInformation *>(cell.GetRefPtr());
            if (wcdma->GetCellId() != cellId || wcdma->GetLac() != lac) {
                wcdma->UpdateLocation(cellId, lac);
                return true;
            }
            break;
        }
        case CellInformation::CellType::CELL_TYPE_TDSCDMA: {
            TdscdmaCellInformation *tdscdma = reinterpret_cast<TdscdmaCellInformation *>(cell.GetRefPtr());
            if (tdscdma->GetCellId() != cellId || tdscdma->GetLac() != lac) {
                tdscdma->UpdateLocation(cellId, lac);
                return true;
            }
            break;
        }
        case CellInformation::CellType::CELL_TYPE_NR: {
            NrCellInformation *nr = reinterpret_cast<NrCellInformation *>(cell.GetRefPtr());
            if (nr->GetCellId() != cellId || nr->GetTac() != lac) {
                nr->UpdateLocation(cellId, lac);
                return true;
            }
            break;
        }
        default:
            TELEPHONY_LOGE("CellInfo::ProcessCellLocation type error");
            break;
    }
    return false;
}

void CellInfo::UpdateSignalLevel(sptr<CellInformation> &cell, CellInformation::CellType cellType)
{
    if (cellType == CellInformation::CellType::CELL_TYPE_NONE || cell->GetNetworkType() != cellType) {
        TELEPHONY_LOGE("CellInfo::UpdateSignalLevel type error");
        return;
    }

    std::shared_ptr<NetworkSearchManager> nsm = networkSearchManager_.lock();
    if (nsm == nullptr) {
        TELEPHONY_LOGE("CellInfo::UpdateSignalLevel nsm is nullptr slotId:%{public}d", slotId_);
        return;
    }

    std::vector<sptr<SignalInformation>> signals;
    nsm->GetSignalInfoList(slotId_, signals);
    int32_t signalLevel = 0;
    int32_t signalIntensity = 0;
    for (const auto &v : signals) {
        if (ConvertToCellType(v->GetNetworkType()) == cellType) {
            TELEPHONY_LOGI("CellInfo::UpdateSignalLevel signal level %{public}d slotId:%{public}d",
                v->GetSignalLevel(), slotId_);
            signalLevel = v->GetSignalLevel();
            signalIntensity = v->GetSignalIntensity();
            break;
        }
    }
    cell->SetSignalLevel(signalLevel);
    cell->SetSignalIntensity(signalIntensity);
}

CellInformation::CellType CellInfo::ConvertToCellType(SignalInformation::NetworkType signalType) const
{
    switch (signalType) {
        case SignalInformation::NetworkType::GSM:
            return CellInformation::CellType::CELL_TYPE_GSM;
        case SignalInformation::NetworkType::WCDMA:
            return CellInformation::CellType::CELL_TYPE_WCDMA;
        case SignalInformation::NetworkType::LTE:
            return CellInformation::CellType::CELL_TYPE_LTE;
        case SignalInformation::NetworkType::CDMA:
            return CellInformation::CellType::CELL_TYPE_CDMA;
        case SignalInformation::NetworkType::TDSCDMA:
            return CellInformation::CellType::CELL_TYPE_TDSCDMA;
        case SignalInformation::NetworkType::NR:
            return CellInformation::CellType::CELL_TYPE_NR;
        default:
            return CellInformation::CellType::CELL_TYPE_NONE;
    }
}

CellInformation::CellType CellInfo::ConvertRatToCellType(int ratType) const
{
    switch (ratType) {
        case RatType::NETWORK_TYPE_GSM:
            return CellInformation::CellType::CELL_TYPE_GSM;
        case RatType::NETWORK_TYPE_WCDMA:
            return CellInformation::CellType::CELL_TYPE_WCDMA;
        case RatType::NETWORK_TYPE_LTE:
            return CellInformation::CellType::CELL_TYPE_LTE;
        case RatType::NETWORK_TYPE_CDMA:
            return CellInformation::CellType::CELL_TYPE_CDMA;
        case RatType::NETWORK_TYPE_TDSCDMA:
            return CellInformation::CellType::CELL_TYPE_TDSCDMA;
        case RatType::NETWORK_TYPE_NR:
            return CellInformation::CellType::CELL_TYPE_NR;
        default:
            return CellInformation::CellType::CELL_TYPE_NONE;
    }
}

CellInformation::CellType CellInfo::ConvertTechToCellType(RadioTech techType) const
{
    switch (techType) {
        case RadioTech::RADIO_TECHNOLOGY_GSM:
            return CellInformation::CellType::CELL_TYPE_GSM;
        case RadioTech::RADIO_TECHNOLOGY_WCDMA:
        case RadioTech::RADIO_TECHNOLOGY_HSPAP:
        case RadioTech::RADIO_TECHNOLOGY_HSPA:
            return CellInformation::CellType::CELL_TYPE_WCDMA;
        case RadioTech::RADIO_TECHNOLOGY_LTE:
        case RadioTech::RADIO_TECHNOLOGY_LTE_CA:
            return CellInformation::CellType::CELL_TYPE_LTE;
        case RadioTech::RADIO_TECHNOLOGY_TD_SCDMA:
            return CellInformation::CellType::CELL_TYPE_TDSCDMA;
        case RadioTech::RADIO_TECHNOLOGY_1XRTT:
        case RadioTech::RADIO_TECHNOLOGY_EVDO:
        case RadioTech::RADIO_TECHNOLOGY_EHRPD:
            return CellInformation::CellType::CELL_TYPE_CDMA;
        case RadioTech::RADIO_TECHNOLOGY_NR:
            return CellInformation::CellType::CELL_TYPE_NR;
        default:
            return CellInformation::CellType::CELL_TYPE_NONE;
    }
}

bool CellInfo::ProcessCurrentCell(CurrentCellInfo *cellInfo)
{
    bool ret = false;
    switch (cellInfo->ratType) {
        case RatType::NETWORK_TYPE_GSM: {
            ret = ProcessCurrentCellGsm(cellInfo);
            break;
        }
        case RatType::NETWORK_TYPE_LTE: {
            ret = ProcessCurrentCellLte(cellInfo);
            break;
        }
        case RatType::NETWORK_TYPE_WCDMA: {
            ret = ProcessCurrentCellWcdma(cellInfo);
            break;
        }
        case RatType::NETWORK_TYPE_TDSCDMA: {
            ret = ProcessCurrentCellTdscdma(cellInfo);
            break;
        }
        case RatType::NETWORK_TYPE_CDMA: {
            ret = ProcessCurrentCellCdma(cellInfo);
            break;
        }
        case RatType::NETWORK_TYPE_NR: {
            ret = ProcessCurrentCellNr(cellInfo);
            break;
        }
        default: {
            TELEPHONY_LOGI("CellInfo::ProcessCurrentCell error rat type:%{public}d slotId:%{public}d",
                cellInfo->ratType, slotId_);
            return false;
        }
    }
    if (!ret) {
        TELEPHONY_LOGI(
            "CellInfo::ProcessCurrentCell currentCellInfo is null or cell info no change slotId:%{public}d",
            slotId_);
        return false;
    }
    return true;
}

bool CellInfo::ProcessNeighboringCellGsm(CellNearbyInfo *cellInfo)
{
    sptr<GsmCellInformation> cell = new GsmCellInformation;
    if (cell != nullptr) {
        int32_t &arfcn = cellInfo->ServiceCellParas.gsm.arfcn;
        int32_t &cellId = cellInfo->ServiceCellParas.gsm.cellId;
        int32_t &bsic = cellInfo->ServiceCellParas.gsm.bsic;
        int32_t &lac = cellInfo->ServiceCellParas.gsm.lac;
        cell->Init(0, 0, cellId);
        cell->SetGsmParam(bsic, lac, arfcn);
        cellInfos_.emplace_back(cell);
        TELEPHONY_LOGI("CellInfo::ProcessNeighboringCellGsm arfcn:%{private}d cellId:%{private}d"
            "bsic:%{private}d lac:%{private}d slotId:%{public}d",
            arfcn, cellId, bsic, lac, slotId_);
        return true;
    }
    return false;
}

bool CellInfo::ProcessNeighboringCellLte(CellNearbyInfo *cellInfo)
{
    sptr<LteCellInformation> cell = new LteCellInformation;
    if (cell != nullptr) {
        int32_t &arfcn = cellInfo->ServiceCellParas.lte.arfcn;
        int32_t pci = cellInfo->ServiceCellParas.lte.pci;
        cell->Init(0, 0, 0);
        cell->SetLteParam(pci, 0, arfcn);
        cellInfos_.emplace_back(cell);
        TELEPHONY_LOGI("CellInfo::ProcessLte arfcn:%{private}d pci:%{private}d slotId:%{public}d", arfcn, pci, slotId_);
        return true;
    }
    return false;
}

bool CellInfo::ProcessNeighboringCellWcdma(CellNearbyInfo *cellInfo)
{
    sptr<WcdmaCellInformation> cell = new WcdmaCellInformation;
    if (cell != nullptr) {
        int32_t &arfcn = cellInfo->ServiceCellParas.wcdma.arfcn;
        int32_t psc = cellInfo->ServiceCellParas.wcdma.psc;
        cell->Init(0, 0, 0);
        cell->SetWcdmaParam(psc, 0, arfcn);
        cellInfos_.emplace_back(cell);
        TELEPHONY_LOGI(
            "CellInfo::ProcessWcdma arfcn:%{private}d psc:%{private}d slotId:%{public}d", arfcn, psc, slotId_);
        return true;
    }
    return false;
}

bool CellInfo::ProcessNeighboringCellCdma(CellNearbyInfo *cellInfo)
{
    sptr<CdmaCellInformation> cell = new CdmaCellInformation;
    if (cell != nullptr) {
        int32_t &baseId = cellInfo->ServiceCellParas.cdma.baseId;
        int32_t &longitude = cellInfo->ServiceCellParas.cdma.longitude;
        int32_t &latitude = cellInfo->ServiceCellParas.cdma.latitude;
        int32_t &networkId = cellInfo->ServiceCellParas.cdma.networkId;
        int32_t &systemId = cellInfo->ServiceCellParas.cdma.systemId;
        cell->Init(0, 0, 0);
        cell->SetCdmaParam(baseId, latitude, longitude, networkId, systemId);
        cellInfos_.emplace_back(cell);
        TELEPHONY_LOGI(
            "CellInfo::ProcessCdma baseId:%{private}d psc:%{private}d slotId:%{public}d", baseId, systemId, slotId_);
        return true;
    }
    return false;
}

bool CellInfo::ProcessNeighboringCellTdscdma(CellNearbyInfo *cellInfo)
{
    sptr<TdscdmaCellInformation> cell = new TdscdmaCellInformation;
    if (cell != nullptr) {
        int32_t &arfcn = cellInfo->ServiceCellParas.tdscdma.arfcn;
        int32_t &cpid = cellInfo->ServiceCellParas.tdscdma.cpid;
        int32_t &lac = cellInfo->ServiceCellParas.tdscdma.lac;
        cell->Init(0, 0, 0);
        cell->SetTdscdmaParam(cpid, lac, arfcn);
        cellInfos_.emplace_back(cell);
        TELEPHONY_LOGI(
            "CellInfo::ProcessTdscdma arfcn:%{private}d psc:%{private}d slotId:%{public}d", arfcn, cpid, slotId_);
        return true;
    }
    return false;
}

bool CellInfo::ProcessNeighboringCellNr(CellNearbyInfo *cellInfo)
{
    sptr<NrCellInformation> cell = new NrCellInformation;
    if (cell != nullptr && cellInfo != nullptr) {
        int32_t &nrArfcn = cellInfo->ServiceCellParas.nr.nrArfcn;
        int32_t &pci = cellInfo->ServiceCellParas.nr.pci;
        int32_t &tac = cellInfo->ServiceCellParas.nr.tac;
        int64_t &nci = cellInfo->ServiceCellParas.nr.nci;
        cell->Init(0, 0, 0);
        cell->SetNrParam(nrArfcn, pci, tac, nci);
        cellInfos_.emplace_back(cell);
        TELEPHONY_LOGI("CellInfo::ProcessNeighboringCellNr arfcn:%{private}d pci:%{private}d slotId:%{public}d",
            nrArfcn, pci, slotId_);
        return true;
    }
    return false;
}

bool CellInfo::ProcessCurrentCellGsm(CurrentCellInfo *cellInfo)
{
    sptr<GsmCellInformation> cell = new GsmCellInformation;
    if (cell != nullptr) {
        int32_t &arfcn = cellInfo->ServiceCellParas.gsm.arfcn;
        int32_t &cellId = cellInfo->ServiceCellParas.gsm.cellId;
        int32_t &bsic = cellInfo->ServiceCellParas.gsm.bsic;
        int32_t &lac = cellInfo->ServiceCellParas.gsm.lac;
        int32_t &rxlev = cellInfo->ServiceCellParas.gsm.rxlev;
        rxlev = ZERO_VALUE - rxlev;
        cell->Init(cellInfo->mcc, cellInfo->mnc, cellId);
        cell->SetGsmParam(bsic, lac, arfcn);
        cell->SetSignalIntensity(rxlev);
        cell->SetIsCamped(true);
        int32_t level = GetCurrentSignalLevelGsm(rxlev);
        cell->SetSignalLevel(level);
        currentCellInfo_ = cell;
        cellInfos_.emplace_back(cell);
        TELEPHONY_LOGI(
            "CellInfo::ProcessCurrentCellGsm arfcn:%{private}d cellId:%{private}d"
            "bsic:%{private}d lac:%{private}d rxlev:%{public}d slotId:%{public}d",
            arfcn, cellId, bsic, lac, rxlev, slotId_);
        return true;
    }
    return false;
}

bool CellInfo::ProcessCurrentCellLte(CurrentCellInfo *cellInfo)
{
    sptr<LteCellInformation> cell = new LteCellInformation;
    if (cell != nullptr) {
        int32_t &arfcn = cellInfo->ServiceCellParas.lte.arfcn;
        int32_t &pci = cellInfo->ServiceCellParas.lte.pci;
        int32_t &cellId = cellInfo->ServiceCellParas.lte.cellId;
        int32_t &tac = cellInfo->ServiceCellParas.lte.tac;
        int32_t &rsrp = cellInfo->ServiceCellParas.lte.rsrp;
        rsrp = ZERO_VALUE - rsrp;
        cell->Init(cellInfo->mcc, cellInfo->mnc, cellId);
        cell->SetLteParam(pci, tac, arfcn);
        cell->SetSignalIntensity(rsrp);
        cell->SetIsCamped(true);
        int32_t level = GetCurrentSignalLevelLte(rsrp);
        cell->SetSignalLevel(level);
        currentCellInfo_ = cell;
        cellInfos_.emplace_back(cell);
        TELEPHONY_LOGI(
            "CellInfo::ProcessCurrentCellLte arfcn:%{private}d pci:%{private}d rsrp:%{public}d slotId:%{public}d",
            arfcn, pci, rsrp, slotId_);
        return true;
    }
    return false;
}

bool CellInfo::ProcessCurrentCellWcdma(CurrentCellInfo *cellInfo)
{
    sptr<WcdmaCellInformation> cell = new WcdmaCellInformation;
    if (cell != nullptr) {
        int32_t &arfcn = cellInfo->ServiceCellParas.wcdma.arfcn;
        int32_t &psc = cellInfo->ServiceCellParas.wcdma.psc;
        int32_t &cellId = cellInfo->ServiceCellParas.wcdma.cellId;
        int32_t &lac = cellInfo->ServiceCellParas.wcdma.lac;
        int32_t &rscp = cellInfo->ServiceCellParas.wcdma.rscp;
        rscp = ZERO_VALUE - rscp;
        cell->Init(cellInfo->mcc, cellInfo->mnc, cellId);
        cell->SetWcdmaParam(psc, lac, arfcn);
        cell->SetSignalIntensity(rscp);
        cell->SetIsCamped(true);
        int32_t level = GetCurrentSignalLevelWcdma(rscp);
        cell->SetSignalLevel(level);
        currentCellInfo_ = cell;
        cellInfos_.emplace_back(cell);
        TELEPHONY_LOGI(
            "CellInfo::ProcessCurrentCellWcdma arfcn:%{private}d psc:%{private}d rscp:%{public}d", arfcn, psc, rscp);
        return true;
    }
    return false;
}

bool CellInfo::ProcessCurrentCellCdma(CurrentCellInfo *cellInfo)
{
    sptr<CdmaCellInformation> cell = new CdmaCellInformation;
    if (cell != nullptr) {
        int32_t &baseId = cellInfo->ServiceCellParas.cdma.baseId;
        int32_t &longitude = cellInfo->ServiceCellParas.cdma.longitude;
        int32_t &latitude = cellInfo->ServiceCellParas.cdma.latitude;
        int32_t &networkId = cellInfo->ServiceCellParas.cdma.networkId;
        int32_t &systemId = cellInfo->ServiceCellParas.cdma.systemId;
        int32_t &pilotStrength = cellInfo->ServiceCellParas.cdma.pilotStrength;
        pilotStrength = ZERO_VALUE - pilotStrength;
        cell->Init(cellInfo->mcc, cellInfo->mnc, baseId);
        cell->SetCdmaParam(baseId, latitude, longitude, networkId, systemId);
        cell->SetSignalIntensity(pilotStrength);
        cell->SetIsCamped(true);
        int32_t level = GetCurrentSignalLevelCdma(pilotStrength);
        cell->SetSignalLevel(level);
        currentCellInfo_ = cell;
        cellInfos_.emplace_back(cell);
        TELEPHONY_LOGI(
            "CellInfo::ProcessCurrentCellCdma baseId:%{private}d networkId:%{private}d pilotStrength:%{public}d",
            baseId, networkId, pilotStrength);
        return true;
    }
    return false;
}

bool CellInfo::ProcessCurrentCellTdscdma(CurrentCellInfo *cellInfo)
{
    sptr<TdscdmaCellInformation> cell = new TdscdmaCellInformation;
    if (cell != nullptr) {
        int32_t &arfcn = cellInfo->ServiceCellParas.tdscdma.arfcn;
        int32_t &cpid = cellInfo->ServiceCellParas.tdscdma.cpid;
        int32_t &cellId = cellInfo->ServiceCellParas.tdscdma.cellId;
        int32_t &lac = cellInfo->ServiceCellParas.tdscdma.lac;
        int32_t &rscp = cellInfo->ServiceCellParas.tdscdma.rscp;
        rscp = ZERO_VALUE - rscp;
        cell->Init(cellInfo->mcc, cellInfo->mnc, cellId);
        cell->SetTdscdmaParam(cpid, lac, arfcn);
        cell->SetSignalIntensity(rscp);
        cell->SetIsCamped(true);
        int32_t level = GetCurrentSignalLevelTdscdma(rscp);
        cell->SetSignalLevel(level);
        currentCellInfo_ = cell;
        cellInfos_.emplace_back(cell);
        TELEPHONY_LOGI("CellInfo::ProcessCurrentCellTdscdma arfcn:%{private}d pci:%{private}d slotId:%{public}d", arfcn,
            cpid, slotId_);
        return true;
    }
    return false;
}

bool CellInfo::ProcessCurrentCellNr(CurrentCellInfo *cellInfo)
{
    sptr<NrCellInformation> cell = new NrCellInformation;
    if (cell != nullptr && cellInfo != nullptr) {
        int32_t &nrArfcn = cellInfo->ServiceCellParas.nr.nrArfcn;
        int32_t &pci = cellInfo->ServiceCellParas.nr.pci;
        int32_t &tac = cellInfo->ServiceCellParas.nr.tac;
        int64_t &nci = cellInfo->ServiceCellParas.nr.nci;
        cell->Init(cellInfo->mcc, cellInfo->mnc, 0);
        cell->SetNrParam(nrArfcn, pci, tac, nci);
        cell->SetIsCamped(true);
        currentCellInfo_ = cell;
        cellInfos_.emplace_back(cell);

        TELEPHONY_LOGI("CellInfo::ProcessCurrentCellNr arfcn:%{private}d pci:%{private}d slotId:%{public}d",
            nrArfcn, pci, slotId_);
        return true;
    }
    return false;
}

int32_t CellInfo::GetCurrentSignalLevelGsm(int32_t rxlev)
{
    int32_t level = SIGNAL_LEVEL_INVALID;
    if (rxlev >= SIGNAL_RSSI_MAXIMUM) {
        TELEPHONY_LOGE("CellInfo::GetCurrentSignalLevelGsm Value is Invalid.");
        return level;
    }
    for (int32_t i = signalBar_; i >= 0; --i) {
        if (rxlev >= GSM_SIGNAL_THRESHOLD[i]) {
            level = i;
            break;
        }
    }
    return level;
}

int32_t CellInfo::GetCurrentSignalLevelLte(int32_t rsrp)
{
    int32_t level = SIGNAL_LEVEL_INVALID;
    if (rsrp >= SIGNAL_RSSI_MAXIMUM) {
        TELEPHONY_LOGE("CellInfo::GetCurrentSignalLevelLte Value is Invalid.");
        return level;
    }
    for (int32_t i = signalBar_; i >= 0; --i) {
        if (rsrp >= LTE_SIGNAL_THRESHOLD[i]) {
            level = i;
            break;
        }
    }
    return level;
}

int32_t CellInfo::GetCurrentSignalLevelWcdma(int32_t rscp)
{
    int32_t level = SIGNAL_LEVEL_INVALID;
    if (rscp >= SIGNAL_RSSI_MAXIMUM) {
        TELEPHONY_LOGE("CellInfo::GetCurrentSignalLevelWcdma Value is Invalid.");
        return level;
    }
    for (int32_t i = signalBar_; i >= 0; --i) {
        if (rscp >= WCDMA_SIGNAL_THRESHOLD[i]) {
            level = i;
            break;
        }
    }
    return level;
}

int32_t CellInfo::GetCurrentSignalLevelCdma(int32_t pilotStrength)
{
    int32_t level = SIGNAL_LEVEL_INVALID;
    if (pilotStrength >= SIGNAL_RSSI_MAXIMUM) {
        TELEPHONY_LOGE("CellInfo::GetCurrentSignalLevelCdma Value is Invalid.");
        return level;
    }
    for (int32_t i = signalBar_; i >= 0; --i) {
        if (pilotStrength >= CDMA_SIGNAL_THRESHOLD[i]) {
            level = i;
            break;
        }
    }
    return level;
}

int32_t CellInfo::GetCurrentSignalLevelTdscdma(int32_t rscp)
{
    int32_t level = SIGNAL_LEVEL_INVALID;
    if (rscp >= SIGNAL_RSSI_MAXIMUM) {
        TELEPHONY_LOGE("CellInfo::GetCurrentSignalLevelTdscdma Value is Invalid.");
        return level;
    }
    for (int32_t i = signalBar_; i >= 0; --i) {
        if (rscp >= TD_SCDMA_SIGNAL_THRESHOLD[i]) {
            level = i;
            break;
        }
    }
    return level;
}

void CellInfo::GetCellInfoList(std::vector<sptr<CellInformation>> &cellInfo)
{
    cellInfo.clear();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto &cell : cellInfos_) {
            AddCellInformation(cell, cellInfo);
        }
    }
    TELEPHONY_LOGI("CellInfo::GetCellInfoList size:%{public}zu slotId:%{public}d", cellInfo.size(), slotId_);
}

void CellInfo::ClearCellInfoList()
{
    std::lock_guard<std::mutex> lock(mutex_);
    currentCellInfo_ = nullptr;
    cellInfos_.clear();
}

void CellInfo::AddCellInformation(sptr<CellInformation> &cellInfo, std::vector<sptr<CellInformation>> &cellInfos)
{
    CellInformation::CellType type = cellInfo->GetNetworkType();
    switch (type) {
        case CellInformation::CellType::CELL_TYPE_GSM: {
            sptr<GsmCellInformation> cell = new GsmCellInformation;
            GsmCellInformation &gsmCell = *cell;
            gsmCell = *(static_cast<GsmCellInformation *>(cellInfo.GetRefPtr()));
            cellInfos.emplace_back(cell);
            break;
        }
        case CellInformation::CellType::CELL_TYPE_LTE: {
            sptr<LteCellInformation> cell = new LteCellInformation;
            LteCellInformation &lteCell = *cell;
            lteCell = *(static_cast<LteCellInformation *>(cellInfo.GetRefPtr()));
            cellInfos.emplace_back(cell);
            break;
        }
        case CellInformation::CellType::CELL_TYPE_WCDMA: {
            sptr<WcdmaCellInformation> cell = new WcdmaCellInformation;
            WcdmaCellInformation &wcdmaCell = *cell;
            wcdmaCell = *(static_cast<WcdmaCellInformation *>(cellInfo.GetRefPtr()));
            cellInfos.emplace_back(cell);
            break;
        }
        case CellInformation::CellType::CELL_TYPE_CDMA: {
            sptr<CdmaCellInformation> cell = new CdmaCellInformation;
            CdmaCellInformation &cdmaCell = *cell;
            cdmaCell = *(static_cast<CdmaCellInformation *>(cellInfo.GetRefPtr()));
            cellInfos.emplace_back(cell);
            break;
        }
        case CellInformation::CellType::CELL_TYPE_TDSCDMA: {
            sptr<TdscdmaCellInformation> cell = new TdscdmaCellInformation;
            TdscdmaCellInformation &tdscdmaCell = *cell;
            tdscdmaCell = *(static_cast<TdscdmaCellInformation *>(cellInfo.GetRefPtr()));
            cellInfos.emplace_back(cell);
            break;
        }
        case CellInformation::CellType::CELL_TYPE_NR: {
            sptr<NrCellInformation> cell = new NrCellInformation;
            NrCellInformation &nrCell = *cell;
            nrCell = *(static_cast<NrCellInformation *>(cellInfo.GetRefPtr()));
            cellInfos.emplace_back(cell);
            break;
        }
        default:
            break;
    }
}

sptr<CellLocation> CellInfo::GetCellLocation()
{
    if (currentCellInfo_ == nullptr) {
        TELEPHONY_LOGE("CellInfo::GetCellLocation is null slotId:%{public}d", slotId_);
        return nullptr;
    }
    CellInformation::CellType type = currentCellInfo_->GetNetworkType();
    switch (type) {
        case CellInformation::CellType::CELL_TYPE_GSM: {
            sptr<TdscdmaCellInformation> cellinfo =
                static_cast<TdscdmaCellInformation *>(currentCellInfo_.GetRefPtr());
            sptr<GsmCellLocation> cellLocation = new GsmCellLocation;
            cellLocation->SetGsmParam(cellinfo->GetCellId(), cellinfo->GetLac());
            return cellLocation;
        }
        case CellInformation::CellType::CELL_TYPE_TDSCDMA: {
            sptr<GsmCellInformation> cellinfo = static_cast<GsmCellInformation *>(currentCellInfo_.GetRefPtr());
            sptr<GsmCellLocation> cellLocation = new GsmCellLocation;
            cellLocation->SetGsmParam(cellinfo->GetCellId(), cellinfo->GetLac());
            return cellLocation;
        }
        case CellInformation::CellType::CELL_TYPE_LTE: {
            sptr<LteCellInformation> cellinfo = static_cast<LteCellInformation *>(currentCellInfo_.GetRefPtr());
            sptr<GsmCellLocation> cellLocation = new GsmCellLocation;
            cellLocation->SetGsmParam(cellinfo->GetCellId(), cellinfo->GetTac());
            return cellLocation;
        }
        case CellInformation::CellType::CELL_TYPE_WCDMA: {
            sptr<WcdmaCellInformation> cellinfo = static_cast<WcdmaCellInformation *>(currentCellInfo_.GetRefPtr());
            sptr<GsmCellLocation> cellLocation = new GsmCellLocation;
            cellLocation->SetGsmParam(cellinfo->GetCellId(), cellinfo->GetLac(), cellinfo->GetPsc());
            return cellLocation;
        }
        case CellInformation::CellType::CELL_TYPE_CDMA: {
            sptr<CdmaCellInformation> cellinfo = static_cast<CdmaCellInformation *>(currentCellInfo_.GetRefPtr());
            sptr<CdmaCellLocation> cellLocation = new CdmaCellLocation;
            cellLocation->SetCdmaParam(cellinfo->GetBaseId(), cellinfo->GetLatitude(), cellinfo->GetLongitude(),
                cellinfo->GetNid(), cellinfo->GetSid());
            return cellLocation;
        }
        default:
            TELEPHONY_LOGE("CellInfo::GetCellLocation cell type error slotId:%{public}d", slotId_);
            break;
    }
    return nullptr;
}
} // namespace Telephony
} // namespace OHOS