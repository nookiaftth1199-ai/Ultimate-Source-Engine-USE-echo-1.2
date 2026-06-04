// ============================================================
// Ultimate Source Engine - Game State
//============================================================
//
// Holds the global state of the game (time, scores, etc.).
// Replicated to all clients in multiplayer games.
// ============================================================

#pragma once

#include "stdafx.h"
#include <vector>
#include <string>

namespace USE {

    class PlayerState; // forward declaration

    class GameState {
    public:
        GameState();
        virtual ~GameState();

        // Reset to initial values
        virtual void Reset();

        // Accessors
        float GetGameTime() const { return m_gameTime; }
        void SetGameTime(float time) { m_gameTime = time; }

        int GetScore(int team = 0) const; // team 0 = overall
        void AddScore(int points, int team = 0);

        // Player states (for multiplayer)
        void AddPlayerState(PlayerState* playerState);
        void RemovePlayerState(PlayerState* playerState);
        const std::vector<PlayerState*>& GetPlayerStates() const { return m_playerStates; }

    protected:
        float m_gameTime;
        std::vector<int> m_teamScores; // index = team ID
        std::vector<PlayerState*> m_playerStates;
    };

} // namespace USE