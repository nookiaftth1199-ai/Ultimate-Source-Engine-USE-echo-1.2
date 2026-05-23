// ============================================================
// Ultimate Source Engine - Debug Draw
// ============================================================
//
// Provides immediate-mode debug rendering of shapes (lines, boxes,
// spheres, etc.) for visualizing physics, AI, or general debugging.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Color.h"
#include "Math/Matrix4.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Renderer/Shader.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include <vector>

namespace USE {

    class Camera;
    class IRenderDevice;

    struct DebugLineVertex {
        Vector3 position;
        Color   color;
    };

    class DebugDraw {
    public:
        DebugDraw();
        ~DebugDraw();

        // Initialize (creates shaders and buffers)
        bool Initialize(IRenderDevice* device);
        void Shutdown();

        // Begin/End frame (clear lists)
        void BeginFrame();
        void EndFrame();

        // Drawing functions (add to current frame)
        void DrawLine(const Vector3& start, const Vector3& end, const Color& color);
        void DrawLine(const Vector3& start, const Vector3& end, const Color& colorStart, const Color& colorEnd);

        void DrawBox(const Vector3& center, const Vector3& halfExtents, const Color& color);
        void DrawBox(const Matrix4& transform, const Color& color); // oriented box

        void DrawSphere(const Vector3& center, float radius, const Color& color, int segments = 16);
        void DrawCapsule(const Vector3& start, const Vector3& end, float radius, const Color& color, int segments = 8);
        void DrawCylinder(const Vector3& center, float radius, float height, const Color& color, int segments = 16);

        void DrawAABB(const Vector3& min, const Vector3& max, const Color& color);
        void DrawAABB(const class AABB& aabb, const Color& color);

        void DrawFrustum(const class Frustum& frustum, const Color& color);

        void DrawAxis(const Matrix4& transform, float length = 1.0f); // draws RGB axes

        void DrawPoint(const Vector3& point, const Color& color, float size = 5.0f);

        // Render all accumulated debug primitives
        void Render(Camera* camera);

    private:
        IRenderDevice* m_device;
        bool           m_initialized;

        // Vertex lists
        std::vector<DebugLineVertex> m_lineVertices;
        std::vector<uint32_t>        m_lineIndices; // for indexed lines

        // GPU resources
        uint32_t m_lineVertexBuffer;
        uint32_t m_lineIndexBuffer;
        Shader*  m_lineShader;
        Material* m_lineMaterial;

        // Helper to ensure buffers are recreated when size changes
        void UploadLineData();

        // Shape generation helpers
        void AddLine(const Vector3& start, const Vector3& end,
                     const Color& colorStart, const Color& colorEnd);
        void AddIndexedLine(const Vector3& start, const Vector3& end,
                            const Color& colorStart, const Color& colorEnd);
    };

} // namespace USE