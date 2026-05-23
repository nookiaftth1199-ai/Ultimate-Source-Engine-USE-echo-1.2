// ============================================================
// Ultimate Source Engine - Vulkan Pipeline Implementation
// ============================================================

#include "stdafx.h"
#include "VKPipeline.h"
#include "Core/Logger.h"

namespace USE {

    // -----------------------------------------------------------------
    // VKPipelineConfig default values
    // -----------------------------------------------------------------
    VKPipelineConfig::VKPipelineConfig()
        : vertexShader(VK_NULL_HANDLE)
        , fragmentShader(VK_NULL_HANDLE)
        , geometryShader(VK_NULL_HANDLE)
        , topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        , primitiveRestartEnable(VK_FALSE)
        , polygonMode(VK_POLYGON_MODE_FILL)
        , cullMode(VK_CULL_MODE_BACK_BIT)
        , frontFace(VK_FRONT_FACE_CLOCKWISE)
        , depthBiasEnable(VK_FALSE)
        , rasterizationSamples(VK_SAMPLE_COUNT_1_BIT)
        , sampleShadingEnable(VK_FALSE)
        , depthTestEnable(VK_TRUE)
        , depthWriteEnable(VK_TRUE)
        , depthCompareOp(VK_COMPARE_OP_LESS)
        , stencilTestEnable(VK_FALSE)
        , layout(VK_NULL_HANDLE)
        , renderPass(VK_NULL_HANDLE)
        , subpass(0)
    {
        // Default color blend attachment (simple)
        VkPipelineColorBlendAttachmentState defaultBlend = {};
        defaultBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                       VK_COLOR_COMPONENT_G_BIT |
                                       VK_COLOR_COMPONENT_B_BIT |
                                       VK_COLOR_COMPONENT_A_BIT;
        defaultBlend.blendEnable = VK_FALSE;
        colorBlendAttachments.push_back(defaultBlend);

        // Default dynamic states (viewport and scissor)
        dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
        dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
    }

    // -----------------------------------------------------------------
    // VKPipeline implementation
    // -----------------------------------------------------------------
    VKPipeline::VKPipeline()
        : m_device(VK_NULL_HANDLE)
        , m_pipeline(VK_NULL_HANDLE)
        , m_initialized(false)
    {
    }

    VKPipeline::~VKPipeline()
    {
        Destroy();
    }

    bool VKPipeline::Create(VkDevice device, const VKPipelineConfig& config)
    {
        if (m_initialized) {
            USE_LOG_WARN("VKPipeline already created, destroying old one");
            Destroy();
        }

        m_device = device;

        // 1. Shader stages
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

        if (config.vertexShader != VK_NULL_HANDLE) {
            VkPipelineShaderStageCreateInfo vertStage = {};
            vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vertStage.module = config.vertexShader;
            vertStage.pName = "main";
            shaderStages.push_back(vertStage);
        }

        if (config.fragmentShader != VK_NULL_HANDLE) {
            VkPipelineShaderStageCreateInfo fragStage = {};
            fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            fragStage.module = config.fragmentShader;
            fragStage.pName = "main";
            shaderStages.push_back(fragStage);
        }

        if (config.geometryShader != VK_NULL_HANDLE) {
            VkPipelineShaderStageCreateInfo geomStage = {};
            geomStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            geomStage.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
            geomStage.module = config.geometryShader;
            geomStage.pName = "main";
            shaderStages.push_back(geomStage);
        }

        if (shaderStages.empty()) {
            USE_LOG_ERROR("VKPipeline::Create: no shader stages provided");
            return false;
        }

        // 2. Vertex input state
        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = (uint32_t)config.vertexBindings.size();
        vertexInputInfo.pVertexBindingDescriptions = config.vertexBindings.data();
        vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)config.vertexAttributes.size();
        vertexInputInfo.pVertexAttributeDescriptions = config.vertexAttributes.data();

        // 3. Input assembly state
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = config.topology;
        inputAssembly.primitiveRestartEnable = config.primitiveRestartEnable;

        // 4. Viewport state (dynamic)
        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;
        // viewports and scissors are dynamic, so we don't set them here

        // 5. Rasterization state
        VkPipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = config.polygonMode;
        rasterizer.cullMode = config.cullMode;
        rasterizer.frontFace = config.frontFace;
        rasterizer.depthBiasEnable = config.depthBiasEnable;
        rasterizer.depthBiasConstantFactor = 0.0f; // can be set via dynamic state
        rasterizer.depthBiasClamp = 0.0f;
        rasterizer.depthBiasSlopeFactor = 0.0f;
        rasterizer.lineWidth = 1.0f; // dynamic? could be, but we keep fixed

        // 6. Multisample state
        VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.rasterizationSamples = config.rasterizationSamples;
        multisampling.sampleShadingEnable = config.sampleShadingEnable;
        multisampling.minSampleShading = 1.0f; // optional
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;

        // 7. Depth/stencil state
        VkPipelineDepthStencilStateCreateInfo depthStencil = {};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = config.depthTestEnable;
        depthStencil.depthWriteEnable = config.depthWriteEnable;
        depthStencil.depthCompareOp = config.depthCompareOp;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = config.stencilTestEnable;
        // stencil ops can be set here if needed
        depthStencil.front.failOp = VK_STENCIL_OP_KEEP;
        depthStencil.front.passOp = VK_STENCIL_OP_KEEP;
        depthStencil.front.depthFailOp = VK_STENCIL_OP_KEEP;
        depthStencil.front.compareOp = VK_COMPARE_OP_ALWAYS;
        depthStencil.back = depthStencil.front;

        // 8. Color blend state
        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // optional
        colorBlending.attachmentCount = (uint32_t)config.colorBlendAttachments.size();
        colorBlending.pAttachments = config.colorBlendAttachments.data();
        // blend constants (if used) are dynamic

        // 9. Dynamic state
        VkPipelineDynamicStateCreateInfo dynamicState = {};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = (uint32_t)config.dynamicStates.size();
        dynamicState.pDynamicStates = config.dynamicStates.data();

        // 10. Pipeline layout (must be provided)
        if (config.layout == VK_NULL_HANDLE) {
            USE_LOG_ERROR("VKPipeline::Create: no pipeline layout provided");
            return false;
        }

        // 11. Render pass (must be provided)
        if (config.renderPass == VK_NULL_HANDLE) {
            USE_LOG_ERROR("VKPipeline::Create: no render pass provided");
            return false;
        }

        // Create pipeline
        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = (uint32_t)shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = config.layout;
        pipelineInfo.renderPass = config.renderPass;
        pipelineInfo.subpass = config.subpass;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline) != VK_SUCCESS) {
            USE_LOG_ERROR("VKPipeline::Create: failed to create graphics pipeline");
            return false;
        }

        m_initialized = true;
        USE_LOG_INFO("Vulkan graphics pipeline created.");
        return true;
    }

    void VKPipeline::Destroy()
    {
        if (m_device != VK_NULL_HANDLE && m_pipeline != VK_NULL_HANDLE) {
            vkDestroyPipeline(m_device, m_pipeline, nullptr);
            m_pipeline = VK_NULL_HANDLE;
        }
        m_initialized = false;
    }

    void VKPipeline::Bind(VkCommandBuffer cmdBuffer, VkPipelineBindPoint bindPoint)
    {
        if (m_pipeline != VK_NULL_HANDLE) {
            vkCmdBindPipeline(cmdBuffer, bindPoint, m_pipeline);
        }
    }

} // namespace USE