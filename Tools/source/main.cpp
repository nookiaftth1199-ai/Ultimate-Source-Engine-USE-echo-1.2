#include "stdafx.h"
#include "AssetBrowser.h"
#include "WorldEditor.h"

class ToolApplication : public Application {
public:
    ToolApplication() : Application("Ultimate Source Engine Tools", 1280, 720) {}

    bool Initialize() override {
        if (!Application::Initialize()) return false;

        // Initialize ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplSDL2_InitForOpenGL(m_pWindow->GetNativeWindow(), m_pRenderer->GetGLContext());
        ImGui_ImplOpenGL3_Init("#version 130");

        // Create tool windows
        m_assetBrowser = std::make_unique<AssetBrowser>();
        m_worldEditor = std::make_unique<WorldEditor>();

        return true;
    }

    void Shutdown() override {
        m_assetBrowser.reset();
        m_worldEditor.reset();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        Application::Shutdown();
    }

    void Update(float deltaTime) override {
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Show tool windows
        if (m_assetBrowser) m_assetBrowser->Draw();
        if (m_worldEditor) m_worldEditor->Draw();

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

private:
    std::unique_ptr<AssetBrowser> m_assetBrowser;
    std::unique_ptr<WorldEditor> m_worldEditor;
};

int main(int argc, char* argv[]) {
    ToolApplication app;
    if (!app.Initialize()) return -1;
    app.Run();
    return 0;
}