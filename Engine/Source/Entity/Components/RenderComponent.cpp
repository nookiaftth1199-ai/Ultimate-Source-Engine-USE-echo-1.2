// ============================================================
// Ultimate Source Engine - Render Component Implementation
// ============================================================

#include "stdafx.h"
#include "RenderComponent.h"

namespace USE {

    RenderComponent::RenderComponent()
        : m_mesh(nullptr)
        , m_material(nullptr)
        , m_castShadows(true)
        , m_receiveShadows(true)
        , m_visible(true)
    {
    }

    RenderComponent::~RenderComponent()
    {
        // The mesh and material are managed by the resource manager;
        // we do not delete them here.
    }

    void RenderComponent::SetMesh(Mesh* mesh)
    {
        m_mesh = mesh;
    }

    void RenderComponent::SetMaterial(Material* material)
    {
        m_material = material;
    }

} // namespace USE