// ============================================================
// Ultimate Source Engine - Quaternion
// ============================================================
//
// Quaternion class for representing 3D rotations.
// All functions are inline for performance.
// ============================================================

#pragma once

#include <cmath>
#include "Vector3.h"
#include "Matrix3.h"
#include "Matrix4.h"

namespace USE {

    class Quaternion {
    public:
        float x, y, z, w;

        // Constructors
        inline Quaternion();
        inline Quaternion(float x, float y, float z, float w);
        inline explicit Quaternion(const Vector3& axis, float angle); // axis-angle
        inline explicit Quaternion(const Matrix3& rot); // from rotation matrix

        // Identity quaternion
        static inline Quaternion Identity();

        // Basic operations
        inline Quaternion operator-() const;
        inline Quaternion operator+(const Quaternion& q) const;
        inline Quaternion operator-(const Quaternion& q) const;
        inline Quaternion operator*(const Quaternion& q) const; // composition
        inline Quaternion operator*(float s) const;
        inline Quaternion operator/(float s) const;
        inline Quaternion& operator+=(const Quaternion& q);
        inline Quaternion& operator-=(const Quaternion& q);
        inline Quaternion& operator*=(const Quaternion& q);
        inline Quaternion& operator*=(float s);
        inline Quaternion& operator/=(float s);

        // Comparison
        inline bool operator==(const Quaternion& q) const;
        inline bool operator!=(const Quaternion& q) const;

        // Length
        inline float LengthSq() const;
        inline float Length() const;

        // Normalization
        inline Quaternion Normalized() const;
        inline void Normalize();

        // Conjugate and inverse
        inline Quaternion Conjugate() const;
        inline Quaternion Inverse() const;

        // Dot product
        inline float Dot(const Quaternion& q) const;

        // Rotate a vector by this quaternion
        inline Vector3 Rotate(const Vector3& v) const;

        // Convert to matrix
        inline Matrix3 ToMatrix3() const;
        inline Matrix4 ToMatrix4() const;

        // Convert to axis-angle
        inline void ToAxisAngle(Vector3& axis, float& angle) const;

        // Static utility functions
        static inline Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);
        static inline Quaternion FromAxisAngle(const Vector3& axis, float angle);
        static inline Quaternion FromEulerAngles(float pitch, float yaw, float roll); // in radians

        // Constants
        static const Quaternion Identity;
        static const Quaternion Zero;
    };

    // -----------------------------------------------------------------
    // Inline implementations
    // -----------------------------------------------------------------

    Quaternion::Quaternion() : x(0), y(0), z(0), w(1) {}

    Quaternion::Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    Quaternion::Quaternion(const Vector3& axis, float angle) {
        float half = angle * 0.5f;
        float s = sinf(half);
        Vector3 n = axis.Normalized();
        x = n.x * s;
        y = n.y * s;
        z = n.z * s;
        w = cosf(half);
    }

    Quaternion::Quaternion(const Matrix3& rot) {
        // Convert rotation matrix to quaternion
        float trace = rot.m[0][0] + rot.m[1][1] + rot.m[2][2];
        if (trace > 0) {
            float s = 0.5f / sqrtf(trace + 1.0f);
            w = 0.25f / s;
            x = (rot.m[2][1] - rot.m[1][2]) * s;
            y = (rot.m[0][2] - rot.m[2][0]) * s;
            z = (rot.m[1][0] - rot.m[0][1]) * s;
        } else {
            if (rot.m[0][0] > rot.m[1][1] && rot.m[0][0] > rot.m[2][2]) {
                float s = 2.0f * sqrtf(1.0f + rot.m[0][0] - rot.m[1][1] - rot.m[2][2]);
                w = (rot.m[2][1] - rot.m[1][2]) / s;
                x = 0.25f * s;
                y = (rot.m[0][1] + rot.m[1][0]) / s;
                z = (rot.m[0][2] + rot.m[2][0]) / s;
            } else if (rot.m[1][1] > rot.m[2][2]) {
                float s = 2.0f * sqrtf(1.0f + rot.m[1][1] - rot.m[0][0] - rot.m[2][2]);
                w = (rot.m[0][2] - rot.m[2][0]) / s;
                x = (rot.m[0][1] + rot.m[1][0]) / s;
                y = 0.25f * s;
                z = (rot.m[1][2] + rot.m[2][1]) / s;
            } else {
                float s = 2.0f * sqrtf(1.0f + rot.m[2][2] - rot.m[0][0] - rot.m[1][1]);
                w = (rot.m[1][0] - rot.m[0][1]) / s;
                x = (rot.m[0][2] + rot.m[2][0]) / s;
                y = (rot.m[1][2] + rot.m[2][1]) / s;
                z = 0.25f * s;
            }
        }
    }

    Quaternion Quaternion::Identity() {
        return Quaternion(0, 0, 0, 1);
    }

    Quaternion Quaternion::operator-() const {
        return Quaternion(-x, -y, -z, -w);
    }

    Quaternion Quaternion::operator+(const Quaternion& q) const {
        return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
    }

    Quaternion Quaternion::operator-(const Quaternion& q) const {
        return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w);
    }

    Quaternion Quaternion::operator*(const Quaternion& q) const {
        // Hamilton product
        return Quaternion(
            w * q.x + x * q.w + y * q.z - z * q.y,
            w * q.y + y * q.w + z * q.x - x * q.z,
            w * q.z + z * q.w + x * q.y - y * q.x,
            w * q.w - x * q.x - y * q.y - z * q.z
        );
    }

    Quaternion Quaternion::operator*(float s) const {
        return Quaternion(x * s, y * s, z * s, w * s);
    }

    Quaternion Quaternion::operator/(float s) const {
        float inv = 1.0f / s;
        return *this * inv;
    }

    Quaternion& Quaternion::operator+=(const Quaternion& q) {
        x += q.x; y += q.y; z += q.z; w += q.w;
        return *this;
    }

    Quaternion& Quaternion::operator-=(const Quaternion& q) {
        x -= q.x; y -= q.y; z -= q.z; w -= q.w;
        return *this;
    }

    Quaternion& Quaternion::operator*=(const Quaternion& q) {
        *this = *this * q;
        return *this;
    }

    Quaternion& Quaternion::operator*=(float s) {
        x *= s; y *= s; z *= s; w *= s;
        return *this;
    }

    Quaternion& Quaternion::operator/=(float s) {
        float inv = 1.0f / s;
        return *this *= inv;
    }

    bool Quaternion::operator==(const Quaternion& q) const {
        return x == q.x && y == q.y && z == q.z && w == q.w;
    }

    bool Quaternion::operator!=(const Quaternion& q) const {
        return !(*this == q);
    }

    float Quaternion::LengthSq() const {
        return x * x + y * y + z * z + w * w;
    }

    float Quaternion::Length() const {
        return sqrtf(LengthSq());
    }

    Quaternion Quaternion::Normalized() const {
        float len = Length();
        if (len > 0.0f) {
            float inv = 1.0f / len;
            return Quaternion(x * inv, y * inv, z * inv, w * inv);
        }
        return *this;
    }

    void Quaternion::Normalize() {
        float len = Length();
        if (len > 0.0f) {
            float inv = 1.0f / len;
            x *= inv;
            y *= inv;
            z *= inv;
            w *= inv;
        }
    }

    Quaternion Quaternion::Conjugate() const {
        return Quaternion(-x, -y, -z, w);
    }

    Quaternion Quaternion::Inverse() const {
        float lenSq = LengthSq();
        if (lenSq > 0.0f) {
            float inv = 1.0f / lenSq;
            return Quaternion(-x * inv, -y * inv, -z * inv, w * inv);
        }
        return *this;
    }

    float Quaternion::Dot(const Quaternion& q) const {
        return x * q.x + y * q.y + z * q.z + w * q.w;
    }

    Vector3 Quaternion::Rotate(const Vector3& v) const {
        // v' = q * v * q^-1 (with v as pure quaternion)
        Quaternion p(v.x, v.y, v.z, 0);
        Quaternion qinv = Inverse();
        Quaternion res = *this * p * qinv;
        return Vector3(res.x, res.y, res.z);
    }

    Matrix3 Quaternion::ToMatrix3() const {
        float xx = x * x, yy = y * y, zz = z * z;
        float xy = x * y, xz = x * z, yz = y * z;
        float wx = w * x, wy = w * y, wz = w * z;

        return Matrix3(
            1 - 2*(yy + zz), 2*(xy - wz),     2*(xz + wy),
            2*(xy + wz),     1 - 2*(xx + zz), 2*(yz - wx),
            2*(xz - wy),     2*(yz + wx),     1 - 2*(xx + yy)
        );
    }

    Matrix4 Quaternion::ToMatrix4() const {
        Matrix3 rot = ToMatrix3();
        return Matrix4(
            rot.m[0][0], rot.m[0][1], rot.m[0][2], 0,
            rot.m[1][0], rot.m[1][1], rot.m[1][2], 0,
            rot.m[2][0], rot.m[2][1], rot.m[2][2], 0,
            0, 0, 0, 1
        );
    }

    void Quaternion::ToAxisAngle(Vector3& axis, float& angle) const {
        Quaternion q = Normalized();
        angle = 2.0f * acosf(q.w);
        float s = sqrtf(1.0f - q.w * q.w);
        if (s < 0.001f) {
            axis.x = 1.0f;
            axis.y = 0.0f;
            axis.z = 0.0f;
        } else {
            axis.x = q.x / s;
            axis.y = q.y / s;
            axis.z = q.z / s;
        }
    }

    Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t) {
        float cosHalfTheta = a.Dot(b);
        if (cosHalfTheta < 0.0f) {
            // Take the shorter path
            return Slerp(a, -b, t);
        }

        if (cosHalfTheta > 0.999f) {
            // Too close, use linear interpolation
            Quaternion result = a + t * (b - a);
            result.Normalize();
            return result;
        }

        float halfTheta = acosf(cosHalfTheta);
        float sinHalfTheta = sqrtf(1.0f - cosHalfTheta * cosHalfTheta);

        float scaleA = sinf((1.0f - t) * halfTheta) / sinHalfTheta;
        float scaleB = sinf(t * halfTheta) / sinHalfTheta;

        return Quaternion(
            a.x * scaleA + b.x * scaleB,
            a.y * scaleA + b.y * scaleB,
            a.z * scaleA + b.z * scaleB,
            a.w * scaleA + b.w * scaleB
        );
    }

    Quaternion Quaternion::FromAxisAngle(const Vector3& axis, float angle) {
        return Quaternion(axis, angle);
    }

    Quaternion Quaternion::FromEulerAngles(float pitch, float yaw, float roll) {
        // Order: Z (yaw), then Y (pitch), then X (roll) – typical for 3D applications
        float cy = cosf(yaw * 0.5f);
        float sy = sinf(yaw * 0.5f);
        float cp = cosf(pitch * 0.5f);
        float sp = sinf(pitch * 0.5f);
        float cr = cosf(roll * 0.5f);
        float sr = sinf(roll * 0.5f);

        Quaternion q;
        q.w = cr * cp * cy + sr * sp * sy;
        q.x = sr * cp * cy - cr * sp * sy;
        q.y = cr * sp * cy + sr * cp * sy;
        q.z = cr * cp * sy - sr * sp * cy;
        return q;
    }

} // namespace USE