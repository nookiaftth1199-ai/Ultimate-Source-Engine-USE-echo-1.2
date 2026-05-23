// ============================================================
// Ultimate Source Engine - Vulkan Shader Module
// ============================================================
//
// Represents a Vulkan shader module loaded from SPIR-V binary.
// Provides methods to create shader stage info for pipeline creation.
// ============================================================

#pragma once

#include "stdafx.h"
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace USE {

    class VKShader {
    public:
        VKShader();
        ~VKShader();

        // Load SPIR-V binary from file (must be pre‑compiled)
        bool LoadFromFile(VkDevice device, const char* filename);

        // Load SPIR-V binary from memory (array of uint32_t words)
        bool LoadFromMemory(VkDevice device, const uint32_t* code, size_t wordCount);

        // Destroy the shader module
        void Destroy();

        // Get the Vulkan handle
        VkShaderModule GetHandle() const { return m_shaderModule; }

        // Create a VkPipelineShaderStageCreateInfo for a given shader stage.
        // The entry point is assumed to be "main".
        VkPipelineShaderStageCreateInfo GetStageCreateInfo(VkShaderStageFlagBits stage) const;

    private:
        VkDevice                m_device;
        VkShaderModule          m_shaderModule;
        std::vector<uint32_t>   m_code;   // retained for debugging (optional)
    };

} // namespace USE