// ============================================================
// BulletWrapper.h
// ============================================================
#pragma once
#include <btBulletDynamicsCommon.h>

namespace USE {
    class BulletWrapper {
    public:
        static bool Initialize();
        static void Shutdown();
        static btDiscreteDynamicsWorld* CreateDynamicsWorld();
        static void DestroyDynamicsWorld(btDiscreteDynamicsWorld* world);
    };
}