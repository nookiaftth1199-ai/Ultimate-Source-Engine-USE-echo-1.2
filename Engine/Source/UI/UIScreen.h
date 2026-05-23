// ============================================================
// Ultimate Source Engine - UI Screen
//============================================================
//
// A top-level UI container representing a distinct screen
// (e.g., main menu, options, gameplay HUD). Provides lifecycle
// callbacks and input handling.
// ============================================================

#pragma once

#include "UIElement.h"
#include <string>

namespace USE {

    class UIScreen : public UIElement {
    public:
        UIScreen(const std::string& name = "Screen");
        virtual ~UIScreen();

        // Screen name (for identification)
        const std::string& GetName() const { return m_name; }
        void SetName(const std::string& name) { m_name = name; }

        // Lifecycle methods (override in derived screens)
        virtual void OnShow();                // Called when screen becomes active
        virtual void OnHide();                // Called when screen is deactivated
        virtual void OnUpdate(float deltaTime); // Called every frame while active
        virtual void OnRender(IRenderDevice* device, Font* defaultFont); // Called every frame while active

        // Override UIElement methods to integrate lifecycle calls
        void Update(float deltaTime) override;
        void Render(IRenderDevice* device, Font* defaultFont) override;

        // Input handling – override to intercept events before children
        virtual bool OnMouseMove(float x, float y) override;
        virtual bool OnMouseButton(int button, bool down, float x, float y) override;
        virtual bool OnKeyPress(int key, bool down) override;
        virtual bool OnCharInput(unsigned int codepoint) override;

    protected:
        std::string m_name;
    };

} // namespace USE