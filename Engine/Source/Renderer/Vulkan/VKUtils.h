// ============================================================
// Ultimate Source Engine - Vulkan Utilities
// ============================================================
//
// Collection of helper functions for Vulkan development.
// Includes error checking, memory management, and common operations.
// ============================================================

#pragma once

#include "stdafx.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <fstream>
#include <cassert>

namespace USE {
namespace VKUtils {

    // -----------------------------------------------------------------
    // Error checking
    // -----------------------------------------------------------------
    // Simple check macro that asserts on failure.
    #define VK_CHECK(result) \
        do { \
            VkResult res = (result); \
            if (res != VK_SUCCESS) { \
                USE_LOG_ERROR("Vulkan error %d at %s:%d", res, __FILE__, __LINE__); \
                assert(res == VK_SUCCESS); \
            } \
        } while (0)

    // -----------------------------------------------------------------
    // Debug messenger (for validation layers)
    // -----------------------------------------------------------------
    VkResult CreateDebugUtilsMessenger(VkInstance instance,
                                       PFN_vkDebugUtilsMessengerCallbackEXT callback,
                                       VkDebugUtilsMessengerEXT* pMessenger,
                                       void* pUserData = nullptr);

    void DestroyDebugUtilsMessenger(VkInstance instance, VkDebugUtilsMessengerEXT messenger);

    // -----------------------------------------------------------------
    // Memory management
    // -----------------------------------------------------------------
    // Find memory type that matches required properties
    uint32_t FindMemoryType(VkPhysicalDevice physicalDevice,
                            uint32_t typeFilter,
                            VkMemoryPropertyFlags properties);

    // -----------------------------------------------------------------
    // Shader module loading
    // -----------------------------------------------------------------
    // Read SPIR-V file into a vector of uint32_t
    std::vector<uint32_t> ReadSPIRVFile(const std::string& filename);

    // Create shader module from SPIR-V file
    VkShaderModule CreateShaderModule(VkDevice device, const std::string& filename);

    // Create shader module from SPIR-V code
    VkShaderModule CreateShaderModule(VkDevice device, const uint32_t* code, size_t wordCount);

    // -----------------------------------------------------------------
    // Image layout transitions
    // -----------------------------------------------------------------
    void TransitionImageLayout(VkDevice device,
                               VkCommandPool commandPool,
                               VkQueue queue,
                               VkImage image,
                               VkFormat format,
                               VkImageLayout oldLayout,
                               VkImageLayout newLayout,
                               uint32_t mipLevels = 1,
                               uint32_t layerCount = 1);

    // -----------------------------------------------------------------
    // Buffer to image copy
    // -----------------------------------------------------------------
    void CopyBufferToImage(VkDevice device,
                           VkCommandPool commandPool,
                           VkQueue queue,
                           VkBuffer buffer,
                           VkImage image,
                           uint32_t width,
                           uint32_t height,
                           uint32_t layerCount = 1);

    // -----------------------------------------------------------------
    // Mipmap generation
    // -----------------------------------------------------------------
    void GenerateMipmaps(VkDevice device,
                         VkPhysicalDevice physicalDevice,
                         VkCommandPool commandPool,
                         VkQueue queue,
                         VkImage image,
                         VkFormat format,
                         uint32_t width,
                         uint32_t height,
                         uint32_t mipLevels);

    // -----------------------------------------------------------------
    // Single-time command buffer helpers
    // -----------------------------------------------------------------
    VkCommandBuffer BeginSingleTimeCommands(VkDevice device, VkCommandPool commandPool);
    void EndSingleTimeCommands(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkCommandBuffer commandBuffer);

    // -----------------------------------------------------------------
    // Queue family helpers
    // -----------------------------------------------------------------
    // Find queue family index with desired capabilities
    uint32_t FindQueueFamily(VkPhysicalDevice device,
                             VkQueueFlags requiredFlags,
                             VkSurfaceKHR surface = VK_NULL_HANDLE,
                             bool requirePresent = false);

    // -----------------------------------------------------------------
    // Format helpers
    // -----------------------------------------------------------------
    bool HasStencilComponent(VkFormat format);
    uint32_t FormatSize(VkFormat format); // bytes per texel (approx)

} // namespace VKUtils
} // namespace USE