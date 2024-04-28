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
        SyncFile = 5,
        GetRemoteTree = 6,
        UpdateRemoteTree = 7
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
    void syncFile(uint32_t id, std::wstring path, std::string relativePath);
    std::unordered_map<uint32_t, std::string> getAllDirIDandName();
    std::shared_ptr<Node> getRemoteTree(uint32_t id) {
        if (!buildConnection()) {
            // logging;
            return nullptr;
        }
        int max_batch_size = 1000;
        char* buffer = (char*)malloc(max_batch_size);
        buffer[0] = GetRemoteTree;
        *(uint32_t*)(buffer+1) = id;
        blockWrite(buffer, 5);

        if (socket_.waitForReadyRead()) {
            blockRead(buffer, 4);
        } else {
            qDebug() << "Error: " << socket_.errorString();
        }
    }

private:
    QTcpSocket socket_;
    std::shared_ptr<KVStore> kvstore_;
};

#endif // COMMUNICATOR_H
