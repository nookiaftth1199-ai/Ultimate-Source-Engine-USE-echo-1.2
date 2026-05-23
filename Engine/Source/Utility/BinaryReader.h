// ============================================================
// BinaryReader.h
// ============================================================
#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace USE {
    class BinaryReader {
    public:
        BinaryReader(const std::vector<uint8_t>& data);
        bool Read(void* out, size_t size);
        template<typename T> bool Read(T& out) { return Read(&out, sizeof(T)); }
        bool ReadString(std::string& out);
        size_t GetPosition() const { return m_pos; }
        void Seek(size_t pos);
        bool IsEOF() const { return m_pos >= m_data.size(); }
    private:
        const std::vector<uint8_t>& m_data;
        size_t m_pos;
    };
}