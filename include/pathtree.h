#ifndef PATHTREE_H
#define PATHTREE_H

#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTreeView>

class Node{
public:
    static const uint32_t EMPTY_ID = 0;
    explicit Node()
        :unique_id_(EMPTY_ID){}
    explicit Node(std::string dir_name)
        : dir_name_(dir_name)
        , unique_id_(EMPTY_ID){}
    void addChild(std::shared_ptr<Node> child) {
        children_.push_back(child);
    }
public:
    std::vector<std::shared_ptr<Node>> children_;
    std::string dir_name_;
    uint32_t unique_id_;
};

class PathTree
{
public:
    PathTree();
    ~PathTree();
    bool addPath(std::string path, uint32_t unique_id, std::vector<uint32_t>& covered_ids);
    void display(QTreeView* treeView);
private:
    std::vector<std::shared_ptr<Node>> roots_;
    QStandardItemModel* model_;
};

#endif // PATHTREE_H
