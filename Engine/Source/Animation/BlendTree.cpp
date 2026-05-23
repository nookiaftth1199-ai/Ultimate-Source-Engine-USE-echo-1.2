// ============================================================
// Ultimate Source Engine - Blend Tree Implementation
// ============================================================

#include "stdafx.h"
#include "BlendTree.h"
#include "Math/MathUtils.h"

namespace USE {

    // -----------------------------------------------------------------
    // BlendParameter
    // -----------------------------------------------------------------
    BlendParameter::BlendParameter(const std::string& name, float defaultValue)
        : m_name(name), m_value(defaultValue)
    {}

    // -----------------------------------------------------------------
    // ClipNode
    // -----------------------------------------------------------------
    ClipNode::ClipNode(AnimationClip* clip, float weight)
        : m_clip(clip), m_weight(weight)
    {}

    void ClipNode::Evaluate(float time, Skeleton* skeleton,
                            std::vector<Transform>& outLocal) const
    {
        if (!m_clip) return;
        // Sample the clip at the given time (non‑looping)
        m_clip->Sample(time, false, outLocal.data(), (int)outLocal.size());
        // Apply weight (scale translation and scale, keep rotation)
        for (size_t i = 0; i < outLocal.size(); ++i) {
            outLocal[i].translation *= m_weight;
            outLocal[i].scale *= m_weight;
            // For rotation, we would need to slerp with identity, but we ignore for simplicity.
        }
    }

    // -----------------------------------------------------------------
    // Blend1DNode
    // -----------------------------------------------------------------
    Blend1DNode::Blend1DNode(BlendParameter* parameter)
        : m_parameter(parameter)
    {}

    void Blend1DNode::AddChild(std::unique_ptr<BlendNode> node, float threshold)
    {
        Child child;
        child.node = std::move(node);
        child.threshold = threshold;
        m_children.push_back(std::move(child));
    }

    void Blend1DNode::Evaluate(float time, Skeleton* skeleton,
                                std::vector<Transform>& outLocal) const
    {
        if (m_children.empty()) return;

        float param = m_parameter->GetValue();

        // Find the two children that surround the parameter value
        if (param <= m_children[0].threshold) {
            // Only first child active
            m_children[0].node->Evaluate(time, skeleton, outLocal);
            return;
        }
        if (param >= m_children.back().threshold) {
            // Only last child active
            m_children.back().node->Evaluate(time, skeleton, outLocal);
            return;
        }

        // Find surrounding children
        int idx = 0;
        for (size_t i = 1; i < m_children.size(); ++i) {
            if (param < m_children[i].threshold) {
                idx = (int)i - 1;
                break;
            }
        }

        const Child& childA = m_children[idx];
        const Child& childB = m_children[idx + 1];

        // Evaluate both children
        std::vector<Transform> localA(outLocal.size());
        std::vector<Transform> localB(outLocal.size());
        childA.node->Evaluate(time, skeleton, localA);
        childB.node->Evaluate(time, skeleton, localB);

        // Compute blend factor
        float range = childB.threshold - childA.threshold;
        float t = (param - childA.threshold) / range;

        // Blend per bone
        for (size_t i = 0; i < outLocal.size(); ++i) {
            outLocal[i].translation = Vector3::Lerp(localA[i].translation, localB[i].translation, t);
            outLocal[i].rotation = Quaternion::Slerp(localA[i].rotation, localB[i].rotation, t);
            outLocal[i].scale = Vector3::Lerp(localA[i].scale, localB[i].scale, t);
        }
    }

    // -----------------------------------------------------------------
    // LinearBlendNode
    // -----------------------------------------------------------------
    LinearBlendNode::LinearBlendNode(std::unique_ptr<BlendNode> nodeA,
                                     std::unique_ptr<BlendNode> nodeB,
                                     float blendFactor)
        : m_nodeA(std::move(nodeA))
        , m_nodeB(std::move(nodeB))
        , m_blendFactor(blendFactor)
    {}

    void LinearBlendNode::Evaluate(float time, Skeleton* skeleton,
                                    std::vector<Transform>& outLocal) const
    {
        std::vector<Transform> localA(outLocal.size());
        std::vector<Transform> localB(outLocal.size());

        m_nodeA->Evaluate(time, skeleton, localA);
        m_nodeB->Evaluate(time, skeleton, localB);

        for (size_t i = 0; i < outLocal.size(); ++i) {
            outLocal[i].translation = Vector3::Lerp(localA[i].translation, localB[i].translation, m_blendFactor);
            outLocal[i].rotation = Quaternion::Slerp(localA[i].rotation, localB[i].rotation, m_blendFactor);
            outLocal[i].scale = Vector3::Lerp(localA[i].scale, localB[i].scale, m_blendFactor);
        }
    }

    // -----------------------------------------------------------------
    // BlendTree
    // -----------------------------------------------------------------
    BlendTree::BlendTree(Skeleton* skeleton)
        : m_skeleton(skeleton)
    {
        int boneCount = skeleton->GetBoneCount();
        m_localTransforms.resize(boneCount);
        m_globalMatrices.resize(boneCount);
        m_skinningMatrices.resize(boneCount);
    }

    BlendParameter* BlendTree::GetParameter(const std::string& name)
    {
        for (auto& p : m_parameters) {
            if (p->GetName() == name) return p.get();
        }
        auto p = std::make_unique<BlendParameter>(name);
        BlendParameter* raw = p.get();
        m_parameters.push_back(std::move(p));
        return raw;
    }

    void BlendTree::Evaluate(float time, std::vector<Matrix4>& outSkinningMatrices)
    {
        if (!m_root) return;

        // Reset local transforms to identity
        for (auto& t : m_localTransforms) t = Transform();

        // Evaluate root node
        m_root->Evaluate(time, m_skeleton, m_localTransforms);

        // Compute global matrices
        m_skeleton->ComputeGlobalTransforms(m_localTransforms.data(), m_globalMatrices.data());

        // Compute skinning matrices (global * inverseBindPose)
        m_skeleton->ComputeSkinningMatrices(m_globalMatrices.data(), m_skinningMatrices.data());

        // Copy to output
        outSkinningMatrices = m_skinningMatrices;
    }

} // namespace USE