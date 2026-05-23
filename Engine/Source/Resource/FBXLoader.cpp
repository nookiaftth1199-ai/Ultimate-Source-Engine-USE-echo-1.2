#pragma once
#include "ResourceLoader.h"

class FBXLoader : public IModelLoader {
public:
    bool LoadModel(const char* path, MeshData& outMesh) override {
        // Using FBX SDK 2015 (compatible with VC++ 2010)
        FbxManager* manager = FbxManager::Create();
        FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
        manager->SetIOSettings(ios);
        
        FbxImporter* importer = FbxImporter::Create(manager, "");
        if(!importer->Initialize(path, -1, manager->GetIOSettings())) {
            return false;
        }
        
        FbxScene* scene = FbxScene::Create(manager, "scene");
        importer->Import(scene);
        
        // Process mesh data, materials, animations
        ProcessNode(scene->GetRootNode(), outMesh);
        
        importer->Destroy();
        manager->Destroy();
        return true;
    }
    
private:
    void ProcessNode(FbxNode* node, MeshData& mesh) {
        // Extract vertices, normals, UVs, materials
    }
};