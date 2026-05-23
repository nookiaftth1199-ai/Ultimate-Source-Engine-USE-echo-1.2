// ============================================================
// DebugDraw.cpp
// ============================================================
#include "DebugDraw.h"
#include "Renderer/Camera.h"
#include "Renderer/IRenderDevice.h"
#include "Renderer/Shader.h"
#include "Renderer/Material.h"
#include "Math/Frustum.h"
#include "Math/MathUtils.h"
#include "Core/Logger.h"
#include <cmath>

static const char* s_lineVertexShader =
    "uniform mat4 viewProj;\n"
    "attribute vec3 position;\n"
    "attribute vec4 color;\n"
    "varying vec4 vColor;\n"
    "void main() {\n"
    "    vColor = color;\n"
    "    gl_Position = viewProj * vec4(position, 1.0);\n"
    "}\n";

static const char* s_lineFragmentShader =
    "varying vec4 vColor;\n"
    "void main() { gl_FragColor = vColor; }\n";

namespace USE {
    DebugDraw::DebugDraw() : m_device(nullptr), m_initialized(false), m_lineVertexBuffer(0), m_lineShader(nullptr), m_lineMaterial(nullptr) {}
    DebugDraw::~DebugDraw() { Shutdown(); }

    bool DebugDraw::Initialize(IRenderDevice* device) {
        if (m_initialized) return true;
        m_device = device;
        m_lineShader = new Shader();
        if (!m_lineShader->LoadFromSource(s_lineVertexShader, s_lineFragmentShader)) {
            USE_LOG_ERROR("DebugDraw: Failed to create line shader");
            delete m_lineShader; m_lineShader = nullptr; return false;
        }
        m_lineMaterial = new Material("DebugDrawLine");
        m_lineMaterial->SetShader(m_lineShader);
        m_lineMaterial->SetDepthTest(true);
        m_lineMaterial->SetDepthWrite(false);
        m_initialized = true;
        return true;
    }

    void DebugDraw::Shutdown() {
        delete m_lineMaterial; m_lineMaterial = nullptr;
        delete m_lineShader; m_lineShader = nullptr;
        if (m_lineVertexBuffer) { m_device->DestroyBuffer(m_lineVertexBuffer); m_lineVertexBuffer = 0; }
        m_initialized = false;
    }

    void DebugDraw::BeginFrame() { m_lineVertices.clear(); }
    void DebugDraw::EndFrame() {}

    void DebugDraw::AddLine(const Vector3& start, const Vector3& end, const Color& startColor, const Color& endColor) {
        m_lineVertices.push_back({start, startColor});
        m_lineVertices.push_back({end, endColor});
    }

    void DebugDraw::DrawLine(const Vector3& start, const Vector3& end, const Color& color) { AddLine(start, end, color, color); }
    void DebugDraw::DrawLine(const Vector3& start, const Vector3& end, const Color& startColor, const Color& endColor) { AddLine(start, end, startColor, endColor); }

    void DebugDraw::DrawBox(const Vector3& center, const Vector3& half, const Color& color) {
        Vector3 corners[8];
        corners[0] = center + Vector3(-half.x, -half.y, -half.z);
        corners[1] = center + Vector3( half.x, -half.y, -half.z);
        corners[2] = center + Vector3( half.x, -half.y,  half.z);
        corners[3] = center + Vector3(-half.x, -half.y,  half.z);
        corners[4] = center + Vector3(-half.x,  half.y, -half.z);
        corners[5] = center + Vector3( half.x,  half.y, -half.z);
        corners[6] = center + Vector3( half.x,  half.y,  half.z);
        corners[7] = center + Vector3(-half.x,  half.y,  half.z);
        for (int i = 0; i < 4; ++i) {
            DrawLine(corners[i], corners[(i+1)%4], color);
            DrawLine(corners[i+4], corners[(i+1)%4+4], color);
            DrawLine(corners[i], corners[i+4], color);
        }
    }

    void DebugDraw::DrawBox(const Matrix4& transform, const Color& color) {
        Vector3 local[8] = {
            {-1,-1,-1},{ 1,-1,-1},{ 1,-1, 1},{-1,-1, 1},
            {-1, 1,-1},{ 1, 1,-1},{ 1, 1, 1},{-1, 1, 1}
        };
        Vector3 world[8];
        for (int i = 0; i < 8; ++i) world[i] = transform.TransformPoint(local[i]);
        for (int i = 0; i < 4; ++i) {
            DrawLine(world[i], world[(i+1)%4], color);
            DrawLine(world[i+4], world[(i+1)%4+4], color);
            DrawLine(world[i], world[i+4], color);
        }
    }

    void DebugDraw::DrawSphere(const Vector3& center, float radius, const Color& color, int seg) {
        float step = 2.0f * (float)M_PI / seg;
        for (int i = 0; i < seg; ++i) {
            float a1 = i*step, a2 = (i+1)*step;
            DrawLine(center + Vector3(radius*cosf(a1), radius*sinf(a1), 0),
                     center + Vector3(radius*cosf(a2), radius*sinf(a2), 0), color);
            DrawLine(center + Vector3(radius*cosf(a1), 0, radius*sinf(a1)),
                     center + Vector3(radius*cosf(a2), 0, radius*sinf(a2)), color);
            DrawLine(center + Vector3(0, radius*cosf(a1), radius*sinf(a1)),
                     center + Vector3(0, radius*cosf(a2), radius*sinf(a2)), color);
        }
    }

    void DebugDraw::DrawCapsule(const Vector3& start, const Vector3& end, float radius, const Color& color, int seg) {
        Vector3 dir = (end - start).Normalized();
        Vector3 right = (dir.Cross(Vector3(0,1,0)).Normalized());
        if (right.LengthSq() < 0.1f) right = (dir.Cross(Vector3(0,0,1)).Normalized());
        Vector3 up = dir.Cross(right);
        float step = 2.0f * (float)M_PI / seg;
        for (int i = 0; i < seg; ++i) {
            float a1 = i*step, a2 = (i+1)*step;
            Vector3 offset1 = right * (radius*cosf(a1)) + up * (radius*sinf(a1));
            Vector3 offset2 = right * (radius*cosf(a2)) + up * (radius*sinf(a2));
            DrawLine(start + offset1, start + offset2, color);
            DrawLine(end + offset1, end + offset2, color);
            DrawLine(start + offset1, end + offset1, color);
        }
        DrawSphere(start, radius, color, seg);
        DrawSphere(end, radius, color, seg);
    }

    void DebugDraw::DrawCylinder(const Vector3& center, float radius, float height, const Color& color, int seg) {
        Vector3 top = center + Vector3(0, height*0.5f, 0);
        Vector3 bottom = center - Vector3(0, height*0.5f, 0);
        float step = 2.0f * (float)M_PI / seg;
        for (int i = 0; i < seg; ++i) {
            float a1 = i*step, a2 = (i+1)*step;
            Vector3 r1(radius*cosf(a1), 0, radius*sinf(a1));
            Vector3 r2(radius*cosf(a2), 0, radius*sinf(a2));
            DrawLine(top + r1, top + r2, color);
            DrawLine(bottom + r1, bottom + r2, color);
            DrawLine(top + r1, bottom + r1, color);
        }
    }

    void DebugDraw::DrawAABB(const Vector3& min, const Vector3& max, const Color& color) {
        Vector3 corners[8] = {
            {min.x, min.y, min.z}, {max.x, min.y, min.z}, {max.x, min.y, max.z}, {min.x, min.y, max.z},
            {min.x, max.y, min.z}, {max.x, max.y, min.z}, {max.x, max.y, max.z}, {min.x, max.y, max.z}
        };
        for (int i = 0; i < 4; ++i) {
            DrawLine(corners[i], corners[(i+1)%4], color);
            DrawLine(corners[i+4], corners[(i+1)%4+4], color);
            DrawLine(corners[i], corners[i+4], color);
        }
    }

    void DebugDraw::DrawAABB(const AABB& aabb, const Color& color) { if (aabb.IsValid()) DrawAABB(aabb.min, aabb.max, color); }

    void DebugDraw::DrawFrustum(const Frustum& frustum, const Color& color) {
        // Not implemented; you would compute corners from planes.
    }

    void DebugDraw::DrawAxis(const Matrix4& transform, float length) {
        Vector3 origin = transform.TransformPoint(Vector3::Zero);
        DrawLine(origin, transform.TransformPoint(Vector3(length,0,0)), Color(1,0,0));
        DrawLine(origin, transform.TransformPoint(Vector3(0,length,0)), Color(0,1,0));
        DrawLine(origin, transform.TransformPoint(Vector3(0,0,length)), Color(0,0,1));
    }

    void DebugDraw::DrawPoint(const Vector3& point, const Color& color, float size) {
        float s = size * 0.5f;
        DrawLine(point - Vector3(s,0,0), point + Vector3(s,0,0), color);
        DrawLine(point - Vector3(0,s,0), point + Vector3(0,s,0), color);
        DrawLine(point - Vector3(0,0,s), point + Vector3(0,0,s), color);
    }

    void DebugDraw::UploadLineData() {
        if (!m_device) return;
        if (m_lineVertexBuffer) m_device->DestroyBuffer(m_lineVertexBuffer);
        if (m_lineVertices.empty()) return;
        m_device->CreateVertexBuffer(m_lineVertices.data(), m_lineVertices.size() * sizeof(DebugLineVertex), m_lineVertexBuffer);
    }

    void DebugDraw::Render(Camera* camera) {
        if (!m_initialized || !camera || m_lineVertices.empty()) return;
        UploadLineData();
        m_lineMaterial->SetUniform("viewProj", camera->GetViewProjectionMatrix());
        m_lineMaterial->Bind();
        m_device->SetVertexBuffer(0, m_lineVertexBuffer, sizeof(DebugLineVertex), 0);
        // Assume IRenderDevice has DrawArrays; for now just log.
        USE_LOG_WARN("DebugDraw::Render: DrawArrays not implemented in IRenderDevice");
    }
}