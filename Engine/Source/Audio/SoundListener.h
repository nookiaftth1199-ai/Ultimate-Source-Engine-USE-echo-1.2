// ============================================================
// Ultimate Source Engine - Sound Listener
//============================================================
//
// Represents the listener in 3D audio space. There is typically
// one listener per audio context. Provides static methods to set
// the listener's position, velocity, orientation, and gain.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector3.h"

namespace USE {

    class SoundListener {
    public:
        // Set listener position (world units)
        static void SetPosition(const Vector3& position);

        // Set listener velocity (for Doppler effect)
        static void SetVelocity(const Vector3& velocity);

        // Set listener orientation (forward and up vectors, must be normalized)
        static void SetOrientation(const Vector3& forward, const Vector3& up);

        // Set master gain for the listener (0.0 to 1.0)
        static void SetGain(float gain);

        // Get current listener properties (if needed)
        static Vector3 GetPosition();
        static Vector3 GetVelocity();
        static void GetOrientation(Vector3& forward, Vector3& up);
        static float GetGain();

    private:
        // Private constructor – all static
        SoundListener() = delete;

        // Cached values (optional, for Get methods)
        static Vector3 s_position;
        static Vector3 s_velocity;
        static Vector3 s_forward;
        static Vector3 s_up;
        static float   s_gain;
    };

} // namespace USE