// Engine/Physics/Bullet/BulletPhysicsSystem.h
#include "btBulletDynamicsCommon.h"

class BulletPhysicsSystem : public IPhysicsSystem {
    btBroadphaseInterface* m_broadphase;
    btDefaultCollisionConfiguration* m_collisionConfig;
    btCollisionDispatcher* m_dispatcher;
    btSequentialImpulseConstraintSolver* m_solver;
    btDiscreteDynamicsWorld* m_world;

    std::unordered_map<uint32_t, btRigidBody*> m_bodies;
    uint32_t m_nextBodyId = 1;

public:
    bool Initialize(const PhysicsWorldSettings& settings) override;
    void Shutdown() override;
    void Update(float deltaTime) override;

    uint32_t CreateRigidBody(const RigidBodyDesc& desc,
                              const CollisionShape& shape) override;
    // ... other methods ...
};