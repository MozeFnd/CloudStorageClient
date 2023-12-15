#include "communicator.h"

Communicator::Communicator(std::shared_ptr<KVStore> kvstore) {
    kvstore_ = kvstore;
}

Communicator::~Communicator(){
    if (socket_.state() == QAbstractSocket::ConnectedState){
        socket_.close();
    }
}

bool Communicator::buildConnection(QString ip, quint32 port) {
    socket_.connectToHost(ip, port); // Adjust the IP and port as needed
    if (!socket_.waitForConnected()) {
        qDebug() << "Error: " << socket_.errorString();
        return false;
    }
    return true;
}

bool Communicator::login(QString uname, QString pwd){
    if (socket_.state() != QAbstractSocket::ConnectedState) {
        // logging
        return false;
    }
    if (uname.length() > 40 || pwd.length() > 40) {
        // logging
        return false;
    }
    char* buffer = (char *)malloc(100);
    buffer[0] = (char)Login;
    uint8_t uname_len = (uint8_t)uname.length();
    uint8_t pwd_len = (uint8_t)pwd.length();
    qDebug() << "uname_len:" << uname_len << " pwd_len:" << pwd_len;
    buffer[1] = (char)uname_len;
    strcpy(buffer+2, uname.toStdString().c_str());
    buffer[uname_len + 2] = pwd_len;
    strcpy(buffer + 3 + uname_len, pwd.toStdString().c_str());
    buffer[3 + uname_len + pwd_len] = '\0';
    const char* buffer_c = buffer;
    socket_.write(buffer_c);
}


