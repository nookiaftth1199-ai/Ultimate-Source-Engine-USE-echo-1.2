// ============================================================
// Ultimate Source Engine - Scene
// ============================================================
#pragma once

#include "Entity/Entity.h"
#include "Entity/World.h"
#include "SceneNode.h"
#include <vector>
#include <string>

namespace USE
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity* CreateEntity(const std::string& name = "Untitled");
		void DestroyEntity(Entity* entity);
		const std::vector<Entity*>& GetEntities() const { return m_entities; }

		SceneNode* GetRootNode() { return &m_rootNode; }

		World* GetWorld() { return &m_world; }

	private:
		std::vector<Entity*> m_entities;
		SceneNode m_rootNode;
		World m_world;
	};
}