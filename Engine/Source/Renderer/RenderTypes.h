// ============================================================
// Ultimate Source Engine - Render Types
// ============================================================
//
// Common types, enums, and structures used throughout the rendering system.
// ============================================================

#pragma once

#include <cstdint>

namespace USE {

    // -----------------------------------------------------------------
    // Color (RGBA floating point)
    // -----------------------------------------------------------------
    struct Color {
        float r, g, b, a;

        Color() : r(0), g(0), b(0), a(1) {}
        Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}

        static const Color Black;
        static const Color White;
        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color Yellow;
        static const Color Cyan;
        static const Color Magenta;
        static const Color Transparent;
    };

    // -----------------------------------------------------------------
    // Rectangle (integer)
    // -----------------------------------------------------------------
    struct Rect {
        int32_t x, y, width, height;

        Rect() : x(0), y(0), width(0), height(0) {}
        Rect(int32_t x, int32_t y, int32_t width, int32_t height)
            : x(x), y(y), width(width), height(height) {}
    };

    // -----------------------------------------------------------------
    // Viewport
    // -----------------------------------------------------------------
    struct Viewport {
        float x, y, width, height;
        float minDepth, maxDepth;

        Viewport()
            : x(0), y(0), width(0), height(0), minDepth(0), maxDepth(1) {}
        Viewport(float x, float y, float width, float height,
                 float minDepth = 0, float maxDepth = 1)
            : x(x), y(y), width(width), height(height), minDepth(minDepth), maxDepth(maxDepth) {}
    };

    // -----------------------------------------------------------------
    // Clear flags (bitmask)
    // -----------------------------------------------------------------
    enum ClearFlags : uint32_t {
        CLEAR_COLOR   = 1 << 0,
        CLEAR_DEPTH   = 1 << 1,
        CLEAR_STENCIL = 1 << 2,
        CLEAR_ALL     = CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL
    };

    // -----------------------------------------------------------------
    // Primitive topology
    // -----------------------------------------------------------------
    enum class PrimitiveTopology {
        PointList,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip,
        TriangleFan,
        PatchList
    };

    // -----------------------------------------------------------------
    // Texture format
    // -----------------------------------------------------------------
    enum class TextureFormat {
        Unknown,

        // Color formats
        R8_UNORM,
        RG8_UNORM,
        RGB8_UNORM,
        RGBA8_UNORM,
        RGBA8_SRGB,

        R16_UNORM,
        RG16_UNORM,
        RGB16_UNORM,
        RGBA16_UNORM,

        R16_FLOAT,
        RG16_FLOAT,
        RGB16_FLOAT,
        RGBA16_FLOAT,

        R32_FLOAT,
        RG32_FLOAT,
        RGB32_FLOAT,
        RGBA32_FLOAT,

        // Compressed formats
        BC1_UNORM,   // DXT1
        BC1_SRGB,
        BC2_UNORM,   // DXT3
        BC2_SRGB,
        BC3_UNORM,   // DXT5
        BC3_SRGB,
        BC4_UNORM,   // RGTC1
        BC4_SNORM,
        BC5_UNORM,   // RGTC2
        BC5_SNORM,
        BC6H_UF16,   // BPTC float
        BC6H_SF16,
        BC7_UNORM,
        BC7_SRGB,

        // Depth/stencil formats
        D16_UNORM,
        D24_UNORM,
        D32_FLOAT,
        D24_UNORM_S8_UINT,
        D32_FLOAT_S8_UINT,

        // Special
        R11G11B10_FLOAT,
        RGB9E5_FLOAT
    };

    // -----------------------------------------------------------------
    // Texture address mode (wrap)
    // -----------------------------------------------------------------
    enum class TextureWrap {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder,
        MirrorClampToEdge
    };

    // -----------------------------------------------------------------
    // Texture filter
    // -----------------------------------------------------------------
    enum class TextureFilter {
        Nearest,
        Linear,
        NearestMipmapNearest,
        LinearMipmapNearest,
        NearestMipmapLinear,
        LinearMipmapLinear
    };

    // -----------------------------------------------------------------
    // Comparison function (depth, stencil)
    // -----------------------------------------------------------------
    enum class CompareFunc {
        Never,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always
    };

    // -----------------------------------------------------------------
    // Stencil operation
    // -----------------------------------------------------------------
    enum class StencilOp {
        Keep,
        Zero,
        Replace,
        IncrementClamp,
        DecrementClamp,
        Invert,
        IncrementWrap,
        DecrementWrap
    };

    // -----------------------------------------------------------------
    // Blend factor
    // -----------------------------------------------------------------
    enum class BlendFactor {
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstColor,
        OneMinusDstColor,
        DstAlpha,
        OneMinusDstAlpha,
        SrcAlphaSaturate,
        BlendColor,
        OneMinusBlendColor
    };

    // -----------------------------------------------------------------
    // Blend operation
    // -----------------------------------------------------------------
    enum class BlendOp {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max
    };

    // -----------------------------------------------------------------
    // Cull mode
    // -----------------------------------------------------------------
    enum class CullMode {
        None,
        Front,
        Back
    };

    // -----------------------------------------------------------------
    // Fill mode
    // -----------------------------------------------------------------
    enum class FillMode {
        Point,
        Wireframe,
        Solid
    };

    // -----------------------------------------------------------------
    // Shader type
    // -----------------------------------------------------------------
    enum class ShaderType {
        Vertex,
        Pixel,
        Geometry,
        Hull,
        Domain,
        Compute
    };

    // -----------------------------------------------------------------
    // Vertex element usage
    // -----------------------------------------------------------------
    enum class VertexUsage {
        Position,
        Normal,
        Tangent,
        Binormal,
        Color,
        TexCoord,
        BlendIndices,
        BlendWeight
    };

    // -----------------------------------------------------------------
    // Vertex element type
    // -----------------------------------------------------------------
    enum class VertexType {
        Float1,
        Float2,
        Float3,
        Float4,
        Byte4,
        UByte4,
        Short2,
        Short4,
        UByte4N,
        Short2N,
        Short4N,
        UShort2N,
        UShort4N,
        Half2,
        Half4
    };

    // -----------------------------------------------------------------
    // Vertex element description
    // -----------------------------------------------------------------
    struct VertexElement {
        uint32_t        stream;      // input slot
        uint32_t        offset;      // offset in bytes
        VertexType      type;
        VertexUsage     usage;
        uint32_t        index;       // usage index (e.g., TEXCOORD0, TEXCOORD1)
    };

    // -----------------------------------------------------------------
    // Shader reflection data (simplified)
    // -----------------------------------------------------------------
    struct ShaderResourceBinding {
        enum Type {
            SRV_CBV,  // Shader resource view / constant buffer
            UAV,      // unordered access view
            Sampler
        } type;
        const char* name;
        uint32_t    slot;
    };

    // -----------------------------------------------------------------
    // Render target description
    // -----------------------------------------------------------------
    struct RenderTargetDesc {
        uint32_t        width;
        uint32_t        height;
        TextureFormat   colorFormat;
        TextureFormat   depthStencilFormat;
        uint32_t        sampleCount;
        uint32_t        sampleQuality;
        bool            useSRGB;

        RenderTargetDesc()
            : width(0), height(0)
            , colorFormat(TextureFormat::RGBA8_UNORM)
            , depthStencilFormat(TextureFormat::D24_UNORM_S8_UINT)
            , sampleCount(1), sampleQuality(0)
            , useSRGB(false)
        {}
    };

} // namespace USE