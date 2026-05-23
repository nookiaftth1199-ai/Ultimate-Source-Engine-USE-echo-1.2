// ============================================================
// Ultimate Source Engine - Navigation Mesh Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "NavMesh.h"
#include "Core/Logger.h"

// Include Recast & Detour headers
#include "Recast.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "DetourCommon.h"

namespace USE {

    NavMesh::NavMesh()
        : m_navMesh(nullptr)
        , m_navQuery(nullptr)
        , m_queryFilter(nullptr)
    {
    }

    NavMesh::~NavMesh()
    {
        if (m_navQuery) dtFreeNavMeshQuery(m_navQuery);
        if (m_navMesh) dtFreeNavMesh(m_navMesh);
        delete m_queryFilter;
    }

    void NavMesh::InitQuery()
    {
        if (!m_navMesh) return;

        if (m_navQuery) dtFreeNavMeshQuery(m_navQuery);
        m_navQuery = dtAllocNavMeshQuery();
        if (m_navQuery) {
            dtStatus status = m_navQuery->init(m_navMesh, 2048);
            if (dtStatusFailed(status)) {
                dtFreeNavMeshQuery(m_navQuery);
                m_navQuery = nullptr;
            }
        }

        if (!m_queryFilter) {
            m_queryFilter = new dtQueryFilter();
        }
    }

    bool NavMesh::Build(const std::vector<Vector3>& vertices,
                        const std::vector<uint32_t>& indices,
                        const NavMeshBuildParams& params)
    {
        // This is a simplified placeholder. Full Recast integration is complex.
        USE_LOG_ERROR("NavMesh::Build: Not fully implemented.");
        return false;
    }

    bool NavMesh::Save(const char* filename) const
    {
        if (!m_navMesh) return false;
        // Serialize navmesh (see Detour documentation)
        USE_LOG_WARN("NavMesh::Save not implemented.");
        return false;
    }

    bool NavMesh::Load(const char* filename)
    {
        // Deserialize navmesh from file
        USE_LOG_WARN("NavMesh::Load not implemented.");
        return false;
    }

    bool NavMesh::FindPath(const Vector3& start, const Vector3& end,
                           std::vector<NavPathPoint>& outPath) const
    {
        if (!m_navMesh || !m_navQuery) return false;

        // Convert start and end to coordinates in Detour format
        float startPos[3] = { start.x, start.y, start.z };
        float endPos[3]   = { end.x, end.y, end.z };

        dtPolyRef startRef, endRef;
        dtStatus status;

        // Find nearest polygons
        status = m_navQuery->findNearestPoly(startPos, m_queryFilter->getExtents(), m_queryFilter, &startRef, 0);
        if (dtStatusFailed(status) || startRef == 0) return false;

        status = m_navQuery->findNearestPoly(endPos, m_queryFilter->getExtents(), m_queryFilter, &endRef, 0);
        if (dtStatusFailed(status) || endRef == 0) return false;

        // Path corridor
        dtPolyRef polys[256];
        int npolys;
        status = m_navQuery->findPath(startRef, endRef, startPos, endPos, m_queryFilter, polys, &npolys, 256);
        if (dtStatusFailed(status) || npolys == 0) return false;

        // Generate smooth path (simplified: just waypoints at polygon corners)
        float pathPoints[256*3];
        int numPathPoints;
        status = m_navQuery->findStraightPath(startPos, endPos, polys, npolys,
                                               pathPoints, nullptr, nullptr, &numPathPoints, 256);
        if (dtStatusFailed(status)) return false;

        outPath.clear();
        for (int i = 0; i < numPathPoints; ++i) {
            NavPathPoint pt;
            pt.position = Vector3(pathPoints[i*3], pathPoints[i*3+1], pathPoints[i*3+2]);
            outPath.push_back(pt);
        }
        return true;
    }

    bool NavMesh::IsPointOnMesh(const Vector3& point) const
    {
        if (!m_navMesh || !m_navQuery) return false;
        float pos[3] = { point.x, point.y, point.z };
        dtPolyRef ref;
        dtStatus status = m_navQuery->findNearestPoly(pos, m_queryFilter->getExtents(), m_queryFilter, &ref, 0);
        return dtStatusSucceed(status) && ref != 0;
    }

    bool NavMesh::GetRandomPoint(Vector3& outPoint) const
    {
        if (!m_navMesh || !m_navQuery) return false;
        dtPolyRef ref;
        float pt[3];
        dtStatus status = m_navQuery->findRandomPoint(m_queryFilter, &ref, pt);
        if (dtStatusSucceed(status)) {
            outPoint = Vector3(pt[0], pt[1], pt[2]);
            return true;
        }
        return false;
    }

    bool NavMesh::GetRandomPointAround(const Vector3& center, float radius, Vector3& outPoint) const
    {
        if (!m_navMesh || !m_navQuery) return false;

        float centerPos[3] = { center.x, center.y, center.z };
        dtPolyRef startRef;
        dtStatus status = m_navQuery->findNearestPoly(centerPos, m_queryFilter->getExtents(), m_queryFilter, &startRef, 0);
        if (dtStatusFailed(status) || startRef == 0) return false;

        dtPolyRef ref;
        float pt[3];
        status = m_navQuery->findRandomPointAroundCircle(startRef, centerPos, radius, m_queryFilter, &ref, pt);
        if (dtStatusSucceed(status)) {
            outPoint = Vector3(pt[0], pt[1], pt[2]);
            return true;
        }
        return false;
    }

    bool NavMesh::GetClosestPoint(const Vector3& position, Vector3& outPoint) const
    {
        if (!m_navMesh || !m_navQuery) return false;
        float pos[3] = { position.x, position.y, position.z };
        dtPolyRef ref;
        float nearest[3];
        dtStatus status = m_navQuery->findNearestPoly(pos, m_queryFilter->getExtents(), m_queryFilter, &ref, nearest);
        if (dtStatusSucceed(status) && ref != 0) {
            outPoint = Vector3(nearest[0], nearest[1], nearest[2]);
            return true;
        }
        return false;
    }

} // namespace USE