// ============================================================
// Ultimate Source Engine - Spawn Point Component
//============================================================
//
// Component that marks an entity as a spawn location for players.
// Used by game modes to determine where to place new players.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Entity/Component.h"

namespace USE {

    class SpawnPoint : public Component {
    public:
        SpawnPoint();
        virtual ~SpawnPoint();

        // Team affiliation (0 = any, >0 = specific team)
        void SetTeam(int teamId) { m_teamId = teamId; }
        int GetTeam() const { return m_teamId; }

        // Priority (higher priority is preferred)
        void SetPriority(int priority) { m_priority = priority; }
        int GetPriority() const { return m_priority; }

        // Occupied flag (set when a player spawns here)
        void SetOccupied(bool occupied) { m_occupied = occupied; }
        bool IsOccupied() const { return m_occupied; }

        // Check if the spawn point is valid (not occupied and not blocked)
        bool IsValid() const;

        // Type name for serialization
        virtual const char* GetTypeName() const override { return "SpawnPoint"; }

    private:
        int  m_teamId;
        int  m_priority;
        bool m_occupied;
    };

} // namespace USE