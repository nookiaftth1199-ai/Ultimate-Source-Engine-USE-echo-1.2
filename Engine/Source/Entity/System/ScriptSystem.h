// ============================================================
// Ultimate Source Engine - Script System
//============================================================
//
// System that updates all entities with ScriptComponent.
// Calls the script's OnUpdate function each frame.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Entity/SystemManager.h"

namespace USE {

    class ScriptSystem : public ISystem {
    public:
        ScriptSystem();
        virtual ~ScriptSystem();

        // ISystem interface
        void Initialize(EntityManager* entityManager, ComponentManager* componentManager) override;
        void Update(float deltaTime) override;
        void Shutdown() override;

    private:
        EntityManager*    m_entityManager;
        ComponentManager* m_componentManager;
    };

} // namespace USE