// Ray.h
#pragma once

#include "Vector3.h"

namespace USE
{
	class Plane;
	class AABB;

	class Ray
	{
	public:
		Vector3 origin;
		Vector3 direction;

		Ray();
		Ray(const Vector3& origin, const Vector3& direction);

		Vector3 GetPoint(float t) const;
		float IntersectPlane(const Plane& plane) const;
		float IntersectAABB(const AABB& aabb) const;
	};
}