// Plane.h
#pragma once

#include "Vector3.h"

namespace USE
{
	class Plane
	{
	public:
		Vector3 normal;
		float   d;

		Plane();
		Plane(const Vector3& n, float d);
	};
}