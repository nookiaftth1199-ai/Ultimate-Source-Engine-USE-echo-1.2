// ============================================================
// Ultimate Source Engine – Jolt Physics Backend
// ============================================================
#pragma once

#include "IPhysicsSystem.h"
#include <vector>
#include <memory>
#include <unordered_map>

// Forward-declare Jolt types (actual headers included only in .cpp)
namespace JPH
{
	class PhysicsSystem;
	class TempAllocator;
	class JobSystem;
	class BodyInterface;
	class Body;
}

namespace USE
{
	class JoltPhysicsSystem : public IPhysicsSystem
	{
	public:
		JoltPhysicsSystem();
		~JoltPhysicsSystem() override;

		bool Initialize() override;
		void Shutdown() override;
		void Update(float deltaTime) override;

		uint32_t CreateRigidBody(const RigidBodyDesc& desc) override;
		void DestroyRigidBody(uint32_t handle) override;

		uint32_t CreateCharacterController(const CharacterControllerDesc& desc) override;
		void DestroyCharacterController(uint32_t handle) override;

		uint32_t CreateVehicle(const VehicleDesc& desc) override;
		void DestroyVehicle(uint32_t handle) override;

		uint32_t CreateJoint(const JointDesc& desc) override;
		void DestroyJoint(uint32_t handle) override;

		bool Raycast(const Vector3& origin, const Vector3& direction, float maxDist,
			RaycastResult& outResult) override;

		void SetRigidBodyTransform(uint32_t handle, const Vector3& pos, const Quaternion& rot) override;
		void GetRigidBodyTransform(uint32_t handle, Vector3& pos, Quaternion& rot) const override;

		void ApplyForceToRigidBody(uint32_t handle, const Vector3& force) override;
		void SetRigidBodyLinearVelocity(uint32_t handle, const Vector3& vel) override;

		void SetGravity(const Vector3& gravity) override;
		Vector3 GetGravity() const override;

	private:
		// Jolt objects (only created when library is available)
		std::unique_ptr<JPH::PhysicsSystem>  m_physicsSystem;
		std::unique_ptr<JPH::TempAllocator>  m_tempAllocator;
		std::unique_ptr<JPH::JobSystem>      m_jobSystem;
		JPH::BodyInterface*                  m_bodyInterface = nullptr;

		// Handle management
		std::unordered_map<uint32_t, JPH::Body*> m_bodies;
		uint32_t m_nextHandle = 1;
	};
}