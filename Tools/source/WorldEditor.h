#pragma once
#include "stdafx.h"

class WorldEditor {
public:
    WorldEditor();
    void Draw();

private:
    void DrawEntityList();
    void DrawTransformControls();
    void DrawPropertyGrid();

    std::string m_selectedEntity;
    float m_cameraSpeed = 5.0f;
    float m_cameraSensitivity = 0.1f;
    bool m_showGrid = true;
    bool m_showPhysicsDebug = false;
};