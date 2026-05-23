// ============================================================
// Ultimate Source Engine - Skybox
// ============================================================
//
// Renders a cubemap texture around the scene to simulate distant
// environment. The skybox is rendered with depth test enabled but
// depth writes disabled, and uses a special shader that samples
// from a cubemap texture.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Matrix4.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Renderer/Texture.h"

namespace USE {

    class Camera;

    class Skybox {
    public:
        Skybox();
        ~Skybox();

        // Load skybox from six individual texture files (order: +X, -X, +Y, -Y, +Z, -Z)
        bool LoadFromFiles(const char* right,  const char* left,
                           const char* top,    const char* bottom,
                           const char* front,  const char* back);

        // Load from a single cubemap file (e.g., DDS cubemap, HDR, etc.) – not implemented yet
        bool LoadFromCubemap(const char* filename);

        // Render the skybox using a camera (view matrix without translation)
        void Render(Camera* camera);

        // Set/get the material (for custom shaders)
        void SetMaterial(Material* material) { m_material = material; }
        Material* GetMaterial() const { return m_material; }

    private:
        Mesh*      m_cubeMesh;      // A unit cube mesh
        Material*  m_material;       // Material with cubemap shader and texture

        // Helper to create the cube mesh (positions only)
        void CreateCubeMesh();
    };

} // namespace USE