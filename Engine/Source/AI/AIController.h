// ============================================================
// Ultimate Source Engine - AI Controller
//============================================================
//
// Controls an AI‑controlled pawn. Runs a behavior tree and handles
// perception, movement, and decision making.
// ============================================================

#pragma once

#include "stdafx.h"
#include "GameFramework/Pawn.h"
#include "AI/BehaviorTree.h"
#include "Math/Vector3.h"
#include <memory>

namespace USE {

    class AIController {
    public:
        AIController();
        virtual ~AIController();

        // Possess/unpossess a pawn
        void Possess(Pawn* pawn);
        void Unpossess();
        Pawn* GetPawn() const { return m_pawn; }

        // Behavior tree
        void SetBehaviorTree(BehaviorTree* tree);
        void RunBehaviorTree();

        // Update (called by AISystem)
        virtual void Update(float deltaTime);

        // Movement commands
        void MoveTo(const Vector3& destination, float acceptanceRadius = 1.0f);
        void StopMovement();

        // Perception (simplified)
        void SetTarget(Entity* target);
        Entity* GetTarget() const { return m_target; }

        // State
        bool HasReachedDestination() const;

    protected:
        Pawn*            m_pawn;
        BehaviorTree*    m_behaviorTree;
        bool             m_behaviorTreeRunning;

        Vector3          m_moveDestination;
        bool             m_hasDestination;
        float            m_acceptanceRadius;

        Entity*          m_target;      // current target
    };

} // namespace USE