// ============================================================
// Ultimate Source Engine - Game Mode
//============================================================
//
// Defines the rules and flow of a game. Manages player spawning,
// win/loss conditions, and game state.
// ============================================================

#pragma once

#include "stdafx.h"
#include <vector>
#include <string>

namespace USE {

    // Forward declarations
    class PlayerController;
    class GameState;

    // Possible game states
    enum class GameModeState {
        WaitingForPlayers,
        InProgress,
        Paused,
        GameOver,
        Restarting
    };

    class GameMode {
    public:
        GameMode();
        virtual ~GameMode();

        // Called when the game mode is activated
        virtual void StartPlay();

        // Called when the game mode is deactivated (e.g., level change)
        virtual void EndPlay();

        // Called every frame while the game mode is active
        virtual void Update(float deltaTime);

        // Player management
        virtual void AddPlayer(PlayerController* player);
        virtual void RemovePlayer(PlayerController* player);
        virtual bool CanPlayerSpawn(PlayerController* player) const;
        virtual void SpawnPlayer(PlayerController* player);
        virtual void OnPlayerKilled(PlayerController* victim, PlayerController* killer);

        // Game flow
        virtual void StartGame();
        virtual void EndGame();
        virtual void RestartGame();
        virtual void PauseGame();
        virtual void ResumeGame();

        // Condition checks
        virtual bool IsGameOver() const;
        virtual bool HasWinner() const;
        virtual PlayerController* GetWinner() const;

        // Accessors
        GameModeState GetGameState() const { return m_gameState; }
        const std::vector<PlayerController*>& GetPlayers() const { return m_players; }

    protected:
        std::vector<PlayerController*> m_players;
        GameModeState                  m_gameState;
        float                           m_gameTime;        // elapsed time since game start
        bool                            m_bIsGameOver;
    };

} // namespace USE