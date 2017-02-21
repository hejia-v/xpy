#include <Python.h>
// Since Python may define some pre-processor definitions which affect the standard headers
// on some systems, you must include Python.h before any standard headers are included
#include "xpy.h"
#include "log.h"
#include "pybind_util_manual.h"
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
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
    if (g_PyProgramName != nullptr)
    {
        delete[] g_PyProgramName;
        g_PyProgramName = nullptr;
    }
    g_PyProgramName = new wchar_t[len];
    mbstowcs_s(&converted, g_PyProgramName, len, program, _TRUNCATE);
    Py_SetProgramName(g_PyProgramName); /* optional but recommended */

    len = strlen(home) + 1;
    converted = 0;
    if (g_PyHomePath != nullptr)
    {
        delete[] g_PyHomePath;
        g_PyHomePath = nullptr;
    }
    g_PyHomePath = new wchar_t[len];
    mbstowcs_s(&converted, g_PyHomePath, len, home, _TRUNCATE);
    Py_SetPythonHome(g_PyHomePath);

    Py_Initialize();
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
    register_util_functions();
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
    logger::info("read pythonfile begin");
    ifstream infile;
    infile.open(scriptroot + string("/main.py"));
    string ret = "";
    string s;
    while (getline(infile, s))
    {
        ret += s + "\n";
    }
    infile.close();
    logger::info(ret.c_str());
    logger::info("read pythonfile end");
    logger::info(scriptroot);

    string script_str = "g_ScriptRoot = '" + string(scriptroot) + "'\n"
        "import sys\n"
        "sys.path.append(g_ScriptRoot)\n";

    PyObject *m, *d, *v;
    m = PyImport_AddModule("__main__");
    if (m == NULL)
    {
        logger::error("Failed to import __main__");
        return -1;
    }
    d = PyModule_GetDict(m);
    v = PyRun_StringFlags(script_str.c_str(), Py_file_input, d, d, NULL);
    if (v == NULL)
    {
        PyErr_Print();
        logger::error("Failed to init python script");
        return -1;
    }
    Py_DECREF(v);

    return 0;
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
