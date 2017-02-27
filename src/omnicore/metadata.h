#ifndef OMNICORE_METADATA_H
#define OMNICORE_METADATA_H

#include "leveldb/db.h"

#include "omnicore/log.h"
#include "omnicore/persistence.h"

#include <set>
#include <stdint.h>
#include <boost/filesystem.hpp>

std::string GetMetadataID(std::string metadata);

/** LevelDB based storage for atomic metadata. */
class COmniMetadataDB : public CDBBase
{
public:
    COmniMetadataDB(const boost::filesystem::path& path, bool fWipe)
    {
        leveldb::Status status = Open(path, fWipe);
        PrintToConsole("Loading metadata database: %s\n", status.ToString());
    }

    virtual ~COmniMetadataDB()
    {
        if (msc_debug_persistence) PrintToLog("COmniMetadataDB closed\n");
    }

    void AddMetadata(int block, std::string address, std::string metadata);
    std::string GetMetadata(std::string address, std::string metadataId);
    std::set<std::string> GetAddressMetadata(std::string address);

    void printAll();
};

namespace mastercore
{
    extern COmniMetadataDB *p_OmniMetadataDB;
}

#endif // OMNICORE_METADATA_H
