// ============================================================
// Ultimate Source Engine - Audio System
// ============================================================
//
// System that updates all entities with AudioComponent.
// Ensures that 3D audio sources are positioned according to
// the entity's transform.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Entity/SystemManager.h"

namespace USE {

    class AudioSystem : public ISystem {
    public:
        AudioSystem();
        virtual ~AudioSystem();

        // ISystem interface
        void Initialize(EntityManager* entityManager, ComponentManager* componentManager) override;
        void Update(float deltaTime) override;
        void Shutdown() override;

    private:
        EntityManager*    m_entityManager;
        ComponentManager* m_componentManager;
    };

} // namespace USE