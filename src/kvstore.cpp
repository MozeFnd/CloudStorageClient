#include "kvstore.h"

KVStore::KVStore() {
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, "testdb", &db);
    if (!status.ok()) {
        qDebug() << QString::fromStdString("fail to open db file");
    }
}

std::string KVStore::read(std::string key){
    std::string value;
    leveldb::Status s = db->Get(leveldb::ReadOptions(), key, &value);
    if (!s.ok()) {
        qDebug() << QString::fromStdString("fail to read key " + key + "from db");
    }
    return value;
}

void KVStore::store(std::string key, std::string value) {
    leveldb::Status s = db->Put(leveldb::WriteOptions(), key, value);
    if (!s.ok()) {
        qDebug() << QString::fromStdString("fail to write kv pair <" + key + "," + value + "> into db");
    }
}

void KVStore::remove(std::string key){
    leveldb::Status s = db->Delete(leveldb::WriteOptions(), key);
    if (!s.ok()) {

    }
}

KVStore::~KVStore(){

}

// KVStore::KVStore() {
//     leveldb::Options options;
//     options.create_if_missing = true;
//     leveldb::Status status = leveldb::DB::Open(options, "testdb", &db);

//     const TCHAR* subKey = TEXT("Software\\XunCloudStorage\\client");
//     LONG openResult = RegCreateKeyEx(
//         HKEY_CURRENT_USER,  // Root key, like HKEY_CURRENT_USER or HKEY_LOCAL_MACHINE
//         subKey,
//         0,
//         NULL,
//         REG_OPTION_NON_VOLATILE,
//         KEY_ALL_ACCESS,
//         NULL,
//         &hKey,
//         NULL
//         );
//     if (openResult != ERROR_SUCCESS) {
//         qDebug() << "error opening db";
//     }
// }

// KVStore::STATUS KVStore::insert(QString key, QString value) {
//     std::wstring wstr = key.toStdWString();
//     const TCHAR* key_c = wstr.c_str();
//     std:: string str = value.toStdString();
//     LPCSTR value_c = str.c_str();
//     LONG writeResult = RegSetValueEx(
//         hKey,
//         key_c,
//         0,
//         REG_SZ,
//         reinterpret_cast<const BYTE*>(value_c),
//         strlen(value_c) + 1
//     );
//     if (writeResult != ERROR_SUCCESS) {
//         return KVStore::STATUS::FAIL;
//     }
//     return KVStore::STATUS::SUCCESS;
// }

// QString KVStore::read(QString key) {
//     std::wstring wstr = key.toStdWString();
//     const TCHAR* key_c = wstr.c_str();
//     DWORD valueType;
//     DWORD valueSize = MAX_PATH;
//     char valueBuffer[MAX_PATH];
//     LONG readResult = RegQueryValueEx(
//         hKey,
//         key_c,
//         NULL,
//         &valueType,
//         reinterpret_cast<LPBYTE>(valueBuffer),
//         &valueSize
//     );
//     QString ret = QString::fromStdString(std::string(valueBuffer));
//     return ret;
// }

// KVStore::~KVStore(){
//     RegCloseKey(hKey);
// }
