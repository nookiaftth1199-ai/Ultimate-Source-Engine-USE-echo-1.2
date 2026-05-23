// ============================================================
// Ultimate Source Engine - Spatial Grid Implementation
// ============================================================

#include "stdafx.h"
#include "SpatialGrid.h"
#include <cmath>

namespace USE {

    template<typename T>
    SpatialGrid<T>::SpatialGrid(float cellSize)
        : m_cellSize(cellSize)
        , m_objectCount(0)
    {
    }

    template<typename T>
    SpatialGrid<T>::~SpatialGrid()
    {
    }

    template<typename T>
    typename SpatialGrid<T>::CellCoord SpatialGrid<T>::GetCoord(const Vector3& pos) const
    {
        CellCoord c;
        c.x = static_cast<int>(floor(pos.x / m_cellSize));
        c.y = static_cast<int>(floor(pos.y / m_cellSize));
        c.z = static_cast<int>(floor(pos.z / m_cellSize));
        return c;
    }

    template<typename T>
    AABB SpatialGrid<T>::GetCellBounds(const CellCoord& coord) const
    {
        Vector3 min(coord.x * m_cellSize, coord.y * m_cellSize, coord.z * m_cellSize);
        Vector3 max = min + Vector3(m_cellSize, m_cellSize, m_cellSize);
        return AABB(min, max);
    }

    template<typename T>
    template<typename Func>
    void SpatialGrid<T>::ForEachCellInBox(const AABB& box, Func func) const
    {
        CellCoord minCoord = GetCoord(box.min);
        CellCoord maxCoord = GetCoord(box.max);

        for (int x = minCoord.x; x <= maxCoord.x; ++x) {
            for (int y = minCoord.y; y <= maxCoord.y; ++y) {
                for (int z = minCoord.z; z <= maxCoord.z; ++z) {
                    CellCoord coord = {x, y, z};
                    auto it = m_grid.find(coord);
                    if (it != m_grid.end()) {
                        func(coord, it->second);
                    }
                }
            }
        }
    }

    template<typename T>
    void SpatialGrid<T>::Insert(const T& object, const AABB& bounds)
    {
        ForEachCellInBox(bounds, [&](const CellCoord& coord, Cell& cell) {
            // Check if object already exists? Not needed, we assume unique.
            cell.objects.push_back(object);
        });
        ++m_objectCount;
    }

    template<typename T>
    void SpatialGrid<T>::Remove(const T& object, const AABB& bounds)
    {
        ForEachCellInBox(bounds, [&](const CellCoord& coord, Cell& cell) {
            // Linear removal (could be optimized if needed)
            cell.objects.erase(std::remove(cell.objects.begin(), cell.objects.end(), object),
                               cell.objects.end());
            // If cell becomes empty, we could optionally erase it.
        });
        --m_objectCount;
    }

    template<typename T>
    void SpatialGrid<T>::Clear()
    {
        m_grid.clear();
        m_objectCount = 0;
    }

    template<typename T>
    void SpatialGrid<T>::QueryBox(const AABB& box, std::vector<T>& out) const
    {
        ForEachCellInBox(box, [&](const CellCoord&, const Cell& cell) {
            // We assume objects in cell may not all intersect the query box,
            // so we need to check each object's bounds. Since we don't store
            // bounds per object, we rely on the caller to filter.
            // Alternative: store bounds with object. For simplicity, we just
            // return all objects in potentially intersecting cells.
            out.insert(out.end(), cell.objects.begin(), cell.objects.end());
        });
    }

    template<typename T>
    void SpatialGrid<T>::QuerySphere(const Vector3& center, float radius, std::vector<T>& out) const
    {
        // Bounding box of sphere
        Vector3 half(radius, radius, radius);
        AABB box(center - half, center + half);
        QueryBox(box, out);
    }

    template<typename T>
    void SpatialGrid<T>::QueryRay(const Ray& ray, float maxDistance, std::vector<T>& out) const
    {
        // Simple approach: march ray through cells (grid traversal)
        // For simplicity, we just query the AABB of the ray segment.
        Vector3 end = ray.origin + ray.direction * maxDistance;
        AABB box;
        box.Encapsulate(ray.origin);
        box.Encapsulate(end);
        QueryBox(box, out);
        // Then the caller must do precise intersection tests.
    }

    // -----------------------------------------------------------------
    // Explicit instantiations for common types
    // -----------------------------------------------------------------
    template class SpatialGrid<Entity*>;
    template class SpatialGrid<int>;

} // namespace USE