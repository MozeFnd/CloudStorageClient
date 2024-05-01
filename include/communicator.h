#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <QTcpSocket>
#include <memory>
#include <unordered_map>
#include "kvstore.h"
#include "jsontool.h"
#include <fstream>
#include "tree.h"
#include "util.h"

class Communicator
{
public:
    enum RequestType{
        EXIT = 99,
        Login = 1,
        AcquireID = 2,
        GetAllDir = 3,
        AddDir = 4,
        UploadFile = 5,
        DownloadFile = 6,
        GetRemoteTree = 7,
        UpdateRemoteTree = 8,
    };
    Communicator(std::shared_ptr<KVStore> kvstore);
    ~Communicator();

    bool buildConnection();

    void closeConnection();

    bool login(QString uname, QString pwd);

    void blockRead(char* buffer, int n);

    void blockWrite(char* buffer, int n);

    uint32_t acquireID();

    void remoteAddNewDirectory(uint32_t id, std::string serialized);

    void uploadFile(uint32_t id, std::wstring path, std::string relativePath);

    void downloadFile(uint32_t id, std::string relativePath);

    std::unordered_map<uint32_t, std::string> getAllDirIDandName();

    std::shared_ptr<Node> getRemoteTree(uint32_t id) {
        if (!buildConnection()) {
            // logging;
            return nullptr;
        }
        char* buffer = (char*)malloc(5);
        buffer[0] = GetRemoteTree;
        *(uint32_t*)(buffer+1) = id;
        blockWrite(buffer, 5);

        blockRead(buffer, 4);
        uint32_t tree_size = *(uint32_t*)buffer;
        free(buffer);

        buffer = (char*)malloc(tree_size);
        blockRead(buffer, tree_size);
        tree::Node* pb_node = new tree::Node();
        std::string tmp_str(buffer, tree_size);
        pb_node->ParseFromString(tmp_str);
        auto ret = Node::fromPbNode(pb_node);

        delete pb_node;
        free(buffer);
        return ret;
    }

private:
    QTcpSocket socket_;
    std::shared_ptr<KVStore> kvstore_;
};

#endif // COMMUNICATOR_H
