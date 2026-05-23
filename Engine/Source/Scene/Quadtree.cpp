// ============================================================
// Ultimate Source Engine - Quadtree Implementation
// ============================================================

#include "stdafx.h"
#include "Quadtree.h"

namespace USE {

    // -----------------------------------------------------------------
    // Constructor / Destructor
    // -----------------------------------------------------------------
    template<typename T>
    Quadtree<T>::Quadtree(const Rect& bounds, int maxDepth, int maxElementsPerNode)
        : m_root(std::make_unique<Node>(bounds))
        , m_maxDepth(maxDepth)
        , m_maxElementsPerNode(maxElementsPerNode)
        , m_objectCount(0)
    {
    }

    template<typename T>
    Quadtree<T>::~Quadtree()
    {
    }

    // -----------------------------------------------------------------
    // Insert
    // -----------------------------------------------------------------
    template<typename T>
    void Quadtree<T>::Insert(const T& object, const Rect& bounds)
    {
        Element elem = { object, bounds };
        Insert(m_root.get(), elem, 0);
        ++m_objectCount;
    }

    template<typename T>
    void Quadtree<T>::Insert(Node* node, const Element& elem, int depth)
    {
        if (node->isLeaf) {
            node->elements.push_back(elem);
            // If leaf is too full and not at max depth, subdivide
            if ((int)node->elements.size() > m_maxElementsPerNode && depth < m_maxDepth) {
                Subdivide(node);
                // Redistribute elements to children
                for (const auto& e : node->elements) {
                    for (int i = 0; i < 4; ++i) {
                        if (node->children[i]->bounds.Intersects(e.bounds)) {
                            Insert(node->children[i].get(), e, depth + 1);
                        }
                    }
                }
                node->elements.clear(); // leaf becomes internal
            }
        } else {
            // Internal node: find which children intersect and insert
            for (int i = 0; i < 4; ++i) {
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
    void Quadtree<T>::Remove(const T& object, const Rect& bounds)
    {
        if (Remove(m_root.get(), object, bounds)) {
            --m_objectCount;
        }
    }

    template<typename T>
    bool Quadtree<T>::Remove(Node* node, const T& object, const Rect& bounds)
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
            for (int i = 0; i < 4; ++i) {
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
    void Quadtree<T>::Clear()
    {
        m_root = std::make_unique<Node>(m_root->bounds);
        m_objectCount = 0;
    }

    // -----------------------------------------------------------------
    // Subdivide
    // -----------------------------------------------------------------
    template<typename T>
    void Quadtree<T>::Subdivide(Node* node)
    {
        float halfWidth = node->bounds.width * 0.5f;
        float halfHeight = node->bounds.height * 0.5f;
        float x = node->bounds.x;
        float y = node->bounds.y;

        // Create four children
        // child 0: (x, y) top-left
        node->children[0] = std::make_unique<Node>(Rect(x, y, halfWidth, halfHeight));
        // child 1: (x + halfWidth, y) top-right
        node->children[1] = std::make_unique<Node>(Rect(x + halfWidth, y, halfWidth, halfHeight));
        // child 2: (x, y + halfHeight) bottom-left
        node->children[2] = std::make_unique<Node>(Rect(x, y + halfHeight, halfWidth, halfHeight));
        // child 3: (x + halfWidth, y + halfHeight) bottom-right
        node->children[3] = std::make_unique<Node>(Rect(x + halfWidth, y + halfHeight, halfWidth, halfHeight));

        node->isLeaf = false;
    }

    // -----------------------------------------------------------------
    // QueryRect
    // -----------------------------------------------------------------
    template<typename T>
    void Quadtree<T>::QueryRect(const Rect& rect, std::vector<T>& out) const
    {
        QueryRect(m_root.get(), rect, out);
    }

    template<typename T>
    void Quadtree<T>::QueryRect(Node* node, const Rect& rect, std::vector<T>& out) const
    {
        if (!node->bounds.Intersects(rect)) return;

        if (node->isLeaf) {
            for (const auto& elem : node->elements) {
                if (elem.bounds.Intersects(rect)) {
                    out.push_back(elem.object);
                }
            }
        } else {
            for (int i = 0; i < 4; ++i) {
                QueryRect(node->children[i].get(), rect, out);
            }
        }
    }

    // -----------------------------------------------------------------
    // Explicit instantiations for common types
    // -----------------------------------------------------------------
    template class Quadtree<Entity*>;
    template class Quadtree<int>;

} // namespace USE