// ============================================================
// Ultimate Source Engine - Entity Implementation
// ============================================================

#include "stdafx.h"
#include "Entity.h"
#include "Component.h"

namespace USE {

    uint64_t Entity::s_nextId = 1;

    Entity::Entity(const std::string& name)
        : m_id(s_nextId++)
        , m_name(name)
        , m_active(true)
        , m_parent(nullptr)
    {
    }

    Entity::~Entity()
    {
        RemoveAllComponents();
    }

    void Entity::SetParent(Entity* parent)
    {
        if (m_parent) {
            // Remove from old parent's children
            auto& siblings = m_parent->m_children;
            siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
        }
        m_parent = parent;
        if (parent) {
            parent->m_children.push_back(this);
        }
    }

    void Entity::AddChild(Entity* child)
    {
        if (child && child->m_parent != this) {
            child->SetParent(this);
        }
    }

    void Entity::RemoveChild(Entity* child)
    {
        if (child && child->m_parent == this) {
            child->SetParent(nullptr);
        }
    }

    template<typename T, typename... Args>
    T* Entity::AddComponent(Args&&... args)
    {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
        auto comp = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw = comp.get();
        raw->SetOwner(this);
        m_components.push_back(std::move(comp));
        return raw;
    }

    template<typename T>
    T* Entity::GetComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
        for (auto& comp : m_components) {
            T* typed = dynamic_cast<T*>(comp.get());
            if (typed) return typed;
        }
        return nullptr;
    }

    template<typename T>
    void Entity::RemoveComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
        auto it = std::remove_if(m_components.begin(), m_components.end(),
            [](const std::unique_ptr<Component>& comp) {
                return dynamic_cast<T*>(comp.get()) != nullptr;
            });
        m_components.erase(it, m_components.end());
    }

    void Entity::RemoveAllComponents()
    {
        m_components.clear();
    }

    // Explicit template instantiations for common types
    // (You may need to add these for specific component types)

} // namespace USE