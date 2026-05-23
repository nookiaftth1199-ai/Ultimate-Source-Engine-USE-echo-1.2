// ============================================================
// Ultimate Source Engine - Navigation System
//============================================================
//
// Provides navigation mesh generation and pathfinding services.
// Uses Recast & Detour libraries.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector3.h"
#include <vector>

namespace USE {

    // Forward declarations for Recast/Detour types
    struct rcContext;
    class dtNavMesh;
    class dtNavMeshQuery;
    class dtQueryFilter;

    // Navigation mesh generation parameters
    struct NavMeshBuildParams {
        float cellSize = 0.3f;          // voxel size in world units
        float cellHeight = 0.2f;         // voxel height
        float agentHeight = 2.0f;
        float agentRadius = 0.5f;
        float agentMaxClimb = 0.5f;
        float agentMaxSlope = 45.0f;     // degrees
        float regionMinSize = 8.0f;
        float regionMergeSize = 20.0f;
        float edgeMaxLen = 12.0f;
        float edgeMaxError = 1.3f;
        float vertsPerPoly = 6.0f;
        float detailSampleDist = 6.0f;
        float detailSampleMaxError = 1.0f;
    };

    // A single path point (position)
    struct NavPathPoint {
        Vector3 position;
    };

    // Navigation system – main class
    class NavigationSystem {
    public:
        NavigationSystem();
        ~NavigationSystem();

        // Initialize the system (call after Recast/Detour init)
        bool Initialize();
        void Shutdown();

        // Build a navigation mesh from a list of triangles (vertices + indices)
        bool BuildNavMesh(const std::vector<Vector3>& vertices,
                          const std::vector<uint32_t>& indices,
                          const NavMeshBuildParams& params = NavMeshBuildParams());

        // Save/load navmesh to/from file (optional)
        bool SaveNavMesh(const char* filename) const;
        bool LoadNavMesh(const char* filename);

        // Find a path from start to end. Returns true if path found.
        bool FindPath(const Vector3& start, const Vector3& end,
                      std::vector<NavPathPoint>& outPath,
                      float maxPathPoints = 256) const;

        // Check if a point is on the navmesh.
        bool IsPointOnMesh(const Vector3& point) const;

        // Get random point on navmesh (useful for wandering).
        bool GetRandomPoint(Vector3& outPoint) const;

        // Get random point around a given position within radius.
        bool GetRandomPointAround(const Vector3& center, float radius, Vector3& outPoint) const;

    private:
        dtNavMesh*       m_navMesh;       // Detour navigation mesh
        dtNavMeshQuery*  m_navQuery;      // Detour query object
        dtQueryFilter*   m_queryFilter;   // Default filter
        rcContext*       m_rcContext;     // Recast context for logging
        bool             m_initialized;
    };

} // namespace USE