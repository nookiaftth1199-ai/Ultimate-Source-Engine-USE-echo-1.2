// ============================================================
// Ultimate Source Engine - Vulkan Buffer
// ============================================================
//
// Manages Vulkan buffer objects (vertex, index, uniform, etc.)
// ============================================================

#pragma once

#include "stdafx.h"
#include <vulkan/vulkan.h>

namespace USE {

    class VKBuffer {
    public:
        VKBuffer();
        ~VKBuffer();

        // Create buffer with specified size, usage, and memory properties.
        // If initialData is provided, the buffer will be filled (requires host visible memory).
        bool Initialize(VkDevice device, VkPhysicalDevice physicalDevice,
                        VkDeviceSize size, VkBufferUsageFlags usage,
                        VkMemoryPropertyFlags properties,
                        const void* initialData = nullptr);

        // Destroy the buffer and free memory
        void Destroy();

        // Map the buffer memory to host address space
        void* Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        // Unmap the buffer memory
        void Unmap();

        // Flush mapped memory (if not host coherent)
        void Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        // Invalidate mapped memory (if not host coherent)
        void Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        // Update buffer data via mapping (simple version)
        bool Update(const void* data, VkDeviceSize size, VkDeviceSize offset = 0);

        // Bind as vertex buffer (call inside command buffer recording)
        void BindAsVertex(VkCommandBuffer cmdBuffer, uint32_t binding, VkDeviceSize offset = 0);

        // Bind as index buffer (call inside command buffer recording)
        void BindAsIndex(VkCommandBuffer cmdBuffer, VkIndexType indexType = VK_INDEX_TYPE_UINT32, VkDeviceSize offset = 0);

        // Getters
        VkBuffer GetHandle() const { return m_buffer; }
        VkDeviceMemory GetMemory() const { return m_memory; }
        VkDeviceSize GetSize() const { return m_size; }

    private:
        VkDevice           m_device;
        VkPhysicalDevice   m_physicalDevice;
        VkBuffer           m_buffer;
        VkDeviceMemory     m_memory;
        VkDeviceSize       m_size;
        VkBufferUsageFlags m_usage;
        VkMemoryPropertyFlags m_properties;
        void*              m_mappedData;

        // Find memory type that satisfies requirements
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    };

} // namespace USE