// ============================================================
// Ultimate Source Engine - Transform Implementation
// ============================================================

#include "stdafx.h"
#include "Transform.h"

namespace USE {

    // -----------------------------------------------------------------
    // Constructors
    // -----------------------------------------------------------------
    Transform::Transform() : translation(0,0,0), rotation(), scale(1,1,1) {}

    Transform::Transform(const Vector3& t, const Quaternion& r, const Vector3& s)
        : translation(t), rotation(r), scale(s) {}

    Transform::Transform(const Vector3& t, const Quaternion& r)
        : translation(t), rotation(r), scale(1,1,1) {}

    Transform::Transform(const Vector3& t)
        : translation(t), rotation(), scale(1,1,1) {}

    Transform::Transform(const Quaternion& r)
        : translation(0,0,0), rotation(r), scale(1,1,1) {}

    Transform::Transform(const Matrix4& m)
    {
        // Extract translation
        translation.x = m.m[3][0];
        translation.y = m.m[3][1];
        translation.z = m.m[3][2];

        // Extract scale (assume orthogonal axes, no shear)
        Vector3 col0(m.m[0][0], m.m[1][0], m.m[2][0]);
        Vector3 col1(m.m[0][1], m.m[1][1], m.m[2][1]);
        Vector3 col2(m.m[0][2], m.m[1][2], m.m[2][2]);

        float sx = col0.Length();
        float sy = col1.Length();
        float sz = col2.Length();

        scale.x = sx;
        scale.y = sy;
        scale.z = sz;

        // Normalize columns to get rotation matrix
        if (sx > 0) col0 /= sx;
        if (sy > 0) col1 /= sy;
        if (sz > 0) col2 /= sz;

        Matrix3 rot;
        rot.m[0][0] = col0.x; rot.m[0][1] = col0.y; rot.m[0][2] = col0.z;
        rot.m[1][0] = col1.x; rot.m[1][1] = col1.y; rot.m[1][2] = col1.z;
        rot.m[2][0] = col2.x; rot.m[2][1] = col2.y; rot.m[2][2] = col2.z;

        rotation = Quaternion(rot);
    }

    // -----------------------------------------------------------------
    // Identity
    // -----------------------------------------------------------------
    Transform Transform::Identity()
    {
        return Transform();
    }

    // -----------------------------------------------------------------
    // ToMatrix
    // -----------------------------------------------------------------
    Matrix4 Transform::ToMatrix() const
    {
        Matrix4 rotMat = rotation.ToMatrix4();
        Matrix4 scaleMat = Matrix4::Scaling(scale);
        Matrix4 transMat = Matrix4::Translation(translation);
        // Order: T * R * S (scale first, then rotate, then translate)
        return transMat * rotMat * scaleMat;
    }

    // -----------------------------------------------------------------
    // Inverse
    // -----------------------------------------------------------------
    Transform Transform::Inverse() const
    {
        Transform inv;
        inv.rotation = rotation.Conjugate();
        inv.scale.x = 1.0f / scale.x;
        inv.scale.y = 1.0f / scale.y;
        inv.scale.z = 1.0f / scale.z;

        // Inverse translation: - (R^-1 * (t / s))
        inv.translation = inv.rotation.Rotate(Vector3(
            -translation.x / scale.x,
            -translation.y / scale.y,
            -translation.z / scale.z
        ));
        return inv;
    }

    // -----------------------------------------------------------------
    // TransformPoint
    // -----------------------------------------------------------------
    Vector3 Transform::TransformPoint(const Vector3& point) const
    {
        // Apply scale, then rotation, then translation
        Vector3 p = point;
        p.x *= scale.x;
        p.y *= scale.y;
        p.z *= scale.z;
        p = rotation.Rotate(p);
        p += translation;
        return p;
    }

    // -----------------------------------------------------------------
    // TransformVector
    // -----------------------------------------------------------------
    Vector3 Transform::TransformVector(const Vector3& vector) const
    {
        // Only rotation and scale, no translation
        Vector3 v = vector;
        v.x *= scale.x;
        v.y *= scale.y;
        v.z *= scale.z;
        v = rotation.Rotate(v);
        return v;
    }

    // -----------------------------------------------------------------
    // Combine transforms
    // -----------------------------------------------------------------
    Transform Transform::operator*(const Transform& other) const
    {
        Transform result;
        result.scale.x = scale.x * other.scale.x;
        result.scale.y = scale.y * other.scale.y;
        result.scale.z = scale.z * other.scale.z;
        result.rotation = rotation * other.rotation;

        // Compute translation: this.translation + this.rotation * (this.scale * other.translation)
        Vector3 t = other.translation;
        t.x *= scale.x;
        t.y *= scale.y;
        t.z *= scale.z;
        result.translation = translation + rotation.Rotate(t);

        return result;
    }

    Transform& Transform::operator*=(const Transform& other)
    {
        *this = *this * other;
        return *this;
    }

    // -----------------------------------------------------------------
    // Equality
    // -----------------------------------------------------------------
    bool Transform::operator==(const Transform& other) const
    {
        return translation == other.translation &&
               rotation == other.rotation &&
               scale == other.scale;
    }

    bool Transform::operator!=(const Transform& other) const
    {
        return !(*this == other);
    }

    // -----------------------------------------------------------------
    // Static factories
    // -----------------------------------------------------------------
    Transform Transform::Translation(const Vector3& t)
    {
        return Transform(t);
    }

    Transform Transform::Rotation(const Quaternion& r)
    {
        return Transform(Vector3::Zero, r, Vector3::One);
    }

    Transform Transform::RotationAxis(const Vector3& axis, float angle)
    {
        return Rotation(Quaternion::FromAxisAngle(axis, angle));
    }

    Transform Transform::Scaling(const Vector3& s)
    {
        return Transform(Vector3::Zero, Quaternion::Identity, s);
    }

    Transform Transform::Scaling(float s)
    {
        return Scaling(Vector3(s, s, s));
    }

    Transform Transform::LookAt(const Vector3& position, const Vector3& target, const Vector3& up)
    {
        // Compute rotation to look from position to target
        Vector3 forward = (target - position).Normalized();
        Vector3 right = up.Cross(forward).Normalized();
        Vector3 newUp = forward.Cross(right).Normalized();

        // Build rotation matrix
        Matrix3 rotMat;
        rotMat.m[0][0] = right.x;   rotMat.m[0][1] = right.y;   rotMat.m[0][2] = right.z;
        rotMat.m[1][0] = newUp.x;   rotMat.m[1][1] = newUp.y;   rotMat.m[1][2] = newUp.z;
        rotMat.m[2][0] = forward.x; rotMat.m[2][1] = forward.y; rotMat.m[2][2] = forward.z;

        Quaternion rot(rotMat);
        return Transform(position, rot, Vector3::One);
    }

} // namespace USE