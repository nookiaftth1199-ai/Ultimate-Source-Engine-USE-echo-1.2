// ============================================================
// Ultimate Source Engine – Shader
// ============================================================
// Minimal shader class. Wraps a GPU program.
// ============================================================

#pragma once

#include <string>
#include <vector>
#include "Math/Vector4.h"
#include "Math/Matrix4.h"

namespace USE
{
	class Shader
	{
	public:
		Shader() = default;
		~Shader() = default;

		// Compile from source strings.
		bool Compile(const std::string& vertexSource, const std::string& pixelSource);

		// Bind / unbind.
		void Bind();
		void Unbind();

		// Uniform setters.
		void SetUniform(const char* name, int value);
		void SetUniform(const char* name, float value);
		void SetUniform(const char* name, const Vector4& value);
		void SetUniform(const char* name, const Matrix4& value);

	private:
		uint32_t m_programID = 0;   // GPU handle
	};
}