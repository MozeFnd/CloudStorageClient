#ifndef CORELOGIC_H
#define CORELOGIC_H
#include <memory>
#include "directoryarea.h"
#include "kvstore.h"
#include "communicator.h"
#include "pathtree.h"
#include "util.h"
#include <filesystem>
#include "tree.h"

#define INVALID_ID 0

class CoreLogic
{
private:
    uint32_t acquireUniqueId(){
        std::string remained_id = kvstore_->read("unusedID");
        if (remained_id.empty()) {
            // get a batch of id from server
            auto newID = communicator_->acquireIDinBatch();
            remained_id.append(std::to_string(newID[0]));
            for (int i = 1;i < newID.size();i++) {
                remained_id.append("," + std::to_string(newID[i]));
            }
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
    CoreLogic() = delete;
    CoreLogic(std::shared_ptr<KVStore> kvstore, std::shared_ptr<Communicator> communicator, std::shared_ptr<PathTree> path_tree);

    std::string read_path_from_id(std::string id) {
        auto key = "path_of_" + id;
        auto path = kvstore_->read(key);
        return path;
    }

    void set_id_path(std::string id, std::string path){
        auto key = "path_of_" + id;
        kvstore_->store(key, path);
    }

    void init(std::shared_ptr<DirectoryArea> directory_area) {
        directory_area_ = directory_area;

        // 从数据库中读取出本地追踪的文件夹的id列表
        auto ids_str = kvstore_->read("used_id");
        auto recordedID = splitStr(ids_str, ',');
        for (auto id : recordedID) {
            auto dir = read_path_from_id(id);
            std::wstring w_path = str2wstr(dir);
            // 将 path 从 string 转为 wstring
            // 否则 std::filesystem::exists() 将对包含中文的路径做出错误判断。
            auto node = Node::fromPath(w_path, str2wstr("") ,true);
            local_tracked_dirs.push_back(node);
            // 展示文件夹
            auto name = wstr2str(node->name);
            directory_area_->addDirectoryItem(name, std::stoul(id));
        }
        // trackDirecotory("C:/Users/10560/Desktop/qt_tests/A");
    }

    void syncDirectory(uint32_t id, std::string path, std::string relativePath) {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                auto filepath = entry.path().string();
                communicator_->syncFile(id, filepath, relativePath + "/" + entry.path().filename().string());
            } else if (entry.is_directory()) {
                auto dirname = entry.path().filename().string();
                syncDirectory(id, entry.path().string(), relativePath + "/" + dirname);
            }
        }
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

        auto archJs = Json::fromPath(path);
        // communicator_->addNewDirectory(id, name, archJs);

        auto dir_name = std::filesystem::path(path).filename().string();
        syncDirectory(id, path, dir_name);

        directory_area_->addDirectoryItem(name, id);
        for (auto& id_to_remove : covered_ids) {
            directory_area_->removeDirectoryItem(id_to_remove);
            eraseIDandPath(id);
        }

        return "文件夹开始同步";
    }

    void scan() {
        for (auto root : local_tracked_dirs) {
            auto id = root->id;

        }
    }

private:
    std::shared_ptr<KVStore> kvstore_;
    std::shared_ptr<Communicator> communicator_;
    std::shared_ptr<PathTree> path_tree_;
    std::shared_ptr<DirectoryArea> directory_area_;
    std::vector<std::shared_ptr<Node>> local_tracked_dirs;  // 所有本地正在追踪的文件夹
};

#endif // CORELOGIC_H
