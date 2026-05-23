// ============================================================
// TimeUtils.h
// ============================================================
#pragma once
#include <chrono>
#include <string>

namespace USE { namespace TimeUtils {
    double GetSeconds();
    uint64_t GetMicroseconds();
    std::string GetTimestamp();
    void SleepMilliseconds(unsigned int ms);
}}