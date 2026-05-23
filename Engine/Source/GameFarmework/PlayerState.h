// ============================================================
// Ultimate Source Engine - Player State
//============================================================
//
// Holds the current state of a player in a multiplayer game.
// This includes score, health, team, etc. Replicated to all clients.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>

namespace USE {

    // Forward declarations
    class PlayerController;

    class PlayerState {
    public:
        PlayerState();
        virtual ~PlayerState();

        // Reset to default values (e.g., at start of round)
        virtual void Reset();

        // Unique player ID (e.g., network ID)
        void SetPlayerId(int id) { m_playerId = id; }
        int GetPlayerId() const { return m_playerId; }

        // Player name
        void SetPlayerName(const std::string& name) { m_playerName = name; }
        const std::string& GetPlayerName() const { return m_playerName; }

        // Score / kills / deaths
        void SetScore(int score) { m_score = score; }
        int GetScore() const { return m_score; }
        void AddScore(int points) { m_score += points; }

        void SetKills(int kills) { m_kills = kills; }
        int GetKills() const { return m_kills; }
        void AddKill() { ++m_kills; }

        void SetDeaths(int deaths) { m_deaths = deaths; }
        int GetDeaths() const { return m_deaths; }
        void AddDeath() { ++m_deaths; }

        // Health
        void SetHealth(float health) { m_health = health; }
        float GetHealth() const { return m_health; }
        void AddHealth(float amount) { m_health += amount; }

        // Team (if applicable)
        void SetTeam(int team) { m_team = team; }
        int GetTeam() const { return m_team; }

        // Ready state (for lobbies)
        void SetReady(bool ready) { m_bReady = ready; }
        bool IsReady() const { return m_bReady; }

        // Associated player controller (optional, not replicated)
        void SetPlayerController(PlayerController* controller) { m_playerController = controller; }
        PlayerController* GetPlayerController() const { return m_playerController; }

    protected:
        int         m_playerId;
        std::string m_playerName;
        int         m_score;
        int         m_kills;
        int         m_deaths;
        float       m_health;
        int         m_team;
        bool        m_bReady;

        // Not replicated, local reference
        PlayerController* m_playerController;
    };

} // namespace USE