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

const char* native_get_current_path()
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

void native_release_memory(void *pBuffer)
{
    if (nullptr != pBuffer)
    {
        delete pBuffer;
        pBuffer = nullptr;
    }
}

void python_start(const char* program, const char* home)
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

bool python_check_interpreter(const char *program)
{
    wstringstream wss1;
    wss1 << Py_GetProgramName();
    wstringstream wss2;
    wss2 << program;
    bool isEmbedded = wss1.str() == wss2.str();
    return isEmbedded;
}

bool python_finalize()
{
    if (Py_FinalizeEx() < 0)
    {
        return false;
    }
    return true;
}
