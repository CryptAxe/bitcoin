// Copyright (c) 2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

// TODO cleanup includes
#include "chainparams.h"
#include "consensus/validation.h"
#include "core_io.h"
#include "miner.h"
#include "random.h"
#include "script/sigcache.h"
#include "script/standard.h"
#include "uint256.h"
#include "utilstrencodings.h"
#include "validation.h"

#include "test/test_bitcoin.h"

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(transaction_criticaldata_tests, TestChain100Setup)

BOOST_AUTO_TEST_CASE(serialization)
{
    CMutableTransaction mtx;
    mtx.vin.resize(1);
    mtx.vout.resize(1);

    mtx.vin[0].prevout.SetNull();
    mtx.vin[0].scriptSig = CScript();

    CScript script;
    script << OP_RETURN;

    mtx.vout[0] = CTxOut(50 * CENT, script);

    mtx.criticalData.n = 42;
    mtx.criticalData.hashCritical = GetRandHash();

    // Get the transaction's serialization
    CDataStream ss(SER_NETWORK, PROTOCOL_VERSION);
    mtx.Serialize(ss);

    // Deserialize
    CTransaction txDeserialized(deserialize, ss);

    // Check that CTransaction was properly deserialized
    BOOST_CHECK(txDeserialized.GetHash() == mtx.GetHash());
}

BOOST_AUTO_TEST_CASE(valid)
{
    // Test in block with a valid data & commit
}

BOOST_AUTO_TEST_CASE(invalid)
{
    // Things to check:
    // invalid locktime / blockheight
    // invalid nSidechain
    // invalid (null) hashCritical in tx
    // valid hashCritical in tx, but missing from coinbase
}

BOOST_AUTO_TEST_SUITE_END()

