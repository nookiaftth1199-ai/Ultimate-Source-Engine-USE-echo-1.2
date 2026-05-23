// ============================================================
// Ultimate Source Engine - Navigation Mesh
//============================================================
//
// Wrapper around Detour navigation mesh. Provides pathfinding
// and spatial queries.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector3.h"
#include <vector>

// Forward declarations for Detour types
struct dtNavMesh;
struct dtNavMeshQuery;
struct dtQueryFilter;

namespace USE {

    // Build parameters (matching Recast)
    struct NavMeshBuildParams {
        float cellSize = 0.3f;
        float cellHeight = 0.2f;
        float agentHeight = 2.0f;
        float agentRadius = 0.5f;
        float agentMaxClimb = 0.5f;
        float agentMaxSlope = 45.0f;
        float regionMinSize = 8.0f;
        float regionMergeSize = 20.0f;
        float edgeMaxLen = 12.0f;
        float edgeMaxError = 1.3f;
        float vertsPerPoly = 6.0f;
        float detailSampleDist = 6.0f;
        float detailSampleMaxError = 1.0f;
    };

    // Single path point
    struct NavPathPoint {
        Vector3 position;
    };

    class NavMesh {
    public:
        NavMesh();
        ~NavMesh();

        // Build navmesh from triangle mesh data (vertices and indices)
        bool Build(const std::vector<Vector3>& vertices,
                   const std::vector<uint32_t>& indices,
                   const NavMeshBuildParams& params = NavMeshBuildParams());

        // Load/save to file
        bool Save(const char* filename) const;
        bool Load(const char* filename);

        // Find path from start to end. Returns true if path found.
        bool FindPath(const Vector3& start, const Vector3& end,
                      std::vector<NavPathPoint>& outPath) const;

        // Check if a point is on the navmesh.
        bool IsPointOnMesh(const Vector3& point) const;

        // Get random point on navmesh.
        bool GetRandomPoint(Vector3& outPoint) const;

        // Get random point around a position within radius.
        bool GetRandomPointAround(const Vector3& center, float radius, Vector3& outPoint) const;

        // Get closest point on navmesh to a given position.
        bool GetClosestPoint(const Vector3& position, Vector3& outPoint) const;

        // Check if navmesh is valid.
        bool IsValid() const { return m_navMesh != nullptr; }

    private:
        dtNavMesh*       m_navMesh;       // Detour navmesh
        dtNavMeshQuery*  m_navQuery;      // Detour query object
        dtQueryFilter*   m_queryFilter;   // Default filter

        // Initialize query object (called when navmesh is set)
        void InitQuery();
    };

} // namespace USE