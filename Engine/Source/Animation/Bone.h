// ============================================================
// Ultimate Source Engine - Bone
// ============================================================
#pragma once

#include <string>
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

namespace USE
{
	struct Bone
	{
		std::string name;
		int32_t     parentIndex = -1;   // -1 = root
		Vector3     bindPosition;
		Quaternion  bindRotation;
		// Optional: inverse bind matrix (can be computed later)
	};
}