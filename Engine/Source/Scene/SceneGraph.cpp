// ============================================================
// Ultimate Source Engine - Scene Graph Implementation
// ============================================================

#include "stdafx.h"
#include "SceneGraph.h"

namespace USE
{
	SceneGraph::SceneGraph() = default;
	SceneGraph::~SceneGraph() = default;

	void SceneGraph::AddNode(SceneNode* parent, SceneNode* child)
	{
		if (parent && child)
			parent->AddChild(child);
	}

	void SceneGraph::RemoveNode(SceneNode* parent, SceneNode* child)
	{
		if (parent && child)
			parent->RemoveChild(child);
	}
}