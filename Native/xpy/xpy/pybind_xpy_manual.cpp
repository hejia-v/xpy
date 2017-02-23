#include <Python.h>
#include "xpy.h"
#include "log.h"
#include <string>
using namespace xpy;

static int numargs = 5;
static csharp_callback sharp_cb = nullptr;

static PyObject *func_proxy = nullptr;
static PyObject *func_object = nullptr;
static PyObject *func_garbage = nullptr;

void init_csharp_python_funcs(csharp_callback cb)
{
    sharp_cb = cb;
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
    PyObject *p1, *p2;

    for (Py_ssize_t i = 0; i < size; i++)
    {
        p1 = PyTuple_GetItem(args, i);
        if (p1 == NULL) { return NULL; }

        /* Check if p1 is numeric */
        if (PyNumber_Check(p1) != 1)
        {
            PyErr_SetString(PyExc_TypeError, "Non-numeric argument.");
            return NULL;
        }

        /* Convert number to python long and than C unsigned long */
        p2 = PyNumber_Long(p1);
        //arr[i] = PyLong_AsUnsignedLong(p2);
        Py_DECREF(p2);

        if (PyErr_Occurred()) {return NULL; }
    }
    return 1;
}

static PyObject * xpy_csharpcall(PyObject *self, PyObject *args)
{
    Py_ssize_t tupleSize = PyTuple_Size(args);

    if (!tupleSize) {
        if (!PyErr_Occurred())
            PyErr_SetString(PyExc_TypeError, "You must supply at least one argument.");
        return NULL;
    }

    var *vs = new var[tupleSize];
    logger::info("sc: {}",tupleSize);
    MarshalArguments(vs, tupleSize, args);

    string_pusher sp = { 0 };
    if (sharp_cb)
    {
        int rt = sharp_cb((int)tupleSize, vs, &sp);
    }

    delete []vs;
    return Py_None;
    int level;
    const char *msg;

    if (!PyArg_ParseTuple(args, "is", &level, &msg))
        return NULL;
    xlog(level, msg, false);
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
