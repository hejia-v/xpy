#include "xpy.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <Python.h>
#include <boost/filesystem.hpp>

//#define  BOOST_DISABLE_ASSERTS
#include <boost/assert.hpp>

namespace fs = boost::filesystem;
using namespace std;

static wchar_t* py_program_name = nullptr;
static wchar_t* py_home_path = nullptr;

DebugCallback gDebugCallback;

void RegisterDebugCallback(DebugCallback callback)
{
    if (callback)
    {
        gDebugCallback = callback;
    }
}

void DebugInUnity(std::string message)
{
    if (gDebugCallback)
    {
        gDebugCallback(message.c_str());
    }
}

const char* Native_GetCurrentPath()
{
    fs::path sCurrPath = boost::filesystem::current_path();
    size_t iBufferSize = sCurrPath.string().length() + 2;
    char *pBuffer = new char[iBufferSize];

    if (nullptr != pBuffer)
    {
         strcpy(pBuffer, sCurrPath.string().c_str());
    }
    return pBuffer;
}

void Native_ReleaseMemory(void *pBuffer)
{
    if (nullptr != pBuffer)
    {
        delete pBuffer;
        pBuffer = nullptr;
    }
}

void Python_Start(const char* program, const char* home)
{
    BOOST_ASSERT(program != nullptr && home != nullptr);
    size_t len, converted;

    len = strlen(program) + 1;
    converted = 0;
    if (py_program_name != nullptr)
    {
        delete[] py_program_name;
        py_program_name = nullptr;
    }
    py_program_name = new wchar_t[len];
    mbstowcs_s(&converted, py_program_name, len, program, _TRUNCATE);
    Py_SetProgramName(py_program_name); /* optional but recommended */

    len = strlen(home) + 1;
    converted = 0;
    if (py_home_path != nullptr)
    {
        delete[] py_home_path;
        py_home_path = nullptr;
    }
    py_home_path = new wchar_t[len];
    mbstowcs_s(&converted, py_home_path, len, home, _TRUNCATE);
    Py_SetPythonHome(py_home_path);

    Py_Initialize();
    PyRun_SimpleString("from time import time,ctime\n"
        "print('Today is', ctime(time()))\n");
}

bool Python_CheckInterpreter(const char *program)
{
    wstringstream wss1;
    wss1 << Py_GetProgramName();
    wstringstream wss2;
    wss2 << program;
    bool isEmbedded = wss1.str() == wss2.str();
    return isEmbedded;
}

bool Python_Finalize()
{
    if (Py_FinalizeEx() < 0)
    {
        return false;
    }
    return true;
}

void Python_RegisterModule()  // 在Py_Initialize之前调用
{
    DebugInUnity("register python module");
}























// #include <boost/python.hpp>



// namespace python = boost::python;

// template <class T>
// void safe_execute(T functor);






// void _python_eval_expression(const char *expression, std::string *result)
// {
//     python::object oPyMainModule = python::import("__main__");
//     python::object oPyMainNamespace = oPyMainModule.attr("__dict__");
//     python::object oResult = python::eval(expression, oPyMainNamespace);
//     *result = python::extract<std::string>(oResult) BOOST_EXTRACT_WORKAROUND;
// }

// std::string python_eval(const char *expression)  // eval函数可以计算Python表达式，并返回结果
// {
//     std::string sValue = "";
//     safe_execute(boost::bind(_python_eval_expression, expression, &sValue));
//     return sValue;
// }

// void _python_exec_code(const char *code)
// {
//     python::object oPyMainModule = python::import("__main__");
//     python::object oPyMainNamespace = oPyMainModule.attr("__dict__");
//     python::object oResult = python::exec(code, oPyMainNamespace);
// }

// void python_exec(const char *code)  // 通过exec可以执行动态Python代码，exec不返回结果
// {
//     safe_execute(boost::bind(_python_exec_code, code));
// }

// void _python_exec_file(const char *filename)
// {
//     python::object oPyMainModule = python::import("__main__");
//     python::object oPyMainNamespace = oPyMainModule.attr("__dict__");
//     python::object result = python::exec_file(filename, oPyMainNamespace, oPyMainNamespace);
// }

// void python_exec_file(const char *filename)
// {
//     safe_execute(boost::bind(_python_exec_file, filename));
// }










// template <class T>
// void safe_execute(T functor)
// {
//     void check_pyerr(bool pyerr_expected = false);
//     if (python::handle_exception(functor))
//     {
//         check_pyerr();
//     }
// };

// void check_pyerr(bool pyerr_expected = false)
// {
//     if (PyErr_Occurred())
//     {
//         if (!pyerr_expected)
//         {
//             //BOOST_ERROR("Python错误");
//             if (PyErr_ExceptionMatches(PyExc_SyntaxError))
//             {
//                 void log_python_exception();
//                 log_python_exception();
//             }
//             else
//             {
//                 PyErr_Print();
//             }
//         }
//         else
//             PyErr_Clear();
//     }
//     //else
//         //BOOST_ERROR("一个C++表达式被抛出，这里没有表达式句柄被注册r.");
// }

// std::string strErrorMsg;

// void log_python_exception()
// {
//     if (!Py_IsInitialized())
//     {
//         strErrorMsg = "Python运行环境没有初始化!";
//         return;
//     }
//     if (PyErr_Occurred() != NULL)
//     {
//         PyObject *type_obj, *value_obj, *traceback_obj;
//         PyErr_Fetch(&type_obj, &value_obj, &traceback_obj);
//         if (value_obj == NULL)
//             return;

//         strErrorMsg.clear();
//         PyErr_NormalizeException(&type_obj, &value_obj, 0);
//         if (PyUnicode_Check(PyObject_Str(value_obj)))
//         {
//             strErrorMsg = _PyUnicode_AsString(PyObject_Str(value_obj));
//         }

//         if (traceback_obj != NULL)
//         {
//             strErrorMsg += "\nTraceback:";
//             PyObject *pModuleName = PyUnicode_FromString("traceback");
//             PyObject *pTraceModule = PyImport_Import(pModuleName);
//             Py_XDECREF(pModuleName);
//             if (pTraceModule != NULL)
//             {
//                 PyObject *pModuleDict = PyModule_GetDict(pTraceModule);
//                 if (pModuleDict != NULL)
//                 {
//                     PyObject *pFunc = PyDict_GetItemString(pModuleDict, "format_exception");
//                     if (pFunc != NULL)
//                     {
//                         PyObject *errList = PyObject_CallFunctionObjArgs(pFunc, type_obj, value_obj, traceback_obj, NULL);
//                         if (errList != NULL)
//                         {
//                             Py_ssize_t listSize = PyList_Size(errList);
//                             for (Py_ssize_t i = 0; i < listSize; ++i)
//                             {
//                                 strErrorMsg += _PyUnicode_AsString(PyList_GetItem(errList, i));
//                             }
//                         }
//                     }
//                 }
//                 Py_XDECREF(pTraceModule);
//             }
//         }
//         Py_XDECREF(type_obj);
//         Py_XDECREF(value_obj);
//         Py_XDECREF(traceback_obj);
//     }
//     strErrorMsg.append("\n");
//     //cwrite(strErrorMsg.c_str(), "red_h");
// }

