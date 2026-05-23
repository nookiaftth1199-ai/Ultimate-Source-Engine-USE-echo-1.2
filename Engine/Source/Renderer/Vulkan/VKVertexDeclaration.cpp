// ============================================================
// Ultimate Source Engine - Vulkan Vertex Declaration Implementation
// ============================================================

#include "stdafx.h"
#include "VKVertexDeclaration.h"
#include <map>
#include <cstring>

namespace USE {

    // -----------------------------------------------------------------
    // VertexType to VkFormat mapping
    // -----------------------------------------------------------------
    VkFormat VertexTypeToVkFormat(VertexType type)
    {
        switch (type) {
            case VertexType::Float1:   return VK_FORMAT_R32_SFLOAT;
            case VertexType::Float2:   return VK_FORMAT_R32G32_SFLOAT;
            case VertexType::Float3:   return VK_FORMAT_R32G32B32_SFLOAT;
            case VertexType::Float4:   return VK_FORMAT_R32G32B32A32_SFLOAT;
            case VertexType::Byte4:    return VK_FORMAT_R8G8B8A8_SINT;
            case VertexType::UByte4:   return VK_FORMAT_R8G8B8A8_UINT;
            case VertexType::Byte4N:   return VK_FORMAT_R8G8B8A8_SNORM;
            case VertexType::UByte4N:  return VK_FORMAT_R8G8B8A8_UNORM;
            case VertexType::Short2:   return VK_FORMAT_R16G16_SINT;
            case VertexType::Short4:   return VK_FORMAT_R16G16B16A16_SINT;
            case VertexType::Short2N:  return VK_FORMAT_R16G16_SNORM;
            case VertexType::Short4N:  return VK_FORMAT_R16G16B16A16_SNORM;
            case VertexType::UShort2N: return VK_FORMAT_R16G16_UNORM;
            case VertexType::UShort4N: return VK_FORMAT_R16G16B16A16_UNORM;
            case VertexType::Half2:    return VK_FORMAT_R16G16_SFLOAT;
            case VertexType::Half4:    return VK_FORMAT_R16G16B16A16_SFLOAT;
            default:                    return VK_FORMAT_UNDEFINED;
        }
    }

    // -----------------------------------------------------------------
    // VKVertexDeclaration implementation
    // -----------------------------------------------------------------
    VKVertexDeclaration::VKVertexDeclaration()
    {
    }

    VKVertexDeclaration::~VKVertexDeclaration()
    {
    }

    void VKVertexDeclaration::AddElements(const VertexElement* elements, uint32_t count)
    {
        Clear();

        // First, compute per-stream stride and collect attributes.
        std::map<uint32_t, uint32_t> streamStride; // binding -> stride

        for (uint32_t i = 0; i < count; ++i) {
            const VertexElement& elem = elements[i];
            uint32_t stream = elem.stream;

            // Compute element size based on type
            uint32_t elemSize = 0;
            switch (elem.type) {
                case VertexType::Float1:   elemSize = 4; break;
                case VertexType::Float2:   elemSize = 8; break;
                case VertexType::Float3:   elemSize = 12; break;
                case VertexType::Float4:   elemSize = 16; break;
                case VertexType::Byte4:
                case VertexType::UByte4:
                case VertexType::Byte4N:
                case VertexType::UByte4N:  elemSize = 4; break;
                case VertexType::Short2:
                case VertexType::Short2N:
                case VertexType::UShort2N:
                case VertexType::Half2:    elemSize = 4; break;
                case VertexType::Short4:
                case VertexType::Short4N:
                case VertexType::UShort4N:
                case VertexType::Half4:    elemSize = 8; break;
                default: break;
            }

            // Update stride: max(offset + elemSize)
            uint32_t end = elem.offset + elemSize;
            if (end > streamStride[stream]) {
                streamStride[stream] = end;
            }
        }

        // Create binding descriptions
        m_bindings.reserve(streamStride.size());
        for (auto& pair : streamStride) {
            VkVertexInputBindingDescription binding = {};
            binding.binding = pair.first;
            binding.stride = pair.second;
            binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // assume per-vertex
            m_bindings.push_back(binding);
        }

        // Create attribute descriptions
        m_attributes.reserve(count);
        for (uint32_t i = 0; i < count; ++i) {
            const VertexElement& elem = elements[i];
            VkVertexInputAttributeDescription attr = {};
            attr.location = i; // layout location (should match shader input location)
            attr.binding = elem.stream;
            attr.format = VertexTypeToVkFormat(elem.type);
            attr.offset = elem.offset;
            m_attributes.push_back(attr);
        }
    }

    void VKVertexDeclaration::Clear()
    {
        m_bindings.clear();
        m_attributes.clear();
    }

    void VKVertexDeclaration::PopulateVertexInputState(VkPipelineVertexInputStateCreateInfo& info) const
    {
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.vertexBindingDescriptionCount = GetBindingCount();
        info.pVertexBindingDescriptions = GetBindingDescriptions();
        info.vertexAttributeDescriptionCount = GetAttributeCount();
        info.pVertexAttributeDescriptions = GetAttributeDescriptions();
    }

} // namespace USE