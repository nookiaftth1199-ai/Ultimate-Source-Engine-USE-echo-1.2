// ============================================================
// Ultimate Source Engine - Camera
// ============================================================
//
// Represents a view into the 3D world. Handles view and projection
// matrices, and provides movement and rotation methods.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Matrix4.h"
#include "Math/Vector3.h"

namespace USE {

    class Camera {
    public:
        Camera();
        ~Camera() = default;

        // Set projection parameters
        void SetPerspective(float fovY, float aspect, float nearZ, float farZ);
        void SetOrthographic(float left, float right, float bottom, float top, float nearZ, float farZ);

        // Set view parameters
        void SetPosition(const Vector3& pos);
        void SetRotation(float yawDegrees, float pitchDegrees); // yaw around Y, pitch around X (degrees)
        void LookAt(const Vector3& target, const Vector3& worldUp = Vector3(0,1,0));

        // Movement (relative to camera orientation)
        void MoveForward(float distance);
        void MoveRight(float distance);
        void MoveUp(float distance);

        // Rotation (in degrees)
        void RotateYaw(float degrees);
        void RotatePitch(float degrees);

        // Get matrices
        const Matrix4& GetViewMatrix();
        const Matrix4& GetProjectionMatrix() const { return m_projMatrix; }
        Matrix4 GetViewProjectionMatrix() const { return m_projMatrix * m_viewMatrix; }

        // Get camera vectors (world space)
        Vector3 GetPosition() const { return m_position; }
        Vector3 GetForward() const;
        Vector3 GetRight() const;
        Vector3 GetUp() const;

        // Get projection parameters (for frustum, etc.)
        float GetNearZ() const { return m_nearZ; }
        float GetFarZ() const  { return m_farZ; }
        float GetFOV() const   { return m_fovY; }
        float GetAspect() const { return m_aspect; }

    private:
        // Position and orientation
        Vector3 m_position;
        float   m_yaw;      // in radians
        float   m_pitch;    // in radians

        // Matrices
        Matrix4 m_viewMatrix;
        Matrix4 m_projMatrix;
        bool    m_viewDirty;

        // Projection parameters
        float m_fovY;
        float m_aspect;
        float m_nearZ;
        float m_farZ;
        bool  m_orthographic;
        float m_left, m_right, m_bottom, m_top; // ortho bounds

        // Recalculate view matrix from position and orientation
        void UpdateViewMatrix();
    };

} // namespace USE