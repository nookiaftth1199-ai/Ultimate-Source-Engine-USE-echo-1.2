// ============================================================
// Ultimate Source Engine - Vulkan Pipeline
// ============================================================
//
// Manages Vulkan graphics pipelines, including pipeline layout,
// shader stages, and state configuration.
// ============================================================

#pragma once

#include "stdafx.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace USE {

    // Forward declaration
    class VKShader;

    // Pipeline configuration structure
    struct VKPipelineConfig {
        // Shader stages
        VkShaderModule vertexShader;
        VkShaderModule fragmentShader;
        VkShaderModule geometryShader;   // optional, can be VK_NULL_HANDLE

        // Vertex input
        std::vector<VkVertexInputBindingDescription>   vertexBindings;
        std::vector<VkVertexInputAttributeDescription> vertexAttributes;

        // Input assembly
        VkPrimitiveTopology topology;
        bool primitiveRestartEnable;

        // Rasterization
        VkPolygonMode polygonMode;
        VkCullModeFlags cullMode;
        VkFrontFace frontFace;
        bool depthBiasEnable;

        // Multisampling
        VkSampleCountFlagBits rasterizationSamples;
        bool sampleShadingEnable;

        // Depth/stencil
        bool depthTestEnable;
        bool depthWriteEnable;
        VkCompareOp depthCompareOp;
        bool stencilTestEnable;

        // Color blending
        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
        bool blendConstants[4];  // not used often

        // Dynamic states
        std::vector<VkDynamicState> dynamicStates;

        // Pipeline layout (must be created separately)
        VkPipelineLayout layout;

        // Render pass and subpass
        VkRenderPass renderPass;
        uint32_t subpass;

        // Default constructor
        VKPipelineConfig();
    };

    class VKPipeline {
    public:
        VKPipeline();
        ~VKPipeline();

        // Create graphics pipeline from config
        bool Create(VkDevice device, const VKPipelineConfig& config);

        // Destroy pipeline
        void Destroy();

        // Bind pipeline to command buffer
        void Bind(VkCommandBuffer cmdBuffer, VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS);

        // Get Vulkan handle
        VkPipeline GetHandle() const { return m_pipeline; }

    private:
        VkDevice    m_device;
        VkPipeline  m_pipeline;
        bool        m_initialized;
    };

} // namespace USE