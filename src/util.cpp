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
