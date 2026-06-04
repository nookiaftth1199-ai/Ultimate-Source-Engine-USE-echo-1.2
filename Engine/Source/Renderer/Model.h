// ============================================================
// Ultimate Source Engine - Model
// ============================================================
// A model is a collection of meshes, each with a material.
// ============================================================

#pragma once

#include "Mesh.h"
#include <vector>
#include <string>

namespace USE
{
	class Model
	{
	public:
		Model();
		~Model();

		// Add a mesh (copied into the model).
		void AddMesh(const Mesh& mesh);

		// Get the number of meshes.
		uint32_t GetMeshCount() const { return static_cast<uint32_t>(m_meshes.size()); }

		// Access a mesh.
		Mesh& GetMesh(uint32_t index) { return m_meshes[index]; }
		const Mesh& GetMesh(uint32_t index) const { return m_meshes[index]; }

		// Name (for identification).
		void SetName(const std::string& name) { m_name = name; }
		const std::string& GetName() const { return m_name; }

	private:
		std::vector<Mesh> m_meshes;
		std::string m_name;
	};
}