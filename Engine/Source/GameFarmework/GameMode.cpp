// ============================================================
// Ultimate Source Engine - Game Mode Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "GameMode.h"
#include "GameFramework/PlayerController.h"
#include "Core/Logger.h"

namespace USE {

    GameMode::GameMode()
        : m_gameState(GameModeState::WaitingForPlayers)
        , m_gameTime(0.0f)
        , m_bIsGameOver(false)
    {
    }

    GameMode::~GameMode()
    {
        EndPlay();
    }

    void GameMode::StartPlay()
    {
        USE_LOG_INFO("GameMode::StartPlay");
    }

    void GameMode::EndPlay()
    {
        USE_LOG_INFO("GameMode::EndPlay");
        m_players.clear();
    }

    void GameMode::Update(float deltaTime)
    {
        if (m_gameState == GameModeState::InProgress) {
            m_gameTime += deltaTime;
        }
        // Override in derived classes
    }

    void GameMode::AddPlayer(PlayerController* player)
    {
        if (!player) return;
        m_players.push_back(player);
        USE_LOG_INFO("GameMode: Player added (total %zu)", m_players.size());
    }

    void GameMode::RemovePlayer(PlayerController* player)
    {
        auto it = std::find(m_players.begin(), m_players.end(), player);
        if (it != m_players.end()) {
            m_players.erase(it);
            USE_LOG_INFO("GameMode: Player removed (total %zu)", m_players.size());
        }
    }

    bool GameMode::CanPlayerSpawn(PlayerController* player) const
    {
        // Default: players can spawn if game is in progress or waiting
        return (m_gameState == GameModeState::WaitingForPlayers ||
                m_gameState == GameModeState::InProgress);
    }

    void GameMode::SpawnPlayer(PlayerController* player)
    {
        if (!player) return;
        if (CanPlayerSpawn(player)) {
            // In a real implementation, you'd call player->Spawn() and set a spawn point.
            USE_LOG_INFO("GameMode: Spawning player %s", player->GetName().c_str());
        }
    }

    void GameMode::OnPlayerKilled(PlayerController* victim, PlayerController* killer)
    {
        // Base implementation does nothing.
    }

    void GameMode::StartGame()
    {
        if (m_gameState == GameModeState::WaitingForPlayers) {
            m_gameState = GameModeState::InProgress;
            m_gameTime = 0.0f;
            m_bIsGameOver = false;
            USE_LOG_INFO("GameMode: Game started");
        }
    }

    void GameMode::EndGame()
    {
        if (m_gameState == GameModeState::InProgress) {
            m_gameState = GameModeState::GameOver;
            m_bIsGameOver = true;
            USE_LOG_INFO("GameMode: Game ended");
        }
    }

    void GameMode::RestartGame()
    {
        m_gameState = GameModeState::Restarting;
        // Reset all players, etc.
        m_gameState = GameModeState::WaitingForPlayers;
        m_bIsGameOver = false;
        m_gameTime = 0.0f;
        USE_LOG_INFO("GameMode: Game restarting");
    }

    void GameMode::PauseGame()
    {
        if (m_gameState == GameModeState::InProgress) {
            m_gameState = GameModeState::Paused;
            USE_LOG_INFO("GameMode: Game paused");
        }
    }

    void GameMode::ResumeGame()
    {
        if (m_gameState == GameModeState::Paused) {
            m_gameState = GameModeState::InProgress;
            USE_LOG_INFO("GameMode: Game resumed");
        }
    }

    bool GameMode::IsGameOver() const
    {
        return m_bIsGameOver;
    }

    bool GameMode::HasWinner() const
    {
        return false; // override in derived
    }

    PlayerController* GameMode::GetWinner() const
    {
        return nullptr; // override
    }

} // namespace USE