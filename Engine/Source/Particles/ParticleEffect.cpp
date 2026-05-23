// ============================================================
// Ultimate Source Engine - Particle Effect Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "ParticleEffect.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"

// JSON library (nlohmann/json)
#include "json.hpp"
using json = nlohmann::json;

namespace USE {

    ParticleEffect::ParticleEffect()
    {
        // Initialize default params
        m_params.emissionRate = 10.0f;
        m_params.lifetimeMin = 1.0f;
        m_params.lifetimeMax = 2.0f;
        m_params.speedMin = 1.0f;
        m_params.speedMax = 3.0f;
        m_params.sizeStart = 0.5f;
        m_params.sizeEnd = 0.0f;
        m_params.colorStart = Color(1,1,1,1);
        m_params.colorEnd = Color(1,0,0,0);
        m_params.direction = Vector3(0,1,0);
        m_params.spread = 0.5f;
        m_params.loop = true;
        m_params.maxParticles = 100;
    }

    bool ParticleEffect::LoadFromFile(const std::string& filename)
    {
        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("ParticleEffect: FileSystem not available");
            return false;
        }

        std::string resolved = fs->ResolvePath(filename);
        if (resolved.empty()) {
            USE_LOG_ERROR("ParticleEffect: File not found: %s", filename.c_str());
            return false;
        }

        auto file = fs->OpenFile(resolved, FILE_READ | FILE_TEXT);
        if (!file) {
            USE_LOG_ERROR("ParticleEffect: Could not open file: %s", resolved.c_str());
            return false;
        }

        std::string content;
        file->ReadAll(content);
        file->Close();

        json j;
        try {
            j = json::parse(content);
        } catch (const std::exception& e) {
            USE_LOG_ERROR("ParticleEffect: JSON parse error: %s", e.what());
            return false;
        }

        // Name
        m_name = j.value("name", "UnnamedEffect");

        // Parse parameters
        if (j.contains("params")) {
            const auto& p = j["params"];
            m_params.emissionRate = p.value("emissionRate", 10.0f);
            m_params.lifetimeMin = p.value("lifetimeMin", 1.0f);
            m_params.lifetimeMax = p.value("lifetimeMax", 2.0f);
            m_params.speedMin = p.value("speedMin", 1.0f);
            m_params.speedMax = p.value("speedMax", 3.0f);
            m_params.sizeStart = p.value("sizeStart", 0.5f);
            m_params.sizeEnd = p.value("sizeEnd", 0.0f);
            m_params.spread = p.value("spread", 0.5f);
            m_params.loop = p.value("loop", true);
            m_params.maxParticles = p.value("maxParticles", 100);

            // Color start
            if (p.contains("colorStart") && p["colorStart"].is_array()) {
                auto& arr = p["colorStart"];
                if (arr.size() >= 4) {
                    m_params.colorStart = Color(arr[0], arr[1], arr[2], arr[3]);
                }
            }
            // Color end
            if (p.contains("colorEnd") && p["colorEnd"].is_array()) {
                auto& arr = p["colorEnd"];
                if (arr.size() >= 4) {
                    m_params.colorEnd = Color(arr[0], arr[1], arr[2], arr[3]);
                }
            }
            // Direction
            if (p.contains("direction") && p["direction"].is_array()) {
                auto& dir = p["direction"];
                if (dir.size() >= 3) {
                    m_params.direction = Vector3(dir[0], dir[1], dir[2]);
                    m_params.direction.Normalize();
                }
            }
        }

        USE_LOG_INFO("ParticleEffect: Loaded '%s' from %s", m_name.c_str(), filename.c_str());
        return true;
    }

    ParticleEmitter* ParticleEffect::CreateEmitter() const
    {
        auto emitter = new ParticleEmitter();
        emitter->Initialize(m_params);
        return emitter;
    }

} // namespace USE