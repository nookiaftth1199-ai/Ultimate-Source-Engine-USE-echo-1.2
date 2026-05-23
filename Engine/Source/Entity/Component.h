// ============================================================
// Ultimate Source Engine - Component
// ============================================================
//
// Base class for all components that can be attached to an Entity.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>

namespace USE {

    // Forward declaration
    class Entity;

    class Component {
    public:
        // Constructor / Destructor
        Component();
        virtual ~Component();

        // Called when component is attached to an entity
        virtual void OnAttach() {}

        // Called when component is detached from an entity
        virtual void OnDetach() {}

        // Called every frame (if the owning entity is active)
        virtual void Update(float deltaTime) {}

        // Called after physics update (for late updates like camera follow)
        virtual void LateUpdate(float deltaTime) {}

        // Get the owning entity
        Entity* GetOwner() const { return m_owner; }

        // Check if component is active (owner must also be active for component to be updated)
        bool IsActive() const { return m_active; }
        void SetActive(bool active) { m_active = active; }

        // Get component type name (for debugging / serialization)
        virtual const char* GetTypeName() const = 0;

    protected:
        // Called by Entity when attaching
        void SetOwner(Entity* owner) { m_owner = owner; }

    private:
        Entity* m_owner;
        bool    m_active;

        friend class Entity; // so Entity can call SetOwner
    };

} // namespace USE