// ============================================================
// Ultimate Source Engine - Scene Node Implementation
// ============================================================

#include "stdafx.h"
#include "SceneNode.h"

namespace USE
{
	SceneNode::SceneNode() = default;
	SceneNode::SceneNode(const std::string& name) : m_name(name) {}
	SceneNode::~SceneNode() = default;

	void SceneNode::AddChild(SceneNode* child)
	{
		if (child && child->m_parent == nullptr)
		{
			child->m_parent = this;
			m_children.push_back(child);
		}
	}

	void SceneNode::RemoveChild(SceneNode* child)
	{
		auto it = std::find(m_children.begin(), m_children.end(), child);
		if (it != m_children.end())
		{
			(*it)->m_parent = nullptr;
			m_children.erase(it);
		}
	}

	void SceneNode::SetParent(SceneNode* parent)
	{
		if (m_parent)
			m_parent->RemoveChild(this);
		if (parent)
			parent->AddChild(this);
	}

	SceneNode* SceneNode::GetParent() const { return m_parent; }
	uint32_t SceneNode::GetChildrenCount() const { return static_cast<uint32_t>(m_children.size()); }
	SceneNode* SceneNode::GetChild(uint32_t index) const { return m_children[index]; }

	Vector3 SceneNode::GetWorldPosition() const
	{
		if (m_parent)
			return m_parent->GetWorldPosition() + m_localPosition;
		return m_localPosition;
	}
}