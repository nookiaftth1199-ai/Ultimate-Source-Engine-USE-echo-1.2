// ============================================================
// Ultimate Source Engine – Bullet Physics Backend
// ============================================================
#pragma once

#include "IPhysicsSystem.h"
#include <vector>
#include <unordered_map>
#include <memory>

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btRigidBody;
class btCollisionShape;

namespace USE
{
	class BulletPhysicsSystem : public IPhysicsSystem
	{
	public:
		BulletPhysicsSystem();
		~BulletPhysicsSystem() override;

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
		std::unique_ptr<btDefaultCollisionConfiguration>  m_collisionConfig;
		std::unique_ptr<btCollisionDispatcher>             m_dispatcher;
		std::unique_ptr<btBroadphaseInterface>             m_broadphase;
		std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;
		std::unique_ptr<btDiscreteDynamicsWorld>            m_world;

		std::vector<btRigidBody*> m_bodies;   // index = handle
		uint32_t m_nextHandle = 1;

		btCollisionShape* CreateShape(const RigidBodyDesc& desc);
	};
}