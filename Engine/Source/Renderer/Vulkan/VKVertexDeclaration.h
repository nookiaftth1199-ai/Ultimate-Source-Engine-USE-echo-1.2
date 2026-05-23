// ============================================================
// Ultimate Source Engine - Vulkan Vertex Declaration
// ============================================================
//
// Manages vertex input descriptions for Vulkan pipelines.
// It holds vertex binding descriptions and attribute descriptions,
// and provides a helper to fill VkPipelineVertexInputStateCreateInfo.
// ============================================================

#pragma once

#include "stdafx.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>

namespace USE {

    // Vertex element semantics (usage)
    enum class VertexUsage {
        Position,
        Normal,
        Tangent,
        Bitangent,
        Color,
        TexCoord,
        BlendIndices,
        BlendWeight
    };

    // Vertex element type (maps to Vulkan formats)
    enum class VertexType {
        Float1,   // VK_FORMAT_R32_SFLOAT
        Float2,   // VK_FORMAT_R32G32_SFLOAT
        Float3,   // VK_FORMAT_R32G32B32_SFLOAT
        Float4,   // VK_FORMAT_R32G32B32A32_SFLOAT
        Byte4,    // VK_FORMAT_R8G8B8A8_SINT
        UByte4,   // VK_FORMAT_R8G8B8A8_UINT
        Byte4N,   // VK_FORMAT_R8G8B8A8_SNORM
        UByte4N,  // VK_FORMAT_R8G8B8A8_UNORM
        Short2,   // VK_FORMAT_R16G16_SINT
        Short4,   // VK_FORMAT_R16G16B16A16_SINT
        Short2N,  // VK_FORMAT_R16G16_SNORM
        Short4N,  // VK_FORMAT_R16G16B16A16_SNORM
        UShort2N, // VK_FORMAT_R16G16_UNORM
        UShort4N, // VK_FORMAT_R16G16B16A16_UNORM
        Half2,    // VK_FORMAT_R16G16_SFLOAT
        Half4     // VK_FORMAT_R16G16B16A16_SFLOAT
    };

    // Convert VertexType to VkFormat
    VkFormat VertexTypeToVkFormat(VertexType type);

    // Vertex element description (similar to earlier, but using our enums)
    struct VertexElement {
        uint32_t        stream;      // binding index (0..max)
        uint32_t        offset;      // offset in bytes from start of vertex
        VertexType      type;        // data type
        VertexUsage     usage;       // semantic
        uint8_t         usageIndex;  // e.g., TEXCOORD0, TEXCOORD1
    };

    // Convenience structure for vertex input state creation
    class VKVertexDeclaration {
    public:
        VKVertexDeclaration();
        ~VKVertexDeclaration();

        // Add elements (this will generate binding descriptions and attribute descriptions)
        void AddElements(const VertexElement* elements, uint32_t count);

        // Clear all data
        void Clear();

        // Get binding descriptions (array pointer and count)
        const VkVertexInputBindingDescription* GetBindingDescriptions() const { return m_bindings.data(); }
        uint32_t GetBindingCount() const { return (uint32_t)m_bindings.size(); }

        // Get attribute descriptions (array pointer and count)
        const VkVertexInputAttributeDescription* GetAttributeDescriptions() const { return m_attributes.data(); }
        uint32_t GetAttributeCount() const { return (uint32_t)m_attributes.size(); }

        // Fill a VkPipelineVertexInputStateCreateInfo structure (pointers set to our internal data)
        void PopulateVertexInputState(VkPipelineVertexInputStateCreateInfo& info) const;

    private:
        std::vector<VkVertexInputBindingDescription>   m_bindings;
        std::vector<VkVertexInputAttributeDescription> m_attributes;
    };

} // namespace USE