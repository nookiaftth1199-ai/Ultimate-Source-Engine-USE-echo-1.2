// ============================================================
// Ultimate Source Engine - Vulkan Buffer Implementation
// ============================================================

#include "stdafx.h"
#include "VKBuffer.h"
#include "Core/Logger.h"

namespace USE {

    VKBuffer::VKBuffer()
        : m_device(VK_NULL_HANDLE)
        , m_physicalDevice(VK_NULL_HANDLE)
        , m_buffer(VK_NULL_HANDLE)
        , m_memory(VK_NULL_HANDLE)
        , m_size(0)
        , m_usage(0)
        , m_properties(0)
        , m_mappedData(nullptr)
    {
    }

    VKBuffer::~VKBuffer()
    {
        Destroy();
    }

    bool VKBuffer::Initialize(VkDevice device, VkPhysicalDevice physicalDevice,
                              VkDeviceSize size, VkBufferUsageFlags usage,
                              VkMemoryPropertyFlags properties,
                              const void* initialData)
    {
        if (size == 0) {
            USE_LOG_ERROR("VKBuffer: size cannot be zero");
            return false;
        }

        m_device = device;
        m_physicalDevice = physicalDevice;
        m_size = size;
        m_usage = usage;
        m_properties = properties;

        // Create buffer
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // single queue family

        if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS) {
            USE_LOG_ERROR("VKBuffer: Failed to create buffer");
            return false;
        }

        // Get memory requirements
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, m_buffer, &memRequirements);

        // Find suitable memory type
        uint32_t memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);
        if (memoryTypeIndex == UINT32_MAX) {
            USE_LOG_ERROR("VKBuffer: Failed to find suitable memory type");
            vkDestroyBuffer(device, m_buffer, nullptr);
            m_buffer = VK_NULL_HANDLE;
            return false;
        }

        // Allocate memory
        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = memoryTypeIndex;

        if (vkAllocateMemory(device, &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
            USE_LOG_ERROR("VKBuffer: Failed to allocate memory");
            vkDestroyBuffer(device, m_buffer, nullptr);
            m_buffer = VK_NULL_HANDLE;
            return false;
        }

        // Bind memory to buffer
        vkBindBufferMemory(device, m_buffer, m_memory, 0);

        // If initial data provided, map and copy
        if (initialData) {
            if (properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
                void* data = Map(size, 0);
                if (data) {
                    memcpy(data, initialData, (size_t)size);
                    Flush(size, 0);
                    Unmap();
                } else {
                    USE_LOG_ERROR("VKBuffer: Failed to map memory for initial data");
                    Destroy();
                    return false;
                }
            } else {
                USE_LOG_ERROR("VKBuffer: Initial data requires host visible memory");
                Destroy();
                return false;
            }
        }

        USE_LOG_INFO("VKBuffer created: size=%llu, usage=%u", size, usage);
        return true;
    }

    void VKBuffer::Destroy()
    {
        if (m_mappedData) {
            Unmap();
        }
        if (m_buffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(m_device, m_buffer, nullptr);
            m_buffer = VK_NULL_HANDLE;
        }
        if (m_memory != VK_NULL_HANDLE) {
            vkFreeMemory(m_device, m_memory, nullptr);
            m_memory = VK_NULL_HANDLE;
        }
        m_size = 0;
    }

    void* VKBuffer::Map(VkDeviceSize size, VkDeviceSize offset)
    {
        if (!(m_properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
            USE_LOG_ERROR("VKBuffer: Buffer not host visible, cannot map");
            return nullptr;
        }

        if (m_mappedData != nullptr) {
            // Already mapped, return existing pointer (offset is ignored)
            return m_mappedData;
        }

        VkDeviceSize mapSize = (size == VK_WHOLE_SIZE) ? m_size : size;
        void* data;
        if (vkMapMemory(m_device, m_memory, offset, mapSize, 0, &data) != VK_SUCCESS) {
            USE_LOG_ERROR("VKBuffer: Failed to map memory");
            return nullptr;
        }

        m_mappedData = data;
        return data;
    }

    void VKBuffer::Unmap()
    {
        if (m_mappedData) {
            vkUnmapMemory(m_device, m_memory);
            m_mappedData = nullptr;
        }
    }

    void VKBuffer::Flush(VkDeviceSize size, VkDeviceSize offset)
    {
        if (!(m_properties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            VkMappedMemoryRange range = {};
            range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            range.memory = m_memory;
            range.offset = offset;
            range.size = (size == VK_WHOLE_SIZE) ? VK_WHOLE_SIZE : size;
            vkFlushMappedMemoryRanges(m_device, 1, &range);
        }
    }

    void VKBuffer::Invalidate(VkDeviceSize size, VkDeviceSize offset)
    {
        if (!(m_properties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            VkMappedMemoryRange range = {};
            range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            range.memory = m_memory;
            range.offset = offset;
            range.size = (size == VK_WHOLE_SIZE) ? VK_WHOLE_SIZE : size;
            vkInvalidateMappedMemoryRanges(m_device, 1, &range);
        }
    }

    bool VKBuffer::Update(const void* data, VkDeviceSize size, VkDeviceSize offset)
    {
        if (!(m_properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
            USE_LOG_ERROR("VKBuffer: Cannot update, buffer not host visible");
            return false;
        }

        void* mapped = Map(size, offset);
        if (!mapped) return false;

        memcpy(mapped, data, (size_t)size);
        Flush(size, offset);
        Unmap();
        return true;
    }

    void VKBuffer::BindAsVertex(VkCommandBuffer cmdBuffer, uint32_t binding, VkDeviceSize offset)
    {
        vkCmdBindVertexBuffers(cmdBuffer, binding, 1, &m_buffer, &offset);
    }

    void VKBuffer::BindAsIndex(VkCommandBuffer cmdBuffer, VkIndexType indexType, VkDeviceSize offset)
    {
        vkCmdBindIndexBuffer(cmdBuffer, m_buffer, offset, indexType);
    }

    uint32_t VKBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        return UINT32_MAX;
    }

} // namespace USE