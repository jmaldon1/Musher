#include <Python.h>
#include <iostream>
#include <string>
#include <unordered_map> 

#include "functional_test.h"

/* Unordered map of python decode functions mapped to their C++ function equivalent */
typedef bool (*DecodeFunction)(const char*); // function pointer type
std::unordered_map<std::string, DecodeFunction> uMapDecodeFuncs({
    {"PyDecodeWav", &DecodeWav}
}); 

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


PyObject* PyDecodeWav(PyObject* self, PyObject* args)
{
    /* Arguments passed in from Python */
    const char* message;

    /* Process arguments from Python */
    PyArg_ParseTuple(args, "s",
                    &message);

    /* Call function */
    DecodeWav(message);

    /* Return nothing */
    return Py_BuildValue("");
}


PyObject* PyAcceptDecode(PyObject* self, PyObject* args)
{
    /* Arguments passed in from Python */
    const char* message;
    PyObject* decodeFunc;

    /* Process arguments from Python */
    PyArg_ParseTuple(args, "sO",
                    &message, &decodeFunc);

    if(!PyCallable_Check(decodeFunc)){
        PyErr_SetString(PyExc_TypeError, "A callable is required");
        return 0;
    }

    /* Get name of python function that was passed in */
    PyObject* funcNameUnicode = PyObject_GetAttrString(decodeFunc, "__name__");
    PyObject* funcNamePythonBytes = PyUnicode_AsEncodedString(funcNameUnicode, "utf-8", "~E~");
    const char *funcName = PyBytes_AsString(funcNamePythonBytes);
    // std::cout << funcName << std::endl;

    Py_XDECREF(funcNameUnicode);
    Py_XDECREF(funcNamePythonBytes);

    /* Find decode function in umap */
    std::unordered_map<std::string, DecodeFunction>::const_iterator got = uMapDecodeFuncs.find(funcName);

    if ( got == uMapDecodeFuncs.end() ) {
        std::cout << "decode function not found";
        return 0;
    }
    else {
        /* Get decode function from umap */
        bool (*decodeFunc)(const char*) = got->second;
        AcceptDecode("Hello from Decode wav", decodeFunc);
    }
    /* Call function */
    // PyObject* cbArgs = PyTuple_New(1);
    // PyTuple_SetItem(cbArgs, 0, Py_BuildValue("s", message));
    // PyObject *rv = PyObject_CallObject(decodeFunc, cbArgs);

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
    {
        "PyDecodeWav",
        PyDecodeWav,
        METH_VARARGS,
        "Decode Wav file"
    },
    {
        "PyAcceptDecode",
        PyAcceptDecode,
        METH_VARARGS,
        "Accept function"
    },

    /* last one must be empty */
    {NULL, NULL, 0, NULL}
};


/* define the module */
static struct PyModuleDef cModule =
{
    PyModuleDef_HEAD_INIT,
    "musher",
    NULL,
    -1,
    cFuncs
};


/* Initialize the Python module */
PyMODINIT_FUNC PyInit_musher(void)
{
    return PyModule_Create(&cModule);
}