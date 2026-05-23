// ============================================================
// Ultimate Source Engine - Scene Loader Implementation
// ============================================================

#include "stdafx.h"
#include "SceneLoader.h"
#include "Scene.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"
#include "Resources/ResourceManager.h"
#include "Entity/Entity.h"
#include "Entity/EntityManager.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/RenderComponent.h"
#include "Entity/Components/LightComponent.h"
#include "Entity/Components/CameraComponent.h"

// JSON library
#include "json.hpp"
using json = nlohmann::json;

namespace USE {

    // -----------------------------------------------------------------
    // Helper: parse Vector3 from JSON array
    // -----------------------------------------------------------------
    static bool ParseVector3(const json& j, Vector3& out)
    {
        if (!j.is_array() || j.size() < 3) return false;
        out.x = j[0].get<float>();
        out.y = j[1].get<float>();
        out.z = j[2].get<float>();
        return true;
    }

    // -----------------------------------------------------------------
    // Helper: parse Quaternion from JSON array (x,y,z,w)
    // -----------------------------------------------------------------
    static bool ParseQuaternion(const json& j, Quaternion& out)
    {
        if (!j.is_array() || j.size() < 4) return false;
        out.x = j[0].get<float>();
        out.y = j[1].get<float>();
        out.z = j[2].get<float>();
        out.w = j[3].get<float>();
        return true;
    }

    // -----------------------------------------------------------------
    // Helper: parse Color from JSON array (r,g,b,a)
    // -----------------------------------------------------------------
    static bool ParseColor(const json& j, Color& out)
    {
        if (!j.is_array() || j.size() < 4) return false;
        out.r = j[0].get<float>();
        out.g = j[1].get<float>();
        out.b = j[2].get<float>();
        out.a = j[3].get<float>();
        return true;
    }

    // -----------------------------------------------------------------
    // Load entity from JSON
    // -----------------------------------------------------------------
    static bool LoadEntity(const json& j,
                           ResourceManager* resourceManager,
                           EntityManager* entityManager)
    {
        std::string name = j.value("name", "Entity");
        Entity* entity = entityManager->CreateEntity(name);

        // Process components
        if (j.contains("components") && j["components"].is_array()) {
            for (const auto& compJson : j["components"]) {
                if (!compJson.contains("type")) continue;
                std::string type = compJson["type"];

                if (type == "Transform") {
                    TransformComponent* tc = entity->AddComponent<TransformComponent>();
                    if (compJson.contains("position")) {
                        Vector3 pos;
                        if (ParseVector3(compJson["position"], pos))
                            tc->localTransform.translation = pos;
                    }
                    if (compJson.contains("rotation")) {
                        Quaternion rot;
                        if (ParseQuaternion(compJson["rotation"], rot))
                            tc->localTransform.rotation = rot;
                    }
                    if (compJson.contains("scale")) {
                        Vector3 scale;
                        if (ParseVector3(compJson["scale"], scale))
                            tc->localTransform.scale = scale;
                    }
                }
                else if (type == "Render") {
                    RenderComponent* rc = entity->AddComponent<RenderComponent>();
                    if (compJson.contains("mesh")) {
                        std::string meshPath = compJson["mesh"];
                        Mesh* mesh = nullptr; // need to load mesh via resource manager
                        // TODO: resourceManager->LoadMesh(meshPath);
                        // For now, just log.
                        USE_LOG_WARN("SceneLoader: Mesh loading not implemented yet: %s", meshPath.c_str());
                    }
                    if (compJson.contains("material")) {
                        std::string matPath = compJson["material"];
                        Material* mat = resourceManager->LoadMaterial(matPath); // need LoadMaterial
                        if (mat) rc->SetMaterial(mat);
                    }
                    rc->SetCastShadows(compJson.value("castShadows", true));
                    rc->SetReceiveShadows(compJson.value("receiveShadows", true));
                    rc->SetVisible(compJson.value("visible", true));
                }
                else if (type == "Light") {
                    LightComponent* lc = entity->AddComponent<LightComponent>();
                    std::string lightType = compJson.value("lightType", "Directional");
                    if (lightType == "Point") lc->SetType(LightType::Point);
                    else if (lightType == "Spot") lc->SetType(LightType::Spot);
                    else lc->SetType(LightType::Directional);

                    if (compJson.contains("color")) {
                        Color col;
                        if (ParseColor(compJson["color"], col))
                            lc->SetColor(col);
                    }
                    lc->SetIntensity(compJson.value("intensity", 1.0f));
                    lc->SetRange(compJson.value("range", 100.0f));
                    lc->SetCastShadows(compJson.value("castShadows", false));

                    // Attenuation
                    if (compJson.contains("attenuation")) {
                        const auto& att = compJson["attenuation"];
                        float constant = att.value("constant", 1.0f);
                        float linear = att.value("linear", 0.09f);
                        float quadratic = att.value("quadratic", 0.032f);
                        lc->SetAttenuation(constant, linear, quadratic);
                    }

                    // Spot angles
                    if (compJson.contains("innerAngle") && compJson.contains("outerAngle")) {
                        lc->SetSpotAngles(compJson["innerAngle"], compJson["outerAngle"]);
                    }
                }
                else if (type == "Camera") {
                    CameraComponent* cc = entity->AddComponent<CameraComponent>();
                    // Could set projection parameters if present
                }
                else {
                    USE_LOG_WARN("SceneLoader: Unknown component type '%s'", type.c_str());
                }
            }
        }

        return true;
    }

    // -----------------------------------------------------------------
    // Load scene from file into a new Scene object
    // -----------------------------------------------------------------
    Scene* SceneLoader::LoadFromFile(const std::string& filename,
                                     ResourceManager* resourceManager)
    {
        if (!resourceManager) {
            USE_LOG_ERROR("SceneLoader: ResourceManager is null");
            return nullptr;
        }

        // Resolve file path
        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("SceneLoader: FileSystem not available");
            return nullptr;
        }

        std::string resolved = fs->ResolvePath(filename);
        if (resolved.empty()) {
            USE_LOG_ERROR("SceneLoader: File not found: %s", filename.c_str());
            return nullptr;
        }

        // Read file
        auto file = fs->OpenFile(resolved, FILE_READ | FILE_TEXT);
        if (!file) {
            USE_LOG_ERROR("SceneLoader: Could not open file: %s", resolved.c_str());
            return nullptr;
        }

        std::string content;
        file->ReadAll(content);
        file->Close();

        // Parse JSON
        json j;
        try {
            j = json::parse(content);
        } catch (const std::exception& e) {
            USE_LOG_ERROR("SceneLoader: JSON parse error: %s", e.what());
            return nullptr;
        }

        // Create scene
        Scene* scene = new Scene();
        if (!scene->Initialize()) {
            delete scene;
            return nullptr;
        }

        // Set ambient light
        if (j.contains("ambientLight")) {
            Color ambient;
            if (ParseColor(j["ambientLight"], ambient)) {
                scene->SetAmbientLight(ambient);
            }
        }

        // Load skybox
        if (j.contains("skybox")) {
            std::string skyboxPath = j["skybox"];
            // In a real engine, load skybox via resource manager
            USE_LOG_INFO("SceneLoader: Skybox would be loaded from %s", skyboxPath.c_str());
        }

        // Load entities
        if (j.contains("entities") && j["entities"].is_array()) {
            for (const auto& entityJson : j["entities"]) {
                if (!LoadEntity(entityJson, resourceManager, scene->GetWorld()->GetEntityManager())) {
                    USE_LOG_WARN("SceneLoader: Failed to load entity, skipping.");
                }
            }
        }

        USE_LOG_INFO("SceneLoader: Successfully loaded scene from %s", filename.c_str());
        return scene;
    }

    // -----------------------------------------------------------------
    // Load scene into existing world
    // -----------------------------------------------------------------
    bool SceneLoader::LoadIntoWorld(const std::string& filename,
                                    ResourceManager* resourceManager,
                                    EntityManager* entityManager)
    {
        if (!resourceManager || !entityManager) {
            USE_LOG_ERROR("SceneLoader: ResourceManager or EntityManager is null");
            return false;
        }

        // Resolve file path
        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("SceneLoader: FileSystem not available");
            return false;
        }

        std::string resolved = fs->ResolvePath(filename);
        if (resolved.empty()) {
            USE_LOG_ERROR("SceneLoader: File not found: %s", filename.c_str());
            return false;
        }

        // Read file
        auto file = fs->OpenFile(resolved, FILE_READ | FILE_TEXT);
        if (!file) {
            USE_LOG_ERROR("SceneLoader: Could not open file: %s", resolved.c_str());
            return false;
        }

        std::string content;
        file->ReadAll(content);
        file->Close();

        // Parse JSON
        json j;
        try {
            j = json::parse(content);
        } catch (const std::exception& e) {
            USE_LOG_ERROR("SceneLoader: JSON parse error: %s", e.what());
            return false;
        }

        // Load entities only (ignore scene-specific settings)
        if (j.contains("entities") && j["entities"].is_array()) {
            for (const auto& entityJson : j["entities"]) {
                LoadEntity(entityJson, resourceManager, entityManager);
            }
        }

        return true;
    }

} // namespace USE