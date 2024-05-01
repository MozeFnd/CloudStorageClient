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

#define SYNCRONIZING 1
#define SYNCRONIZED 2

class CoreLogic
{
private:
    uint32_t acquireUniqueId(){
        auto newID = communicator_->acquireID();
        return newID;
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

    void init(std::shared_ptr<DirectoryArea> directory_area) {
        directory_area_ = directory_area;

        // 从数据库中读取出本地追踪的文件夹的id列表
        auto used_ids = kvstore_->get_used_id();
        for (auto id : used_ids) {
            auto dir = kvstore_->read_path_from_id(std::to_string(id));
            std::wstring w_path = str2wstr(dir);
            // 将 path 从 string 转为 wstring
            // 否则 std::filesystem::exists() 将对包含中文的路径做出错误判断。
            auto node = Node::fromPath(w_path, str2wstr("") ,true);
            local_tracked_dirs_.push_back(node);
            // 展示文件夹
            auto name = wstr2str(node->name);
            directory_area_->addDirectoryItem(name, id);
        }
    }


    void uploadDirectory(uint32_t id, std::wstring w_path, std::string relativePath) {
        for (const auto& entry : std::filesystem::directory_iterator(w_path)) {
            if (entry.is_regular_file()) {
                auto wfilepath = entry.path().wstring();
                auto new_relative_path = relativePath + "/" + entry.path().filename().string();
                communicator_->uploadFile(id, wfilepath, new_relative_path);
            } else if (entry.is_directory()) {
                auto dirname = entry.path().filename().string();
                uploadDirectory(id, entry.path().wstring(), relativePath + "/" + dirname);
            }
        }
    }

    void setTreeState(uint32_t id, uint32_t state) {
        std::string key = "tree" + std::to_string(id);
        auto& lock = lock_manager.get_lock(key);
        Guard tmp_guard(lock);
        states[id] = state;
    }

    uint32_t getTreeState(uint32_t id) {
        std::string key = "tree" + std::to_string(id);
        auto& lock = lock_manager.get_lock(key);
        Guard tmp_guard(lock);
        return states[id];
    }

    /**
     * @brief 为指定路径分配uniqueID并开始追踪
     * @param path
     * @return
     */
    std::string beginToTrackDirecotory(std::string path, uint32_t id = INVALID_ID){
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

        directory_area_->addDirectoryItem(name, id);
        for (auto& id_to_remove : covered_ids) {
            directory_area_->removeDirectoryItem(id_to_remove);
            eraseIDandPath(id);
        }

        auto w_path = str2wstr(path);
        auto dir_name = std::filesystem::path(w_path).filename().string();
        std::string key = "tree" + std::to_string(id);
        auto& lock = lock_manager.get_lock(key);
        Guard tmp_guard(lock);
        uploadDirectory(id, w_path, dir_name);

        kvstore_->set_id_path(std::to_string(id), path);
        kvstore_->add_used_id(id);

        return "文件夹开始同步";
    }

    // 删除本地文件夹
    void deleteLocal(std::wstring path) {
        try {
            std::filesystem::path path2delete(path);
            std::filesystem::remove_all(path2delete);
        } catch (const std::filesystem::filesystem_error& ex) {
            std::cerr << "Error deleting folder: " << ex.what() << '\n';
        }
    }

    void syncronize(std::shared_ptr<Node> local, std::shared_ptr<Node> remote, int parent_compare) {
        assert(local->file_type == DIRECTORY);
        assert(remote->file_type == DIRECTORY);
        int res = CompareFileTime(&local->max_last_modified, &remote->max_last_modified);
        // case1: 已经一致，无需同步
        if (res == 0) {
            return;
        }
        // case2: 根节点时间戳不一致，需要同步
        auto& remote_children = remote->children;
        for (auto local_child : local->children) {
            // 遍历本地根节点的所有子节点
            std::shared_ptr<Node> peer = nullptr;
            // 寻找子节点在 remote 中的对应节点
            for (auto remote_child : remote_children) {
                if (local_child->file_type == remote_child->file_type
                    && local_child->name == remote_child->name) {
                    peer = remote_child;
                    break;
                }
            }
            // case2.1: remote 没有找到同名文件（夹）
            if (peer == nullptr) {
                if (parent_compare < 0) {
                    // 由于 remote 更新，说明本地的文件夹已经过时，应当删除
                    deleteLocal(local_child->abs_path);
                } else {
                    // 反之，说明 remote 缺失对应文件夹，应当上传
                    if (local_child->file_type == DIRECTORY) {
                        uploadDirectory(local_child->id, local_child->abs_path, wstr2str(local_child->relative_path));
                    } else {
                        communicator_->uploadFile(local_child->id, local_child->abs_path, wstr2str(local_child->relative_path));
                    }
                }
                continue;
            }

            // case2.2: remote 中存在同名文件（夹）
            if (local_child->file_type == DIRECTORY) {
                // case 2.2.1: 类型为文件夹
                syncronize(local_child, peer, res);
            } else {
                // case 2.2.2: 类型为文件
                int tmp_res = CompareFileTime(&local_child->max_last_modified, &peer->max_last_modified);
                if (tmp_res < 0) {
                    // remote 相比 local 更新，用远端文件进行覆盖
                    communicator_->downloadFile(local_child->id, wstr2str(local_child->relative_path));
                } else if (tmp_res > 0) {
                    // local 相比 remote 更新，上传本地文件
                    communicator_->uploadFile(local_child->id, local_child->abs_path, wstr2str(local_child->relative_path));
                }
                // tmp_res = 0 (即时间戳一致) 时无需同步
            }
        }
    }

    // 扫描所有本地追踪的tree
    void scan() {
        for (auto root : local_tracked_dirs_) {
            auto id = root->id;
            std::string key = "tree" + std::to_string(id);
            auto& lock = lock_manager.get_lock(key);
            Guard tmp_guard(lock);

            auto remote_root = communicator_->getRemoteTree(id);
            auto res = Node::compare(root, remote_root);
            if (res == 0) {
                // 未发生改变
                continue;
            }
            std::shared_ptr<Node> older, newer;
            if (res == 1) {
                newer = root;
                older = remote_root;
            } else {
                newer = remote_root;
                older = root;
            }

        }
    }

private:
    std::shared_ptr<KVStore> kvstore_;
    std::shared_ptr<Communicator> communicator_;
    std::shared_ptr<PathTree> path_tree_;
    std::shared_ptr<DirectoryArea> directory_area_;
    std::vector<std::shared_ptr<Node>> local_tracked_dirs_;  // 所有本地正在追踪的文件夹
    std::unordered_map<uint32_t, uint32_t> states;
    LockManager lock_manager;
};

#endif // CORELOGIC_H
