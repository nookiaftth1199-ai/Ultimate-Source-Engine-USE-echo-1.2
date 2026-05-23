// ============================================================
// Ultimate Source Engine - Scene Graph Implementation
// ============================================================

#include "stdafx.h"
#include "SceneGraph.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/RenderComponent.h"
#include "Math/AABB.h"

namespace USE {

    SceneGraph::SceneGraph()
    {
    }

    SceneGraph::~SceneGraph()
    {
        Clear();
    }

    void SceneGraph::AddRoot(Entity* entity)
    {
        if (!entity) return;
        // Avoid duplicates
        for (auto* e : m_roots) {
            if (e == entity) return;
        }
        m_roots.push_back(entity);
    }

    void SceneGraph::RemoveRoot(Entity* entity)
    {
        auto it = std::find(m_roots.begin(), m_roots.end(), entity);
        if (it != m_roots.end()) {
            m_roots.erase(it);
        }
    }

    void SceneGraph::Clear()
    {
        m_roots.clear();
    }

    void SceneGraph::UpdateWorldTransforms()
    {
        for (auto* root : m_roots) {
            TraverseUpdateTransforms(root);
        }
    }

    void SceneGraph::TraverseUpdateTransforms(Entity* entity)
    {
        if (!entity) return;
        TransformComponent* tc = entity->GetComponent<TransformComponent>();
        if (tc) {
            tc->UpdateWorldTransform(); // Recursively updates children
        } else {
            // If no transform, still recurse children? They might have transforms.
            // We'll assume children are managed via TransformComponent hierarchy.
        }
    }

    std::vector<Entity*> SceneGraph::FrustumCull(const Frustum& frustum) const
    {
        std::vector<Entity*> visible;
        for (auto* root : m_roots) {
            Traverse(root, [&](Entity* entity) {
                // Check if entity has a bounding volume (AABB)
                TransformComponent* tc = entity->GetComponent<TransformComponent>();
                if (!tc) return; // cannot cull without transform? still visible.

                // For now, assume all entities are visible if they have no bounding volume.
                // In a real engine, you'd have a bounding volume component or compute from mesh.
                RenderComponent* rc = entity->GetComponent<RenderComponent>();
                if (!rc) return; // not renderable, skip

                // We need a bounding box. For simplicity, we'll just add if we can't cull.
                // This is a placeholder – you'd get the AABB from the mesh or a dedicated component.
                // Since we don't have a bounding volume yet, we'll just add.
                visible.push_back(entity);
            });
        }
        return visible;
    }

    SceneGraph::RaycastHit SceneGraph::Raycast(const Ray& ray, float maxDistance) const
    {
        RaycastHit closest;
        closest.distance = maxDistance;

        for (auto* root : m_roots) {
            Traverse(root, [&](Entity* entity) {
                // Check if entity has a bounding volume
                TransformComponent* tc = entity->GetComponent<TransformComponent>();
                if (!tc) return;

                RenderComponent* rc = entity->GetComponent<RenderComponent>();
                if (!rc) return;

                // We need a bounding box. For now, we'll skip.
                // In a real engine, you'd have a bounding volume component.
                // For demonstration, we'll assume the entity has an AABB.
                // This is a placeholder.
            });
        }

        return closest;
    }

    void SceneGraph::Traverse(Entity* entity, std::function<void(Entity*)> func) const
    {
        if (!entity) return;
        func(entity);

        TransformComponent* tc = entity->GetComponent<TransformComponent>();
        if (tc) {
            for (auto* child : tc->children) {
                Entity* childEntity = child->GetOwner(); // The child component's owner
                if (childEntity) {
                    Traverse(childEntity, func);
                }
            }
        }
    }

} // namespace USE