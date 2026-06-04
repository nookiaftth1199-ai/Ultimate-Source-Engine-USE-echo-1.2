// Transform.h
#pragma once

#include "Vector3.h"
#include "Quaternion.h"
#include "Matrix4.h"

namespace USE
{
	class Transform
	{
	public:
		Vector3    position;
		Quaternion rotation;
		Vector3    scale;

		Transform();
		Transform(const Vector3& pos, const Quaternion& rot, const Vector3& scl);

		Matrix4 GetMatrix() const;
		Vector3 TransformPoint(const Vector3& point) const;
		Vector3 TransformDirection(const Vector3& dir) const;
		Transform operator*(const Transform& other) const;
		Transform Inverse() const;
		static Transform Lerp(const Transform& a, const Transform& b, float t);
	};
}