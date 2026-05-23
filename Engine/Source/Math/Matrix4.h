// ============================================================
// Ultimate Source Engine - Matrix4
// ============================================================
//
// 4x4 matrix class for 3D transformations (translation, rotation, scale,
// projection). Column-major storage for compatibility with OpenGL.
// All functions are inline for performance.
// ============================================================

#pragma once

#include <cmath>
#include <cstring>
#include "Vector3.h"
#include "Vector4.h"

namespace USE {

    class Matrix4 {
    public:
        // Column-major order: m[column][row]
        float m[4][4];

        // Constructors
        inline Matrix4();
        inline Matrix4(float m00, float m01, float m02, float m03,
                       float m10, float m11, float m12, float m13,
                       float m20, float m21, float m22, float m23,
                       float m30, float m31, float m32, float m33);
        inline explicit Matrix4(const float* data); // data in column-major order

        // Identity matrix
        static inline Matrix4 Identity();

        // Zero matrix
        static inline Matrix4 Zero();

        // Access elements (column, row)
        inline float& operator()(int col, int row);
        inline float operator()(int col, int row) const;

        // Matrix multiplication
        inline Matrix4 operator*(const Matrix4& b) const;
        inline Matrix4& operator*=(const Matrix4& b);

        // Vector multiplication
        inline Vector4 operator*(const Vector4& v) const;

        // Transform a point (w=1) and a vector (w=0)
        inline Vector3 TransformPoint(const Vector3& p) const;
        inline Vector3 TransformVector(const Vector3& v) const;

        // Addition, subtraction (element-wise)
        inline Matrix4 operator+(const Matrix4& b) const;
        inline Matrix4 operator-(const Matrix4& b) const;
        inline Matrix4& operator+=(const Matrix4& b);
        inline Matrix4& operator-=(const Matrix4& b);

        // Scalar multiplication
        inline Matrix4 operator*(float s) const;
        inline Matrix4& operator*=(float s);
        inline Matrix4 operator/(float s) const;
        inline Matrix4& operator/=(float s);

        // Comparison
        inline bool operator==(const Matrix4& b) const;
        inline bool operator!=(const Matrix4& b) const;

        // Transpose
        inline Matrix4 Transposed() const;
        inline void Transpose();

        // Determinant
        inline float Determinant() const;

        // Inverse (returns identity if singular)
        inline Matrix4 Inverse() const;

        // Translation matrices
        static inline Matrix4 Translation(const Vector3& t);
        static inline Matrix4 Translation(float x, float y, float z);

        // Scale matrices
        static inline Matrix4 Scaling(const Vector3& s);
        static inline Matrix4 Scaling(float sx, float sy, float sz);
        static inline Matrix4 Scaling(float s); // uniform

        // Rotation matrices (angle in radians)
        static inline Matrix4 RotationX(float angle);
        static inline Matrix4 RotationY(float angle);
        static inline Matrix4 RotationZ(float angle);

        // Rotation from axis-angle
        static inline Matrix4 RotationAxis(const Vector3& axis, float angle);

        // LookAt matrix (view matrix)
        static inline Matrix4 LookAt(const Vector3& eye, const Vector3& target, const Vector3& up);

        // Perspective projection matrix
        static inline Matrix4 Perspective(float fovY, float aspect, float nearZ, float farZ);

        // Orthographic projection matrix
        static inline Matrix4 Orthographic(float left, float right, float bottom, float top, float nearZ, float farZ);

        // Frustum projection (custom)
        static inline Matrix4 Frustum(float left, float right, float bottom, float top, float nearZ, float farZ);
    };

    // -----------------------------------------------------------------
    // Inline implementations
    // -----------------------------------------------------------------

    Matrix4::Matrix4() {
        // Default to identity
        m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
        m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = 0;
        m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0;
        m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
    }

    Matrix4::Matrix4(float m00, float m01, float m02, float m03,
                     float m10, float m11, float m12, float m13,
                     float m20, float m21, float m22, float m23,
                     float m30, float m31, float m32, float m33) {
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
        m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
    }

    Matrix4::Matrix4(const float* data) {
        memcpy(m, data, 16 * sizeof(float));
    }

    Matrix4 Matrix4::Identity() {
        return Matrix4();
    }

    Matrix4 Matrix4::Zero() {
        return Matrix4(
            0,0,0,0,
            0,0,0,0,
            0,0,0,0,
            0,0,0,0
        );
    }

    float& Matrix4::operator()(int col, int row) {
        return m[col][row];
    }

    float Matrix4::operator()(int col, int row) const {
        return m[col][row];
    }

    Matrix4 Matrix4::operator*(const Matrix4& b) const {
        Matrix4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = m[i][0] * b.m[0][j] +
                                 m[i][1] * b.m[1][j] +
                                 m[i][2] * b.m[2][j] +
                                 m[i][3] * b.m[3][j];
            }
        }
        return result;
    }

    Matrix4& Matrix4::operator*=(const Matrix4& b) {
        *this = *this * b;
        return *this;
    }

    Vector4 Matrix4::operator*(const Vector4& v) const {
        return Vector4(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
            m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
        );
    }

    Vector3 Matrix4::TransformPoint(const Vector3& p) const {
        float w = m[3][0] * p.x + m[3][1] * p.y + m[3][2] * p.z + m[3][3];
        if (w != 0.0f) {
            float invW = 1.0f / w;
            return Vector3(
                (m[0][0] * p.x + m[0][1] * p.y + m[0][2] * p.z + m[0][3]) * invW,
                (m[1][0] * p.x + m[1][1] * p.y + m[1][2] * p.z + m[1][3]) * invW,
                (m[2][0] * p.x + m[2][1] * p.y + m[2][2] * p.z + m[2][3]) * invW
            );
        }
        return Vector3::Zero;
    }

    Vector3 Matrix4::TransformVector(const Vector3& v) const {
        return Vector3(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z
        );
    }

    Matrix4 Matrix4::operator+(const Matrix4& b) const {
        Matrix4 result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                result.m[i][j] = m[i][j] + b.m[i][j];
        return result;
    }

    Matrix4 Matrix4::operator-(const Matrix4& b) const {
        Matrix4 result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                result.m[i][j] = m[i][j] - b.m[i][j];
        return result;
    }

    Matrix4& Matrix4::operator+=(const Matrix4& b) {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] += b.m[i][j];
        return *this;
    }

    Matrix4& Matrix4::operator-=(const Matrix4& b) {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] -= b.m[i][j];
        return *this;
    }

    Matrix4 Matrix4::operator*(float s) const {
        Matrix4 result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                result.m[i][j] = m[i][j] * s;
        return result;
    }

    Matrix4& Matrix4::operator*=(float s) {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] *= s;
        return *this;
    }

    Matrix4 Matrix4::operator/(float s) const {
        float inv = 1.0f / s;
        return *this * inv;
    }

    Matrix4& Matrix4::operator/=(float s) {
        float inv = 1.0f / s;
        *this *= inv;
        return *this;
    }

    bool Matrix4::operator==(const Matrix4& b) const {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                if (m[i][j] != b.m[i][j])
                    return false;
        return true;
    }

    bool Matrix4::operator!=(const Matrix4& b) const {
        return !(*this == b);
    }

    Matrix4 Matrix4::Transposed() const {
        Matrix4 result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                result.m[i][j] = m[j][i];
        return result;
    }

    void Matrix4::Transpose() {
        for (int i = 0; i < 4; ++i)
            for (int j = i + 1; j < 4; ++j) {
                float tmp = m[i][j];
                m[i][j] = m[j][i];
                m[j][i] = tmp;
            }
    }

    float Matrix4::Determinant() const {
        float a0 = m[0][0] * m[1][1] - m[0][1] * m[1][0];
        float a1 = m[0][0] * m[1][2] - m[0][2] * m[1][0];
        float a2 = m[0][0] * m[1][3] - m[0][3] * m[1][0];
        float a3 = m[0][1] * m[1][2] - m[0][2] * m[1][1];
        float a4 = m[0][1] * m[1][3] - m[0][3] * m[1][1];
        float a5 = m[0][2] * m[1][3] - m[0][3] * m[1][2];

        float b0 = m[2][0] * m[3][1] - m[2][1] * m[3][0];
        float b1 = m[2][0] * m[3][2] - m[2][2] * m[3][0];
        float b2 = m[2][0] * m[3][3] - m[2][3] * m[3][0];
        float b3 = m[2][1] * m[3][2] - m[2][2] * m[3][1];
        float b4 = m[2][1] * m[3][3] - m[2][3] * m[3][1];
        float b5 = m[2][2] * m[3][3] - m[2][3] * m[3][2];

        return a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;
    }

    Matrix4 Matrix4::Inverse() const {
        float det = Determinant();
        if (std::abs(det) < 1e-12f) {
            return Identity(); // Singular matrix
        }

        float invDet = 1.0f / det;

        Matrix4 inv;

        inv.m[0][0] = (m[1][1] * m[2][2] * m[3][3] + m[1][2] * m[2][3] * m[3][1] + m[1][3] * m[2][1] * m[3][2] -
                       m[1][1] * m[2][3] * m[3][2] - m[1][2] * m[2][1] * m[3][3] - m[1][3] * m[2][2] * m[3][1]) * invDet;
        inv.m[0][1] = (m[0][1] * m[2][3] * m[3][2] + m[0][2] * m[2][1] * m[3][3] + m[0][3] * m[2][2] * m[3][1] -
                       m[0][1] * m[2][2] * m[3][3] - m[0][2] * m[2][3] * m[3][1] - m[0][3] * m[2][1] * m[3][2]) * invDet;
        inv.m[0][2] = (m[0][1] * m[1][2] * m[3][3] + m[0][2] * m[1][3] * m[3][1] + m[0][3] * m[1][1] * m[3][2] -
                       m[0][1] * m[1][3] * m[3][2] - m[0][2] * m[1][1] * m[3][3] - m[0][3] * m[1][2] * m[3][1]) * invDet;
        inv.m[0][3] = (m[0][1] * m[1][3] * m[2][2] + m[0][2] * m[1][1] * m[2][3] + m[0][3] * m[1][2] * m[2][1] -
                       m[0][1] * m[1][2] * m[2][3] - m[0][2] * m[1][3] * m[2][1] - m[0][3] * m[1][1] * m[2][2]) * invDet;

        inv.m[1][0] = (m[1][0] * m[2][3] * m[3][2] + m[1][2] * m[2][0] * m[3][3] + m[1][3] * m[2][2] * m[3][0] -
                       m[1][0] * m[2][2] * m[3][3] - m[1][2] * m[2][3] * m[3][0] - m[1][3] * m[2][0] * m[3][2]) * invDet;
        inv.m[1][1] = (m[0][0] * m[2][2] * m[3][3] + m[0][2] * m[2][3] * m[3][0] + m[0][3] * m[2][0] * m[3][2] -
                       m[0][0] * m[2][3] * m[3][2] - m[0][2] * m[2][0] * m[3][3] - m[0][3] * m[2][2] * m[3][0]) * invDet;
        inv.m[1][2] = (m[0][0] * m[1][3] * m[3][2] + m[0][2] * m[1][0] * m[3][3] + m[0][3] * m[1][2] * m[3][0] -
                       m[0][0] * m[1][2] * m[3][3] - m[0][2] * m[1][3] * m[3][0] - m[0][3] * m[1][0] * m[3][2]) * invDet;
        inv.m[1][3] = (m[0][0] * m[1][2] * m[2][3] + m[0][2] * m[1][3] * m[2][0] + m[0][3] * m[1][0] * m[2][2] -
                       m[0][0] * m[1][3] * m[2][2] - m[0][2] * m[1][0] * m[2][3] - m[0][3] * m[1][2] * m[2][0]) * invDet;

        inv.m[2][0] = (m[1][0] * m[2][1] * m[3][3] + m[1][1] * m[2][3] * m[3][0] + m[1][3] * m[2][0] * m[3][1] -
                       m[1][0] * m[2][3] * m[3][1] - m[1][1] * m[2][0] * m[3][3] - m[1][3] * m[2][1] * m[3][0]) * invDet;
        inv.m[2][1] = (m[0][0] * m[2][3] * m[3][1] + m[0][1] * m[2][0] * m[3][3] + m[0][3] * m[2][1] * m[3][0] -
                       m[0][0] * m[2][1] * m[3][3] - m[0][1] * m[2][3] * m[3][0] - m[0][3] * m[2][0] * m[3][1]) * invDet;
        inv.m[2][2] = (m[0][0] * m[1][1] * m[3][3] + m[0][1] * m[1][3] * m[3][0] + m[0][3] * m[1][0] * m[3][1] -
                       m[0][0] * m[1][3] * m[3][1] - m[0][1] * m[1][0] * m[3][3] - m[0][3] * m[1][1] * m[3][0]) * invDet;
        inv.m[2][3] = (m[0][0] * m[1][3] * m[2][1] + m[0][1] * m[1][0] * m[2][3] + m[0][3] * m[1][1] * m[2][0] -
                       m[0][0] * m[1][1] * m[2][3] - m[0][1] * m[1][3] * m[2][0] - m[0][3] * m[1][0] * m[2][1]) * invDet;

        inv.m[3][0] = (m[1][0] * m[2][2] * m[3][1] + m[1][1] * m[2][0] * m[3][2] + m[1][2] * m[2][1] * m[3][0] -
                       m[1][0] * m[2][1] * m[3][2] - m[1][1] * m[2][2] * m[3][0] - m[1][2] * m[2][0] * m[3][1]) * invDet;
        inv.m[3][1] = (m[0][0] * m[2][1] * m[3][2] + m[0][1] * m[2][2] * m[3][0] + m[0][2] * m[2][0] * m[3][1] -
                       m[0][0] * m[2][2] * m[3][1] - m[0][1] * m[2][0] * m[3][2] - m[0][2] * m[2][1] * m[3][0]) * invDet;
        inv.m[3][2] = (m[0][0] * m[1][2] * m[3][1] + m[0][1] * m[1][0] * m[3][2] + m[0][2] * m[1][1] * m[3][0] -
                       m[0][0] * m[1][1] * m[3][2] - m[0][1] * m[1][2] * m[3][0] - m[0][2] * m[1][0] * m[3][1]) * invDet;
        inv.m[3][3] = (m[0][0] * m[1][1] * m[2][2] + m[0][1] * m[1][2] * m[2][0] + m[0][2] * m[1][0] * m[2][1] -
                       m[0][0] * m[1][2] * m[2][1] - m[0][1] * m[1][0] * m[2][2] - m[0][2] * m[1][1] * m[2][0]) * invDet;

        return inv;
    }

    Matrix4 Matrix4::Translation(const Vector3& t) {
        return Matrix4(
            1, 0, 0, t.x,
            0, 1, 0, t.y,
            0, 0, 1, t.z,
            0, 0, 0, 1
        );
    }

    Matrix4 Matrix4::Translation(float x, float y, float z) {
        return Translation(Vector3(x, y, z));
    }

    Matrix4 Matrix4::Scaling(const Vector3& s) {
        return Matrix4(
            s.x, 0,   0,   0,
            0,   s.y, 0,   0,
            0,   0,   s.z, 0,
            0,   0,   0,   1
        );
    }

    Matrix4 Matrix4::Scaling(float sx, float sy, float sz) {
        return Scaling(Vector3(sx, sy, sz));
    }

    Matrix4 Matrix4::Scaling(float s) {
        return Scaling(Vector3(s, s, s));
    }

    Matrix4 Matrix4::RotationX(float angle) {
        float c = cosf(angle);
        float s = sinf(angle);
        return Matrix4(
            1, 0,  0, 0,
            0, c, -s, 0,
            0, s,  c, 0,
            0, 0,  0, 1
        );
    }

    Matrix4 Matrix4::RotationY(float angle) {
        float c = cosf(angle);
        float s = sinf(angle);
        return Matrix4(
            c, 0, s, 0,
            0, 1, 0, 0,
            -s,0, c, 0,
            0, 0, 0, 1
        );
    }

    Matrix4 Matrix4::RotationZ(float angle) {
        float c = cosf(angle);
        float s = sinf(angle);
        return Matrix4(
            c, -s, 0, 0,
            s,  c, 0, 0,
            0,  0, 1, 0,
            0,  0, 0, 1
        );
    }

    Matrix4 Matrix4::RotationAxis(const Vector3& axis, float angle) {
        Vector3 a = axis.Normalized();
        float c = cosf(angle);
        float s = sinf(angle);
        float t = 1.0f - c;

        float x = a.x;
        float y = a.y;
        float z = a.z;

        return Matrix4(
            t*x*x + c,   t*x*y - s*z, t*x*z + s*y, 0,
            t*x*y + s*z, t*y*y + c,   t*y*z - s*x, 0,
            t*x*z - s*y, t*y*z + s*x, t*z*z + c,   0,
            0,           0,           0,           1
        );
    }

    Matrix4 Matrix4::LookAt(const Vector3& eye, const Vector3& target, const Vector3& up) {
        Vector3 z = (eye - target).Normalized(); // forward
        Vector3 x = up.Cross(z).Normalized();    // right
        Vector3 y = z.Cross(x).Normalized();     // up (re-orthogonalized)

        return Matrix4(
            x.x, x.y, x.z, -x.Dot(eye),
            y.x, y.y, y.z, -y.Dot(eye),
            z.x, z.y, z.z, -z.Dot(eye),
            0,   0,   0,   1
        );
    }

    Matrix4 Matrix4::Perspective(float fovY, float aspect, float nearZ, float farZ) {
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

    Matrix4 Matrix4::Orthographic(float left, float right, float bottom, float top, float nearZ, float farZ) {
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

    Matrix4 Matrix4::Frustum(float left, float right, float bottom, float top, float nearZ, float farZ) {
        float invWidth = 1.0f / (right - left);
        float invHeight = 1.0f / (top - bottom);
        float invDepth = 1.0f / (farZ - nearZ);

        float A = (right + left) * invWidth;
        float B = (top + bottom) * invHeight;
        float C = -(farZ + nearZ) * invDepth;
        float D = -2.0f * farZ * nearZ * invDepth;

        return Matrix4(
            2.0f * nearZ * invWidth, 0,          A,   0,
            0,                        2.0f * nearZ * invHeight, B, 0,
            0,                        0,          C,   D,
            0,                        0,          -1,  0
        );
    }

} // namespace USE