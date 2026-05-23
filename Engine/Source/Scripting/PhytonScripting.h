// ============================================================
// Ultimate Source Engine - Python Scripting
//============================================================
//
// Provides a high‑level Python scripting interface.
// Handles loading scripts, calling functions, and binding
// C++ functions to Python.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>

// Forward declaration of Python objects (opaque)
struct _object;
typedef _object PyObject;

namespace USE {

    class PythonScripting {
    public:
        // Get singleton instance
        static PythonScripting& Get();

        // Initialize Python interpreter and register built‑in bindings
        bool Initialize();
        void Shutdown();

        // Load and execute a Python script file. Returns true on success.
        bool DoFile(const std::string& filename);

        // Execute a Python code string. Returns true on success.
        bool DoString(const std::string& code);

        // Call a Python function (global) with no arguments.
        bool CallFunction(const std::string& funcName);

        // Call a Python function with a single float argument (e.g., delta time).
        bool CallFunction(const std::string& funcName, float arg);

        // Call a Python function with a single string argument.
        bool CallFunction(const std::string& funcName, const std::string& arg);

        // Get a Python object by name (e.g., a module, function, variable).
        // The caller must decref the returned object when done.
        PyObject* GetAttribute(const std::string& name);

        // Check if a global function exists.
        bool HasFunction(const std::string& funcName);

    private:
        PythonScripting();
        ~PythonScripting();

        bool m_initialized;

        // Register engine bindings (e.g., print, Vector3)
        void RegisterBindings();
    };

} // namespace USE