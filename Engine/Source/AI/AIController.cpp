// ============================================================
// Ultimate Source Engine - AI Controller Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "AIController.h"
#include "Core/Engine.h"
#include "NavigationSystem.h"
#include "Math/MathUtils.h"

namespace USE {

    AIController::AIController()
        : m_pawn(nullptr)
        , m_behaviorTree(nullptr)
        , m_behaviorTreeRunning(false)
        , m_hasDestination(false)
        , m_acceptanceRadius(1.0f)
        , m_target(nullptr)
    {
    }

    AIController::~AIController()
    {
        Unpossess();
    }

    void AIController::Possess(Pawn* pawn)
    {
        if (m_pawn == pawn) return;
        Unpossess();
        m_pawn = pawn;
        if (m_pawn) {
            // Optionally set controller reference in pawn
        }
    }

    void AIController::Unpossess()
    {
        if (m_pawn) {
            // Clean up pawn association
            m_pawn = nullptr;
        }
    }

    void AIController::SetBehaviorTree(BehaviorTree* tree)
    {
        m_behaviorTree = tree;
    }

    void AIController::RunBehaviorTree()
    {
        if (m_behaviorTree) {
            m_behaviorTree->Reset();
            m_behaviorTreeRunning = true;
        }
    }

    void AIController::Update(float deltaTime)
    {
        if (!m_pawn) return;

        // Update behavior tree
        if (m_behaviorTree && m_behaviorTreeRunning) {
            BTStatus status = m_behaviorTree->Tick(deltaTime);
            if (status == BTStatus::Success || status == BTStatus::Failure) {
                m_behaviorTreeRunning = false;
            }
        }

        // Move towards destination
        if (m_hasDestination) {
            Vector3 toTarget = m_moveDestination - m_pawn->GetPosition();
            float distance = toTarget.Length();
            if (distance <= m_acceptanceRadius) {
                m_hasDestination = false;
                m_pawn->Move(Vector3::Zero, 0.0f);
            } else {
                Vector3 direction = toTarget / distance;
                m_pawn->Move(direction, m_pawn->GetMoveSpeed());
            }
        }
    }

    void AIController::MoveTo(const Vector3& destination, float acceptanceRadius)
    {
        m_moveDestination = destination;
        m_acceptanceRadius = acceptanceRadius;
        m_hasDestination = true;
    }

    void AIController::StopMovement()
    {
        m_hasDestination = false;
        if (m_pawn) {
            m_pawn->Move(Vector3::Zero, 0.0f);
        }
    }

    bool AIController::HasReachedDestination() const
    {
        if (!m_hasDestination || !m_pawn) return false;
        return (m_pawn->GetPosition() - m_moveDestination).Length() <= m_acceptanceRadius;
    }

    void AIController::SetTarget(Entity* target)
    {
        m_target = target;
    }

} // namespace USE