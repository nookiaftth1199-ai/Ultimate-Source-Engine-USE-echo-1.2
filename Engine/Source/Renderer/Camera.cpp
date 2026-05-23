// ============================================================
// Ultimate Source Engine - Camera Implementation
// ============================================================

#include "stdafx.h"
#include "Camera.h"
#include "Math/MathUtils.h"

namespace USE {

    Camera::Camera()
        : m_position(0, 0, 0)
        , m_yaw(0)
        , m_pitch(0)
        , m_viewDirty(true)
        , m_fovY(60.0f * MathUtils::DEG_TO_RAD)
        , m_aspect(16.0f/9.0f)
        , m_nearZ(0.1f)
        , m_farZ(1000.0f)
        , m_orthographic(false)
        , m_left(-1), m_right(1), m_bottom(-1), m_top(1)
    {
        UpdateViewMatrix();
        SetPerspective(m_fovY, m_aspect, m_nearZ, m_farZ);
    }

    void Camera::SetPerspective(float fovY, float aspect, float nearZ, float farZ)
    {
        m_fovY = fovY;
        m_aspect = aspect;
        m_nearZ = nearZ;
        m_farZ = farZ;
        m_orthographic = false;
        m_projMatrix = Matrix4::Perspective(fovY, aspect, nearZ, farZ);
    }

    void Camera::SetOrthographic(float left, float right, float bottom, float top, float nearZ, float farZ)
    {
        m_left = left;
        m_right = right;
        m_bottom = bottom;
        m_top = top;
        m_nearZ = nearZ;
        m_farZ = farZ;
        m_orthographic = true;
        m_projMatrix = Matrix4::Orthographic(left, right, bottom, top, nearZ, farZ);
    }

    void Camera::SetPosition(const Vector3& pos)
    {
        m_position = pos;
        m_viewDirty = true;
    }

    void Camera::SetRotation(float yawDegrees, float pitchDegrees)
    {
        m_yaw = MathUtils::Radians(yawDegrees);
        m_pitch = MathUtils::Radians(pitchDegrees);
        // Clamp pitch to avoid gimbal lock
        m_pitch = MathUtils::Clamp(m_pitch, -MathUtils::HALF_PI + 0.01f, MathUtils::HALF_PI - 0.01f);
        m_viewDirty = true;
    }

    void Camera::LookAt(const Vector3& target, const Vector3& worldUp)
    {
        Vector3 forward = (target - m_position).Normalized();
        Vector3 right = worldUp.Cross(forward).Normalized();
        Vector3 up = forward.Cross(right).Normalized();

        // Build view matrix directly
        m_viewMatrix = Matrix4::LookAt(m_position, target, worldUp);
        m_viewDirty = false;

        // Optionally extract yaw/pitch from forward vector
        // Not needed for movement if we rely on direct view matrix.
        // But we can compute for consistency:
        // forward = GetForward(); then m_yaw = atan2(forward.z, forward.x); m_pitch = asin(forward.y);
        // However, we might leave them as is; they'll be overwritten if we use SetRotation later.
    }

    void Camera::MoveForward(float distance)
    {
        m_position += GetForward() * distance;
        m_viewDirty = true;
    }

    void Camera::MoveRight(float distance)
    {
        m_position += GetRight() * distance;
        m_viewDirty = true;
    }

    void Camera::MoveUp(float distance)
    {
        m_position += Vector3(0,1,0) * distance; // world up, not camera up
        m_viewDirty = true;
    }

    void Camera::RotateYaw(float degrees)
    {
        m_yaw += MathUtils::Radians(degrees);
        m_viewDirty = true;
    }

    void Camera::RotatePitch(float degrees)
    {
        m_pitch += MathUtils::Radians(degrees);
        m_pitch = MathUtils::Clamp(m_pitch, -MathUtils::HALF_PI + 0.01f, MathUtils::HALF_PI - 0.01f);
        m_viewDirty = true;
    }

    const Matrix4& Camera::GetViewMatrix()
    {
        if (m_viewDirty) {
            UpdateViewMatrix();
        }
        return m_viewMatrix;
    }

    Vector3 Camera::GetForward() const
    {
        // Standard forward based on yaw/pitch
        float cosPitch = cosf(m_pitch);
        return Vector3(
            cosf(m_yaw) * cosPitch,
            sinf(m_pitch),
            sinf(m_yaw) * cosPitch
        ).Normalized();
    }

    Vector3 Camera::GetRight() const
    {
        // Right = cross(forward, worldUp) normalized
        Vector3 forward = GetForward();
        return Vector3(0,1,0).Cross(forward).Normalized();
    }

    Vector3 Camera::GetUp() const
    {
        // Up = cross(right, forward)
        return GetRight().Cross(GetForward()).Normalized();
    }

    void Camera::UpdateViewMatrix()
    {
        Vector3 forward = GetForward();
        Vector3 right = GetRight();
        Vector3 up = GetUp();

        // Build view matrix as lookAt using these vectors
        // The standard lookAt matrix: [right.x, right.y, right.z, -dot(right,eye);
        //                               up.x,   up.y,   up.z,   -dot(up,eye);
        //                               -forward.x, -forward.y, -forward.z, dot(forward,eye);
        //                               0,0,0,1]
        // However, our LookAt function might do the same. But we can construct directly.

        m_viewMatrix = Matrix4(
            right.x, right.y, right.z, -right.Dot(m_position),
            up.x,    up.y,    up.z,    -up.Dot(m_position),
            -forward.x, -forward.y, -forward.z, forward.Dot(m_position),
            0, 0, 0, 1
        );

        m_viewDirty = false;
    }

} // namespace USE