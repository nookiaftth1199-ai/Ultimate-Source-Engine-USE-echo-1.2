// ============================================================
// Ultimate Source Engine - Vector3
// ============================================================
//
// 3D vector class with common operations.
// All functions are inline for performance.
// ============================================================

#pragma once

#include <cmath>
#include <algorithm>

namespace USE {

    class Vector3 {
    public:
        float x, y, z;

        // Constructors
        inline Vector3();
        inline explicit Vector3(float s);
        inline Vector3(float x, float y, float z);

        // Unary operators
        inline Vector3 operator-() const;

        // Binary operators
        inline Vector3 operator+(const Vector3& v) const;
        inline Vector3 operator-(const Vector3& v) const;
        inline Vector3 operator*(float s) const;
        inline Vector3 operator/(float s) const;

        // Compound assignment
        inline Vector3& operator+=(const Vector3& v);
        inline Vector3& operator-=(const Vector3& v);
        inline Vector3& operator*=(float s);
        inline Vector3& operator/=(float s);

        // Comparison
        inline bool operator==(const Vector3& v) const;
        inline bool operator!=(const Vector3& v) const;

        // Length
        inline float LengthSq() const;
        inline float Length() const;

        // Normalization
        inline Vector3 Normalized() const;
        inline void Normalize();

        // Dot product
        inline float Dot(const Vector3& v) const;

        // Cross product
        inline Vector3 Cross(const Vector3& v) const;

        // Static utility functions
        static inline float Distance(const Vector3& a, const Vector3& b);
        static inline float DistanceSq(const Vector3& a, const Vector3& b);
        static inline Vector3 Lerp(const Vector3& a, const Vector3& b, float t);

        // Constants (defined in .cpp)
        static const Vector3 Zero;
        static const Vector3 One;
        static const Vector3 Right;   // (1,0,0)
        static const Vector3 Left;    // (-1,0,0)
        static const Vector3 Up;      // (0,1,0)
        static const Vector3 Down;    // (0,-1,0)
        static const Vector3 Forward; // (0,0,1)
        static const Vector3 Back;    // (0,0,-1)
    };

    // Scalar multiplication (s * v)
    inline Vector3 operator*(float s, const Vector3& v);

    // -----------------------------------------------------------------
    // Inline implementations
    // -----------------------------------------------------------------

    Vector3::Vector3() : x(0), y(0), z(0) {}
    Vector3::Vector3(float s) : x(s), y(s), z(s) {}
    Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector3 Vector3::operator-() const { return Vector3(-x, -y, -z); }

    Vector3 Vector3::operator+(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
    Vector3 Vector3::operator-(const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
    Vector3 Vector3::operator*(float s) const { return Vector3(x * s, y * s, z * s); }
    Vector3 Vector3::operator/(float s) const { float inv = 1.0f / s; return Vector3(x * inv, y * inv, z * inv); }

    Vector3& Vector3::operator+=(const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    Vector3& Vector3::operator-=(const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    Vector3& Vector3::operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
    Vector3& Vector3::operator/=(float s) { float inv = 1.0f / s; x *= inv; y *= inv; z *= inv; return *this; }

    bool Vector3::operator==(const Vector3& v) const { return x == v.x && y == v.y && z == v.z; }
    bool Vector3::operator!=(const Vector3& v) const { return x != v.x || y != v.y || z != v.z; }

    float Vector3::LengthSq() const { return x * x + y * y + z * z; }
    float Vector3::Length() const { return std::sqrt(LengthSq()); }

    Vector3 Vector3::Normalized() const {
        float len = Length();
        if (len > 0) {
            float inv = 1.0f / len;
            return Vector3(x * inv, y * inv, z * inv);
        }
        return *this;
    }

    void Vector3::Normalize() {
        float len = Length();
        if (len > 0) {
            float inv = 1.0f / len;
            x *= inv;
            y *= inv;
            z *= inv;
        }
    }

    float Vector3::Dot(const Vector3& v) const { return x * v.x + y * v.y + z * v.z; }

    Vector3 Vector3::Cross(const Vector3& v) const {
        return Vector3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }

    float Vector3::Distance(const Vector3& a, const Vector3& b) { return (a - b).Length(); }
    float Vector3::DistanceSq(const Vector3& a, const Vector3& b) { return (a - b).LengthSq(); }

    Vector3 Vector3::Lerp(const Vector3& a, const Vector3& b, float t) {
        return a + t * (b - a);
    }

    Vector3 operator*(float s, const Vector3& v) { return v * s; }

} // namespace USE