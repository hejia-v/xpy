#include <Python.h>
// Since Python may define some pre-processor definitions which affect the standard headers
// on some systems, you must include Python.h before any standard headers are included
#include "xpy.h"
#include "log.h"
#include "pybind_xpy_manual.h"
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>

//#define  BOOST_DISABLE_ASSERTS
#include <boost/assert.hpp>

namespace fs = boost::filesystem;
using namespace std;
using namespace xpy;

static wchar_t* g_PyProgramName = nullptr;
static wchar_t* g_PyHomePath = nullptr;

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

void Native_ReleaseMemory(void **pBuffer)
{
    if (nullptr != *pBuffer)
    {
        delete *pBuffer;
        *pBuffer = nullptr;
    }
}

void Python_Start(const char* program, const char* home)
{
    BOOST_ASSERT(program != nullptr && home != nullptr);

    charstr_to_wcharstr(program, g_PyProgramName);
    Py_SetProgramName(g_PyProgramName);  /* optional but recommended */

    charstr_to_wcharstr(home, g_PyHomePath);
    Py_SetPythonHome(g_PyHomePath);

    Py_Initialize();
    //TODO: Py_IsInitialized检查是否成功, 如果失败，将stderr里的信息提取出来,Python_Start加一个void **err参数, 输出错误信息
    PyRun_SimpleString("print('python start!')");
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
    register_xpy_functions();
    logger::info("register python module");
}

vector<string> ParseArgs(const char* args)
{
    vector<string> tokens;
    boost::split(tokens, args, boost::is_any_of(","));  // boost::is_any_of这里相当于分割规则
    for_each(tokens.begin(), tokens.end(), [](string &s) {boost::trim(s); });
    tokens.erase(remove_if(tokens.begin(), tokens.end(), [](string s) {return s == ""; }), tokens.end());
    return tokens;
}

int Python_InitScript(const char *scriptroot)
{
    fs::path f(scriptroot);
    string root = f.lexically_normal().make_preferred().string();
    boost::replace_all(root, "\\", "/");  // python识别不了反斜杠
    logger::info("script root: {}", root.c_str());

    string script_str = "g_ScriptRoot = '" + root + "'\n"
        "import sys\n"
        "sys.path.append(g_ScriptRoot)\n";

    if (0 != Python_RunString(script_str.c_str()))
    {
        logger::error("Failed to init python script");
        return -1;
    }

    return 0;
}

int Python_RunString(const char* script)
{
    PyObject *m, *d, *v;
    m = PyImport_AddModule("__main__");
    if (m == NULL)
    {
        logger::error("Failed to import __main__");
        return -1;
    }
    d = PyModule_GetDict(m);
    v = PyRun_StringFlags(script, Py_file_input, d, d, NULL);
    if (v == NULL)
    {
        PyErr_Print();
        logger::error("Failed to run python script");
        return -1;
    }
    Py_DECREF(v);

    return 0;
}

int Python_InitSharpCall(csharp_callback cb)
{
    int ret = init_csharp_python_funcs(cb);  // 注册sharp的回调
    return ret;
}

const char *Python_GetFunction(const char *module, const char *funcname, int *id)
{
    const char *error = get_python_function(module, funcname, id);
    return error;
}

int Python_CallFunction(int argc, var *argv, int strc, const char **strs, const char **err)
{
    return call_python_function(argc, argv, strc, strs, err);
}

int Python_SharpCollectGarbage(int n, int *result)
{
    return sharp_collect_garbage(n, result);
}

int Python_RunFunction(const char* pythonfile, const char* funcname, const char* args)
{
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pValue;
    int i;

    pName = PyUnicode_DecodeFSDefault(pythonfile);
    /* Error checking of pName left out */

    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL)
    {
        pFunc = PyObject_GetAttrString(pModule, funcname);
        /* pFunc is a new reference */

        if (pFunc && PyCallable_Check(pFunc))
        {
            vector<string> vargs = ParseArgs(args);
            pArgs = PyTuple_New(vargs.size());
            for (i = 0; i < vargs.size(); ++i)
            {
                pValue = PyUnicode_FromString(vargs[i].c_str());
                if (!pValue)
                {
                    Py_DECREF(pArgs);
                    Py_DECREF(pModule);
                    logger::error("Cannot convert argument");
                    return 1;
                }
                /* pValue reference stolen here: */
                PyTuple_SetItem(pArgs, i, pValue);
            }
            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            if (pValue != NULL)
            {
                logger::debug("Result of call: {}", PyLong_AsLong(pValue));
                Py_DECREF(pValue);
            }
            else
            {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                logger::error("Call failed");
                return 1;
            }
        }
        else
        {
            if (PyErr_Occurred())
                PyErr_Print();
            logger::error("Cannot find function \"{}\"", funcname);
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else
    {
        PyErr_Print();
        logger::error("Failed to load \"{}\"", pythonfile);
        return 1;
    }
    return 0;
}
