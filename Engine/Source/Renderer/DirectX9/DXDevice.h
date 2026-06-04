// ============================================================
// Ultimate Source Engine – DirectX 9 Render Device
// ============================================================
#pragma once

#include "../IRenderDevice.h"
#include <d3d9.h>
#include <unordered_map>

namespace USE
{
	class DXDevice : public IRenderDevice
	{
	public:
		DXDevice();
		~DXDevice() override;

		bool Initialize(void* windowHandle, uint32_t width, uint32_t height, bool vsync) override;
		void Shutdown() override;
		void ResizeBackBuffer(uint32_t width, uint32_t height) override;

		void BeginFrame() override;
		void EndFrame() override;
		void Present() override;

		void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) override;
		void SetScissorRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h) override;
		void SetDepthStencilState(bool depthTest, bool depthWrite) override;
		void SetRasterizerState(bool cullBackFaces, bool wireframe) override;
		void SetBlendState(bool enable) override;

		void Clear(bool color, bool depth, bool stencil,
			const Vector4& clearColor, float clearDepth, uint8_t clearStencil) override;

		void Draw(PrimitiveType type, uint32_t vertexCount, uint32_t startVertex = 0) override;
		void DrawIndexed(PrimitiveType type, uint32_t indexCount, uint32_t startIndex = 0,
			uint32_t baseVertex = 0) override;
		void DrawInstanced(PrimitiveType type, uint32_t vertexCountPerInstance,
			uint32_t instanceCount, uint32_t startVertex = 0,
			uint32_t startInstance = 0) override {}
		void DrawIndexedInstanced(PrimitiveType type, uint32_t indexCountPerInstance,
			uint32_t instanceCount, uint32_t startIndex = 0,
			uint32_t baseVertex = 0,
			uint32_t startInstance = 0) override {}
		void DrawIndirect(BufferType buffer, uint32_t offset) override {}

		void Dispatch(uint32_t groupX, uint32_t groupY, uint32_t groupZ) override {}

		// Resource creation (stubs – real implementation would use D3D resources)
		uint32_t CreateVertexBuffer(const void* data, uint32_t size, BufferUsage usage) override { return 0; }
		uint32_t CreateIndexBuffer(const void* data, uint32_t size, BufferUsage usage, bool is32Bit) override { return 0; }
		uint32_t CreateConstantBuffer(uint32_t size) override { return 0; }
		uint32_t CreateStructuredBuffer(const void* data, uint32_t stride, uint32_t count,
			BufferUsage usage) override {
			return 0;
		}
		uint32_t CreateTexture2D(uint32_t width, uint32_t height, uint32_t mipLevels,
			TextureFormat format, const void* data) override {
			return 0;
		}
		uint32_t CreateRenderTarget(uint32_t width, uint32_t height, TextureFormat format,
			bool createDepthStencil) override {
			return 0;
		}
		uint32_t CreateDepthStencil(uint32_t width, uint32_t height, TextureFormat format) override { return 0; }
		uint32_t CreateShader(ShaderType type, const std::string& source,
			const std::string& entryPoint) override {
			return 0;
		}
		uint32_t CreateProgram(const std::vector<uint32_t>& shaderHandles) override { return 0; }
		uint32_t CreateVertexDeclaration(const std::vector<VertexElement>& elements) override { return 0; }

		void SetVertexBuffer(uint32_t handle, uint32_t slot, uint32_t stride, uint32_t offset = 0) override {}
		void SetIndexBuffer(uint32_t handle, bool is32Bit) override {}
		void SetConstantBuffer(uint32_t handle, uint32_t slot) override {}
		void SetTexture(uint32_t handle, uint32_t slot) override {}
		void SetRenderTarget(uint32_t handle, uint32_t depthStencilHandle = 0) override {}
		void SetProgram(uint32_t handle) override {}
		void SetVertexDeclaration(uint32_t handle) override {}

		void UpdateBuffer(uint32_t handle, const void* data, uint32_t size) override {}
		void UpdateTexture(uint32_t handle, uint32_t mipLevel, const void* data) override {}

		void DestroyBuffer(uint32_t handle) override {}
		void DestroyTexture(uint32_t handle) override {}
		void DestroyShader(uint32_t handle) override {}
		void DestroyProgram(uint32_t handle) override {}
		void DestroyVertexDeclaration(uint32_t handle) override {}

		uint32_t GetBackBufferWidth() const override { return m_width; }
		uint32_t GetBackBufferHeight() const override { return m_height; }
		bool IsValid() const override { return m_initialized; }

	private:
		HWND m_hwnd = nullptr;
		IDirect3D9* m_d3d = nullptr;
		IDirect3DDevice9* m_device = nullptr;
		D3DPRESENT_PARAMETERS m_presentParams{};
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		bool m_initialized = false;
	};
}