// ============================================================
// Ultimate Source Engine - Vector4
// ============================================================
//
// 4D vector class with common operations.
// All functions are inline for performance.
// ============================================================

#pragma once

#include <cmath>
#include <algorithm>

namespace USE {

    class Vector4 {
    public:
        float x, y, z, w;

        // Constructors
        inline Vector4();
        inline explicit Vector4(float s);
        inline Vector4(float x, float y, float z, float w);

        // Unary operators
        inline Vector4 operator-() const;

        // Binary operators
        inline Vector4 operator+(const Vector4& v) const;
        inline Vector4 operator-(const Vector4& v) const;
        inline Vector4 operator*(float s) const;
        inline Vector4 operator/(float s) const;

        // Compound assignment
        inline Vector4& operator+=(const Vector4& v);
        inline Vector4& operator-=(const Vector4& v);
        inline Vector4& operator*=(float s);
        inline Vector4& operator/=(float s);

        // Comparison
        inline bool operator==(const Vector4& v) const;
        inline bool operator!=(const Vector4& v) const;

        // Length
        inline float LengthSq() const;
        inline float Length() const;

        // Normalization
        inline Vector4 Normalized() const;
        inline void Normalize();

        // Dot product
        inline float Dot(const Vector4& v) const;

        // Static utility functions
        static inline Vector4 Lerp(const Vector4& a, const Vector4& b, float t);

        // Constants (defined in .cpp)
        static const Vector4 Zero;
        static const Vector4 One;
    };

    // Scalar multiplication (s * v)
    inline Vector4 operator*(float s, const Vector4& v);

    // -----------------------------------------------------------------
    // Inline implementations
    // -----------------------------------------------------------------

    Vector4::Vector4() : x(0), y(0), z(0), w(0) {}
    Vector4::Vector4(float s) : x(s), y(s), z(s), w(s) {}
    Vector4::Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    Vector4 Vector4::operator-() const { return Vector4(-x, -y, -z, -w); }

    Vector4 Vector4::operator+(const Vector4& v) const { return Vector4(x + v.x, y + v.y, z + v.z, w + v.w); }
    Vector4 Vector4::operator-(const Vector4& v) const { return Vector4(x - v.x, y - v.y, z - v.z, w - v.w); }
    Vector4 Vector4::operator*(float s) const { return Vector4(x * s, y * s, z * s, w * s); }
    Vector4 Vector4::operator/(float s) const { float inv = 1.0f / s; return Vector4(x * inv, y * inv, z * inv, w * inv); }

    Vector4& Vector4::operator+=(const Vector4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
    Vector4& Vector4::operator-=(const Vector4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
    Vector4& Vector4::operator*=(float s) { x *= s; y *= s; z *= s; w *= s; return *this; }
    Vector4& Vector4::operator/=(float s) { float inv = 1.0f / s; x *= inv; y *= inv; z *= inv; w *= inv; return *this; }

    bool Vector4::operator==(const Vector4& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
    bool Vector4::operator!=(const Vector4& v) const { return x != v.x || y != v.y || z != v.z || w != v.w; }

    float Vector4::LengthSq() const { return x * x + y * y + z * z + w * w; }
    float Vector4::Length() const { return std::sqrt(LengthSq()); }

    Vector4 Vector4::Normalized() const {
        float len = Length();
        if (len > 0) {
            float inv = 1.0f / len;
            return Vector4(x * inv, y * inv, z * inv, w * inv);
        }
        return *this;
    }

    void Vector4::Normalize() {
        float len = Length();
        if (len > 0) {
            float inv = 1.0f / len;
            x *= inv;
            y *= inv;
            z *= inv;
            w *= inv;
        }
    }

    float Vector4::Dot(const Vector4& v) const { return x * v.x + y * v.y + z * v.z + w * v.w; }

    Vector4 Vector4::Lerp(const Vector4& a, const Vector4& b, float t) {
        return a + t * (b - a);
    }

    Vector4 operator*(float s, const Vector4& v) { return v * s; }

} // namespace USE