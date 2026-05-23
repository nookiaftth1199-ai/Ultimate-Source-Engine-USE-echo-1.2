// ============================================================
// Ultimate Source Engine - Vulkan Render Device
// ============================================================
//
// Implements the IRenderDevice interface using Vulkan.
// Provides rendering context management and low-level drawing.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Renderer/IRenderDevice.h"
#include "Math/Color.h"
#include "Math/Matrix4.h"
#include <vulkan/vulkan.h>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vector>

namespace USE {

    class VKDevice : public IRenderDevice {
    public:
        VKDevice();
        virtual ~VKDevice();

        // IRenderDevice implementation
        bool Initialize(Window* window, bool vsync) override;
        void Shutdown() override;

        void BeginFrame() override;
        void EndFrame() override;
        void Present() override;

        void Clear(uint32_t flags, const Color& color, float depth, uint32_t stencil) override;
        void SetViewport(int x, int y, int width, int height) override;
        void SetScissorRect(int x, int y, int width, int height) override;
        void EnableScissor(bool enable) override;

        void DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, uint32_t baseVertexLocation) override;

        // Resource creation stubs
        bool CreateVertexBuffer(const void* data, size_t size, uint32_t& bufferHandle) override;
        bool CreateIndexBuffer(const void* data, size_t size, uint32_t& bufferHandle) override;
        bool CreateTexture2D(int width, int height, TextureFormat format, const void* data, uint32_t& textureHandle) override;
        bool CreateShader(ShaderType type, const char* source, uint32_t& shaderHandle) override;

        void DestroyBuffer(uint32_t handle) override;
        void DestroyTexture(uint32_t handle) override;
        void DestroyShader(uint32_t handle) override;

        // Vulkan-specific access
        VkInstance       GetInstance() const       { return m_instance; }
        VkPhysicalDevice GetPhysicalDevice() const { return m_physicalDevice; }
        VkDevice         GetDevice() const         { return m_device; }
        VkQueue          GetGraphicsQueue() const  { return m_graphicsQueue; }
        VkQueue          GetPresentQueue() const   { return m_presentQueue; }
        VkSwapchainKHR   GetSwapchain() const      { return m_swapchain; }
        VkRenderPass     GetRenderPass() const     { return m_renderPass; }

    private:
        // Vulkan handles
        VkInstance                 m_instance;
        VkPhysicalDevice           m_physicalDevice;
        VkDevice                   m_device;
        VkQueue                    m_graphicsQueue;
        VkQueue                    m_presentQueue;
        VkSurfaceKHR               m_surface;
        VkSwapchainKHR             m_swapchain;
        VkRenderPass               m_renderPass;
        VkPipelineLayout           m_pipelineLayout;
        VkPipeline                 m_pipeline;
        std::vector<VkImageView>   m_swapchainImageViews;
        std::vector<VkFramebuffer> m_swapchainFramebuffers;

        VkCommandPool              m_commandPool;
        std::vector<VkCommandBuffer> m_commandBuffers;

        VkSemaphore                m_imageAvailableSemaphore;
        VkSemaphore                m_renderFinishedSemaphore;
        VkFence                    m_inFlightFence;

        // Window and surface
        SDL_Window*                m_window;
        bool                       m_vsync;

        // Initialization helpers
        bool CreateInstance();
        bool CreateSurface(SDL_Window* window);
        bool SelectPhysicalDevice();
        bool CreateLogicalDevice();
        bool CreateSwapchain();
        bool CreateRenderPass();
        bool CreatePipeline();
        bool CreateFramebuffers();
        bool CreateCommandPool();
        bool CreateCommandBuffers();
        bool CreateSyncObjects();

        void DestroySwapchain();
        void Cleanup();

        // Helper to find queue families
        uint32_t FindQueueFamily(VkQueueFlags flags, bool present = false);
    };

} // namespace USE