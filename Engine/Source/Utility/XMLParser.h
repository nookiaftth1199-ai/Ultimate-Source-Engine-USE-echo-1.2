// ============================================================
// XMLParser.h
// ============================================================
#pragma once
#include <string>

namespace USE {
    class XMLNode {
    public:
        virtual ~XMLNode() = default;
        virtual const char* GetName() const = 0;
        virtual const char* GetText() const = 0;
        virtual XMLNode* GetFirstChild(const char* name = nullptr) = 0;
        virtual XMLNode* GetNextSibling(const char* name = nullptr) = 0;
    };

    class XMLParser {
    public:
        static XMLNode* ParseFile(const std::string& filename);
        static void FreeNode(XMLNode* node);
    };
}