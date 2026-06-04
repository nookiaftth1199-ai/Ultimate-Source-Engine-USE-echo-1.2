#include "stdafx.h"
#include "Matrix3.h"
#include "Quaternion.h"
#include <cstring>

namespace USE
{
	Matrix3::Matrix3() { std::memset(m, 0, sizeof(m)); }
	Matrix3::Matrix3(const Quaternion& q) {
		float x = q.x, y = q.y, z = q.z, w = q.w;
		float xx = x * x, yy = y * y, zz = z * z;
		float xy = x * y, xz = x * z, yz = y * z;
		float wx = w * x, wy = w * y, wz = w * z;
		m[0][0] = 1 - 2 * (yy + zz); m[0][1] = 2 * (xy - wz);   m[0][2] = 2 * (xz + wy);
		m[1][0] = 2 * (xy + wz);   m[1][1] = 1 - 2 * (xx + zz); m[1][2] = 2 * (yz - wx);
		m[2][0] = 2 * (xz - wy);   m[2][1] = 2 * (yz + wx);   m[2][2] = 1 - 2 * (xx + yy);
	}
	Matrix3 Matrix3::Identity() {
		Matrix3 r;
		r.m[0][0] = r.m[1][1] = r.m[2][2] = 1;
		return r;
	}
}