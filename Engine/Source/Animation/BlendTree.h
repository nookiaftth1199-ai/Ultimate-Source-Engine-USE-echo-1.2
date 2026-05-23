// ============================================================
// Ultimate Source Engine - Blend Tree
// ============================================================
//
// Node‑based animation blending system. Supports 1D blending
// based on a parameter (e.g., speed). Can be used as the root
// animation source in an Animator.
// ============================================================

#pragma once

#include "stdafx.h"
#include "AnimationClip.h"
#include "Skeleton.h"
#include <vector>
#include <string>
#include <memory>

namespace USE {

    // Forward declarations
    class BlendNode;

    // A parameter used in blend trees (e.g., speed, direction).
    class BlendParameter {
    public:
        BlendParameter(const std::string& name, float defaultValue = 0.0f);
        void SetValue(float value) { m_value = value; }
        float GetValue() const { return m_value; }
        const std::string& GetName() const { return m_name; }
    private:
        std::string m_name;
        float       m_value;
    };

    // Base class for all blend nodes.
    class BlendNode {
    public:
        virtual ~BlendNode() = default;
        // Evaluate the node at the current time and store local transforms in 'outLocal'.
        virtual void Evaluate(float time, Skeleton* skeleton,
                              std::vector<Transform>& outLocal) const = 0;
    };

    // Leaf node: plays a single animation clip.
    class ClipNode : public BlendNode {
    public:
        ClipNode(AnimationClip* clip, float weight = 1.0f);
        void Evaluate(float time, Skeleton* skeleton,
                      std::vector<Transform>& outLocal) const override;
        void SetWeight(float weight) { m_weight = weight; }
    private:
        AnimationClip* m_clip;
        float          m_weight;
    };

    // 1D blend node: blends between multiple children based on a parameter.
    class Blend1DNode : public BlendNode {
    public:
        struct Child {
            std::unique_ptr<BlendNode> node;
            float threshold;            // parameter value at which this child is fully active
        };

        Blend1DNode(BlendParameter* parameter);
        void AddChild(std::unique_ptr<BlendNode> node, float threshold);
        void Evaluate(float time, Skeleton* skeleton,
                      std::vector<Transform>& outLocal) const override;

    private:
        BlendParameter*        m_parameter;
        std::vector<Child>     m_children;
    };

    // Optional: a linear blend node (direct blend of two nodes with fixed weight)
    class LinearBlendNode : public BlendNode {
    public:
        LinearBlendNode(std::unique_ptr<BlendNode> nodeA,
                        std::unique_ptr<BlendNode> nodeB,
                        float blendFactor);
        void Evaluate(float time, Skeleton* skeleton,
                      std::vector<Transform>& outLocal) const override;
    private:
        std::unique_ptr<BlendNode> m_nodeA;
        std::unique_ptr<BlendNode> m_nodeB;
        float                       m_blendFactor;
    };

    // The main blend tree: holds a root node and evaluates it.
    class BlendTree {
    public:
        BlendTree(Skeleton* skeleton);
        ~BlendTree() = default;

        // Set the root node (takes ownership)
        void SetRoot(std::unique_ptr<BlendNode> root) { m_root = std::move(root); }

        // Evaluate the tree at the given time, producing skinning matrices.
        void Evaluate(float time, std::vector<Matrix4>& outSkinningMatrices);

        // Parameter access
        BlendParameter* GetParameter(const std::string& name);

    private:
        Skeleton*                           m_skeleton;
        std::unique_ptr<BlendNode>           m_root;
        std::vector<std::unique_ptr<BlendParameter>> m_parameters;
        // Cached arrays for intermediate results
        mutable std::vector<Transform>       m_localTransforms;
        mutable std::vector<Matrix4>         m_globalMatrices;
        mutable std::vector<Matrix4>         m_skinningMatrices;
    };

} // namespace USE