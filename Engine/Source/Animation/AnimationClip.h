// ============================================================
// Ultimate Source Engine - Animation Clip
// ============================================================
//
// Represents a single animation clip with keyframe data for each bone.
// Provides methods to sample the pose at a given time.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Transform.h"
#include "Math/Quaternion.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace USE {

    struct PositionKey {
        float time;
        Vector3 value;
    };

    struct RotationKey {
        float time;
        Quaternion value;
    };

    struct ScaleKey {
        float time;
        Vector3 value;
    };

    struct BoneTrack {
        int boneIndex;                      // Index of the bone in the skeleton
        std::vector<PositionKey> positions;
        std::vector<RotationKey> rotations;
        std::vector<ScaleKey> scales;
    };

    class AnimationClip {
    public:
        AnimationClip();
        explicit AnimationClip(const std::string& name);
        ~AnimationClip();

        // Name of the clip
        void SetName(const std::string& name) { m_name = name; }
        const std::string& GetName() const { return m_name; }

        // Duration of the clip in seconds
        void SetDuration(float duration) { m_duration = duration; }
        float GetDuration() const { return m_duration; }

        // Add a bone track (usually loaded from file)
        void AddTrack(const BoneTrack& track);

        // Get number of tracks
        size_t GetTrackCount() const { return m_tracks.size(); }

        // Get a track by index
        const BoneTrack* GetTrack(size_t index) const;

        // Sample the animation at a given time and output local transforms
        // for all bones in the skeleton (the array must be sized to boneCount).
        // The time is clamped/looped based on the loop flag.
        void Sample(float time, bool loop, Transform* outLocalTransforms, int boneCount) const;

    private:
        std::string m_name;
        float m_duration;
        std::unordered_map<int, BoneTrack> m_tracks; // keyed by bone index

        // Helper to get keyframe indices
        static int FindKeyIndex(const std::vector<PositionKey>& keys, float time);
        static int FindKeyIndex(const std::vector<RotationKey>& keys, float time);
        static int FindKeyIndex(const std::vector<ScaleKey>& keys, float time);
    };

} // namespace USE