// ============================================================
// Ultimate Source Engine - Scene Graph
// ============================================================
//
// Manages a hierarchical tree of entities. Root nodes are added
// explicitly, and the graph can be traversed to update world
// transforms, perform frustum culling, or raycast against all
// entities.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Entity/Entity.h"
#include "Math/Frustum.h"
#include "Math/Ray.h"
#include <vector>

namespace USE {

    // Forward declarations
    class TransformComponent;

    class SceneGraph {
    public:
        SceneGraph();
        ~SceneGraph();

        // Add an entity as a root node (if not already present).
        void AddRoot(Entity* entity);

        // Remove an entity from the roots (does not destroy the entity).
        void RemoveRoot(Entity* entity);

        // Clear all roots (does not destroy entities).
        void Clear();

        // Get all root entities.
        const std::vector<Entity*>& GetRoots() const { return m_roots; }

        // Update world transforms of all entities in the graph.
        // This recursively updates children based on parent transforms.
        // Usually called once per frame before rendering.
        void UpdateWorldTransforms();

        // Collect all entities whose bounding boxes are inside or intersect the frustum.
        // If the entity does not have a bounding volume, it is assumed to be visible.
        // Returns a vector of Entity* (the visible ones).
        std::vector<Entity*> FrustumCull(const Frustum& frustum) const;

        // Raycast against all entities in the graph.
        // Returns the closest hit, or a result with hit=false.
        // This requires that entities have a bounding volume (AABB) and possibly a mesh.
        struct RaycastHit {
            bool hit = false;
            Entity* entity = nullptr;
            float distance = 0.0f;
            Vector3 point;
            Vector3 normal;
        };
        RaycastHit Raycast(const Ray& ray, float maxDistance = FLT_MAX) const;

    private:
        std::vector<Entity*> m_roots;

        // Helper to recursively traverse the hierarchy
        void Traverse(Entity* entity, std::function<void(Entity*)> func) const;
        void TraverseUpdateTransforms(Entity* entity);
    };

} // namespace USE