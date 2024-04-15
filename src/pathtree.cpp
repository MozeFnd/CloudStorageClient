#include "pathtree.h"
#include "util.h"
#include <functional>

PathTree::PathTree() {
    model_ = new QStandardItemModel();
}

PathTree::~PathTree(){
    delete model_;
}

bool PathTree::addPath(std::string path, uint32_t unique_id, std::vector<uint32_t>& covered_ids){
    std::vector<std::string> dirs = splitStr(path, '/');

    std::shared_ptr<PNode> root = nullptr;
    for (auto tmp : roots_) {
        if (tmp->dir_name_ == dirs[0]) {
            root = tmp;
            break;
        }
    }

    if (root == nullptr) {
        root = std::make_shared<PNode>(dirs[0]);
        auto tmp = root;
        for (size_t i = 1;i < dirs.size();i++) {
            auto dir = dirs[i];
            auto next = std::make_shared<PNode>(dir);
            tmp->addChild(next);
            tmp = next;
        }
        roots_.push_back(root);
        return false;
    }

    bool alreadyTraced = false;
    size_t i = 0;
    auto curNode = root;
    // cur->dir_name_ == dirs[0] must hold
    while (true) {
        if (curNode->children_.empty()) {
            alreadyTraced = true;
            break;
        }
        i++;
        if (i >= dirs.size()) {
            std::queue<std::shared_ptr<PNode>> q;
            q.push(curNode);
            while(!q.empty()){
                auto node = q.front();
                if (node->children_.empty()) {
                    covered_ids.push_back(node->unique_id_);
                } else {
                    for (auto& child : node->children_) {
                        q.push(child);
                    }
                }
                q.pop();
            }
            curNode->children_.clear();
            break;
        }

        bool found = false;
        for (auto& child : curNode->children_) {
            if (child->dir_name_ == dirs[i]) {
                curNode = child;
                found = true;
                break;
            }
        }
        if (found) {
            continue;
        } else {
            auto start = std::make_shared<PNode>(dirs[i]);
            auto tmp = start;
            i++;
            for (;i < dirs.size();i++) {
                auto next = std::make_shared<PNode>(dirs[i]);
                tmp->addChild(next);
                tmp = next;
            }
            tmp->unique_id_ = unique_id;
            curNode->addChild(start);
            break;
        }
    }

    return alreadyTraced;
}

void PathTree::display(QTreeView* treeView){
    // Create the root item
    // model->clear();
    std::function<void(std::shared_ptr<PNode>, QStandardItem*)> build = [&](std::shared_ptr<PNode> node, QStandardItem* node_item){
        for (auto& child : node->children_) {
            QStandardItem *child_item = new QStandardItem(QString::fromStdString(child->dir_name_));
            node_item->appendRow(child_item);
            build(child, child_item);
        }
    };
    qDebug() << roots_.size();
    for (auto& root : roots_) {
        QStandardItem *root_item = new QStandardItem(QString::fromStdString(root->dir_name_));
        model_->appendRow(root_item);
        build(root, root_item);
    }

    // auto tmp_model = new QStandardItemModel();
    treeView->setModel(model_);

    // Show the tree view
    treeView->show();
}
