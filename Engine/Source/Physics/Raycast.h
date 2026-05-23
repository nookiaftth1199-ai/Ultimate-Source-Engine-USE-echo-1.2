// ============================================================
// Ultimate Source Engine - Raycast
//============================================================
//
// Defines the result structure for physics raycasts.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector3.h"

namespace USE {

    // Raycast hit result
    struct RaycastResult {
        bool hit;           // whether the ray hit anything
        float fraction;     // distance fraction along the ray (0 = start, 1 = max distance)
        Vector3 point;      // world space hit point
        Vector3 normal;     // world space surface normal at hit point
        uint32_t bodyId;    // ID of the hit rigid body (0 if none)
        void* userData;     // optional user data associated with the hit body

        RaycastResult()
            : hit(false)
            , fraction(1.0f)
            , point(0,0,0)
            , normal(0,1,0)
            , bodyId(0)
            , userData(nullptr)
        {}
    };

} // namespace USE