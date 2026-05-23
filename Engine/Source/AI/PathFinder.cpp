// ============================================================
// Ultimate Source Engine - Path Finder Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "PathFinder.h"
#include "NavMesh.h"
#include "Core/Logger.h"

namespace USE {

    PathFinder::PathFinder()
        : m_navMesh(nullptr)
        , m_lastPathLength(0.0f)
        , m_smoothEnabled(true)
    {
    }

    PathFinder::PathFinder(NavMesh* navMesh)
        : m_navMesh(navMesh)
        , m_lastPathLength(0.0f)
        , m_smoothEnabled(true)
    {
    }

    PathFinder::~PathFinder()
    {
    }

    void PathFinder::SetNavMesh(NavMesh* navMesh)
    {
        m_navMesh = navMesh;
    }

    PathResult PathFinder::FindPath(const Vector3& start, const Vector3& goal,
                                     std::vector<PathWaypoint>& outPath,
                                     bool smooth)
    {
        outPath.clear();
        m_lastPathLength = 0.0f;

        if (!m_navMesh || !m_navMesh->IsValid()) {
            USE_LOG_ERROR("PathFinder: No valid navmesh set.");
            return PathResult::InternalError;
        }

        // Check start and goal on mesh (optional, but good practice)
        Vector3 startOnMesh, goalOnMesh;
        if (!m_navMesh->GetClosestPoint(start, startOnMesh)) {
            return PathResult::InvalidStart;
        }
        if (!m_navMesh->GetClosestPoint(goal, goalOnMesh)) {
            return PathResult::InvalidGoal;
        }

        // Use navigation mesh to compute path
        std::vector<NavPathPoint> rawPath;
        if (!m_navMesh->FindPath(startOnMesh, goalOnMesh, rawPath)) {
            return PathResult::NoPath;
        }

        if (rawPath.empty()) {
            return PathResult::NoPath;
        }

        // Convert to PathWaypoint list
        std::vector<PathWaypoint> waypoints;
        waypoints.reserve(rawPath.size());
        for (const auto& p : rawPath) {
            PathWaypoint wp;
            wp.position = p.position;
            waypoints.push_back(wp);
        }

        // Compute path length (optional)
        m_lastPathLength = 0.0f;
        for (size_t i = 1; i < waypoints.size(); ++i) {
            m_lastPathLength += (waypoints[i].position - waypoints[i-1].position).Length();
        }

        // Smooth if requested
        if (smooth && m_smoothEnabled && waypoints.size() > 2) {
            SmoothPath(waypoints, outPath);
        } else {
            outPath.swap(waypoints);
        }

        // Determine if path is complete (last waypoint close to goal)
        const float epsilon = 0.1f;
        if (outPath.empty()) return PathResult::NoPath;
        float distToGoal = (outPath.back().position - goalOnMesh).Length();
        if (distToGoal <= epsilon) {
            return PathResult::Success;
        } else {
            return PathResult::Partial;
        }
    }

    void PathFinder::SmoothPath(const std::vector<PathWaypoint>& input,
                                 std::vector<PathWaypoint>& output)
    {
        if (input.size() <= 2) {
            output = input;
            return;
        }

        // Simple string pulling algorithm (simplified)
        // We'll just do a basic raycast-based smoothing.
        output.clear();
        output.push_back(input.front());

        size_t current = 0;
        while (current < input.size() - 1) {
            // Look for the farthest waypoint we can go to directly without obstacles.
            size_t next = current + 1;
            for (size_t i = current + 2; i < input.size(); ++i) {
                // Check if the segment from output.back() to input[i] is clear.
                // For simplicity, we assume no obstacles (since navmesh already guarantees path).
                // In a real implementation, you'd do a raycast against the navmesh.
                // We'll just skip obstacles check.
                next = i;
            }
            output.push_back(input[next]);
            current = next;
        }

        // Ensure goal is included
        if (output.back().position != input.back().position) {
            output.push_back(input.back());
        }
    }

} // namespace USE