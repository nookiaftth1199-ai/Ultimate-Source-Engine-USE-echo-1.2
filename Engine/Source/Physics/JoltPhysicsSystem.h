// Engine/Physics/Jolt/JoltPhysicsSystem.h
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>

class JoltPhysicsSystem : public IPhysicsSystem {
    JPH::TempAllocator* m_tempAllocator;
    JPH::JobSystem* m_jobSystem;
    JPH::PhysicsSystem* m_physicsSystem;

    std::unordered_map<uint32_t, JPH::BodyID> m_bodies;
    uint32_t m_nextBodyId = 1;

public:
    bool Initialize(const PhysicsWorldSettings& settings) override;
    void Shutdown() override;
    void Update(float deltaTime) override;

    // ... methods ...
};