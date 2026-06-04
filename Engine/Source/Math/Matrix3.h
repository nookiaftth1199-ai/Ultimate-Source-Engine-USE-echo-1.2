#pragma once
#include "Vector3.h"

namespace USE
{
	class Quaternion;   // forward declare

	class Matrix3
	{
	public:
		float m[3][3];
		Matrix3();
		Matrix3(const Quaternion& q);
		static Matrix3 Identity();
	};
}