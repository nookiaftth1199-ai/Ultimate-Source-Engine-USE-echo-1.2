// ============================================================
// Ultimate Source Engine - Component Implementation
// ============================================================

#include "stdafx.h"
#include "Component.h"

namespace USE {

    Component::Component()
        : m_owner(nullptr)
        , m_active(true)
    {
    }

    Component::~Component()
    {
    }

} // namespace USE