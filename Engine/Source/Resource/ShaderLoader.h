// ============================================================
// Ultimate Source Engine - Shader Loader
// ============================================================
//
// Loads shader programs from source files and compiles them
// using the appropriate rendering backend.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>

namespace USE {

    class Shader;

    class ShaderLoader {
    public:
        // Load a shader from vertex and fragment source files.
        // Returns a new Shader object (owned by caller) or nullptr on failure.
        static Shader* LoadFromFile(const std::string& vertexPath,
                                    const std::string& fragmentPath,
                                    const std::string& geometryPath = "");

        // Load a shader from vertex and fragment source strings.
        // Returns a new Shader object (owned by caller) or nullptr on failure.
        static Shader* LoadFromSource(const std::string& vertexSource,
                                      const std::string& fragmentSource,
                                      const std::string& geometrySource = "");
    };

} // namespace USE