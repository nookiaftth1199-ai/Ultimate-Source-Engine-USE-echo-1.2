// ============================================================
// Ultimate Source Engine - Transform Component Implementation
// ============================================================

#include "stdafx.h"
#include "TransformComponent.h"

namespace USE {

    TransformComponent::TransformComponent()
        : parent(nullptr)
    {
        worldTransform = localTransform; // initially same
    }

    TransformComponent::~TransformComponent()
    {
        // Remove from parent if any
        if (parent) {
            parent->RemoveChild(this);
        }
        // Orphan children
        for (auto* child : children) {
            child->parent = nullptr;
        }
        children.clear();
    }

    void TransformComponent::SetParent(TransformComponent* newParent)
    {
        if (parent == newParent) return;
        if (parent) {
            parent->RemoveChild(this);
        }
        parent = newParent;
        if (parent) {
            parent->AddChild(this);
        }
        UpdateWorldTransform();
    }

    void TransformComponent::AddChild(TransformComponent* child)
    {
        if (!child) return;
        // Avoid duplicates (should not happen if used correctly)
        for (auto* c : children) {
            if (c == child) return;
        }
        children.push_back(child);
        if (child->parent != this) {
            child->parent = this;
        }
        child->UpdateWorldTransform();
    }

    void TransformComponent::RemoveChild(TransformComponent* child)
    {
        if (!child) return;
        auto it = std::find(children.begin(), children.end(), child);
        if (it != children.end()) {
            children.erase(it);
            if (child->parent == this) {
                child->parent = nullptr;
            }
            child->UpdateWorldTransform();
        }
    }

    void TransformComponent::UpdateWorldTransform()
    {
        if (parent) {
            worldTransform = parent->worldTransform * localTransform;
        } else {
            worldTransform = localTransform;
        }
        // Recursively update children
        for (auto* child : children) {
            child->UpdateWorldTransform();
        }
    }

} // namespace USE