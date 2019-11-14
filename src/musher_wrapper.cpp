#include <Python.h>
#include <iostream>
#include <string>

#include "functional_test.h"


PyObject* PrintFunctionalMessage(PyObject* self, PyObject* args)
{
    /* Arguments passed in from Python */
    const char* message;

    /* Process arguments from Python */
    PyArg_ParseTuple(args, "s",
                    &message);

    /* Call function */
    CPrintFunctionalMessage(message);

    /* Return nothing */
    return Py_BuildValue("");
}


/* define the functions provided by the module */
static PyMethodDef cFuncs[] =
{
    /* Wrapper for PrintFunctionalMessage */
    {
        "PrintFunctionalMessage",
        PrintFunctionalMessage,
        METH_VARARGS,
        "Print a message from a function"
    },

    /* last one must be empty */
    {NULL, NULL, 0, NULL}
};


/* define the module */
static struct PyModuleDef cModule =
{
    PyModuleDef_HEAD_INIT,
    "Musher",
    NULL,
    -1,
    cFuncs
};


/* Initialize the Python module */
PyMODINIT_FUNC PyInit_Musher(void)
{
    return PyModule_Create(&cModule);
}