// Copyright (c) 2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_SIDECHAINDB_H
#define BITCOIN_SIDECHAINDB_H

#include "primitives/transaction.h"

#include <map>
#include <queue>
#include <vector>

class CScript;
class uint256;

struct Sidechain;
struct SidechainDeposit;
struct SidechainWTJoinState;

class SidechainDB
{
public:
    SidechainDB();

    /** Add deposit to cache */
    void AddDeposits(const std::vector<CTransaction>& vtx);

    /** Add a new WT^ to the database */
    bool AddWTJoin(uint8_t nSidechain, const CTransaction& tx);

    /** Return true if the deposit is cached */
    bool HaveDepositCached(const SidechainDeposit& deposit) const;

    /** Return true if the full WT^ CTransaction is cached */
    bool HaveWTJoinCached(const uint256& wtxid) const;

    /** Get status of nSidechain's WT^(s) (public for unit tests) */
    std::vector<SidechainWTJoinState> GetState(uint8_t nSidechain) const;

    /** Return vector of deposits this tau for nSidechain. */
    std::vector<SidechainDeposit> GetDeposits(uint8_t nSidechain) const;

    /** Return B-WT^ for sidechain if one has been verified */
    CTransaction GetWTJoinTx(uint8_t nSidechain, int nHeight) const;

    /** Create a script with OP_RETURN data representing the DB state */
    CScript CreateStateScript(int nHeight) const;

    /** Return serialization hash of SCDB latest verification(s) */
    uint256 CreateSCDBHash() const;

    /** Check SCDB WT^ verification status */
    bool CheckWorkScore(const uint8_t& nSidechain, const uint256& wtxid) const;

    /** Print SCDB WT^ verification status */
    std::string ToString() const;

    /**
     * Update the DB state. This function is the only function that
     * updates the SCDB state during normal operation. The update
     * overload exists to facilitate testing.
     */
    bool Update(int nHeight, const uint256& hashBlock, const CTransactionRef& coinbase);

    /** Update the DB state (public for unit tests) */
    bool Update(uint8_t nSidechain, uint16_t nBlocks, uint16_t nScore, uint256 wtxid, bool fJustCheck = false);

    /** Return the hash of the last block SCDB processed */
    uint256 GetHashBlockLastSeen();

    std::multimap<uint256, int> GetLinkingData() const;

private:
    /** Sidechain state database */
    std::vector<std::vector<SidechainWTJoinState>> SCDB;

    /** Cache of potential WT^ transactions */
    std::vector<CTransaction> vWTJoinCache;

    /** Track deposits created during this tau */
    std::vector<SidechainDeposit> vDepositCache;

    std::multimap<uint256, int> mapBMMLD;
    std::queue<uint256> queueBMMLD;

    /** The most recent block that SCDB has processed */
    uint256 hashBlockLastSeen;

    /** Is there anything being tracked by the SCDB? */
    bool HasState() const;

    /** Try to read state from a coinbase and apply it if valid */
    bool ReadStateScript(const CTransactionRef &coinbase);

    /** Apply the results of ReadStateScript() to SCDB */
    bool ApplyStateScript(const CScript& state, const std::vector<std::vector<SidechainWTJoinState>>& vState, bool fJustCheck = false);

    /**
     * Submit default state update vote for all sidechains.
     * Used when either the miner of a block does not include a state
     * script, or the state script is invalid.
     */
    bool ApplyDefaultUpdate();
};

#endif // BITCOIN_SIDECHAINDB_H

