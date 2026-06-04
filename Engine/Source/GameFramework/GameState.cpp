// ============================================================
// Ultimate Source Engine - Game State Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "GameState.h"
#include "PlayerState.h"
#include "Core/Logger.h"

namespace USE {

    GameState::GameState()
        : m_gameTime(0.0f)
    {
        // Default: at least one team (team 0)
        m_teamScores.push_back(0);
    }

    GameState::~GameState()
    {
        // Player states are managed elsewhere
    }

    void GameState::Reset()
    {
        m_gameTime = 0.0f;
        for (auto& score : m_teamScores) {
            score = 0;
        }
    }

    int GameState::GetScore(int team) const
    {
        if (team >= 0 && team < (int)m_teamScores.size())
            return m_teamScores[team];
        return 0;
    }

    void GameState::AddScore(int points, int team)
    {
        if (team < 0) team = 0;
        // Ensure vector size is large enough
        if (team >= (int)m_teamScores.size()) {
            m_teamScores.resize(team + 1, 0);
        }
        m_teamScores[team] += points;
        USE_LOG_INFO("GameState: Team %d score now %d", team, m_teamScores[team]);
    }

    void GameState::AddPlayerState(PlayerState* playerState)
    {
        if (!playerState) return;
        m_playerStates.push_back(playerState);
    }

    void GameState::RemovePlayerState(PlayerState* playerState)
    {
        auto it = std::find(m_playerStates.begin(), m_playerStates.end(), playerState);
        if (it != m_playerStates.end()) {
            m_playerStates.erase(it);
        }
    }

} // namespace USE