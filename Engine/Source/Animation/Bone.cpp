// ============================================================
// Ultimate Source Engine - Bone Implementation
// ============================================================

#include "stdafx.h"
#include "Bone.h"

namespace USE {

    Bone::Bone()
        : parentIndex(-1)
    {
    }

    Bone::Bone(const std::string& name, int parent)
        : name(name)
        , parentIndex(parent)
    {
    }

} // namespace USE