// ============================================================
// Ultimate Source Engine - Morph Animation Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "MorphAnimation.h"

namespace USE {

    MorphAnimation::MorphAnimation()
    {
    }

    MorphAnimation::~MorphAnimation()
    {
    }

    void MorphAnimation::AddTarget(const std::string& name, int index)
    {
        MorphTarget target;
        target.name = name;
        target.index = index;
        m_targets.push_back(target);
        m_weights.push_back(0.0f);
    }

    int MorphAnimation::GetTargetIndex(const std::string& name) const
    {
        for (size_t i = 0; i < m_targets.size(); ++i) {
            if (m_targets[i].name == name)
                return (int)i;
        }
        return -1;
    }

    void MorphAnimation::SetWeight(int targetIndex, float weight)
    {
        if (targetIndex >= 0 && targetIndex < (int)m_weights.size()) {
            m_weights[targetIndex] = weight;
        }
    }

    float MorphAnimation::GetWeight(int targetIndex) const
    {
        if (targetIndex >= 0 && targetIndex < (int)m_weights.size()) {
            return m_weights[targetIndex];
        }
        return 0.0f;
    }

    void MorphAnimation::SetWeight(const std::string& name, float weight)
    {
        int idx = GetTargetIndex(name);
        if (idx != -1) {
            SetWeight(idx, weight);
        }
    }

} // namespace USE