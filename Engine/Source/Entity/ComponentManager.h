// ============================================================
// Ultimate Source Engine - Component Manager
// ============================================================
//
// Manages component pools for a data‑oriented entity‑component system.
// Each component type is stored in a contiguous array (pool) for cache‑efficient
// iteration. Entities are identified by a simple integer ID.
//
// Example usage:
//   ComponentManager cm;
//   auto entity = cm.CreateEntity();
//   cm.AddComponent<Transform>(entity, position, rotation, scale);
//   auto* transform = cm.GetComponent<Transform>(entity);
//   for (auto [e, t] : cm.View<Transform>()) { ... }
// ============================================================

#pragma once

#include "stdafx.h"
#include <unordered_map>
#include <vector>
#include <tuple>
#include <typeindex>
#include <memory>
#include <cstdint>

namespace USE {

    // -----------------------------------------------------------------
    // Component pool base (type‑erased interface)
    // -----------------------------------------------------------------
    class IComponentPool {
    public:
        virtual ~IComponentPool() = default;
        virtual void EntityDestroyed(uint32_t entity) = 0;
    };

    // -----------------------------------------------------------------
    // Typed component pool
    // -----------------------------------------------------------------
    template<typename T>
    class ComponentPool : public IComponentPool {
    public:
        ComponentPool() = default;

        // Add a component to an entity (if it already exists, it is replaced)
        template<typename... Args>
        T& AddComponent(uint32_t entity, Args&&... args) {
            auto it = m_entityToIndex.find(entity);
            if (it != m_entityToIndex.end()) {
                // Replace existing
                uint32_t index = it->second;
                m_components[index] = T(std::forward<Args>(args)...);
                return m_components[index];
            } else {
                // Add new
                uint32_t index = static_cast<uint32_t>(m_components.size());
                m_components.emplace_back(std::forward<Args>(args)...);
                m_entityToIndex[entity] = index;
                m_indexToEntity.push_back(entity);
                return m_components.back();
            }
        }

        // Remove component from entity
        void RemoveComponent(uint32_t entity) {
            auto it = m_entityToIndex.find(entity);
            if (it == m_entityToIndex.end()) return;

            uint32_t index = it->second;
            uint32_t lastIndex = static_cast<uint32_t>(m_components.size()) - 1;
            uint32_t lastEntity = m_indexToEntity.back();

            // Swap with last element
            m_components[index] = std::move(m_components[lastIndex]);
            m_indexToEntity[index] = lastEntity;
            m_entityToIndex[lastEntity] = index;

            m_components.pop_back();
            m_indexToEntity.pop_back();
            m_entityToIndex.erase(it);
        }

        // Get component pointer (nullptr if entity doesn't have this component)
        T* GetComponent(uint32_t entity) {
            auto it = m_entityToIndex.find(entity);
            return (it != m_entityToIndex.end()) ? &m_components[it->second] : nullptr;
        }

        // Called when an entity is destroyed
        void EntityDestroyed(uint32_t entity) override {
            RemoveComponent(entity);
        }

        // Iteration support (view over all entities with this component)
        struct ViewIterator {
            using iterator_category = std::forward_iterator_tag;
            using value_type = std::pair<uint32_t, T&>;
            using difference_type = ptrdiff_t;
            using pointer = void;
            using reference = value_type;

            std::vector<T>* components;
            std::vector<uint32_t>* entities;
            size_t index;

            ViewIterator(std::vector<T>* comps, std::vector<uint32_t>* ents, size_t idx)
                : components(comps), entities(ents), index(idx) {}

            value_type operator*() const { return {(*entities)[index], (*components)[index]}; }
            ViewIterator& operator++() { ++index; return *this; }
            bool operator!=(const ViewIterator& other) const { return index != other.index; }
        };

        ViewIterator begin() { return ViewIterator(&m_components, &m_indexToEntity, 0); }
        ViewIterator end()   { return ViewIterator(&m_components, &m_indexToEntity, m_components.size()); }

        size_t Size() const { return m_components.size(); }

    private:
        std::vector<T>           m_components;        // contiguous component storage
        std::vector<uint32_t>    m_indexToEntity;     // entity ID for each index
        std::unordered_map<uint32_t, uint32_t> m_entityToIndex; // mapping entity -> index
    };

    // -----------------------------------------------------------------
    // Main Component Manager
    // -----------------------------------------------------------------
    class ComponentManager {
    public:
        ComponentManager();
        ~ComponentManager() = default;

        // Create a new entity and return its ID
        uint32_t CreateEntity();

        // Destroy an entity (removes all its components)
        void DestroyEntity(uint32_t entity);

        // Check if an entity is still alive
        bool IsAlive(uint32_t entity) const;

        // Add a component to an entity
        template<typename T, typename... Args>
        T& AddComponent(uint32_t entity, Args&&... args);

        // Remove a component from an entity
        template<typename T>
        void RemoveComponent(uint32_t entity);

        // Get component (returns nullptr if entity doesn't have it)
        template<typename T>
        T* GetComponent(uint32_t entity);

        // View over all entities that have a set of components
        template<typename... Ts>
        class View {
            // Not implemented in this simplified version; you can add a complex iterator later.
        };

    private:
        std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> m_pools;
        std::vector<bool> m_entityAlive;      // sparse array of alive entities
        std::vector<uint32_t> m_freeEntities; // recycled IDs
        uint32_t m_nextEntityId;

        template<typename T>
        ComponentPool<T>* GetPool() {
            auto it = m_pools.find(typeid(T));
            if (it == m_pools.end()) {
                auto pool = std::make_unique<ComponentPool<T>>();
                ComponentPool<T>* raw = pool.get();
                m_pools[typeid(T)] = std::move(pool);
                return raw;
            }
            return static_cast<ComponentPool<T>*>(it->second.get());
        }
    };

    // -----------------------------------------------------------------
    // Template implementations
    // -----------------------------------------------------------------
    template<typename T, typename... Args>
    T& ComponentManager::AddComponent(uint32_t entity, Args&&... args)
    {
        if (!IsAlive(entity)) {
            throw std::runtime_error("Attempted to add component to dead entity");
        }
        return GetPool<T>()->AddComponent(entity, std::forward<Args>(args)...);
    }

    template<typename T>
    void ComponentManager::RemoveComponent(uint32_t entity)
    {
        if (!IsAlive(entity)) return;
        auto pool = GetPool<T>();
        pool->RemoveComponent(entity);
    }

    template<typename T>
    T* ComponentManager::GetComponent(uint32_t entity)
    {
        if (!IsAlive(entity)) return nullptr;
        return GetPool<T>()->GetComponent(entity);
    }

} // namespace USE