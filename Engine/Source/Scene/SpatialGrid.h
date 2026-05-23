// ============================================================
// Ultimate Source Engine - Spatial Grid
// ============================================================
//
// A uniform spatial grid for fast broad-phase queries.
// Divides space into fixed-size cells. Objects are stored in
// cells based on their bounding box or position. Supports
// insertion, removal, and queries (box, sphere, ray).
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector3.h"
#include "Math/AABB.h"
#include "Math/Ray.h"
#include <unordered_map>
#include <vector>
#include <functional>

namespace USE {

    template<typename T>
    class SpatialGrid {
    public:
        // Constructor: specify cell size (width, height, depth).
        SpatialGrid(float cellSize = 1.0f);
        ~SpatialGrid();

        // Insert an object with its bounding box.
        void Insert(const T& object, const AABB& bounds);

        // Remove an object with its bounding box (must match insertion).
        // The object is identified by pointer/equality? We'll use equality operator.
        void Remove(const T& object, const AABB& bounds);

        // Clear all objects.
        void Clear();

        // Query objects whose bounds intersect the given AABB.
        void QueryBox(const AABB& box, std::vector<T>& out) const;

        // Query objects whose bounds intersect the given sphere (center + radius).
        void QuerySphere(const Vector3& center, float radius, std::vector<T>& out) const;

        // Query objects intersected by a ray.
        void QueryRay(const Ray& ray, float maxDistance, std::vector<T>& out) const;

        // Get number of objects.
        int GetObjectCount() const { return m_objectCount; }

    private:
        // Cell coordinates (integer)
        struct CellCoord {
            int x, y, z;
            bool operator==(const CellCoord& other) const {
                return x == other.x && y == other.y && z == other.z;
            }
        };

        // Hash function for CellCoord
        struct CellCoordHash {
            std::size_t operator()(const CellCoord& c) const {
                // simple hash combining
                std::size_t h1 = std::hash<int>()(c.x);
                std::size_t h2 = std::hash<int>()(c.y);
                std::size_t h3 = std::hash<int>()(c.z);
                return h1 ^ (h2 << 1) ^ (h3 << 2);
            }
        };

        // Cell data
        struct Cell {
            std::vector<T> objects;
            AABB bounds; // approximate (could be derived from coords)
        };

        float m_cellSize;
        std::unordered_map<CellCoord, Cell, CellCoordHash> m_grid;
        int m_objectCount;

        // Compute cell coordinate from a world position.
        CellCoord GetCoord(const Vector3& pos) const;

        // Compute cell AABB from coordinates.
        AABB GetCellBounds(const CellCoord& coord) const;

        // Helper to iterate over all cells that intersect a region.
        template<typename Func>
        void ForEachCellInBox(const AABB& box, Func func) const;
    };

} // namespace USE

#include "SpatialGrid.cpp"