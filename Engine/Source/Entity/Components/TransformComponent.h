#pragma once
#include "../Component.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

namespace USE
{
	class TransformComponent : public Component
	{
	public:
		Vector3 GetLocalPosition() const { return m_position; }
		void SetLocalPosition(const Vector3& pos) { m_position = pos; }
		Quaternion GetLocalRotation() const { return m_rotation; }
		void SetLocalRotation(const Quaternion& rot) { m_rotation = rot; }
		Vector3 GetLocalScale() const { return m_scale; }
		void SetLocalScale(const Vector3& scale) { m_scale = scale; }

		Vector3 GetWorldPosition() const;   // recursion through parent
		Quaternion GetWorldRotation() const;
		Matrix4 GetWorldMatrix() const;

		Vector3 m_position = { 0,0,0 };
		Quaternion m_rotation = Quaternion::Identity();
		Vector3 m_scale = { 1,1,1 };
	};
}