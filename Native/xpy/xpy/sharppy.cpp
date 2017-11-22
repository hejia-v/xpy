#include <string>
/**
* Since Python may define some pre-processor definitions which affect the standard headers
* on some systems, you must include Python.h before any standard headers are included
*/
#include <Python.h> // 里面的一些宏会影响 ndk r15 cstdio 里一些函数的定义
#include "sharppy.h"
#include "helper/log.h"
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

#include "common/util.h"
#include "common/resmgr.h"
#include "xpyhelp.h"

namespace fs = boost::filesystem;
using namespace std;
using namespace xpy;

#define MAXRET 256

static wchar_t* g_PyProgramName = nullptr;
static wchar_t* g_PyHomePath = nullptr;

static csharp_callback sharp_cb = nullptr;

static PyObject *func_proxy = nullptr;
static PyObject *func_object = nullptr;
static PyObject *func_garbage = nullptr;

void Native_ReleaseMemory(void **pBuffer)
{
    if (nullptr != *pBuffer)
    {
        delete *pBuffer;
        *pBuffer = nullptr;
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

void Python_RegisterModule()  // 在Py_Initialize之前调用
{
    int register_sharppy_functions();
    register_sharppy_functions();
    logger::info("register python module");
}

bool Python_Start(const char* program, const char* home)
{
    BOOST_ASSERT(program != nullptr && home != nullptr);

    string shome = fs::path(home).lexically_normal().make_preferred().string();
    boost::replace_all(shome, "\\", "/");  // python识别不了反斜杠

    g_PyProgramName = char2wchar(program);
    Py_SetProgramName(g_PyProgramName); /* optional but recommended */

    g_PyHomePath = char2wchar(shome.c_str());
    Py_SetPythonHome(g_PyHomePath);

    Py_Initialize();
    if (!Py_IsInitialized())
    {
        // TODO: 如果失败，将stderr里的信息提取出来,Python_Start加一个void **err参数, 输出错误信息
        logger::error("Py_Initialize is failed!\n");
        return false;
    }
    PyRun_SimpleString("print('python start!')");
    PyRun_SimpleString("from time import time,ctime\n"
                       "print('Today is', ctime(time()))\n");
    return true;
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
    reset_log_handler();

    if (Py_FinalizeEx() != 0)
    {
        return false;
    }
    return true;
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
    m = PyImport_AddModule("__main__");  // Borrowed reference
    if (m == NULL)
    {
        logger::error("Failed to import __main__");
        return -1;
    }
    d = PyModule_GetDict(m);  // Borrowed reference
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
    sharp_cb = cb;

    PyObject *pModule, *pClass;
    int ret = 0;

    pModule = PyImport_ImportModule("sharp");
    if (pModule != NULL)
    {
        pClass = PyObject_GetAttrString(pModule, "sharp");
        if (pClass)
        {
            func_proxy = PyObject_GetAttrString(pClass, "_proxy");
            if (!(func_proxy && PyCallable_Check(func_proxy)))
            {
                logger::error("Cannot find method \"{}\"", "_proxy");
                ret = -1;
            }

            func_object = PyObject_GetAttrString(pClass, "_object");
            if (!(func_object && PyCallable_Check(func_object)))
            {
                logger::error("Cannot find method \"{}\"", "_object");
                ret = -1;
            }

            func_garbage = PyObject_GetAttrString(pClass, "_garbage");
            if (!(func_garbage && PyCallable_Check(func_garbage)))
            {
                logger::error("Cannot find method \"{}\"", "_garbage");
                ret = -1;
            }

            Py_DECREF(pClass);
        }
        else
        {
            logger::error("Cannot find class \"{}\"", "sharp");
            Py_DECREF(pModule);
            return -1;
        }
        Py_DECREF(pModule);
    }
    else
    {
        logger::error("Failed to load module: \"{}\"", "sharp");
        return -1;
    }
    return ret;
}

int Python_GetFunction(const char *module, const char *funcname, int *id, const char **err)
{
    PyObject *pModule, *pFunc, *pArgs, *pValue;
    std::string s;

    *id = 0;

    pModule = PyImport_ImportModule(module);
    if (pModule != NULL)
    {
        pFunc = PyObject_GetAttrString(pModule, funcname);
        if (pFunc == NULL)
        {
            s = fmt::format("Cannot find function \"{}\"", funcname);
            error_nc(*err, s.c_str());
            Py_DECREF(pModule);
            return -1;
        }
        if (!PyCallable_Check(pFunc))
        {
            const char *type_name = pFunc->ob_type->tp_name;
            s = fmt::format("Invalid type {} for [{}.{}]", type_name, module, funcname);
            error_nc(*err, s.c_str());
            Py_DECREF(pFunc);
            Py_DECREF(pModule);
            return -1;
        }

        pArgs = PyTuple_New(1);
        PyTuple_SetItem(pArgs, 0, pFunc);  // PyTuple_SetItem "steals" a reference to pFunc.
        Py_INCREF(pFunc);
        pValue = PyObject_CallObject(func_proxy, pArgs);
        Py_DECREF(pArgs);

        if (pValue == NULL)
        {
            Py_DECREF(pFunc);
            Py_DECREF(pModule);
            error_nc(*err, "call sharp._proxy failed");
            return -1;
        }

        PyObject *pRet, *pRetValue;
        pRet = PyTuple_GetItem(pValue, 0);  // Borrowed reference
        const char *type = PyUnicode_AsUTF8(pRet);

        if (type == NULL || type[0] != 'P')
        {
            Py_DECREF(pValue);
            Py_DECREF(pFunc);
            Py_DECREF(pModule);
            error_nc(*err, "Not a python object");
            return -1;
        }

        pRet = PyTuple_GetItem(pValue, 1);
        pRetValue = PyNumber_Long(pRet);
        long n = PyLong_AsLong(pRetValue);
        *id = n;

        Py_DECREF(pRetValue);
        Py_DECREF(pValue);
        Py_DECREF(pFunc);
        Py_DECREF(pModule);
    }
    else
    {
        error_nc(*err, "Failed to load module: \"sharp\"");
        return -1;
    }

    return 0;
}

int args_to_pyargs(PyObject **ppArgs, int argc, var *argv, int strc, const char **strs, const char **err)
{
    PyObject *pArgs, *pValue;

    pArgs = PyTuple_New(argc);
    for (int i = 0; i < argc; i++)
    {
        var v = argv[i];
        var_type t = v.type;
        switch (t)
        {
        case var_type::NONE:
            PyTuple_SetItem(pArgs, i, Py_None);
            Py_INCREF(Py_None);
            break;
        case var_type::INTEGER:
            pValue = PyLong_FromLong(v.d);
            PyTuple_SetItem(pArgs, i, pValue);
            break;
        case var_type::INT64:
            pValue = PyLong_FromLongLong(v.d64);
            PyTuple_SetItem(pArgs, i, pValue);
            break;
        case var_type::REAL:
            pValue = PyFloat_FromDouble(v.f);
            PyTuple_SetItem(pArgs, i, pValue);
            break;
        case var_type::BOOLEAN:
            if (v.d)
            {
                PyTuple_SetItem(pArgs, i, Py_True);
                Py_INCREF(Py_True);
            }
            else
            {
                PyTuple_SetItem(pArgs, i, Py_False);
                Py_INCREF(Py_False);
            }
            break;
        case var_type::STRING:
            // todo: add short string cache
            if (strs)
            {
                if (v.d < 0 || v.d >= strc)
                {
                    error_nc(*err, "Invalid string id");
                    Py_DECREF(pArgs);
                    return -1;
                }
                pValue = PyUnicode_FromString(strs[v.d]);
            }
            else
            {
                pValue = PyUnicode_FromString((const char *)v.ptr);
            }
            PyTuple_SetItem(pArgs, i, pValue);
            break;
        case var_type::POINTER:
            pValue = PyCapsule_New(v.ptr, NULL, NULL);
            PyTuple_SetItem(pArgs, i, pValue);
            break;
        case var_type::PYTHONOBJ:
        case var_type::SHARPOBJ:
            PyObject *pObjectArgs, *pType, *pId;

            pObjectArgs = PyTuple_New(2);
            if (t == var_type::PYTHONOBJ)
                pType = PyUnicode_FromString("P");
            else
                pType = PyUnicode_FromString("S");
            PyTuple_SetItem(pObjectArgs, 0, pType);

            pId = PyLong_FromLong(v.d);
            PyTuple_SetItem(pObjectArgs, 1, pId);

            pValue = PyObject_CallObject(func_object, pObjectArgs);
            Py_DECREF(pObjectArgs);

            if (pValue == NULL)
            {
                error_nc(*err, "call sharp._object failed");
                Py_DECREF(pArgs);
                return -1;
            }

            PyTuple_SetItem(pArgs, i, pValue);
            break;
        default:
            std::string s = fmt::format("Invalid type {}", (int)v.type);
            error_nc(*err, s.c_str());
            Py_DECREF(pArgs);
            return -1;
        }
    }
    *ppArgs = pArgs;
    return 0;
}

int marshal_var(var &v, PyObject *pItem)
{
    PyObject *pArgs, *pValue;

    if (Py_None == pItem)
    {
        v.type = var_type::NONE;
    }
    else if (PyBool_Check(pItem) == 1)
    {
        v.type = var_type::BOOLEAN;
        if (Py_True == pItem)
            v.d = 1;
        else
            v.d = 0;
    }
    else if (PyNumber_Check(pItem) == 1)
    {
        if (PyLong_Check(pItem) == 1)
        {
            v.type = var_type::INTEGER;
            pValue = PyNumber_Long(pItem);

            int overflow;
            long result = PyLong_AsLongAndOverflow(pValue, &overflow);
            if (overflow)
            {
                v.type = var_type::INT64;
                v.d64 = PyLong_AsLongLong(pValue);
            }
            else
            {
                v.type = var_type::INTEGER;
                v.d = result;
            }
            Py_DECREF(pValue);
        }
        else if (PyFloat_Check(pItem) == 1)
        {
            v.type = var_type::REAL;
            pValue = PyNumber_Float(pItem);
            v.f = PyFloat_AsDouble(pValue);
            Py_DECREF(pValue);
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "Unsupported PyNumber argument.");
            return -1;
        }
    }
    else if (PyUnicode_Check(pItem) == 1)
    {
        v.type = var_type::STRING;
        const char *s = PyUnicode_AsUTF8(pItem);
        char *new_s = new char[strlen(s) + 1];
        strcpy(new_s, s);
        v.str = (void *)new_s;  // 接收者释放
    }
    else if (PyCapsule_CheckExact(pItem) == 1)
    {
        v.type = var_type::POINTER;
        v.ptr = PyCapsule_GetPointer(pItem, NULL);
    }
    else
    {
        // func_proxy: call sharp._proxy to get proxy
        pArgs = PyTuple_New(1);
        PyTuple_SetItem(pArgs, 0, pItem);
        Py_INCREF(pItem);
        pValue = PyObject_CallObject(func_proxy, pArgs);
        Py_DECREF(pArgs);

        if (pValue == NULL)
        {
            return -1;  // call _proxy failed
        }

        PyObject *pRet, *pRetValue;
        pRet = PyTuple_GetItem(pValue, 0);
        const char *type = PyUnicode_AsUTF8(pRet);
        if (type == NULL)
        {
            PyErr_SetString(PyExc_RuntimeError, "Invalid proxy function.");
            Py_DECREF(pValue);
            return -1;
        }
        else
        {
            if (type[0] == 'S')
            {
                v.type = var_type::SHARPOBJ;
            }
            else // type[0] == 'P'
            {
                v.type = var_type::PYTHONOBJ;
            }

            pRet = PyTuple_GetItem(pValue, 1);
            pRetValue = PyNumber_Long(pRet);
            long n = PyLong_AsLong(pRetValue);
            v.d = n;
            Py_DECREF(pRetValue);
        }
        Py_DECREF(pValue);
    }
    return 1;
}

int Python_CreateClassInstance(const char *module, const char *classname, int *id, int argc, var *argv, int strc, const char **strs, const char **err)
{
    PyObject *pModule, *pClass;
    std::string s;

    pModule = PyImport_ImportModule(module);
    if (pModule != NULL)
    {
        pClass = PyObject_GetAttrString(pModule, classname);
        Py_DECREF(pModule);
        if (!pClass)
        {
            s = fmt::format("Cannot find class \"{}\"", classname);
            error_nc(*err, s.c_str());
            return -1;
        }
    }
    else
    {
        s = fmt::format("Failed to load module: \"{}\"", module);
        error_nc(*err, s.c_str());
        return -1;
    }

    PyObject *pArgs, *pInstance;
    if (args_to_pyargs(&pArgs, argc, argv, strc, strs, err) != 0)
    {
        return -1;
    }

    pInstance = PyObject_Call(pClass, pArgs, NULL);
    Py_DECREF(pClass);
    Py_DECREF(pArgs);

    if (pInstance == NULL)
    {
        s = fmt::format("Create class instance failed.\n{}", fetch_py_exception_msg());
        error_nc(*err, s.c_str());
        return -1;
    }

    int marshal_var(var &v, PyObject *pItem);
    if (marshal_var(argv[0], pInstance) != 1)
    {
        Py_DECREF(pInstance);
        error_nc(*err, "Marshal var failed");
        return -1;
    }

    Py_DECREF(pInstance);
    return 0;
}

int Python_CallFunction(int argc, var *argv, int strc, const char **strs, const char **err)
{
    assert(*err == 0);

    if (argc <= 0 || argv->type != var_type::PYTHONOBJ)
    {
        error_nc(*err, "Need Function");
        return -1;
    }

    PyObject *pFunc, *pArgs, *pPArgs, *pValue;

    if (args_to_pyargs(&pArgs, argc, argv, strc, strs, err) != 0)
    {
        return -1;
    }

    pFunc = PyTuple_GetItem(pArgs, 0);
    if (argc == 1)
    {
        pValue = PyObject_CallObject(pFunc, NULL);
    }
    else
    {
        pPArgs = PySequence_GetSlice(pArgs, 1, PySequence_Size(pArgs));
        pValue = PyObject_CallObject(pFunc, pPArgs);
        Py_DECREF(pPArgs);
    }
    Py_DECREF(pArgs);

    if (pValue == NULL)
    {
        std::string s = fmt::format("Call python function failed.\n{}", fetch_py_exception_msg());
        error_nc(*err, s.c_str());
        return -1;
    }

    int marshal_arguments(var *v, PyObject *args);

    int result = marshal_arguments(argv, pValue);
    Py_DECREF(pValue);
    return result;
}

int marshal_arguments(var *v, PyObject *args)
{
    if (!PyTuple_Check(args))
    {
        if (marshal_var(v[0], args) != 1)
        {
            return -1;
        }
        return 1;
    }
    else
    {
        Py_ssize_t size = PyTuple_Size(args);

        if (size <= 0)
        {
            if (!PyErr_Occurred())
                PyErr_SetString(PyExc_TypeError, "You must supply at least one argument.");
            return -1;
        }

        PyObject *pItem;

        for (Py_ssize_t i = 0; i < size; i++)
        {
            pItem = PyTuple_GetItem(args, i); // dosen't increase refcount
            if (pItem == NULL)
            {
                return -1;
            }

            if (marshal_var(v[i], pItem) != 1)
            {
                return -1;
            }

            if (PyErr_Occurred())
            {
                return -1;
            }
        }

        return (int)size;
    }
}

int Python_GetInstanceMethod(const char *method, var *argv, const char **err)
{
    PyObject *pObjArgs, *pType, *pId, *pInstance;

    pObjArgs = PyTuple_New(2);
    pType = PyUnicode_FromString("P");
    PyTuple_SetItem(pObjArgs, 0, pType);

    pId = PyLong_FromLong(argv[0].d);
    PyTuple_SetItem(pObjArgs, 1, pId);

    pInstance = PyObject_CallObject(func_object, pObjArgs);
    Py_DECREF(pObjArgs);

    if (pInstance == NULL)
    {
        error_nc(*err, "call sharp._object failed");
        return -1;
    }

    PyObject *pMethod;
    std::string s;

    pMethod = PyObject_GetAttrString(pInstance, method);
    const char *classname = "";  // TODO:
    Py_DECREF(pInstance);
    if (!pMethod)
    {
        s = fmt::format("Cannot find method \"{}.{}\"", classname, method);
        error_nc(*err, s.c_str());
        return -1;
    }
    if (!PyCallable_Check(pMethod))
    {
        const char *type_name = pMethod->ob_type->tp_name;
        s = fmt::format("Invalid type {} for [{}.{}]", type_name, classname, method);
        error_nc(*err, s.c_str());
        Py_DECREF(pMethod);
        return -1;
    }

    if (marshal_var(argv[0], pMethod) != 1)
    {
        Py_DECREF(pMethod);
        error_nc(*err, "Marshal var failed");
        return -1;
    }

    Py_DECREF(pMethod);
    return 0;
}

int Python_SharpCollectGarbage(int n, int *result)
{
    PyObject *pValue, *pNum;
    int ret = 0;

    for (int i = 0; i < n; i++)
    {
        pValue = PyObject_CallObject(func_garbage, NULL);

        if (pValue == NULL)
        {
            break; // ignore error message
        }

        if (Py_None == pValue)
        {
            Py_DECREF(pValue);
            break;
        }

        pNum = PyNumber_Long(pValue);

        int overflow;
        long id = PyLong_AsLongAndOverflow(pNum, &overflow);
        if (overflow)
        {
            // TODO:
        }
        else
        {
            result[ret] = id;
            ret++;
        }
        Py_DECREF(pValue);
        Py_DECREF(pNum);
    }
    return ret;
}

vector<string> ParseArgs(const char* args)
{
    vector<string> tokens;
    boost::split(tokens, args, boost::is_any_of(","));  // boost::is_any_of这里相当于分割规则
    for_each(tokens.begin(), tokens.end(), [](string &s) {boost::trim(s); });
    tokens.erase(remove_if(tokens.begin(), tokens.end(), [](string s) {return s == ""; }), tokens.end());
    return tokens;
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

    if (pModule == NULL)
    {
        PyErr_Print();
        logger::error("Failed to load \"{}\"", pythonfile);
        return -1;
    }

    pFunc = PyObject_GetAttrString(pModule, funcname);
    /* pFunc is a new reference */

    if (!PyCallable_Check(pFunc))
    {

        if (PyErr_Occurred())
            PyErr_Print();
        logger::error("Cannot find function \"{}\"", funcname);
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
        return -1;
    }

    vector<string> vargs = ParseArgs(args);
    pArgs = PyTuple_New(vargs.size());
    for (i = 0; i < vargs.size(); ++i)
    {
        pValue = PyUnicode_FromString(vargs[i].c_str());
        if (pValue == NULL)
        {
            Py_DECREF(pArgs);
            Py_DECREF(pFunc);
            Py_DECREF(pModule);
            logger::error("Cannot convert argument");
            return -1;
        }
        /* pValue reference stolen here: */
        PyTuple_SetItem(pArgs, i, pValue);
    }
    pValue = PyObject_CallObject(pFunc, pArgs);
    Py_DECREF(pArgs);

    if (pValue == NULL)
    {
        Py_DECREF(pFunc);
        Py_DECREF(pModule);
        PyErr_Print();
        logger::error("Call failed");
        return -1;
    }

    logger::debug("Result of call: {}", PyLong_AsLong(pValue));
    Py_DECREF(pValue);
    Py_DECREF(pFunc);
    Py_DECREF(pModule);

    return 0;
}

static PyObject *xpy_csharpcall(PyObject *self, PyObject *args)
{
    var arg[MAXRET];
    int argsnum = marshal_arguments(arg, args);

    logger::info("xpy_csharpcall argc: {}", argsnum);
    if (argsnum == -1)
    {
        return NULL; // when use PyErr_SetString in a c extension function, you must return NULL.
    }

    string_pusher sp = {0};
    if (sharp_cb)
    {
        int rt = sharp_cb(argsnum, arg, &sp);
        // TODO: 根据具体需求完善
    }

    Py_RETURN_NONE;
}

static PyObject * xpy_writelog(PyObject *self, PyObject *args)
{
    int level;
    const char *msg;

    if (!PyArg_ParseTuple(args, "is", &level, &msg))
        return NULL;
    xlog(level, msg, false);
    Py_RETURN_NONE;
}

static PyMethodDef EmbMethods[] = {
    {"csharpcall",  xpy_csharpcall, METH_VARARGS,
     "call csharp."},
    {"writelog",  xpy_writelog, METH_VARARGS,
     "output log."},
    {NULL, NULL, 0, NULL}
};

static PyModuleDef EmbModule = {
    PyModuleDef_HEAD_INIT, "xpy", NULL, -1, EmbMethods,
    NULL, NULL, NULL, NULL
};

static PyObject *PyInit_emb(void)
{
    return PyModule_Create(&EmbModule);
}

int register_sharppy_functions()
{
    PyImport_AppendInittab("xpy", &PyInit_emb);
    return 0;
}

// -------------------------------------------------------------------------------------
int SHARPPY_init()
{
    ResMgr::instance();
    return 1;
}

int SHARPPY_uninit()
{
    ResMgr::destroyInstance();
    return 1;
}

int SHARPPY_AddScriptSearchPath(const char *realpath, const char *mntpoint, int append)
{
    return ResMgr::getInstance()->AddToSearchPath(realpath, mntpoint, append);
}
