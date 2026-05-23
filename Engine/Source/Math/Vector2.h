// ============================================================
// Ultimate Source Engine - Vector2
// ============================================================
//
// 2D vector class with common operations.
// All functions are inline for performance.
// ============================================================

#pragma once

#include <cmath>
#include <algorithm>

namespace USE {

    class Vector2 {
    public:
        float x, y;

        // Constructors
        inline Vector2();
        inline explicit Vector2(float s);
        inline Vector2(float x, float y);

        // Unary operators
        inline Vector2 operator-() const;

        // Binary operators
        inline Vector2 operator+(const Vector2& v) const;
        inline Vector2 operator-(const Vector2& v) const;
        inline Vector2 operator*(float s) const;
        inline Vector2 operator/(float s) const;

        // Compound assignment
        inline Vector2& operator+=(const Vector2& v);
        inline Vector2& operator-=(const Vector2& v);
        inline Vector2& operator*=(float s);
        inline Vector2& operator/=(float s);

        // Comparison
        inline bool operator==(const Vector2& v) const;
        inline bool operator!=(const Vector2& v) const;

        // Length
        inline float LengthSq() const;
        inline float Length() const;

        // Normalization
        inline Vector2 Normalized() const;
        inline void Normalize();

        // Dot product
        inline float Dot(const Vector2& v) const;

        // Cross product (returns scalar, the "perp dot product")
        inline float Cross(const Vector2& v) const;

        // Static utility functions
        static inline float Distance(const Vector2& a, const Vector2& b);
        static inline float DistanceSq(const Vector2& a, const Vector2& b);
        static inline Vector2 Lerp(const Vector2& a, const Vector2& b, float t);

        // Constants (defined in .cpp)
        static const Vector2 Zero;
        static const Vector2 One;
        static const Vector2 Right;  // (1,0)
        static const Vector2 Left;   // (-1,0)
        static const Vector2 Up;      // (0,1)
        static const Vector2 Down;    // (0,-1)
    };

    // Scalar multiplication (s * v)
    inline Vector2 operator*(float s, const Vector2& v);

    // -----------------------------------------------------------------
    // Inline implementations
    // -----------------------------------------------------------------

    Vector2::Vector2() : x(0), y(0) {}
    Vector2::Vector2(float s) : x(s), y(s) {}
    Vector2::Vector2(float x, float y) : x(x), y(y) {}

    Vector2 Vector2::operator-() const { return Vector2(-x, -y); }

    Vector2 Vector2::operator+(const Vector2& v) const { return Vector2(x + v.x, y + v.y); }
    Vector2 Vector2::operator-(const Vector2& v) const { return Vector2(x - v.x, y - v.y); }
    Vector2 Vector2::operator*(float s) const { return Vector2(x * s, y * s); }
    Vector2 Vector2::operator/(float s) const { float inv = 1.0f / s; return Vector2(x * inv, y * inv); }

    Vector2& Vector2::operator+=(const Vector2& v) { x += v.x; y += v.y; return *this; }
    Vector2& Vector2::operator-=(const Vector2& v) { x -= v.x; y -= v.y; return *this; }
    Vector2& Vector2::operator*=(float s) { x *= s; y *= s; return *this; }
    Vector2& Vector2::operator/=(float s) { float inv = 1.0f / s; x *= inv; y *= inv; return *this; }

    bool Vector2::operator==(const Vector2& v) const { return x == v.x && y == v.y; }
    bool Vector2::operator!=(const Vector2& v) const { return x != v.x || y != v.y; }

    float Vector2::LengthSq() const { return x * x + y * y; }
    float Vector2::Length() const { return std::sqrt(LengthSq()); }

    Vector2 Vector2::Normalized() const {
        float len = Length();
        if (len > 0) {
            float inv = 1.0f / len;
            return Vector2(x * inv, y * inv);
        }
        return *this;
    }

    void Vector2::Normalize() {
        float len = Length();
        if (len > 0) {
            float inv = 1.0f / len;
            x *= inv;
            y *= inv;
        }
    }

    float Vector2::Dot(const Vector2& v) const { return x * v.x + y * v.y; }
    float Vector2::Cross(const Vector2& v) const { return x * v.y - y * v.x; }

    float Vector2::Distance(const Vector2& a, const Vector2& b) { return (a - b).Length(); }
    float Vector2::DistanceSq(const Vector2& a, const Vector2& b) { return (a - b).LengthSq(); }

    Vector2 Vector2::Lerp(const Vector2& a, const Vector2& b, float t) {
        return a + t * (b - a);
    }

    Vector2 operator*(float s, const Vector2& v) { return v * s; }

} // namespace USE