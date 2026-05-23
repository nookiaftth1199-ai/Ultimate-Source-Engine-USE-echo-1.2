// Engine/Physics/PhysX/PhysXPhysicsSystem.h
#include "PxPhysicsAPI.h"

class PhysXPhysicsSystem : public IPhysicsSystem {
    physx::PxFoundation* m_foundation;
    physx::PxPhysics* m_physics;
    physx::PxScene* m_scene;
    physx::PxDefaultAllocator m_allocator;
    physx::PxDefaultErrorCallback m_errorCallback;

    std::unordered_map<uint32_t, physx::PxRigidActor*> m_bodies;
    uint32_t m_nextBodyId = 1;

public:
    bool Initialize(const PhysicsWorldSettings& settings) override;
    void Shutdown() override;
    void Update(float deltaTime) override;

    // ... methods ...
};