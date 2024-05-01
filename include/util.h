#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>
#include <iostream>
#include <windows.h>
#include <filesystem>
#include <fileapi.h>
#include <codecvt>
#include <QDebug>
#include <stdlib.h>
#include <mutex>
// #include <openssl/md5.h>

typedef std::unique_lock<std::mutex> Guard;

class LockManager{
public:
    LockManager(){}
    ~LockManager(){}
    std::mutex& get_lock(std::string key) {
        return locks[key];
    }

    void lock(std::string key) {
        Guard tmp_guard(manager_lock);
        locks[key].lock();
    }

    void unlock(std::string key) {
        Guard tmp_guard(manager_lock);
        locks[key].unlock();
    }
private:
    std::mutex manager_lock;
    std::unordered_map<std::string, std::mutex> locks;
};

extern FILETIME LONG_LONG_AGO;

void log_info(const std::string& message);

FILETIME MicrosecondsToFileTime(uint64_t microseconds);

uint64_t FileTimeToMicroseconds(const FILETIME& filetime);

inline std::vector<std::string> splitStr(std::string str, char delimiter) {
    std::vector<std::string> arr;
    std::string cur = str;
    while (true) {
        size_t slash_pos = cur.find(delimiter);
        if (slash_pos == std::string::npos) {
            arr.push_back(cur);
            break;
        } else {
            std::string tmp = cur.substr(0, slash_pos);
            arr.push_back(tmp);
            cur = cur.substr(slash_pos+1);
        }
    }
    return arr;
}

inline std::string replace_char(std::string str, char init, char target){
    std::string ret = str;
    for (int i = 0;i < ret.size();i++) {
        if (ret[i] == init) {
            ret[i] = target;
        }
    }
    return ret;
}

// inline std::wstring str2wstr(std::string str){
//     std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
//     std::wstring wstr = converter.from_bytes(str);
//     return wstr;
// }

// inline std::string wstr2str(const std::wstring wstr)
// {
//     std::string str;
//     int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL,NULL);
//     char*buffer = new char[len + 1];
//     WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
//     buffer[len] = '\0';
//     str.append(buffer);
//     delete[]buffer;
//     return str;
// }

inline std::wstring str2wstr(std::string str){
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
    return converterX.from_bytes(str);
}

inline std::string wstr2str(const std::wstring wstr)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
    return converterX.to_bytes(wstr);
}

union WCharUnion {
    wchar_t wc;  // 宽字符
#ifdef __linux__
    struct {
        uint8_t byte1;  // 第1个字节
        uint8_t byte2;  // 第2个字节
        uint8_t byte3;  // 第3个字节
        uint8_t byte4;  // 第4个字节
    } bytes;
#else
    struct {
        uint8_t byte1;  // 第1个字节
        uint8_t byte2;  // 第2个字节
    } bytes;
#endif

};

inline std::string wstr2bytes(const std::wstring& wstr) {
    size_t buf_len = wstr.size() * 2;
    qDebug() << "buf_len: " << buf_len;
    char* buffer = (char*)malloc(buf_len);
    uint32_t idx = 0;
    for (auto w_ch : wstr) {
        WCharUnion wu;
        wu.wc = w_ch;
        char tmp1 = wu.bytes.byte1;
        char tmp2 = wu.bytes.byte2;
        buffer[idx++] = tmp1;
        buffer[idx++] = tmp2;
    }
    std::string ret(buffer, buf_len);
    free(buffer);
    return ret;
}

inline FILETIME timestampToFileTime(uint64_t timestamp) {
    FILETIME ft;
    // Convert timestamp to 100-nanosecond intervals
    ULONGLONG intervals = timestamp * 10000000ULL; // 100-nanosecond intervals in a second
    // Assign the intervals to the FILETIME structure
    ft.dwLowDateTime = static_cast<DWORD>(intervals);
    ft.dwHighDateTime = static_cast<DWORD>(intervals >> 32);
    return ft;
}

inline std::string format_filetime(FILETIME ftime){
    ULARGE_INTEGER tmp;
    tmp.LowPart = ftime.dwLowDateTime;
    tmp.HighPart = ftime.dwHighDateTime;
    time_t modifiedTime = static_cast<time_t>((tmp.QuadPart - 116444736000000000ULL) / 10000000ULL);
    return std::string(std::ctime(&modifiedTime));
}

inline FILETIME file_time_type2FILETIME(const std::filesystem::file_time_type& ft) {
    // Convert file_time_type to duration since epoch
    auto dur = ft.time_since_epoch();

    // Convert duration to milliseconds (100 nanoseconds per tick for FILETIME)
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(dur);

    // Convert milliseconds to 100 nanoseconds intervals
    ULONGLONG interval = millis.count() * 10000;

    // Create FILETIME structure
    FILETIME fileTime;
    fileTime.dwLowDateTime = interval & 0xFFFFFFFF;
    fileTime.dwHighDateTime = interval >> 32;

    return fileTime;
}

// 参考 https://learn.microsoft.com/en-us/windows/win32/sysinfo/retrieving-the-last-write-time
inline FILETIME get_last_modified(std::wstring filename){
    HANDLE hFile;
    const wchar_t* wcstr = filename.c_str();
    TCHAR* tmp = const_cast<TCHAR*>(wcstr);
    hFile = CreateFile(tmp, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, 0, NULL);

    if (hFile != INVALID_HANDLE_VALUE) {
        FILETIME ftCreate, ftAccess, ftWrite;

        // Retrieve the file times for the file.
        if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)) {
            log_info("Failed to get last modified time, filename:" + wstr2str(filename));
            return LONG_LONG_AGO;
        }
        std::string to_log = "Last modified time of " + wstr2str(filename) + format_filetime(ftWrite);
        log_info(to_log);
        CloseHandle(hFile);
        return ftWrite;
    } else {
        log_info("Failed to open file " + wstr2str(filename));
        return LONG_LONG_AGO;
    }
}

inline FILETIME get_dir_last_modified(std::wstring path) {
    std::filesystem::file_time_type ftime = std::filesystem::last_write_time(path);
    qDebug() << ftime.time_since_epoch().count();
    auto ret = file_time_type2FILETIME(ftime);
    return ret;
}

inline void set_last_modified(std::wstring filename, FILETIME new_time){
    HANDLE hFile;
    const wchar_t* wcstr = filename.c_str();
    TCHAR* tmp = const_cast<TCHAR*>(wcstr);
    hFile = CreateFile(tmp, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, 0, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        FILETIME ftCreate, ftAccess, ftWrite;
        ftCreate = new_time;
        ftAccess = new_time;
        ftWrite = new_time;
        if (!SetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)){
            log_info("Failed to set last modified time of file " + wstr2str(filename));
        } else {
            log_info("The last modified time of file " + wstr2str(filename) + " is set to " + format_filetime(new_time));
        }
        CloseHandle(hFile);
    } else {
        log_info("Failed to open file.");
    }
}

inline FILETIME max_filetime(FILETIME t1, FILETIME t2) {
    int result = CompareFileTime(&t1, &t2);
    if (result < 0) {
        return t2;
    }
    return t1;
}

inline std::string calculate_MD5() {

}

#endif // UTIL_H
