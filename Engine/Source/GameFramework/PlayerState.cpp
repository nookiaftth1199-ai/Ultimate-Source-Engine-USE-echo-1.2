// ============================================================
// Ultimate Source Engine - Player State Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "PlayerState.h"

namespace USE {

    PlayerState::PlayerState()
        : m_playerId(0)
        , m_playerName("Player")
        , m_score(0)
        , m_kills(0)
        , m_deaths(0)
        , m_health(100.0f)
        , m_team(0)
        , m_bReady(false)
        , m_playerController(nullptr)
    {
    }

    PlayerState::~PlayerState()
    {
    }

    void PlayerState::Reset()
    {
        m_score = 0;
        m_kills = 0;
        m_deaths = 0;
        m_health = 100.0f;
        m_bReady = false;
        // Do not reset playerId, name, team (they are persistent)
    }

} // namespace USE