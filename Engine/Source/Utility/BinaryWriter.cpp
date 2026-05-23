// ============================================================
// BinaryWriter.cpp
// ============================================================
#include "BinaryWriter.h"
#include <cstring>

namespace USE {
    void BinaryWriter::Write(const void* data, size_t size) {
        const uint8_t* bytes = (const uint8_t*)data;
        m_data.insert(m_data.end(), bytes, bytes + size);
    }
    void BinaryWriter::WriteString(const std::string& str) {
        uint32_t len = (uint32_t)str.size();
        Write(len);
        Write(str.c_str(), len);
    }
}