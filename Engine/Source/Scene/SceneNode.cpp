// ============================================================
// Ultimate Source Engine - Scene Node Implementation
// ============================================================

#include "stdafx.h"
#include "SceneNode.h"
#include "Entity/Entity.h"
#include "Entity/Components/TransformComponent.h"
#include <algorithm>

namespace USE {

    SceneNode::SceneNode()
        : m_entity(nullptr)
        , m_parent(nullptr)
    {
    }

    SceneNode::SceneNode(Entity* entity)
        : m_entity(entity)
        , m_parent(nullptr)
    {
        // If the entity has a transform component, use it as local transform.
        if (entity) {
            TransformComponent* tc = entity->GetComponent<TransformComponent>();
            if (tc) {
                m_localTransform = tc->localTransform;
                m_worldTransform = tc->worldTransform;
            }
        }
    }

    SceneNode::~SceneNode()
    {
        // Detach from parent
        if (m_parent) {
            m_parent->RemoveChild(this);
        }
        // Delete children? Not owned, just references.
    }

    void SceneNode::UpdateWorldTransform(bool recursive)
    {
        if (m_parent) {
            m_worldTransform = m_parent->GetWorldTransform() * m_localTransform;
        } else {
            m_worldTransform = m_localTransform;
        }

        // Propagate to children if recursive
        if (recursive) {
            for (auto* child : m_children) {
                child->UpdateWorldTransform(true);
            }
        }
    }

    void SceneNode::SetParent(SceneNode* parent)
    {
        if (m_parent == parent) return;
        if (m_parent) {
            m_parent->RemoveChild(this);
        }
        m_parent = parent;
        if (m_parent) {
            m_parent->AddChild(this);
        }
        UpdateWorldTransform(true);
    }

    void SceneNode::AddChild(SceneNode* child)
    {
        if (!child) return;
        // Avoid duplicates
        for (auto* c : m_children) {
            if (c == child) return;
        }
        m_children.push_back(child);
        if (child->GetParent() != this) {
            child->SetParent(this);
        }
    }

    void SceneNode::RemoveChild(SceneNode* child)
    {
        auto it = std::find(m_children.begin(), m_children.end(), child);
        if (it != m_children.end()) {
            m_children.erase(it);
            if (child->GetParent() == this) {
                child->SetParent(nullptr);
            }
        }
    }

} // namespace USE