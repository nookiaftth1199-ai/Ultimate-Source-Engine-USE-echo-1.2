// BlendTree.cpp
#include "stdafx.h"
#include "BlendTree.h"

namespace USE
{
	void BlendTree::AddNode(PoseProvider provider, float weight)
	{
		m_nodes.push_back({ provider, weight });
	}

	void BlendTree::Update(float)
	{
		if (m_nodes.empty()) return;
		// Simple weighted blending (positions and rotations)
		m_result.SetBoneCount(m_nodes[0].provider().GetLocalPosition(0) /* need bone count */);
		// Simplified: blend only the first node for now.
		// Full implementation would blend all nodes.
		m_result = m_nodes[0].provider();
	}
}