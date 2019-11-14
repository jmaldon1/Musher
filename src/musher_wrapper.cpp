#include <Python.h>
#include <iostream>
#include <string>

#include "functional_test.h"

#if 0
PyObject* construct(PyObject* self, PyObject* args)
{
    /* Arguments passed in from Python */
    const char* name;
    double length;

    /* Process arguments from Python */
    PyArg_ParseTuple(args, "sd",
                    &name,
                    &length);

    Song* song = new Song(name, length);

    /* Create Python capsule with pointer to object */
    PyObject* songCapsule = PyCapsule_New((void*)song, "SongPtr", NULL);
    PyCapsule_SetPointer(songCapsule, (void*)song);

    /* Return the Python capsule */
    return Py_BuildValue("O", songCapsule);
}


PyObject* delete_object(PyObject* self, PyObject* args)
{
    /* Arguments passed in from Python */
    PyObject* songCapsule;

    /* Process arguments from Python */
    PyArg_ParseTuple(args, "O",
                    &songCapsule);

    /* Get Song pointer */
    Song* song = (Song*)PyCapsule_GetPointer(songCapsule, "SongPtr");

    delete song;

    /* Return nothing */
    return Py_BuildValue("");
}


PyObject* SetName(PyObject* self, PyObject* args)
{
    /* Arguments passed in from Python */
    PyObject* songCapsule;
    const char* name;

    /* Process arguments from Python */
    PyArg_ParseTuple(args, "Os",
                    &songCapsule,
                    &name);

    /* Get Song pointer */
    Song* song = (Song*)PyCapsule_GetPointer(songCapsule, "SongPtr");

    /* Call function */
    song->SetName(name);

    /* Return nothing */
    return Py_BuildValue("");
}


PyObject* SetLength(PyObject* self, PyObject* args)
{
    /* Arguments passed in from Python */
    PyObject* songCapsule;
    double length;

    /* Process arguments from Python */
    PyArg_ParseTuple(args, "Od",
                    &songCapsule,
                    &length);

    /* Get Song pointer */
    Song* song = (Song*)PyCapsule_GetPointer(songCapsule, "SongPtr");

    /* Call function */
    song->SetLength(length);

    /* Return nothing */
    return Py_BuildValue("");
}


PyObject* GetName(PyObject* self, PyObject* args)
{
    /* Arguments passed in from Python */
    PyObject* songCapsule;

    /* Process arguments from Python */
    PyArg_ParseTuple(args, "O",
                    &songCapsule);

    /* Get Song pointer */
    Song* song = (Song*)PyCapsule_GetPointer(songCapsule, "SongPtr");

    /* Call function */
    std::string name = song->GetName();

    /* Return the name */
    return Py_BuildValue("s", name.c_str());
}


PyObject* GetLength(PyObject* self, PyObject* args)
{
    /* Arguments passed in from Python */
    PyObject* songCapsule;

    /* Process arguments from Python */
    PyArg_ParseTuple(args, "O",
                    &songCapsule);

    /* Get Song pointer */
    Song* song = (Song*)PyCapsule_GetPointer(songCapsule, "SongPtr");

    /* Call function */
    double length = song->GetLength();

    /* Return the length */
    return Py_BuildValue("d", length);
}


PyObject* PrintMessage(PyObject* self, PyObject* args)
{
      /* Arguments passed in from Python */
    PyObject* songCapsule;

    /* Process arguments from Python */
    PyArg_ParseTuple(args, "O",
                    &songCapsule);

    /* Get Song pointer */
    Song* song = (Song*)PyCapsule_GetPointer(songCapsule, "SongPtr");

    /* Call function */
    song->PrintMessage();

    /* Return nothing */
    return Py_BuildValue("");  
}
#endif


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
    #if 0
    /* c++/Python constructor */
    {
        "construct",
        construct, METH_VARARGS,
        "Create `Song` object"
    },

    /* c++/Python destructor */
    {
        "delete_object",
        delete_object, METH_VARARGS,
        "Delete `Song` object"
    },

    /* Wrapper for SetName */
    {
        "SetName",
        SetName, METH_VARARGS,
        "Set name for `Song` object"
    },

    /* Wrapper for SetLength */
    {
        "SetLength",
        SetLength, METH_VARARGS,
        "Set length for `Song` object"
    },

    /* Wrapper for GetName */
    {
        "GetName",
        GetName, METH_VARARGS,
        "Get name from `Song` object"
    },

    /* Wrapper for GetLength */
     {
        "GetLength",
        GetLength, METH_VARARGS,
        "Get length from `Song` object"
    },

    /* Wrapper for PrintMessage */
    {
        "PrintMessage",
        PrintMessage, METH_VARARGS,
        "Print Message from `Song` object"
    },
    #endif

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