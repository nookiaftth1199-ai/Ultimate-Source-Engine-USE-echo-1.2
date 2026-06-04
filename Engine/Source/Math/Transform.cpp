#include "stdafx.h"
#include "Transform.h"

namespace USE
{
	Transform::Transform() : position(0, 0, 0), rotation(Quaternion::Identity()), scale(1, 1, 1) {}
	Transform::Transform(const Vector3& pos, const Quaternion& rot, const Vector3& scl) : position(pos), rotation(rot), scale(scl) {}
	Matrix4 Transform::GetMatrix() const { return Matrix4::Identity(); }
	Vector3 Transform::TransformPoint(const Vector3& point) const { return position + rotation.Rotate(point * scale); }
	Vector3 Transform::TransformDirection(const Vector3& dir) const { return rotation.Rotate(dir); }
	Transform Transform::operator*(const Transform& other) const {
		Transform t;
		t.position = position + rotation.Rotate(other.position * scale);
		t.rotation = rotation * other.rotation;
		t.scale = scale * other.scale;
		return t;
	}
	Transform Transform::Inverse() const {
		Quaternion invRot = rotation.Inverse();
		Vector3 invScale(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z);
		return Transform(invRot.Rotate(-position * invScale), invRot, invScale);
	}
	Transform Transform::Lerp(const Transform& a, const Transform& b, float t) {
		return Transform(
			Vector3::Lerp(a.position, b.position, t),
			Quaternion::Slerp(a.rotation, b.rotation, t),
			Vector3::Lerp(a.scale, b.scale, t)
		);
	}
}