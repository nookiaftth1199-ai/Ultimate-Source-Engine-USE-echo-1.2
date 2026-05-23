// ============================================================
// DebugMenu.cpp
// ============================================================
#include "DebugMenu.h"
#include "Core/Logger.h"
#include <algorithm>

namespace USE {
    DebugMenu& DebugMenu::Get() { static DebugMenu instance; return instance; }

    void DebugMenu::AddEntry(const std::string& name, std::function<void()> callback) {
        RemoveEntry(name);
        m_entries.push_back({name, callback});
    }

    void DebugMenu::RemoveEntry(const std::string& name) {
        m_entries.erase(std::remove_if(m_entries.begin(), m_entries.end(),
            [&](const Entry& e) { return e.name == name; }), m_entries.end());
    }

    void DebugMenu::Update() {
        // In a real implementation, you'd handle keyboard input (e.g., F1 toggles).
        // For simplicity, we rely on external toggling.
    }

    void DebugMenu::Draw() {
        if (!m_visible) return;
        // Placeholder – you would use ImGui or a custom 2D renderer.
        USE_LOG_INFO("DebugMenu: draw %zu entries", m_entries.size());
    }
}