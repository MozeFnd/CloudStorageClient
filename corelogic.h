#ifndef CORELOGIC_H
#define CORELOGIC_H
#include <memory>
#include "directoryarea.h"
#include "kvstore.h"
#include "communicator.h"
#include "pathtree.h"
#include "util.h"

#define INVALID_ID 0

class CoreLogic
{
public:
    CoreLogic() = delete;
    CoreLogic(std::shared_ptr<KVStore> kvstore, std::shared_ptr<Communicator> communicator, std::shared_ptr<PathTree> path_tree);

private:
    uint32_t acquireUniqueId(){
        std::string remained_id = kvstore_->read("unusedID");
        if (remained_id.empty()) {
            kvstore_->store("unusedID", "10,11,12,13,15,16,17");
            // get a batch of id from server
            remained_id = kvstore_->read("unusedID");
        }
        size_t comma_pos = remained_id.find(',');
        uint32_t id;
        if (comma_pos == std::string::npos) {
            id = std::stoi(remained_id);
            remained_id = "";
        } else {
            auto id_str = remained_id.substr(0, comma_pos);
            remained_id = remained_id.substr(comma_pos + 1);
            id = stoi(id_str);
        }
        kvstore_->store("unusedID", remained_id);
        return id;
    }
    void releaseUniqueId(uint32_t id){
        std::string remained_id = kvstore_->read("unusedID");
        if (remained_id.empty()) {
            remained_id = std::to_string(id);
        } else {
            remained_id = remained_id + "," + std::to_string(id);
        }
        kvstore_->store("unusedID", remained_id);
    }

    void recordIDandPath(uint32_t id, std::string path){
        kvstore_->store(std::to_string(id), path);
        auto usedID = kvstore_->read("usedID");
        if (usedID.find(std::to_string(id)) != std::string::npos) {
            return;
        }
        if (usedID.empty()) {
            kvstore_->store("usedID", std::to_string(id));
        } else {
            kvstore_->store("usedID", usedID + "," + std::to_string(id));
        }
    }
    void eraseIDandPath(uint32_t id) {
        kvstore_->remove(std::to_string(id));
        auto usedID = kvstore_->read("usedID");
        auto idStr = std::to_string(id);
        size_t pos = usedID.find(idStr);
        if (pos == std::string::npos) {
            // logging
            return;
        }
        if (pos + idStr.size() == usedID.size()) {
            // at the last
            usedID = usedID.substr(0, pos);
        } else {
            // not at the last
            usedID = usedID.substr(0, pos) + usedID.substr(pos + idStr.size() + 1);
        }
        kvstore_->store("usedID", usedID);
    }

public:
    void init(std::shared_ptr<DirectoryArea> directory_area) {
        directory_area_ = directory_area;
        auto recordedID = splitStr(kvstore_->read("usedID"), ',');
        for (auto id : recordedID) {
            auto path = kvstore_->read(id);
            trackDirecotory(path, std::stoi(id));
        }
        // trackDirecotory("C:/Users/10560/Desktop/qt_tests/A");
        // trackDirecotory("C:/Users/10560/Desktop/qt_tests/B");
        // trackDirecotory("C:/Users/10560/Desktop/qt_tests/C");
        // trackDirecotory("C:/Users/10560/Desktop/qt_tests/D");
        // trackDirecotory("C:/Users/10560/Desktop/qt_tests/E");
    }

    /**
     * @brief 为指定路径分配uniqueID并开始追踪
     * @param path
     * @return
     */
    std::string trackDirecotory(std::string path, uint32_t id = INVALID_ID){
        if (id == INVALID_ID) {
            // id 为空时，说明path是由操作添加的
            id = acquireUniqueId();
            recordIDandPath(id, path);
        }
        // 否则说明正在进行初始化时的批量添加，此时不需要申请新的id
        std::vector<uint32_t> covered_ids;
        bool already_traced = path_tree_->addPath(path, id, covered_ids);
        if (already_traced) {
            releaseUniqueId(id);
            return "该文件夹（或其父文件夹）已同步，无需重复添加";
        }

        auto dirs = splitStr(path, '/');
        auto name = dirs[dirs.size() - 1];
        directory_area_->createDirectoryItem(name, id);
        for (auto& id_to_remove : covered_ids) {
            directory_area_->removeDirectoryItem(id_to_remove);
            eraseIDandPath(id);
        }
        return "文件夹开始同步";
    }

private:
    std::shared_ptr<KVStore> kvstore_;
    std::shared_ptr<Communicator> communicator_;
    std::shared_ptr<PathTree> path_tree_;
    std::shared_ptr<DirectoryArea> directory_area_;
};

#endif // CORELOGIC_H
