/**
 * @file metadata.cpp
 *
 * This file contains code for handling atomic metadata.
 *
 * TODO: Required - add a DeleteAboveBlock() function to remove entries from metadata DB during reorg
 */

#include "omnicore/metadata.h"

#include "omnicore/omnicore.h"
#include "omnicore/log.h"

#include "leveldb/db.h"
#include "arith_uint256.h"
#include "uint256.h"

#include <stdint.h>

#include <openssl/sha.h>

#include <boost/algorithm/string.hpp>

using namespace mastercore;

// Gets a metadata ID from a string (first and last bytes of the sha256 hash of the string)
std::string GetMetadataID(std::string metadata)
{
    uint256 hash;
    SHA256_CTX shaCtx;
    SHA256_Init(&shaCtx);
    SHA256_Update(&shaCtx, metadata.c_str(), metadata.length());
    SHA256_Final((unsigned char*)&hash, &shaCtx);

    std::string metadataId = "";
    std::string hashStr = hash.GetHex();
    if (hashStr.length() == 64) {
        metadataId = hashStr.substr(0,2) + hashStr.substr(62,2);
    }

    return metadataId;
}

// Adds metadata to the DB
void COmniMetadataDB::AddMetadata(int block, std::string address, std::string metadata)
{
    std::string key = strprintf("%s:%s", address, GetMetadataID(metadata));
    std::string value = strprintf("%d:%s", block, metadata);

    leveldb::Status status = pdb->Put(writeoptions, key, value);
    assert(status.ok());

    return;
}

// Obtains the metadata for a given metadata id
std::string COmniMetadataDB::GetMetadata(std::string address, std::string metadataId)
{
    assert(pdb);

    std::string key = strprintf("%s:%s", address, metadataId);
    std::string value = "";
    std::string metadata = "";

    leveldb::Status status = pdb->Get(readoptions, key, &value);
    if (status.ok()) {
        std::vector<std::string> vstr;
        boost::split(vstr, value, boost::is_any_of(":"), boost::token_compress_on);
        assert(2 == vstr.size());
        metadata = vstr[1];
    }

    return metadata;
}

// Obtains all the metadata published by a given address
std::set<std::string> COmniMetadataDB::GetAddressMetadata(std::string address)
{
    assert(pdb);

    std::set<std::string> metadataIds;
    leveldb::Iterator* it = NewIterator();

    for(it->SeekToFirst(); it->Valid(); it->Next()) {
        std::vector<std::string> vstr;
        std::string key = it->key().ToString();
        boost::split(vstr, key, boost::is_any_of(":"), boost::token_compress_on);
        assert(2 == vstr.size());
        std::string entryAddress = vstr[0];
        if (entryAddress == address) {
            metadataIds.insert(vstr[1]);
        }
    }

    delete it;

    return metadataIds;
}

void COmniMetadataDB::printAll()
{
    int count = 0;
    leveldb::Iterator* it = NewIterator();

    for(it->SeekToFirst(); it->Valid(); it->Next()) {
        ++count;
        PrintToConsole("entry #%8d= %s:%s\n", count, it->key().ToString(), it->value().ToString());
    }

    delete it;
}
