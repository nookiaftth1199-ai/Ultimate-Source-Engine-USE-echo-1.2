// ============================================================
// Ultimate Source Engine - DirectX 9 Texture Implementation
// ============================================================

#include "stdafx.h"
#include "DXTexture.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"

namespace USE {

    DXTexture::DXTexture()
        : m_texture(nullptr)
        , m_width(0)
        , m_height(0)
        , m_format(TextureFormat::Unknown)
    {
    }

    DXTexture::~DXTexture()
    {
        if (m_texture) {
            m_texture->Release();
            m_texture = nullptr;
        }
    }

    bool DXTexture::LoadFromFile(IDirect3DDevice9* device, const char* filename)
    {
        // Use FileSystem to get resolved path
        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("DXTexture::LoadFromFile: FileSystem not available");
            return false;
        }

        std::string resolved = fs->ResolvePath(filename);
        if (resolved.empty()) {
            USE_LOG_ERROR("DXTexture: file not found: %s", filename);
            return false;
        }

        // Release any existing texture
        if (m_texture) {
            m_texture->Release();
            m_texture = nullptr;
        }

        // Load via D3DX
        if (FAILED(D3DXCreateTextureFromFileA(device, resolved.c_str(), &m_texture))) {
            USE_LOG_ERROR("DXTexture: failed to load texture: %s", filename);
            return false;
        }

        // Retrieve texture dimensions and format
        D3DSURFACE_DESC desc;
        m_texture->GetLevelDesc(0, &desc);
        m_width = desc.Width;
        m_height = desc.Height;
        // Convert D3DFORMAT to TextureFormat (simplified; we can store the native format)
        // For now, we assume RGBA8 (common). A real implementation would map properly.
        m_format = TextureFormat::RGBA8_UNORM;

        USE_LOG_INFO("DXTexture loaded: %s (%dx%d)", filename, m_width, m_height);
        return true;
    }

    bool DXTexture::Create(IDirect3DDevice9* device, int width, int height,
                           TextureFormat format, const void* data)
    {
        if (width <= 0 || height <= 0) {
            USE_LOG_ERROR("DXTexture::Create: invalid dimensions");
            return false;
        }

        // Release existing texture
        if (m_texture) {
            m_texture->Release();
            m_texture = nullptr;
        }

        D3DFORMAT d3dFormat = ToD3DFormat(format);
        if (d3dFormat == D3DFMT_UNKNOWN) {
            USE_LOG_ERROR("DXTexture::Create: unsupported texture format");
            return false;
        }

        // Create texture
        if (FAILED(device->CreateTexture(
                width, height,
                1,                          // mip levels (1 for simplicity)
                0,                          // usage
                d3dFormat,
                D3DPOOL_MANAGED,
                &m_texture,
                nullptr))) {
            USE_LOG_ERROR("DXTexture::Create: failed to create texture");
            return false;
        }

        // If data provided, fill the texture
        if (data) {
            D3DLOCKED_RECT lockedRect;
            if (SUCCEEDED(m_texture->LockRect(0, &lockedRect, nullptr, 0))) {
                // Copy row by row – assume packed data (no pitch)
                int bpp = 4; // bytes per pixel (for RGBA8)
                // Actually we should compute based on format, but for now assume 4.
                // For proper implementation, we need a function to get bpp.
                int pitch = width * bpp;
                for (int y = 0; y < height; ++y) {
                    memcpy((char*)lockedRect.pBits + y * lockedRect.Pitch,
                           (const char*)data + y * pitch, pitch);
                }
                m_texture->UnlockRect(0);
            }
        }

        m_width = width;
        m_height = height;
        m_format = format;

        USE_LOG_INFO("DXTexture created: %dx%d format %d", width, height, (int)format);
        return true;
    }

    void DXTexture::Bind(IDirect3DDevice9* device, int stage)
    {
        if (m_texture) {
            device->SetTexture(stage, m_texture);
        }
    }

    void DXTexture::Unbind(IDirect3DDevice9* device, int stage)
    {
        device->SetTexture(stage, nullptr);
    }

    void DXTexture::SetWrap(IDirect3DDevice9* device, int stage, TextureWrap s, TextureWrap t)
    {
        DWORD d3dAddressS = D3DTADDRESS_WRAP;
        DWORD d3dAddressT = D3DTADDRESS_WRAP;

        switch (s) {
            case TextureWrap::Repeat:          d3dAddressS = D3DTADDRESS_WRAP; break;
            case TextureWrap::MirroredRepeat:  d3dAddressS = D3DTADDRESS_MIRROR; break;
            case TextureWrap::ClampToEdge:     d3dAddressS = D3DTADDRESS_CLAMP; break;
            case TextureWrap::ClampToBorder:   d3dAddressS = D3DTADDRESS_BORDER; break;
            default: break;
        }
        switch (t) {
            case TextureWrap::Repeat:          d3dAddressT = D3DTADDRESS_WRAP; break;
            case TextureWrap::MirroredRepeat:  d3dAddressT = D3DTADDRESS_MIRROR; break;
            case TextureWrap::ClampToEdge:     d3dAddressT = D3DTADDRESS_CLAMP; break;
            case TextureWrap::ClampToBorder:   d3dAddressT = D3DTADDRESS_BORDER; break;
            default: break;
        }

        device->SetSamplerState(stage, D3DSAMP_ADDRESSU, d3dAddressS);
        device->SetSamplerState(stage, D3DSAMP_ADDRESSV, d3dAddressT);
    }

    void DXTexture::SetFilter(IDirect3DDevice9* device, int stage, TextureFilter min, TextureFilter mag)
    {
        // For DirectX 9, we need to convert to D3DTEXTUREFILTERTYPE
        DWORD d3dMin = D3DTEXF_LINEAR;
        DWORD d3dMag = D3DTEXF_LINEAR;

        // Map min filter
        switch (min) {
            case TextureFilter::Nearest:                d3dMin = D3DTEXF_POINT; break;
            case TextureFilter::Linear:                 d3dMin = D3DTEXF_LINEAR; break;
            case TextureFilter::NearestMipmapNearest:   d3dMin = D3DTEXF_POINT; // mip filter separate
            case TextureFilter::LinearMipmapNearest:    d3dMin = D3DTEXF_LINEAR;
            case TextureFilter::NearestMipmapLinear:    d3dMin = D3DTEXF_POINT;
            case TextureFilter::LinearMipmapLinear:     d3dMin = D3DTEXF_LINEAR;
            default: break;
        }
        // Map mag filter
        switch (mag) {
            case TextureFilter::Nearest:                d3dMag = D3DTEXF_POINT; break;
            case TextureFilter::Linear:                 d3dMag = D3DTEXF_LINEAR; break;
            default: break;
        }

        device->SetSamplerState(stage, D3DSAMP_MINFILTER, d3dMin);
        device->SetSamplerState(stage, D3DSAMP_MAGFILTER, d3dMag);

        // Set mip filter based on min filter (simplified)
        DWORD d3dMip = D3DTEXF_NONE;
        if (min == TextureFilter::NearestMipmapNearest || min == TextureFilter::LinearMipmapNearest)
            d3dMip = D3DTEXF_POINT;
        else if (min == TextureFilter::NearestMipmapLinear || min == TextureFilter::LinearMipmapLinear)
            d3dMip = D3DTEXF_LINEAR;
        device->SetSamplerState(stage, D3DSAMP_MIPFILTER, d3dMip);
    }

    D3DFORMAT DXTexture::ToD3DFormat(TextureFormat format) const
    {
        // Basic mapping; extend as needed.
        switch (format) {
            case TextureFormat::R8_UNORM:      return D3DFMT_L8;
            case TextureFormat::RG8_UNORM:     return D3DFMT_A8L8;  // Not exactly, but close
            case TextureFormat::RGB8_UNORM:    return D3DFMT_X8R8G8B8;
            case TextureFormat::RGBA8_UNORM:   return D3DFMT_A8R8G8B8;
            case TextureFormat::RGBA8_SRGB:    return D3DFMT_A8R8G8B8; // DX9 has no sRGB texture format
            case TextureFormat::R16_FLOAT:     return D3DFMT_R16F;
            case TextureFormat::RG16_FLOAT:    return D3DFMT_G16R16F;
            case TextureFormat::RGBA16_FLOAT:  return D3DFMT_A16B16G16R16F;
            case TextureFormat::R32_FLOAT:     return D3DFMT_R32F;
            case TextureFormat::RG32_FLOAT:    return D3DFMT_G32R32F;
            case TextureFormat::RGBA32_FLOAT:  return D3DFMT_A32B32G32R32F;
            case TextureFormat::D24_UNORM:     return D3DFMT_D24S8;  // depth only
            case TextureFormat::D32_FLOAT:     return D3DFMT_D32F_LOCKABLE;
            case TextureFormat::D24_UNORM_S8_UINT: return D3DFMT_D24S8;
            default:                            return D3DFMT_UNKNOWN;
        }
    }

} // namespace USE