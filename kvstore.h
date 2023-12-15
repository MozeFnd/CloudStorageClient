#ifndef KVSTORE_H
#define KVSTORE_H

#include <windows.h>
#include <QDebug>
#include <string>
#include "leveldb/cache.h"
#include "leveldb/db.h"
#include <string>


class KVStore
{
public:
    leveldb::DB* db;
    KVStore();
    void store(std::string key, std::string value);
    std::string read(std::string key);
    void remove(std::string key);
    ~KVStore();
private:
    HKEY hKey;
};

#endif // KVSTORE_H
