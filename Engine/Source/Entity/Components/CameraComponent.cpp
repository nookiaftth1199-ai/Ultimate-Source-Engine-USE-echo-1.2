// ============================================================
// Ultimate Source Engine - Camera Component Implementation
// ============================================================

#include "stdafx.h"
#include "CameraComponent.h"
#include "Entity/Entity.h"
#include "TransformComponent.h"
#include "Math/Matrix4.h"

namespace USE {

    CameraComponent::CameraComponent()
        : m_projectionValid(false)
        , m_isPerspective(true)
        , m_fovY(60.0f * 3.14159f / 180.0f)
        , m_aspect(16.0f / 9.0f)
        , m_nearZ(0.1f)
        , m_farZ(1000.0f)
        , m_left(-1), m_right(1), m_bottom(-1), m_top(1)
    {
        // Default perspective
        SetPerspective(m_fovY, m_aspect, m_nearZ, m_farZ);
    }

    CameraComponent::~CameraComponent()
    {
    }

    void CameraComponent::SetPerspective(float fovY, float aspect, float nearZ, float farZ)
    {
        m_fovY = fovY;
        m_aspect = aspect;
        m_nearZ = nearZ;
        m_farZ = farZ;
        m_isPerspective = true;
        m_projectionMatrix = Matrix4::Perspective(fovY, aspect, nearZ, farZ);
        m_projectionValid = true;
    }

    void CameraComponent::SetOrthographic(float left, float right, float bottom, float top,
                                          float nearZ, float farZ)
    {
        m_left = left;
        m_right = right;
        m_bottom = bottom;
        m_top = top;
        m_nearZ = nearZ;
        m_farZ = farZ;
        m_isPerspective = false;
        m_projectionMatrix = Matrix4::Orthographic(left, right, bottom, top, nearZ, farZ);
        m_projectionValid = true;
    }

    Matrix4 CameraComponent::GetViewMatrix() const
    {
        // Get the owning entity's transform
        Entity* owner = GetOwner();
        if (!owner) return Matrix4::Identity();

        TransformComponent* tc = owner->GetComponent<TransformComponent>();
        if (!tc) return Matrix4::Identity();

        // The view matrix is the inverse of the camera's world transform.
        // In a right‑handed coordinate system, we often use a "look at" matrix
        // built from position, forward, up, right. We can derive from transform.
        Matrix4 world = tc->worldTransform.ToMatrix();
        // Invert to get view matrix (camera space)
        return world.Inverse();
    }

} // namespace USE