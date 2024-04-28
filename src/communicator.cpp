#include "communicator.h"
#include <QThread>

Communicator::Communicator(std::shared_ptr<KVStore> kvstore) {
    kvstore_ = kvstore;
}

Communicator::~Communicator(){
    if (socket_.state() == QAbstractSocket::ConnectedState){
        socket_.close();
    }
}

bool Communicator::buildConnection() {
    QString ip = "111.230.93.167";
    quint32 port = 6900;
    socket_.connectToHost(ip, port); // Adjust the IP and port as needed
    if (!socket_.waitForConnected()) {
        qDebug() << "Error: " << socket_.errorString();
        return false;
    }
    if (socket_.state() != QAbstractSocket::ConnectedState) {
        // logging
        return false;
    }
    return true;
}

void Communicator::closeConnection(){
    if (socket_.state() != QAbstractSocket::ConnectedState) {
        // logging
        return;
    }
    socket_.close();
    // socket_.disconnectFromHost();
    // if (!socket_.waitForDisconnected()) {
    //     qDebug() << "Error: " << socket_.errorString();
    //     return;
    // }
}

void Communicator::blockRead(char* buffer, int n) {
    if (socket_.waitForReadyRead()) {
        int acc = 0;
        while (acc < n) {
            acc += socket_.read(buffer + acc, n - acc);
        }
    }
}

void Communicator::blockWrite(char* buffer, int n) {
    int acc = 0;
    while (acc < n) {
        acc += socket_.write(buffer + acc, n - acc);
    }
    if (!socket_.waitForBytesWritten(300)) {
        qDebug() << "writting bytes fail";
    }
    // socket_.flush();
}

// ┌────────────┐
// │Login(1byte)│
// ├────────────┴───┬────────────────────────┐
// │uname_len(1byte)│          uname         │
// ├────────────────┼────────────────────────┤
// │ pwd_len(1byte) │           pwd          │
// └────────────────┴────────────────────────┘
bool Communicator::login(QString uname, QString pwd){
    if (buildConnection() == false) {
        return false;
    }
    if (uname.length() > 40 || pwd.length() > 40) {
        // logging
        closeConnection();
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
    socket_.flush();

    free(buffer);
    closeConnection();
    return true;
}

// ┌───────────┐
// │len(8bytes)│
// ├───────────┴────────────┐
// │      id(32bytes)       │
// ├────────────────────────┤
// │      id(32bytes)       │
// └────────────────────────┘

uint32_t Communicator::acquireID() {
    if (buildConnection() == false) {
        return 0;
    }
    size_t buffer_size = 10;
    char* buffer = (char *)malloc(buffer_size);
    buffer[0] = (char)AcquireID;
    blockWrite(buffer, 1);
    memset(buffer, 0, buffer_size);
    // if (socket_.waitForReadyRead()) {
    //     socket_.read(buffer, 1);
    // }
    // if (socket_.waitForReadyRead()) {
    //     socket_.read(buffer, 1);
    // } else {
    //     qDebug() << "Error: " << socket_.errorString();
    // }
    blockRead(buffer, 4);
    uint32_t id = *(uint32_t*)buffer;
    free(buffer);
    closeConnection();
    return id;
}

void Communicator::remoteAddNewDirectory(uint32_t id, std::string serialized) {
    if (buildConnection() == false) {
        return;
    }

    auto data = serialized;
    auto buffer_size = 5 + data.size();
    char* buffer = (char*)malloc(buffer_size);

    *buffer = AddDir;
    *(uint32_t*)(buffer + 1) = data.size();
    // strcpy(buffer + 5, data.c_str());
    memcpy(buffer + 5, data.c_str(), data.size());
    QString tmp(buffer);
    qDebug() << tmp;
    blockWrite(buffer, buffer_size);

    free(buffer);
    closeConnection();
}

// void Communicator::addNewDirectory(uint32_t id, std::string name, std::shared_ptr<Json> archJs) {
//     if (buildConnection() == false) {
//         return;
//     }

//     Json js;
//     js.addProperty("device", "max_sun");
//     js.addProperty("id", std::to_string(id));
//     js.addProperty("name", name);
//     js.addChild("arch", archJs);
//     auto data = js.toString();
//     auto buffer_size = 5 + data.size();
//     char* buffer = (char*)malloc(buffer_size);

//     *buffer = AddDir;
//     *(uint32_t*)(buffer + 1) = data.size();

//     strcpy(buffer + 5, data.c_str());
//     QString tmp(buffer);
//     qDebug() << tmp;
//     socket_.write(buffer, buffer_size);
//     socket_.flush();

//     free(buffer);
//     closeConnection();
// }

// ┌───────────────┬─────────────────────────┐
// │SyncFile(1byte)│    json_size(4bytes)    │
// ├───────────────┴─────────────────────────┤
// │          json(id,relative_path)         │
// └─────────────────────────────────────────┘
// ┌────────────────────────────────┬───────────────┐
// │        data_size(4bytes)       │ is_last(1byte)│
// ├────────────────────────────────┴───────────────┤
// │                     data                       │
// │                                                │
// └────────────────────────────────────────────────┘
void Communicator::syncFile(uint32_t id, std::wstring wfilepath, std::string relativePath) {
    if (!buildConnection()) {
        // logging;
        return;
    }
    int max_batch_size = 1000;
    char* buffer = (char*)malloc(max_batch_size + 5);
    buffer[0] = SyncFile;
    Json js;
    js.addProperty("id", std::to_string(id));
    js.addProperty("relative_path", relativePath);
    auto jsonStr = js.toString();
    *(uint32_t*)(buffer+1) = jsonStr.size();
    strcpy(buffer + 5, jsonStr.c_str());
    blockWrite(buffer, jsonStr.size() + 5);
    memset(buffer, 0, jsonStr.size() + 5);

    std::filesystem::path w_path(wfilepath);
    std::ifstream file(w_path, std::ios::binary);
    if (!file.is_open()) {
        qDebug() << "fail to open file " << wfilepath;
        closeConnection();
    }
    int cnt = 0;
    while (!file.eof()) {
        cnt++;
        // if (cnt % 100 == 0) {
        //     QThread::msleep(200);
        // }
        file.read(buffer + 5, max_batch_size);
        int bytesRead = file.gcount();
        if (bytesRead < max_batch_size) {
            *(buffer + 4) = 1;
        } else {
            *(buffer + 4) = 0;
        }
        *(uint32_t*)buffer = bytesRead;
        blockWrite(buffer, bytesRead + 5);
        memset(buffer, 0, bytesRead + 5);
    }
    file.close();

    free(buffer);
    closeConnection();
}

std::unordered_map<uint32_t, std::string> Communicator::getAllDirIDandName() {
    if (buildConnection() == false) {
        return {};
    }

    char* buffer = (char *)malloc(1000);
    buffer[0] = (char)GetAllDir;
    blockWrite(buffer, 1);

    if (socket_.waitForReadyRead()) {
        blockRead(buffer, 4);
    } else {
        qDebug() << "Error: " << socket_.errorString();
    }

    uint32_t len = *(uint32_t*)buffer;
    blockRead(buffer, len);
    closeConnection();

    std::vector<uint32_t> id;
    for (size_t i = 0;i < len;i++) {
        uint32_t tmp = reinterpret_cast<uint32_t*>(buffer)[i];
        id.push_back(tmp);
    }
    std::unordered_map<uint32_t, std::string> ret;

    free(buffer);
    return ret;
}


