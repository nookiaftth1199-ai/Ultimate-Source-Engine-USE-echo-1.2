#include "stdafx.h"
#include "BulletWrapper.h"
#include "Core/Logger.h"

// #define USE_BULLET

#ifdef USE_BULLET
#include <btBulletDynamicsCommon.h>
#endif

namespace USE
{
	BulletWrapper::BulletWrapper() = default;
	BulletWrapper::~BulletWrapper() { Shutdown(); }

	bool BulletWrapper::Init()
	{
#ifdef USE_BULLET
		m_collisionConfig = new btDefaultCollisionConfiguration();
		m_dispatcher = new btCollisionDispatcher(m_collisionConfig);
		m_broadphase = new btDbvtBroadphase();
		m_solver = new btSequentialImpulseConstraintSolver();
		m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfig);
		m_world->setGravity(btVector3(0, -9.81f, 0));
		return true;
#else
		USE_LOG_WARN("BulletWrapper: Bullet not enabled. Define USE_BULLET to enable.");
		return false;
#endif
	}

	void BulletWrapper::Step(float dt)
	{
#ifdef USE_BULLET
		if (m_world) m_world->stepSimulation(dt);
#endif
	}

	void BulletWrapper::Shutdown()
	{
#ifdef USE_BULLET
		delete m_world; m_world = nullptr;
		delete m_solver; m_solver = nullptr;
		delete m_broadphase; m_broadphase = nullptr;
		delete m_dispatcher; m_dispatcher = nullptr;
		delete m_collisionConfig; m_collisionConfig = nullptr;
#endif
	}
}