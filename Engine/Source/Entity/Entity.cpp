// ============================================================
// Ultimate Source Engine - Entity Implementation
// ============================================================

#include "stdafx.h"
#include "Entity.h"
#include "Core/Logger.h"

namespace USE
{
	Entity::Entity(const std::string& name) : m_name(name) {}
	Entity::~Entity()
	{
		// Detach from parent and children
		if (m_parent)
			m_parent->RemoveChild(this);
		for (Entity* child : m_children)
			child->m_parent = nullptr;
	}

	void Entity::SetParent(Entity* parent)
	{
		if (m_parent)
			m_parent->RemoveChild(this);
		m_parent = parent;
		if (parent)
			parent->AddChild(this);
	}

	void Entity::AddChild(Entity* child)
	{
		if (child && child->m_parent == nullptr)
		{
			child->m_parent = this;
			m_children.push_back(child);
		}
	}

	void Entity::RemoveChild(Entity* child)
	{
		auto it = std::find(m_children.begin(), m_children.end(), child);
		if (it != m_children.end())
		{
			(*it)->m_parent = nullptr;
			m_children.erase(it);
		}
	}
}