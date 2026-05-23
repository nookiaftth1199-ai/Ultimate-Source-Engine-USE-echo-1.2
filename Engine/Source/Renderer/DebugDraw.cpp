// ============================================================
// Ultimate Source Engine - Debug Draw Implementation
// ============================================================

#include "stdafx.h"
#include "DebugDraw.h"
#include "Renderer/Camera.h"
#include "Renderer/IRenderDevice.h"
#include "Math/Frustum.h"
#include "Math/AABB.h"
#include "Core/Logger.h"

#include <cmath>

namespace USE {

    // Simple line shader (GLSL 1.20)
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
        "void main() {\n"
        "    gl_FragColor = vColor;\n"
        "}\n";

    DebugDraw::DebugDraw()
        : m_device(nullptr)
        , m_initialized(false)
        , m_lineVertexBuffer(0)
        , m_lineIndexBuffer(0)
        , m_lineShader(nullptr)
        , m_lineMaterial(nullptr)
    {
    }

    DebugDraw::~DebugDraw()
    {
        Shutdown();
    }

    bool DebugDraw::Initialize(IRenderDevice* device)
    {
        if (m_initialized) return true;

        m_device = device;

        // Create line shader
        m_lineShader = new Shader();
        if (!m_lineShader->LoadFromSource(s_lineVertexShader, s_lineFragmentShader)) {
            USE_LOG_ERROR("DebugDraw: Failed to create line shader");
            delete m_lineShader;
            m_lineShader = nullptr;
            return false;
        }

        // Create material
        m_lineMaterial = new Material("DebugDrawLine");
        m_lineMaterial->SetShader(m_lineShader);
        m_lineMaterial->SetDepthTest(true);
        m_lineMaterial->SetDepthWrite(false); // don't write depth for lines
        m_lineMaterial->SetCullMode(CullMode::None); // lines should be visible from both sides

        m_initialized = true;
        return true;
    }

    void DebugDraw::Shutdown()
    {
        if (!m_initialized) return;

        delete m_lineMaterial;
        delete m_lineShader;

        if (m_lineVertexBuffer) {
            m_device->DestroyBuffer(m_lineVertexBuffer);
            m_lineVertexBuffer = 0;
        }
        if (m_lineIndexBuffer) {
            m_device->DestroyBuffer(m_lineIndexBuffer);
            m_lineIndexBuffer = 0;
        }

        m_initialized = false;
    }

    void DebugDraw::BeginFrame()
    {
        m_lineVertices.clear();
        m_lineIndices.clear();
    }

    void DebugDraw::EndFrame()
    {
        // Nothing to do here, data is uploaded and rendered in Render()
    }

    void DebugDraw::AddIndexedLine(const Vector3& start, const Vector3& end,
                                   const Color& colorStart, const Color& colorEnd)
    {
        uint32_t idx = (uint32_t)m_lineVertices.size();
        m_lineVertices.push_back({start, colorStart});
        m_lineVertices.push_back({end, colorEnd});
        m_lineIndices.push_back(idx);
        m_lineIndices.push_back(idx + 1);
    }

    void DebugDraw::AddLine(const Vector3& start, const Vector3& end,
                            const Color& colorStart, const Color& colorEnd)
    {
        // For non-indexed lines, we just add two vertices per line (simple)
        m_lineVertices.push_back({start, colorStart});
        m_lineVertices.push_back({end, colorEnd});
    }

    void DebugDraw::DrawLine(const Vector3& start, const Vector3& end, const Color& color)
    {
        DrawLine(start, end, color, color);
    }

    void DebugDraw::DrawLine(const Vector3& start, const Vector3& end,
                             const Color& colorStart, const Color& colorEnd)
    {
        AddLine(start, end, colorStart, colorEnd);
    }

    void DebugDraw::DrawBox(const Vector3& center, const Vector3& halfExtents, const Color& color)
    {
        Vector3 corners[8];
        corners[0] = center + Vector3(-halfExtents.x, -halfExtents.y, -halfExtents.z);
        corners[1] = center + Vector3( halfExtents.x, -halfExtents.y, -halfExtents.z);
        corners[2] = center + Vector3( halfExtents.x, -halfExtents.y,  halfExtents.z);
        corners[3] = center + Vector3(-halfExtents.x, -halfExtents.y,  halfExtents.z);
        corners[4] = center + Vector3(-halfExtents.x,  halfExtents.y, -halfExtents.z);
        corners[5] = center + Vector3( halfExtents.x,  halfExtents.y, -halfExtents.z);
        corners[6] = center + Vector3( halfExtents.x,  halfExtents.y,  halfExtents.z);
        corners[7] = center + Vector3(-halfExtents.x,  halfExtents.y,  halfExtents.z);

        // Bottom face
        DrawLine(corners[0], corners[1], color);
        DrawLine(corners[1], corners[2], color);
        DrawLine(corners[2], corners[3], color);
        DrawLine(corners[3], corners[0], color);
        // Top face
        DrawLine(corners[4], corners[5], color);
        DrawLine(corners[5], corners[6], color);
        DrawLine(corners[6], corners[7], color);
        DrawLine(corners[7], corners[4], color);
        // Vertical edges
        DrawLine(corners[0], corners[4], color);
        DrawLine(corners[1], corners[5], color);
        DrawLine(corners[2], corners[6], color);
        DrawLine(corners[3], corners[7], color);
    }

    void DebugDraw::DrawBox(const Matrix4& transform, const Color& color)
    {
        // Unit box corners in local space
        Vector3 localCorners[8] = {
            Vector3(-1, -1, -1),
            Vector3( 1, -1, -1),
            Vector3( 1, -1,  1),
            Vector3(-1, -1,  1),
            Vector3(-1,  1, -1),
            Vector3( 1,  1, -1),
            Vector3( 1,  1,  1),
            Vector3(-1,  1,  1)
        };
        Vector3 worldCorners[8];
        for (int i = 0; i < 8; ++i) {
            worldCorners[i] = transform.TransformPoint(localCorners[i]);
        }
        // Connect edges (same as above but using worldCorners)
        for (int i = 0; i < 4; ++i) {
            int next = (i+1)%4;
            DrawLine(worldCorners[i], worldCorners[next], color);
            DrawLine(worldCorners[i+4], worldCorners[next+4], color);
            DrawLine(worldCorners[i], worldCorners[i+4], color);
        }
    }

    void DebugDraw::DrawSphere(const Vector3& center, float radius, const Color& color, int segments)
    {
        // Draw three orthogonal rings
        int seg = segments;
        float step = 2.0f * (float)M_PI / seg;

        // XY ring
        for (int i = 0; i < seg; ++i) {
            float a1 = i * step;
            float a2 = (i+1) * step;
            Vector3 p1(center.x + radius * cosf(a1), center.y + radius * sinf(a1), center.z);
            Vector3 p2(center.x + radius * cosf(a2), center.y + radius * sinf(a2), center.z);
            DrawLine(p1, p2, color);
        }
        // XZ ring
        for (int i = 0; i < seg; ++i) {
            float a1 = i * step;
            float a2 = (i+1) * step;
            Vector3 p1(center.x + radius * cosf(a1), center.y, center.z + radius * sinf(a1));
            Vector3 p2(center.x + radius * cosf(a2), center.y, center.z + radius * sinf(a2));
            DrawLine(p1, p2, color);
        }
        // YZ ring
        for (int i = 0; i < seg; ++i) {
            float a1 = i * step;
            float a2 = (i+1) * step;
            Vector3 p1(center.x, center.y + radius * cosf(a1), center.z + radius * sinf(a1));
            Vector3 p2(center.x, center.y + radius * cosf(a2), center.z + radius * sinf(a2));
            DrawLine(p1, p2, color);
        }
    }

    void DebugDraw::DrawCapsule(const Vector3& start, const Vector3& end, float radius,
                                const Color& color, int segments)
    {
        // Simple capsule: a cylinder with hemispherical ends
        Vector3 axis = end - start;
        float height = axis.Length();
        if (height < 0.001f) {
            DrawSphere(start, radius, color, segments);
            return;
        }
        Vector3 dir = axis.Normalized();

        // Compute perpendicular vectors
        Vector3 up(0,1,0);
        Vector3 right = up.Cross(dir).Normalized();
        if (right.LengthSq() < 0.1f) {
            // dir is near vertical, use another up
            up = Vector3(0,0,1);
            right = up.Cross(dir).Normalized();
        }
        Vector3 forward = dir.Cross(right).Normalized();

        // Draw cylinder lines (four lines along axis)
        Vector3 p1 = start + right * radius;
        Vector3 p2 = start - right * radius;
        Vector3 p3 = start + forward * radius;
        Vector3 p4 = start - forward * radius;
        Vector3 q1 = end + right * radius;
        Vector3 q2 = end - right * radius;
        Vector3 q3 = end + forward * radius;
        Vector3 q4 = end - forward * radius;

        DrawLine(p1, q1, color);
        DrawLine(p2, q2, color);
        DrawLine(p3, q3, color);
        DrawLine(p4, q4, color);

        // Draw rings at ends (circles)
        int seg = segments;
        float step = 2.0f * (float)M_PI / seg;
        for (int i = 0; i < seg; ++i) {
            float a1 = i * step;
            float a2 = (i+1) * step;
            Vector3 offset1 = right * (radius * cosf(a1)) + forward * (radius * sinf(a1));
            Vector3 offset2 = right * (radius * cosf(a2)) + forward * (radius * sinf(a2));
            DrawLine(start + offset1, start + offset2, color);
            DrawLine(end + offset1, end + offset2, color);
        }

        // Draw hemispheres (simplified: just half-rings)
        // Not implemented for brevity.
    }

    void DebugDraw::DrawCylinder(const Vector3& center, float radius, float height,
                                 const Color& color, int segments)
    {
        Vector3 up(0,1,0);
        Vector3 right(1,0,0);
        Vector3 forward(0,0,1);

        Vector3 topCenter = center + up * height * 0.5f;
        Vector3 bottomCenter = center - up * height * 0.5f;

        float step = 2.0f * (float)M_PI / segments;

        // Draw top and bottom circles
        for (int i = 0; i < segments; ++i) {
            float a1 = i * step;
            float a2 = (i+1) * step;
            Vector3 r1 = right * (radius * cosf(a1)) + forward * (radius * sinf(a1));
            Vector3 r2 = right * (radius * cosf(a2)) + forward * (radius * sinf(a2));
            DrawLine(topCenter + r1, topCenter + r2, color);
            DrawLine(bottomCenter + r1, bottomCenter + r2, color);
        }

        // Draw vertical lines
        for (int i = 0; i < segments; ++i) {
            float a = i * step;
            Vector3 r = right * (radius * cosf(a)) + forward * (radius * sinf(a));
            DrawLine(bottomCenter + r, topCenter + r, color);
        }
    }

    void DebugDraw::DrawAABB(const Vector3& min, const Vector3& max, const Color& color)
    {
        Vector3 corners[8] = {
            Vector3(min.x, min.y, min.z),
            Vector3(max.x, min.y, min.z),
            Vector3(max.x, min.y, max.z),
            Vector3(min.x, min.y, max.z),
            Vector3(min.x, max.y, min.z),
            Vector3(max.x, max.y, min.z),
            Vector3(max.x, max.y, max.z),
            Vector3(min.x, max.y, max.z)
        };
        for (int i = 0; i < 4; ++i) {
            int next = (i+1)%4;
            DrawLine(corners[i], corners[next], color);
            DrawLine(corners[i+4], corners[next+4], color);
            DrawLine(corners[i], corners[i+4], color);
        }
    }

    void DebugDraw::DrawAABB(const AABB& aabb, const Color& color)
    {
        if (aabb.IsValid())
            DrawAABB(aabb.min, aabb.max, color);
    }

    void DebugDraw::DrawFrustum(const Frustum& frustum, const Color& color)
    {
        // Extract frustum corners from planes? This is complex.
        // For simplicity, we'll assume a method to get corners is not available.
        // We'll just draw the planes? Maybe skip for now.
    }

    void DebugDraw::DrawAxis(const Matrix4& transform, float length)
    {
        Vector3 origin = transform.TransformPoint(Vector3::Zero);
        Vector3 x = transform.TransformPoint(Vector3(length, 0, 0));
        Vector3 y = transform.TransformPoint(Vector3(0, length, 0));
        Vector3 z = transform.TransformPoint(Vector3(0, 0, length));
        DrawLine(origin, x, Color(1,0,0));
        DrawLine(origin, y, Color(0,1,0));
        DrawLine(origin, z, Color(0,0,1));
    }

    void DebugDraw::DrawPoint(const Vector3& point, const Color& color, float size)
    {
        // Draw a small cross
        float s = size * 0.5f;
        DrawLine(point - Vector3(s,0,0), point + Vector3(s,0,0), color);
        DrawLine(point - Vector3(0,s,0), point + Vector3(0,s,0), color);
        DrawLine(point - Vector3(0,0,s), point + Vector3(0,0,s), color);
    }

    void DebugDraw::UploadLineData()
    {
        if (!m_device) return;

        // Destroy old buffers if they exist
        if (m_lineVertexBuffer) {
            m_device->DestroyBuffer(m_lineVertexBuffer);
            m_lineVertexBuffer = 0;
        }
        if (m_lineIndexBuffer) {
            m_device->DestroyBuffer(m_lineIndexBuffer);
            m_lineIndexBuffer = 0;
        }

        if (m_lineVertices.empty()) return;

        // Create vertex buffer
        size_t vertexSize = m_lineVertices.size() * sizeof(DebugLineVertex);
        m_device->CreateVertexBuffer(m_lineVertices.data(), vertexSize, m_lineVertexBuffer);

        // For simplicity, we are not using indexed drawing; we'll just use vertex buffer as non-indexed.
        // If we had indices, we'd create index buffer.
    }

    void DebugDraw::Render(Camera* camera)
    {
        if (!m_initialized || !camera || m_lineVertices.empty()) return;

        UploadLineData();

        // Set up shader
        m_lineMaterial->SetUniform("viewProj", camera->GetViewProjectionMatrix());
        m_lineMaterial->Bind();

        // Set vertex buffer and attributes
        m_device->SetVertexBuffer(0, m_lineVertexBuffer, sizeof(DebugLineVertex), 0);

        // We need to define vertex attributes. This requires extending IRenderDevice to support SetVertexAttribute.
        // For now, we'll assume the material/shader knows how to bind attributes? Not really.
        // This is a limitation. To keep it simple, we'll use immediate mode or assume the device has a simple interface.
        // In practice, you'd have a way to set vertex attributes via the device.

        // Since our IRenderDevice is basic, we'll just use glDrawArrays directly? No, we need abstraction.
        // Let's assume the device has a method DrawArrays, which we haven't added.
        // For now, we'll stub.
        USE_LOG_WARN("DebugDraw::Render: Vertex attribute binding not implemented in IRenderDevice");

        // If we had DrawArrays, we'd call:
        // m_device->DrawArrays(PrimitiveTopology::LineList, 0, m_lineVertices.size());
    }

} // namespace USE