// ============================================================
// Ultimate Source Engine - Entity
// ============================================================
//
// Base class for all game objects in the world. An entity has a
// transform and can have multiple components attached.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Transform.h"
#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace USE {

    // Forward declarations
    class Component;
    class EntityManager;

    class Entity {
    public:
        // Constructor / Destructor
        Entity(const std::string& name = "Entity");
        virtual ~Entity();

        // Unique ID
        uint64_t GetID() const { return m_id; }

        // Name
        void SetName(const std::string& name) { m_name = name; }
        const std::string& GetName() const { return m_name; }

        // Active flag
        void SetActive(bool active) { m_active = active; }
        bool IsActive() const { return m_active; }
       // In EntityManager.h:
        void Clear() { m_entities.clear(); }
        // Transform
        Transform& GetTransform() { return m_transform; }
        const Transform& GetTransform() const { return m_transform; }

        // Component management
        template<typename T, typename... Args>
        T* AddComponent(Args&&... args);

        template<typename T>
        T* GetComponent();

        template<typename T>
        void RemoveComponent();

        void RemoveAllComponents();

        // Called when entity is spawned (after components added)
        virtual void OnSpawn() {}
        virtual void OnDespawn() {}

        // Called every frame (if active)
        virtual void Update(float deltaTime) {}

        // Scene management (optional)
        void SetParent(Entity* parent);
        Entity* GetParent() const { return m_parent; }
        void AddChild(Entity* child);
        void RemoveChild(Entity* child);
        const std::vector<Entity*>& GetChildren() const { return m_children; }

    private:
        uint64_t            m_id;
        std::string         m_name;
        bool                m_active;
        Transform           m_transform;

        std::vector<std::unique_ptr<Component>> m_components;

        Entity*             m_parent;
        std::vector<Entity*> m_children;

        static uint64_t     s_nextId;

        friend class EntityManager;
    };

} // namespace USE