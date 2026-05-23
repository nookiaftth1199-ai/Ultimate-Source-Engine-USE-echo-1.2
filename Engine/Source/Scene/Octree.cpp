// ============================================================
// Ultimate Source Engine - Octree Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "Octree.h"

namespace USE {

    // -----------------------------------------------------------------
    // Constructor / Destructor
    // -----------------------------------------------------------------
    template<typename T>
    Octree<T>::Octree(const AABB& bounds, int maxDepth, int maxElementsPerNode)
        : m_root(std::make_unique<Node>(bounds))
        , m_maxDepth(maxDepth)
        , m_maxElementsPerNode(maxElementsPerNode)
        , m_objectCount(0)
    {
    }

    template<typename T>
    Octree<T>::~Octree()
    {
    }

    // -----------------------------------------------------------------
    // Insert
    // -----------------------------------------------------------------
    template<typename T>
    void Octree<T>::Insert(const T& object, const AABB& bounds)
    {
        Element elem = { object, bounds };
        Insert(m_root.get(), elem, 0);
        ++m_objectCount;
    }

    template<typename T>
    void Octree<T>::Insert(Node* node, const Element& elem, int depth)
    {
        if (node->isLeaf) {
            node->elements.push_back(elem);
            // If leaf is too full and not at max depth, subdivide
            if ((int)node->elements.size() > m_maxElementsPerNode && depth < m_maxDepth) {
                Subdivide(node);
                // Redistribute elements to children
                for (const auto& e : node->elements) {
                    for (int i = 0; i < 8; ++i) {
                        if (node->children[i]->bounds.Intersects(e.bounds)) {
                            Insert(node->children[i].get(), e, depth + 1);
                        }
                    }
                }
                node->elements.clear(); // leaf becomes internal
            }
        } else {
            // Internal node: find which children intersect and insert
            for (int i = 0; i < 8; ++i) {
                if (node->children[i]->bounds.Intersects(elem.bounds)) {
                    Insert(node->children[i].get(), elem, depth + 1);
                }
            }
        }
    }

    // -----------------------------------------------------------------
    // Remove
    // -----------------------------------------------------------------
    template<typename T>
    void Octree<T>::Remove(const T& object, const AABB& bounds)
    {
        if (Remove(m_root.get(), object, bounds)) {
            --m_objectCount;
        }
    }

    template<typename T>
    bool Octree<T>::Remove(Node* node, const T& object, const AABB& bounds)
    {
        if (node->isLeaf) {
            // Linear search
            for (auto it = node->elements.begin(); it != node->elements.end(); ++it) {
                if (it->object == object) { // requires T to have operator==
                    node->elements.erase(it);
                    return true;
                }
            }
            return false;
        } else {
            // Recursively remove from children
            bool removed = false;
            for (int i = 0; i < 8; ++i) {
                if (node->children[i]->bounds.Intersects(bounds)) {
                    if (Remove(node->children[i].get(), object, bounds)) {
                        removed = true;
                    }
                }
            }
            return removed;
        }
    }

    // -----------------------------------------------------------------
    // Clear
    // -----------------------------------------------------------------
    template<typename T>
    void Octree<T>::Clear()
    {
        m_root = std::make_unique<Node>(m_root->bounds);
        m_objectCount = 0;
    }

    // -----------------------------------------------------------------
    // Subdivide
    // -----------------------------------------------------------------
    template<typename T>
    void Octree<T>::Subdivide(Node* node)
    {
        Vector3 center = node->bounds.Center();
        Vector3 half = node->bounds.Extents();

        // Create eight children with half extents
        for (int i = 0; i < 8; ++i) {
            Vector3 childCenter = center;
            childCenter.x += ((i & 1) ? half.x : -half.x) * 0.5f;
            childCenter.y += ((i & 2) ? half.y : -half.y) * 0.5f;
            childCenter.z += ((i & 4) ? half.z : -half.z) * 0.5f;

            AABB childBounds(childCenter - half * 0.5f, childCenter + half * 0.5f);
            node->children[i] = std::make_unique<Node>(childBounds);
        }
        node->isLeaf = false;
    }

    // -----------------------------------------------------------------
    // Queries
    // -----------------------------------------------------------------
    template<typename T>
    void Octree<T>::QueryFrustum(const Frustum& frustum, std::vector<T>& out) const
    {
        QueryFrustum(m_root.get(), frustum, out);
    }

    template<typename T>
    void Octree<T>::QueryFrustum(Node* node, const Frustum& frustum, std::vector<T>& out) const
    {
        // If node bounds are outside frustum, skip entire subtree
        if (!frustum.ContainsAABB(node->bounds)) return;

        if (node->isLeaf) {
            // Add all elements in this leaf
            for (const auto& elem : node->elements) {
                if (frustum.ContainsAABB(elem.bounds)) {
                    out.push_back(elem.object);
                }
            }
        } else {
            // Recurse into children
            for (int i = 0; i < 8; ++i) {
                QueryFrustum(node->children[i].get(), frustum, out);
            }
        }
    }

    template<typename T>
    void Octree<T>::QueryBox(const AABB& box, std::vector<T>& out) const
    {
        QueryBox(m_root.get(), box, out);
    }

    template<typename T>
    void Octree<T>::QueryBox(Node* node, const AABB& box, std::vector<T>& out) const
    {
        if (!node->bounds.Intersects(box)) return;

        if (node->isLeaf) {
            for (const auto& elem : node->elements) {
                if (elem.bounds.Intersects(box)) {
                    out.push_back(elem.object);
                }
            }
        } else {
            for (int i = 0; i < 8; ++i) {
                QueryBox(node->children[i].get(), box, out);
            }
        }
    }

    template<typename T>
    void Octree<T>::QueryRay(const Ray& ray, float maxDistance, std::vector<T>& out) const
    {
        QueryRay(m_root.get(), ray, maxDistance, out);
    }

    template<typename T>
    void Octree<T>::QueryRay(Node* node, const Ray& ray, float maxDistance, std::vector<T>& out) const
    {
        float tNear, tFar;
        if (!ray.Intersects(node->bounds, tNear, tFar)) return;
        if (tNear > maxDistance) return;

        if (node->isLeaf) {
            for (const auto& elem : node->elements) {
                float t;
                if (ray.Intersects(elem.bounds, t, t) && t <= maxDistance) {
                    out.push_back(elem.object);
                }
            }
        } else {
            for (int i = 0; i < 8; ++i) {
                QueryRay(node->children[i].get(), ray, maxDistance, out);
            }
        }
    }

    // -----------------------------------------------------------------
    // Explicit instantiations for common types
    // -----------------------------------------------------------------
    template class Octree<Entity*>;
    template class Octree<int>; // for testing

} // namespace USE