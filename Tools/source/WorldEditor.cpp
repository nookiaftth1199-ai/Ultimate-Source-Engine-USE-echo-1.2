#include "WorldEditor.h"
#include "USE_Engine/Scene/Scene.h"
#include "USE_Engine/Entity/Entity.h"
#include "USE_Engine/Entity/Components/TransformComponent.h"
#include "USE_Engine/Entity/Components/RenderComponent.h"

WorldEditor::WorldEditor() {}

void WorldEditor::Draw() {
    ImGui::Begin("World Editor");

    // Toolbar
    if (ImGui::Button("Play")) {
        // Start game simulation
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop")) {
        // Stop simulation
    }
    ImGui::SameLine();
    ImGui::Checkbox("Grid", &m_showGrid);
    ImGui::SameLine();
    ImGui::Checkbox("Physics Debug", &m_showPhysicsDebug);
    if (m_showPhysicsDebug)
        Engine::Get()->GetPhysics()->DebugDraw();

    // Viewport placeholder (would be a separate render target)
    ImGui::Text("Viewport (coming soon)");
    ImGui::Separator();

    // Entity list
    DrawEntityList();

    // Transform controls for selected entity
    DrawTransformControls();

    // Property grid
    DrawPropertyGrid();

    ImGui::End();
}

void WorldEditor::DrawEntityList() {
    ImGui::BeginChild("EntityList", ImVec2(250, 0), true);
    ImGui::Text("Entities");

    if (ImGui::Button("Create Empty")) {
        Engine::Get()->GetWorld()->CreateEntity("NewEntity");
    }

    auto entities = Engine::Get()->GetWorld()->GetAllEntities();
    for (auto* e : entities) {
        std::string name = e->GetName();
        if (ImGui::Selectable(name.c_str(), m_selectedEntity == name)) {
            m_selectedEntity = name;
        }
    }
    ImGui::EndChild();
}

void WorldEditor::DrawTransformControls() {
    if (m_selectedEntity.empty()) return;

    Entity* e = Engine::Get()->GetWorld()->GetEntityByName(m_selectedEntity);
    if (!e) return;

    TransformComponent* tc = e->GetComponent<TransformComponent>();
    if (!tc) return;

    ImGui::Separator();
    ImGui::Text("Transform: %s", m_selectedEntity.c_str());

    float pos[3] = { tc->localTransform.translation.x,
                     tc->localTransform.translation.y,
                     tc->localTransform.translation.z };
    if (ImGui::DragFloat3("Position", pos, 0.1f)) {
        tc->localTransform.translation = Vector3(pos[0], pos[1], pos[2]);
        tc->UpdateWorldTransform();
    }

    float rot[3] = { tc->localTransform.rotation.ToEulerAngles().x,
                     tc->localTransform.rotation.ToEulerAngles().y,
                     tc->localTransform.rotation.ToEulerAngles().z };
    if (ImGui::DragFloat3("Rotation (deg)", rot, 1.0f)) {
        tc->localTransform.rotation = Quaternion::FromEulerAngles(rot[0], rot[1], rot[2]);
        tc->UpdateWorldTransform();
    }

    float scale[3] = { tc->localTransform.scale.x,
                       tc->localTransform.scale.y,
                       tc->localTransform.scale.z };
    if (ImGui::DragFloat3("Scale", scale, 0.1f)) {
        tc->localTransform.scale = Vector3(scale[0], scale[1], scale[2]);
        tc->UpdateWorldTransform();
    }
}

void WorldEditor::DrawPropertyGrid() {
    ImGui::Separator();
    ImGui::Text("Properties");
    // Could list components, add/remove, edit component values
}