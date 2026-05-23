// ============================================================
// BinaryReader.cpp
// ============================================================
#include "BinaryReader.h"
#include <cstring>

namespace USE {
    BinaryReader::BinaryReader(const std::vector<uint8_t>& data) : m_data(data), m_pos(0) {}
    bool BinaryReader::Read(void* out, size_t size) {
        if (m_pos + size > m_data.size()) return false;
        memcpy(out, &m_data[m_pos], size);
        m_pos += size;
        return true;
    }
    bool BinaryReader::ReadString(std::string& out) {
        uint32_t len;
        if (!Read(len)) return false;
        out.resize(len);
        if (!Read(&out[0], len)) return false;
        return true;
    }
    void BinaryReader::Seek(size_t pos) { m_pos = pos; }
}