// ============================================================
// Ultimate Source Engine - Morph Animation (Blend Shapes)
//============================================================
//
// Manages blend shape targets and weights. Morph targets are
// used for facial expressions, muscle bulges, etc.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>
#include <vector>

namespace USE {

    // Represents a single blend shape target.
    struct MorphTarget {
        std::string name;       // Name of the target (e.g., "smile", "blink")
        int         index;      // Index in the mesh's blend shape array
        // Actual vertex delta data is stored in the mesh asset.
    };

    class MorphAnimation {
    public:
        MorphAnimation();
        ~MorphAnimation();

        // Add a target (usually called when loading the mesh).
        void AddTarget(const std::string& name, int index);

        // Find target index by name (returns -1 if not found).
        int GetTargetIndex(const std::string& name) const;

        // Get number of targets.
        size_t GetTargetCount() const { return m_targets.size(); }

        // Set/get weight for a target (by index).
        void SetWeight(int targetIndex, float weight);
        float GetWeight(int targetIndex) const;

        // Convenience: set weight by name.
        void SetWeight(const std::string& name, float weight);

        // Get all weights (for passing to shader).
        const std::vector<float>& GetWeights() const { return m_weights; }

    private:
        std::vector<MorphTarget> m_targets;
        std::vector<float>       m_weights;   // parallel to m_targets
    };

} // namespace USE