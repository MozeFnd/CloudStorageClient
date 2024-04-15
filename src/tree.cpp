#include "include/tree.h"
#include <filesystem>
#include <windows.h>
#include <QDebug>
#include <util.h>

std::shared_ptr<Node> Node::fromPath(std::wstring abs_path, std::wstring relative_path, bool is_root){
    std::filesystem::path p(abs_path);
    if (!std::filesystem::exists(p)) {
        qDebug() << "path: " << QString::fromStdWString(abs_path) << "does not exist.";
        return nullptr;
    }
    auto node = std::make_shared<Node>();
    node->is_root = is_root;
    node->abs_path = abs_path;
    node->last_modified = get_dir_last_modified(abs_path);
    node->max_last_modified = node->last_modified;
    node->name = p.filename().wstring();
    node->relative_path = relative_path + node->name;
    node->file_type = DIRECTORY;
    FILETIME tmp_max = node->last_modified;

    for (const auto& entry : std::filesystem::directory_iterator(abs_path)) {
        std::wstring tmp_wpath = entry.path().wstring();
        auto tmp_path = wstr2str(tmp_wpath);
        if (entry.is_regular_file()) {
            auto child = std::make_shared<Node>();
            child->file_type = REGULAR_FILE;
            child->abs_path = tmp_wpath;
            child->last_modified = get_last_modified(tmp_wpath);
            child->max_last_modified = child->last_modified;
            tmp_max = max_filetime(tmp_max, child->max_last_modified);
            child->name = entry.path().filename().wstring();
            child->relative_path = node->relative_path + str2wstr("/") + child->name;
            node->add_child(child);
        } else if (entry.is_directory()) {
            auto toSearch = tmp_wpath;
            auto child = fromPath(toSearch, node->relative_path + str2wstr("/"), false);
            tmp_max = max_filetime(tmp_max, child->max_last_modified);
            node->add_child(child);
        }
    }
    node->max_last_modified = tmp_max;
    return node;
}
