// ============================================================
// Ultimate Source Engine - Frustum Culler Implementation
// ============================================================

#include "stdafx.h"
#include "FrustumCuller.h"
#include "Scene.h"
#include "Entity/Components/RenderComponent.h"
#include "Math/Frustum.h"

namespace USE
{
	FrustumCuller::FrustumCuller() = default;
	FrustumCuller::~FrustumCuller() = default;

	void FrustumCuller::Cull(const Frustum& frustum, const std::vector<Entity*>& entities,
		std::vector<Entity*>& outVisible)
	{
		outVisible.clear();
		for (auto* entity : entities)
		{
			auto* renderComp = entity->GetComponent<RenderComponent>();
			if (!renderComp) continue;
			// Simple bounding sphere check – you can extend with AABB
			if (frustum.ContainsSphere(renderComp->GetBoundsCenter(), renderComp->GetBoundsRadius()))
				outVisible.push_back(entity);
		}
	}
}