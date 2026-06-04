// GLTFLoader.cpp – loads glTF 2.0 using cgltf (if available)
#include "stdafx.h"
#include "GLTFLoader.h"
#include "Renderer/Model.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

// Set to 1 after you download cgltf.h into ThirdParty/
#define USE_CGLTF 0

#if USE_CGLTF
#define CGLTF_IMPLEMENTATION
#include "../ThirdParty/cgltf.h"
#endif

namespace USE
{
	bool GLTFLoader::Load(const std::string& filePath, Model& outModel)
	{
#if USE_CGLTF
		// Read the whole file
		auto data = FileSystem::ReadAllBytes(filePath);
		if (data.empty())
		{
			USE_LOG_ERROR("GLTFLoader: Cannot open %s", filePath.c_str());
			return false;
		}

		cgltf_options options = {};
		cgltf_data* cgltfData = nullptr;
		cgltf_result result = cgltf_parse(&options, data.data(), data.size(), &cgltfData);
		if (result != cgltf_result_success)
		{
			USE_LOG_ERROR("GLTFLoader: cgltf failed to parse %s", filePath.c_str());
			return false;
		}

		result = cgltf_load_buffers(&options, cgltfData, filePath.c_str());
		if (result != cgltf_result_success)
		{
			USE_LOG_ERROR("GLTFLoader: cgltf failed to load buffers for %s", filePath.c_str());
			cgltf_free(cgltfData);
			return false;
		}

		// Process each mesh primitive as a USE Mesh
		for (size_t mi = 0; mi < cgltfData->meshes_count; ++mi)
		{
			cgltf_mesh& cmesh = cgltfData->meshes[mi];
			for (size_t pi = 0; pi < cmesh.primitives_count; ++pi)
			{
				cgltf_primitive& prim = cmesh.primitives[pi];
				if (prim.type != cgltf_primitive_type_triangles) continue;

				// Extract vertex attributes
				struct Vertex { Vector3 pos; Vector3 normal; Vector2 uv; };
				std::vector<Vertex> vertices;
				// We'll assume the primitive has POSITION, NORMAL, TEXCOORD_0
				// For simplicity, we'll just load positions for now
				for (size_t ai = 0; ai < prim.attributes_count; ++ai)
				{
					cgltf_attribute& attr = prim.attributes[ai];
					if (attr.type == cgltf_attribute_type_position)
					{
						cgltf_accessor* accessor = attr.data;
						vertices.resize(accessor->count);
						cgltf_accessor_unpack_floats(accessor, (float*)vertices.data(), accessor->count * sizeof(Vertex) / sizeof(float));
					}
					// (normals, uvs omitted for brevity – you can add them later)
				}

				// Indices
				std::vector<uint32_t> indices;
				if (prim.indices)
				{
					indices.resize(prim.indices->count);
					cgltf_accessor_unpack_uint(prim.indices, indices.data(), prim.indices->count * sizeof(uint32_t));
				}

				// Create a Mesh
				Mesh mesh;
				mesh.SetVertices(vertices.data(), (uint32_t)vertices.size(), sizeof(Vertex),
					{ { "POSITION",0,TextureFormat::R32G32B32_FLOAT,0,0,sizeof(Vertex) } });
				mesh.SetIndices(indices.data(), (uint32_t)indices.size());

				// Material (placeholder)
				Material mat;
				mat.SetName(cmesh.name ? cmesh.name : "glTF_Material");
				mesh.SetMaterial(0, mat);

				outModel.AddMesh(mesh);
			}
		}

		cgltf_free(cgltfData);
		USE_LOG_INFO("GLTFLoader: Loaded %zu meshes from %s", cgltfData->meshes_count, filePath.c_str());
		return true;
#else
		USE_LOG_WARN("GLTFLoader: cgltf not enabled. Set USE_CGLTF=1 and place cgltf.h in ThirdParty/.");
		return false;
#endif
	}
}