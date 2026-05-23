// ============================================================
// ImGuiIntegration.cpp
// ============================================================
#include "ImGuiIntegration.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "Core/Logger.h"

namespace USE {
    ImGuiIntegration& ImGuiIntegration::Get() { static ImGuiIntegration instance; return instance; }

    bool ImGuiIntegration::Initialize(SDL_Window* window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        if (!ImGui_ImplSDL2_InitForOpenGL(window, SDL_GL_GetCurrentContext())) {
            USE_LOG_ERROR("ImGui: Failed to init SDL2 backend");
            return false;
        }
        if (!ImGui_ImplOpenGL3_Init("#version 130")) {
            USE_LOG_ERROR("ImGui: Failed to init OpenGL3 backend");
            return false;
        }
        USE_LOG_INFO("ImGui initialized");
        return true;
    }

    void ImGuiIntegration::Shutdown() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiIntegration::BeginFrame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiIntegration::EndFrame() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void ImGuiIntegration::ProcessEvent(const SDL_Event& event) {
        ImGui_ImplSDL2_ProcessEvent(&event);
    }

    bool ImGuiIntegration::WantCaptureMouse() const { return ImGui::GetIO().WantCaptureMouse; }
    bool ImGuiIntegration::WantCaptureKeyboard() const { return ImGui::GetIO().WantCaptureKeyboard; }
}