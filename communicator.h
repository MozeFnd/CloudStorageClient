#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <QTcpSocket>
#include <memory>
#include "kvstore.h"

class Communicator
{
public:
    enum RequestType{
        Login = 1,

    };
    Communicator(std::shared_ptr<KVStore> kvstore);
    ~Communicator();
    bool buildConnection(QString ip, quint32 port);
    bool login(QString uname, QString pwd);

private:
    QTcpSocket socket_;
    std::shared_ptr<KVStore> kvstore_;
};

#endif // COMMUNICATOR_H
