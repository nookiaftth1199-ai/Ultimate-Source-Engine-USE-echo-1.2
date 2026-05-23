// ============================================================
// AssimpWrapper.h
// ============================================================
#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace USE {
    class AssimpWrapper {
    public:
        static Assimp::Importer* CreateImporter();
        static void DestroyImporter(Assimp::Importer* importer);
    };
}