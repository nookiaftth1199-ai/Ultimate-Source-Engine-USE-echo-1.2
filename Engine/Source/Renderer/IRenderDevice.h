// ============================================================
// Ultimate Source Engine - Render Device Interface
// ============================================================
// Abstract GPU device. All rendering backends implement this.
// ============================================================

#pragma once

#include "../Math/Vector4.h"
#include "../Math/Matrix4.h"
#include <string>
#include <vector>
#include <cstdint>

namespace USE
{
	// -----------------------------------------------------------------
	// Enums & constants
	// -----------------------------------------------------------------
	enum class PrimitiveType
	{
		Triangles,
		TriangleStrip,
		Lines,
		LineStrip,
		Points
	};

	enum class BufferType
	{
		Vertex,
		Index,
		Constant,
		Structured,
		Indirect
	};

	enum class BufferUsage
	{
		Static,
		Dynamic,
		Stream
	};

	enum class TextureFormat
	{
		R8G8B8A8_UNORM,
		R32G32B32A32_FLOAT,
		R32_FLOAT,
		D24S8,
		D32,
		R32G32_FLOAT,
		R32G32B32_FLOAT,
		D24_UNORM_S8_UINT,
		RGBA8_UNORM
	};

	enum class ShaderType
	{
		Vertex,
		Pixel,
		Geometry,
		Compute,
		Hull,
		Domain
	};

	struct VertexElement
	{
		std::string semantic;
		uint32_t    index;
		TextureFormat format;
		uint32_t    slot;
		uint32_t    offset;
		uint32_t    stride;
	};

	// -----------------------------------------------------------------
	// Interface
	// -----------------------------------------------------------------
	class IRenderDevice
	{
	public:
		virtual ~IRenderDevice() = default;

		// Initialization
		virtual bool Initialize(void* windowHandle, uint32_t width, uint32_t height, bool vsync) = 0;
		virtual void Shutdown() = 0;

		virtual void ResizeBackBuffer(uint32_t width, uint32_t height) = 0;

		// Frame control
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void Present() = 0;

		// Viewport & states
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetScissorRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetDepthStencilState(bool depthTest, bool depthWrite) = 0;
		virtual void SetRasterizerState(bool cullBackFaces, bool wireframe) = 0;
		virtual void SetBlendState(bool enable) = 0;

		// Clearing
		virtual void Clear(bool color, bool depth, bool stencil,
			const Vector4& clearColor, float clearDepth, uint8_t clearStencil) = 0;

		// Drawing
		virtual void Draw(PrimitiveType type, uint32_t vertexCount, uint32_t startVertex = 0) = 0;
		virtual void DrawIndexed(PrimitiveType type, uint32_t indexCount, uint32_t startIndex = 0, uint32_t baseVertex = 0) = 0;
		virtual void DrawInstanced(PrimitiveType type, uint32_t vertexCountPerInstance,
			uint32_t instanceCount, uint32_t startVertex = 0, uint32_t startInstance = 0) = 0;
		virtual void DrawIndexedInstanced(PrimitiveType type, uint32_t indexCountPerInstance,
			uint32_t instanceCount, uint32_t startIndex = 0,
			uint32_t baseVertex = 0, uint32_t startInstance = 0) = 0;
		virtual void DrawIndirect(BufferType buffer, uint32_t offset) = 0;

		// Compute
		virtual void Dispatch(uint32_t groupX, uint32_t groupY, uint32_t groupZ) = 0;

		// Resource creation
		virtual uint32_t CreateVertexBuffer(const void* data, uint32_t size, BufferUsage usage) = 0;
		virtual uint32_t CreateIndexBuffer(const void* data, uint32_t size, BufferUsage usage, bool is32Bit) = 0;
		virtual uint32_t CreateConstantBuffer(uint32_t size) = 0;
		virtual uint32_t CreateStructuredBuffer(const void* data, uint32_t stride, uint32_t count, BufferUsage usage) = 0;
		virtual uint32_t CreateTexture2D(uint32_t width, uint32_t height, uint32_t mipLevels,
			TextureFormat format, const void* data) = 0;
		virtual uint32_t CreateRenderTarget(uint32_t width, uint32_t height, TextureFormat format,
			bool createDepthStencil) = 0;
		virtual uint32_t CreateDepthStencil(uint32_t width, uint32_t height, TextureFormat format) = 0;
		virtual uint32_t CreateShader(ShaderType type, const std::string& source, const std::string& entryPoint) = 0;
		virtual uint32_t CreateProgram(const std::vector<uint32_t>& shaderHandles) = 0;
		virtual uint32_t CreateVertexDeclaration(const std::vector<VertexElement>& elements) = 0;

		// Resource binding
		virtual void SetVertexBuffer(uint32_t handle, uint32_t slot, uint32_t stride, uint32_t offset = 0) = 0;
		virtual void SetIndexBuffer(uint32_t handle, bool is32Bit) = 0;
		virtual void SetConstantBuffer(uint32_t handle, uint32_t slot) = 0;
		virtual void SetTexture(uint32_t handle, uint32_t slot) = 0;
		virtual void SetRenderTarget(uint32_t handle, uint32_t depthStencilHandle = 0) = 0;
		virtual void SetProgram(uint32_t handle) = 0;
		virtual void SetVertexDeclaration(uint32_t handle) = 0;

		// Resource updates
		virtual void UpdateBuffer(uint32_t handle, const void* data, uint32_t size) = 0;
		virtual void UpdateTexture(uint32_t handle, uint32_t mipLevel, const void* data) = 0;

		// Resource deletion
		virtual void DestroyBuffer(uint32_t handle) = 0;
		virtual void DestroyTexture(uint32_t handle) = 0;
		virtual void DestroyShader(uint32_t handle) = 0;
		virtual void DestroyProgram(uint32_t handle) = 0;
		virtual void DestroyVertexDeclaration(uint32_t handle) = 0;

		// Queries
		virtual uint32_t GetBackBufferWidth() const = 0;
		virtual uint32_t GetBackBufferHeight() const = 0;
		virtual bool IsValid() const = 0;
	};
}