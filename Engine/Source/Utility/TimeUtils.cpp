// ============================================================
// TimeUtils.cpp
// ============================================================
#include "TimeUtils.h"
#include <thread>
#include <ctime>

namespace USE { namespace TimeUtils {
    double GetSeconds() {
        static auto start = std::chrono::high_resolution_clock::now();
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(now - start).count();
    }
    uint64_t GetMicroseconds() {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    }
    std::string GetTimestamp() {
        time_t t = time(nullptr);
        char buf[64];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
        return buf;
    }
    void SleepMilliseconds(unsigned int ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
}}