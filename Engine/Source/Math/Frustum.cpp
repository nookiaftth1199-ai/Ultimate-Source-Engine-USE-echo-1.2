#include "stdafx.h"
#include "Frustum.h"
#include "Matrix4.h"

namespace USE
{
	Frustum::Frustum() {
		for (int i = 0; i < 6; ++i) planes[i] = Plane(Vector3(0, 0, 0), 0);
	}
	void Frustum::BuildFromMatrix(const Matrix4& vp) {
		planes[0] = Plane(Vector3(vp(0, 3) + vp(0, 0), vp(1, 3) + vp(1, 0), vp(2, 3) + vp(2, 0)), vp(3, 3) + vp(3, 0));
		planes[1] = Plane(Vector3(vp(0, 3) - vp(0, 0), vp(1, 3) - vp(1, 0), vp(2, 3) - vp(2, 0)), vp(3, 3) - vp(3, 0));
		planes[2] = Plane(Vector3(vp(0, 3) + vp(0, 1), vp(1, 3) + vp(1, 1), vp(2, 3) + vp(2, 1)), vp(3, 3) + vp(3, 1));
		planes[3] = Plane(Vector3(vp(0, 3) - vp(0, 1), vp(1, 3) - vp(1, 1), vp(2, 3) - vp(2, 1)), vp(3, 3) - vp(3, 1));
		planes[4] = Plane(Vector3(vp(0, 2), vp(1, 2), vp(2, 2)), vp(3, 2));
		planes[5] = Plane(Vector3(vp(0, 3) - vp(0, 2), vp(1, 3) - vp(1, 2), vp(2, 3) - vp(2, 2)), vp(3, 3) - vp(3, 2));
		for (int i = 0; i < 6; ++i) {
			float len = planes[i].normal.Length();
			if (len > 0.000001f) {
				planes[i].normal = planes[i].normal * (1.0f / len);
				planes[i].d /= len;
			}
		}
	}
	bool Frustum::ContainsSphere(const Vector3& c, float r) const {
		for (int i = 0; i < 6; ++i)
			if (Vector3::Dot(planes[i].normal, c) + planes[i].d + r < 0) return false;
		return true;
	}
	bool Frustum::ContainsAABB(const AABB& a) const {
		for (int i = 0; i < 6; ++i) {
			Vector3 p(planes[i].normal.x > 0 ? a.max.x : a.min.x,
				planes[i].normal.y > 0 ? a.max.y : a.min.y,
				planes[i].normal.z > 0 ? a.max.z : a.min.z);
			if (Vector3::Dot(planes[i].normal, p) + planes[i].d < 0) return false;
		}
		return true;
	}
}