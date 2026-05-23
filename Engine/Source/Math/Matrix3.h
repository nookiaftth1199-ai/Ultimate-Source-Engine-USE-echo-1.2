// ============================================================
// Ultimate Source Engine - Matrix3
// ============================================================
//
// 3x3 matrix class for 3D transformations (rotation, scale).
// Column-major storage for compatibility with OpenGL.
// All functions are inline for performance.
// ============================================================

#pragma once

#include <cmath>
#include <cstring>

namespace USE {

    class Matrix3 {
    public:
        // Column-major order: m[column][row]
        float m[3][3];

        // Constructors
        inline Matrix3();
        inline Matrix3(float m00, float m01, float m02,
                       float m10, float m11, float m12,
                       float m20, float m21, float m22);

        // Identity matrix
        static inline Matrix3 Identity();

        // Matrix multiplication
        inline Matrix3 operator*(const Matrix3& b) const;

        // Vector multiplication (transform vector)
        inline class Vector3 operator*(const class Vector3& v) const;

        // Transpose
        inline Matrix3 Transposed() const;
        inline void Transpose();

        // Determinant
        inline float Determinant() const;

        // Inverse (returns identity if singular)
        inline Matrix3 Inverse() const;

        // Rotation matrices (angle in radians)
        static inline Matrix3 RotationX(float angle);
        static inline Matrix3 RotationY(float angle);
        static inline Matrix3 RotationZ(float angle);

        // Scale matrix
        static inline Matrix3 Scaling(float sx, float sy, float sz);
        static inline Matrix3 Scaling(const class Vector3& scale);

        // Convert to 4x4 matrix (for compatibility)
        inline class Matrix4 ToMatrix4() const;
    };

    // -----------------------------------------------------------------
    // Inline implementations
    // -----------------------------------------------------------------

    Matrix3::Matrix3() {
        m[0][0] = 1; m[0][1] = 0; m[0][2] = 0;
        m[1][0] = 0; m[1][1] = 1; m[1][2] = 0;
        m[2][0] = 0; m[2][1] = 0; m[2][2] = 1;
    }

    Matrix3::Matrix3(float m00, float m01, float m02,
                     float m10, float m11, float m12,
                     float m20, float m21, float m22) {
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22;
    }

    Matrix3 Matrix3::Identity() {
        return Matrix3();
    }

    Matrix3 Matrix3::operator*(const Matrix3& b) const {
        Matrix3 result;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                result.m[i][j] = m[i][0] * b.m[0][j] +
                                 m[i][1] * b.m[1][j] +
                                 m[i][2] * b.m[2][j];
            }
        }
        return result;
    }

    Vector3 Matrix3::operator*(const Vector3& v) const {
        return Vector3(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z
        );
    }

    Matrix3 Matrix3::Transposed() const {
        Matrix3 result;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                result.m[i][j] = m[j][i];
        return result;
    }

    void Matrix3::Transpose() {
        float tmp;
        for (int i = 0; i < 3; ++i) {
            for (int j = i + 1; j < 3; ++j) {
                tmp = m[i][j];
                m[i][j] = m[j][i];
                m[j][i] = tmp;
            }
        }
    }

    float Matrix3::Determinant() const {
        return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1])
             - m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])
             + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    }

    Matrix3 Matrix3::Inverse() const {
        float det = Determinant();
        if (std::abs(det) < 1e-12f) {
            return Identity(); // Singular matrix
        }

        float invDet = 1.0f / det;
        Matrix3 inv;

        inv.m[0][0] =  (m[1][1] * m[2][2] - m[1][2] * m[2][1]) * invDet;
        inv.m[0][1] = -(m[0][1] * m[2][2] - m[0][2] * m[2][1]) * invDet;
        inv.m[0][2] =  (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invDet;

        inv.m[1][0] = -(m[1][0] * m[2][2] - m[1][2] * m[2][0]) * invDet;
        inv.m[1][1] =  (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invDet;
        inv.m[1][2] = -(m[0][0] * m[1][2] - m[0][2] * m[1][0]) * invDet;

        inv.m[2][0] =  (m[1][0] * m[2][1] - m[1][1] * m[2][0]) * invDet;
        inv.m[2][1] = -(m[0][0] * m[2][1] - m[0][1] * m[2][0]) * invDet;
        inv.m[2][2] =  (m[0][0] * m[1][1] - m[0][1] * m[1][0]) * invDet;

        return inv;
    }

    Matrix3 Matrix3::RotationX(float angle) {
        float c = cosf(angle);
        float s = sinf(angle);
        return Matrix3(
            1, 0, 0,
            0, c, -s,
            0, s,  c
        );
    }

    Matrix3 Matrix3::RotationY(float angle) {
        float c = cosf(angle);
        float s = sinf(angle);
        return Matrix3(
            c, 0, s,
            0, 1, 0,
            -s, 0, c
        );
    }

    Matrix3 Matrix3::RotationZ(float angle) {
        float c = cosf(angle);
        float s = sinf(angle);
        return Matrix3(
            c, -s, 0,
            s,  c, 0,
            0,  0, 1
        );
    }

    Matrix3 Matrix3::Scaling(float sx, float sy, float sz) {
        return Matrix3(
            sx, 0,  0,
            0,  sy, 0,
            0,  0,  sz
        );
    }

    Matrix3 Matrix3::Scaling(const Vector3& scale) {
        return Scaling(scale.x, scale.y, scale.z);
    }

    // ToMatrix4 would require Matrix4 definition, so we'll include that later
    // For now, just declare (Matrix4 will be defined elsewhere)
    // We'll leave ToMatrix4 implementation in the cpp if needed, but here we'll keep it simple.

} // namespace USE