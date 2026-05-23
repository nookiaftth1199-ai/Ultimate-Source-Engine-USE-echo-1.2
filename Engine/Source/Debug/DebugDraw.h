// ============================================================
// DebugDraw.h
// ============================================================
#pragma once
#include "Math/Vector3.h"
#include "Math/Color.h"
#include "Math/Matrix4.h"
#include "Math/AABB.h"
#include <vector>

namespace USE {
    class Camera;
    class IRenderDevice;

    struct DebugLineVertex {
        Vector3 position;
        Color color;
    };

    class DebugDraw {
    public:
        DebugDraw();
        ~DebugDraw();

        bool Initialize(IRenderDevice* device);
        void Shutdown();

        void BeginFrame();
        void EndFrame();

        void DrawLine(const Vector3& start, const Vector3& end, const Color& color);
        void DrawLine(const Vector3& start, const Vector3& end, const Color& startColor, const Color& endColor);
        void DrawBox(const Vector3& center, const Vector3& halfExtents, const Color& color);
        void DrawBox(const Matrix4& transform, const Color& color);
        void DrawSphere(const Vector3& center, float radius, const Color& color, int segments = 16);
        void DrawCapsule(const Vector3& start, const Vector3& end, float radius, const Color& color, int segments = 8);
        void DrawCylinder(const Vector3& center, float radius, float height, const Color& color, int segments = 16);
        void DrawAABB(const Vector3& min, const Vector3& max, const Color& color);
        void DrawAABB(const AABB& aabb, const Color& color);
        void DrawFrustum(const class Frustum& frustum, const Color& color);
        void DrawAxis(const Matrix4& transform, float length = 1.0f);
        void DrawPoint(const Vector3& point, const Color& color, float size = 5.0f);

        void Render(Camera* camera);

    private:
        IRenderDevice* m_device;
        bool m_initialized;
        std::vector<DebugLineVertex> m_lineVertices;
        uint32_t m_lineVertexBuffer;
        class Shader* m_lineShader;
        class Material* m_lineMaterial;

        void UploadLineData();
        void AddLine(const Vector3& start, const Vector3& end, const Color& startColor, const Color& endColor);
    };
}