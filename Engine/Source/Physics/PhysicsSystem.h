// ============================================================
// Ultimate Source Engine – Physics System
// ============================================================
#pragma once

#include "IPhysicsSystem.h"
#include <memory>
#include <string>

namespace USE
{
	class PhysicsSystem
	{
	public:
		PhysicsSystem();
		~PhysicsSystem();

		bool Initialize();
		void Shutdown();
		void Update(float deltaTime);

		// Access the underlying physics world (or delegate calls)
		uint32_t CreateRigidBody(const RigidBodyDesc& desc);
		void DestroyRigidBody(uint32_t handle);
		bool Raycast(const Vector3& origin, const Vector3& direction, float maxDist,
			RaycastResult& outResult);
		void SetGravity(const Vector3& gravity);
		Vector3 GetGravity() const;

		// Switch backend at runtime (e.g., "jolt" or "bullet")
		bool SetBackend(const std::string& name);

	private:
		std::unique_ptr<IPhysicsSystem> m_backend;
		std::string m_currentBackend;
	};
}