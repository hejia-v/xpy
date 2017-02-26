#include <Python.h>
#include "xpy.h"
#include "log.h"
#include <string>
using namespace xpy;

#define MAXRET 256

static int numargs = 5;
static csharp_callback sharp_cb = nullptr;

static PyObject *func_proxy = nullptr;
static PyObject *func_object = nullptr;
static PyObject *func_garbage = nullptr;

int init_csharp_python_funcs(csharp_callback cb)
{
    sharp_cb = cb;

    PyObject *pModule, *pClass;
    int ret = 1;

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

/* Return the number of arguments of the application command line */
static PyObject *xpy_numargs(PyObject *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":numargs"))
        return NULL;
    return PyLong_FromLong(numargs);
}

static PyObject * xpy_writelog(PyObject *self, PyObject *args)
{
    int level;
    const char *msg;

    if (!PyArg_ParseTuple(args, "is", &level, &msg))
        return NULL;
    xlog(level, msg, false);
    return Py_None;
}

int MarshalArguments(var *v, Py_ssize_t size, PyObject *args)
{
    PyObject *pItem, *pValue;
    PyObject *pArgs;

    for (Py_ssize_t i = 0; i < size; i++)
    {
        pItem = PyTuple_GetItem(args, i);  // dosen't increase refcount
        if (pItem == NULL)
        {
            return -1;
        }

        if (Py_None == pItem)
        {
            v[i].type = var_type::NONE;
        }
        else if (PyBool_Check(pItem) == 1)
        {
            v[i].type = var_type::BOOLEAN;
            if (Py_True == pItem)
                v[i].d = 1;
            else
                v[i].d = 0;
        }
        else if (PyNumber_Check(pItem) == 1)
        {
            if (PyLong_Check(pItem) == 1)
            {
                v[i].type = var_type::INTEGER;
                pValue = PyNumber_Long(pItem);

                int overflow;
                long result = PyLong_AsLongAndOverflow(pValue, &overflow);
                if (overflow)
                {
                    v[i].type = var_type::INT64;
                    v[i].d64 = PyLong_AsLongLong(pValue);
                }
                else
                {
                    v[i].type = var_type::INTEGER;
                    v[i].d = result;
                }
                Py_DECREF(pValue);
            }
            else if (PyFloat_Check(pItem) == 1)
            {
                v[i].type = var_type::REAL;
                pValue = PyNumber_Float(pItem);
                v[i].f = PyFloat_AsDouble(pValue);
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
            v[i].type = var_type::STRING;
            v[i].ptr = (void *)PyUnicode_AsUTF8(pItem);
        }
        else if (PyCapsule_CheckExact(pItem) == 1)
        {
            v[i].type = var_type::POINTER;
            v[i].ptr = PyCapsule_GetPointer(pItem, NULL);
        }
        else
        {
            // func_proxy: call sharp._proxy to get proxy
            pArgs = PyTuple_New(1);
            PyTuple_SetItem(pArgs, 0, pItem);
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
                v[0].type = var_type::STRING;
                v[0].ptr = (void *)("Invalid proxy function");
            }
            else
            {
                if (type[0] == 'S')
                {
                    v[i].type = var_type::SHARPOBJ;
                }
                else // type[0] == 'P'
                {
                    v[i].type = var_type::PYTHONOBJ;
                }

                pRet = PyTuple_GetItem(pValue, 1);
                pRetValue = PyNumber_Long(pRet);
                long n = PyLong_AsLong(pRetValue);
                v[i].d = n;
                Py_DECREF(pRetValue);
            }
            Py_DECREF(pValue);
        }

        if (PyErr_Occurred())
        {
            return -1;
        }
    }
    return 1;
}

static PyObject * xpy_csharpcall(PyObject *self, PyObject *args)
{
    Py_ssize_t tupleSize = PyTuple_Size(args);

    if (!tupleSize)
    {
        if (!PyErr_Occurred())
            PyErr_SetString(PyExc_TypeError, "You must supply at least one argument.");
        return NULL;
    }

    var arg[MAXRET];
    logger::info("xpy_csharpcall argc: {}", tupleSize);
    if (MarshalArguments(arg, tupleSize, args) != 1)
    {
        return NULL; // when use PyErr_SetString in a c extension function, you must return NULL.
    }

    string_pusher sp = {0};
    if (sharp_cb)
    {
        int rt = sharp_cb((int)tupleSize, arg, &sp);
    }

    return Py_None;
}

static PyMethodDef EmbMethods[] = {
    {"numargs", xpy_numargs, METH_VARARGS,
     "Return the number of arguments received by the process."},
    {"writelog",  xpy_writelog, METH_VARARGS,
     "output log."},
    {"csharpcall",  xpy_csharpcall, METH_VARARGS,
     "call csharp."},
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

int register_xpy_functions()
{
    PyImport_AppendInittab("xpy", &PyInit_emb);
    return 0;
}
