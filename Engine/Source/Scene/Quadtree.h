// ============================================================
// Ultimate Source Engine - Quadtree
// ============================================================
//
// A generic quadtree for 2D spatial partitioning. Stores objects
// with axis‑aligned rectangles (2D bounds). Supports insertion,
// removal, and queries (rectangle, ray, frustum? but frustum is 3D).
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Rect.h"
#include "Math/Ray.h" // but ray is 3D; we may need a 2D ray. For simplicity, we'll stick to rectangle queries.
#include <vector>
#include <memory>

namespace USE {

    template<typename T>
    class Quadtree {
    public:
        struct Element {
            T object;
            Rect bounds; // x, y, width, height
        };

        Quadtree(const Rect& bounds, int maxDepth = 8, int maxElementsPerNode = 8);
        ~Quadtree();

        // Insert an object with its bounds.
        void Insert(const T& object, const Rect& bounds);

        // Remove an object (uses equality comparison).
        void Remove(const T& object, const Rect& bounds);

        // Clear all objects.
        void Clear();

        // Query functions: return all objects whose bounds intersect the query rectangle.
        void QueryRect(const Rect& rect, std::vector<T>& out) const;

        // Get total number of objects.
        int GetObjectCount() const { return m_objectCount; }

    private:
        struct Node {
            Rect bounds;
            std::vector<Element> elements;
            std::unique_ptr<Node> children[4];
            bool isLeaf;

            Node(const Rect& b) : bounds(b), isLeaf(true) {}
        };

        std::unique_ptr<Node> m_root;
        int m_maxDepth;
        int m_maxElementsPerNode;
        int m_objectCount;

        // Recursive insertion
        void Insert(Node* node, const Element& elem, int depth);

        // Recursive removal (returns true if removed)
        bool Remove(Node* node, const T& object, const Rect& bounds);

        // Subdivide a node
        void Subdivide(Node* node);

        // Query helpers
        void QueryRect(Node* node, const Rect& rect, std::vector<T>& out) const;
    };

} // namespace USE

#include "Quadtree.cpp" // include implementation for template