// ============================================================
// Ultimate Source Engine - Render Component
// ============================================================
//
// Component that holds rendering data for an entity: mesh, material,
// and rendering options (cast shadows, receive shadows, etc.).
// ============================================================

#pragma once

#include "stdafx.h"
#include "Entity/Component.h"

namespace USE {

    // Forward declarations
    class Mesh;
    class Material;

    class RenderComponent : public Component {
    public:
        RenderComponent();
        virtual ~RenderComponent();

        // Mesh
        void SetMesh(Mesh* mesh);
        Mesh* GetMesh() const { return m_mesh; }

        // Material
        void SetMaterial(Material* material);
        Material* GetMaterial() const { return m_material; }

        // Shadow options
        void SetCastShadows(bool cast) { m_castShadows = cast; }
        bool GetCastShadows() const { return m_castShadows; }

        void SetReceiveShadows(bool receive) { m_receiveShadows = receive; }
        bool GetReceiveShadows() const { return m_receiveShadows; }

        // Visibility
        void SetVisible(bool visible) { m_visible = visible; }
        bool IsVisible() const { return m_visible; }

        // Type name
        virtual const char* GetTypeName() const override { return "RenderComponent"; }

    private:
        Mesh*     m_mesh;
        Material* m_material;
        bool      m_castShadows;
        bool      m_receiveShadows;
        bool      m_visible;
    };

} // namespace USE