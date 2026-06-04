#pragma once
#include "../Component.h"
#include "Math/Vector3.h"

namespace USE
{
	class Model;

	class RenderComponent : public Component
	{
	public:
		void SetModel(Model* model) { m_model = model; }
		Model* GetModel() const { return m_model; }

		Vector3 GetBoundsCenter() const { return m_boundsCenter; }
		float GetBoundsRadius() const { return m_boundsRadius; }

	private:
		Model* m_model = nullptr;
		Vector3 m_boundsCenter = { 0,0,0 };
		float m_boundsRadius = 1.0f;
	};
}