// ============================================================
// Ultimate Source Engine - Vulkan Swapchain Implementation
// ============================================================

#include "stdafx.h"
#include "VKSwapchain.h"
#include "Core/Logger.h"
#include <algorithm>

namespace USE {

    VKSwapchain::VKSwapchain()
        : m_physicalDevice(VK_NULL_HANDLE)
        , m_device(VK_NULL_HANDLE)
        , m_surface(VK_NULL_HANDLE)
        , m_swapchain(VK_NULL_HANDLE)
        , m_imageFormat(VK_FORMAT_UNDEFINED)
        , m_extent{0, 0}
    {
    }

    VKSwapchain::~VKSwapchain()
    {
        Shutdown();
    }

    bool VKSwapchain::Initialize(VkPhysicalDevice physicalDevice, VkDevice device,
                                 VkSurfaceKHR surface, SDL_Window* window, bool vsync)
    {
        m_physicalDevice = physicalDevice;
        m_device = device;
        m_surface = surface;

        if (!CreateSwapchain(window, vsync)) {
            return false;
        }
        CreateImageViews();

        USE_LOG_INFO("Vulkan swapchain created: %u images, format %d, extent %dx%d",
                     (uint32_t)m_images.size(), m_imageFormat, m_extent.width, m_extent.height);
        return true;
    }

    void VKSwapchain::Shutdown()
    {
        DestroySwapchain();
    }

    VkResult VKSwapchain::AcquireNextImage(VkSemaphore semaphore, uint32_t& imageIndex)
    {
        return vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX,
                                      semaphore, VK_NULL_HANDLE, &imageIndex);
    }

    VkResult VKSwapchain::Present(VkQueue presentQueue, uint32_t imageIndex, VkSemaphore waitSemaphore)
    {
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &waitSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_swapchain;
        presentInfo.pImageIndices = &imageIndex;

        return vkQueuePresentKHR(presentQueue, &presentInfo);
    }

    bool VKSwapchain::CreateSwapchain(SDL_Window* window, bool vsync)
    {
        // 1. Query surface capabilities
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, m_surface, &capabilities);

        // 2. Choose surface format
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &formatCount, nullptr);
        std::vector<VkSurfaceFormatKHR> formats(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &formatCount, formats.data());

        // Prefer VK_FORMAT_B8G8R8A8_UNORM with SRGB nonlinear
        VkSurfaceFormatKHR desiredFormat = formats[0];
        for (const auto& fmt : formats) {
            if (fmt.format == VK_FORMAT_B8G8R8A8_UNORM &&
                fmt.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                desiredFormat = fmt;
                break;
            }
        }
        m_imageFormat = desiredFormat.format;

        // 3. Choose present mode
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, nullptr);
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, presentModes.data());

        VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR; // FIFO always available
        if (!vsync) {
            for (auto mode : presentModes) {
                if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                    presentMode = mode;
                    break;
                }
                if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                    presentMode = mode; // mailbox is better than immediate
                }
            }
        }

        // 4. Choose extent (use window size)
        int width, height;
        SDL_GetWindowSize(window, &width, &height);
        VkExtent2D extent;
        if (capabilities.currentExtent.width != UINT32_MAX) {
            extent = capabilities.currentExtent;
        } else {
            extent.width = std::max(capabilities.minImageExtent.width,
                                     std::min(capabilities.maxImageExtent.width, (uint32_t)width));
            extent.height = std::max(capabilities.minImageExtent.height,
                                      std::min(capabilities.maxImageExtent.height, (uint32_t)height));
        }
        m_extent = extent;

        // 5. Image count (prefer triple buffering)
        uint32_t imageCount = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
            imageCount = capabilities.maxImageCount;
        }

        // 6. Create swapchain
        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = m_imageFormat;
        createInfo.imageColorSpace = desiredFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        // Get queue families for sharing mode
        uint32_t graphicsFamily = VK_QUEUE_FAMILY_IGNORED;
        uint32_t presentFamily  = VK_QUEUE_FAMILY_IGNORED;
        // We need a way to get these indices. We'll assume they are stored somewhere.
        // For simplicity, we'll just use exclusive mode if they are the same, concurrent otherwise.
        // This requires that the device knows the queue families. We'll need to pass them or store in device.
        // For now, we'll use exclusive mode (most common).

        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;

        createInfo.preTransform = capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain) != VK_SUCCESS) {
            USE_LOG_ERROR("VKSwapchain: Failed to create swapchain");
            return false;
        }

        // 7. Retrieve images
        vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr);
        m_images.resize(imageCount);
        vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, m_images.data());

        return true;
    }

    void VKSwapchain::CreateImageViews()
    {
        m_imageViews.resize(m_images.size());
        for (size_t i = 0; i < m_images.size(); ++i) {
            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = m_images[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = m_imageFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_device, &viewInfo, nullptr, &m_imageViews[i]) != VK_SUCCESS) {
                USE_LOG_ERROR("VKSwapchain: Failed to create image view %zu", i);
                m_imageViews[i] = VK_NULL_HANDLE;
            }
        }
    }

    void VKSwapchain::DestroySwapchain()
    {
        if (m_device) {
            for (auto iv : m_imageViews) {
                vkDestroyImageView(m_device, iv, nullptr);
            }
            m_imageViews.clear();

            if (m_swapchain) {
                vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
                m_swapchain = VK_NULL_HANDLE;
            }
        }
        m_images.clear();
    }

} // namespace USE