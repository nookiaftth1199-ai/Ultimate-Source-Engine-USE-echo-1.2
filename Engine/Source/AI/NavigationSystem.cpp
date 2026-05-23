// ============================================================
// Ultimate Source Engine - Navigation System Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "NavigationSystem.h"
#include "Core/Logger.h"

// Include Recast & Detour headers
#include "Recast.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"

namespace USE {

    NavigationSystem::NavigationSystem()
        : m_navMesh(nullptr)
        , m_navQuery(nullptr)
        , m_queryFilter(nullptr)
        , m_rcContext(nullptr)
        , m_initialized(false)
    {
    }

    NavigationSystem::~NavigationSystem()
    {
        Shutdown();
    }

    bool NavigationSystem::Initialize()
    {
        if (m_initialized) return true;

        m_navQuery = dtAllocNavMeshQuery();
        m_queryFilter = new dtQueryFilter();
        m_rcContext = new rcContext();

        m_initialized = true;
        USE_LOG_INFO("NavigationSystem initialized.");
        return true;
    }

    void NavigationSystem::Shutdown()
    {
        if (m_navMesh) {
            dtFreeNavMesh(m_navMesh);
            m_navMesh = nullptr;
        }
        if (m_navQuery) {
            dtFreeNavMeshQuery(m_navQuery);
            m_navQuery = nullptr;
        }
        delete m_queryFilter;
        delete m_rcContext;
        m_initialized = false;
        USE_LOG_INFO("NavigationSystem shut down.");
    }

    bool NavigationSystem::BuildNavMesh(const std::vector<Vector3>& vertices,
                                         const std::vector<uint32_t>& indices,
                                         const NavMeshBuildParams& params)
    {
        // Convert input to Recast format
        // This is a simplified version – real implementation would be much longer.
        // We'll just log a placeholder.
        USE_LOG_INFO("NavigationSystem::BuildNavMesh not fully implemented (placeholder).");
        return false;
    }

    bool NavigationSystem::SaveNavMesh(const char* filename) const
    {
        if (!m_navMesh) return false;
        // Serialize navmesh to file (dtNavMesh::getDataSize, etc.)
        USE_LOG_INFO("NavigationSystem::SaveNavMesh not implemented.");
        return false;
    }

    bool NavigationSystem::LoadNavMesh(const char* filename)
    {
        // Load from file and create navmesh
        USE_LOG_INFO("NavigationSystem::LoadNavMesh not implemented.");
        return false;
    }

    bool NavigationSystem::FindPath(const Vector3& start, const Vector3& end,
                                     std::vector<NavPathPoint>& outPath,
                                     float maxPathPoints) const
    {
        if (!m_navMesh || !m_navQuery) return false;

        // Convert start/end to Detour polygon references
        // Simplified placeholder
        USE_LOG_INFO("NavigationSystem::FindPath not fully implemented.");
        return false;
    }

    bool NavigationSystem::IsPointOnMesh(const Vector3& point) const
    {
        if (!m_navMesh) return false;
        // Query closest polygon
        USE_LOG_INFO("NavigationSystem::IsPointOnMesh not implemented.");
        return false;
    }

    bool NavigationSystem::GetRandomPoint(Vector3& outPoint) const
    {
        if (!m_navMesh) return false;
        dtPolyRef ref;
        float point[3];
        dtStatus status = m_navQuery->findRandomPoint(m_queryFilter, frand, &ref, point);
        if (dtStatusSucceed(status)) {
            outPoint = Vector3(point[0], point[1], point[2]);
            return true;
        }
        return false;
    }

    bool NavigationSystem::GetRandomPointAround(const Vector3& center, float radius, Vector3& outPoint) const
    {
        if (!m_navMesh) return false;
        dtPolyRef ref;
        float pt[3] = {center.x, center.y, center.z};
        float randPt[3];
        dtStatus status = m_navQuery->findRandomPointAroundCircle(ref, pt, radius, m_queryFilter, frand, &ref, randPt);
        if (dtStatusSucceed(status)) {
            outPoint = Vector3(randPt[0], randPt[1], randPt[2]);
            return true;
        }
        return false;
    }

} // namespace USE