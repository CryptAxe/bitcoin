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

BOOST_FIXTURE_TEST_SUITE(transactionV3_tests, TestChain100Setup)

BOOST_AUTO_TEST_CASE(transactionV3_serialization)
{
    // Create a CTransactionV3
    CMutableTransactionV3 mtx;
    mtx.vin.resize(1);
    mtx.vout.resize(1);

    mtx.nSidechain = 0;
    mtx.hashCritical = GetRandHash();

    // Get the V3 transaction's serialization
    CDataStream ss(SER_NETWORK, PROTOCOL_VERSION);
    mtx.Serialize(ss);

    // Deserialize CTransactionV3
    CTransactionV3 txDeserialized(deserialize, ss);

    // Check that CTransactionV3 was properly deserialized
    BOOST_CHECK(txDeserialized.GetHash() == mtx.GetHash());
}

BOOST_AUTO_TEST_CASE(transactionV3_valid)
{
    // Test a CTransactionV3 in a block with a valid hashCritical & commit
}

BOOST_AUTO_TEST_CASE(transactionV3_invalid)
{
    // Things to check:
    // invalid locktime / blockheight
    // invalid nSidechain
    // invalid (null) hashCritical in tx
    // valid hashCritical in tx, but missing from coinbase
}

BOOST_AUTO_TEST_SUITE_END()
