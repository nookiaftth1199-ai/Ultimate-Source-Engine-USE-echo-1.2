// ============================================================
// Ultimate Source Engine - Animation System
// ============================================================
//
// System that updates animation components, advances animation time,
// and computes bone transforms for skeletal meshes.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Entity/SystemManager.h"

namespace USE {

    // Forward declarations
    class AnimationComponent;

    class AnimationSystem : public ISystem {
    public:
        AnimationSystem();
        virtual ~AnimationSystem();

        // ISystem interface
        void Initialize(EntityManager* entityManager, ComponentManager* componentManager) override;
        void Update(float deltaTime) override;
        void Shutdown() override;

    private:
        EntityManager*    m_entityManager;
        ComponentManager* m_componentManager;
    };

} // namespace USE