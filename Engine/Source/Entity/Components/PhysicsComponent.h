#pragma once
#include "../Component.h"

namespace USE
{
	class PhysicsComponent : public Component
	{
	public:
		void SetMass(float mass) { m_mass = mass; }
		float GetMass() const { return m_mass; }

	private:
		float m_mass = 1.0f;
	};
}