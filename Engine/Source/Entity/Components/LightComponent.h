#pragma once
#include "../Component.h"
#include "Math/Vector4.h"

namespace USE
{
	class LightComponent : public Component
	{
	public:
		void SetColor(const Vector4& color) { m_color = color; }
		Vector4 GetColor() const { return m_color; }
		void SetRange(float range) { m_range = range; }
		float GetRange() const { return m_range; }

	private:
		Vector4 m_color = { 1,1,1,1 };
		float m_range = 10.0f;
	};
}