// ============================================================
// Ultimate Source Engine - Render Statistics
// ============================================================
//
// Contains structures for collecting rendering performance data.
// ============================================================

#pragma once

#include <cstdint>

namespace USE {

    // -----------------------------------------------------------------
    // Render statistics counters
    // -----------------------------------------------------------------
    struct RenderStats {
        uint64_t frameCount;          // Number of frames rendered
        uint32_t drawCalls;            // Number of draw calls issued
        uint32_t trianglesDrawn;       // Number of triangles drawn
        uint32_t clearCount;            // Number of clear operations
        uint32_t textureBinds;          // Number of texture bindings
        uint32_t shaderBinds;           // Number of shader program switches
        uint32_t vertexBufferBinds;     // Number of vertex buffer bindings
        uint32_t indexBufferBinds;      // Number of index buffer bindings
        uint32_t renderTargetSwitches;  // Number of render target changes
        uint32_t stateChanges;           // Number of render state changes

        RenderStats() {
            Reset();
        }

        void Reset() {
            frameCount = 0;
            drawCalls = 0;
            trianglesDrawn = 0;
            clearCount = 0;
            textureBinds = 0;
            shaderBinds = 0;
            vertexBufferBinds = 0;
            indexBufferBinds = 0;
            renderTargetSwitches = 0;
            stateChanges = 0;
        }

        // Add another stats object to this one
        RenderStats& operator+=(const RenderStats& other) {
            frameCount += other.frameCount;
            drawCalls += other.drawCalls;
            trianglesDrawn += other.trianglesDrawn;
            clearCount += other.clearCount;
            textureBinds += other.textureBinds;
            shaderBinds += other.shaderBinds;
            vertexBufferBinds += other.vertexBufferBinds;
            indexBufferBinds += other.indexBufferBinds;
            renderTargetSwitches += other.renderTargetSwitches;
            stateChanges += other.stateChanges;
            return *this;
        }

        // Compute average per frame (if frameCount > 0)
        RenderStats AveragePerFrame() const {
            RenderStats avg;
            if (frameCount == 0) return avg;
            avg.drawCalls = drawCalls / (uint32_t)frameCount;
            avg.trianglesDrawn = trianglesDrawn / (uint32_t)frameCount;
            // ... etc.
            return avg;
        }
    };

} // namespace USE