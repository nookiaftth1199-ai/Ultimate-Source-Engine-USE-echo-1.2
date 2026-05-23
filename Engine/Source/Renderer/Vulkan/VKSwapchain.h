// ============================================================
// Ultimate Source Engine - Vulkan Swapchain
// ============================================================
//
// Manages the Vulkan swapchain, including creation,
// image acquisition, and presentation.
// ============================================================

#pragma once

#include "stdafx.h"
#include <vulkan/vulkan.h>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vector>

namespace USE {

    class VKSwapchain {
    public:
        VKSwapchain();
        ~VKSwapchain();

        // Initialize the swapchain (requires device, physical device, surface, window, vsync)
        bool Initialize(VkPhysicalDevice physicalDevice, VkDevice device,
                        VkSurfaceKHR surface, SDL_Window* window, bool vsync);
        void Shutdown();

        // Get the next image index, wait with semaphore
        VkResult AcquireNextImage(VkSemaphore semaphore, uint32_t& imageIndex);

        // Present the image
        VkResult Present(VkQueue presentQueue, uint32_t imageIndex, VkSemaphore waitSemaphore);

        // Getters
        VkSwapchainKHR      GetHandle() const        { return m_swapchain; }
        VkFormat            GetImageFormat() const   { return m_imageFormat; }
        VkExtent2D          GetExtent() const        { return m_extent; }
        uint32_t            GetImageCount() const    { return (uint32_t)m_images.size(); }
        const VkImageView*  GetImageViews() const    { return m_imageViews.data(); }
        VkImageView         GetImageView(uint32_t index) const { return m_imageViews[index]; }

    private:
        VkPhysicalDevice    m_physicalDevice;
        VkDevice            m_device;
        VkSurfaceKHR        m_surface;
        VkSwapchainKHR      m_swapchain;
        VkFormat            m_imageFormat;
        VkExtent2D          m_extent;

        std::vector<VkImage>       m_images;
        std::vector<VkImageView>   m_imageViews;

        bool CreateSwapchain(SDL_Window* window, bool vsync);
        void CreateImageViews();
        void DestroySwapchain();
    };

} // namespace USE