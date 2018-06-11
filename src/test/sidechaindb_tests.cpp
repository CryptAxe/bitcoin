// Copyright (c) 2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "consensus/validation.h"
#include "core_io.h"
#include "miner.h"
#include "random.h"
#include "script/sigcache.h"
#include "script/standard.h"
#include "sidechain.h"
#include "sidechaindb.h"
#include "uint256.h"
#include "utilstrencodings.h"
#include "validation.h"

#include "test/test_bitcoin.h"

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(sidechaindb_tests, TestChain100Setup)

BOOST_AUTO_TEST_CASE(sidechaindb_isolated)
{
    // Test SidechainDB without blocks
    uint256 hashWTTest = GetRandHash();
    uint256 hashWTHivemind = GetRandHash();
    uint256 hashWTWimble = GetRandHash();

    // SIDECHAIN_TEST
    SidechainWTPrimeState wtTest;
    wtTest.hashWTPrime = hashWTTest;
    // Start at +1 because we decrement in the loop
    wtTest.nBlocksLeft = SIDECHAIN_VERIFICATION_PERIOD + 1;
    wtTest.nSidechain = SIDECHAIN_TEST;
    for (int i = 1; i <= SIDECHAIN_MIN_WORKSCORE; i++) {
        std::vector<SidechainWTPrimeState> vWT;
        wtTest.nWorkScore = i;
        wtTest.nBlocksLeft--;
        vWT.push_back(wtTest);
        scdb.UpdateSCDBIndex(vWT);
    }

    // SIDECHAIN_HIVEMIND
    SidechainWTPrimeState wtHivemind;
    wtHivemind.hashWTPrime = hashWTHivemind;
    // Start at +1 because we decrement in the loop
    wtHivemind.nBlocksLeft = SIDECHAIN_VERIFICATION_PERIOD + 1;
    wtHivemind.nSidechain = SIDECHAIN_HIVEMIND;
    for (int i = 1; i <= (SIDECHAIN_MIN_WORKSCORE / 2); i++) {
        std::vector<SidechainWTPrimeState> vWT;
        wtHivemind.nWorkScore = i;
        wtHivemind.nBlocksLeft--;
        vWT.push_back(wtHivemind);
        scdb.UpdateSCDBIndex(vWT);
    }

    // SIDECHAIN_WIMBLE
    SidechainWTPrimeState wtWimble;
    wtWimble.hashWTPrime = hashWTWimble;
    // Start at +1 because we decrement in the loop
    wtWimble.nBlocksLeft = SIDECHAIN_VERIFICATION_PERIOD + 1;
    wtWimble.nSidechain = SIDECHAIN_WIMBLE;
    wtWimble.nWorkScore = 1;

    std::vector<SidechainWTPrimeState> vWT;
    vWT.push_back(wtWimble);
    scdb.UpdateSCDBIndex(vWT);

    // WT^ 0 should pass with valid workscore (100/100)
    BOOST_CHECK(scdb.CheckWorkScore(SIDECHAIN_TEST, hashWTTest));
    // WT^ 1 should fail with unsatisfied workscore (50/100)
    BOOST_CHECK(!scdb.CheckWorkScore(SIDECHAIN_HIVEMIND, hashWTHivemind));
    // WT^ 2 should fail with unsatisfied workscore (0/100)
    BOOST_CHECK(!scdb.CheckWorkScore(SIDECHAIN_WIMBLE, hashWTWimble));

    // Reset SCDB after testing
    scdb.Reset();
}

BOOST_AUTO_TEST_CASE(sidechaindb_MultipleVerificationPeriods)
{
    // Test SCDB with multiple verification periods,
    // approve multiple WT^s on the same sidechain.

    // WT^ hash for first period
    uint256 hashWTTest1 = GetRandHash();

    // Verify first transaction, check work score
    SidechainWTPrimeState wt1;
    wt1.hashWTPrime = hashWTTest1;
    // Start at +1 because we decrement in the loop
    wt1.nBlocksLeft = SIDECHAIN_VERIFICATION_PERIOD + 1;
    wt1.nSidechain = SIDECHAIN_TEST;
    for (int i = 1; i <= SIDECHAIN_MIN_WORKSCORE; i++) {
        std::vector<SidechainWTPrimeState> vWT;
        wt1.nWorkScore = i;
        wt1.nBlocksLeft--;
        vWT.push_back(wt1);
        scdb.UpdateSCDBIndex(vWT);
    }
    BOOST_CHECK(scdb.CheckWorkScore(SIDECHAIN_TEST, hashWTTest1));

    // Create dummy coinbase tx
    CMutableTransaction mtx;
    mtx.nVersion = 1;
    mtx.vin.resize(1);
    mtx.vout.resize(1);
    mtx.vin[0].scriptSig = CScript() << 486604799;
    mtx.vout.push_back(CTxOut(50 * CENT, CScript() << OP_RETURN));

    uint256 hashBlock = GetRandHash();

    // Update SCDB (will clear out old data from first period)
    std::string strError = "";
    scdb.Update(SIDECHAIN_VERIFICATION_PERIOD, hashBlock, mtx.vout, strError);

    // WT^ hash for second period
    uint256 hashWTTest2 = GetRandHash();

    // Add new WT^
    std::vector<SidechainWTPrimeState> vWT;
    SidechainWTPrimeState wt2;
    wt2.hashWTPrime = hashWTTest2;
    wt2.nBlocksLeft = SIDECHAIN_VERIFICATION_PERIOD;
    wt2.nSidechain = SIDECHAIN_TEST;
    wt2.nWorkScore = 1;
    vWT.push_back(wt2);
    scdb.UpdateSCDBIndex(vWT);
    BOOST_CHECK(!scdb.CheckWorkScore(SIDECHAIN_TEST, hashWTTest2));

    // Verify that SCDB has updated to correct WT^
    const std::vector<SidechainWTPrimeState> vState = scdb.GetState(SIDECHAIN_TEST);
    BOOST_CHECK(vState.size() == 1 && vState[0].hashWTPrime == hashWTTest2);

    // Give second transaction sufficient workscore and check work score
    for (int i = 1; i <= SIDECHAIN_MIN_WORKSCORE; i++) {
        std::vector<SidechainWTPrimeState> vWT;
        wt2.nWorkScore = i;
        wt2.nBlocksLeft--;
        vWT.push_back(wt2);
        scdb.UpdateSCDBIndex(vWT);
    }
    BOOST_CHECK(scdb.CheckWorkScore(SIDECHAIN_TEST, hashWTTest2));

    // Reset SCDB after testing
    scdb.Reset();
}

BOOST_AUTO_TEST_CASE(sidechaindb_MT_single)
{
    // Merkle tree based SCDB update test with only
    // SCDB data (no LD) in the tree, and a single
    // WT^ to be updated.

    // Create SCDB with initial WT^
    std::vector<SidechainWTPrimeState> vWT;

    SidechainWTPrimeState wt;
    wt.hashWTPrime = GetRandHash();
    wt.nBlocksLeft = SIDECHAIN_VERIFICATION_PERIOD;
    wt.nWorkScore = 1;
    wt.nSidechain = SIDECHAIN_TEST;

    vWT.push_back(wt);
    scdb.UpdateSCDBIndex(vWT);

    // Create a copy of the SCDB to manipulate
    SidechainDB scdbCopy = scdb;

    // Update the SCDB copy to get a new MT hash
    vWT.clear();
    wt.nWorkScore++;
    wt.nBlocksLeft--;
    vWT.push_back(wt);
    scdbCopy.UpdateSCDBIndex(vWT);

    // Simulate receiving Sidechain WT^ update message
    SidechainUpdateMSG msg;
    msg.nSidechain = SIDECHAIN_TEST;
    msg.hashWTPrime = wt.hashWTPrime;
    msg.nWorkScore = 2;

    SidechainUpdatePackage updatePackage;
    updatePackage.nHeight = 2;
    updatePackage.vUpdate.push_back(msg);

    scdb.AddSidechainNetworkUpdatePackage(updatePackage);

    BOOST_CHECK(scdb.UpdateSCDBMatchMT(2, scdbCopy.GetSCDBHash()));

    // Reset SCDB after testing
    scdb.Reset();
}

BOOST_AUTO_TEST_CASE(sidechaindb_MT_multipleSC)
{
    // Merkle tree based SCDB update test with multiple sidechains
    // that each have one WT^ to update. Only one WT^ out of the
    // three will be updated. This test ensures that nBlocksLeft is
    // properly decremented even when a WT^'s score is unchanged.

    // Add initial WT^s to SCDB
    SidechainWTPrimeState wtTest;
    wtTest.hashWTPrime = GetRandHash();
    wtTest.nBlocksLeft = SIDECHAIN_VERIFICATION_PERIOD;
    wtTest.nSidechain = SIDECHAIN_TEST;
    wtTest.nWorkScore = 1;

    SidechainWTPrimeState wtHivemind;
    wtHivemind.hashWTPrime = GetRandHash();
    wtHivemind.nBlocksLeft = SIDECHAIN_VERIFICATION_PERIOD;
    wtHivemind.nSidechain = SIDECHAIN_HIVEMIND;
    wtHivemind.nWorkScore = 1;

    SidechainWTPrimeState wtWimble;
    wtWimble.hashWTPrime = GetRandHash();
    wtWimble.nBlocksLeft = SIDECHAIN_VERIFICATION_PERIOD;
    wtWimble.nSidechain = SIDECHAIN_WIMBLE;
    wtWimble.nWorkScore = 1;

    std::vector<SidechainWTPrimeState> vWT;
    vWT.push_back(wtTest);
    vWT.push_back(wtHivemind);
    vWT.push_back(wtWimble);

    scdb.UpdateSCDBIndex(vWT);

    // Create a copy of the SCDB to manipulate
    SidechainDB scdbCopy = scdb;

    // Update the SCDB copy to get a new MT hash
    wtTest.nBlocksLeft--;
    wtTest.nWorkScore++;
    wtHivemind.nBlocksLeft--;
    wtWimble.nBlocksLeft--;
    vWT.clear();
    vWT.push_back(wtTest);
    vWT.push_back(wtHivemind);
    vWT.push_back(wtWimble);

    scdbCopy.UpdateSCDBIndex(vWT);

    // Simulate receiving Sidechain WT^ update message
    SidechainUpdateMSG msgTest;
    msgTest.nSidechain = SIDECHAIN_TEST;
    msgTest.hashWTPrime = wtTest.hashWTPrime;
    msgTest.nWorkScore = 2;

    SidechainUpdatePackage updatePackage;
    updatePackage.nHeight = 2;
    updatePackage.vUpdate.push_back(msgTest);

    scdb.AddSidechainNetworkUpdatePackage(updatePackage);

    // Use MT hash prediction to update the original SCDB
    BOOST_CHECK(scdb.UpdateSCDBMatchMT(2, scdbCopy.GetSCDBHash()));

    // Reset SCDB after testing
    scdb.Reset();
}

BOOST_AUTO_TEST_CASE(sidechaindb_MT_multipleWT)
{
    // Merkle tree based SCDB update test with multiple sidechains
    // and multiple WT^(s) being updated. This tests that MT based
    // SCDB update will work if work scores are updated for more
    // than one sidechain per block.

    // Add initial WT^s to SCDB
    SidechainWTPrimeState wtTest;
    wtTest.hashWTPrime = GetRandHash();
    wtTest.nBlocksLeft = SIDECHAIN_VERIFICATION_PERIOD;
    wtTest.nSidechain = SIDECHAIN_TEST;
    wtTest.nWorkScore = 1;

    SidechainWTPrimeState wtHivemind;
    wtHivemind.hashWTPrime = GetRandHash();
    wtHivemind.nBlocksLeft = SIDECHAIN_VERIFICATION_PERIOD;
    wtHivemind.nSidechain = SIDECHAIN_HIVEMIND;
    wtHivemind.nWorkScore = 1;

    SidechainWTPrimeState wtWimble;
    wtWimble.hashWTPrime = GetRandHash();
    wtWimble.nBlocksLeft = SIDECHAIN_VERIFICATION_PERIOD;
    wtWimble.nSidechain = SIDECHAIN_WIMBLE;
    wtWimble.nWorkScore = 1;

    std::vector<SidechainWTPrimeState> vWT;
    vWT.push_back(wtTest);
    vWT.push_back(wtHivemind);
    vWT.push_back(wtWimble);
    scdb.UpdateSCDBIndex(vWT);

    // Create a copy of the SCDB to manipulate
    SidechainDB scdbCopy = scdb;

    // Update the SCDB copy to get a new MT hash
    wtTest.nWorkScore++;
    wtTest.nBlocksLeft--;
    wtHivemind.nBlocksLeft--;
    wtWimble.nWorkScore++;
    wtWimble.nBlocksLeft--;

    vWT.clear();
    vWT.push_back(wtTest);
    vWT.push_back(wtHivemind);
    vWT.push_back(wtWimble);

    scdbCopy.UpdateSCDBIndex(vWT);

    // Simulate receiving Sidechain WT^ update message
    SidechainUpdateMSG msgTest;
    msgTest.nSidechain = SIDECHAIN_TEST;
    msgTest.hashWTPrime = wtTest.hashWTPrime;
    msgTest.nWorkScore = 2;

    SidechainUpdateMSG msgWimble;
    msgWimble.nSidechain = SIDECHAIN_WIMBLE;
    msgWimble.hashWTPrime = wtWimble.hashWTPrime;
    msgWimble.nWorkScore = 2;

    SidechainUpdatePackage updatePackage;
    updatePackage.nHeight = 2;
    updatePackage.vUpdate.push_back(msgTest);
    updatePackage.vUpdate.push_back(msgWimble);

    scdb.AddSidechainNetworkUpdatePackage(updatePackage);

    // Use MT hash prediction to update the original SCDB
    BOOST_CHECK(scdb.UpdateSCDBMatchMT(2, scdbCopy.GetSCDBHash()));

    // Reset SCDB after testing
    scdb.Reset();
}

BOOST_AUTO_TEST_CASE(new_deposit_test_p)
{
    BOOST_CHECK(chainActive.Height() == 100);

    // Generate a block
    CreateAndProcessBlock({}, GetScriptForRawPubKey(coinbaseKey.GetPubKey()));

    // Checking that we can make blocks normally
    BOOST_CHECK(chainActive.Height() == 101);

    // Use the test chain coinbase key as an example sidechain destination
    CKeyID destinationKey = coinbaseKey.GetPubKey().GetID();

    // Create sidechain deposit (P's style)
    CMutableTransaction mtx;
    mtx.nVersion = 2;
    mtx.vin.resize(1);
    mtx.vout.resize(1);
    mtx.vin[0].prevout.hash = coinbaseTxns[0].GetHash();
    mtx.vin[0].prevout.n = 0;

    // Sidechain deposit script
    CScript depositScript;
    depositScript.resize(6);
    depositScript[0] = OP_TRUE;
    depositScript[1] = 0x04; // Push the next 4 bytes to the stack
    depositScript[2] = 0x0e; // Flag bytes
    depositScript[3] = 0x0d;
    depositScript[4] = 0x0d;
    depositScript[5] = 0x0e;
    depositScript << ToByteVector(destinationKey);
    depositScript << CScriptNum::serialize(255); // nSidechain
    depositScript << OP_DROP << OP_2DROP; // leave only OP_TRUE on the stack

    // Note that the 3 DROP operations are needed in the case that the deposit
    // is to sidechain #0. We could make the KeyID the last thing pushed to the
    // stack and remove the DROP commands. I think it looks better this way
    // because OP_TRUE being the only thing we leave on the stack makes
    // intentions clear.

    mtx.vout[0].scriptPubKey = depositScript;
    mtx.vout[0].nValue = 50 * CENT;

    // Sign (the coinbase input)
    const CTransaction txToSign(mtx);
    std::vector<unsigned char> vchSig;
    uint256 hash = SignatureHash(GetScriptForRawPubKey(coinbaseKey.GetPubKey()), txToSign, 0, SIGHASH_ALL, 0, SIGVERSION_BASE);
    BOOST_CHECK(coinbaseKey.Sign(hash, vchSig));
    vchSig.push_back((unsigned char)SIGHASH_ALL);
    mtx.vin[0].scriptSig << vchSig;

    CreateAndProcessBlock({mtx}, GetScriptForRawPubKey(coinbaseKey.GetPubKey()), false, false);

    BOOST_CHECK(chainActive.Height() == 102);

    // Now spend the sidechain deposit output we created
    CMutableTransaction mtxSpend;
    mtxSpend.nVersion = 2;
    mtxSpend.vin.resize(1);
    mtxSpend.vout.resize(1);
    mtxSpend.vin[0].prevout.hash = mtx.GetHash();
    mtxSpend.vin[0].prevout.n = 0;
    mtxSpend.vout[0].scriptPubKey = CScript() << OP_0;
    mtxSpend.vout[0].nValue = 25 * CENT;

    CreateAndProcessBlock({mtxSpend}, GetScriptForRawPubKey(coinbaseKey.GetPubKey()), false, false);

    BOOST_CHECK(chainActive.Height() == 103);
}

BOOST_AUTO_TEST_CASE(new_deposit_test_m)
{
    BOOST_CHECK(chainActive.Height() == 100);

    // Generate a block
    CreateAndProcessBlock({}, GetScriptForRawPubKey(coinbaseKey.GetPubKey()));

    // Checking that we can make blocks normally
    BOOST_CHECK(chainActive.Height() == 101);

    // Use the test chain coinbase key as an example sidechain destination
    CKeyID destinationKey = coinbaseKey.GetPubKey().GetID();
    std::vector<unsigned char> vchKeyID = ToByteVector(destinationKey);

    // Create sidechain deposit (M's style)
    CMutableTransaction mtx;
    mtx.nVersion = 2;
    mtx.vin.resize(1);
    mtx.vout.resize(1);
    mtx.vin[0].prevout.hash = coinbaseTxns[0].GetHash();
    mtx.vin[0].prevout.n = 0;

    CScript depositScript;
    depositScript.resize(6);
    depositScript[0] = 0x19; // Push the next 25 bytes to the stack
    depositScript[1] = 0x0e; // Flag bytes
    depositScript[2] = 0x0d;
    depositScript[3] = 0x0d;
    depositScript[4] = 0x0e;
    depositScript[5] = 0xff; // Sidechain number (255)
    depositScript.insert(depositScript.end(), vchKeyID.begin(), vchKeyID.end());

    mtx.vout[0].scriptPubKey = depositScript;
    mtx.vout[0].nValue = 50 * CENT;

    // Sign (the coinbase input)
    const CTransaction txToSign(mtx);
    std::vector<unsigned char> vchSig;
    uint256 hash = SignatureHash(GetScriptForRawPubKey(coinbaseKey.GetPubKey()), txToSign, 0, SIGHASH_ALL, 0, SIGVERSION_BASE);
    BOOST_CHECK(coinbaseKey.Sign(hash, vchSig));
    vchSig.push_back((unsigned char)SIGHASH_ALL);
    mtx.vin[0].scriptSig << vchSig;

    CreateAndProcessBlock({mtx}, GetScriptForRawPubKey(coinbaseKey.GetPubKey()), false, false);

    BOOST_CHECK(chainActive.Height() == 102);

    // Now spend the sidechain deposit output we created
    CMutableTransaction mtxSpend;
    mtxSpend.nVersion = 2;
    mtxSpend.vin.resize(1);
    mtxSpend.vout.resize(1);
    mtxSpend.vin[0].prevout.hash = mtx.GetHash();
    mtxSpend.vin[0].prevout.n = 0;
    mtxSpend.vout[0].scriptPubKey = CScript() << OP_0;
    mtxSpend.vout[0].nValue = 21 * CENT;

    CreateAndProcessBlock({mtxSpend}, GetScriptForRawPubKey(coinbaseKey.GetPubKey()), false, false);

    BOOST_CHECK(chainActive.Height() == 103);
}

BOOST_AUTO_TEST_SUITE_END()
