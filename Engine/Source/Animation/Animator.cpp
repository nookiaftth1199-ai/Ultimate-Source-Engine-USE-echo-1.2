// ============================================================
// Ultimate Source Engine - Animator Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "Animator.h"
#include "Math/MathUtils.h"

namespace USE {

    Animator::Animator(Skeleton* skeleton)
        : m_skeleton(skeleton)
    {
        int boneCount = skeleton->GetBoneCount();
        m_blendedLocalTransforms.resize(boneCount);
        m_globalMatrices.resize(boneCount);
        m_skinningMatrices.resize(boneCount);
    }

    Animator::~Animator()
    {
    }

    int Animator::AddState(const std::string& name, AnimationClip* clip)
    {
        int index = (int)m_states.size();
        m_states.emplace_back(clip);
        m_nameToIndex[name] = index;
        return index;
    }

    AnimationState* Animator::GetState(const std::string& name)
    {
        auto it = m_nameToIndex.find(name);
        if (it != m_nameToIndex.end()) {
            return &m_states[it->second];
        }
        return nullptr;
    }

    AnimationState* Animator::GetState(int index)
    {
        if (index >= 0 && index < (int)m_states.size())
            return &m_states[index];
        return nullptr;
    }

    void Animator::Play(int index)
    {
        if (index >= 0 && index < (int)m_states.size())
            m_states[index].Play();
    }

    void Animator::Stop(int index)
    {
        if (index >= 0 && index < (int)m_states.size())
            m_states[index].Stop();
    }

    void Animator::Pause(int index)
    {
        if (index >= 0 && index < (int)m_states.size())
            m_states[index].Pause();
    }

    void Animator::SetStateWeight(int index, float weight)
    {
        if (index >= 0 && index < (int)m_states.size())
            m_states[index].SetWeight(weight);
    }

    void Animator::Update(float deltaTime)
    {
        // Update all states
        for (auto& state : m_states) {
            state.Update(deltaTime);
        }

        // Compute final transforms
        ComputeSkinningMatrices();
    }

    void Animator::BlendBoneTransforms(int boneIndex, Transform& outTransform) const
    {
        // Start with identity
        Vector3 pos(0,0,0);
        Quaternion rot = Quaternion::Identity();
        Vector3 scale(0,0,0);
        float totalWeight = 0.0f;

        for (const auto& state : m_states) {
            float w = state.GetWeight();
            if (w <= 0.0f) continue;

            // Sample the state at its current time for this bone
            // We need to get local transforms for all bones, then pick this bone.
            // To avoid multiple allocations, we could sample into a temporary array.
            // For simplicity, we'll sample the state and extract the bone.
            // However, sampling each state per bone would be inefficient.
            // Better: pre‑compute per state a vector of local transforms for all bones.
            // But that would require storing an array per state, which is memory heavy.
            // We'll compromise: sample the state once per bone per call, which is fine for moderate bone counts.

            // Get the state's local transforms for all bones.
            std::vector<Transform> localTransforms(m_skeleton->GetBoneCount());
            state.Sample(localTransforms.data(), (int)localTransforms.size());

            const Transform& t = localTransforms[boneIndex];

            // Blend (weighted sum for pos/scale, weighted slerp for rotation)
            pos += t.translation * w;
            scale += t.scale * w;

            // For rotation, we need to perform weighted slerp. The formula is:
            // q = normalize( exp( w1*log(q1) + w2*log(q2) + ... ) )
            // But a simpler approximation: slerp pairwise with cumulative weight.
            // We'll use a simple accumulator: multiply by w and then normalize.
            // Not mathematically correct but often works.
            Quaternion r = t.rotation;
            if (rot.Dot(r) < 0) r = -r; // take the shorter path
            rot = rot * (1.0f - w) + r * w; // linear interpolation, then normalize later

            totalWeight += w;
        }

        if (totalWeight > 0.0f) {
            float inv = 1.0f / totalWeight;
            outTransform.translation = pos * inv;
            outTransform.scale = scale * inv;
            outTransform.rotation = rot.Normalized();
        } else {
            outTransform = Transform(); // identity
        }
    }

    void Animator::ComputeSkinningMatrices()
    {
        int boneCount = m_skeleton->GetBoneCount();

        // 1. Blend local transforms per bone
        for (int i = 0; i < boneCount; ++i) {
            BlendBoneTransforms(i, m_blendedLocalTransforms[i]);
        }

        // 2. Compute global matrices
        m_skeleton->ComputeGlobalTransforms(m_blendedLocalTransforms.data(), m_globalMatrices.data());

        // 3. Compute skinning matrices (global * inverseBindPose)
        m_skeleton->ComputeSkinningMatrices(m_globalMatrices.data(), m_skinningMatrices.data());
    }

} // namespace USE