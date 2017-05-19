// Copyright (c) 2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "sidechain.h"
#include "utilstrencodings.h"

#include <sstream>

bool SidechainNumberValid(uint8_t nSidechain)
{
    if (!(nSidechain < ARRAYLEN(ValidSidechains)))
        return false;

    // Check that number corresponds to a valid sidechain
    switch (nSidechain) {
    case SIDECHAIN_TEST:
    case SIDECHAIN_HIVEMIND:
    case SIDECHAIN_WIMBLE:
        return true;
    default:
        return false;
    }
}

std::string Sidechain::GetSidechainName() const
{
    // Check that number corresponds to a valid sidechain
    switch (nSidechain) {
    case SIDECHAIN_TEST:
        return "SIDECHAIN_TEST";
    case SIDECHAIN_HIVEMIND:
        return "SIDECHAIN_HIVEMIND";
    case SIDECHAIN_WIMBLE:
        return "SIDECHAIN_WIMBLE";
    default:
        break;
    }
    return "SIDECHAIN_UNKNOWN";
}

uint16_t Sidechain::GetTau() const
{
    return nWaitPeriod + nVerificationPeriod;
}

int Sidechain::GetLastTauHeight(int nHeight) const
{
    uint16_t nTau = GetTau();
    for (;;) {
        if (nHeight < 0)
            return -1;
        if (nHeight % nTau == 0 || nHeight == 0)
            break;
        nHeight--;
    }
    return nHeight;
}

bool SidechainDeposit::operator==(const SidechainDeposit& a) const
{
    return (a.nSidechain == nSidechain &&
            a.keyID == keyID &&
            a.hex == hex);
}

std::string Sidechain::ToString() const
{
    std::stringstream ss;
    ss << "nSidechain=" << (unsigned int)nSidechain << std::endl;
    ss << "nWaitPeriod=" << nWaitPeriod << std::endl;
    ss << "nVerificationPeriod=" << nVerificationPeriod << std::endl;
    ss << "nMinWorkScore=" << nMinWorkScore << std::endl;
    return ss.str();
}

std::string SidechainDeposit::ToString() const
{
    std::stringstream ss;
    ss << "nSidechain=" << (unsigned int)nSidechain << std::endl;
    ss << "keyID=" << keyID.ToString() << std::endl;
    ss << "hex=" << hex << std::endl;
    return ss.str();
}

std::string SidechainWTJoinState::ToString() const
{
    std::stringstream ss;
    ss << "nSidechain=" << (unsigned int)nSidechain << std::endl;
    ss << "nBlocksLeft=" << (unsigned int)nBlocksLeft << std::endl;
    ss << "nWorkScore=" << (unsigned int)nWorkScore << std::endl;
    ss << "wtxid=" << wtxid.ToString() << std::endl;
    return ss.str();
}
