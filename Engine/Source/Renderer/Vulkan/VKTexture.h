// ============================================================
// Ultimate Source Engine - Vulkan Texture
// ============================================================
//
// Manages Vulkan image resources, including image creation,
// memory allocation, image view, and sampler.
// ============================================================

#pragma once

#include "stdafx.h"
#include <vulkan/vulkan.h>
#include <string>

namespace USE {

    // Forward declaration
    class VKBuffer;

    class VKTexture {
    public:
        VKTexture();
        ~VKTexture();

        // Load texture from file (requires staging via command pool)
        bool LoadFromFile(VkDevice device, VkPhysicalDevice physicalDevice,
                          VkCommandPool cmdPool, VkQueue graphicsQueue,
                          const char* filename, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM,
                          bool generateMipmaps = true);

        // Create texture from raw data (e.g., from memory)
        bool CreateFromData(VkDevice device, VkPhysicalDevice physicalDevice,
                            VkCommandPool cmdPool, VkQueue graphicsQueue,
                            uint32_t width, uint32_t height, VkFormat format,
                            const void* data, size_t dataSize,
                            bool generateMipmaps = true);

        // Create texture as a render target (no initial data)
        bool CreateRenderTarget(VkDevice device, VkPhysicalDevice physicalDevice,
                                uint32_t width, uint32_t height, VkFormat format,
                                VkImageUsageFlags additionalUsage = 0);

        // Create a sampler (defaults to linear filtering, repeat mode)
        bool CreateSampler(VkFilter magFilter = VK_FILTER_LINEAR,
                           VkFilter minFilter = VK_FILTER_LINEAR,
                           VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
                           VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                           float maxAnisotropy = 16.0f);

        // Destroy all Vulkan resources
        void Destroy();

        // Getters
        VkImage         GetImage() const       { return m_image; }
        VkImageView     GetImageView() const   { return m_imageView; }
        VkSampler       GetSampler() const     { return m_sampler; }
        VkFormat        GetFormat() const      { return m_format; }
        uint32_t        GetWidth() const       { return m_width; }
        uint32_t        GetHeight() const      { return m_height; }
        uint32_t        GetMipLevels() const   { return m_mipLevels; }

    private:
        VkDevice        m_device;
        VkPhysicalDevice m_physicalDevice;
        VkImage         m_image;
        VkDeviceMemory  m_memory;
        VkImageView     m_imageView;
        VkSampler       m_sampler;
        VkFormat        m_format;
        uint32_t        m_width;
        uint32_t        m_height;
        uint32_t        m_mipLevels;

        // Helper: transition image layout
        void TransitionLayout(VkCommandPool cmdPool, VkQueue queue,
                              VkImageLayout oldLayout, VkImageLayout newLayout);

        // Helper: copy buffer to image (for staging)
        void CopyBufferToImage(VkCommandPool cmdPool, VkQueue queue,
                               VkBuffer buffer, uint32_t width, uint32_t height);

        // Helper: generate mipmaps
        bool GenerateMipmaps(VkCommandPool cmdPool, VkQueue graphicsQueue);

        // Helper: find memory type
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    };

} // namespace USE