#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <QTcpSocket>
#include <memory>
#include <unordered_map>
#include "kvstore.h"
#include "jsontool.h"
#include <fstream>

class Communicator
{
public:
    enum RequestType{
        Login = 1,
        AcquireID = 2,
        GetAllDir = 3,
        AddDir = 4,
        SyncFile = 5,
    };
    Communicator(std::shared_ptr<KVStore> kvstore);
    ~Communicator();
    bool buildConnection();
    void closeConnection();
    bool login(QString uname, QString pwd);
    void blockRead(char* buffer, int n);
    void blockWrite(char* buffer, int n);
    std::vector<uint32_t> acquireIDinBatch();
    void addNewDirectory(uint32_t id, std::string name, std::shared_ptr<Json> archJs);
    void syncFile(uint32_t id, std::string path, std::string relativePath);
    std::unordered_map<uint32_t, std::string> getAllDirIDandName();

private:
    QTcpSocket socket_;
    std::shared_ptr<KVStore> kvstore_;
};

#endif // COMMUNICATOR_H
