#include "stdafx.h"
#include "MathUtils.h"
#include <algorithm>

namespace USE
{
	float MathUtils::MinComponent(const Vector2& v) { return std::min(v.x, v.y); }
	float MathUtils::MaxComponent(const Vector2& v) { return std::max(v.x, v.y); }
	float MathUtils::MinComponent(const Vector3& v) { return std::min({ v.x, v.y, v.z }); }
	float MathUtils::MaxComponent(const Vector3& v) { return std::max({ v.x, v.y, v.z }); }
	float MathUtils::MinComponent(const Vector4& v) { return std::min({ v.x, v.y, v.z, v.w }); }
	float MathUtils::MaxComponent(const Vector4& v) { return std::max({ v.x, v.y, v.z, v.w }); }
	float MathUtils::Lerp(float a, float b, float t) { return a + (b - a) * t; }
	float MathUtils::Clamp(float v, float mn, float mx) { if (v < mn) return mn; if (v > mx) return mx; return v; }
}