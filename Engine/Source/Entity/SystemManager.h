// ============================================================
// Ultimate Source Engine - System Manager
// ============================================================
//
// Manages the systems that process entities and components.
// Systems are updated in the order they were added.
// ============================================================

#pragma once

#include "stdafx.h"
#include <vector>
#include <memory>

namespace USE {

    // Forward declarations
    class EntityManager;
    class ComponentManager;

    // -----------------------------------------------------------------
    // Base class for all systems
    // -----------------------------------------------------------------
    class ISystem {
    public:
        virtual ~ISystem() = default;

        // Called once when the system is registered
        virtual void Initialize(EntityManager* entityManager, ComponentManager* componentManager) {}

        // Called every frame to update the system
        virtual void Update(float deltaTime) = 0;

        // Called when the system is removed (or engine shuts down)
        virtual void Shutdown() {}

        // Enable/disable the system (disabled systems are skipped during Update)
        void SetEnabled(bool enabled) { m_enabled = enabled; }
        bool IsEnabled() const { return m_enabled; }

    protected:
        EntityManager*      m_entityManager = nullptr;
        ComponentManager*   m_componentManager = nullptr;
        bool                m_enabled = true;
    };

    // -----------------------------------------------------------------
    // System Manager class
    // -----------------------------------------------------------------
    class SystemManager {
    public:
        SystemManager();
        ~SystemManager();

        // Initialize with references to entity and component managers
        void Initialize(EntityManager* entityManager, ComponentManager* componentManager);

        // Register a system (takes ownership)
        template<typename T, typename... Args>
        T* RegisterSystem(Args&&... args);

        // Get a registered system by type (returns nullptr if not found)
        template<typename T>
        T* GetSystem() const;

        // Update all enabled systems (call every frame)
        void Update(float deltaTime);

        // Enable/disable all systems
        void SetAllEnabled(bool enabled);

        // Remove all systems
        void Clear();

    private:
        EntityManager*               m_entityManager;
        ComponentManager*            m_componentManager;
        std::vector<std::unique_ptr<ISystem>> m_systems;
    };

    // -----------------------------------------------------------------
    // Template implementations
    // -----------------------------------------------------------------
    template<typename T, typename... Args>
    T* SystemManager::RegisterSystem(Args&&... args)
    {
        static_assert(std::is_base_of<ISystem, T>::value, "T must derive from ISystem");
        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        system->Initialize(m_entityManager, m_componentManager);
        T* raw = system.get();
        m_systems.push_back(std::move(system));
        return raw;
    }

    template<typename T>
    T* SystemManager::GetSystem() const
    {
        static_assert(std::is_base_of<ISystem, T>::value, "T must derive from ISystem");
        for (const auto& sys : m_systems) {
            T* casted = dynamic_cast<T*>(sys.get());
            if (casted) return casted;
        }
        return nullptr;
    }

} // namespace USE