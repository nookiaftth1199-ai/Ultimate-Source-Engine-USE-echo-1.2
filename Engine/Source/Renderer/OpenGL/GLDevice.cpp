#include "stdafx.h"
#include "GLDevice.h"
#include "Core/Logger.h"

namespace USE
{
	GLDevice::GLDevice() = default;
	GLDevice::~GLDevice() { Shutdown(); }

	bool GLDevice::Initialize(void* windowHandle, uint32_t width, uint32_t height, bool vsync)
	{
		m_window = static_cast<SDL_Window*>(windowHandle);
		if (!m_window) return false;

		m_width = width;
		m_height = height;

		m_context = SDL_GL_GetCurrentContext();
		if (!m_context)
		{
			USE_LOG_ERROR("GLDevice: No current OpenGL context.");
			return false;
		}

		SDL_GL_SetSwapInterval(vsync ? 1 : 0);
		glViewport(0, 0, width, height);

		m_initialized = true;
		USE_LOG_INFO("GLDevice initialized (%u x %u).", width, height);
		return true;
	}

	void GLDevice::Shutdown()
	{
		for (auto& p : m_buffers)  glDeleteBuffers(1, &p.second);
		for (auto& p : m_textures) glDeleteTextures(1, &p.second);
		for (auto& p : m_shaders)  glDeleteShader(p.second);
		for (auto& p : m_programs) glDeleteProgram(p.second);
		m_buffers.clear();
		m_textures.clear();
		m_shaders.clear();
		m_programs.clear();
		m_initialized = false;
	}

	void GLDevice::ResizeBackBuffer(uint32_t w, uint32_t h) { m_width = w; m_height = h; glViewport(0, 0, w, h); }
	void GLDevice::BeginFrame() {}
	void GLDevice::EndFrame() {}
	void GLDevice::Present() { SDL_GL_SwapWindow(m_window); }

	void GLDevice::SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) { glViewport(x, y, w, h); }
	void GLDevice::SetScissorRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h) { glScissor(x, y, w, h); glEnable(GL_SCISSOR_TEST); }

	void GLDevice::SetDepthStencilState(bool test, bool write) {
		if (test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
		glDepthMask(write ? GL_TRUE : GL_FALSE);
	}

	void GLDevice::SetRasterizerState(bool cull, bool wire) {
		if (cull) { glEnable(GL_CULL_FACE); glCullFace(GL_BACK); }
		else glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, wire ? GL_LINE : GL_FILL);
	}

	void GLDevice::SetBlendState(bool en) {
		if (en) { glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); }
		else glDisable(GL_BLEND);
	}

	void GLDevice::Clear(bool color, bool depth, bool stencil, const Vector4& cc, float cd, uint8_t cs) {
		GLbitfield mask = 0;
		if (color) { glClearColor(cc.x, cc.y, cc.z, cc.w); mask |= GL_COLOR_BUFFER_BIT; }
		if (depth) { glClearDepth(cd); mask |= GL_DEPTH_BUFFER_BIT; }
		if (stencil) { glClearStencil(cs); mask |= GL_STENCIL_BUFFER_BIT; }
		if (mask) glClear(mask);
	}

	void GLDevice::Draw(PrimitiveType type, uint32_t count, uint32_t start) {
		GLenum mode = GL_TRIANGLES;
		switch (type) {
		case PrimitiveType::Triangles:     mode = GL_TRIANGLES; break;
		case PrimitiveType::TriangleStrip: mode = GL_TRIANGLE_STRIP; break;
		case PrimitiveType::Lines:         mode = GL_LINES; break;
		case PrimitiveType::LineStrip:     mode = GL_LINE_STRIP; break;
		case PrimitiveType::Points:        mode = GL_POINTS; break;
		}
		glDrawArrays(mode, start, count);
	}

	void GLDevice::DrawIndexed(PrimitiveType type, uint32_t count, uint32_t start, uint32_t) {
		GLenum mode = GL_TRIANGLES;
		switch (type) {
		case PrimitiveType::Triangles:     mode = GL_TRIANGLES; break;
		case PrimitiveType::TriangleStrip: mode = GL_TRIANGLE_STRIP; break;
		case PrimitiveType::Lines:         mode = GL_LINES; break;
		case PrimitiveType::LineStrip:     mode = GL_LINE_STRIP; break;
		case PrimitiveType::Points:        mode = GL_POINTS; break;
		}
		glDrawElements(mode, count, GL_UNSIGNED_INT, (void*)(start * sizeof(uint32_t)));
	}

	uint32_t GLDevice::CreateVertexBuffer(const void* data, uint32_t size, BufferUsage usage) {
		GLuint buf;
		glGenBuffers(1, &buf);
		glBindBuffer(GL_ARRAY_BUFFER, buf);
		glBufferData(GL_ARRAY_BUFFER, size, data, (usage == BufferUsage::Dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		uint32_t handle = AllocHandle(); m_buffers[handle] = buf; return handle;
	}

	uint32_t GLDevice::CreateIndexBuffer(const void* data, uint32_t size, BufferUsage usage, bool) {
		GLuint buf;
		glGenBuffers(1, &buf);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, (usage == BufferUsage::Dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		uint32_t handle = AllocHandle(); m_buffers[handle] = buf; return handle;
	}

	uint32_t GLDevice::CreateTexture2D(uint32_t w, uint32_t h, uint32_t, TextureFormat fmt, const void* data) {
		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		GLenum internalFormat = (fmt == TextureFormat::R8G8B8A8_UNORM) ? GL_RGBA8 : GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		uint32_t handle = AllocHandle(); m_textures[handle] = tex; return handle;
	}

	uint32_t GLDevice::CreateShader(ShaderType type, const std::string& source, const std::string&) {
		GLenum glType = (type == ShaderType::Vertex) ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
		GLuint shader = glCreateShader(glType);
		const char* src = source.c_str();
		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);
		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (!status) {
			char log[512];
			glGetShaderInfoLog(shader, 512, nullptr, log);
			USE_LOG_ERROR("GLDevice: Shader compile error: %s", log);
			glDeleteShader(shader);
			return 0;
		}
		uint32_t handle = AllocHandle(); m_shaders[handle] = shader; return handle;
	}

	uint32_t GLDevice::CreateProgram(const std::vector<uint32_t>& shaders) {
		GLuint prog = glCreateProgram();
		for (uint32_t sh : shaders) {
			auto it = m_shaders.find(sh);
			if (it != m_shaders.end()) glAttachShader(prog, it->second);
		}
		glLinkProgram(prog);
		GLint status;
		glGetProgramiv(prog, GL_LINK_STATUS, &status);
		if (!status) {
			char log[512];
			glGetProgramInfoLog(prog, 512, nullptr, log);
			USE_LOG_ERROR("GLDevice: Program link error: %s", log);
			glDeleteProgram(prog);
			return 0;
		}
		uint32_t handle = AllocHandle(); m_programs[handle] = prog; return handle;
	}

	void GLDevice::SetVertexBuffer(uint32_t handle, uint32_t, uint32_t, uint32_t) {
		auto it = m_buffers.find(handle);
		if (it != m_buffers.end()) glBindBuffer(GL_ARRAY_BUFFER, it->second);
	}

	void GLDevice::SetIndexBuffer(uint32_t handle, bool) {
		auto it = m_buffers.find(handle);
		if (it != m_buffers.end()) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, it->second);
	}

	void GLDevice::SetTexture(uint32_t handle, uint32_t slot) {
		glActiveTexture(GL_TEXTURE0 + slot);
		auto it = m_textures.find(handle);
		if (it != m_textures.end()) glBindTexture(GL_TEXTURE_2D, it->second);
		else glBindTexture(GL_TEXTURE_2D, 0);
	}

	void GLDevice::SetProgram(uint32_t handle) {
		auto it = m_programs.find(handle);
		glUseProgram(it != m_programs.end() ? it->second : 0);
	}

	void GLDevice::UpdateBuffer(uint32_t handle, const void* data, uint32_t size) {
		auto it = m_buffers.find(handle);
		if (it != m_buffers.end()) {
			glBindBuffer(GL_ARRAY_BUFFER, it->second);
			glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
		}
	}

	void GLDevice::UpdateTexture(uint32_t handle, uint32_t, const void* data) {
		auto it = m_textures.find(handle);
		if (it != m_textures.end()) {
			glBindTexture(GL_TEXTURE_2D, it->second);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
	}

	void GLDevice::DestroyBuffer(uint32_t handle) {
		auto it = m_buffers.find(handle);
		if (it != m_buffers.end()) { glDeleteBuffers(1, &it->second); m_buffers.erase(it); }
	}

	void GLDevice::DestroyTexture(uint32_t handle) {
		auto it = m_textures.find(handle);
		if (it != m_textures.end()) { glDeleteTextures(1, &it->second); m_textures.erase(it); }
	}

	void GLDevice::DestroyShader(uint32_t handle) {
		auto it = m_shaders.find(handle);
		if (it != m_shaders.end()) { glDeleteShader(it->second); m_shaders.erase(it); }
	}

	void GLDevice::DestroyProgram(uint32_t handle) {
		auto it = m_programs.find(handle);
		if (it != m_programs.end()) { glDeleteProgram(it->second); m_programs.erase(it); }
	}
}