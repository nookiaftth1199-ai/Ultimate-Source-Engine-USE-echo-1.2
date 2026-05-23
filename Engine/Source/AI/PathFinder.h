// ============================================================
// Ultimate Source Engine - Path Finder
//============================================================
//
// High-level pathfinding interface that uses a navigation mesh
// to compute paths between two points. Supports path smoothing,
// partial paths, and path status.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector3.h"
#include <vector>

namespace USE {

    class NavMesh;

    // Path finder result status
    enum class PathResult {
        Success,           // Path found and complete
        Partial,           // Path found but only partial (couldn't reach goal)
        InvalidStart,      // Start point not on navmesh
        InvalidGoal,       // Goal point not on navmesh
        NoPath,            // No path exists
        InternalError      // Something went wrong
    };

    // A single waypoint on the path
    struct PathWaypoint {
        Vector3 position;
    };

    class PathFinder {
    public:
        PathFinder();
        explicit PathFinder(NavMesh* navMesh);
        ~PathFinder();

        // Set the navigation mesh to use for queries.
        void SetNavMesh(NavMesh* navMesh);

        // Find a path from start to goal. The result is stored in outPath.
        PathResult FindPath(const Vector3& start, const Vector3& goal,
                            std::vector<PathWaypoint>& outPath,
                            bool smooth = true);

        // Get the length of the last computed path (in world units).
        float GetLastPathLength() const { return m_lastPathLength; }

        // Enable/disable path smoothing (via string pulling).
        void SetSmoothingEnabled(bool enabled) { m_smoothEnabled = enabled; }
        bool IsSmoothingEnabled() const { return m_smoothEnabled; }

    private:
        NavMesh* m_navMesh;
        float    m_lastPathLength;
        bool     m_smoothEnabled;

        // Internal smoothing function (string pulling).
        void SmoothPath(const std::vector<PathWaypoint>& input,
                        std::vector<PathWaypoint>& output);
    };

} // namespace USE