#ifndef KVSTORE_H
#define KVSTORE_H

#include <windows.h>
#include <QDebug>
#include <string>
#include "leveldb/cache.h"
#include "leveldb/db.h"
#include <string>

/*
    KEY                    VALUE含义
path_of_{id}       被追踪的文件夹的id对应的文件夹的路径
used_id                本地已经使用的id列表

*/


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
