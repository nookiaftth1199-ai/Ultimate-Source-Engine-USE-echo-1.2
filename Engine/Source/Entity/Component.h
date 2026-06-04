// ============================================================
// Ultimate Source Engine - Component Base
// ============================================================
#pragma once

namespace USE
{
	class Entity;

	class Component
	{
	public:
		virtual ~Component() = default;
		Entity* GetOwner() const { return m_owner; }
		void SetOwner(Entity* owner) { m_owner = owner; }

	private:
		Entity* m_owner = nullptr;
	};
}