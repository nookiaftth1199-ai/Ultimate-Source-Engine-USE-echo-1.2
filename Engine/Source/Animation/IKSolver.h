// ============================================================
// Ultimate Source Engine - Inverse Kinematics (IK) System
//============================================================
//
// Provides methods to solve IK chains for character limbs.
// Currently implements a CCD (Cyclic Coordinate Descent) solver
// for a single chain of bones.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector3.h"
#include "Math/Transform.h"
#include <vector>

namespace USE {

    class Skeleton;

    class IKSolver {
    public:
        // Describes a chain of bones from root to end effector.
        struct Chain {
            std::vector<int> boneIndices;   // indices from root to effector (inclusive)
            int              effectorIndex; // index of the last bone in the chain (same as boneIndices.back())
            float            maxIterations = 20;
            float            tolerance = 0.01f;  // distance tolerance to target
        };

        // Solve a single chain using CCD. Returns true if converged.
        // The solver directly modifies the local transforms of the bones in the skeleton.
        static bool SolveCCD(Skeleton* skeleton, const Chain& chain, const Vector3& target);

        // Compute the world position of a bone's joint (the end of the bone).
        // This is the position of the bone's tip (after applying its local transform and all parent transforms).
        static Vector3 GetBoneTipWorldPosition(const Skeleton* skeleton, int boneIndex);

    private:
        // Helper: compute the length of a bone (distance from joint to tip).
        static float GetBoneLength(const Skeleton* skeleton, int boneIndex);
    };

} // namespace USE