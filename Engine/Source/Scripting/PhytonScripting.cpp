// ============================================================
// Ultimate Source Engine - Python Scripting Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "PythonScripting.h"
#include "Core/Logger.h"
#include "Core/FileSystem.h"
#include "Math/Vector3.h"

// Python headers
#include <Python.h>

namespace USE {

    // -----------------------------------------------------------------
    // Helper: Python print function (replaces built‑in print to go to engine log)
    // -----------------------------------------------------------------
    static PyObject* py_print(PyObject* self, PyObject* args) {
        PyObject* repr = PyObject_Repr(args);
        if (!repr) return nullptr;
        const char* str = PyUnicode_AsUTF8(repr);
        if (str) {
            USE_LOG_INFO("[Python] %s", str);
        }
        Py_DECREF(repr);
        Py_RETURN_NONE;
    }

    // -----------------------------------------------------------------
    // Helper: create a Vector3 object (as a Python tuple or custom class)
    // For simplicity, we return a tuple (x, y, z)
    // -----------------------------------------------------------------
    static PyObject* py_vector3(PyObject* self, PyObject* args) {
        float x = 0.0f, y = 0.0f, z = 0.0f;
        if (!PyArg_ParseTuple(args, "|fff", &x, &y, &z))
            return nullptr;
        PyObject* tuple = PyTuple_New(3);
        PyTuple_SetItem(tuple, 0, PyFloat_FromDouble(x));
        PyTuple_SetItem(tuple, 1, PyFloat_FromDouble(y));
        PyTuple_SetItem(tuple, 2, PyFloat_FromDouble(z));
        return tuple;
    }

    // -----------------------------------------------------------------
    // Singleton
    // -----------------------------------------------------------------
    PythonScripting& PythonScripting::Get() {
        static PythonScripting instance;
        return instance;
    }

    PythonScripting::PythonScripting()
        : m_initialized(false)
    {
    }

    PythonScripting::~PythonScripting() {
        Shutdown();
    }

    bool PythonScripting::Initialize() {
        if (m_initialized) return true;

        // Set Python home if needed? Not required if PYTHONHOME is set.
        Py_Initialize();
        if (!Py_IsInitialized()) {
            USE_LOG_ERROR("PythonScripting: Failed to initialize Python interpreter");
            return false;
        }

        RegisterBindings();

        m_initialized = true;
        USE_LOG_INFO("PythonScripting initialized");
        return true;
    }

    void PythonScripting::Shutdown() {
        if (m_initialized) {
            Py_Finalize();
            m_initialized = false;
        }
    }

    void PythonScripting::RegisterBindings() {
        // Create a module named "use"
        PyObject* module = PyModule_Create(&use_module_def); // We need to define a module definition
        // For simplicity, we'll use a static method: add builtins to __main__
        PyObject* main = PyImport_AddModule("__main__");
        PyObject* dict = PyModule_GetDict(main);

        // Add print function
        PyObject* printFunc = PyCFunction_New(&py_print_def, nullptr);
        PyDict_SetItemString(dict, "print", printFunc);
        Py_DECREF(printFunc);

        // Add Vector3 function
        PyObject* vec3Func = PyCFunction_New(&py_vector3_def, nullptr);
        PyDict_SetItemString(dict, "Vector3", vec3Func);
        Py_DECREF(vec3Func);
    }

    bool PythonScripting::DoFile(const std::string& filename) {
        if (!m_initialized) return false;

        FileSystem* fs = FileSystem::Get();
        std::string resolved = filename;
        if (fs) {
            resolved = fs->ResolvePath(filename);
            if (resolved.empty()) {
                USE_LOG_ERROR("PythonScripting: File not found: %s", filename.c_str());
                return false;
            }
        }

        FILE* fp = fopen(resolved.c_str(), "r");
        if (!fp) {
            USE_LOG_ERROR("PythonScripting: Could not open file: %s", resolved.c_str());
            return false;
        }
        int result = PyRun_SimpleFile(fp, resolved.c_str());
        fclose(fp);
        if (result != 0) {
            USE_LOG_ERROR("PythonScripting: Error executing file: %s", resolved.c_str());
            if (PyErr_Occurred()) PyErr_Print();
            return false;
        }
        return true;
    }

    bool PythonScripting::DoString(const std::string& code) {
        if (!m_initialized) return false;
        int result = PyRun_SimpleString(code.c_str());
        if (result != 0) {
            USE_LOG_ERROR("PythonScripting: Error executing string");
            if (PyErr_Occurred()) PyErr_Print();
            return false;
        }
        return true;
    }

    bool PythonScripting::CallFunction(const std::string& funcName) {
        if (!m_initialized) return false;
        PyObject* func = PyObject_GetAttrString(PyImport_AddModule("__main__"), funcName.c_str());
        if (!func || !PyCallable_Check(func)) {
            Py_XDECREF(func);
            return false;
        }
        PyObject* result = PyObject_CallObject(func, nullptr);
        Py_DECREF(func);
        if (!result) {
            USE_LOG_ERROR("PythonScripting: Error calling function '%s'", funcName.c_str());
            PyErr_Print();
            return false;
        }
        Py_DECREF(result);
        return true;
    }

    bool PythonScripting::CallFunction(const std::string& funcName, float arg) {
        if (!m_initialized) return false;
        PyObject* func = PyObject_GetAttrString(PyImport_AddModule("__main__"), funcName.c_str());
        if (!func || !PyCallable_Check(func)) {
            Py_XDECREF(func);
            return false;
        }
        PyObject* args = Py_BuildValue("(f)", arg);
        PyObject* result = PyObject_CallObject(func, args);
        Py_DECREF(args);
        Py_DECREF(func);
        if (!result) {
            USE_LOG_ERROR("PythonScripting: Error calling function '%s'", funcName.c_str());
            PyErr_Print();
            return false;
        }
        Py_DECREF(result);
        return true;
    }

    bool PythonScripting::CallFunction(const std::string& funcName, const std::string& arg) {
        if (!m_initialized) return false;
        PyObject* func = PyObject_GetAttrString(PyImport_AddModule("__main__"), funcName.c_str());
        if (!func || !PyCallable_Check(func)) {
            Py_XDECREF(func);
            return false;
        }
        PyObject* args = Py_BuildValue("(s)", arg.c_str());
        PyObject* result = PyObject_CallObject(func, args);
        Py_DECREF(args);
        Py_DECREF(func);
        if (!result) {
            USE_LOG_ERROR("PythonScripting: Error calling function '%s'", funcName.c_str());
            PyErr_Print();
            return false;
        }
        Py_DECREF(result);
        return true;
    }

    PyObject* PythonScripting::GetAttribute(const std::string& name) {
        if (!m_initialized) return nullptr;
        return PyObject_GetAttrString(PyImport_AddModule("__main__"), name.c_str());
    }

    bool PythonScripting::HasFunction(const std::string& funcName) {
        if (!m_initialized) return false;
        PyObject* func = PyObject_GetAttrString(PyImport_AddModule("__main__"), funcName.c_str());
        bool result = func && PyCallable_Check(func);
        Py_XDECREF(func);
        return result;
    }

} // namespace USE