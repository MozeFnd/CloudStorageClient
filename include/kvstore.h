#ifndef KVSTORE_H
#define KVSTORE_H

#include <windows.h>
#include <QDebug>
#include <string>
#include "leveldb/cache.h"
#include "leveldb/db.h"
#include "util.h"
#include <mutex>
#include <unordered_map>
#include <string>

/*
    KEY                    VALUE含义
path_of_{id}       被追踪的文件夹的id对应的文件夹的路径
used_id                本地已经使用的id列表

*/

typedef std::unique_lock<std::mutex> guard;

namespace db_key {
    const std::string USED_ID = "used_id";
};

class LockManager{
public:
    LockManager(){}
    ~LockManager(){}
    std::mutex& get_lock(std::string key) {
        return locks[key];
    }

    void lock(std::string key) {
        guard tmp_guard(manager_lock);
        locks[key].lock();
    }

    void unlock(std::string key) {
        guard tmp_guard(manager_lock);
        locks[key].unlock();
    }
private:
    std::mutex manager_lock;
    std::unordered_map<std::string, std::mutex> locks;
};

class KVStore
{
private:


public:
    leveldb::DB* db;
    KVStore();
    void store(std::string key, std::string value);
    std::string read(std::string key);
    void remove(std::string key);
    ~KVStore();

    std::vector<uint32_t> get_used_id() {
        auto& tmp_mutex = lock_manager.get_lock(db_key::USED_ID);
        guard tmp_guard(tmp_mutex);
        std::vector<uint32_t> ret;
        std::string id_str = read(db_key::USED_ID);
        auto id_str_arr = splitStr(id_str, ',');
        for (auto tmp : id_str_arr) {
            ret.push_back(std::stoul(tmp));
        }
        return ret;
    }

    // 增加一个被使用的id
    // 增加成果返回 true，增加失败返回 false
    void add_used_id(uint32_t id){
        auto& tmp_mutex = lock_manager.get_lock(db_key::USED_ID);
        guard tmp_guard(tmp_mutex);
        std::string new_val = read(db_key::USED_ID);
        if (new_val.empty()) {
            new_val = std::to_string(id);
        } else {
            new_val = new_val + "," + std::to_string(id);
        }
        store(db_key::USED_ID, new_val);
    }

    void set_id_path(std::string id, std::string path){
        auto key = "path_of_" + id;
        store(key, path);
    }

    std::string read_path_from_id(std::string id) {
        auto key = "path_of_" + id;
        auto path = read(key);
        return path;
    }
private:
    HKEY hKey;
    LockManager lock_manager;
};

#endif // KVSTORE_H
