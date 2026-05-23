// ============================================================
// Ultimate Source Engine - Mathematics Library
// ============================================================
//
// Single-header math library for 3D graphics and game development.
// Contains vectors, matrices, quaternions, transforms, and utilities.
// All functions are inline for performance.
// ============================================================

#pragma once

#include <cmath>
#include <cstring>
#include <algorithm>
#include <limits>

#ifndef USE_PI
#define USE_PI 3.14159265358979323846f
#endif

#ifndef USE_HALF_PI
#define USE_HALF_PI 1.57079632679489661923f
#endif

#ifndef USE_TAU
#define USE_TAU 6.28318530717958647692f
#endif

#ifndef USE_DEG_TO_RAD
#define USE_DEG_TO_RAD (USE_PI / 180.0f)
#endif

#ifndef USE_RAD_TO_DEG
#define USE_RAD_TO_DEG (180.0f / USE_PI)
#endif

#ifndef USE_FLOAT_EPSILON
#define USE_FLOAT_EPSILON 1e-6f
#endif

namespace USE {

    // -----------------------------------------------------------------
    // Basic math utilities
    // -----------------------------------------------------------------
    inline float Abs(float x) { return x < 0 ? -x : x; }
    inline float Min(float a, float b) { return a < b ? a : b; }
    inline float Max(float a, float b) { return a > b ? a : b; }
    inline float Clamp(float x, float minVal, float maxVal) {
        return x < minVal ? minVal : (x > maxVal ? maxVal : x);
    }
    inline float Lerp(float a, float b, float t) { return a + t * (b - a); }
    inline float Radians(float degrees) { return degrees * USE_DEG_TO_RAD; }
    inline float Degrees(float radians) { return radians * USE_RAD_TO_DEG; }

    // -----------------------------------------------------------------
    // Vector2
    // -----------------------------------------------------------------
    class Vector2 {
    public:
        float x, y;

        Vector2() : x(0), y(0) {}
        explicit Vector2(float s) : x(s), y(s) {}
        Vector2(float x, float y) : x(x), y(y) {}

        Vector2 operator-() const { return Vector2(-x, -y); }

        Vector2 operator+(const Vector2& v) const { return Vector2(x + v.x, y + v.y); }
        Vector2 operator-(const Vector2& v) const { return Vector2(x - v.x, y - v.y); }
        Vector2 operator*(float s) const { return Vector2(x * s, y * s); }
        Vector2 operator/(float s) const { float inv = 1.0f / s; return Vector2(x * inv, y * inv); }

        Vector2& operator+=(const Vector2& v) { x += v.x; y += v.y; return *this; }
        Vector2& operator-=(const Vector2& v) { x -= v.x; y -= v.y; return *this; }
        Vector2& operator*=(float s) { x *= s; y *= s; return *this; }
        Vector2& operator/=(float s) { float inv = 1.0f / s; x *= inv; y *= inv; return *this; }

        bool operator==(const Vector2& v) const { return x == v.x && y == v.y; }
        bool operator!=(const Vector2& v) const { return x != v.x || y != v.y; }

        float LengthSq() const { return x * x + y * y; }
        float Length() const { return sqrtf(LengthSq()); }

        Vector2 Normalized() const {
            float len = Length();
            if (len > 0) {
                float inv = 1.0f / len;
                return Vector2(x * inv, y * inv);
            }
            return *this;
        }

        void Normalize() {
            float len = Length();
            if (len > 0) {
                float inv = 1.0f / len;
                x *= inv;
                y *= inv;
            }
        }

        float Dot(const Vector2& v) const { return x * v.x + y * v.y; }
        float Cross(const Vector2& v) const { return x * v.y - y * v.x; } // scalar cross product

        static float Distance(const Vector2& a, const Vector2& b) { return (a - b).Length(); }
        static float DistanceSq(const Vector2& a, const Vector2& b) { return (a - b).LengthSq(); }

        static const Vector2 Zero;
        static const Vector2 One;
        static const Vector2 Right;   // (1,0)
        static const Vector2 Left;    // (-1,0)
        static const Vector2 Up;       // (0,1)
        static const Vector2 Down;     // (0,-1)
    };

    inline Vector2 operator*(float s, const Vector2& v) { return v * s; }

    // -----------------------------------------------------------------
    // Vector3
    // -----------------------------------------------------------------
    class Vector3 {
    public:
        float x, y, z;

        Vector3() : x(0), y(0), z(0) {}
        explicit Vector3(float s) : x(s), y(s), z(s) {}
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

        Vector3 operator-() const { return Vector3(-x, -y, -z); }

        Vector3 operator+(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
        Vector3 operator-(const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
        Vector3 operator*(float s) const { return Vector3(x * s, y * s, z * s); }
        Vector3 operator/(float s) const { float inv = 1.0f / s; return Vector3(x * inv, y * inv, z * inv); }

        Vector3& operator+=(const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
        Vector3& operator-=(const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
        Vector3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
        Vector3& operator/=(float s) { float inv = 1.0f / s; x *= inv; y *= inv; z *= inv; return *this; }

        bool operator==(const Vector3& v) const { return x == v.x && y == v.y && z == v.z; }
        bool operator!=(const Vector3& v) const { return x != v.x || y != v.y || z != v.z; }

        float LengthSq() const { return x * x + y * y + z * z; }
        float Length() const { return sqrtf(LengthSq()); }

        Vector3 Normalized() const {
            float len = Length();
            if (len > 0) {
                float inv = 1.0f / len;
                return Vector3(x * inv, y * inv, z * inv);
            }
            return *this;
        }

        void Normalize() {
            float len = Length();
            if (len > 0) {
                float inv = 1.0f / len;
                x *= inv;
                y *= inv;
                z *= inv;
            }
        }

        float Dot(const Vector3& v) const { return x * v.x + y * v.y + z * v.z; }
        Vector3 Cross(const Vector3& v) const {
            return Vector3(
                y * v.z - z * v.y,
                z * v.x - x * v.z,
                x * v.y - y * v.x
            );
        }

        static float Distance(const Vector3& a, const Vector3& b) { return (a - b).Length(); }
        static float DistanceSq(const Vector3& a, const Vector3& b) { return (a - b).LengthSq(); }

        static const Vector3 Zero;
        static const Vector3 One;
        static const Vector3 Right;   // (1,0,0)
        static const Vector3 Left;    // (-1,0,0)
        static const Vector3 Up;       // (0,1,0)
        static const Vector3 Down;     // (0,-1,0)
        static const Vector3 Forward;  // (0,0,1)
        static const Vector3 Back;     // (0,0,-1)
    };

    inline Vector3 operator*(float s, const Vector3& v) { return v * s; }

    // -----------------------------------------------------------------
    // Vector4
    // -----------------------------------------------------------------
    class Vector4 {
    public:
        float x, y, z, w;

        Vector4() : x(0), y(0), z(0), w(0) {}
        explicit Vector4(float s) : x(s), y(s), z(s), w(s) {}
        Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
        Vector4(const Vector3& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}

        Vector4 operator-() const { return Vector4(-x, -y, -z, -w); }

        Vector4 operator+(const Vector4& v) const { return Vector4(x + v.x, y + v.y, z + v.z, w + v.w); }
        Vector4 operator-(const Vector4& v) const { return Vector4(x - v.x, y - v.y, z - v.z, w - v.w); }
        Vector4 operator*(float s) const { return Vector4(x * s, y * s, z * s, w * s); }
        Vector4 operator/(float s) const { float inv = 1.0f / s; return Vector4(x * inv, y * inv, z * inv, w * inv); }

        Vector4& operator+=(const Vector4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
        Vector4& operator-=(const Vector4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
        Vector4& operator*=(float s) { x *= s; y *= s; z *= s; w *= s; return *this; }
        Vector4& operator/=(float s) { float inv = 1.0f / s; x *= inv; y *= inv; z *= inv; w *= inv; return *this; }

        bool operator==(const Vector4& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
        bool operator!=(const Vector4& v) const { return x != v.x || y != v.y || z != v.z || w != v.w; }

        float LengthSq() const { return x * x + y * y + z * z + w * w; }
        float Length() const { return sqrtf(LengthSq()); }

        Vector4 Normalized() const {
            float len = Length();
            if (len > 0) {
                float inv = 1.0f / len;
                return Vector4(x * inv, y * inv, z * inv, w * inv);
            }
            return *this;
        }

        void Normalize() {
            float len = Length();
            if (len > 0) {
                float inv = 1.0f / len;
                x *= inv;
                y *= inv;
                z *= inv;
                w *= inv;
            }
        }

        float Dot(const Vector4& v) const { return x * v.x + y * v.y + z * v.z + w * v.w; }

        static const Vector4 Zero;
        static const Vector4 One;
    };

    inline Vector4 operator*(float s, const Vector4& v) { return v * s; }

    // -----------------------------------------------------------------
    // Matrix3 (3x3)
    // -----------------------------------------------------------------
    class Matrix3 {
    public:
        float m[3][3];

        Matrix3() {
            m[0][0] = 1; m[0][1] = 0; m[0][2] = 0;
            m[1][0] = 0; m[1][1] = 1; m[1][2] = 0;
            m[2][0] = 0; m[2][1] = 0; m[2][2] = 1;
        }

        Matrix3(float m00, float m01, float m02,
                float m10, float m11, float m12,
                float m20, float m21, float m22) {
            m[0][0] = m00; m[0][1] = m01; m[0][2] = m02;
            m[1][0] = m10; m[1][1] = m11; m[1][2] = m12;
            m[2][0] = m20; m[2][1] = m21; m[2][2] = m22;
        }

        static Matrix3 Identity() { return Matrix3(); }

        Matrix3 operator*(const Matrix3& b) const {
            Matrix3 result;
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    result.m[i][j] = m[i][0] * b.m[0][j] + m[i][1] * b.m[1][j] + m[i][2] * b.m[2][j];
                }
            }
            return result;
        }

        Vector3 operator*(const Vector3& v) const {
            return Vector3(
                m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
                m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
                m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z
            );
        }

        Matrix3 Transposed() const {
            Matrix3 result;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    result.m[i][j] = m[j][i];
            return result;
        }

        float Determinant() const {
            return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1])
                 - m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])
                 + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
        }

        // Rotation matrices
        static Matrix3 RotationX(float angle) {
            float c = cosf(angle), s = sinf(angle);
            return Matrix3(
                1, 0, 0,
                0, c, -s,
                0, s,  c
            );
        }

        static Matrix3 RotationY(float angle) {
            float c = cosf(angle), s = sinf(angle);
            return Matrix3(
                c, 0, s,
                0, 1, 0,
                -s, 0, c
            );
        }

        static Matrix3 RotationZ(float angle) {
            float c = cosf(angle), s = sinf(angle);
            return Matrix3(
                c, -s, 0,
                s,  c, 0,
                0,  0, 1
            );
        }
    };

    // -----------------------------------------------------------------
    // Matrix4 (4x4)
    // -----------------------------------------------------------------
    class Matrix4 {
    public:
        float m[4][4];

        Matrix4() {
            m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
            m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = 0;
            m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0;
            m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
        }

        Matrix4(float m00, float m01, float m02, float m03,
                float m10, float m11, float m12, float m13,
                float m20, float m21, float m22, float m23,
                float m30, float m31, float m32, float m33) {
            m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
            m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
            m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
            m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
        }

        static Matrix4 Identity() { return Matrix4(); }

        Matrix4 operator*(const Matrix4& b) const {
            Matrix4 result;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    result.m[i][j] = m[i][0] * b.m[0][j] + m[i][1] * b.m[1][j] +
                                      m[i][2] * b.m[2][j] + m[i][3] * b.m[3][j];
                }
            }
            return result;
        }

        Vector4 operator*(const Vector4& v) const {
            return Vector4(
                m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
                m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
                m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
                m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
            );
        }

        Vector3 TransformPoint(const Vector3& p) const {
            float w = m[3][0] * p.x + m[3][1] * p.y + m[3][2] * p.z + m[3][3];
            if (w != 0) {
                float invW = 1.0f / w;
                return Vector3(
                    (m[0][0] * p.x + m[0][1] * p.y + m[0][2] * p.z + m[0][3]) * invW,
                    (m[1][0] * p.x + m[1][1] * p.y + m[1][2] * p.z + m[1][3]) * invW,
                    (m[2][0] * p.x + m[2][1] * p.y + m[2][2] * p.z + m[2][3]) * invW
                );
            }
            return Vector3();
        }

        Vector3 TransformVector(const Vector3& v) const {
            return Vector3(
                m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
                m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
                m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z
            );
        }

        Matrix4 Transposed() const {
            Matrix4 result;
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    result.m[i][j] = m[j][i];
            return result;
        }

        // Transformations
        static Matrix4 Translation(const Vector3& t) {
            return Matrix4(
                1, 0, 0, t.x,
                0, 1, 0, t.y,
                0, 0, 1, t.z,
                0, 0, 0, 1
            );
        }

        static Matrix4 Scaling(const Vector3& s) {
            return Matrix4(
                s.x, 0,   0,   0,
                0,   s.y, 0,   0,
                0,   0,   s.z, 0,
                0,   0,   0,   1
            );
        }

        static Matrix4 Scaling(float s) {
            return Scaling(Vector3(s, s, s));
        }

        static Matrix4 RotationX(float angle) {
            float c = cosf(angle), s = sinf(angle);
            return Matrix4(
                1, 0,  0, 0,
                0, c, -s, 0,
                0, s,  c, 0,
                0, 0,  0, 1
            );
        }

        static Matrix4 RotationY(float angle) {
            float c = cosf(angle), s = sinf(angle);
            return Matrix4(
                c, 0, s, 0,
                0, 1, 0, 0,
                -s,0, c, 0,
                0, 0, 0, 1
            );
        }

        static Matrix4 RotationZ(float angle) {
            float c = cosf(angle), s = sinf(angle);
            return Matrix4(
                c, -s, 0, 0,
                s,  c, 0, 0,
                0,  0, 1, 0,
                0,  0, 0, 1
            );
        }

        // View matrix (look-at)
        static Matrix4 LookAt(const Vector3& eye, const Vector3& target, const Vector3& up) {
            Vector3 z = (eye - target).Normalized(); // forward (actually camera looks down -z)
            Vector3 x = up.Cross(z).Normalized();
            Vector3 y = z.Cross(x).Normalized();

            return Matrix4(
                x.x, x.y, x.z, -x.Dot(eye),
                y.x, y.y, y.z, -y.Dot(eye),
                z.x, z.y, z.z, -z.Dot(eye),
                0, 0, 0, 1
            );
        }

        // Perspective projection
        static Matrix4 Perspective(float fovY, float aspect, float nearZ, float farZ) {
            float yScale = 1.0f / tanf(fovY * 0.5f);
            float xScale = yScale / aspect;
            float zRange = farZ / (farZ - nearZ);

            return Matrix4(
                xScale, 0,      0,                        0,
                0,      yScale, 0,                        0,
                0,      0,      zRange,                   -nearZ * zRange,
                0,      0,      1,                        0
            );
        }

        // Orthographic projection
        static Matrix4 Orthographic(float left, float right, float bottom, float top, float nearZ, float farZ) {
            float invWidth = 1.0f / (right - left);
            float invHeight = 1.0f / (top - bottom);
            float invDepth = 1.0f / (farZ - nearZ);

            return Matrix4(
                2.0f * invWidth, 0,                0,                -(right + left) * invWidth,
                0,                2.0f * invHeight, 0,                -(top + bottom) * invHeight,
                0,                0,                -2.0f * invDepth, -(farZ + nearZ) * invDepth,
                0,                0,                0,                1
            );
        }
    };

    // -----------------------------------------------------------------
    // Quaternion
    // -----------------------------------------------------------------
    class Quaternion {
    public:
        float x, y, z, w;

        Quaternion() : x(0), y(0), z(0), w(1) {}
        Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

        static Quaternion Identity() { return Quaternion(0, 0, 0, 1); }

        Quaternion operator-() const { return Quaternion(-x, -y, -z, -w); }

        Quaternion operator*(const Quaternion& q) const {
            return Quaternion(
                w * q.x + x * q.w + y * q.z - z * q.y,
                w * q.y + y * q.w + z * q.x - x * q.z,
                w * q.z + z * q.w + x * q.y - y * q.x,
                w * q.w - x * q.x - y * q.y - z * q.z
            );
        }

        Vector3 operator*(const Vector3& v) const {
            // Rotate vector by quaternion
            Vector3 qv(x, y, z);
            Vector3 t = 2.0f * qv.Cross(v);
            return v + w * t + qv.Cross(t);
        }

        Quaternion& operator*=(const Quaternion& q) {
            *this = *this * q;
            return *this;
        }

        float LengthSq() const { return x * x + y * y + z * z + w * w; }
        float Length() const { return sqrtf(LengthSq()); }

        Quaternion Normalized() const {
            float len = Length();
            if (len > 0) {
                float inv = 1.0f / len;
                return Quaternion(x * inv, y * inv, z * inv, w * inv);
            }
            return *this;
        }

        void Normalize() {
            float len = Length();
            if (len > 0) {
                float inv = 1.0f / len;
                x *= inv;
                y *= inv;
                z *= inv;
                w *= inv;
            }
        }

        Quaternion Conjugate() const { return Quaternion(-x, -y, -z, w); }
        Quaternion Inverse() const { return Conjugate() / LengthSq(); }

        Quaternion operator/(float s) const {
            float inv = 1.0f / s;
            return Quaternion(x * inv, y * inv, z * inv, w * inv);
        }

        // Axis-angle rotation
        static Quaternion FromAxisAngle(const Vector3& axis, float angle) {
            Vector3 norm = axis.Normalized();
            float halfAngle = angle * 0.5f;
            float s = sinf(halfAngle);
            return Quaternion(norm.x * s, norm.y * s, norm.z * s, cosf(halfAngle)).Normalized();
        }

        // Convert to matrix
        Matrix3 ToMatrix3() const {
            float xx = x * x, yy = y * y, zz = z * z;
            float xy = x * y, xz = x * z, yz = y * z;
            float wx = w * x, wy = w * y, wz = w * z;

            return Matrix3(
                1 - 2*(yy + zz), 2*(xy - wz),     2*(xz + wy),
                2*(xy + wz),     1 - 2*(xx + zz), 2*(yz - wx),
                2*(xz - wy),     2*(yz + wx),     1 - 2*(xx + yy)
            );
        }

        Matrix4 ToMatrix4() const {
            Matrix3 rot = ToMatrix3();
            return Matrix4(
                rot.m[0][0], rot.m[0][1], rot.m[0][2], 0,
                rot.m[1][0], rot.m[1][1], rot.m[1][2], 0,
                rot.m[2][0], rot.m[2][1], rot.m[2][2], 0,
                0, 0, 0, 1
            );
        }

        // Spherical linear interpolation
        static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t) {
            float dot = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
            if (dot < 0.0f) {
                // Use the opposite quaternion to take the shorter path
                return Slerp(a, -b, t);
            }

            if (dot > 0.9995f) {
                // If quaternions are very close, use linear interpolation
                Quaternion result = a + t * (b - a);
                result.Normalize();
                return result;
            }

            float theta = acosf(Clamp(dot, -1.0f, 1.0f));
            float sinTheta = sinf(theta);
            float invSinTheta = 1.0f / sinTheta;

            float scaleA = sinf((1.0f - t) * theta) * invSinTheta;
            float scaleB = sinf(t * theta) * invSinTheta;

            return Quaternion(
                a.x * scaleA + b.x * scaleB,
                a.y * scaleA + b.y * scaleB,
                a.z * scaleA + b.z * scaleB,
                a.w * scaleA + b.w * scaleB
            );
        }
    };

    inline Quaternion operator*(float s, const Quaternion& q) {
        return Quaternion(q.x * s, q.y * s, q.z * s, q.w * s);
    }

    inline Quaternion operator+(const Quaternion& a, const Quaternion& b) {
        return Quaternion(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
    }

    inline Quaternion operator-(const Quaternion& a, const Quaternion& b) {
        return Quaternion(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
    }

    // -----------------------------------------------------------------
    // Transform (combined translation, rotation, scale)
    // -----------------------------------------------------------------
    class Transform {
    public:
        Vector3    translation;
        Quaternion rotation;
        Vector3    scale;

        Transform() : translation(0, 0, 0), rotation(), scale(1, 1, 1) {}

        Matrix4 ToMatrix() const {
            Matrix4 rot = rotation.ToMatrix4();
            Matrix4 trans = Matrix4::Translation(translation);
            Matrix4 scl = Matrix4::Scaling(scale);
            return trans * rot * scl; // Apply scale first, then rotation, then translation
        }

        Vector3 TransformPoint(const Vector3& point) const {
            return translation + rotation * (scale * point);
        }

        Vector3 TransformVector(const Vector3& vec) const {
            return rotation * (scale * vec);
        }

        Transform Inverse() const {
            Transform inv;
            inv.rotation = rotation.Conjugate();
            inv.scale = Vector3(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z);
            inv.translation = inv.rotation * (-translation * inv.scale);
            return inv;
        }

        Transform operator*(const Transform& other) const {
            Transform result;
            result.scale = scale * other.scale;
            result.rotation = rotation * other.rotation;
            result.translation = translation + rotation * (scale * other.translation);
            return result;
        }
    };

    // -----------------------------------------------------------------
    // Axis-Aligned Bounding Box
    // -----------------------------------------------------------------
    class AABB {
    public:
        Vector3 min;
        Vector3 max;

        AABB() : min(FLT_MAX, FLT_MAX, FLT_MAX), max(-FLT_MAX, -FLT_MAX, -FLT_MAX) {}
        AABB(const Vector3& min, const Vector3& max) : min(min), max(max) {}

        void Encapsulate(const Vector3& point) {
            if (point.x < min.x) min.x = point.x;
            if (point.y < min.y) min.y = point.y;
            if (point.z < min.z) min.z = point.z;
            if (point.x > max.x) max.x = point.x;
            if (point.y > max.y) max.y = point.y;
            if (point.z > max.z) max.z = point.z;
        }

        void Encapsulate(const AABB& box) {
            Encapsulate(box.min);
            Encapsulate(box.max);
        }

        Vector3 Center() const { return (min + max) * 0.5f; }
        Vector3 Extents() const { return (max - min) * 0.5f; }
        Vector3 Size() const { return max - min; }

        bool Contains(const Vector3& point) const {
            return point.x >= min.x && point.x <= max.x &&
                   point.y >= min.y && point.y <= max.y &&
                   point.z >= min.z && point.z <= max.z;
        }

        bool Intersects(const AABB& other) const {
            return (min.x <= other.max.x && max.x >= other.min.x) &&
                   (min.y <= other.max.y && max.y >= other.min.y) &&
                   (min.z <= other.max.z && max.z >= other.min.z);
        }

        void Transform(const Matrix4& matrix) {
            Vector3 corners[8] = {
                min,
                Vector3(max.x, min.y, min.z),
                Vector3(min.x, max.y, min.z),
                Vector3(max.x, max.y, min.z),
                Vector3(min.x, min.y, max.z),
                Vector3(max.x, min.y, max.z),
                Vector3(min.x, max.y, max.z),
                max
            };

            min = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
            max = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

            for (int i = 0; i < 8; ++i) {
                Vector3 p = matrix.TransformPoint(corners[i]);
                Encapsulate(p);
            }
        }
    };

    // -----------------------------------------------------------------
    // Ray
    // -----------------------------------------------------------------
    class Ray {
    public:
        Vector3 origin;
        Vector3 direction;

        Ray() {}
        Ray(const Vector3& origin, const Vector3& direction) : origin(origin), direction(direction.Normalized()) {}

        Vector3 GetPoint(float t) const { return origin + direction * t; }

        // Distance to point
        float DistanceToPoint(const Vector3& point) const {
            Vector3 toPoint = point - origin;
            float t = toPoint.Dot(direction);
            Vector3 proj = origin + direction * t;
            return (point - proj).Length();
        }

        // Intersection with AABB
        bool Intersects(const AABB& box, float& tNear, float& tFar) const {
            Vector3 invDir = Vector3(1.0f / direction.x, 1.0f / direction.y, 1.0f / direction.z);
            Vector3 t1 = (box.min - origin) * invDir;
            Vector3 t2 = (box.max - origin) * invDir;

            float tMinX = Min(t1.x, t2.x);
            float tMaxX = Max(t1.x, t2.x);
            float tMinY = Min(t1.y, t2.y);
            float tMaxY = Max(t1.y, t2.y);
            float tMinZ = Min(t1.z, t2.z);
            float tMaxZ = Max(t1.z, t2.z);

            float tEnter = Max(Max(tMinX, tMinY), tMinZ);
            float tExit = Min(Min(tMaxX, tMaxY), tMaxZ);

            tNear = tEnter;
            tFar = tExit;

            return tEnter <= tExit && tExit >= 0;
        }

        // Intersection with plane
        bool Intersects(const class Plane& plane, float& t) const;
    };

    // -----------------------------------------------------------------
    // Plane
    // -----------------------------------------------------------------
    class Plane {
    public:
        Vector3 normal;
        float distance; // distance from origin along normal

        Plane() : normal(0, 1, 0), distance(0) {}
        Plane(const Vector3& normal, float distance) : normal(normal.Normalized()), distance(distance) {}
        Plane(const Vector3& point, const Vector3& normal) : normal(normal.Normalized()), distance(normal.Dot(point)) {}

        float GetDistance(const Vector3& point) const { return normal.Dot(point) - distance; }
        Vector3 Project(const Vector3& point) const { return point - normal * GetDistance(point); }
    };

    inline bool Ray::Intersects(const Plane& plane, float& t) const {
        float denom = direction.Dot(plane.normal);
        if (Abs(denom) < USE_FLOAT_EPSILON) return false;
        t = (plane.distance - origin.Dot(plane.normal)) / denom;
        return t >= 0;
    }

    // -----------------------------------------------------------------
    // Frustum (6 planes)
    // -----------------------------------------------------------------
    class Frustum {
    public:
        enum PlaneSide { LEFT = 0, RIGHT, BOTTOM, TOP, NEAR, FAR, COUNT };

        Plane planes[COUNT];

        void ExtractFromMatrix(const Matrix4& viewProj);

        bool ContainsPoint(const Vector3& point) const {
            for (int i = 0; i < COUNT; ++i) {
                if (planes[i].GetDistance(point) < 0) return false;
            }
            return true;
        }

        bool ContainsSphere(const Vector3& center, float radius) const {
            for (int i = 0; i < COUNT; ++i) {
                float d = planes[i].GetDistance(center);
                if (d < -radius) return false;
                if (Abs(d) < radius) return true; // intersecting
            }
            return true; // fully inside
        }

        bool ContainsAABB(const AABB& box) const {
            Vector3 corners[8] = {
                box.min,
                Vector3(box.max.x, box.min.y, box.min.z),
                Vector3(box.min.x, box.max.y, box.min.z),
                Vector3(box.max.x, box.max.y, box.min.z),
                Vector3(box.min.x, box.min.y, box.max.z),
                Vector3(box.max.x, box.min.y, box.max.z),
                Vector3(box.min.x, box.max.y, box.max.z),
                box.max
            };

            for (int i = 0; i < COUNT; ++i) {
                bool outside = true;
                for (int j = 0; j < 8; ++j) {
                    if (planes[i].GetDistance(corners[j]) >= 0) {
                        outside = false;
                        break;
                    }
                }
                if (outside) return false;
            }
            return true;
        }
    };

    inline void Frustum::ExtractFromMatrix(const Matrix4& m) {
        // Left:   m3 + m0
        planes[LEFT].normal.x = m.m[3][0] + m.m[0][0];
        planes[LEFT].normal.y = m.m[3][1] + m.m[0][1];
        planes[LEFT].normal.z = m.m[3][2] + m.m[0][2];
        planes[LEFT].distance = m.m[3][3] + m.m[0][3];
        planes[LEFT].normal.Normalize();

        // Right:  m3 - m0
        planes[RIGHT].normal.x = m.m[3][0] - m.m[0][0];
        planes[RIGHT].normal.y = m.m[3][1] - m.m[0][1];
        planes[RIGHT].normal.z = m.m[3][2] - m.m[0][2];
        planes[RIGHT].distance = m.m[3][3] - m.m[0][3];
        planes[RIGHT].normal.Normalize();

        // Bottom: m3 + m1
        planes[BOTTOM].normal.x = m.m[3][0] + m.m[1][0];
        planes[BOTTOM].normal.y = m.m[3][1] + m.m[1][1];
        planes[BOTTOM].normal.z = m.m[3][2] + m.m[1][2];
        planes[BOTTOM].distance = m.m[3][3] + m.m[1][3];
        planes[BOTTOM].normal.Normalize();

        // Top:    m3 - m1
        planes[TOP].normal.x = m.m[3][0] - m.m[1][0];
        planes[TOP].normal.y = m.m[3][1] - m.m[1][1];
        planes[TOP].normal.z = m.m[3][2] - m.m[1][2];
        planes[TOP].distance = m.m[3][3] - m.m[1][3];
        planes[TOP].normal.Normalize();

        // Near:   m3 + m2
        planes[NEAR].normal.x = m.m[3][0] + m.m[2][0];
        planes[NEAR].normal.y = m.m[3][1] + m.m[2][1];
        planes[NEAR].normal.z = m.m[3][2] + m.m[2][2];
        planes[NEAR].distance = m.m[3][3] + m.m[2][3];
        planes[NEAR].normal.Normalize();

        // Far:    m3 - m2
        planes[FAR].normal.x = m.m[3][0] - m.m[2][0];
        planes[FAR].normal.y = m.m[3][1] - m.m[2][1];
        planes[FAR].normal.z = m.m[3][2] - m.m[2][2];
        planes[FAR].distance = m.m[3][3] - m.m[2][3];
        planes[FAR].normal.Normalize();
    }

    // -----------------------------------------------------------------
    // Random number generation (simple)
    // -----------------------------------------------------------------
    class Random {
    public:
        static void Seed(unsigned int seed) { srand(seed); }
        static float Range(float min, float max) {
            return min + (max - min) * (rand() / (float)RAND_MAX);
        }
        static int Range(int min, int max) {
            return min + rand() % (max - min + 1);
        }
        static float Value() { return rand() / (float)RAND_MAX; }
    };

    // -----------------------------------------------------------------
    // Static vector constants
    // -----------------------------------------------------------------
    const Vector2 Vector2::Zero(0, 0);
    const Vector2 Vector2::One(1, 1);
    const Vector2 Vector2::Right(1, 0);
    const Vector2 Vector2::Left(-1, 0);
    const Vector2 Vector2::Up(0, 1);
    const Vector2 Vector2::Down(0, -1);

    const Vector3 Vector3::Zero(0, 0, 0);
    const Vector3 Vector3::One(1, 1, 1);
    const Vector3 Vector3::Right(1, 0, 0);
    const Vector3 Vector3::Left(-1, 0, 0);
    const Vector3 Vector3::Up(0, 1, 0);
    const Vector3 Vector3::Down(0, -1, 0);
    const Vector3 Vector3::Forward(0, 0, 1);
    const Vector3 Vector3::Back(0, 0, -1);

    const Vector4 Vector4::Zero(0, 0, 0, 0);
    const Vector4 Vector4::One(1, 1, 1, 1);

} // namespace USE