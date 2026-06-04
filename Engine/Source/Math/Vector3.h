#pragma once
#include <cmath>

namespace USE
{
	class Vector3
	{
	public:
		float x, y, z;

		Vector3() : x(0), y(0), z(0) {}
		Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

		// Arithmetic
		Vector3 operator+(const Vector3& o) const { return Vector3(x + o.x, y + o.y, z + o.z); }
		Vector3 operator-(const Vector3& o) const { return Vector3(x - o.x, y - o.y, z - o.z); }
		Vector3 operator*(float s) const { return Vector3(x*s, y*s, z*s); }
		Vector3 operator/(float s) const { return Vector3(x / s, y / s, z / s); }
		Vector3 operator-() const { return Vector3(-x, -y, -z); }
		friend Vector3 operator*(float s, const Vector3& v) { return v * s; }

		// Component‑wise multiply
		Vector3 operator*(const Vector3& o) const { return Vector3(x*o.x, y*o.y, z*o.z); }

		float Length() const { return std::sqrt(x*x + y * y + z * z); }
		float LengthSquared() const { return x * x + y * y + z * z; }
		Vector3 Normalized() const {
			float len = Length();
			return (len > 0.000001f) ? (*this) / len : Vector3(0, 0, 0);
		}

		// Static helpers
		static Vector3 Zero() { return Vector3(0, 0, 0); }
		static Vector3 One() { return Vector3(1, 1, 1); }
		static Vector3 Up() { return Vector3(0, 1, 0); }
		static Vector3 Forward() { return Vector3(0, 0, -1); }
		static Vector3 Right() { return Vector3(1, 0, 0); }
		static Vector3 Back() { return Vector3(0, 0, 1); }   // opposite of Forward

		// Dot product (static and instance)
		static float Dot(const Vector3& a, const Vector3& b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
		float Dot(const Vector3& o) const { return Dot(*this, o); }

		// Cross product (static and instance)
		static Vector3 Cross(const Vector3& a, const Vector3& b) {
			return Vector3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
		}
		Vector3 Cross(const Vector3& o) const { return Cross(*this, o); }

		// Linear interpolation
		static Vector3 Lerp(const Vector3& a, const Vector3& b, float t) { return a + (b - a) * t; }

		// Array access
		float operator[](int i) const { return (&x)[i]; }
		float& operator[](int i) { return (&x)[i]; }
	};
}