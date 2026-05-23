// ============================================================
// BinaryWriter.h
// ============================================================
#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace USE {
    class BinaryWriter {
    public:
        void Write(const void* data, size_t size);
        template<typename T> void Write(const T& value) { Write(&value, sizeof(T)); }
        void WriteString(const std::string& str);
        const std::vector<uint8_t>& GetData() const { return m_data; }
        void Clear() { m_data.clear(); }
    private:
        std::vector<uint8_t> m_data;
    };
}