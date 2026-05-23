// ============================================================
// Ultimate Source Engine - Transform Component
// ============================================================
//
// Component that holds the local transform (position, rotation, scale)
// of an entity. Supports hierarchical transformations via parent‑child
// relationships.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Entity/Component.h"
#include "Math/Transform.h"

namespace USE {

    class TransformComponent : public Component {
    public:
        TransformComponent();
        virtual ~TransformComponent();

        // Local transform (relative to parent)
        Transform localTransform;

        // Cached world transform (absolute)
        Transform worldTransform;

        // Hierarchy
        TransformComponent* parent;
        std::vector<TransformComponent*> children;

        // Set parent (automatically updates hierarchy)
        void SetParent(TransformComponent* newParent);

        // Add/remove child (manages parent pointer)
        void AddChild(TransformComponent* child);
        void RemoveChild(TransformComponent* child);

        // Update world transform based on local and parent's world transform.
        // Call when local transform or parent changes.
        void UpdateWorldTransform();

        // Get type name for debugging/serialization
        virtual const char* GetTypeName() const override { return "TransformComponent"; }
    };

} // namespace USE