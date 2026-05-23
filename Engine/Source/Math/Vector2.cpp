// ============================================================
// Ultimate Source Engine - Vector2 (static constant definitions)
// ============================================================

#include "stdafx.h"
#include "Vector2.h"

namespace USE {

    // Define static constants
    const Vector2 Vector2::Zero(0.0f, 0.0f);
    const Vector2 Vector2::One(1.0f, 1.0f);
    const Vector2 Vector2::Right(1.0f, 0.0f);
    const Vector2 Vector2::Left(-1.0f, 0.0f);
    const Vector2 Vector2::Up(0.0f, 1.0f);
    const Vector2 Vector2::Down(0.0f, -1.0f);

} // namespace USE