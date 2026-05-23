// ============================================================
// Ultimate Source Engine - Scene Serializer Implementation
// ============================================================

#include "stdafx.h"
#include "SceneSerializer.h"
#include "Scene.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"
#include "Resources/ResourceManager.h"
#include "Entity/Entity.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/RenderComponent.h"
#include "Entity/Components/LightComponent.h"
#include "Entity/Components/CameraComponent.h"
#include "Renderer/Light.h" // for LightType enum

// JSON library
#include "json.hpp"
using json = nlohmann::json;

namespace USE {

    // -----------------------------------------------------------------
    // Helper: convert Vector3 to JSON array
    // -----------------------------------------------------------------
    static json Vector3ToJson(const Vector3& v)
    {
        return json::array({v.x, v.y, v.z});
    }

    // -----------------------------------------------------------------
    // Helper: convert Quaternion to JSON array (x,y,z,w)
    // -----------------------------------------------------------------
    static json QuaternionToJson(const Quaternion& q)
    {
        return json::array({q.x, q.y, q.z, q.w});
    }

    // -----------------------------------------------------------------
    // Helper: convert Color to JSON array (r,g,b,a)
    // -----------------------------------------------------------------
    static json ColorToJson(const Color& c)
    {
        return json::array({c.r, c.g, c.b, c.a});
    }

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
    // Helper: parse Quaternion from JSON array
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
    // Helper: parse Color from JSON array
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
    // Save an entity to JSON
    // -----------------------------------------------------------------
    static json SaveEntity(Entity* entity)
    {
        json j;
        j["name"] = entity->GetName();
        j["active"] = entity->IsActive();

        json components = json::array();

        // TransformComponent
        TransformComponent* tc = entity->GetComponent<TransformComponent>();
        if (tc) {
            json comp;
            comp["type"] = "Transform";
            comp["position"] = Vector3ToJson(tc->localTransform.translation);
            comp["rotation"] = QuaternionToJson(tc->localTransform.rotation);
            comp["scale"] = Vector3ToJson(tc->localTransform.scale);
            components.push_back(comp);
        }

        // RenderComponent
        RenderComponent* rc = entity->GetComponent<RenderComponent>();
        if (rc) {
            json comp;
            comp["type"] = "Render";
            // Note: mesh and material are asset paths; we need to store references.
            // In a real engine, you'd get the path from the resource manager.
            // For now, we just store placeholders.
            if (rc->GetMesh()) {
                comp["mesh"] = "mesh_reference"; // TODO: get actual path
            }
            if (rc->GetMaterial()) {
                comp["material"] = "material_reference"; // TODO
            }
            comp["castShadows"] = rc->GetCastShadows();
            comp["receiveShadows"] = rc->GetReceiveShadows();
            comp["visible"] = rc->IsVisible();
            components.push_back(comp);
        }

        // LightComponent
        LightComponent* lc = entity->GetComponent<LightComponent>();
        if (lc) {
            json comp;
            comp["type"] = "Light";
            switch (lc->GetType()) {
                case LightType::Point: comp["lightType"] = "Point"; break;
                case LightType::Spot:  comp["lightType"] = "Spot"; break;
                default:                comp["lightType"] = "Directional"; break;
            }
            comp["color"] = ColorToJson(lc->GetColor());
            comp["intensity"] = lc->GetIntensity();
            comp["range"] = lc->GetRange();
            comp["castShadows"] = lc->GetCastShadows();

            // Attenuation
            json att;
            att["constant"] = lc->GetConstantAttenuation();
            att["linear"] = lc->GetLinearAttenuation();
            att["quadratic"] = lc->GetQuadraticAttenuation();
            comp["attenuation"] = att;

            // Spot angles
            if (lc->GetType() == LightType::Spot) {
                comp["innerAngle"] = lc->GetInnerSpotAngle();
                comp["outerAngle"] = lc->GetOuterSpotAngle();
            }
            components.push_back(comp);
        }

        // CameraComponent
        CameraComponent* cc = entity->GetComponent<CameraComponent>();
        if (cc) {
            json comp;
            comp["type"] = "Camera";
            // We could store projection parameters if needed.
            // For now, just an empty object.
            components.push_back(comp);
        }

        // Add more components as needed...

        if (!components.empty()) {
            j["components"] = components;
        }

        return j;
    }

    // -----------------------------------------------------------------
    // Load an entity from JSON
    // -----------------------------------------------------------------
    static bool LoadEntity(const json& j,
                           ResourceManager* resourceManager,
                           EntityManager* entityManager)
    {
        std::string name = j.value("name", "Entity");
        Entity* entity = entityManager->CreateEntity(name);
        if (j.contains("active")) {
            entity->SetActive(j["active"]);
        }

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
                        // TODO: load mesh via resource manager
                        USE_LOG_WARN("SceneSerializer: Mesh loading not implemented yet: %s", meshPath.c_str());
                    }
                    if (compJson.contains("material")) {
                        std::string matPath = compJson["material"];
                        // TODO: load material via resource manager
                        USE_LOG_WARN("SceneSerializer: Material loading not implemented yet: %s", matPath.c_str());
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
                    USE_LOG_WARN("SceneSerializer: Unknown component type '%s'", type.c_str());
                }
            }
        }

        return true;
    }

    // -----------------------------------------------------------------
    // Save scene to file
    // -----------------------------------------------------------------
    bool SceneSerializer::SaveToFile(const Scene* scene,
                                     const std::string& filename,
                                     ResourceManager* resourceManager)
    {
        if (!scene) {
            USE_LOG_ERROR("SceneSerializer: scene is null");
            return false;
        }

        json j;

        // Scene global properties
        j["ambientLight"] = ColorToJson(scene->GetAmbientLight());
        // Skybox reference (if any) – we'd need to get the skybox asset path.
        // For now, just a placeholder.
        if (scene->GetSkybox()) {
            j["skybox"] = "skybox_reference";
        }

        // Entities
        json entitiesJson = json::array();
        std::vector<Entity*> entities = scene->GetWorld()->GetAllEntities();
        for (Entity* entity : entities) {
            entitiesJson.push_back(SaveEntity(entity));
        }
        j["entities"] = entitiesJson;

        // Write to file
        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("SceneSerializer: FileSystem not available");
            return false;
        }

        auto file = fs->OpenFile(filename, FILE_WRITE | FILE_TEXT | FILE_TRUNCATE);
        if (!file) {
            USE_LOG_ERROR("SceneSerializer: Cannot create file: %s", filename.c_str());
            return false;
        }

        file->WriteLine(j.dump(4)); // pretty print with 4 spaces
        file->Close();

        USE_LOG_INFO("SceneSerializer: Saved scene to %s", filename.c_str());
        return true;
    }

    // -----------------------------------------------------------------
    // Load scene from file
    // -----------------------------------------------------------------
    Scene* SceneSerializer::LoadFromFile(const std::string& filename,
                                         ResourceManager* resourceManager)
    {
        if (!resourceManager) {
            USE_LOG_ERROR("SceneSerializer: ResourceManager is null");
            return nullptr;
        }

        // Resolve file path
        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("SceneSerializer: FileSystem not available");
            return nullptr;
        }

        std::string resolved = fs->ResolvePath(filename);
        if (resolved.empty()) {
            USE_LOG_ERROR("SceneSerializer: File not found: %s", filename.c_str());
            return nullptr;
        }

        // Read file
        auto file = fs->OpenFile(resolved, FILE_READ | FILE_TEXT);
        if (!file) {
            USE_LOG_ERROR("SceneSerializer: Could not open file: %s", resolved.c_str());
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
            USE_LOG_ERROR("SceneSerializer: JSON parse error: %s", e.what());
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
            USE_LOG_INFO("SceneSerializer: Skybox would be loaded from %s", skyboxPath.c_str());
        }

        // Load entities
        if (j.contains("entities") && j["entities"].is_array()) {
            for (const auto& entityJson : j["entities"]) {
                if (!LoadEntity(entityJson, resourceManager, scene->GetWorld()->GetEntityManager())) {
                    USE_LOG_WARN("SceneSerializer: Failed to load entity, skipping.");
                }
            }
        }

        USE_LOG_INFO("SceneSerializer: Successfully loaded scene from %s", filename.c_str());
        return scene;
    }

} // namespace USE