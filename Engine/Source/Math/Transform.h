// ============================================================
// Ultimate Source Engine - Transform
// ============================================================
//
// Combines translation, rotation, and scale into a single object.
// Provides methods to convert to matrix, transform points/vectors,
// combine transforms, and compute inverse.
// ============================================================

#pragma once

#include "Vector3.h"
#include "Quaternion.h"
#include "Matrix4.h"

namespace USE {

    class Transform {
    public:
        Vector3 translation;
        Quaternion rotation;
        Vector3 scale;

        // Constructors
        inline Transform();
        inline Transform(const Vector3& t, const Quaternion& r, const Vector3& s);
        inline Transform(const Vector3& t, const Quaternion& r);
        inline explicit Transform(const Vector3& t); // translation only
        inline explicit Transform(const Quaternion& r); // rotation only
        inline explicit Transform(const Matrix4& m); // extract from matrix (no shear)

        // Identity transform
        static inline Transform Identity();

        // Conversion to matrix
        inline Matrix4 ToMatrix() const;

        // Inverse transform
        inline Transform Inverse() const;

        // Transform point and vector
        inline Vector3 TransformPoint(const Vector3& point) const;
        inline Vector3 TransformVector(const Vector3& vector) const;

        // Combine transforms (this * other)
        inline Transform operator*(const Transform& other) const;
        inline Transform& operator*=(const Transform& other);

        // Equality
        inline bool operator==(const Transform& other) const;
        inline bool operator!=(const Transform& other) const;

        // Static factories for common transforms
        static inline Transform Translation(const Vector3& t);
        static inline Transform Rotation(const Quaternion& r);
        static inline Transform RotationAxis(const Vector3& axis, float angle);
        static inline Transform Scaling(const Vector3& s);
        static inline Transform Scaling(float s); // uniform

        // Look-at transform (world to local? typical: rotation that points +Z at target)
        static inline Transform LookAt(const Vector3& position, const Vector3& target, const Vector3& up = Vector3::Up);
    };

} // namespace USE