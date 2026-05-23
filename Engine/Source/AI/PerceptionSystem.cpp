// ============================================================
// Ultimate Source Engine - Perception System Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "PerceptionSystem.h"
#include "Entity/Entity.h"
#include "Entity/Components/PerceptionComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Core/Timer.h"
#include "Math/MathUtils.h"

namespace USE {

    PerceptionSystem::PerceptionSystem()
        : m_entityManager(nullptr)
        , m_componentManager(nullptr)
    {
    }

    PerceptionSystem::~PerceptionSystem()
    {
        Shutdown();
    }

    void PerceptionSystem::Initialize(EntityManager* entityManager, ComponentManager* componentManager)
    {
        m_entityManager = entityManager;
        m_componentManager = componentManager;

        // Cache perception components (for efficiency)
        // In a real ECS, you'd have a component pool; here we just query each frame.
    }

    void PerceptionSystem::Update(float deltaTime)
    {
        // First, collect all active perception components
        m_perceptionComponents.clear();
        std::vector<Entity*> entities = m_entityManager->GetAllEntities();
        for (Entity* entity : entities) {
            if (!entity->IsActive()) continue;
            PerceptionComponent* pc = entity->GetComponent<PerceptionComponent>();
            if (pc && pc->IsActive()) {
                m_perceptionComponents.push_back(pc);
            }
        }

        // Process all registered stimuli
        ProcessStimuli();

        // Clear stimuli for next frame
        m_stimuli.clear();
    }

    void PerceptionSystem::Shutdown()
    {
        m_perceptionComponents.clear();
        m_stimuli.clear();
    }

    void PerceptionSystem::RegisterStimulus(const Stimulus& stimulus)
    {
        m_stimuli.push_back(stimulus);
    }

    void PerceptionSystem::ProcessStimuli()
    {
        // For each perception component, test against all stimuli
        float currentTime = Engine::Get()->GetTotalTime(); // assume Engine has GetTotalTime()

        for (PerceptionComponent* pc : m_perceptionComponents) {
            Entity* owner = pc->GetOwner();
            if (!owner || !owner->IsActive()) continue;

            TransformComponent* tc = owner->GetComponent<TransformComponent>();
            if (!tc) continue;
            Vector3 sensorPos = tc->worldTransform.translation;

            for (const Stimulus& stim : m_stimuli) {
                // Check if this component can sense this stimulus type
                if (!pc->CanSenseType(stim.type)) continue;

                // Compute distance
                float dist = (stim.position - sensorPos).Length();
                if (dist > stim.radius) continue;

                // Check line of sight if required
                bool losOk = true;
                if (pc->RequiresLineOfSight() && stim.type == StimulusType::Sight) {
                    losOk = pc->CheckLineOfSight(sensorPos, stim.position);
                }

                if (losOk) {
                    // Calculate intensity based on distance (falloff)
                    float intensity = stim.intensity * (1.0f - dist / stim.radius);
                    pc->ReportStimulus(stim, intensity, currentTime);
                }
            }

            // Update memory decay, etc. (could be handled in PerceptionComponent::Update)
            pc->UpdateMemory(currentTime);
        }
    }

} // namespace USE