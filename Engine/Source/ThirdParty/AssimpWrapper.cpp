// ============================================================
// AssimpWrapper.cpp
// ============================================================
#include "AssimpWrapper.h"
#include "Core/Logger.h"

namespace USE {
    Assimp::Importer* AssimpWrapper::CreateImporter() {
        return new Assimp::Importer();
    }

    void AssimpWrapper::DestroyImporter(Assimp::Importer* importer) {
        delete importer;
    }
}