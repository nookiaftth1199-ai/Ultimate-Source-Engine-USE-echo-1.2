// ============================================================
// Ultimate Source Engine - Octree
//============================================================
//
// A generic octree for spatial partitioning. Stores objects with
// axis‑aligned bounding boxes. Supports insertion, removal, and
// queries (frustum, ray, box).
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/AABB.h"
#include "Math/Frustum.h"
#include "Math/Ray.h"
#include <vector>
#include <memory>

namespace USE {

    template<typename T>
    class Octree {
    public:
        struct Element {
            T object;
            AABB bounds;
        };

        Octree(const AABB& bounds, int maxDepth = 8, int maxElementsPerNode = 8);
        ~Octree();

        // Insert an object with its bounds.
        void Insert(const T& object, const AABB& bounds);

        // Remove an object (uses equality comparison).
        void Remove(const T& object, const AABB& bounds);

        // Clear all objects.
        void Clear();

        // Query functions: return all objects whose bounds intersect the query volume.
        void QueryFrustum(const Frustum& frustum, std::vector<T>& out) const;
        void QueryBox(const AABB& box, std::vector<T>& out) const;
        void QueryRay(const Ray& ray, std::vector<T>& out, float maxDistance = FLT_MAX) const;

        // Get total number of objects.
        int GetObjectCount() const { return m_objectCount; }

    private:
        struct Node {
            AABB bounds;
            std::vector<Element> elements;
            std::unique_ptr<Node> children[8];
            bool isLeaf;

            Node(const AABB& b) : bounds(b), isLeaf(true) {}
        };

        std::unique_ptr<Node> m_root;
        int m_maxDepth;
        int m_maxElementsPerNode;
        int m_objectCount;

        // Recursive insertion
        void Insert(Node* node, const Element& elem, int depth);

        // Recursive removal (returns true if removed)
        bool Remove(Node* node, const T& object, const AABB& bounds);

        // Subdivide a node
        void Subdivide(Node* node);

        // Query helpers
        void QueryFrustum(Node* node, const Frustum& frustum, std::vector<T>& out) const;
        void QueryBox(Node* node, const AABB& box, std::vector<T>& out) const;
        void QueryRay(Node* node, const Ray& ray, float maxDistance, std::vector<T>& out) const;
    };

} // namespace USE

#include "Octree.cpp" // include implementation for template