#include <Python.h>
#include "xpy.h"

static int numargs = 5;

/* Return the number of arguments of the application command line */
static PyObject *emb_numargs(PyObject *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":numargs"))
        return NULL;
    return PyLong_FromLong(numargs);
}

static PyObject * emb_writelog(PyObject *self, PyObject *args)
{
    const char *msg;

    if (!PyArg_ParseTuple(args, "s", &msg))
        return NULL;
    xlog(1, msg);
    return Py_None;
}

static PyMethodDef EmbMethods[] = {
    {"numargs", emb_numargs, METH_VARARGS,
     "Return the number of arguments received by the process."},
    {"writelog",  emb_writelog, METH_VARARGS,
     "output log."},
    {NULL, NULL, 0, NULL}
};

static PyModuleDef EmbModule = {
    PyModuleDef_HEAD_INIT, "emb", NULL, -1, EmbMethods,
    NULL, NULL, NULL, NULL
};

static PyObject *PyInit_emb(void)
{
    return PyModule_Create(&EmbModule);
}

int register_util_functions()
{
    PyImport_AppendInittab("emb", &PyInit_emb);
    return 0;
}
