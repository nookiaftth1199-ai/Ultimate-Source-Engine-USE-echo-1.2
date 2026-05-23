// ============================================================
// BulletWrapper.cpp
// ============================================================
#include "BulletWrapper.h"
#include "Core/Logger.h"

namespace USE {
    static btDefaultCollisionConfiguration* s_collisionConfig = nullptr;
    static btCollisionDispatcher* s_dispatcher = nullptr;
    static btDbvtBroadphase* s_broadphase = nullptr;
    static btSequentialImpulseConstraintSolver* s_solver = nullptr;

    bool BulletWrapper::Initialize() {
        s_collisionConfig = new btDefaultCollisionConfiguration();
        s_dispatcher = new btCollisionDispatcher(s_collisionConfig);
        s_broadphase = new btDbvtBroadphase();
        s_solver = new btSequentialImpulseConstraintSolver();
        USE_LOG_INFO("Bullet physics initialized");
        return true;
    }

    void BulletWrapper::Shutdown() {
        delete s_solver; s_solver = nullptr;
        delete s_broadphase; s_broadphase = nullptr;
        delete s_dispatcher; s_dispatcher = nullptr;
        delete s_collisionConfig; s_collisionConfig = nullptr;
        USE_LOG_INFO("Bullet physics shut down");
    }

    btDiscreteDynamicsWorld* BulletWrapper::CreateDynamicsWorld() {
        return new btDiscreteDynamicsWorld(s_dispatcher, s_broadphase, s_solver, s_collisionConfig);
    }

    void BulletWrapper::DestroyDynamicsWorld(btDiscreteDynamicsWorld* world) {
        delete world;
    }
}