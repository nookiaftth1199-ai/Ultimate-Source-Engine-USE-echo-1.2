// ============================================================
// Ultimate Source Engine - Bone
// ============================================================
//
// Represents a single bone in a skeleton. Stores its name,
// parent index, local transform, and inverse bind pose matrix.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Transform.h"
#include "Math/Matrix4.h"
#include <string>

namespace USE {

    struct Bone {
        std::string name;               // Bone name (for identification)
        int         parentIndex;         // Index of parent bone (-1 for root)
        Transform   localTransform;      // Transform relative to parent
        Matrix4     inverseBindPose;     // Inverse of global bind pose (used for skinning)

        Bone();
        explicit Bone(const std::string& name, int parent = -1);
    };

} // namespace USE