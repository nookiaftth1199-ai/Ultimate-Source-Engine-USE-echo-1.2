// ============================================================
// Ultimate Source Engine - Animation Clip Implementation
// ============================================================

#include "stdafx.h"
#include "AnimationClip.h"
#include <algorithm>

namespace USE {

    AnimationClip::AnimationClip()
        : m_duration(0.0f)
    {
    }

    AnimationClip::AnimationClip(const std::string& name)
        : m_name(name)
        , m_duration(0.0f)
    {
    }

    AnimationClip::~AnimationClip()
    {
    }

    void AnimationClip::AddTrack(const BoneTrack& track)
    {
        m_tracks[track.boneIndex] = track;
    }

    const BoneTrack* AnimationClip::GetTrack(size_t index) const
    {
        if (index < m_tracks.size()) {
            auto it = m_tracks.begin();
            std::advance(it, index);
            return &it->second;
        }
        return nullptr;
    }

    int AnimationClip::FindKeyIndex(const std::vector<PositionKey>& keys, float time)
    {
        int lo = 0;
        int hi = (int)keys.size() - 1;
        while (lo <= hi) {
            int mid = (lo + hi) / 2;
            if (keys[mid].time < time)
                lo = mid + 1;
            else if (keys[mid].time > time)
                hi = mid - 1;
            else
                return mid;
        }
        return lo - 1;
    }

    int AnimationClip::FindKeyIndex(const std::vector<RotationKey>& keys, float time)
    {
        int lo = 0, hi = (int)keys.size() - 1;
        while (lo <= hi) {
            int mid = (lo + hi) / 2;
            if (keys[mid].time < time)
                lo = mid + 1;
            else if (keys[mid].time > time)
                hi = mid - 1;
            else
                return mid;
        }
        return lo - 1;
    }

    int AnimationClip::FindKeyIndex(const std::vector<ScaleKey>& keys, float time)
    {
        int lo = 0, hi = (int)keys.size() - 1;
        while (lo <= hi) {
            int mid = (lo + hi) / 2;
            if (keys[mid].time < time)
                lo = mid + 1;
            else if (keys[mid].time > time)
                hi = mid - 1;
            else
                return mid;
        }
        return lo - 1;
    }

    void AnimationClip::Sample(float time, bool loop, Transform* outLocalTransforms, int boneCount) const
    {
        if (loop) {
            time = fmodf(time, m_duration);
        } else {
            if (time < 0) time = 0;
            if (time > m_duration) time = m_duration;
        }

        for (int i = 0; i < boneCount; ++i) {
            outLocalTransforms[i] = Transform();
        }

        for (const auto& pair : m_tracks) {
            int boneIdx = pair.first;
            if (boneIdx < 0 || boneIdx >= boneCount) continue;

            const BoneTrack& track = pair.second;
            Transform& result = outLocalTransforms[boneIdx];

            if (!track.positions.empty()) {
                int idx = FindKeyIndex(track.positions, time);
                if (idx < 0) {
                    result.translation = track.positions[0].value;
                } else if (idx >= (int)track.positions.size() - 1) {
                    result.translation = track.positions.back().value;
                } else {
                    const PositionKey& k0 = track.positions[idx];
                    const PositionKey& k1 = track.positions[idx + 1];
                    float t = (time - k0.time) / (k1.time - k0.time);
                    result.translation = Vector3::Lerp(k0.value, k1.value, t);
                }
            }

            if (!track.rotations.empty()) {
                int idx = FindKeyIndex(track.rotations, time);
                if (idx < 0) {
                    result.rotation = track.rotations[0].value;
                } else if (idx >= (int)track.rotations.size() - 1) {
                    result.rotation = track.rotations.back().value;
                } else {
                    const RotationKey& k0 = track.rotations[idx];
                    const RotationKey& k1 = track.rotations[idx + 1];
                    float t = (time - k0.time) / (k1.time - k0.time);
                    result.rotation = Quaternion::Slerp(k0.value, k1.value, t);
                }
            }

            if (!track.scales.empty()) {
                int idx = FindKeyIndex(track.scales, time);
                if (idx < 0) {
                    result.scale = track.scales[0].value;
                } else if (idx >= (int)track.scales.size() - 1) {
                    result.scale = track.scales.back().value;
                } else {
                    const ScaleKey& k0 = track.scales[idx];
                    const ScaleKey& k1 = track.scales[idx + 1];
                    float t = (time - k0.time) / (k1.time - k0.time);
                    result.scale = Vector3::Lerp(k0.value, k1.value, t);
                }
            }
        }
    }

} // namespace USE