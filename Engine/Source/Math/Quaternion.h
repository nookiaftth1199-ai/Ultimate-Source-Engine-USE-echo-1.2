// ============================================================
// Ultimate Source Engine - Quaternion
// ============================================================
#pragma once

#include "Vector3.h"

namespace USE
{
	class Matrix3;   // forward declaration instead of #include "Matrix3.h"
	class Matrix4;

	class Quaternion
	{
	public:
		float x, y, z, w;

		Quaternion();
		Quaternion(float _x, float _y, float _z, float _w);

		static Quaternion Identity();

		Quaternion operator*(const Quaternion& q) const;
		Quaternion operator*(float s) const;
		friend Quaternion operator*(float s, const Quaternion& q);

		Quaternion Normalized() const;
		static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);
		static Quaternion FromAxisAngle(const Vector3& axis, float angle);
		static Quaternion FromEuler(const Vector3& eulerRad);

		Vector3 Rotate(const Vector3& v) const;
		Quaternion Conjugate() const;
		Quaternion Inverse() const;
		static float Dot(const Quaternion& a, const Quaternion& b);

		Vector3 ToEuler() const;
		Matrix3 ToMatrix3() const;
		Matrix4 ToMatrix4() const;

		bool operator==(const Quaternion& other) const;
		bool operator!=(const Quaternion& other) const;
	};
}