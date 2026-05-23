// ============================================================
// DebugMenu.h
// ============================================================
#pragma once
#include <string>
#include <vector>
#include <functional>

namespace USE {
    class DebugMenu {
    public:
        static DebugMenu& Get();

        void AddEntry(const std::string& name, std::function<void()> callback);
        void RemoveEntry(const std::string& name);
        void Show() { m_visible = true; }
        void Hide() { m_visible = false; }
        void Toggle() { m_visible = !m_visible; }
        bool IsVisible() const { return m_visible; }
        void Update();
        void Draw();

    private:
        DebugMenu() = default;
        struct Entry { std::string name; std::function<void()> callback; };
        std::vector<Entry> m_entries;
        bool m_visible = false;
    };
}