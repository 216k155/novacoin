// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_WALLETDB_H
#define BITCOIN_WALLETDB_H

#include "db.h"
#include "keystore.h"

using namespace std;

class CKeyPool;
class CAccount;
class CAccountingEntry;

// Error statuses for the wallet database
enum DBErrors
{
    DB_LOAD_OK,
    DB_CORRUPT,
    DB_NONCRITICAL_ERROR,
    DB_TOO_NEW,
    DB_LOAD_FAIL,
    DB_NEED_REWRITE
};

class CKeyMetadata
{
public:
    static const int CURRENT_VERSION=1;
    int nVersion;
    int64_t nCreateTime; // 0 means unknown

    CKeyMetadata();
    CKeyMetadata(int64_t nCreateTime_);

    IMPLEMENT_SERIALIZE
    (
        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(nCreateTime);
    )

    void SetNull();
};

// Access to the wallet database (wallet.dat)
class CWalletDB : public CDB
{
public:
    CWalletDB(string strFilename, const char* pszMode="r+");
private:
    CWalletDB(const CWalletDB&);
    void operator=(const CWalletDB&);
public:
    bool WriteName(const string& strAddress, const string& strName);
    bool EraseName(const string& strAddress);
    bool WriteTx(uint256 hash, const CWalletTx& wtx);
    bool EraseTx(uint256 hash);
    bool WriteKey(const CPubKey& key, const CPrivKey& vchPrivKey, const CKeyMetadata &keyMeta);
    bool WriteMalleableKey(const CMalleableKeyView& keyView, const CSecret& vchSecretH, const CKeyMetadata &keyMeta);
    bool WriteCryptedMalleableKey(const CMalleableKeyView& keyView, const vector<unsigned char>& vchCryptedSecretH, const CKeyMetadata &keyMeta);
    bool WriteCryptedKey(const CPubKey& key, const vector<unsigned char>& vchCryptedSecret, const CKeyMetadata &keyMeta);
    bool WriteMasterKey(unsigned int nID, const CMasterKey& kMasterKey);
    bool EraseMasterKey(unsigned int nID);
    bool EraseCryptedKey(const CPubKey& key);
    bool EraseCryptedMalleableKey(const CMalleableKeyView& keyView);
    bool WriteCScript(const uint160& hash, const CScript& redeemScript);
    bool WriteWatchOnly(const CScript &dest);
    bool EraseWatchOnly(const CScript &dest);
    bool WriteBestBlock(const CBlockLocator& locator);
    bool ReadBestBlock(CBlockLocator& locator);
    bool WriteOrderPosNext(int64_t nOrderPosNext);
    bool WriteDefaultKey(const CPubKey& key);
    bool ReadPool(int64_t nPool, CKeyPool& keypool);
    bool WritePool(int64_t nPool, const CKeyPool& keypool);
    bool ErasePool(int64_t nPool);
    bool WriteMinVersion(int nVersion);
    bool ReadAccount(const string& strAccount, CAccount& account);
    bool WriteAccount(const string& strAccount, const CAccount& account);
private:
    bool WriteAccountingEntry(const uint64_t nAccEntryNum, const CAccountingEntry& acentry);
public:
    bool WriteAccountingEntry(const CAccountingEntry& acentry);
    int64_t GetAccountCreditDebit(const string& strAccount);
    void ListAccountCreditDebit(const string& strAccount, list<CAccountingEntry>& acentries);

    DBErrors ReorderTransactions(CWallet*);
    DBErrors LoadWallet(CWallet* pwallet);
    DBErrors FindWalletTx(CWallet* pwallet, vector<uint256>& vTxHash);
    DBErrors ZapWalletTx(CWallet* pwallet);

    static bool Recover(CDBEnv& dbenv, string filename, bool fOnlyKeys);
    static bool Recover(CDBEnv& dbenv, string filename);
};

#endif // BITCOIN_WALLETDB_H
