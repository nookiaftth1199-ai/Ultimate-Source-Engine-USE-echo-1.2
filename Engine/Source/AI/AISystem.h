// ============================================================
// Ultimate Source Engine - AI System
//============================================================
//
// Manages AI controllers, behavior trees, and global AI queries.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Entity/SystemManager.h"
#include <vector>

namespace USE {

    class AIController;
    class BehaviorTree;

    class AISystem : public ISystem {
    public:
        AISystem();
        virtual ~AISystem();

        // ISystem interface
        void Initialize(EntityManager* entityManager, ComponentManager* componentManager) override;
        void Update(float deltaTime) override;
        void Shutdown() override;

        // Register an AI controller (optional, if not using ECS)
        void RegisterController(AIController* controller);
        void UnregisterController(AIController* controller);

    private:
        EntityManager*    m_entityManager;
        ComponentManager* m_componentManager;
        std::vector<AIController*> m_controllers; // for non‑ECS approach
    };

} // namespace USE