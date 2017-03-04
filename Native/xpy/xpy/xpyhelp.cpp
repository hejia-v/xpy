#include <Python.h>
#include "xpyhelp.h"
#include <string>

static std::string g_ErrorMsg;

const char *fetch_py_exception_msg()
{
    if (!Py_IsInitialized())
    {
        g_ErrorMsg = "Python is not initialized!";
        return "";
    }
    if (PyErr_Occurred() != NULL)
    {
        PyObject *type_obj, *value_obj, *traceback_obj;

        PyErr_Fetch(&type_obj, &value_obj, &traceback_obj);
        if (value_obj == NULL)
            return "";

        g_ErrorMsg.clear();

        PyErr_NormalizeException(&type_obj, &value_obj, NULL);
        if (PyUnicode_Check(PyObject_Str(value_obj)))
        {
            g_ErrorMsg = _PyUnicode_AsString(PyObject_Str(value_obj));
        }

        if (traceback_obj != NULL)
        {
            g_ErrorMsg += "\nTraceback:";
            PyObject *pModuleName = PyUnicode_FromString("traceback");
            PyObject *pTraceModule = PyImport_Import(pModuleName);
            Py_XDECREF(pModuleName);
            if (pTraceModule != NULL)
            {
                PyObject *pModuleDict = PyModule_GetDict(pTraceModule);
                if (pModuleDict != NULL)
                {
                    PyObject *pFunc = PyDict_GetItemString(pModuleDict, "format_exception");
                    if (pFunc != NULL)
                    {
                        PyObject *errList = PyObject_CallFunctionObjArgs(pFunc, type_obj, value_obj, traceback_obj, NULL);
                        if (errList != NULL)
                        {
                            Py_ssize_t listSize = PyList_Size(errList);
                            for (Py_ssize_t i = 0; i < listSize; ++i)
                            {
                                g_ErrorMsg += _PyUnicode_AsString(PyList_GetItem(errList, i));
                            }
                        }
                    }
                }
                Py_XDECREF(pTraceModule);
            }
        }
        Py_XDECREF(type_obj);
        Py_XDECREF(value_obj);
        Py_XDECREF(traceback_obj);
    }
    g_ErrorMsg.append("\n");

    return g_ErrorMsg.c_str();
}
