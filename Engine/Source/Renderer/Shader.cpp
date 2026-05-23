// ============================================================
// Ultimate Source Engine - Shader Implementation
// ============================================================

#include "stdafx.h"
#include "Shader.h"
#include "Core/Engine.h"
#include "Renderer/RenderSystem.h"
#include "Renderer/OpenGL/GLShader.h"
#include "Renderer/DirectX9/DXShader.h"
#include "Renderer/Vulkan/VKShader.h"

namespace USE {

    Shader* Shader::Create()
    {
        RenderSystem* renderer = Engine::Get()->GetRenderer();
        if (!renderer) return nullptr;

        RenderBackend backend = renderer->GetBackend();

        switch (backend) {
            case RenderBackend::OpenGL:
                return new GLShader();

            case RenderBackend::DirectX9:
                return new DXShader();

            case RenderBackend::Vulkan:
                return new VKShader();

            default:
                return nullptr;
        }
    }

} // namespace USE