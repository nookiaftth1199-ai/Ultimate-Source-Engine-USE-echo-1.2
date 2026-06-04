#include "stdafx.h"
#include "Quaternion.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include <algorithm>
#include <cmath>

namespace USE
{
	Quaternion::Quaternion() : x(0), y(0), z(0), w(1) {}
	Quaternion::Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	Quaternion Quaternion::Identity() { return Quaternion(); }

	Quaternion Quaternion::operator*(const Quaternion& q) const {
		return Quaternion(w*q.x + x * q.w + y * q.z - z * q.y,
			w*q.y - x * q.z + y * q.w + z * q.x,
			w*q.z + x * q.y - y * q.x + z * q.w,
			w*q.w - x * q.x - y * q.y - z * q.z);
	}
	Quaternion Quaternion::operator*(float s) const { return Quaternion(x*s, y*s, z*s, w*s); }
	Quaternion operator*(float s, const Quaternion& q) { return q * s; }

	Quaternion Quaternion::Normalized() const {
		float len = std::sqrt(x*x + y * y + z * z + w * w);
		if (len > 0.000001f) return Quaternion(x / len, y / len, z / len, w / len);
		return Identity();
	}

	Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t) {
		float dot = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
		dot = std::max(-1.0f, std::min(1.0f, dot));
		float theta = std::acos(std::abs(dot));
		float sinTheta = std::sin(theta);
		if (sinTheta < 0.001f) return a;
		float s0 = std::sin((1 - t)*theta) / sinTheta;
		float s1 = (dot >= 0 ? 1 : -1) * std::sin(t*theta) / sinTheta;
		return Quaternion(a.x*s0 + b.x*s1, a.y*s0 + b.y*s1, a.z*s0 + b.z*s1, a.w*s0 + b.w*s1);
	}

	Quaternion Quaternion::FromAxisAngle(const Vector3& axis, float angle) {
		float half = angle * 0.5f;
		float s = std::sin(half);
		return Quaternion(axis.x*s, axis.y*s, axis.z*s, std::cos(half));
	}

	Vector3 Quaternion::Rotate(const Vector3& v) const {
		Quaternion qv(v.x, v.y, v.z, 0);
		Quaternion r = (*this) * qv * Conjugate();
		return Vector3(r.x, r.y, r.z);
	}

	Quaternion Quaternion::Conjugate() const { return Quaternion(-x, -y, -z, w); }
	Quaternion Quaternion::Inverse() const {
		float sq = x * x + y * y + z * z + w * w;
		if (sq > 0) { float inv = 1.0f / sq; return Quaternion(-x * inv, -y * inv, -z * inv, w*inv); }
		return Identity();
	}
	float Quaternion::Dot(const Quaternion& a, const Quaternion& b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }
	Vector3 Quaternion::ToEuler() const { return Vector3(0, 0, 0); }
	Matrix3 Quaternion::ToMatrix3() const { return Matrix3(*this); }
	Matrix4 Quaternion::ToMatrix4() const { return Matrix4::Identity(); }
}