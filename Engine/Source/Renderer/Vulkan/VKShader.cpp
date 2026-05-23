// ============================================================
// Ultimate Source Engine - Vulkan Shader Module Implementation
// ============================================================

#include "stdafx.h"
#include "VKShader.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"

namespace USE {

    VKShader::VKShader()
        : m_device(VK_NULL_HANDLE)
        , m_shaderModule(VK_NULL_HANDLE)
    {
    }

    VKShader::~VKShader()
    {
        Destroy();
    }

    bool VKShader::LoadFromFile(VkDevice device, const char* filename)
    {
        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("VKShader: FileSystem not available");
            return false;
        }

        std::string resolved = fs->ResolvePath(filename);
        if (resolved.empty()) {
            USE_LOG_ERROR("VKShader: File not found: %s", filename);
            return false;
        }

        auto file = fs->OpenFile(resolved.c_str(), FILE_READ | FILE_BINARY);
        if (!file) {
            USE_LOG_ERROR("VKShader: Failed to open file: %s", resolved.c_str());
            return false;
        }

        size_t fileSize = file->GetSize();
        if (fileSize == 0 || fileSize % sizeof(uint32_t) != 0) {
            USE_LOG_ERROR("VKShader: Invalid SPIR‑V file size (not multiple of 4)");
            return false;
        }

        std::vector<uint32_t> code(fileSize / sizeof(uint32_t));
        if (file->Read(code.data(), 1, code.size()) != code.size()) {
            USE_LOG_ERROR("VKShader: Failed to read file: %s", resolved.c_str());
            return false;
        }

        file->Close();

        return LoadFromMemory(device, code.data(), code.size());
    }

    bool VKShader::LoadFromMemory(VkDevice device, const uint32_t* code, size_t wordCount)
    {
        if (m_shaderModule != VK_NULL_HANDLE) {
            Destroy();
        }

        m_device = device;
        m_code.assign(code, code + wordCount);

        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = wordCount * sizeof(uint32_t);
        createInfo.pCode = code;

        if (vkCreateShaderModule(device, &createInfo, nullptr, &m_shaderModule) != VK_SUCCESS) {
            USE_LOG_ERROR("VKShader: Failed to create shader module");
            m_code.clear();
            return false;
        }

        USE_LOG_INFO("VKShader created, size %zu words", wordCount);
        return true;
    }

    void VKShader::Destroy()
    {
        if (m_shaderModule != VK_NULL_HANDLE) {
            vkDestroyShaderModule(m_device, m_shaderModule, nullptr);
            m_shaderModule = VK_NULL_HANDLE;
        }
        m_code.clear();
    }

    VkPipelineShaderStageCreateInfo VKShader::GetStageCreateInfo(VkShaderStageFlagBits stage) const
    {
        VkPipelineShaderStageCreateInfo stageInfo = {};
        stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageInfo.stage = stage;
        stageInfo.module = m_shaderModule;
        stageInfo.pName = "main";               // entry point
        // pSpecializationInfo can be set later if needed
        return stageInfo;
    }

} // namespace USE