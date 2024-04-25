#include "util.h"
#include <fstream>
#include <mutex>

std::mutex g_mutex; // Global mutex for thread safety
std::string LOG_FILE = "client.log";
FILETIME LONG_LONG_AGO = timestampToFileTime(10000000ULL);   // 1 seconds since January 1, 1601

void log_info(const std::string& message) {
    std::lock_guard<std::mutex> lock(g_mutex); // Lock the global mutex
    std::ofstream ofs(LOG_FILE, std::ios_base::app);
    if (!ofs) {
        qDebug() << QString::fromStdString("Error opening file: " + LOG_FILE);
        return;
    }

    std::time_t now = std::time(nullptr);
    std::string timestamp = std::asctime(std::localtime(&now));
    timestamp.erase(timestamp.find('\n')); // Remove the newline character

    ofs << "[" << timestamp << "] " << message << std::endl;
}

FILETIME MicrosecondsToFileTime(uint64_t microseconds) {
    uint64_t tmp;
    tmp = (microseconds + 11644473600ULL * 1000000ULL) * 10;  // Convert microseconds to 100-nanosecond intervals and adjust for the FILETIME epoch
    FILETIME filetime;
    filetime.dwLowDateTime = tmp & 0xFFFFFFFF;
    filetime.dwHighDateTime = static_cast<uint32_t>(tmp >> 32);
    return filetime;
}

uint64_t FileTimeToMicroseconds(const FILETIME& filetime) {
    uint64_t tmp;
    tmp = filetime.dwLowDateTime;
    tmp |= (static_cast<uint64_t>(filetime.dwHighDateTime) << 32);
    return tmp / 10 - 11644473600000000ULL;  // Convert 100-nanosecond intervals to microseconds and adjust for the Unix epoch
}
