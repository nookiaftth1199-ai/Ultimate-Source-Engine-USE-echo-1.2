#include "stdafx.h"
#include "DXShader.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"

namespace USE {

    DXShader::DXShader()
        : m_type(ShaderType::Vertex)
        , m_shader(nullptr)
        , m_constTable(nullptr)
        , m_device(nullptr)
    {
    }

    DXShader::~DXShader()
    {
        Release();
    }

    void DXShader::Release()
    {
        if (m_constTable) {
            m_constTable->Release();
            m_constTable = nullptr;
        }
        if (m_shader) {
            m_shader->Release();
            m_shader = nullptr;
        }
        m_device = nullptr;
    }

    bool DXShader::Compile(IDirect3DDevice9* device, ShaderType type, const char* source,
                           const char* entryPoint, const char* profile)
    {
        LPD3DXBUFFER shaderBuf = nullptr;
        LPD3DXBUFFER errorBuf = nullptr;

        // Determine profile if not provided
        char defaultProfile[16];
        if (profile == nullptr) {
            if (type == ShaderType::Vertex)
                strcpy_s(defaultProfile, "vs_2_0");
            else
                strcpy_s(defaultProfile, "ps_2_0");
            profile = defaultProfile;
        }

        HRESULT hr = D3DXCompileShader(
            source,
            (UINT)strlen(source),
            nullptr,           // defines
            nullptr,           // include
            entryPoint,
            profile,
            0,                 // flags
            &shaderBuf,
            &errorBuf,
            &m_constTable);

        if (FAILED(hr)) {
            if (errorBuf) {
                USE_LOG_ERROR("DXShader compile error:\n%s",
                              (const char*)errorBuf->GetBufferPointer());
                errorBuf->Release();
            }
            return false;
        }

        // Create shader object
        if (type == ShaderType::Vertex) {
            IDirect3DVertexShader9* vs = nullptr;
            hr = device->CreateVertexShader(
                (const DWORD*)shaderBuf->GetBufferPointer(),
                &vs);
            if (SUCCEEDED(hr)) {
                m_shader = vs;
            }
        } else {
            IDirect3DPixelShader9* ps = nullptr;
            hr = device->CreatePixelShader(
                (const DWORD*)shaderBuf->GetBufferPointer(),
                &ps);
            if (SUCCEEDED(hr)) {
                m_shader = ps;
            }
        }

        shaderBuf->Release();

        if (FAILED(hr)) {
            USE_LOG_ERROR("DXShader: Failed to create shader object");
            if (m_constTable) {
                m_constTable->Release();
                m_constTable = nullptr;
            }
            return false;
        }

        m_type = type;
        m_device = device; // store for constant setting
        return true;
    }

    bool DXShader::CreateFromSource(IDirect3DDevice9* device, ShaderType type, const char* source,
                                    const char* entryPoint, const char* profile)
    {
        Release();
        return Compile(device, type, source, entryPoint, profile);
    }

    bool DXShader::CreateFromFile(IDirect3DDevice9* device, ShaderType type, const char* filename,
                                  const char* entryPoint, const char* profile)
    {
        Release();

        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("DXShader: FileSystem not available");
            return false;
        }

        std::string resolved = fs->ResolvePath(filename);
        if (resolved.empty()) {
            USE_LOG_ERROR("DXShader: File not found: %s", filename);
            return false;
        }

        LPD3DXBUFFER shaderBuf = nullptr;
        LPD3DXBUFFER errorBuf = nullptr;

        char defaultProfile[16];
        if (profile == nullptr) {
            if (type == ShaderType::Vertex)
                strcpy_s(defaultProfile, "vs_2_0");
            else
                strcpy_s(defaultProfile, "ps_2_0");
            profile = defaultProfile;
        }

        HRESULT hr = D3DXCompileShaderFromFileA(
            resolved.c_str(),
            nullptr,
            nullptr,
            entryPoint,
            profile,
            0,
            &shaderBuf,
            &errorBuf,
            &m_constTable);

        if (FAILED(hr)) {
            if (errorBuf) {
                USE_LOG_ERROR("DXShader compile error from file:\n%s",
                              (const char*)errorBuf->GetBufferPointer());
                errorBuf->Release();
            }
            return false;
        }

        if (type == ShaderType::Vertex) {
            IDirect3DVertexShader9* vs = nullptr;
            hr = device->CreateVertexShader(
                (const DWORD*)shaderBuf->GetBufferPointer(),
                &vs);
            if (SUCCEEDED(hr)) m_shader = vs;
        } else {
            IDirect3DPixelShader9* ps = nullptr;
            hr = device->CreatePixelShader(
                (const DWORD*)shaderBuf->GetBufferPointer(),
                &ps);
            if (SUCCEEDED(hr)) m_shader = ps;
        }

        shaderBuf->Release();

        if (FAILED(hr)) {
            USE_LOG_ERROR("DXShader: Failed to create shader object");
            if (m_constTable) {
                m_constTable->Release();
                m_constTable = nullptr;
            }
            return false;
        }

        m_type = type;
        m_device = device;
        return true;
    }

    void DXShader::Bind(IDirect3DDevice9* device)
    {
        if (!m_shader) return;
        if (m_type == ShaderType::Vertex) {
            device->SetVertexShader((IDirect3DVertexShader9*)m_shader);
        } else {
            device->SetPixelShader((IDirect3DPixelShader9*)m_shader);
        }
        m_device = device; // update device for constant setting
    }

    void DXShader::Unbind(IDirect3DDevice9* device)
    {
        if (m_type == ShaderType::Vertex) {
            device->SetVertexShader(nullptr);
        } else {
            device->SetPixelShader(nullptr);
        }
    }

    void DXShader::SetBool(const char* name, bool value)
    {
        if (m_constTable) {
            D3DXHANDLE h = m_constTable->GetConstantByName(nullptr, name);
            if (h) {
                BOOL v = value ? TRUE : FALSE;
                m_constTable->SetBool(m_device, h, v);
            }
        }
    }

    void DXShader::SetInt(const char* name, int value)
    {
        if (m_constTable) {
            D3DXHANDLE h = m_constTable->GetConstantByName(nullptr, name);
            if (h) m_constTable->SetInt(m_device, h, value);
        }
    }

    void DXShader::SetFloat(const char* name, float value)
    {
        if (m_constTable) {
            D3DXHANDLE h = m_constTable->GetConstantByName(nullptr, name);
            if (h) m_constTable->SetFloat(m_device, h, value);
        }
    }

    void DXShader::SetVector(const char* name, const float* values, int count)
    {
        if (m_constTable) {
            D3DXHANDLE h = m_constTable->GetConstantByName(nullptr, name);
            if (h) {
                // Assume count is 4 for float4; could be extended
                m_constTable->SetFloatArray(m_device, h, values, count);
            }
        }
    }

    void DXShader::SetMatrix(const char* name, const float* matrix)
    {
        if (m_constTable) {
            D3DXHANDLE h = m_constTable->GetConstantByName(nullptr, name);
            if (h) {
                // Convert to D3DXMATRIX? SetMatrix expects D3DXMATRIX*.
                D3DXMATRIX mat(matrix);
                m_constTable->SetMatrix(m_device, h, &mat);
            }
        }
    }

    void DXShader::SetBool(int reg, bool value)
    {
        if (m_constTable) {
            m_constTable->SetBool(m_device, reg, value ? TRUE : FALSE);
        }
    }

    void DXShader::SetInt(int reg, int value)
    {
        if (m_constTable) {
            m_constTable->SetInt(m_device, reg, value);
        }
    }

    void DXShader::SetFloat(int reg, float value)
    {
        if (m_constTable) {
            m_constTable->SetFloat(m_device, reg, value);
        }
    }

    void DXShader::SetVector(int reg, const float* values, int count)
    {
        if (m_constTable) {
            m_constTable->SetFloatArray(m_device, reg, values, count);
        }
    }

    void DXShader::SetMatrix(int reg, const float* matrix)
    {
        if (m_constTable) {
            D3DXMATRIX mat(matrix);
            m_constTable->SetMatrix(m_device, reg, &mat);
        }
    }

} // namespace USE