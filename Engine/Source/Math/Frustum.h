// Frustum.h
#pragma once

#include "Plane.h"
#include "Vector3.h"
#include "AABB.h"

namespace USE
{
	class Frustum
	{
	public:
		Plane planes[6];

		Frustum();
		void BuildFromMatrix(const Matrix4& vp);
		bool ContainsSphere(const Vector3& center, float radius) const;
		bool ContainsAABB(const AABB& aabb) const;
	};
}