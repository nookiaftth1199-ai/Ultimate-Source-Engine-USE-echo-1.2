// ============================================================
// Ultimate Source Engine - Physics System Interface
// ============================================================
// Abstract interface for the physics backend.
// All physics implementations (Jolt, Bullet, Havok, etc.)
// must derive from this class.
// ============================================================

#pragma once

#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include <cstdint>
#include <vector>
#include <string>

namespace USE
{
	// -----------------------------------------------------------------
	// Enums
	// -----------------------------------------------------------------
	enum class CollisionShapeType
	{
		Box,
		Sphere,
		Capsule,
		ConvexHull,
		TriangleMesh,
		Compound,
	};

	enum class JointType
	{
		Fixed,
		Hinge,
		Slider,
		BallAndSocket,
		Distance,
	};

	// -----------------------------------------------------------------
	// Descriptor structures
	// -----------------------------------------------------------------
	struct RigidBodyDesc
	{
		Vector3     position = { 0, 0, 0 };
		Quaternion  rotation = Quaternion::Identity();
		float       mass = 1.0f;
		bool        isStatic = false;
		float       friction = 0.5f;
		float       restitution = 0.0f;
		float       linearDamping = 0.0f;
		float       angularDamping = 0.0f;

		// Collision shape description.
		CollisionShapeType shapeType = CollisionShapeType::Box;
		Vector3  halfExtents = { 1, 1, 1 };   // for Box
		float    radius = 1.0f;           // for Sphere / Capsule
		float    height = 1.0f;           // for Capsule
		// For convex hull / trimesh, the actual data is stored elsewhere (e.g. a shared shape resource).
	};

	struct CharacterControllerDesc
	{
		Vector3  position = { 0, 0, 0 };
		float    radius = 0.5f;
		float    height = 1.8f;
		float    stepHeight = 0.3f;
	};

	struct VehicleDesc
	{
		// Basic vehicle params (placeholder for a full vehicle setup).
		std::string type;    // "wheeled", "tracked", etc.
		// Additional data would be filled by the implementation.
	};

	struct JointDesc
	{
		JointType  type = JointType::Fixed;
		uint32_t   bodyA = 0;
		uint32_t   bodyB = 0;
		Vector3    pivotA = { 0, 0, 0 };
		Vector3    pivotB = { 0, 0, 0 };
		Vector3    axisA = { 1, 0, 0 };
		Vector3    axisB = { 1, 0, 0 };
		float      minLimit = 0.0f;
		float      maxLimit = 0.0f;
	};

	struct RaycastResult
	{
		bool     hit = false;
		Vector3  position;
		Vector3  normal;
		float    distance = 0.0f;
		uint32_t bodyHandle = 0;
	};

	// -----------------------------------------------------------------
	// Pure virtual interface
	// -----------------------------------------------------------------
	class IPhysicsSystem
	{
	public:
		virtual ~IPhysicsSystem() = default;

		// Initialise / shutdown the physics world.
		virtual bool Initialize() = 0;
		virtual void Shutdown() = 0;

		// Advance the simulation by deltaTime seconds.
		virtual void Update(float deltaTime) = 0;

		// --- Rigid bodies ---
		virtual uint32_t CreateRigidBody(const RigidBodyDesc& desc) = 0;
		virtual void DestroyRigidBody(uint32_t handle) = 0;

		virtual void SetRigidBodyTransform(uint32_t handle, const Vector3& pos, const Quaternion& rot) = 0;
		virtual void GetRigidBodyTransform(uint32_t handle, Vector3& pos, Quaternion& rot) const = 0;

		virtual void ApplyForceToRigidBody(uint32_t handle, const Vector3& force) = 0;
		virtual void SetRigidBodyLinearVelocity(uint32_t handle, const Vector3& vel) = 0;

		// --- Character controllers ---
		virtual uint32_t CreateCharacterController(const CharacterControllerDesc& desc) = 0;
		virtual void DestroyCharacterController(uint32_t handle) = 0;

		// --- Vehicles ---
		virtual uint32_t CreateVehicle(const VehicleDesc& desc) = 0;
		virtual void DestroyVehicle(uint32_t handle) = 0;

		// --- Joints ---
		virtual uint32_t CreateJoint(const JointDesc& desc) = 0;
		virtual void DestroyJoint(uint32_t handle) = 0;

		// --- Raycasting ---
		virtual bool Raycast(const Vector3& origin, const Vector3& direction,
			float maxDist, RaycastResult& outResult) = 0;

		// --- Global physics settings ---
		virtual void SetGravity(const Vector3& gravity) = 0;
		virtual Vector3 GetGravity() const = 0;

		// --- Debug rendering ---
		virtual void DebugDraw() {}

		// --- Memory statistics ---
		virtual uint32_t GetBodyCount() const { return 0; }
	};
}