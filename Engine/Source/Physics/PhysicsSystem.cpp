// ============================================================
// Ultimate Source Engine – Physics System Implementation
// ============================================================

#include "stdafx.h"
#include "PhysicsSystem.h"
#include "JoltPhysicsSystem.h"
#include "BulletPhysicsSystem.h"
#include "Core/Logger.h"

namespace USE
{
	PhysicsSystem::PhysicsSystem()
	{
		// Default to Jolt (or Bullet if Jolt not available)
#ifdef USE_JOLT
		m_backend = std::make_unique<JoltPhysicsSystem>();
		m_currentBackend = "jolt";
#elif defined(USE_BULLET)
		m_backend = std::make_unique<BulletPhysicsSystem>();
		m_currentBackend = "bullet";
#else
		m_currentBackend = "none";
#endif
	}

	PhysicsSystem::~PhysicsSystem() { Shutdown(); }

	bool PhysicsSystem::Initialize()
	{
		if (!m_backend)
		{
			USE_LOG_ERROR("PhysicsSystem: No backend available.");
			return false;
		}
		return m_backend->Initialize();
	}

	void PhysicsSystem::Shutdown()
	{
		if (m_backend) m_backend->Shutdown();
	}

	void PhysicsSystem::Update(float deltaTime)
	{
		if (m_backend) m_backend->Update(deltaTime);
	}

	uint32_t PhysicsSystem::CreateRigidBody(const RigidBodyDesc& desc)
	{
		return m_backend ? m_backend->CreateRigidBody(desc) : 0;
	}

	void PhysicsSystem::DestroyRigidBody(uint32_t handle)
	{
		if (m_backend) m_backend->DestroyRigidBody(handle);
	}

	bool PhysicsSystem::Raycast(const Vector3& origin, const Vector3& direction, float maxDist,
		RaycastResult& outResult)
	{
		return m_backend ? m_backend->Raycast(origin, direction, maxDist, outResult) : false;
	}

	void PhysicsSystem::SetGravity(const Vector3& gravity)
	{
		if (m_backend) m_backend->SetGravity(gravity);
	}

	Vector3 PhysicsSystem::GetGravity() const
	{
		return m_backend ? m_backend->GetGravity() : Vector3(0, -9.81f, 0);
	}

	bool PhysicsSystem::SetBackend(const std::string& name)
	{
		if (name == "jolt")
		{
#ifdef USE_JOLT
			m_backend = std::make_unique<JoltPhysicsSystem>();
#else
			USE_LOG_WARN("PhysicsSystem: Jolt not compiled in.");
			return false;
#endif
		}
		else if (name == "bullet")
		{
#ifdef USE_BULLET
			m_backend = std::make_unique<BulletPhysicsSystem>();
#else
			USE_LOG_WARN("PhysicsSystem: Bullet not compiled in.");
			return false;
#endif
		}
		else
		{
			USE_LOG_WARN("PhysicsSystem: Unknown backend '%s'.", name.c_str());
			return false;
		}

		if (m_backend)
		{
			m_backend->Shutdown();
			m_backend.reset();
		}
		if (!m_backend->Initialize()) return false;
		m_currentBackend = name;
		USE_LOG_INFO("PhysicsSystem: Switched to backend '%s'.", name.c_str());
		return true;
	}
}