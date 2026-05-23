// ============================================================
// Ultimate Source Engine - Scene Node
// ============================================================
//
// Represents a node in the scene graph. Each node can contain
// an Entity and have child nodes. The node's world transform is
// derived from its local transform and its parent's world transform.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Transform.h"
#include <vector>
#include <memory>

namespace USE {

    // Forward declarations
    class Entity;

    class SceneNode {
    public:
        SceneNode();
        explicit SceneNode(Entity* entity);
        ~SceneNode();

        // Entity management
        void SetEntity(Entity* entity) { m_entity = entity; }
        Entity* GetEntity() const { return m_entity; }

        // Local transform (relative to parent)
        Transform& GetLocalTransform() { return m_localTransform; }
        const Transform& GetLocalTransform() const { return m_localTransform; }

        // World transform (absolute)
        const Transform& GetWorldTransform() const { return m_worldTransform; }

        // Update world transform based on local and parent's world transform.
        // Call when local transform or parent changes.
        void UpdateWorldTransform(bool recursive = true);

        // Parent/child hierarchy
        SceneNode* GetParent() const { return m_parent; }
        void SetParent(SceneNode* parent);

        void AddChild(SceneNode* child);
        void RemoveChild(SceneNode* child);

        const std::vector<SceneNode*>& GetChildren() const { return m_children; }

    private:
        Entity*                 m_entity;
        Transform               m_localTransform;
        Transform               m_worldTransform;
        SceneNode*              m_parent;
        std::vector<SceneNode*> m_children;
    };

} // namespace USE