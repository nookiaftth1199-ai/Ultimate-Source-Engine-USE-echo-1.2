// BlendTree.h
#pragma once
#include "Pose.h"
#include <vector>
#include <functional>

namespace USE
{
	class BlendTree
	{
	public:
		// A single input to the tree (a pose provider)
		using PoseProvider = std::function<Pose()>;

		struct Node
		{
			PoseProvider provider;
			float weight = 0.0f;
		};

		void AddNode(PoseProvider provider, float weight);
		void Update(float deltaTime);
		const Pose& GetResultPose() const { return m_result; }

	private:
		std::vector<Node> m_nodes;
		Pose m_result;
	};
}